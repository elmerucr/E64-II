//  monitor_screen.hpp
//  E64-II
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.
//
//  c256_debug_screen covers 32 rows (256 lines)
//  the bottom 64 lines are used to display part of the current framebuffer

#include "common.hpp"

#ifndef monitor_screen_hpp
#define monitor_screen_hpp

namespace E64
{

void monitor_screen_init();
void monitor_screen_update();
void monitor_screen_render_scanline(int line_number);

}

extern uint8_t monitor_screen_character_buffer[];
extern uint16_t monitor_screen_foreground_color_buffer[];
extern uint16_t monitor_screen_background_color_buffer[];

extern uint16_t monitor_screen_pixel_cursor_blink_time_countdown;

#endif
