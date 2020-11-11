//  timer.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "timer.hpp"
#include "common.hpp"

void E64::timer_ic::reset()
{
    pc.TTL74LS148->release_line(interrupt_device_number);
    
    registers[0] = 0x00;        // no pending irq's
    registers[1] = 0x00;        // all timers turned off
    
    // load data register with value 1 bpm
    registers[2] = 0x00;    // hi
    registers[3] = 0x01;    // lo
    
    // may never be zero
    timer0_bpm = (registers[2] << 8) | registers[3];
    timer1_bpm = (registers[2] << 8) | registers[3];
    timer2_bpm = (registers[2] << 8) | registers[3];
    timer3_bpm = (registers[2] << 8) | registers[3];

    timer0_clock_interval = bpm_to_clock_interval(timer0_bpm);
    timer1_clock_interval = bpm_to_clock_interval(timer1_bpm);
    timer2_clock_interval = bpm_to_clock_interval(timer2_bpm);
    timer3_clock_interval = bpm_to_clock_interval(timer3_bpm);

    timer0_counter = 0;
    timer1_counter = 0;
    timer2_counter = 0;
    timer3_counter = 0;
}

void E64::timer_ic::run(uint32_t number_of_cycles)
{
    timer0_counter += number_of_cycles;
    timer1_counter += number_of_cycles;
    timer2_counter += number_of_cycles;
    timer3_counter += number_of_cycles;

    if( (timer0_counter >= timer0_clock_interval) && (registers[1] & 0x01) )
    {
        timer0_counter -= timer0_clock_interval;
        pc.TTL74LS148->pull_line(interrupt_device_number);
        registers[0] |= 0b00000001;
    }
    if( (timer1_counter >= timer1_clock_interval) && (registers[1] & 0x02) )
    {
        timer1_counter -= timer1_clock_interval;
        pc.TTL74LS148->pull_line(interrupt_device_number);
        registers[0] |= 0b00000010;
    }
    if( (timer2_counter >= timer2_clock_interval) && (registers[1] & 0x04) )
    {
        timer2_counter -= timer2_clock_interval;
        pc.TTL74LS148->pull_line(interrupt_device_number);
        registers[0] |= 0b00000100;
    }
    if( (timer3_counter >= timer3_clock_interval) && (registers[1] & 0x08) )
    {
        timer3_counter -= timer3_clock_interval;
        pc.TTL74LS148->pull_line(interrupt_device_number);
        registers[0] |= 0b00001000;
    }
}

uint32_t E64::timer_ic::bpm_to_clock_interval(uint16_t bpm)
{
    uint32_t result = (60.0 / bpm) * CPU_CLOCK_SPEED;
    return result;
}

uint8_t E64::timer_ic::read_byte(uint8_t address)
{
    return registers[ address & 0x03 ];
}

void E64::timer_ic::write_byte(uint8_t address, uint8_t byte)
{
    switch(address & 0x03)
    {
        case 0x00:
            /*
             *  b s   r
             *  0 0 = 0
             *  0 1 = 1
             *  1 0 = 0
             *  1 1 = 0
             *
             *  b = bit that's written
             *  s = status (on if an interrupt was caused)
             *  r = boolean result (acknowledge an interrupt (s=1) if it is written to with a 1
             *  r = (~b) & s
	     */

            registers[0] = (~(byte & 0x0f)) & registers[0];

            if ((registers[0] & 0x0f) == 0) {
                // no timers left causing interrupts
                pc.TTL74LS148->release_line(interrupt_device_number);
            }
            break;
        case 0x01:
        {
            uint8_t turned_on = byte & (~registers[1]);
		
            if (turned_on & 0b00000001) {
                timer0_bpm = (uint16_t)(registers[2] << 8) | registers[3];
                if(timer0_bpm == 0) timer0_bpm = 1;
                timer0_clock_interval = bpm_to_clock_interval(timer0_bpm);
                timer0_counter = 0;
            }
            if (turned_on & 0b00000010) {
                timer1_bpm = (uint16_t)(registers[2] << 8) | registers[3];
                if(timer1_bpm == 0) timer1_bpm = 1;
                timer1_clock_interval = bpm_to_clock_interval(timer1_bpm);
                timer1_counter = 0;
            }
            if (turned_on & 0b00000100) {
                timer2_bpm = (uint16_t)(registers[2] << 8) | registers[3];
                if(timer2_bpm == 0) timer2_bpm = 1;
                timer2_clock_interval = bpm_to_clock_interval(timer2_bpm);
                timer2_counter = 0;
            }
            if (turned_on & 0b00001000) {
                timer3_bpm = (uint16_t)(registers[2] << 8) | registers[3];
                if(timer3_bpm == 0) timer3_bpm = 1;
                timer3_clock_interval = bpm_to_clock_interval(timer3_bpm);
                timer3_counter = 0;
            }
            registers[0x01] = byte & 0x0f;	// turn off all the rest?
            break;
        }
        default:
            registers[ address & 0x03 ] = byte;
            break;
    }
}

uint64_t E64::timer_ic::get_timer0_counter()
{
    return timer0_counter;
}

uint64_t E64::timer_ic::get_timer0_clock_interval()
{
    return timer0_clock_interval;
}

uint64_t E64::timer_ic::get_timer1_counter()
{
    return timer1_counter;
}

uint64_t E64::timer_ic::get_timer1_clock_interval()
{
    return timer1_clock_interval;
}

uint64_t E64::timer_ic::get_timer2_counter()
{
    return timer2_counter;
}

uint64_t E64::timer_ic::get_timer2_clock_interval()
{
    return timer2_clock_interval;
}

uint64_t E64::timer_ic::get_timer3_counter()
{
    return timer3_counter;
}

uint64_t E64::timer_ic::get_timer3_clock_interval()
{
    return timer3_clock_interval;
}
