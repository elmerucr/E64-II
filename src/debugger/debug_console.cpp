//  debug_console.cpp
//  E64
//
//  Copyright © 2018 elmerucr. All rights reserved.

#include <cstdio>

#include "common.hpp"
#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "debug_command.hpp"
#include "debug_status_bar.hpp"

debug_console_struct debug_console;

char console_help_string[2048];

void debug_console_init()
{
    debug_console.cursor_pos = 0;
    // 55 is blink speed in xcode :-) 32 is c64
    debug_console.cursor_blink_time = 32;
    debug_console.current_foreground_color = COBALT_06;
    debug_console.current_background_color = COBALT_01;
    for(int i=0; i<2048; i++)
    {
        debug_console.console_character_buffer[i] = ascii_to_screencode[ASCII_SPACE];
        debug_console.console_foreground_color_buffer[i] = debug_console.current_foreground_color;
        debug_console.console_background_color_buffer[i] = debug_console.current_background_color;
    }
    debug_console_cursor_activate();

    // status bar stuff
    debug_console.status_bar_active = true;
    debug_console.status_bar_rows = 16;
    debug_console.status_bar_total_chars = debug_console.status_bar_rows * 64;
    debug_console.status_bar_cursor_pos = 0;
    debug_console.status_bar_base_pos = debug_console.status_bar_cursor_pos & (64-1);
    debug_status_bar_refresh();
    //
    for(int i=0; i<(debug_console.status_bar_rows + 1); i++) debug_console_print("\n");
    debug_console_version();
    debug_console_welcome();
}

void debug_console_version()
{
    snprintf(console_help_string,256,"E64-II (C)%i - version %i.%i.%i\n", E64_YEAR, E64_MAJOR_VERSION, E64_MINOR_VERSION, E64_BUILD);
    debug_console_print(console_help_string);
}

void debug_console_welcome()
{
    debug_console_print("Debugger - type <help> for more information\n");
}

void debug_console_add_bottom_row()
{
    debug_console.cursor_pos -= 64;
    // move all text one line up
    for(int i=0; i<(2048-64); i++)
    {
        debug_console.console_character_buffer[i] = debug_console.console_character_buffer[i+64];
        debug_console.console_foreground_color_buffer[i] = debug_console.console_foreground_color_buffer[i+64];
        debug_console.console_background_color_buffer[i] = debug_console.console_background_color_buffer[i+64];
    }
    uint16_t start_pos = debug_console.cursor_pos & 0xffc0;
    for(int i=0; i<64; i++)
    {
        debug_console.console_character_buffer[start_pos] = ascii_to_screencode[ASCII_SPACE];
        debug_console.console_foreground_color_buffer[start_pos] = debug_console.current_foreground_color;
        debug_console.console_background_color_buffer[start_pos] = debug_console.current_background_color;
        start_pos++;
    }
}

void debug_console_add_top_row()
{
    debug_console.cursor_pos += 64;
    for(int i=2047; i >= (debug_console.cursor_pos & 0xffc0) + 64; i--)
    {
        debug_console.console_character_buffer[i] = debug_console.console_character_buffer[i-64];
        debug_console.console_foreground_color_buffer[i] = debug_console.console_foreground_color_buffer[i-64];
        debug_console.console_background_color_buffer[i] = debug_console.console_background_color_buffer[i-64];
    }
    uint16_t start_pos = debug_console.cursor_pos & 0xffc0;
    for(int i=0; i<64; i++)
    {
        debug_console.console_character_buffer[start_pos] = ascii_to_screencode[ASCII_SPACE];
        debug_console.console_foreground_color_buffer[start_pos] = debug_console.current_foreground_color;
        debug_console.console_background_color_buffer[start_pos] = debug_console.current_background_color;
        start_pos++;
    }
}

void debug_console_put_char(char character)
{
    debug_console_cursor_deactivate();
    character = character & 0x7f;
    switch(character)
    {
        case ASCII_LF:
            debug_console.cursor_pos = (debug_console.cursor_pos + 64) & 0xffc0;
            break;
        case ASCII_CR:
            debug_console.cursor_pos = debug_console.cursor_pos & 0xffc0;
            break;
        default:
            debug_console.console_character_buffer[debug_console.cursor_pos] = ascii_to_screencode[character];
            debug_console.console_foreground_color_buffer[debug_console.cursor_pos] = debug_console.current_foreground_color;
            debug_console.console_background_color_buffer[debug_console.cursor_pos] = debug_console.current_background_color;
            debug_console.cursor_pos++;
            break;
    }
    // cursor out of current screen?
    if( debug_console.cursor_pos > 2047 ) debug_console_add_bottom_row();
    debug_console_cursor_activate();
}

void debug_console_put_screencode(char screencode)
{
    debug_console_cursor_deactivate();
    debug_console.console_character_buffer[debug_console.cursor_pos] = screencode;
    debug_console.console_foreground_color_buffer[debug_console.cursor_pos] = debug_console.current_foreground_color;
    debug_console.console_background_color_buffer[debug_console.cursor_pos] = debug_console.current_background_color;
    debug_console.cursor_pos++;
    // cursor out of current screen?
    if( debug_console.cursor_pos > 2047 ) debug_console_add_bottom_row();
    debug_console_cursor_activate();
}

