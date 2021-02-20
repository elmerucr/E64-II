//  monitor_console.cpp
//  E64-II
//
//  Copyright © 2018-2021 elmerucr. All rights reserved.

#include <cstdio>

#include "common.hpp"
#include "monitor_console.hpp"
#include "screen.hpp"
#include "monitor_command.hpp"
#include "monitor_status_bar.hpp"

monitor_console monitor_console_0;

char console_help_string[2048];

void debug_console_init()
{
	monitor_console_0.cursor_pos = 0;
    // 55 is blink speed in xcode :-) 32 is c64
	monitor_console_0.cursor_blink_time = 32;
	monitor_console_0.current_foreground_color = COBALT_06;
	monitor_console_0.current_background_color = COBALT_01;
    for(int i=0; i<(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS; i++)
    {
	    monitor_console_0.character_buffer[i] = ASCII_SPACE;
	    monitor_console_0.foreground_color_buffer[i] = monitor_console_0.current_foreground_color;
	    monitor_console_0.background_color_buffer[i] = monitor_console_0.current_background_color;
    }
    debug_console_cursor_activate();

    // status bar stuff
	monitor_console_0.status_bar_active = false;
	monitor_console_0.status_bar_rows = 14;
	monitor_console_0.status_bar_total_chars = monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS;
	monitor_console_0.status_bar_cursor_pos = 0;
	monitor_console_0.status_bar_base_pos = monitor_console_0.status_bar_cursor_pos & (64-1);
	monitor_console_0.status_bar_hex_view = false;
    debug_status_bar_refresh();

	debug_console_put_char('\n');
	//for(int i=0; i<(debug_console.status_bar_rows + 1); i++) debug_console_print("\n");
	debug_console_version();
	debug_console_prompt();
}

void debug_console_version()
{
    snprintf(console_help_string,256,"E64-II (C)%i - version %i.%i (%i)\n", E64_II_YEAR, E64_II_MAJOR_VERSION, E64_II_MINOR_VERSION, E64_II_BUILD);
    debug_console_print(console_help_string);
}

void debug_console_add_bottom_row()
{
	monitor_console_0.cursor_pos -= VICV_CHAR_COLUMNS;
    // move all text one line up
    for(int i=0; i<((VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8))-VICV_CHAR_COLUMNS); i++)
    {
	    monitor_console_0.character_buffer[i] = monitor_console_0.character_buffer[i+VICV_CHAR_COLUMNS];
	    monitor_console_0.foreground_color_buffer[i] = monitor_console_0.foreground_color_buffer[i+VICV_CHAR_COLUMNS];
	    monitor_console_0.background_color_buffer[i] = monitor_console_0.background_color_buffer[i+VICV_CHAR_COLUMNS];
    }
    uint16_t start_pos = monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS);
    for(int i=0; i<VICV_CHAR_COLUMNS; i++)
    {
	    monitor_console_0.character_buffer[start_pos] = ASCII_SPACE;
	    monitor_console_0.foreground_color_buffer[start_pos] = monitor_console_0.current_foreground_color;
	    monitor_console_0.background_color_buffer[start_pos] = monitor_console_0.current_background_color;
        start_pos++;
    }
}

void debug_console_add_top_row()
{
	monitor_console_0.cursor_pos += VICV_CHAR_COLUMNS;
    for(int i=((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1; i >= (monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS)) + VICV_CHAR_COLUMNS; i--)
    {
	    monitor_console_0.character_buffer[i] = monitor_console_0.character_buffer[i-VICV_CHAR_COLUMNS];
	    monitor_console_0.foreground_color_buffer[i] = monitor_console_0.foreground_color_buffer[i-VICV_CHAR_COLUMNS];
	    monitor_console_0.background_color_buffer[i] = monitor_console_0.background_color_buffer[i-VICV_CHAR_COLUMNS];
    }
    uint16_t start_pos = monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS);
    for(int i=0; i<VICV_CHAR_COLUMNS; i++)
    {
	    monitor_console_0.character_buffer[start_pos] = ASCII_SPACE;
	    monitor_console_0.foreground_color_buffer[start_pos] = monitor_console_0.current_foreground_color;
	    monitor_console_0.background_color_buffer[start_pos] = monitor_console_0.current_background_color;
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
		    monitor_console_0.cursor_pos += VICV_CHAR_COLUMNS;
		    monitor_console_0.cursor_pos = monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS);
            break;
        case ASCII_CR:
		    monitor_console_0.cursor_pos = monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS);
            break;
        default:
		    monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = character;
		    monitor_console_0.foreground_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_foreground_color;
		    monitor_console_0.background_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_background_color;
		    monitor_console_0.cursor_pos++;
            break;
    }
    // cursor out of current screen?
    if( monitor_console_0.cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1) ) debug_console_add_bottom_row();
    debug_console_cursor_activate();
}

