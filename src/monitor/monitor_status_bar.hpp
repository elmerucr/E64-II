//  monitor_status_bar.hpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#include <cstdint>

#ifndef MONITOR_STATUS_BAR_HPP
#define MONITOR_STATUS_BAR_HPP

extern uint8_t status_bar_chars[];
extern uint16_t status_bar_foreground_color_buffer[];
extern uint16_t status_bar_background_color_buffer[];

void debug_status_bar_clear();
void debug_status_bar_refresh();

void debug_status_bar_set_cursor_pos(uint16_t pos);
void debug_status_bar_putchar(char character);
void debug_status_bar_print(const char *string_to_print);

#endif
