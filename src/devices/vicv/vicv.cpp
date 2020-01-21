//  vicv.cpp
//  E64
//
//  Copyright © 2017 elmerucr. All rights reserved.

#include <cstdio>

#include "vicv.hpp"
#include "common_defs.hpp"

E64::vicv::vicv()
{
    screen_buffer_0 = new uint32_t[VICV_PIXELS_PER_SCANLINE*320];
    screen_buffer_1 = new uint32_t[VICV_PIXELS_PER_SCANLINE*320];
    color_palette   = new uint32_t[256];
    overlay_present = true;
    reset();
}

E64::vicv::~vicv()
{
    delete [] screen_buffer_0;
    delete [] screen_buffer_1;
    delete [] color_palette;
    screen_buffer_0 = nullptr;
    screen_buffer_1 = nullptr;
    color_palette = nullptr;
}

void E64::vicv::reset()
{
    // standard C64 colors
    color_palette[0x00] = C64_BLACK;
    color_palette[0x01] = C64_WHITE;
    color_palette[0x02] = C64_RED;
    color_palette[0x03] = C64_CYAN;
    color_palette[0x04] = C64_PURPLE;
    color_palette[0x05] = C64_GREEN;
    color_palette[0x06] = C64_BLUE;
    color_palette[0x07] = C64_YELLOW;
    color_palette[0x08] = C64_ORANGE;
    color_palette[0x09] = C64_BROWN;
    color_palette[0x0a] = C64_LIGHTRED;
    color_palette[0x0b] = C64_DARKGREY;
    color_palette[0x0c] = C64_GREY;
    color_palette[0x0d] = C64_LIGHTGREEN;
    color_palette[0x0e] = C64_LIGHTBLUE;
    color_palette[0x0f] = C64_LIGHTGREY;

    // 16 grey tones from 0x10-0x1f
    color_palette[0x10] = C64_GREY_00;
    color_palette[0x11] = C64_GREY_01;
    color_palette[0x12] = C64_GREY_02;
    color_palette[0x13] = C64_GREY_03;
    color_palette[0x14] = C64_GREY_04;
    color_palette[0x15] = C64_GREY_05;
    color_palette[0x16] = C64_GREY_06;
    color_palette[0x17] = C64_GREY_07;
    color_palette[0x18] = C64_GREY_08;
    color_palette[0x19] = C64_GREY_09;
    color_palette[0x1a] = C64_GREY_10;
    color_palette[0x1b] = C64_GREY_11;
    color_palette[0x1c] = C64_GREY_12;
    color_palette[0x1d] = C64_GREY_13;
    color_palette[0x1e] = C64_GREY_14;
    color_palette[0x1f] = C64_GREY_15;

    // 16 green tones from 0x20-0x2f
    color_palette[0x20] = C64_GRN_00;
    color_palette[0x21] = C64_GRN_01;
    color_palette[0x22] = C64_GRN_02;
    color_palette[0x23] = C64_GRN_03;
    color_palette[0x24] = C64_GRN_04;
    color_palette[0x25] = C64_GRN_05;
    color_palette[0x26] = C64_GRN_06;
    color_palette[0x27] = C64_GRN_07;
    color_palette[0x28] = C64_GRN_08;
    color_palette[0x29] = C64_GRN_09;
    color_palette[0x2a] = C64_GRN_10;
    color_palette[0x2b] = C64_GRN_11;
    color_palette[0x2c] = C64_GRN_12;
    color_palette[0x2d] = C64_GRN_13;
    color_palette[0x2e] = C64_GRN_14;
    color_palette[0x2f] = C64_GRN_15;

    // 16 blue tones from 0x30-0x3f
    color_palette[0x30] = C64_COBALT_00;
    color_palette[0x31] = C64_COBALT_01;
    color_palette[0x32] = C64_COBALT_02;
    color_palette[0x33] = C64_COBALT_03;
    color_palette[0x34] = C64_COBALT_04;
    color_palette[0x35] = C64_COBALT_05;
    color_palette[0x36] = C64_COBALT_06;
    color_palette[0x37] = C64_COBALT_07;
    color_palette[0x38] = C64_COBALT_08;
    color_palette[0x39] = C64_COBALT_09;
    color_palette[0x3a] = C64_COBALT_10;
    color_palette[0x3b] = C64_COBALT_11;
    color_palette[0x3c] = C64_COBALT_12;
    color_palette[0x3d] = C64_COBALT_13;
    color_palette[0x3e] = C64_COBALT_14;
    color_palette[0x3f] = C64_COBALT_15;
    
    // 16 brown tones from 0x40-0x4f
    color_palette[0x40] = C64_AMBER_00;
    color_palette[0x41] = C64_AMBER_01;
    color_palette[0x42] = C64_AMBER_02;
    color_palette[0x43] = C64_AMBER_03;
    color_palette[0x44] = C64_AMBER_04;
    color_palette[0x45] = C64_AMBER_05;
    color_palette[0x46] = C64_AMBER_06;
    color_palette[0x47] = C64_AMBER_07;
    color_palette[0x48] = C64_AMBER_08;
    color_palette[0x49] = C64_AMBER_09;
    color_palette[0x4a] = C64_AMBER_10;
    color_palette[0x4b] = C64_AMBER_11;
    color_palette[0x4c] = C64_AMBER_12;
    color_palette[0x4d] = C64_AMBER_13;
    color_palette[0x4e] = C64_AMBER_14;
    color_palette[0x4f] = C64_AMBER_15;
    
    // the rest (0x50-0xff) is C64_BLUE
    for(int i = 0x50; i < 0x100; i++) color_palette[i] = C64_BLUE;

    // set irq line
    irq_line = true;
    
    frame_done = false;

    current_pixel = 0;
    current_xpos = 0;
    current_scanline = current_pixel >> 9;

    front_buffer = screen_buffer_1;
    back_buffer  = screen_buffer_0;
}

