/*
 * vicv.cpp
 * E64-II
 *
 * Copyright © 2017-2021 elmerucr. All rights reserved.
 */
 
#include <cstdio>
#include "vicv.hpp"
#include "common.hpp"

uint16_t disk_icon[128] = {
	// empty
	COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_01,COBALT_02,
	COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,

	// disk inside
	COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_04,
	COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04,COBALT_04
};

E64::vicv_ic::vicv_ic()
{
	stats_overlay_present = false;

	framebuffer0 = (uint16_t *)&pc.mmu->ram[VICV_FRAMEBUFFER0];
	framebuffer1 = (uint16_t *)&pc.mmu->ram[VICV_FRAMEBUFFER1];

	breakpoint_reached = false;
	clear_scanline_breakpoints();
	old_y_pos = 0;

	stats_text = nullptr;
}

void E64::vicv_ic::reset()
{
	pc.TTL74LS148->release_line(vblank_interrupt_device_number);

	frame_done = false;

	cycle_clock = dot_clock = 0;

	for (int i=0; i<256; i++)
		registers[i] = 0;

	frontbuffer = framebuffer0;
	backbuffer  = framebuffer1;
}

void E64::vicv_ic::run(uint32_t number_of_cycles)
{
	/*
	 * y_pos needs initialization otherwise compiler complains.
	 * Chosen for bogus 0xff value => probably in initialized data
	 * section. Seems to be fastest when looking at cpu usage.
	 */
	uint32_t y_pos = 0xff;
	
	uint32_t x_pos;

	while (number_of_cycles > 0) {
		y_pos = cycle_clock /
			(VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK);
		x_pos = cycle_clock -
			(y_pos * (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK));
		bool hblank = (x_pos >= VICV_PIXELS_PER_SCANLINE);
		bool vblank = cycle_clock>=((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES);
		bool blank = hblank || vblank;
		bool hborder = (y_pos < registers[VICV_REG_BORDER_SIZE]) ||
			(y_pos > ((VICV_SCANLINES-1)-registers[VICV_REG_BORDER_SIZE]));

		if (!blank) {
			if (hborder)
				host_video.backbuffer[dot_clock] =
					host_video.palette[*((uint16_t *)(&registers[VICV_REG_HOR_BOR_HIGH]))];
			else
				host_video.backbuffer[dot_clock] =
					host_video.palette[frontbuffer[dot_clock]];
			dot_clock++;	// progr dot clock if pixel was sent (!blank)
		}

		cycle_clock++;
        
		switch (cycle_clock) {
		case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES:
			// start of vblank
			pc.TTL74LS148->pull_line(vblank_interrupt_device_number);
			break;
		case (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK):
			// finished vblank, do other necessary stuff
				if (stats_overlay_present) {
					render_stats(72, 276);
					render_disk_activity(4, 276);
				}
			host_video.swap_buffers();
			cycle_clock = dot_clock = 0;
			frame_done = true;
			break;
		}
	number_of_cycles--;
	}

	if ((y_pos != old_y_pos) && scanline_breakpoints[y_pos] == true)
		breakpoint_reached = true;
	old_y_pos = y_pos;
}

#define Y_POS           (cycle_clock / (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK))
#define X_POS           (cycle_clock - (Y_POS * (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)))
#define HBLANK          (X_POS >= VICV_PIXELS_PER_SCANLINE)
#define VBLANK          (cycle_clock>=((VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*VICV_SCANLINES))

bool E64::vicv_ic::is_hblank() { return HBLANK; }

bool E64::vicv_ic::is_vblank() { return VBLANK; }

inline void E64::vicv_ic::render_stats(uint16_t xpos, uint16_t ypos)
{
	uint32_t base = ((ypos * VICV_PIXELS_PER_SCANLINE) + xpos) %
		(VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
	uint8_t  eight_pixels = 0;

	for (int y=0; y<8; y++) {
		char *temp_text = stats_text;
		uint16_t x = 0;
		// are we still pointing at a character
		while (*temp_text) {
			// are we at the first pixel of a char
			if (!(x & 7)) {
				eight_pixels =
					rom[CBM_CP437_FONT_ADDRESS + ((*temp_text * 8) + y)];
			}

			host_video.backbuffer[base + x] = (eight_pixels & 0x80) ?
				host_video.palette[COBALT_06] :
				host_video.palette[COBALT_02];

			eight_pixels = eight_pixels << 1;
			x++;
			// increase the text pointer only when necessary
			if (!(x & 7))
				temp_text++;
		}
		// go to the next line
		base = (base + VICV_PIXELS_PER_SCANLINE) %
			(VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
	}
}


inline void E64::vicv_ic::render_disk_activity(uint16_t xpos, uint16_t ypos)
{
	uint32_t base = ((ypos * VICV_PIXELS_PER_SCANLINE) + xpos) %
		(VICV_PIXELS_PER_SCANLINE * VICV_SCANLINES);
	
	uint16_t icon_offset = 0;
	if (pc.fd0->read_byte(0x00) & 0x01)
		icon_offset = 64;
	
	for (int x=0; x<8; x++) {
		for (int y=0; y<8; y++) {
			host_video.backbuffer[base + (VICV_PIXELS_PER_SCANLINE*y) + x] =
				host_video.palette[disk_icon[(y*8)+x+icon_offset]];
		}
	}
}

uint16_t E64::vicv_ic::get_current_scanline() { return Y_POS; }

uint16_t E64::vicv_ic::get_current_pixel() { return X_POS; }

void E64::vicv_ic::clear_scanline_breakpoints()
{
	for (int i=0; i<1024; i++)
		scanline_breakpoints[i] = false;
}

void E64::vicv_ic::add_scanline_breakpoint(uint16_t scanline)
{
	scanline_breakpoints[scanline & 1023] = true;
}

void E64::vicv_ic::remove_scanline_breakpoint(uint16_t scanline)
{
	scanline_breakpoints[scanline & 1023] = false;
}

bool E64::vicv_ic::is_scanline_breakpoint(uint16_t scanline)
{
	return scanline_breakpoints[scanline & 1023];
}

uint8_t E64::vicv_ic::read_byte(uint8_t address)
{
	return registers[address & 0x07];
}

void E64::vicv_ic::write_byte(uint8_t address, uint8_t byte)
{
	switch (address) {
	case VICV_REG_ISR:
		if (byte & 0b00000001)
			pc.TTL74LS148->release_line(vblank_interrupt_device_number);  // acknowledge pending irq
		break;
	case VICV_REG_BUFFERSWAP:
		if (byte & 0b00000001) {
			if (pc.blitter->current_state != IDLE) {
				pc.blitter->current_state = IDLE;
				printf("[VICV] warning: blitter was not finished when swapping buffers\n");
			}
			uint16_t *tempbuffer = frontbuffer;
			frontbuffer = backbuffer;
			backbuffer = tempbuffer;
		}
		break;
	default:
		registers[address & 0x07] = byte;
		break;
	}
}
