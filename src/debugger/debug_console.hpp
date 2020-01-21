//  debug_console.hpp
//  E64
//
//  Copyright © 2018 elmerucr. All rights reserved.

#include <cstdint>

#ifndef debug_console_hpp
#define debug_console_hpp

typedef struct
{
    uint8_t console_character_buffer[2048];
    uint8_t console_foreground_color_buffer[2048];
    uint8_t console_background_color_buffer[2048];

    // cursor related vars
    int16_t    cursor_pos;
    bool        cursor_pos_original_reverse;
    uint8_t     cursor_blink_time;
    uint8_t     cursor_count_down;

    uint8_t  current_foreground_color;
    uint8_t  current_background_color;

    // status bar related things
    bool        status_bar_active;
    uint8_t     status_bar_rows;
    uint16_t    status_bar_total_chars;
    uint16_t    status_bar_cursor_pos;
    uint16_t    status_bar_base_pos;
    uint8_t     status_bar_foreground_color;
    uint8_t     status_bar_background_color;
    //
} debug_console_struct;

extern debug_console_struct debug_console;

void debug_console_init();
void debug_console_version();
void debug_console_welcome();
void debug_console_blit_to_debug_screen();
void debug_console_putchar(char character);
void debug_console_print(const char *string_to_print);

void debug_console_cursor_activate();
void debug_console_cursor_deactivate();
bool debug_console_cursor_flash();         // returns true if state of cursor changed

void debug_console_enter();
void debug_console_insert();
void debug_console_backspace();
void debug_console_arrow_left();
void debug_console_arrow_right();
void debug_console_arrow_up();
void debug_console_arrow_down();
void debug_console_add_bottom_row_if_necessary();
void debug_console_add_top_row();
void debug_console_clear();

void debug_console_toggle_status_bar();

#endif