void debug_console_print(const char *string_to_print)
{
    char *temp_char = (char *)string_to_print;
    while(*temp_char != ASCII_NULL)
    {
        debug_console_put_char(*temp_char);
        temp_char++;
    }
}

void debug_console_blit_to_debug_screen()
{
    for(int i = 0; i < 2048; i++)
    {
        debug_screen_character_buffer[i] = debug_console.console_character_buffer[i];
        debug_screen_foreground_color_buffer[i] = debug_console.console_foreground_color_buffer[i];
        debug_screen_background_color_buffer[i] = debug_console.console_background_color_buffer[i];
    }
    if( debug_console.status_bar_active == true )
    {
        for(int i = 0; i < (debug_console.status_bar_rows * 64); i++)
        {
            debug_screen_character_buffer[i] = status_bar_chars[i];
            debug_screen_foreground_color_buffer[i] = status_bar_foreground_color_buffer[i];
            debug_screen_background_color_buffer[i] = status_bar_background_color_buffer[i];
        }
    }
}

void debug_console_cursor_activate()
{
    if(debug_console.console_character_buffer[debug_console.cursor_pos] & 0x80)
    {
        debug_console.cursor_pos_original_reverse = true;
    }
    else
    {
        debug_console.cursor_pos_original_reverse = false;
    }
    debug_console.cursor_count_down = 0;
}

void debug_console_cursor_deactivate()
{
    if(debug_console.cursor_pos_original_reverse)
    {
        debug_console.console_character_buffer[debug_console.cursor_pos] = debug_console.console_character_buffer[debug_console.cursor_pos] | 0x80;
    }
    else
    {
        debug_console.console_character_buffer[debug_console.cursor_pos] = debug_console.console_character_buffer[debug_console.cursor_pos] & 0x7f;
    }
}

bool debug_console_cursor_flash()
{
    if(debug_console.cursor_count_down == 0)
    {
        // blink
        if(debug_console.console_character_buffer[debug_console.cursor_pos] & 0x80) {
            debug_console.console_character_buffer[debug_console.cursor_pos] = debug_console.console_character_buffer[debug_console.cursor_pos] & 0x7f;
        }
        else
        {
            debug_console.console_character_buffer[debug_console.cursor_pos] = debug_console.console_character_buffer[debug_console.cursor_pos] | 0x80;
        }
        debug_console.cursor_count_down = debug_console.cursor_blink_time;
        debug_console.cursor_count_down--;
        return true;
    }
    else
    {
        debug_console.cursor_count_down--;
        return false;
    }
}

void debug_console_arrow_left()
{
    debug_console_cursor_deactivate();
    debug_console.cursor_pos--;
    if( debug_console.status_bar_active)
    {
        if( debug_console.cursor_pos < (debug_console.status_bar_rows * 64) )
        {
            debug_console.cursor_pos++;
        }
    }
    else
    {
        if( debug_console.cursor_pos < 0 )
        {
            debug_console.cursor_pos++;
        }
    }
    debug_console_cursor_activate();
}

void debug_console_arrow_right()
{
    debug_console_cursor_deactivate();
    debug_console.cursor_pos++;
    // cursor out of current screen?
    if( debug_console.cursor_pos > 2047 ) debug_console_add_bottom_row();
    debug_console_cursor_activate();
}

void debug_console_arrow_down()
{
    debug_console_cursor_deactivate();
    debug_console.cursor_pos += 0x40;
    
    // cursor out of current screen?
    if( debug_console.cursor_pos > 2047 )
    {
        uint32_t address;
        
        switch( debug_console_check_output(false, &address) )
        {
            case NOTHING:
                debug_console_add_bottom_row();
                break;
            case ASCII:
                debug_console_add_bottom_row();
                E64::debug_command_memory_dump((address+8) & (RAM_SIZE - 1), 1);
                break;
            case CHARACTER:
                debug_console_add_bottom_row();
                E64::debug_command_memory_character_dump((address+16) & (RAM_SIZE - 1), 1);
                break;
        }
//        if( debug_console_check_output(false, &address))
//        {
//            debug_console_add_bottom_row();
//            E64::debug_command_memory_dump((address+8) & (RAM_SIZE - 1), 1);
//        }
//        else
//        {
//            debug_console_add_bottom_row();
//        }
    }
    debug_console_cursor_activate();
}

void debug_console_arrow_up()
{
    debug_console_cursor_deactivate();
    debug_console.cursor_pos -= 0x40;
    
    if(debug_console.cursor_pos<(debug_console.status_bar_active ? debug_console.status_bar_rows * 64 : 0))
    {
        uint32_t address;
        
        switch( debug_console_check_output(true, &address) )
        {
            case NOTHING:
                debug_console_add_top_row();
                break;
            case ASCII:
                debug_console_add_top_row();
                E64::debug_command_memory_dump((address-8) & (RAM_SIZE - 1), 1);
                break;
            case CHARACTER:
                debug_console_add_top_row();
                E64::debug_command_memory_character_dump((address-16) & (RAM_SIZE - 1), 1);
                break;
        }
    }
    debug_console_cursor_activate();
}

