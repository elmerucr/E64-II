//  blitter.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "blitter.hpp"
#include "common.hpp"

/*
 *  blend_color function takes the current color (destination, which is
 *  also the destination) and the color that must be blended (source). It
 *  returns the value of the blend which, normally, will be written to the
 *  destination.
 *  The ordering (from little endian perspective) seems strange: GBAR4444
 *  Actually, it isn't: inside the virtual machine (big endian) it is
 *  in ARGB444 format. At first, this function seemed to drag down total
 *  emulation speed. But, with optimizations (minimum -O2) turned on, it
 *  is ok.
 *
 *  The idea to use an inline function (and not a lookup table comes from
 *  this website:
 *  https://stackoverflow.com/questions/30849261/alpha-blending-using-table-lookup-is-not-as-fast-as-expected
 *
 *  In three steps a derivation:
 *  (1) ((source * a) + (destination * (COLOR_MAX - a))) / COLOR_MAX
 *  (2) ((source * a) - (destination * a) + (destination * COLOR_MAX)) / COLOR_MAX
 *  (3) destination + (((source - destination) * a) / COLOR_MAX)
 *
 */

inline uint16_t alpha_blend(uint16_t destination, uint16_t source)
{
    uint8_t r_dest, g_dest, b_dest;
    uint8_t a_src, r_src, g_src, b_src;
    
    r_dest = (destination & 0x000f);
    g_dest = (destination & 0xf000) >> 12;
    b_dest = (destination & 0x0f00) >> 8;

    a_src = (source & 0x00f0) >> 4;
    r_src = (source & 0x000f);
    g_src = (source & 0xf000) >> 12;
    b_src = (source & 0x0f00) >> 8;
    
    r_dest = r_dest + (((r_src - r_dest) * a_src) / 15);
    g_dest = g_dest + (((g_src - g_dest) * a_src) / 15);
    b_dest = b_dest + (((b_src - b_dest) * a_src) / 15);
    
    return (g_dest << 12) | (b_dest << 8) | 0x00f0 | r_dest;
}

void E64::blitter::reset()
{
    current_state = IDLE;
    
    head = 0;
    tail = 0;
}

void E64::blitter::run(int no_of_cycles)
{
    while(no_of_cycles > 0)
    {
        no_of_cycles--;
        
        switch( current_state )
        {
            case IDLE:
                // check for a new operation in FIFO list
                if( head != tail)
                {
                    switch( operations[tail].type )
                    {
                        case CLEAR_FRAMEBUFFER:
                            current_state = CLEARING_FRAMEBUFFER;
                            width = VICV_PIXELS_PER_SCANLINE;
                            height = VICV_SCANLINES;
                            max_count = (width * height);
                            counter = 0;
                            clear_color = ((operations[tail].data_element) & 0x0000ff0f) | 0x00f0;
                            tail++;
                            break;
                        case BLIT:
                            // switch state
                            current_state = BLITTING;
                            
                            // set up the blitting finite state machine
                            
                            // check flags
                            is_double_width  = (operations[tail].this_blit.flags_1 & 0b00000001) ? 1 : 0;
                            is_double_height = (operations[tail].this_blit.flags_1 & 0b00000010) ? 1 : 0;
                            
                            char_width  = operations[tail].this_blit.width  & 0b00000111;
                            char_height = operations[tail].this_blit.height & 0b00000111;
                            
                            width_power_of_2  = 3 + char_width + is_double_width;
                            height_power_of_2 = 3 + char_height + is_double_height;
                            
                            width  = 1 << width_power_of_2;
                            height = 1 << height_power_of_2;
                            width_mask = width - 1;
                            counter = 0;
                            max_count = width * height;
                            
                            x = operations[tail].this_blit.x_low_byte | operations[tail].this_blit.x_high_byte << 8;
                            y = operations[tail].this_blit.y_low_byte | operations[tail].this_blit.y_high_byte << 8;
                            
                            pixel_data = (uint16_t *)
                            &computer.mmu_ic->ram  [
                                                        operations[tail].this_blit.pixel_data__0__7       |
                                                        operations[tail].this_blit.pixel_data__8_15 <<  8 |
                                                        operations[tail].this_blit.pixel_data_16_23 << 16 |
                                                        operations[tail].this_blit.pixel_data_24_31 << 24
                                                    ];
                            
                            tail++;
                            
                            break;
                    }
                }
                else
                {
                    // do something to take cpu time
                }
                break;
            case CLEARING_FRAMEBUFFER:
                if( counter < max_count )
                {
                    computer.vicv_ic->backbuffer[counter] = clear_color;
                    counter++;
                }
                else
                {
                    current_state = IDLE;
                }
                break;
            case BLITTING:
                if( counter < max_count )
                {
                    scr_x = x + (counter & width_mask);
                    
                    if( !(scr_x & 0b1111111000000000) )
                    {
                        scr_y = y + ( counter >> width_power_of_2 );
                        
                        if( (scr_y >= 0) && (scr_y < VICV_SCANLINES) )
                        {
                            computer.vicv_ic->backbuffer[ scr_x | (scr_y << 9) ] = alpha_blend
                            (
                                computer.vicv_ic->backbuffer[ scr_x | (scr_y << 9) ],
                                pixel_data
                                [
                                    // Rather complex piece of bitwise logic to pick the right pixels
                                    // from the source material, depending on double width and height
                                    // settings.
                                    // First, it shifts all ...
                                    (((counter >> is_double_height) & ~width_mask) | (counter & width_mask)) >> is_double_width
                                ]
                            );
                            
                        }
                    }
                    counter++;
                }
                else
                {
                    current_state = IDLE;
                }
                break;
        }        
    }
}

void E64::blitter::add_operation(enum operation_type type, uint32_t data_element)
{
    switch( type )
    {
        case CLEAR_FRAMEBUFFER:
            operations[head].type = CLEAR_FRAMEBUFFER;
            operations[head].data_element = data_element;
            head++;
            break;
        case BLIT:
            data_element &= 0x00ffffe0;
            
            struct surface_blit *temp_blit = (struct surface_blit *)&computer.mmu_ic->ram[data_element];
            
            operations[head].type = BLIT;
            operations[head].data_element = data_element;
            operations[head].this_blit = *temp_blit;
            
            head++;
            
            break;
    }
}

uint8_t E64::blitter::read_byte(uint8_t address)
{
    switch( address )
    {
        case 0x00:
            if( current_state == IDLE )
            {
                return 0b00000000;
            }
            else
            {
                return 0b00000001;
            }
            break;
        default:
            return registers[address];
            break;
    }
}

void E64::blitter::write_byte(uint8_t address, uint8_t byte)
{
    switch( address )
    {
        case 0x00:
            if( byte & 0b00000001 ) add_operation(CLEAR_FRAMEBUFFER, (registers[0x05] << 8) | registers[0x04]);
            if( byte & 0b00000010 ) add_operation(BLIT, (registers[0x02] << 24) | (registers[0x03] << 16) | (registers[0x04] << 8) | registers[0x05]);
            break;
        default:
            registers[address] = byte;
            break;
    }
}
