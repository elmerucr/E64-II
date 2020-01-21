//  debug_status_bar_E64_II.cpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <cstdio>

#include "common_defs.hpp"
#include "debug_status_bar_E64_II.hpp"
#include "debug_console.hpp"

uint8_t status_bar_chars[16*64];
uint8_t status_bar_foreground_color_buffer[16*64];
uint8_t status_bar_background_color_buffer[16*64];

char help_string[2048];
char help_string_2[2048];

void debug_status_bar_clear()
{
    for(int i=0; i<(debug_console.status_bar_rows * 64); i++)
    {
        status_bar_chars[i] = ascii_to_screencode[ASCII_SPACE];
        status_bar_foreground_color_buffer[i] = 0x3d;
        status_bar_background_color_buffer[i] = 0x36;
    }
    debug_console.status_bar_cursor_pos = 0;
    debug_console.status_bar_foreground_color = 0x3d;  // default value
    debug_console.status_bar_background_color = 0x36;  // default value
}

void debug_status_bar_refresh()
{
    debug_status_bar_clear();
    
    // registers
    computer.m68k_ic->dump_registers(help_string);
    debug_status_bar_set_cursor_pos( 1*64 +  0);
    debug_status_bar_print(help_string);
    
    // disassembly
    debug_status_bar_set_cursor_pos(10*64 + 0);
    uint32_t temp_pc = computer.m68k_ic->getPC();
    for(int i=0; i<6; i++ )
    {
        if(computer.m68k_ic->debugger.breakpoints.isSetAt(temp_pc)) debug_console.status_bar_foreground_color = 0x4b; // bright amber
        snprintf(help_string, 256, "%06x ", temp_pc );
        debug_status_bar_print(help_string);
        int no_of_bytes = computer.m68k_ic->disassemble(temp_pc, help_string);
        switch( no_of_bytes )
        {
            case 2:
                snprintf(help_string_2, 256, "%04x           ", computer.mmu_ic->read_memory_16(temp_pc));
                debug_status_bar_print(help_string_2);
                break;
            case 4:
                snprintf(help_string_2, 256, "%04x %04x      ", computer.mmu_ic->read_memory_16(temp_pc), computer.mmu_ic->read_memory_16(temp_pc+2));
                debug_status_bar_print(help_string_2);
                break;
            case 6:
                snprintf(help_string_2, 256, "%04x %04x %04x ", computer.mmu_ic->read_memory_16(temp_pc), computer.mmu_ic->read_memory_16(temp_pc+2), computer.mmu_ic->read_memory_16(temp_pc+4));
                debug_status_bar_print(help_string_2);
                break;
            default:
                snprintf(help_string_2, 256, "%04x %04x %04x ", computer.mmu_ic->read_memory_16(temp_pc), computer.mmu_ic->read_memory_16(temp_pc+2), computer.mmu_ic->read_memory_16(temp_pc+4));
                debug_status_bar_print(help_string_2);
                status_bar_foreground_color_buffer[debug_console.status_bar_cursor_pos-4] -= 2;
                status_bar_foreground_color_buffer[debug_console.status_bar_cursor_pos-3] -= 4;
                status_bar_foreground_color_buffer[debug_console.status_bar_cursor_pos-2] -= 6;
                break;
        };
        debug_status_bar_print(help_string);
        debug_status_bar_putchar('\n');
        debug_console.status_bar_foreground_color = 0x3d; // revert to normal color
        temp_pc += no_of_bytes;
    }
    
    // vicv scanlines
    snprintf(help_string, 256, "line: %3u\npix : %3u", computer.vicv_ic->return_current_scanline(), computer.vicv_ic->return_current_pixel() );
    debug_status_bar_set_cursor_pos(5*64 + 54);
    debug_status_bar_print(help_string);

    // set accent colors for titles etc...
    debug_console.status_bar_foreground_color = 0x3f;
    debug_console.status_bar_background_color = 0x39;

    snprintf(help_string, 256, " CPU Status                              ");
    debug_status_bar_set_cursor_pos(0);
    debug_status_bar_print(help_string);
    snprintf(help_string, 256, " Disassembly                                                    ");
    debug_status_bar_set_cursor_pos(9*64);
    debug_status_bar_print(help_string);
    snprintf(help_string, 256, "  vic v  ");
    debug_status_bar_set_cursor_pos(4*64 + 54);
    debug_status_bar_print(help_string);
}

void debug_status_bar_set_cursor_pos(uint16_t pos)
{
    // confine cursor
    debug_console.status_bar_cursor_pos = pos & (debug_console.status_bar_total_chars - 1);
    debug_console.status_bar_base_pos = debug_console.status_bar_cursor_pos & (64 - 1);
}

void debug_status_bar_putchar(char character)
{
    character = character & 0x7f;
    switch(character)
    {
        case ASCII_LF:
            debug_console.status_bar_cursor_pos += 64;
            debug_console.status_bar_cursor_pos &= 0xffc0;
            debug_console.status_bar_cursor_pos += debug_console.status_bar_base_pos;
            debug_console.status_bar_cursor_pos &= debug_console.status_bar_total_chars - 1;
            break;
        default:
            status_bar_chars[debug_console.status_bar_cursor_pos] = ascii_to_screencode[character];
            status_bar_foreground_color_buffer[debug_console.status_bar_cursor_pos] = debug_console.status_bar_foreground_color;
            status_bar_background_color_buffer[debug_console.status_bar_cursor_pos] = debug_console.status_bar_background_color;
            debug_console.status_bar_cursor_pos++;
            debug_console.status_bar_cursor_pos &= debug_console.status_bar_total_chars - 1;
            break;
    }
}

void debug_status_bar_print(const char *string_to_print)
{
    char *temp_char = (char *)string_to_print;
    while(*temp_char != ASCII_NULL)
    {
        debug_status_bar_putchar(*temp_char);
        temp_char++;
    }
}