void debug_console_put_screencode(char screencode)
{
    debug_console_cursor_deactivate();
	monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = screencode;
	monitor_console_0.foreground_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_foreground_color;
	monitor_console_0.background_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_background_color;
	monitor_console_0.cursor_pos++;
    // cursor out of current screen?
    if( monitor_console_0.cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1) ) debug_console_add_bottom_row();
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
    for(int i = 0; i < ((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS); i++)
    {
        screen_character_buffer[i] = monitor_console_0.character_buffer[i];
        screen_foreground_color_buffer[i] = monitor_console_0.foreground_color_buffer[i];
        screen_background_color_buffer[i] = monitor_console_0.background_color_buffer[i];
    }
    if( monitor_console_0.status_bar_active == true )
    {
        for(int i = 0; i < (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS); i++)
        {
            screen_character_buffer[i] = status_bar_chars[i];
            screen_foreground_color_buffer[i] = status_bar_foreground_color_buffer[i];
            screen_background_color_buffer[i] = status_bar_background_color_buffer[i];
        }
    }
}

void debug_console_cursor_activate()
{
    if(monitor_console_0.character_buffer[monitor_console_0.cursor_pos] & 0x80)
    {
	    monitor_console_0.cursor_pos_original_reverse = true;
    }
    else
    {
	    monitor_console_0.cursor_pos_original_reverse = false;
    }
	monitor_console_0.cursor_count_down = 0;
}

void debug_console_cursor_deactivate()
{
    if(monitor_console_0.cursor_pos_original_reverse)
    {
	    monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = monitor_console_0.character_buffer[monitor_console_0.cursor_pos] | 0x80;
    }
    else
    {
	    monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = monitor_console_0.character_buffer[monitor_console_0.cursor_pos] & 0x7f;
    }
}

bool debug_console_cursor_flash()
{
    if(monitor_console_0.cursor_count_down == 0)
    {
        // blink
        if(monitor_console_0.character_buffer[monitor_console_0.cursor_pos] & 0x80) {
		monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = monitor_console_0.character_buffer[monitor_console_0.cursor_pos] & 0x7f;
        }
        else
        {
		monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = monitor_console_0.character_buffer[monitor_console_0.cursor_pos] | 0x80;
        }
	    monitor_console_0.cursor_count_down = monitor_console_0.cursor_blink_time;
	    monitor_console_0.cursor_count_down--;
        return true;
    }
    else
    {
	    monitor_console_0.cursor_count_down--;
        return false;
    }
}

void debug_console_arrow_left()
{
    debug_console_cursor_deactivate();
	monitor_console_0.cursor_pos--;
    if( monitor_console_0.status_bar_active)
    {
        if( monitor_console_0.cursor_pos < (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS) )
        {
		monitor_console_0.cursor_pos++;
        }
    }
    else
    {
        if( monitor_console_0.cursor_pos < 0 )
        {
		monitor_console_0.cursor_pos++;
        }
    }
    debug_console_cursor_activate();
}

void debug_console_arrow_right()
{
    debug_console_cursor_deactivate();
	monitor_console_0.cursor_pos++;
    // cursor out of current screen?
    if( monitor_console_0.cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1) ) debug_console_add_bottom_row();
    debug_console_cursor_activate();
}

