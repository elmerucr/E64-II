//  vicv_sprite.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#ifndef VICV_SPRITE_HPP
#define VICV_SPRITE_HPP

#include <cstdint>

namespace E64 {

enum sprite_mode {
    CHARACTER_BASED,
    LINE_BASED
};

class sprite {
private:
    enum sprite_mode mode;
    uint16_t width_mask;
    uint16_t height_mask;
    int16_t x_position;
    int16_t y_position;
public:
    sprite();
    
    /*
     * Width and height are set in an exponential way.
     * 2^width or 2^height will be the result (in no. of chars).
     * The argument will be truncated to 3 bits before any calculation.
     * Minimum size = 2^0 = 1 char (8 pix) OR 2^7 = 128 chars (1024 pix)
     */
    void set_width(uint8_t width);
    void set_height(uint8_t height);
    
    /*
     * This function takes the values of the current dot being drawn.
     * The sprite logic (knowing its own coordinates) will decide if
     * and what color pixel to draw.
     */
    bool render_pixel(int16_t x, int16_t y);
};

}

#endif // VICV_SPRITE_HPP
