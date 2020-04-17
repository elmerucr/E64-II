//  blitter.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "blitter.hpp"
#include "common_defs.hpp"

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
    g_dest = g_dest + (((g_src - g_dest) * g_src) / 15);
    b_dest = b_dest + (((b_src - b_dest) * b_src) / 15);
    
    return (g_dest << 12) | (b_dest << 8) | 0x00f0 | r_dest;
}

void E64::blitter::reset()
{
    current_state = IDLE;
    
    head_fifo = 0;
    tail_fifo = 0;
}

void E64::blitter::run(int no_of_cycles)
{
    uint16_t destination_color = computer.mmu_ic->ram[0x00d001];
    uint16_t source_color = computer.mmu_ic->ram[0x00eff2];
    
    while(no_of_cycles > 0)
    {
        no_of_cycles--;
        
        switch( current_state )
        {
            case IDLE:
                // check for a new operation in FIFO list
                destination_color = alpha_blend(destination_color, source_color);
                break;
            case CLEARING_FRAMEBUFFER:
                break;
            case BLITTING:
                break;
        }        
    }
}

void E64::blitter::add_operation(enum operation_type type, uint32_t data_element)
{
    switch( type )
    {
        case CLEAR_FRAMEBUFFER:
            printf("dummy for clearing screen by blitter with color $%04x\n", data_element);
            break;
        case BLIT:
            printf("dummy for blit by blitter described at address $%08x\n", data_element);
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
            if( byte & 0b00000001 ) add_operation(CLEAR_FRAMEBUFFER, (registers[0x04] << 8) | registers[0x05]);
            if( byte & 0b00000010 ) add_operation(BLIT, (registers[0x02] << 24) | (registers[0x03] << 16) | (registers[0x04] << 8) | registers[0x05]);
            break;
        default:
            registers[address] = byte;
            break;
    }
}
