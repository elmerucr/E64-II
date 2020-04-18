//  blitter.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

/*
 *  Blitter is able to copy data very fast between memory locations, at a
 *  clock speed of 8 times CPU. Copy operations run independent and can
 *  be added to a FIFO linked list through an I/O memory mapped interface.
 *
 *  Registers
 *  Reg 0x00 read: bit 0: On 1 blitter is busy, on 0 blitter is idle.
 *
 *  Reg 0x02 read/write: data bits 24 - 31
 *  Reg 0x03 read/write: data bits 16 - 23
 *  Reg 0x04 read/write: data bits 8 - 15
 *  Reg 0x05 read/write: data bits 0 - 7
 *
 */

#ifndef BLITTER_HPP
#define BLITTER_HPP

#include <cstdint>

namespace E64
{

enum blitter_state
{
    IDLE,
    CLEARING_FRAMEBUFFER,
    BLITTING
};

enum operation_type
{
    CLEAR_FRAMEBUFFER,
    BLIT
};

struct surface_blit
{
    /*  bit 0    : Character mode (0) or bit map mode (1)
     *  bit 1    : Single color (0), or multi color mode (1)
     *  bit 2-7  : Reserved
     */
    uint8_t     flags_0;
    
    /*  bit 0    : Horizontal single pixel (0) or double pixel (1) size
     *  bit 1    : Vertical single pixel (0) or double pixel (1) size
     *  bit 2-7  : Reserved
     */
    uint8_t     flags_1;
    
    /*  16 bit big endian number
     *  with the x position of the
     *  surface
     */
    uint8_t     x_pos_high_byte;
    uint8_t     x_pos_low_byte;
    
    /*  16 bit big endian number
     *  with the y position of the
     *  surface
     */
    uint8_t     y_pos_high_byte;
    uint8_t     y_pos_low_byte;
};

struct operation
{
    enum operation_type type;
    
    /*
     *  In the case of a CLEAR_FRAMEBUFFER, this data_element contains
     *  the color in gbar4444 format (lower 16 bits). In the case of a
     *  BLIT operation, this data_element contains the address of the
     *  surface_blit descriptor (see other structure).
     *
     */
    uint32_t data_element;
    
    struct surface_blit this_blit;
};

class blitter
{
private:
    uint8_t registers[256];
    
    enum blitter_state current_state;
    
    // circular buffer containing operations
    // if more than 256 operation would be written (unlikely) and not
    // finished, something will be overwritten
    struct operation operations[256];
    uint8_t head;
    uint8_t tail;
    
    // finite state machine clearing framebuffer
    uint16_t clear_color;
    uint32_t clear_counter;
    
    
    // finite state machine blitting
    
public:
    void reset();
    void run(int no_of_cycles);
    
    void add_operation(enum operation_type type, uint32_t data_element);
    
    // Register access
    uint8_t read_byte(uint8_t address);
    void write_byte(uint8_t address, uint8_t byte);
};

}

#endif // BLITTER_HPP
