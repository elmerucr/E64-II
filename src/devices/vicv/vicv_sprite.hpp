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
    uint16_t width;              // only bit 0-2 are relevant
    uint16_t height;
    int16_t x_position;
    int16_t y_position;
public:
    sprite();
    
    /*
     * This function takes the values of the current dot being drawn.
     * The sprite logic (knowing its own coordinates) will decide if
     * and what color pixel to draw.
     */
    bool render_pixel(int16_t x, int16_t y);
};

}

#endif // VICV_SPRITE_HPP
