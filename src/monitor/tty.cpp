//  tty.cpp
//  E64-II
//
//  Copyright © 2018-2021 elmerucr. All rights reserved.

#include <cstdio>

#include "common.hpp"
#include "tty.hpp"
#include "command.hpp"

char console_help_string[2048];

E64::tty_t::tty_t()
{
	character_buffer = new uint8_t[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];
	foreground_color_buffer = new uint16_t[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];
	background_color_buffer = new uint16_t[(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS];
	
	init();
}

E64::tty_t::~tty_t()
{
	delete character_buffer;
	delete foreground_color_buffer;
	delete background_color_buffer;
}

void E64::tty_t::init()
{
	cursor_pos = 0;
	// 55 is blink speed in xcode :-) 32 is c64
	cursor_blink_time = 32;
	current_foreground_color = COBALT_06;
	current_background_color = COBALT_01;
	for (int i=0; i<(VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS; i++) {
		character_buffer[i] = ASCII_SPACE;
		foreground_color_buffer[i] = current_foreground_color;
		background_color_buffer[i] = current_background_color;
	}
	cursor_activate();

	// status bar stuff
	status_bar_active = false;
	status_bar_rows = 14;
	status_bar_total_chars = status_bar_rows * VICV_CHAR_COLUMNS;
	status_bar_cursor_pos = 0;
	status_bar_base_pos = status_bar_cursor_pos & (64-1);
	status_bar_hex_view = false;
	putchar('\n');
	version();
	prompt();
}

void E64::tty_t::version()
{
	snprintf(console_help_string,256,"E64-II (C)%i - version %i.%i (%i)\n", E64_II_YEAR, E64_II_MAJOR_VERSION, E64_II_MINOR_VERSION, E64_II_BUILD);
	print(console_help_string);
}

void E64::tty_t::blit_to_screen()
{
	for (int i = 0; i < ((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS); i++) {
		monitor.screen->character_buffer[i] = character_buffer[i];
		monitor.screen->foreground_color_buffer[i] = foreground_color_buffer[i];
		monitor.screen->background_color_buffer[i] = background_color_buffer[i];
	}
	if (status_bar_active == true) {
		for (int i = 0; i < (status_bar_rows * VICV_CHAR_COLUMNS); i++) {
			monitor.screen->character_buffer[i] = monitor.status_bar->chars[i];
			monitor.screen->foreground_color_buffer[i] = monitor.status_bar->foreground_color_buffer[i];
			monitor.screen->background_color_buffer[i] = monitor.status_bar->background_color_buffer[i];
		}
	}
}

void E64::tty_t::putchar(char c)
{
	cursor_deactivate();
	c = c & 0x7f;
	switch (c) {
		case ASCII_LF:
			cursor_pos += VICV_CHAR_COLUMNS;
			cursor_pos = cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS);
			break;
		case ASCII_CR:
		    	cursor_pos = cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS);
			break;
		default:
			character_buffer[cursor_pos] = c;
			foreground_color_buffer[cursor_pos] = current_foreground_color;
			background_color_buffer[cursor_pos] = current_background_color;
			cursor_pos++;
			break;
	}
	// cursor out of current screen?
	if (cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1))
		add_bottom_row();
	cursor_activate();
}

void E64::tty_t::put_screencode(char sc)
{
	cursor_deactivate();
	character_buffer[cursor_pos] = sc;
	foreground_color_buffer[cursor_pos] = current_foreground_color;
	background_color_buffer[cursor_pos] = current_background_color;
	cursor_pos++;
	// cursor out of current screen?
	if (cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1))
		add_bottom_row();
    	cursor_activate();
}

void E64::tty_t::print(const char *string_to_print)
{
	char *temp_char = (char *)string_to_print;
	while (*temp_char != ASCII_NULL) {
		putchar(*temp_char);
		temp_char++;
	}
}

void E64::tty_t::prompt()
{
	print("\nready.\n");
}

void E64::tty_t::cursor_activate()
{
	if (character_buffer[cursor_pos] & 0x80) {
		cursor_pos_original_reverse = true;
	} else {
	    	cursor_pos_original_reverse = false;
	}
	cursor_count_down = 0;
}

void E64::tty_t::cursor_deactivate()
{
	if (cursor_pos_original_reverse) {
		character_buffer[cursor_pos] = character_buffer[cursor_pos] | 0x80;
	} else {
		character_buffer[cursor_pos] = character_buffer[cursor_pos] & 0x7f;
	}
}

bool E64::tty_t::cursor_flash()
{
	if (cursor_count_down == 0) {
		// blink
		if (character_buffer[cursor_pos] & 0x80) {
			character_buffer[cursor_pos] = character_buffer[cursor_pos] & 0x7f;
		} else {
			character_buffer[cursor_pos] = character_buffer[cursor_pos] | 0x80;
		}
	    	cursor_count_down = cursor_blink_time;
	    	cursor_count_down--;
		return true;
	} else {
		cursor_count_down--;
		return false;
	}
}

