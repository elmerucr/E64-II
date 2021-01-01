//  monitor_console.hpp
//  E64-II
//
//  Copyright © 2018-2021 elmerucr. All rights reserved.

#include <cstdint>
#include "common.hpp"

#ifndef MONITOR_CONSOLE_HPP
#define MONITOR_CONSOLE_HPP

enum monitor_type {
	NOTHING,
	ASCII,
	CHARACTER,
	BINARY,
	DISK
};

struct monitor_console {
	uint8_t character_buffer[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];
	uint16_t foreground_color_buffer[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];
	uint16_t background_color_buffer[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];

	// cursor related vars
	int16_t		cursor_pos;
	bool		cursor_pos_original_reverse;
	uint8_t		cursor_blink_time;
	uint8_t		cursor_count_down;

	uint16_t  	current_foreground_color;
	uint16_t  	current_background_color;

	// status bar related things
	bool		status_bar_active;
	uint8_t		status_bar_rows;
	uint16_t	status_bar_total_chars;
	uint16_t	status_bar_cursor_pos;
	uint16_t	status_bar_base_pos;
	uint16_t	status_bar_foreground_color;
	uint16_t	status_bar_background_color;
	bool		status_bar_hex_view;
};

extern monitor_console monitor_console_0;

void debug_console_init();
void debug_console_version();
void debug_console_blit_to_debug_screen();
void debug_console_put_char(char character);
void debug_console_put_screencode(char screencode);
void debug_console_print(const char *string_to_print);
void debug_console_prompt();

void debug_console_cursor_activate();
void debug_console_cursor_deactivate();
bool debug_console_cursor_flash();         // returns true if state of cursor changed

void debug_console_enter();
void debug_console_insert();
void debug_console_backspace();
void debug_console_arrow_left();
void debug_console_arrow_right();

void debug_console_arrow_up();

void debug_console_arrow_down();

void debug_console_add_bottom_row();

/*
 * This function checks the screen output for presence of specific
 * monitor output such as ':' at the beginning of a line.
 *
 * The function will return the type if something was found.
 *
 * If the 1st argument is TRUE, it will look top down, if FALSE, bottom
 * up. The 2nd argument is a pointer to an uint16_t in which the result
 * will be written if something was found.
 */
enum monitor_type debug_console_check_output(bool top_down, uint32_t *address);

void debug_console_add_top_row();
void debug_console_clear();

void debug_console_toggle_status_bar();

#endif
