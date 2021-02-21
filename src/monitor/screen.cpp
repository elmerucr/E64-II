//  screen.cpp
//  E64-II
//
//  Copyright © 2017-2021 elmerucr. All rights reserved.

#include "screen.hpp"
#include "common.hpp"

uint8_t screen_character_buffer[VICV_CHAR_ROWS*VICV_CHAR_COLUMNS];
uint16_t screen_foreground_color_buffer[VICV_CHAR_ROWS*VICV_CHAR_COLUMNS];
uint16_t screen_background_color_buffer[VICV_CHAR_ROWS*VICV_CHAR_COLUMNS];

uint16_t screen_pixel_cursor_blink_time;
uint16_t screen_pixel_cursor_blink_time_countdown;

void E64::screen_init()
{
	screen_pixel_cursor_blink_time = 40;
	screen_pixel_cursor_blink_time_countdown =
		screen_pixel_cursor_blink_time;
}

void E64::screen_update()
{
	// update all scanlines
	for (int i=0; i < VICV_SCANLINES - 64; i++)
		screen_render_scanline(i);

	// copy relevant area of vicv screen buffer to bottom of debug screen buffer
	uint16_t scanline_normalized;
	if (machine.vicv->get_current_scanline() >= (VICV_SCANLINES - 32)) {
		scanline_normalized = VICV_SCANLINES - 64;
	} else if(machine.vicv->get_current_scanline() >= 32) {
		scanline_normalized = machine.vicv->get_current_scanline() - 32;
	} else {
		scanline_normalized = 0;
	}

	/*
	 * Copy relevant part of the framebuffer onto the debug screen
	 */
	uint32_t base = VICV_PIXELS_PER_SCANLINE * scanline_normalized;

	for (int i=0; i<(VICV_PIXELS_PER_SCANLINE*64); i++) {
		/*
		 * Backbuffer is the one currently being drawn into, so that
		 * one should be shown.
		 */
		host.video->monitor_framebuffer[((VICV_SCANLINES-64)*VICV_PIXELS_PER_SCANLINE) + i] =
			host.video->framebuffer[base + i];
	}

	uint16_t current_pixel = machine.vicv->get_current_pixel();
	uint16_t current_scanline = machine.vicv->get_current_scanline();
	uint32_t pixel_cursor_color = 0xff00ff00;
	if (current_pixel >= VICV_PIXELS_PER_SCANLINE) {
		current_pixel = VICV_PIXELS_PER_SCANLINE - 1;
		pixel_cursor_color = 0xffff0000;
	}
	if(current_scanline > (VICV_SCANLINES - 1)) {
		current_scanline = VICV_SCANLINES - 1;
		pixel_cursor_color = 0xffff0000;
	}

	host.video->monitor_framebuffer[((VICV_SCANLINES - 64)*VICV_PIXELS_PER_SCANLINE) + ((current_scanline - scanline_normalized)*VICV_PIXELS_PER_SCANLINE) + current_pixel ] = pixel_cursor_color;
}

inline void E64::screen_render_scanline(int line_number)
{
	int base;

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

	for (int x=0; x < VICV_PIXELS_PER_SCANLINE; x++) {
		// if it's the first pixel of a char position, retrieve byte from char_rom
		if (!(x & 7)) {
			int current_text_column = (x >> 3);
			uint16_t char_position = (((current_text_row * VICV_CHAR_COLUMNS) + current_text_column));
			current_char = screen_character_buffer[char_position];
			current_foreground_color = screen_foreground_color_buffer[char_position];
			current_background_color = screen_background_color_buffer[char_position];
			eight_pixels = rom[CBM_CP437_FONT_ADDRESS + ((current_char<<3) | current_character_line)];
		}
		host.video->monitor_framebuffer[base+x] = (eight_pixels & 0x80) ? host.video->palette[current_foreground_color] : host.video->palette[current_background_color];

		// shift all bits in internal byte 1 place to the left
		eight_pixels = eight_pixels << 1;
	}
}
