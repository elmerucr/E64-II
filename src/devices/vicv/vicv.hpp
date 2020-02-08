//  vicv.hpp
//  E64
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.

#include <cstdint>
#include "vicv_surface.hpp"

#ifndef vicv_hpp
#define vicv_hpp

// internal vicv registers (appear at a different base address in main memory)
#define VICV_REG_BOR            0x00    // reg00 - Border color register
#define VICV_REG_BKG            0x01    // reg01 - Background color register
// reg02-05 combined are a 32 bit pointer for the location of the default text screen (2k), big endian
#define VICV_REG_TXT            0x02    // reg02 - Textscreen pointer
// reg06-09 combined are a 32 bit pointer for the location of the default color screen (2k), big endian
#define VICV_REG_COL            0x06    // reg06 - Colorscreen pointer
#define VICV_REG_BORDER_SIZE    0x0a    // a byte telling the size of the horizontal border

namespace E64 {

class vicv
{
private:
    // this chip contains 256 registers (and are mapped to a specific page)
    uint8_t registers[256];
    
    // 16 surfaces
    surface surfaces[16];
    
    /*
     * these are "internal" framebuffers for double buffering
     * we need two buffers as the calls for refreshing the screen will be asynchronous
     * to the outside world only front_buffer and back_buffer are known
     */
    uint32_t *screen_buffer_0;
    uint32_t *screen_buffer_1;

    inline void swap_buffers()
    {
        uint32_t *temp = frontbuffer;
        frontbuffer = backbuffer;
        backbuffer = temp;
    }

    // internal stuff
    uint32_t cycle_clock;
    uint32_t dot_clock;
    uint16_t current_xpos;
    uint16_t current_scanline;

    bool overlay_present;

    void render_scanline();
    void render_border_scanline();
    void render_overlay(uint16_t xpos, uint16_t ypos, char *text);
    void setup_color_palettes();
    uint32_t borders_contrast_foreground_color();
public:
    vicv(void);
    ~vicv(void);
    
    bool irq_line;

    // pointer to the buffer that currently can be shown
    uint32_t *frontbuffer;
    // pointer to the buffer that's currently being written to
    uint32_t *backbuffer;

    // move this member to private again????   !!!!
    uint32_t *color_palette;

    // this will be flagged if a frame is completely done
    bool frame_done;

    void reset();

    // run cycles on this chip
    void run(uint32_t number_of_cycles);
    // experimental version
    void run2(uint32_t number_of_dots);

    uint16_t    get_current_scanline();
    uint16_t    get_current_pixel();
    uint32_t    get_cycle_clock() { return cycle_clock; }
    uint32_t    get_dot_clock() { return dot_clock; }
    bool        is_hblank();
    bool        is_vblank();
    
    void toggle_overlay() { overlay_present = !overlay_present; }
    
    // Register access to vicv
    inline uint8_t read_byte(uint8_t address)  { return registers[address]; }
    inline void write_byte(uint8_t address, uint8_t byte) { registers[address] = byte; }
};

}

#endif
