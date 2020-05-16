//  vicv.cpp
//  E64
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.

#include <cstdio>

#include "vicv.hpp"
#include "common.hpp"

E64::vicv::vicv()
{
    overlay_present = false;

    framebuffer0 = (uint16_t *)&computer.mmu_ic->ram[0x00e00000];
    framebuffer1 = (uint16_t *)&computer.mmu_ic->ram[0x00e80000];
    
    breakpoint_reached = false;
    clear_scanline_breakpoints();
    old_y_pos = 0;
}

E64::vicv::~vicv()
{
    //
}

void E64::vicv::reset()
{
    computer.TTL74LS148_ic->release_line(interrupt_device_no_vblank);

    frame_done = false;

    cycle_clock = dot_clock = 0;

    for(int i=0; i<256; i++) registers[i] = 0;
    
    frontbuffer = framebuffer0;
    backbuffer  = framebuffer1;
}

#define Y_POS           (cycle_clock / (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK))
#define X_POS           (cycle_clock - (Y_POS * (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)))
//#define X_POS           (cycle_clock % (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK))

//#define HBLANK          (X_POS > (VICV_PIXELS_PER_SCANLINE-1))
#define HBLANK          (X_POS & 0xfffffe00)
#define VBLANK          (cycle_clock>=((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES))
#define BLANK           (HBLANK || VBLANK)
#define HBORDER         (Y_POS < registers[VICV_REG_BORDER_SIZE]) || (Y_POS > (319-registers[VICV_REG_BORDER_SIZE]))

void E64::vicv::run(uint32_t number_of_cycles)
{
    while(number_of_cycles > 0)
    {
        if(!BLANK)
        {
            if(HBORDER)
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

//        if(cycle_clock == ((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES) )
//        {
//            // start of vblank
//            computer.TTL74LS148_ic->pull_line(interrupt_device_no_vblank);
//        }
//        if(cycle_clock == ((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK)) )
//        {
//            // finished vblank, do other necessary stuff
//            if(overlay_present) render_overlay(117, 300, frame_delay.stats());
//            host_video.swap_buffers();
//            cycle_clock = dot_clock = 0;
//            frame_done = true;
//        }
        
        switch(cycle_clock)
        {
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES:
                // start of vblank
                computer.TTL74LS148_ic->pull_line(interrupt_device_no_vblank);
                break;
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK):
                // finished vblank, do other necessary stuff
                if(overlay_present) render_overlay(117, 300, frame_delay.stats());
                host_video.swap_buffers();
                cycle_clock = dot_clock = 0;
                frame_done = true;
                break;
        }
        
        number_of_cycles--;
    }
    
    if( (Y_POS != old_y_pos) && scanline_breakpoints[Y_POS] == true) breakpoint_reached = true;
    old_y_pos = Y_POS;
}

bool E64::vicv::is_hblank() { return HBLANK; }
bool E64::vicv::is_vblank() { return VBLANK; }

inline void E64::vicv::render_overlay(uint16_t xpos, uint16_t ypos, char *text)
{
    uint32_t base = ((ypos * VICV_PIXELS_PER_SCANLINE) + xpos) % (VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
    uint8_t  eight_pixels = 0;

    for(int y=0; y<8; y++)
    {
        char *temp_text = text;
        uint16_t x = 0;
        // are we still pointing at a character
        while(*temp_text)
        {
            // are we at the first pixel of a char
            if( !(x & 7) )
            {
                eight_pixels = patched_char_rom[((ascii_to_screencode[*temp_text]) * 8) + y];
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
            if( byte & 0b00000001 ) computer.TTL74LS148_ic->release_line(interrupt_device_no_vblank);  // acknowledge pending irq
            break;
        case VICV_REG_BUFFERSWAP:
            if( byte & 0b00000001 )
            {
                if( computer.blitter_ic->current_state != IDLE )
                {
                    computer.blitter_ic->current_state = IDLE;
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
