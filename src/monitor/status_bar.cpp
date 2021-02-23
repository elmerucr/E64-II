//  status_bar.cpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#include <cstdio>

#include "common.hpp"
#include "status_bar.hpp"

char help_string[2048];
char help_string_2[2048];

E64::status_bar_t::status_bar_t()
{
	chars = new uint8_t[16*VICV_CHAR_COLUMNS];
	foreground_color_buffer = new uint16_t[16*VICV_CHAR_COLUMNS];
	background_color_buffer = new uint16_t[16*VICV_CHAR_COLUMNS];
}

E64::status_bar_t::~status_bar_t()
{
	delete chars;
	delete foreground_color_buffer;
	delete background_color_buffer;
}

void E64::status_bar_t::clear()
{
	for (int i=0; i < (monitor.tty->status_bar_rows * VICV_CHAR_COLUMNS); i++) {
		chars[i] = ASCII_SPACE;
		foreground_color_buffer[i] = COBALT_06;
		background_color_buffer[i] = COBALT_02;
	}
	monitor.tty->status_bar_cursor_pos = 0;
	monitor.tty->status_bar_foreground_color = COBALT_06;  // default value
	monitor.tty->status_bar_background_color = COBALT_02;  // default value
}

void E64::status_bar_t::set_cursor_pos(uint16_t pos)
{
	// confine cursor
	monitor.tty->status_bar_cursor_pos = pos % monitor.tty->status_bar_total_chars;
	monitor.tty->status_bar_base_pos = monitor.tty->status_bar_cursor_pos % VICV_CHAR_COLUMNS;
}

void E64::status_bar_t::putchar(char c)
{
	c = c & 0x7f;
	switch (c) {
	case ASCII_LF:
			monitor.tty->status_bar_cursor_pos += VICV_CHAR_COLUMNS;
			monitor.tty->status_bar_cursor_pos -= (monitor.tty->status_bar_cursor_pos % VICV_CHAR_COLUMNS);
			monitor.tty->status_bar_cursor_pos += monitor.tty->status_bar_base_pos;
	    break;
	default:
	    chars[monitor.tty->status_bar_cursor_pos] = c;
	    foreground_color_buffer[monitor.tty->status_bar_cursor_pos] = monitor.tty->status_bar_foreground_color;
	    background_color_buffer[monitor.tty->status_bar_cursor_pos] = monitor.tty->status_bar_background_color;
			monitor.tty->status_bar_cursor_pos++;
			monitor.tty->status_bar_cursor_pos %= monitor.tty->status_bar_total_chars;
	    break;
    }
}

void E64::status_bar_t::print(const char *string_to_print)
{
	char *temp_char = (char *)string_to_print;
	while (*temp_char != ASCII_NULL) {
		putchar(*temp_char);
		temp_char++;
	}
}

void E64::status_bar_t::refresh()
{
	clear();

	// registers
	machine.m68k->dump_registers(help_string);
	set_cursor_pos(1*VICV_CHAR_COLUMNS + 0);
	print(help_string);
    
	// disassembly
	set_cursor_pos(10*VICV_CHAR_COLUMNS + 0);
	uint32_t temp_pc = machine.m68k->getPC();
	for (int i=0; i<(monitor.tty->status_bar_rows - 10); i++) {
		if (machine.m68k->debugger.breakpoints.isSetAt(temp_pc)) {
			monitor.tty->status_bar_foreground_color = AMBER_06; // bright amber
		}
		snprintf(help_string, 256, "%06x ", temp_pc );
		print(help_string);
		int no_of_bytes = machine.m68k->disassemble(temp_pc, help_string);
	
		if (monitor.tty->status_bar_hex_view == true) {
			for (int i = 0; i< (no_of_bytes/2); i++) {
				snprintf(help_string_2, 256, "%04x ", machine.mmu->read_memory_16(temp_pc + (2*i) ) );
				print(help_string_2);
			}
		} else {
			print(help_string);
		}
	
		putchar('\n');
		monitor.tty->status_bar_foreground_color = COBALT_06;  // revert to normal color
		temp_pc += no_of_bytes;
	}
    
	// vicv
	snprintf(help_string, 256, "    pixel: %3u\n scanline: %3u\n   hblank: %3u\n   vblank: %3u", machine.vicv->get_current_pixel(), machine.vicv->get_current_scanline(), machine.vicv->is_hblank() ? 1 : 0, machine.vicv->is_vblank() ? 1 : 0 );
	set_cursor_pos(1*VICV_CHAR_COLUMNS + 44);
	print(help_string);

	// set accent colors for titles etc...
	monitor.tty->status_bar_foreground_color = COBALT_07;
	monitor.tty->status_bar_background_color = COBALT_04;

	snprintf(help_string, 256, " CPU Status                              ");
	set_cursor_pos(0);
	print(help_string);
	snprintf(help_string, 256, " Disassembly                             ");
	set_cursor_pos(9*VICV_CHAR_COLUMNS + 0);
	print(help_string);
	snprintf(help_string, 256, "     vic v      ");
	set_cursor_pos(0*64 + 44);
	print(help_string);
}