void E64::tty_t::enter()
{
	// find starting position of the current row
	uint16_t start_of_row = cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS);
    
	/* Copy screen contents into helper character string */
	for (int i=0; i<64; i++) {
		console_help_string[i] = (character_buffer[start_of_row + i]) & 0x7f;
	}
    
	console_help_string[64] = ASCII_NULL;
    
	E64::monitor_command_execute(console_help_string);
}

void E64::tty_t::insert()
{
	cursor_deactivate();
	for (int i = cursor_pos | 0x003f; i > cursor_pos; i--) {
		character_buffer[i] = character_buffer[i-1];
		foreground_color_buffer[i] = foreground_color_buffer[i-1];
		background_color_buffer[i] = background_color_buffer[i-1];
	}
	character_buffer[cursor_pos] = ASCII_SPACE;
	background_color_buffer[cursor_pos] = current_background_color;
	foreground_color_buffer[cursor_pos] = current_foreground_color;
	cursor_activate();
}

void E64::tty_t::backspace()
{
	cursor_deactivate();
	cursor_pos--;
	bool cursor_move = true;
	if (status_bar_active) {
		if (cursor_pos < (status_bar_rows * VICV_CHAR_COLUMNS) ) {
			cursor_pos++;
			cursor_move = false;
		}
	} else {
		if (cursor_pos < 0) {
			cursor_pos++;
			cursor_move = false;
		}
	}
	if (cursor_move) {
		// move chars to the left
		for (int i = cursor_pos; i < (cursor_pos | 0x003f); i++) {
			character_buffer[i] = character_buffer[i+1];
			foreground_color_buffer[i] = foreground_color_buffer[i+1];
			background_color_buffer[i] = background_color_buffer[i+1];
		}
		// last char in current row becomes empty space
		character_buffer[cursor_pos | 0x003f] = ASCII_SPACE;
	    	background_color_buffer[cursor_pos | 0x003f] = current_background_color;
	    	foreground_color_buffer[cursor_pos | 0x003f] = current_foreground_color;
	}
	cursor_activate();
}

void E64::tty_t::arrow_left()
{
	cursor_deactivate();
	cursor_pos--;
	if (status_bar_active) {
		if (cursor_pos < (status_bar_rows * VICV_CHAR_COLUMNS)) {
			cursor_pos++;
		}
	} else {
		if (cursor_pos < 0) {
			cursor_pos++;
		}
	}
	cursor_activate();
}

void E64::tty_t::arrow_right()
{
	cursor_deactivate();
	cursor_pos++;
	// cursor out of current screen?
	if (cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1))
		add_bottom_row();
	cursor_activate();
}

void E64::tty_t::arrow_up()
{
    	cursor_deactivate();
	cursor_pos -= VICV_CHAR_COLUMNS;

	if (cursor_pos<(status_bar_active ? status_bar_rows * VICV_CHAR_COLUMNS : 0)) {
		uint32_t address;
	
		switch (check_output(true, &address)) {
			case E64::NOTHING:
				add_top_row();
				break;
			case E64::ASCII:
				add_top_row();
				E64::monitor_command_memory_dump((address-8) & (RAM_SIZE - 1), 1);
				break;
			case E64::CHARACTER:
				add_top_row();
				E64::monitor_command_memory_character_dump((address-16) & (RAM_SIZE - 1), 1);
				break;
			case E64::BINARY:
				add_top_row();
				E64::monitor_command_memory_binary_dump((address - 1) & (RAM_SIZE - 1), 1);
				break;
			case E64::DISK:
				add_top_row();
				E64::monitor_command_fd_dump(address - 0x08, 1);
				break;
		}
	}
	cursor_activate();
}

