//  screen.hpp
//  E64-II
//
//  Copyright © 2017-2021 elmerucr. All rights reserved.
//
//  covers 32 rows (256 lines)
//  the bottom 64 lines are used to display part of the current framebuffer

#include <cstdint>

#ifndef SCREEN_HPP
#define SCREEN_HPP

namespace E64 {

class screen_t {
public:
	screen_t();
	~screen_t();
	
	uint8_t *character_buffer;
	uint16_t *foreground_color_buffer;
	uint16_t *background_color_buffer;
	
	void update();
	void render_scanline(int line_number);
};

}

#endif
