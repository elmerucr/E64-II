//  status_bar.hpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#include <cstdint>

#ifndef STATUS_BAR_HPP
#define STATUS_BAR_HPP

namespace E64 {

class status_bar_t {
public:
	uint8_t *chars;
	uint16_t *foreground_color_buffer;
	uint16_t *background_color_buffer;
	
	status_bar_t();
	~status_bar_t();
	
	void clear();
	void refresh();
	void set_cursor_pos(uint16_t pos);
	void putchar(char c);
	void print(const char *string_to_print);
};

}

#endif
