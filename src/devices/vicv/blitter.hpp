//  blitter.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#ifndef BLITTER_HPP
#define BLITTER_HPP

#include <cstdint>

namespace E64
{

struct surface
{
    /* bit 0    : Character mode (0) or bit map mode (1)
     * bit 1    : Single color (0), or multi color mode (1)
     * bit 2-7  : Reserved
     * bit 8    : Horizontal single pixel (0) or double pixel (1) size
     * bit 9    : Vertical single pixel (0) or double pixel (1) size
     * bit 10-15: Reserved
     */
    uint16_t    flags;
    
    /* 16 bit big endian number
     * with the x position of the
     * surface
     */
    uint8_t     x_pos_high_byte;
    uint8_t     x_pos_low__byte;
    
    /* 16 bit big endian number
     * with the y position of the
     * surface
     */
    uint8_t     y_pos_high_byte;
    uint8_t     y_pos_low__byte;
};

class blitter
{
public:
    void run(int no_of_cycles);
};

}

#endif // BLITTER_HPP
