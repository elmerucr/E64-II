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
    /*  The size of this structure is 32 bytes. Inside the machine,
     *  alignment at 2^5 = 32 bytes must be arranged.
     */
    
    /*  Flags 0
     *
     *  7 6 5 4 3 2 1 0
     *          | | | |
     *          | | | +-- Tile Mode (0) / Bitmap Mode (1)
     *          | | +---- Background (0 = off, 1 = on)
     *          | +------ Simple Color (0) / Multi Color (1)
     *          +-------- Color per tile (0 = off, 1 = on)
     *
     *  bits 4-7: Reserved
     */
    uint8_t     flags_0;

    /*  Flags 1
     *
     *  7 6 5 4 3 2 1 0
     *      | |   |   |
     *      | |   |   +-- Horizontal double pixel size (1 = on)
     *      | |   +------ Vertical double pixel size (1 = on)
     *      | +---------- Horizontal flip (1 = on)
     *      +------------ Vertical flip (1 = on)
     *
     *  bits 1, 3, 6 and 7: Reserved
     */
    uint8_t     flags_1;
    
    /*  Width and height of blit, 8 bit unsigned.
     *
     *  The 5 most significant bits are unused. Then the 3 least significant
     *  bits indicate a number of 0 - 7 (n). Finally, a bit shift occurs:
     *      0b00000001 << n
     *  Resulting in the final width/height in 'tiles' (8 pixels per tile)
     *  { 1, 2, 4, 8, 16, 32, 64, 128 }
     */
    uint8_t     width_in_tiles_log2;
    uint8_t     height_in_tiles_log2;
    
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
    
    /*  16 unsigned little endian number
     *  containing the foreground color
     *  if single color.
     */
    uint8_t     foreground_color__0__7;
    uint8_t     foreground_color__8_15;
    
    /*  16 unsigned little endian number
     *  containing the background color
     *  if single color.
     */
    uint8_t     background_color__0__7;
    uint8_t     background_color__8_15;
    
    /*  32 bit pointer to pixels (can be tile pixels or bitmap pixels */
    uint8_t     pixel_data_24_31;
    uint8_t     pixel_data_16_23;
    uint8_t     pixel_data__8_15;
    uint8_t     pixel_data__0__7;
    
    /*  32 bit pointer to start of tiles */
    uint8_t     tile_data_24_31;
    uint8_t     tile_data_16_23;
    uint8_t     tile_data__8_15;
    uint8_t     tile_data__0__7;
    
    /*  32 bit pointer to start of tile color */
    uint8_t     tile_color_data_24_31;
    uint8_t     tile_color_data_16_23;
    uint8_t     tile_color_data__8_15;
    uint8_t     tile_color_data__0__7;
    
    /*  32 bit pointer to start of tile background color */
    uint8_t     tile_background_color_data_24_31;
    uint8_t     tile_background_color_data_16_23;
    uint8_t     tile_background_color_data__8_15;
    uint8_t     tile_background_color_data__0__7;
    
    /*  user data */
    uint8_t     user_data_24_31;
    uint8_t     user_data_16_23;
    uint8_t     user_data__8_15;
    uint8_t     user_data__0__7;
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
    uint8_t  registers[256];
    
    /*  Keeping track of busy and idle cycles. This way, it is possible
     *  to estimate the % of usage of the blitter chip. It is best to
     *  calculate this only once per frame.
     */
    uint64_t cycles_busy;
    uint64_t cycles_idle;
    
public:
    uint8_t  read_byte(uint8_t address);
    void     write_byte(uint8_t address, uint8_t byte);
    
    /*  Returns the fraction of time the blitter was NOT idle.
     */
    double   fraction_busy();

private:
    /*  Circular buffer containing operations. If more than 256 operations
     *  would be written (unlikely) and not finished, something will be
     *  overwritten.
     */
    struct operation operations[256];
    uint8_t head;
    uint8_t tail;
    void add_operation(enum operation_type type, uint32_t data_element);
    
    
    // finite state machine
    bool        bitmap_mode;
    bool        background;
    bool        multicolor_mode;
    bool        color_per_tile;
    bool        hrz_flip;
    bool        vrt_flip;
    uint16_t    double_width;
    uint16_t    double_height;
    
    uint16_t width_in_tiles_log2;
    uint16_t width_log2;
    uint16_t width_on_screen_log2;
    uint16_t width;
    uint16_t width_on_screen;
    
    uint16_t height_in_tiles_log2;
    uint16_t height_log2;
    uint16_t height_on_screen_log2;
    uint16_t height;
    uint16_t height_on_screen;
    
    uint32_t pixel_no;
    uint32_t normalized_pixel_no;
    uint32_t total_pixel_no;
    
    // specific for clearing framebuffer
    uint16_t clear_color;

    int16_t x;
    int16_t y;
    
    uint16_t scr_x;             // final screen x
    uint16_t scr_y;             // final screen y
    
    uint16_t x_in_blit;
    uint16_t y_in_blit;
    
    uint16_t tile_x;
    uint16_t tile_y;
    
    
    uint16_t    tile_number;
    uint8_t     tile_index;
    uint16_t    current_background_color;
    uint8_t     pixel_in_tile;
    
    uint16_t source_color;
    
    uint16_t width_mask;
    uint16_t width_on_screen_mask;
    
    uint16_t foreground_color;
    uint16_t background_color;
    
    uint32_t pixel_data;
    uint32_t tile_data;
    uint32_t tile_color_data;
    uint32_t tile_background_color_data;
    uint32_t user_data;
    
public:
    
    enum blitter_state current_state;
    
    void reset();
    void run(int no_of_cycles);
};

}

#endif // BLITTER_HPP
