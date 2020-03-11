//  blitter.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

/* Blitter is able to copy data very fast between memory locations, at a
 * clock speed of 8 times CPU. Copy operations run concurrently and can
 * be added to a FIFO linked list through an I/O memory mapped interface.
 *
 * Reg 00: bit 0: On 1, blitter is busy, on 0, blitter is idle.
 */

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
private:
    uint8_t registers[256];
public:
    void run(int no_of_cycles);
    
    void add_operation();
    
    // Register access
    uint8_t read_byte(uint8_t address);
    void write_byte(uint8_t address, uint8_t byte);
};

}

#endif // BLITTER_HPP
