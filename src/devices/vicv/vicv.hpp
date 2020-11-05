//  vicv.hpp
//  E64
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.

#include <cstdint>

#ifndef vicv_hpp
#define vicv_hpp

/*  Registers 0x00-0x01 combined are a 16 bit color value for the current
 *  border color.
 */
#define VICV_REG_BOR_HIGH       0x00
#define VICV_REG_BOR_LOW        0x01

/*  Registers 0x02-0x0b currently unused  */

/*  Register 0x0c is a byte telling the size of the top and bottom
 *  borders.
 */
#define VICV_REG_BORDER_SIZE    0x0c

/*  Register 0x0d currently unused  */

/*  Register 0x0e is interrupt status register. Write to bit 0 means
 *  acknowledge VBLANK interrupt.
 */
#define VICV_REG_ISR            0x0e

/*  Register 0x0f currently unused  */

/*  Register 0x10, write to bit 0 = swap front and back buffer
 *  (machine internally).
 */
#define VICV_REG_BUFFERSWAP     0x10

namespace E64 {

class vicv
{
private:
    // this chip contains 256 registers (and are mapped to a specific page)
    uint8_t registers[256];
    
    // framebuffer pointers inside the virtual machine
    uint16_t *framebuffer0;
    uint16_t *framebuffer1;

    uint32_t cycle_clock;       // measures all cycles
    uint32_t dot_clock;         // measures only cycles that have written a pixel

    
    // breakpoint related
private:
    bool scanline_breakpoints[1024];
    uint16_t old_y_pos;
public:
    bool breakpoint_reached;

    
    // stats overlay
private:
    bool stats_overlay_present;
    void render_stats(uint16_t xpos, uint16_t ypos);
    char *stats_text;
public:
    inline void toggle_stats() { stats_overlay_present = !stats_overlay_present; }
    void set_stats(char *text) { stats_text = text; }

    
public:
    vicv(void);
    
    // framebuffer pointers inside the virtual machine
    uint16_t *frontbuffer;
    uint16_t *backbuffer;

    // interrupt device no for vblanc irq
    uint8_t vblank_interrupt_device_number;

    // this will be flagged if a frame is completely done
    bool frame_done;

    void reset();

    // run cycles on this chip
    void run(uint32_t number_of_dots);

    uint16_t        get_current_scanline();
    uint16_t        get_current_pixel();
    //inline uint32_t get_cycle_clock() { return cycle_clock; }
    //inline uint32_t get_dot_clock() { return dot_clock; }
    bool            is_hblank();
    bool            is_vblank();
    
    void clear_scanline_breakpoints();
    void add_scanline_breakpoint(uint16_t scanline);
    void remove_scanline_breakpoint(uint16_t scanline);
    bool is_scanline_breakpoint(uint16_t scanline);
    
    // Register access to vicv
    uint8_t read_byte(uint8_t address);
    void write_byte(uint8_t address, uint8_t byte);
};

}

#endif
