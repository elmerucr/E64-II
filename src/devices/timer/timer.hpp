//  timer.hpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.
//
//  register 0 is Interrupt Status Register
//  ON READ
//    bit 0     set: timer0 event waiting
//          not set: no timer0 event waiting
//    bit 1     set: timer1 event waiting
//          not set: no timer1 event waiting
//    bit 2     set: timer2 event waiting
//          not set: no timer2 event waiting
//    bit 3     set: timer3 event waiting
//          not set: no timer3 event waiting
//    .
//    bit 4-6: currently unused
//    .
//    bit 7: An interrupt occurred and it has not yet been acknowledged (check bits 0 - 3)
//
//  ON WRITE
//    bit 0: Write a 1: Acknowledge timer0 interrupt
//    bit 1: Write a 1: Acknowledge timer1 interrupt
//    bit 2: Write a 1: Acknowledge timer2 interrupt
//    bit 3: Write a 1: Acknowledge timer3 interrupt
//    .
//
//  register 1 is Interrupt Control Register
//  READ and WRITE:
//    bit 0: Timer0 interrupts 1=on, 0=off
//    bit 1: Timer1 interrupts 1=on, 0=off
//    bit 2: Timer2 interrupts 1=on, 0=off
//    bit 3: Timer3 interrupts 1=on, 0=off
//    .
//    register 2 and 3 are respectively the hi and lo byte
//    of an unsigned 16bit value (big endian format!)

#ifndef timer_hpp
#define timer_hpp

#include <cstdint>

namespace E64
{
    class timer
    {
    private:
        uint8_t registers[4];

        uint16_t timer0_bpm;
        uint16_t timer1_bpm;
        uint16_t timer2_bpm;
        uint16_t timer3_bpm;
        
        uint32_t timer0_clock_interval;
        uint32_t timer1_clock_interval;
        uint32_t timer2_clock_interval;
        uint32_t timer3_clock_interval;

        uint64_t timer0_counter;
        uint64_t timer1_counter;
        uint64_t timer2_counter;
        uint64_t timer3_counter;

        uint32_t bpm_to_clock_interval(uint16_t bpm);
    public:
        // constructor
        timer();
        // reset, called by constructor
        void reset(void);
        // irq pin is owned by the timer
        bool irq_pin;

        // register access functions
        uint8_t read_byte(uint8_t address);
        void write_byte(uint8_t address, uint8_t byte);

        // get / set functions
        uint64_t get_timer0_counter(void);
        uint64_t get_timer0_clock_interval(void);
        uint64_t get_timer1_counter(void);
        uint64_t get_timer1_clock_interval(void);
        uint64_t get_timer2_counter(void);
        uint64_t get_timer2_clock_interval(void);
        uint64_t get_timer3_counter(void);
        uint64_t get_timer3_clock_interval(void);
        
        // run cycles on this ic
        void run(uint32_t number_of_cycles);
    };
}

#endif /* timer_hpp */
