//  debug_screen.cpp
//  E64
//
//  Copyright © 2017 elmerucr. All rights reserved.

#include "debug_screen.hpp"
#include "common_defs.hpp"

uint32_t debug_screen_buffer[VICV_PIXELS_PER_SCANLINE * 320];

uint8_t debug_screen_character_buffer[32*64];            // 32 lines of 64 chars
uint8_t debug_screen_foreground_color_buffer[32*64];     // 32x64
uint8_t debug_screen_background_color_buffer[32*64];     // 32x64

void E64::debug_screen_update()
{
    // update all 32 rows (0-31), 32x8 = 256 scanlines
    for(int i=0; i < 256; i++) debug_screen_render_scanline(i);

    // copy relevant area of vicv screen buffer to bottom of debug screen buffer
    uint8_t scanline_normalized;
    if(computer.vicv_ic->return_current_scanline() > 287)
    {
        scanline_normalized = 255;
    }
    else if(computer.vicv_ic->return_current_scanline() >= 32)
    {
        scanline_normalized = computer.vicv_ic->return_current_scanline() - 32;
    }
    else
    {
        scanline_normalized = 0;
    }
    for(int i=0; i<VICV_PIXELS_PER_SCANLINE*64; i++)
    {
        debug_screen_buffer[(256*VICV_PIXELS_PER_SCANLINE)+i] = computer.vicv_ic->back_buffer[i+(VICV_PIXELS_PER_SCANLINE*scanline_normalized)];
    }
}

inline void E64::debug_screen_render_scanline(int line_number)
{
    int base;

    // base in frameBuffer is lineNo * VICV_PIXELS_PER_SCANLINE
    base = line_number * VICV_PIXELS_PER_SCANLINE;
    // get the current textrow, divide lineNo by 8
    int current_text_row = (line_number >> 3);
    // get current line within character
    int current_character_line = line_number & 0x07;
    // reserve a byte for internal rendering use, defaults to 0
    uint8_t eight_pixels = 0;
    uint8_t current_char = 0;
    uint8_t current_foreground_color = 0;
    uint8_t current_background_color = 0;

    for(int x=0; x < VICV_PIXELS_PER_SCANLINE; x++)
    {
        // if it's the first pixel of a char position, retrieve byte from char_rom
        if(!(x & 7))
        {
            int current_text_column = (x >> 3);
            uint16_t char_position = (((current_text_row << 6) | current_text_column));
            current_char = debug_screen_character_buffer[char_position];
            current_foreground_color = debug_screen_foreground_color_buffer[char_position];
            current_background_color = debug_screen_background_color_buffer[char_position];
            eight_pixels = patched_char_rom[(current_char<<3) | current_character_line];
        }
        debug_screen_buffer[base|x] = (eight_pixels & 0x80) ? computer.vicv_ic->color_palette[current_foreground_color] : computer.vicv_ic->color_palette[current_background_color];
        // shift all bits in internal byte 1 place to the left
        eight_pixels = eight_pixels << 1;
    }
}