void debug_console_arrow_down()
{
    debug_console_cursor_deactivate();
	monitor_console_0.cursor_pos += VICV_CHAR_COLUMNS;
    
    // cursor out of current screen?
    if( monitor_console_0.cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1) )
    {
        uint32_t address;
        
        switch (debug_console_check_output(false, &address))
        {
            case NOTHING:
                debug_console_add_bottom_row();
                break;
            case ASCII:
                debug_console_add_bottom_row();
                E64::monitor_command_memory_dump((address+8) & (RAM_SIZE - 1), 1);
                break;
            case CHARACTER:
                debug_console_add_bottom_row();
                E64::monitor_command_memory_character_dump((address+16) & (RAM_SIZE - 1), 1);
                break;
		case BINARY:
			debug_console_add_bottom_row();
			E64::monitor_command_memory_binary_dump((address + 1) & (RAM_SIZE - 1), 1);
			break;
		case DISK:
			debug_console_add_bottom_row();
			E64::monitor_command_fd_dump(address + 0x08, 1);
			break;
        }
    }
    debug_console_cursor_activate();
}

void debug_console_arrow_up()
{
    debug_console_cursor_deactivate();
	monitor_console_0.cursor_pos -= VICV_CHAR_COLUMNS;
    
    if(monitor_console_0.cursor_pos<(monitor_console_0.status_bar_active ? monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS : 0))
    {
        uint32_t address;
        
        switch( debug_console_check_output(true, &address) )
        {
            case NOTHING:
                debug_console_add_top_row();
                break;
            case ASCII:
                debug_console_add_top_row();
                E64::monitor_command_memory_dump((address-8) & (RAM_SIZE - 1), 1);
                break;
            case CHARACTER:
                debug_console_add_top_row();
                E64::monitor_command_memory_character_dump((address-16) & (RAM_SIZE - 1), 1);
                break;
		case BINARY:
			debug_console_add_top_row();
			E64::monitor_command_memory_binary_dump((address - 1) & (RAM_SIZE - 1), 1);
			break;
		case DISK:
			debug_console_add_top_row();
			E64::monitor_command_fd_dump(address - 0x08, 1);
			break;
        }
    }
    debug_console_cursor_activate();
}

void debug_console_enter()
{
    // find starting position of the current row
    uint16_t start_of_row = monitor_console_0.cursor_pos - (monitor_console_0.cursor_pos % VICV_CHAR_COLUMNS);
    
    /*  Copy screen contents into helper character string
     */
    for(int i=0; i<64; i++)
    {
        console_help_string[i] = (monitor_console_0.character_buffer[start_of_row + i]) & 0x7f;
    }
    
    console_help_string[64] = ASCII_NULL;
    
    E64::monitor_command_execute(console_help_string);
}

void debug_console_backspace()
{
    debug_console_cursor_deactivate();
	monitor_console_0.cursor_pos--;
    bool cursor_move = true;
    if( monitor_console_0.status_bar_active)
    {
        if( monitor_console_0.cursor_pos < (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS) )
        {
		monitor_console_0.cursor_pos++;
            cursor_move = false;
        }
    }
    else
    {
        if( monitor_console_0.cursor_pos < 0 )
        {
		monitor_console_0.cursor_pos++;
            cursor_move = false;
        }
    }
    if(cursor_move)
    {
        // move chars to the left
        for(int i = monitor_console_0.cursor_pos; i < (monitor_console_0.cursor_pos | 0x003f); i++)
        {
		monitor_console_0.character_buffer[i] = monitor_console_0.character_buffer[i+1];
		monitor_console_0.foreground_color_buffer[i] = monitor_console_0.foreground_color_buffer[i+1];
		monitor_console_0.background_color_buffer[i] = monitor_console_0.background_color_buffer[i+1];
        }
        // last char in current row becomes empty space
	    monitor_console_0.character_buffer[monitor_console_0.cursor_pos | 0x003f] = ASCII_SPACE;
	    monitor_console_0.background_color_buffer[monitor_console_0.cursor_pos | 0x003f] = monitor_console_0.current_background_color;
	    monitor_console_0.foreground_color_buffer[monitor_console_0.cursor_pos | 0x003f] = monitor_console_0.current_foreground_color;
    }
    debug_console_cursor_activate();
}

