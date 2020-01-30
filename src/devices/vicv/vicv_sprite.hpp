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
    uint8_t width;              // only bit 0-2 are relevant (0-7
    uint8_t height;
    int16_t x_position;
    int16_t y_position;
public:
    uint32_t render_pixel();
};

}

#endif // VICV_SPRITE_HPP
