//  blitter.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

/*
 *  Blitter is able to copy data very fast from a memory location to the
 *  backbuffer (framebuffer), at a clock speed of eight times CPU. Copy
 *  operations run independently and can be added to a FIFO linked list
 *  through an I/O memory mapped interface.
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

/*  The next structure is a description of the surface blit how it appears
 *  in the memory of E64-II. Once a pointer to this structure is passed to
 *  the blitter (addition of an operation), the structure is read and converted
 *  into finite state machine data of the blitter.
 *
 *  Size of this structure:
 *  -
 */

struct surface_blit
{
    /*  The size of this structure is max 32 bytes. Inside the machine,
     *  alignment at 2^5 = 32 bytes must be arranged.
     */
    
    /*  bit 0    : Character mode (0) or bitmap mode (1)
     *  bit 1    : Single color (0), or multi color mode (1)
     *  bit 2-7  : Reserved
     */
    uint8_t     flags_0;
    
    /*  bit 0    : Horizontal single pixel (0) or double pixel (1) size
     *  bit 1    : Vertical single pixel (0) or double pixel (1) size
     *  bit 2    : Horizontal flip on (1)
     *  bit 3    : Vertical flip on (1)
     *  bit 4-7  : Reserved
     */
    uint8_t     flags_1;
    
    /*  Width and height of blit, 8 bit unsigned.
     *
     *  The 5 most significant bits are unused. Then, the 3 least significant
     *  bits indicate a number of 0 - 7 (n). Finally, a bit shift occurs:
     *      0b00000001 << n
     *  Resulting in the final width/height in 'chars' (8 pixels / char)
     *  { 1, 2, 4, 8, 16, 32, 64, 128 }
     */
    uint8_t     width;
    uint8_t     height;
    
    /*  16 bit signed big endian number
     *  with the x position of the
     *  surface
     */
    uint8_t     x_high_byte;
    uint8_t     x_low_byte;
    
    /*  16 bit signed big endian number
     *  with the y position of the
     *  surface
     */
    uint8_t     y_high_byte;
    uint8_t     y_low_byte;
    
    /*  32 bit pointer to pixels (can be character pixels or bitmap pixels */
    uint8_t     pixel_data_24_31;
    uint8_t     pixel_data_16_23;
    uint8_t     pixel_data__8_15;
    uint8_t     pixel_data__0__7;
    
    /*  32 bit pointer to start of characters */
    uint8_t     character_data_24_31;
    uint8_t     character_data_16_23;
    uint8_t     character_data__8_15;
    uint8_t     character_data__0__7;
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
    
    /*
     *  Registers
     *  Reg 0x00 read: bit 0: On 1 blitter is busy, on 0 blitter is idle.
     *
     *  Reg 0x02 read/write: data bits 24 - 31
     *  Reg 0x03 read/write: data bits 16 - 23
     *  Reg 0x04 read/write: data bits 8 - 15
     *  Reg 0x05 read/write: data bits 0 - 7
     *
     */
    
private:
    uint8_t registers[256];
public:
    uint8_t read_byte(uint8_t address);
    void    write_byte(uint8_t address, uint8_t byte);

private:
    // circular buffer containing operations
    // if more than 256 operation would be written (unlikely) and not
    // finished, something will be overwritten
    struct operation operations[256];
    uint8_t head;
    uint8_t tail;
    void add_operation(enum operation_type type, uint32_t data_element);
    
    
    // finite state machine general
    uint16_t width_log2;
    uint16_t height_log2;
    uint16_t width;
    uint16_t height;
    uint32_t counter;
    uint32_t normalized_counter;
    uint32_t max_count;
    
    // finite state clearing framebuffer
    uint16_t clear_color;
    
    // finite state blitting
    bool bitmap_mode;
    bool multicolor_mode;
    
    uint16_t scr_x;             // final screen x
    uint16_t scr_y;             // final screen y
    
    uint16_t char_width_log2;        // width of blit measured in chars
    uint16_t char_height_log2;       // height of blit measured in chars
    
    uint16_t char_number;
    uint8_t current_char;
    uint8_t pixel_in_char;
    
    uint16_t source_color;
    
    uint16_t width_mask;
    
    int16_t x;
    int16_t y;
    
    uint16_t is_double_width;
    uint16_t is_double_height;
    
    bool horizontal_flip;
    bool vertical_flip;
    
    uint32_t pixel_data;
    uint32_t character_data;
    
public:
    
    enum blitter_state current_state;
    
    void reset();
    void run(int no_of_cycles);
};

}

#endif // BLITTER_HPP
