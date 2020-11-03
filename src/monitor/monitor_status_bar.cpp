//  monitor_status_bar.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <cstdio>

#include "common.hpp"
#include "monitor_status_bar.hpp"
#include "monitor_console.hpp"

uint8_t status_bar_chars[16*VICV_CHAR_COLUMNS];
uint16_t status_bar_foreground_color_buffer[16*VICV_CHAR_COLUMNS];
uint16_t status_bar_background_color_buffer[16*VICV_CHAR_COLUMNS];

char help_string[2048];
char help_string_2[2048];

void debug_status_bar_clear()
{
    for(int i=0; i<(debug_console.status_bar_rows * VICV_CHAR_COLUMNS); i++)
    {
        status_bar_chars[i] = ASCII_SPACE;
        //status_bar_chars[i] = ascii_to_screencode[ASCII_SPACE];
        status_bar_foreground_color_buffer[i] = COBALT_06;
        status_bar_background_color_buffer[i] = COBALT_02;
    }
    debug_console.status_bar_cursor_pos = 0;
    debug_console.status_bar_foreground_color = COBALT_06;  // default value
    debug_console.status_bar_background_color = COBALT_02;  // default value
}

void debug_status_bar_refresh()
{
    debug_status_bar_clear();
    
    // registers
    pc.m68k_ic->dump_registers(help_string);
    debug_status_bar_set_cursor_pos( 1*VICV_CHAR_COLUMNS +  0);
    debug_status_bar_print(help_string);
    
    // disassembly
    debug_status_bar_set_cursor_pos(10*VICV_CHAR_COLUMNS + 0);
    uint32_t temp_pc = pc.m68k_ic->getPC();
    for(int i=0; i<(debug_console.status_bar_rows - 10); i++ )
    {
        if(pc.m68k_ic->debugger.breakpoints.isSetAt(temp_pc))
        {
            debug_console.status_bar_foreground_color = AMBER_06; // bright amber
        }
        snprintf(help_string, 256, "%06x ", temp_pc );
        debug_status_bar_print(help_string);
        int no_of_bytes = pc.m68k_ic->disassemble(temp_pc, help_string);
        
        if( debug_console.status_bar_hex_view == true )
        {
            for(int i = 0; i< (no_of_bytes/2); i++)
            {
                snprintf(help_string_2, 256, "%04x ", pc.mmu_ic->read_memory_16(temp_pc + (2*i) ) );
                debug_status_bar_print(help_string_2);
            }
        }
        else
        {
            debug_status_bar_print(help_string);
        }
        
        debug_status_bar_putchar('\n');
        
        debug_console.status_bar_foreground_color = COBALT_06;  // revert to normal color
        
        temp_pc += no_of_bytes;
    }
    
    // vicv
    snprintf(help_string, 256, "    pixel: %3u\n scanline: %3u\n   hblank: %3u\n   vblank: %3u", pc.vicv_ic->get_current_pixel(), pc.vicv_ic->get_current_scanline(), pc.vicv_ic->is_hblank() ? 1 : 0, pc.vicv_ic->is_vblank() ? 1 : 0 );
    debug_status_bar_set_cursor_pos(1*VICV_CHAR_COLUMNS + 44);
    debug_status_bar_print(help_string);

    // set accent colors for titles etc...
    debug_console.status_bar_foreground_color = COBALT_07;
    debug_console.status_bar_background_color = COBALT_04;

    snprintf(help_string, 256, " CPU Status                              ");
    debug_status_bar_set_cursor_pos(0);
    debug_status_bar_print(help_string);
    snprintf(help_string, 256, " Disassembly                             ");
    debug_status_bar_set_cursor_pos(9*VICV_CHAR_COLUMNS + 0);
    debug_status_bar_print(help_string);
    snprintf(help_string, 256, "     vic v      ");
    debug_status_bar_set_cursor_pos(0*64 + 44);
    debug_status_bar_print(help_string);
}

void debug_status_bar_set_cursor_pos(uint16_t pos)
{
    // confine cursor
    debug_console.status_bar_cursor_pos = pos % debug_console.status_bar_total_chars;
    debug_console.status_bar_base_pos = debug_console.status_bar_cursor_pos % VICV_CHAR_COLUMNS;
}

void debug_status_bar_putchar(char character)
{
    character = character & 0x7f;
    switch(character)
    {
        case ASCII_LF:
            debug_console.status_bar_cursor_pos += VICV_CHAR_COLUMNS;
            debug_console.status_bar_cursor_pos -= (debug_console.status_bar_cursor_pos % VICV_CHAR_COLUMNS);
            debug_console.status_bar_cursor_pos += debug_console.status_bar_base_pos;
            break;
        default:
            status_bar_chars[debug_console.status_bar_cursor_pos] = character;
            //status_bar_chars[debug_console.status_bar_cursor_pos] = ascii_to_screencode[character];
            status_bar_foreground_color_buffer[debug_console.status_bar_cursor_pos] = debug_console.status_bar_foreground_color;
            status_bar_background_color_buffer[debug_console.status_bar_cursor_pos] = debug_console.status_bar_background_color;
            debug_console.status_bar_cursor_pos++;
            debug_console.status_bar_cursor_pos %= debug_console.status_bar_total_chars;
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