void E64::vicv::swap_buffers()
{
    uint32_t *temp = front_buffer;
    front_buffer = back_buffer;
    back_buffer = temp;
}

uint8_t E64::vicv::read_byte(uint8_t address)
{
    // temporary situation
    return registers[address];
}

void E64::vicv::write_byte(uint8_t address, uint8_t byte)
{
    registers[address] = byte;
}

void E64::vicv::render_current_scanline()
{
    if( (current_scanline < 32) || (current_scanline > 287) )
    {
        render_border_scanline();
    }
    else
    {
        render_scanline();
    }
    current_scanline++;
    if(current_scanline == 320)
    {
        render_overlay(117, 300, frame_delay.stats());
        swap_buffers();
        current_scanline = 0;
        frame_done = true;
    }
}

void E64::vicv::render_scanline(void) {
    // Reserve a byte for internal rendering use, defaults to 0
    uint8_t eight_pixels = 0;
    // 1st byte of screen buffer to be used for display
    int base = current_scanline * VICV_PIXELS_PER_SCANLINE;
    // get the current textrow, divide lineNo by 8
    int currentTextRow = ((current_scanline-32) >> 3);
    // get current line within character
    int currentCharacterLine = (current_scanline-32) & 0x07;

    uint32_t background_color = color_palette[registers[VICV_REG_BKG]];

    uint32_t start_screen_buffer = (registers[VICV_REG_TXT] << 24) + (registers[VICV_REG_TXT+1] << 16) + (registers[VICV_REG_TXT+2] << 8) + registers[VICV_REG_TXT+3];
    //uint32_t start_screen_buffer = (registers[VICV_REG_TSL] | (registers[VICV_REG_TSH] << 8)) << 11;
    uint32_t start_color_buffer = (registers[VICV_REG_COL] << 24) + (registers[VICV_REG_COL+1] << 16) + (registers[VICV_REG_COL+2] << 8) + registers[VICV_REG_COL+3];
    //uint32_t start_color_buffer = (registers[VICV_REG_CSL] | (registers[VICV_REG_CSH] << 8)) << 11;

    for(int x=0; x<VICV_PIXELS_PER_SCANLINE; x++)
    {
        // First, apply general background color stored @ VICV_REG0
        back_buffer[ base | x ] = background_color;
        // get current text column, current x divided by 8 (yields 0-63)
        int currentTextColumn = (x >> 3);
        uint8_t currentChar = computer.mmu_ic->ram[(start_screen_buffer | (((currentTextRow << 6) | currentTextColumn))) & 0x00ffffff];
        uint32_t currentCharColor = computer.vicv_ic->color_palette[computer.mmu_ic->ram[(start_color_buffer + (((currentTextRow << 6) | currentTextColumn))) & 0x00ffffff] ];
        if( !(x & 7) )
        {
            // if it's the first pixel of a char position, get byte information from char_rom
            eight_pixels = patched_char_rom[(currentChar<<3) | currentCharacterLine];
        }
        if(eight_pixels & 0x80)
        {
            // if leftmost bit is '1', then...
            back_buffer[base | x] = currentCharColor;
        }
        // shift all bits in internal byte 1 place to the left
        eight_pixels = eight_pixels << 1;
    }
}

