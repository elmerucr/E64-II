//  tty.hpp
//  E64-II
//
//  Copyright © 2018-2021 elmerucr. All rights reserved.

#include <cstdint>

#ifndef TTY_HPP
#define TTY_HPP

namespace E64 {

enum output_type {
	NOTHING,
	ASCII,
	CHARACTER,
	BINARY,
	DISK
};

class tty_t {
public:
	uint8_t *character_buffer;
	uint16_t *foreground_color_buffer;
	uint16_t *background_color_buffer;
	
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
public:
	tty_t();
	~tty_t();
	
	void init();
	void version();
	void blit_to_screen();
	void putchar(char c);
	void put_screencode(char sc);
	void puts(const char *string_to_print);
	int print(const char *format, ...);
	void prompt();
	
	void cursor_activate();
	void cursor_deactivate();
	bool cursor_flash();         // returns true if state of cursor changed
	
	void enter();
	void insert();
	void backspace();
	
	void arrow_left();
	void arrow_right();
	void arrow_up();
	void arrow_down();
	
	void add_bottom_row();
	
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
	enum output_type check_output(bool top_down, uint32_t *address);
	
	void add_top_row();
	void clear();

	void toggle_status_bar();
};

}

#endif
