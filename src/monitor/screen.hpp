//  screen.hpp
//  E64-II
//
//  Copyright © 2017-2021 elmerucr. All rights reserved.
//
//  c256_debug_screen covers 32 rows (256 lines)
//  the bottom 64 lines are used to display part of the current framebuffer

#include "common.hpp"

#ifndef SCREEN_HPP
#define SCREEN_HPP

namespace E64
{

void screen_init();
void screen_update();
void screen_render_scanline(int line_number);

}

extern uint8_t screen_character_buffer[];
extern uint16_t screen_foreground_color_buffer[];
extern uint16_t screen_background_color_buffer[];

extern uint16_t screen_pixel_cursor_blink_time_countdown;

#endif