void E64::tty_t::arrow_down()
{
	cursor_deactivate();
	cursor_pos += VICV_CHAR_COLUMNS;
    
	// cursor out of current screen?
	if (cursor_pos > (((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1)) {
		uint32_t address;
	
		switch (check_output(false, &address)) {
			case E64::NOTHING:
				add_bottom_row();
				break;
			case E64::ASCII:
				add_bottom_row();
				E64::monitor_command_memory_dump((address+8) & (RAM_SIZE - 1), 1);
				break;
			case E64::CHARACTER:
				add_bottom_row();
				E64::monitor_command_memory_character_dump((address+16) & (RAM_SIZE - 1), 1);
				break;
			case E64::BINARY:
				add_bottom_row();
				E64::monitor_command_memory_binary_dump((address + 1) & (RAM_SIZE - 1), 1);
				break;
			case E64::DISK:
				add_bottom_row();
				E64::monitor_command_fd_dump(address + 0x08, 1);
				break;
		}
	}
	cursor_activate();
}

void E64::tty_t::add_bottom_row()
{
	cursor_pos -= VICV_CHAR_COLUMNS;
	// move all text one line up
	for (int i=0; i<((VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8))-VICV_CHAR_COLUMNS); i++) {
		character_buffer[i] = character_buffer[i+VICV_CHAR_COLUMNS];
		foreground_color_buffer[i] = foreground_color_buffer[i+VICV_CHAR_COLUMNS];
		background_color_buffer[i] = background_color_buffer[i+VICV_CHAR_COLUMNS];
	}
	uint16_t start_pos = cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS);
	for (int i=0; i<VICV_CHAR_COLUMNS; i++) {
		character_buffer[start_pos] = ASCII_SPACE;
		foreground_color_buffer[start_pos] = current_foreground_color;
		background_color_buffer[start_pos] = current_background_color;
		start_pos++;
	}
}

enum E64::output_type E64::tty_t::check_output(bool top_down, uint32_t *address)
{
	enum output_type output = NOTHING;
    
	uint16_t start_pos = status_bar_active ? (status_bar_rows * VICV_CHAR_COLUMNS) : 0;
    
	for (int i = start_pos; i < (VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8)); i += VICV_CHAR_COLUMNS) {
		if (character_buffer[i] == ':') {
			output = ASCII;

			char potential_address[7];
			for (int j=0; j<6; j++) {
				potential_address[j] =
				character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down) break;
		} else if (character_buffer[i] == ';') {
			output = CHARACTER;
			char potential_address[7];
			for (int j=0; j<6; j++) {
				potential_address[j] =
				character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down) break;
		} else if (character_buffer[i] == '\'') {
			output = BINARY;
			char potential_address[7];
			for (int j=0; j<6; j++) {
				potential_address[j] =
				character_buffer[i+1+j];
			}
			potential_address[6] = 0;
			E64::monitor_command_hex_string_to_int(potential_address, address);
			if (top_down) break;
		} else if (character_buffer[i] == '"') {
			output = DISK;
			char potential_sector[9];
			uint32_t sector;
			for (int j=0; j<8; j++) {
				potential_sector[j] =
					character_buffer[i+3+j];
			}
			potential_sector[8] = 0;
			E64::monitor_command_hex_string_to_int(potential_sector, &sector);
			char potential_offset[5];
			uint32_t offset;
			for (int j=0; j<4; j++) {
				potential_offset[j] =
					character_buffer[i+12+j];
			}
			potential_offset[4] = 0;
			E64::monitor_command_hex_string_to_int(potential_offset, &offset);
			*address = (sector * machine.fd0->bytes_per_sector()) + offset;
			if (top_down) break;
		}
	}
	return output;
}

void E64::tty_t::add_top_row()
{
	cursor_pos += VICV_CHAR_COLUMNS;
	for (int i=((VICV_CHAR_ROWS-8)*VICV_CHAR_COLUMNS)-1; i >= (cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS)) + VICV_CHAR_COLUMNS; i--) {
		character_buffer[i] = character_buffer[i-VICV_CHAR_COLUMNS];
		foreground_color_buffer[i] = foreground_color_buffer[i-VICV_CHAR_COLUMNS];
		background_color_buffer[i] = background_color_buffer[i-VICV_CHAR_COLUMNS];
	}
	uint16_t start_pos = cursor_pos - (cursor_pos % VICV_CHAR_COLUMNS);
	for (int i=0; i<VICV_CHAR_COLUMNS; i++) {
		character_buffer[start_pos] = ASCII_SPACE;
		foreground_color_buffer[start_pos] = current_foreground_color;
		background_color_buffer[start_pos] = current_background_color;
		start_pos++;
	}
}

void E64::tty_t::clear()
{
	cursor_pos = 0;
	for (int i = 0; i < (VICV_CHAR_COLUMNS*(VICV_CHAR_ROWS-8)); i++) {
		character_buffer[i] = ASCII_SPACE;
		background_color_buffer[i] = current_background_color;
		foreground_color_buffer[i] = current_foreground_color;
	}
	if (status_bar_active == true) {
		if (cursor_pos <
		    (status_bar_rows * VICV_CHAR_COLUMNS)) {
			cursor_pos =
			status_bar_rows * VICV_CHAR_COLUMNS;
		}
	}
}

void E64::tty_t::toggle_status_bar()
{
	if (status_bar_active == true) {
		status_bar_active = false;
	} else {
		cursor_deactivate();
		status_bar_active = true;
		if (cursor_pos <
		    (status_bar_rows * VICV_CHAR_COLUMNS)) {
			cursor_pos =
			status_bar_rows * VICV_CHAR_COLUMNS;
		}
		cursor_activate();
	}
}
