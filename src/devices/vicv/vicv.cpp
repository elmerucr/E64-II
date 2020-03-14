//  vicv.cpp
//  E64
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.

#include <cstdio>

#include "vicv.hpp"
#include "common_defs.hpp"

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
    vblank_irq = true;

    frame_done = false;

    cycle_clock = dot_clock = 0;

    for(int i=0; i<256; i++) registers[i] = 0;
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
                host_video.backbuffer[dot_clock] = framebuffer0[dot_clock];
                
                //host_video.backbuffer[dot_clock] = framebuffer0[dot_clock] ? framebuffer0[dot_clock] : host_video.palette[*((uint16_t *)(&registers[VICV_REG_BKG]))];
            }
            // only progress the dot clock if a pixel was actually sent
            // to screen (!BLANK)
            dot_clock++;
        }

        cycle_clock++;

        switch(cycle_clock)
        {
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES):
                vblank_irq = false;
                break;
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK):
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

//inline void E64::vicv::render_scanline()
//{
//    // 1st index in screen buffer to be used for display
//    int base = current_scanline * VICV_PIXELS_PER_SCANLINE;
//    // get the current textrow, divide by 8
//    int currentTextRow = ((current_scanline-32) >> 3);
//    // get current line within character
//    int currentCharacterLine = (current_scanline-32) & 0x07;
//
//    uint32_t background_color = color_palette[registers[VICV_REG_BKG]];
//
//    uint32_t start_screen_buffer = (registers[VICV_REG_TXT] << 24) + (registers[VICV_REG_TXT+1] << 16) + (registers[VICV_REG_TXT+2] << 8) + registers[VICV_REG_TXT+3];
//    uint32_t start_color_buffer = (registers[VICV_REG_COL] << 24) + (registers[VICV_REG_COL+1] << 16) + (registers[VICV_REG_COL+2] << 8) + registers[VICV_REG_COL+3];
//
//    // Reserve a byte for internal rendering use, defaults to 0
//    uint8_t eight_pixels = 0;
//    int current_column;
//    uint8_t current_char;
//    uint32_t current_char_color = 0;
//
//    for(int x=0; x<VICV_PIXELS_PER_SCANLINE; x++)
//    {
//        if( !(x & 0b00000111) ) // it's the first pixel of a char
//        {
//            // get current text column, current x divided by 8 (yields 0-63)
//            current_column = (x >> 3);
//            current_char = computer.mmu_ic->ram[(start_screen_buffer | (((currentTextRow << 6) | current_column))) & 0x00ffffff];
//            current_char_color = computer.vicv_ic->color_palette[computer.mmu_ic->ram[(start_color_buffer | (((currentTextRow << 6) | current_column))) & 0x00ffffff] ];
//            // get byte information from char_rom
//            eight_pixels = patched_char_rom[(current_char<<3) | currentCharacterLine];
//        }
//        host_backbuffer[base | x] = (eight_pixels & 0x80) ? current_char_color : background_color;
//        // shift all bits in internal byte 1 place to the left
//        eight_pixels = eight_pixels << 1;
//    }
//}

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
