//  vicv.cpp
//  E64
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.

#include <cstdio>

#include "vicv.hpp"
#include "common.hpp"


E64::vicv::vicv()
{
    stats_overlay_present = false;

    framebuffer0 = (uint16_t *)&pc.mmu_ic->ram[0x00f00000];
    framebuffer1 = (uint16_t *)&pc.mmu_ic->ram[0x00f48000];
    
    breakpoint_reached = false;
    clear_scanline_breakpoints();
    old_y_pos = 0;
    
    stats_text = nullptr;
}


void E64::vicv::reset()
{
    pc.TTL74LS148_ic->release_line(vblank_interrupt_device_number);

    frame_done = false;

    cycle_clock = dot_clock = 0;

    for(int i=0; i<256; i++) registers[i] = 0;
    
    frontbuffer = framebuffer0;
    backbuffer  = framebuffer1;
}


void E64::vicv::run(uint32_t number_of_cycles)
{
    uint32_t y_pos;
    
    while(number_of_cycles > 0)
    {
        y_pos = (cycle_clock / (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK));
        uint32_t x_pos = (cycle_clock - (y_pos * (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)));
        bool hblank = (x_pos >= VICV_PIXELS_PER_SCANLINE);
        bool vblank = (cycle_clock>=((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES));
        bool blank = hblank || vblank;
        bool hborder = (y_pos < registers[VICV_REG_BORDER_SIZE]) || (y_pos > ((VICV_SCANLINES-1)-registers[VICV_REG_BORDER_SIZE]));
        
        if(!blank)
        {
            if(hborder)
            {
                host_video.backbuffer[dot_clock] = host_video.palette[*((uint16_t *)(&registers[VICV_REG_BOR]))];
            }
            else
            {
                host_video.backbuffer[dot_clock] = host_video.palette[frontbuffer[dot_clock]];
            }
            // only progress the dot clock if a pixel was actually sent to screen (!BLANK)
            dot_clock++;
        }

        cycle_clock++;
        
        switch(cycle_clock)
        {
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES:
                // start of vblank
                pc.TTL74LS148_ic->pull_line(vblank_interrupt_device_number);
                break;
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK):
                // finished vblank, do other necessary stuff
                if(stats_overlay_present) render_stats(44, 276);
                host_video.swap_buffers();
                cycle_clock = dot_clock = 0;
                frame_done = true;
                break;
        }
        
        number_of_cycles--;
    }
    
    if( (y_pos != old_y_pos) && scanline_breakpoints[y_pos] == true) breakpoint_reached = true;
    old_y_pos = y_pos;
}


#define Y_POS           (cycle_clock / (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK))
#define X_POS           (cycle_clock - (Y_POS * (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)))
#define HBLANK          (X_POS >= VICV_PIXELS_PER_SCANLINE)
#define VBLANK          (cycle_clock>=((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES))


bool E64::vicv::is_hblank() { return HBLANK; }
bool E64::vicv::is_vblank() { return VBLANK; }


inline void E64::vicv::render_stats(uint16_t xpos, uint16_t ypos)
{
    uint32_t base = ((ypos * VICV_PIXELS_PER_SCANLINE) + xpos) % (VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
    uint8_t  eight_pixels = 0;

    for(int y=0; y<8; y++)
    {
        char *temp_text = stats_text;
        uint16_t x = 0;
        // are we still pointing at a character
        while(*temp_text)
        {
            // are we at the first pixel of a char
            if( !(x & 7) )
            {
                eight_pixels = cbm_cp437_font[(*temp_text * 8) + y];
                //eight_pixels = patched_char_rom[((ascii_to_screencode[*temp_text]) * 8) + y];
            }

            host_video.backbuffer[base + x] = (eight_pixels & 0x80) ? host_video.palette[COBALT_06] : host_video.palette[COBALT_01];

            eight_pixels = eight_pixels << 1;
            x++;
            // increase the text pointer only when necessary
            if( !(x & 7) ) temp_text++;
        }
        // go to the next line
        base = (base + VICV_PIXELS_PER_SCANLINE) % (VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
    }
}


uint16_t E64::vicv::get_current_scanline() { return Y_POS; }
uint16_t E64::vicv::get_current_pixel() { return X_POS; }


void E64::vicv::clear_scanline_breakpoints()
{
    for(int i=0; i<1024; i++) scanline_breakpoints[i] = false;
}


void E64::vicv::add_scanline_breakpoint(uint16_t scanline)
{
    scanline_breakpoints[scanline & 1023] = true;
}


void E64::vicv::remove_scanline_breakpoint(uint16_t scanline)
{
    scanline_breakpoints[scanline & 1023] = false;
}


bool E64::vicv::is_scanline_breakpoint(uint16_t scanline)
{
    return scanline_breakpoints[scanline & 1023];
}


uint8_t E64::vicv::read_byte(uint8_t address)
{
    return registers[address];
}


void E64::vicv::write_byte(uint8_t address, uint8_t byte)
{
    switch( address )
    {
        case VICV_REG_ISR:
            if( byte & 0b00000001 ) pc.TTL74LS148_ic->release_line(vblank_interrupt_device_number);  // acknowledge pending irq
            break;
        case VICV_REG_BUFFERSWAP:
            if( byte & 0b00000001 )
            {
                if( pc.blitter_ic->current_state != IDLE )
                {
                    pc.blitter_ic->current_state = IDLE;
                    printf("[VICV] warning: blitter was not finished when swapping buffers\n");
                }
                uint16_t *tempbuffer = frontbuffer;
                frontbuffer = backbuffer;
                backbuffer = tempbuffer;
            }
            break;
        default:
            registers[address] = byte;
            break;
    }
}
