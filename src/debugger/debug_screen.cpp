//  debug_screen.cpp
//  E64
//
//  Copyright © 2017 elmerucr. All rights reserved.

#include "debug_screen.hpp"
#include "common_defs.hpp"

uint8_t debug_screen_character_buffer[32*64];            // 32 lines of 64 chars
uint16_t debug_screen_foreground_color_buffer[32*64];     // 32x64
uint16_t debug_screen_background_color_buffer[32*64];     // 32x64

uint16_t debug_screen_pixel_cursor_blink_time;
uint16_t debug_screen_pixel_cursor_blink_time_countdown;

void E64::debug_screen_init()
{
    debug_screen_pixel_cursor_blink_time = 40;
    debug_screen_pixel_cursor_blink_time_countdown = debug_screen_pixel_cursor_blink_time;
}

void E64::debug_screen_pixel_cursor_flash()
{
    debug_screen_pixel_cursor_blink_time_countdown--;
    if(debug_screen_pixel_cursor_blink_time_countdown == 1) E64::debug_screen_pixel_cursor_reset();
}

void E64::debug_screen_pixel_cursor_reset()
{
    debug_screen_pixel_cursor_blink_time_countdown = debug_screen_pixel_cursor_blink_time;
}

void E64::debug_screen_update()
{
    // update all 32 rows (0-31), 32x8 = 256 scanlines
    for(int i=0; i < 256; i++) debug_screen_render_scanline(i);

    // copy relevant area of vicv screen buffer to bottom of debug screen buffer
    uint8_t scanline_normalized;
    if(computer.vicv_ic->get_current_scanline() > 287)
    {
        scanline_normalized = 255;
    }
    else if(computer.vicv_ic->get_current_scanline() >= 32)
    {
        scanline_normalized = computer.vicv_ic->get_current_scanline() - 32;
    }
    else
    {
        scanline_normalized = 0;
    }
    
    /* Copy relevant part of the framebuffer onto the debug screen
     */
    uint32_t base = VICV_PIXELS_PER_SCANLINE * scanline_normalized;
    
    for(int i=0; i<VICV_PIXELS_PER_SCANLINE*64; i++)
    {
        // Backbuffer is the one currently being drawn into, so that one
        // should be shown.
        host_video.debug_screen_buffer[(256*VICV_PIXELS_PER_SCANLINE) + i] = host_video.backbuffer[base + i];
    }
    
    uint16_t current_pixel = computer.vicv_ic->get_current_pixel();
    uint16_t current_scanline = computer.vicv_ic->get_current_scanline();
    uint32_t pixel_cursor_color = 0xff00ff00;
    if(current_pixel > 511)
    {
        current_pixel = 511;
        pixel_cursor_color = 0xffff0000;
    }
    if(current_scanline > 319)
    {
        current_scanline = 319;
        pixel_cursor_color = 0xffff0000;
    }
    
    if(debug_screen_pixel_cursor_blink_time_countdown > (debug_screen_pixel_cursor_blink_time / 2))
    {
        host_video.debug_screen_buffer[(256*VICV_PIXELS_PER_SCANLINE) + ((current_scanline - scanline_normalized)*VICV_PIXELS_PER_SCANLINE) + current_pixel ] = pixel_cursor_color;
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
    uint16_t current_foreground_color = 0;
    uint16_t current_background_color = 0;

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
        host_video.debug_screen_buffer[base|x] = (eight_pixels & 0x80) ? host_video.palette[current_foreground_color] : host_video.palette[current_background_color];
        
        // shift all bits in internal byte 1 place to the left
        eight_pixels = eight_pixels << 1;
    }
}
