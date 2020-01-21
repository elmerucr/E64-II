//  debug_screen.hpp
//  E64
//
//  Copyright © 2017 elmerucr. All rights reserved.
//
//  c256_debug_screen covers 32 rows (256 lines)
//  the bottom 64 lines are used to display part of the current framebuffer

#include "common_defs.hpp"

#ifndef debug_screen_hpp
#define debug_screen_hpp

namespace E64
{
    void debug_screen_update(void);
    void debug_screen_render_scanline(int line_number);
}

extern uint32_t debug_screen_buffer[];

extern uint8_t debug_screen_character_buffer[];
extern uint8_t debug_screen_foreground_color_buffer[];
extern uint8_t debug_screen_background_color_buffer[];

#endif
