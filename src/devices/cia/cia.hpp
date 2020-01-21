//  cia.hpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.
//
//  register 0 is Interrupt Status Register
//  READ
//    bit 0 set: one or more keyboard events waiting
//          not set: no keyboard events are available anymore
//    .
//    bit 1-6: currently unused
//    .
//    bit 7: An interrupt occurred and it has not yet been acknowledged
//  WRITE
//    bit 0: Write a 1: Acknowledge keyboard interrupt
//    .
//    .
//    .
//
//  register 1 is Interrupt Control Register
//  READ and WRITE:
//    bit 0: 1=Keyboard generates interrupts on key press or release events, 0=off
//    .
//    .
//
//  register 2 contains the scancode for the last key press
//  READ only

#include <cstdint>

#ifndef cia_hpp
#define cia_hpp

namespace E64
{
    enum scancodes
    {
        SCANCODE_EMPTY = 0x00,     // 0x00
        SCANCODE_ESCAPE,
        SCANCODE_F1,
        SCANCODE_F2,
        SCANCODE_F3,
        SCANCODE_F4,
        SCANCODE_F5,
        SCANCODE_F6,
        SCANCODE_F7,               // 0x08
        SCANCODE_F8,
        SCANCODE_GRAVE,
        SCANCODE_1,
        SCANCODE_2,
        SCANCODE_3,
        SCANCODE_4,
        SCANCODE_5,
        SCANCODE_6,                // 0x10
        SCANCODE_7,
        SCANCODE_8,
        SCANCODE_9,
        SCANCODE_0,
        SCANCODE_MINUS,
        SCANCODE_EQUALS,
        SCANCODE_BACKSPACE,
        SCANCODE_TAB,              // 0x18
        SCANCODE_Q,
        SCANCODE_W,
        SCANCODE_E,
        SCANCODE_R,
        SCANCODE_T,
        SCANCODE_Y,
        SCANCODE_U,
        SCANCODE_I,               // 0x20
        SCANCODE_O,
        SCANCODE_P,
        SCANCODE_LEFTBRACKET,
        SCANCODE_RIGHTBRACKET,
        SCANCODE_RETURN,
        SCANCODE_A,
        SCANCODE_S,
        SCANCODE_D,                // 0x28
        SCANCODE_F,
        SCANCODE_G,
        SCANCODE_H,
        SCANCODE_J,
        SCANCODE_K,
        SCANCODE_L,
        SCANCODE_SEMICOLON,
        SCANCODE_APOSTROPHE,       // 0x30
        SCANCODE_BACKSLASH,
        SCANCODE_LSHIFT,
        SCANCODE_Z,
        SCANCODE_X,
        SCANCODE_C,
        SCANCODE_V,
        SCANCODE_B,
        SCANCODE_N,                // 0x38
        SCANCODE_M,
        SCANCODE_COMMA,
        SCANCODE_PERIOD,
        SCANCODE_SLASH,
        SCANCODE_RSHIFT,
        SCANCODE_LCTRL,
        SCANCODE_LALT,
        SCANCODE_LGUI,              // 0x40
        SCANCODE_SPACE,
        SCANCODE_RGUI,
        SCANCODE_RALT,
        SCANCODE_RCTRL,
        SCANCODE_LEFT,
        SCANCODE_UP,
        SCANCODE_DOWN,
        SCANCODE_RIGHT             // 0x48
    };

    class cia
    {
    private:
        void push_event(uint8_t event);
        uint8_t pop_event();
        // implement a fifo queue, important for key presses, you don't want them in the wrong order
        uint8_t event_queue[256];
        // always points to the next available location to store an item
        uint8_t event_stack_pointer_head;
        // always points to the currently available item
        // if (head - tail) == 0, then no item available
        uint8_t event_stack_pointer_tail;
    public:
        // constructor
        cia();
        // reset, also called by constructor
        void reset();
        // irq pin is owned by the timer
        bool irq_pin;
        
        // THESE NEXT ONES SHOULD BE PRIVATE IN THE FUTURE!
        uint8_t scancodes_last_known_state[128];
        uint8_t registers[256];
        //        
        
        // unlike other components, the CIA doesn't need a specified no. of cycles as argument
        void run();
        // register access functions
        uint8_t read_byte(uint8_t address);
        void write_byte(uint8_t address, uint8_t byte);
    };
}

#endif