void debug_console_insert()
{
    debug_console_cursor_deactivate();
    for(int i = monitor_console_0.cursor_pos | 0x003f; i > monitor_console_0.cursor_pos; i--)
    {
	    monitor_console_0.character_buffer[i] = monitor_console_0.character_buffer[i-1];
	    monitor_console_0.foreground_color_buffer[i] = monitor_console_0.foreground_color_buffer[i-1];
	    monitor_console_0.background_color_buffer[i] = monitor_console_0.background_color_buffer[i-1];
    }
	monitor_console_0.character_buffer[monitor_console_0.cursor_pos] = ASCII_SPACE;
	monitor_console_0.background_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_background_color;
	monitor_console_0.foreground_color_buffer[monitor_console_0.cursor_pos] = monitor_console_0.current_foreground_color;
    debug_console_cursor_activate();
}

void debug_console_clear()
{
	monitor_console_0.cursor_pos = 0;
	for (int i = 0; i < (VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8)); i++) {
		monitor_console_0.character_buffer[i] = ASCII_SPACE;
		monitor_console_0.background_color_buffer[i] =
			monitor_console_0.current_background_color;
		monitor_console_0.foreground_color_buffer[i] =
			monitor_console_0.current_foreground_color;
	}
	if (monitor_console_0.status_bar_active == true) {
		if (monitor_console_0.cursor_pos <
		    (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS)) {
			monitor_console_0.cursor_pos =
			monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS;
		}
	}
}

void debug_console_toggle_status_bar()
{
	if (monitor_console_0.status_bar_active == true) {
		monitor_console_0.status_bar_active = false;
	} else {
		debug_console_cursor_deactivate();
		monitor_console_0.status_bar_active = true;
		if (monitor_console_0.cursor_pos <
		    (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS)) {
			monitor_console_0.cursor_pos =
			monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS;
		}
		debug_console_cursor_activate();
	}
}

enum monitor_type debug_console_check_output(bool top_down, uint32_t *address)
{
	enum monitor_type output_type = NOTHING;
    
	uint16_t start_pos =
	monitor_console_0.status_bar_active ? (monitor_console_0.status_bar_rows * VICV_CHAR_COLUMNS) : 0;
    
	for (int i = start_pos; i < (VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8)); i += VICV_CHAR_COLUMNS) {
		if (monitor_console_0.character_buffer[i] == ':') {
			output_type = ASCII;

			char potential_address[7];
			for(int j=0; j<6; j++) {
				potential_address[j] =
				monitor_console_0.character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down)
				break;
		} else if (monitor_console_0.character_buffer[i] == ';') {
			output_type = CHARACTER;

			char potential_address[7];
			for (int j=0; j<6; j++) {
				potential_address[j] =
				monitor_console_0.character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down)
				break;
		} else if (monitor_console_0.character_buffer[i] == '\'') {
			output_type = BINARY;
		
			char potential_address[7];
			for (int j=0; j<6; j++) {
				potential_address[j] =
				monitor_console_0.character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down) break;
		} else if (monitor_console_0.character_buffer[i] == '"') {
			output_type = DISK;
			
			char potential_sector[9];
			uint32_t sector;
			for (int j=0; j<8; j++) {
				potential_sector[j] =
					monitor_console_0.character_buffer[i+3+j];
			}
			potential_sector[8] = 0;
			E64::monitor_command_hex_string_to_int(potential_sector, &sector);
			
			char potential_offset[5];
			uint32_t offset;
			for (int j=0; j<4; j++) {
				potential_offset[j] =
					monitor_console_0.character_buffer[i+12+j];
			}
			potential_offset[4] = 0;
			E64::monitor_command_hex_string_to_int(potential_offset, &offset);
			
			*address = (sector * pc.fd0->bytes_per_sector()) + offset;
			if (top_down) break;
		}
	}
	return output_type;
}

void debug_console_prompt()
{
	debug_console_print("\nready.\n");
}
