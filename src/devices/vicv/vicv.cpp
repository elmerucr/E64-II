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

//#define IS_HBLANK       (X_POS > (VICV_PIXELS_PER_SCANLINE-1))
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
            host_video.backbuffer[dot_clock] = framebuffer0[dot_clock] ? framebuffer0[dot_clock] : host_video.palette[*((uint16_t *)(&registers[VICV_REG_BKG]))];

            if(HBORDER) host_video.backbuffer[dot_clock] = host_video.palette[*((uint16_t *)(&registers[VICV_REG_BOR]))];

            dot_clock++;
        }

        cycle_clock++;

        switch(cycle_clock)
        {
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES):
                vblank_irq = false;
                break;
            case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_PIXELS_VBLANK):
                if(overlay_present) render_overlay(117, 300, frame_delay.stats());
                host_video.swap_buffers();
                cycle_clock = dot_clock = 0;
                frame_done = true;
                break;
        }
        number_of_cycles--;
    }
}



bool E64::vicv::is_hblank() { return HBLANK; }
bool E64::vicv::is_vblank() { return VBLANK; }



///* Note: each cycle on vicv results in one pixel (or dot) to be
// * produced.
// */
//void E64::vicv::run_old(uint32_t number_of_cycles) {
//    current_xpos = current_xpos + number_of_cycles;
//
//    /* It looks like an <if> statement could have been used below.
//     * The <while> is necessary however to make sure ALL scanlines
//     * are processed. In theory, the number of pixels to run, could be
//     * more than an extra scanline, so several have to be done.
//     */
//    while( current_xpos >= VICV_PIXELS_PER_SCANLINE )
//    {
//        if( (current_scanline < 32) || (current_scanline > 287) )
//        {
//            render_border_scanline();
//        }
//        else
//        {
//            render_scanline();
//        }
//        current_scanline++;
//        if(current_scanline == VICV_SCANLINES)
//        {
//            if(overlay_present) render_overlay(117, 300, frame_delay.stats());
//            swap_buffers();
//            current_scanline = 0;
//            frame_done = true;
//        }
//        current_xpos -= VICV_PIXELS_PER_SCANLINE;
//    }
//}

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

//inline void E64::vicv::render_border_scanline()
//{
//    int base = current_scanline * VICV_PIXELS_PER_SCANLINE;
//    uint32_t background_color = color_palette[registers[VICV_REG_BOR]];
//    for(int x = 0; x < VICV_PIXELS_PER_SCANLINE; x++)
//    {
//        host_backbuffer[base | x] = background_color;
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

            host_video.backbuffer[base + x] = (eight_pixels & 0x80) ? host_video.palette[NEW_COBALT_06] : host_video.palette[NEW_COBALT_01];

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

uint8_t E64::vicv::read_byte(uint8_t address)
{
    return registers[address];
    
}

void E64::vicv::write_byte(uint8_t address, uint8_t byte)
{
    switch( address )
    {
        case VICV_REG_ISR:
            if( byte & 0b00000001 ) vblank_irq = true;
            break;
        default:
            registers[address] = byte;
            break;
    }
}