void E64::vicv::render_border_scanline()
{
    int base = current_scanline * VICV_PIXELS_PER_SCANLINE;
    uint32_t background_color = color_palette[registers[VICV_REG_BOR]];
    for(int x = 0; x < VICV_PIXELS_PER_SCANLINE; x++)
    {
        back_buffer[base | x] = background_color;
    }
}

void E64::vicv::toggle_overlay()
{
    overlay_present = !overlay_present;
}

void E64::vicv::render_overlay(uint16_t xpos, uint16_t ypos, char *text)
{
    if(overlay_present) {
        uint32_t contrast_color = borders_contrast_foreground_color();
        uint32_t base = ((ypos * VICV_PIXELS_PER_SCANLINE) + xpos) % (VICV_PIXELS_PER_SCANLINE * 320);
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
                // is leftmost bit on?
                if(eight_pixels & 0x80)
                {
                    // draw it onto overlay
                    back_buffer[base + x] = contrast_color;
                }
                eight_pixels = eight_pixels << 1;
                x++;
                // increase the text pointer only when necessary
                if( !(x & 7) ) temp_text++;
            }
            // go to the next line
            base = (base + VICV_PIXELS_PER_SCANLINE) % (VICV_PIXELS_PER_SCANLINE * 320);
        }
    }
}

uint32_t E64::vicv::borders_contrast_foreground_color(void)
{
    uint32_t temp_color = computer.vicv_ic->color_palette[computer.vicv_ic->registers[VICV_REG_BOR]];
    uint8_t red  = (temp_color >> 16) & 0xff;
    uint8_t grn  = (temp_color >> 8) & 0xff;
    uint8_t blue = temp_color & 0xff;
    uint16_t color_sum = red + grn + blue;
    // 5 is the default value
    uint8_t intensity = 5;
    if( color_sum > (3*128) )
    {
        // reduce brightness
        red  = ((32 - intensity) * red ) >> 5;
        grn  = ((32 - intensity) * grn ) >> 5;
        blue = ((32 - intensity) * blue) >> 5;
    }
    else
    {
        // increase brightness
        uint16_t white_intensity = (intensity << 8) - intensity;
        red  = (white_intensity + ((32 - intensity) * red )) >> 5;
        grn  = (white_intensity + ((32 - intensity) * grn )) >> 5;
        blue = (white_intensity + ((32 - intensity) * blue)) >> 5;
    }
    return 0xff000000 | red << 16 | grn << 8 | blue;
}

uint16_t E64::vicv::return_current_scanline()
{
    return current_scanline;
}

uint16_t E64::vicv::return_current_pixel()
{
    return current_xpos;
}

void E64::vicv::run(uint32_t number_of_cycles)
{
    current_xpos = current_xpos + number_of_cycles;
    
    while( current_xpos >= VICV_PIXELS_PER_SCANLINE )
    {
        render_current_scanline();
        current_xpos -= VICV_PIXELS_PER_SCANLINE;
        //current_xpos &= (VICV_PIXELS_PER_SCANLINE - 1);
    }
}
