//  vicv_sprite.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#ifndef VICV_SPRITE_HPP
#define VICV_SPRITE_HPP

#include <cstdint>

/*
 * Register 0 (1 byte) of a sprite descriptor contains some flags. Each
 * bit has a different usage
 *
 * bit 0 - Inactive and not visible (0) or active and visible (1)
 * bit 1 - Character mode (0) or bit map mode (1)
 * bit 2 - Single color (0), or multi color mode (1)
 * bit 3 - Reserved
 * bit 4 - Horizontal single pixel (0) or double pixel (1) size
 * bit 5 - Vertical single pixel (0) or double pixel (1) size
 * bit 6 - Reserved
 * bit 7 - Back set of sprites (0) or front set (1)
 *
 */
#define SPRITE_REG_FLAGS    0x00

/*
 * Register 1 (1 byte) is reserved
 */
#define SPRITE_REG_ONE      0x01

/*
 * Register 2 (1 byte) is the horizontal size. The lowest 3 bits matter.
 * A value of 0 results in a width of 2^0=1 char (8 pixels). Value 1 means
 * 2^1=2 chars (8 pixels). The maximum is 2^7=128 chars (1024 pixels).
 */
#define SPRITE_REG_WIDTH    0x02

/*
 * Register 3 (1 byte) is the vertical size. Same explanation as register 2.
 */
#define SPRITE_REG_HEIGHT   0x03

/*
 * Registers 4 and 5 combined (a 16 bit signed word, big endian) form the
 * horizontal position of the sprite
 */
#define SPRITE_REG_HOR_POS  0x04

/*
 * Registers 6 and 7 combined (a 16 bit signed word, big endian) form the
 * vertical position of the sprite
 */
#define SPRITE_REG_VER_POS  0x06

/*
 * Registers 8 to 11 combined (32 bit pointer) form the address
 */
#define SPRITE_REG_


namespace E64 {

class sprite {
private:
    uint8_t registers[16];
    uint16_t width_mask;
    uint16_t height_mask;
    int16_t x_position;
    int16_t y_position;
public:
    sprite();
    
    uint8_t read(uint8_t address);
    
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