void debug_console_enter()
{
    // find starting position of the current row
    uint16_t start_of_row = debug_console.cursor_pos & 0xffc0;
    for(int i=0; i<64; i++)
    {
        console_help_string[i] = screencode_to_ascii[ (debug_console.console_character_buffer[start_of_row + i]) & 0x7f ];
    }
    console_help_string[64] = ASCII_NULL;
    E64::debug_command_execute(console_help_string);
}

void debug_console_backspace()
{
    debug_console_cursor_deactivate();
    debug_console.cursor_pos--;
    bool cursor_move = true;
    if( debug_console.status_bar_active)
    {
        if( debug_console.cursor_pos < (debug_console.status_bar_rows * 64) )
        {
            debug_console.cursor_pos++;
            cursor_move = false;
        }
    }
    else
    {
        if( debug_console.cursor_pos < 0 )
        {
            debug_console.cursor_pos++;
            cursor_move = false;
        }
    }
    if(cursor_move)
    {
        // move chars to the left
        for(int i = debug_console.cursor_pos; i < (debug_console.cursor_pos | 0x003f); i++)
        {
            debug_console.console_character_buffer[i] = debug_console.console_character_buffer[i+1];
            debug_console.console_foreground_color_buffer[i] = debug_console.console_foreground_color_buffer[i+1];
            debug_console.console_background_color_buffer[i] = debug_console.console_background_color_buffer[i+1];
        }
        // last char in current row becomes empty space
        debug_console.console_character_buffer[debug_console.cursor_pos | 0x003f] = ascii_to_screencode[ASCII_SPACE];
        debug_console.console_background_color_buffer[debug_console.cursor_pos | 0x003f] = debug_console.current_background_color;
        debug_console.console_foreground_color_buffer[debug_console.cursor_pos | 0x003f] = debug_console.current_foreground_color;
    }
    debug_console_cursor_activate();
}

void debug_console_insert()
{
    debug_console_cursor_deactivate();
    for(int i = debug_console.cursor_pos | 0x003f; i > debug_console.cursor_pos; i--)
    {
        debug_console.console_character_buffer[i] = debug_console.console_character_buffer[i-1];
        debug_console.console_foreground_color_buffer[i] = debug_console.console_foreground_color_buffer[i-1];
        debug_console.console_background_color_buffer[i] = debug_console.console_background_color_buffer[i-1];
    }
    debug_console.console_character_buffer[debug_console.cursor_pos] = ascii_to_screencode[ASCII_SPACE];
    debug_console.console_background_color_buffer[debug_console.cursor_pos] = debug_console.current_background_color;
    debug_console.console_foreground_color_buffer[debug_console.cursor_pos] = debug_console.current_foreground_color;
    debug_console_cursor_activate();
}

void debug_console_clear()
{
    debug_console.cursor_pos = 0;
    for(int i = 0; i < 2048; i++)
    {
        debug_console.console_character_buffer[i] = ascii_to_screencode[ASCII_SPACE];
        debug_console.console_background_color_buffer[i] = debug_console.current_background_color;
        debug_console.console_foreground_color_buffer[i] = debug_console.current_foreground_color;
    }
    if( debug_console.status_bar_active == true )
    {
        if( debug_console.cursor_pos < (debug_console.status_bar_rows * 64) )
        {
            debug_console.cursor_pos = debug_console.status_bar_rows * 64;
        }
    }
}

void debug_console_toggle_status_bar()
{
    if( debug_console.status_bar_active == true )
    {
        debug_console.status_bar_active = false;
    }
    else
    {
        debug_console.status_bar_active = true;
        if( debug_console.cursor_pos < (debug_console.status_bar_rows * 64) )
        {
            debug_console.cursor_pos = debug_console.status_bar_rows * 64;
        }
    }
}

enum monitor_type debug_console_check_output(bool top_down, uint32_t *address)
{
    enum monitor_type output_type = NOTHING;
    
    uint16_t start_pos = debug_console.status_bar_active ? 1024 : 0;
    
    for(int i = start_pos; i < 2048; i += 0x40)
    {
        if(debug_console.console_character_buffer[i] == ascii_to_screencode[':'] )
        {
            output_type = ASCII;
            
            char potential_address[7];
            for(int j=0; j<6; j++)
            {
                potential_address[j] = screencode_to_ascii[debug_console.console_character_buffer[i+1+j]];
            }
            potential_address[6] = 0;
            E64::debug_command_hex_string_to_int(potential_address, address);
            if(top_down) break;
        }
        if(debug_console.console_character_buffer[i] == ascii_to_screencode[';'] )
        {
            output_type = CHARACTER;
            
            char potential_address[7];
            for(int j=0; j<6; j++)
            {
                potential_address[j] = screencode_to_ascii[debug_console.console_character_buffer[i+1+j]];
            }
            potential_address[6] = 0;
            E64::debug_command_hex_string_to_int(potential_address, address);
            if(top_down) break;
        }
    }
    return output_type;
}
