//  vicv_sprite.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "vicv_sprite.hpp"

E64::sprite::sprite()
{
    width = 0b1111111;
    height = 0b111111;
    x_position = 350;
    y_position = 64;
}

#define X_RELATIVE  (x - x_position)
#define Y_RELATIVE  (y - y_position)
#define X_IN_SPRITE (((0xffff - width) & X_RELATIVE) == 0)
#define Y_IN_SPRITE (((0xffff - height) & Y_RELATIVE) == 0)

bool E64::sprite::render_pixel(int16_t x, int16_t y)
{
    if( X_IN_SPRITE && Y_IN_SPRITE ) return true;
    return false;
}
