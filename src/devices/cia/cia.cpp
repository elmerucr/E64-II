//  cia.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "cia.hpp"
#include "common.hpp"
#include <cstdio>

E64::cia::cia()
{
    cycles_per_interval = CPU_CLOCK_SPEED / 100; // no of cycles @ CPU clockspeed for a total of 10 ms
    reset();
}

void E64::cia::reset()
{
    cycle_counter = 0;
    
    for(int i=0; i<256; i++) registers[i] = 0x00;
    for(int i=0; i<128; i++) keys_last_known_state[i] = 0x00;
    for(int i=0; i<256; i++) event_list[i] = 0x00;
    head = tail = 0;
    
    generate_key_events = false;
    
    key_down = false;
    keyboard_repeat_delay = 60;
    keyboard_repeat_speed = 5;
    keyboard_repeat_counter = 0;
}

void E64::cia::push_event(uint8_t event)
{
    event_list[head] = event;
    head++;
    if( head == tail) { tail++; printf("too many items in list\n"); }
    printf("head: %u tail: %u %u items in list\n", head, tail, (uint8_t)(head - tail) );
}

uint8_t E64::cia::pop_event()
{
    uint8_t result;
    
    if( head == tail )
    {
        // no events in queue / stack return empty scancode
        result = E64::SCANCODE_EMPTY;
    }
    else
    {
        result = event_list[tail];
        tail++;
    }
    
    return result;
}

void E64::cia::run(int no_of_cycles)
{
    cycle_counter += no_of_cycles;
    
    if( cycle_counter >= cycles_per_interval )
    {
        cycle_counter -= cycles_per_interval;
        
        // registers 128 to 255 reflect the current keyboard state
        // shift each register one bit to the left, bit 0 is only set if key is pressed
        // if one of the keys changed its state, push an event
        for(int i=0x00; i<0x80; i++)
        {
            registers[0x80|i] = (registers[0x80|i] << 1) | keys_last_known_state[i];

            switch(registers[0x80|i] & 0b00000011)
            {
                case 0b01:
                    // Event: key pressed
                    if(generate_key_events)
                    {
                        key_down = true;
                        last_key = i;
                        keyboard_repeat_current_max = keyboard_repeat_delay;
                        keyboard_repeat_counter = 0;
                    }
                    break;
                case 0b10:
                    // Event: key released
                    if(generate_key_events)
                    {
                        if( i == last_key ) key_down = false;
                    }
                    break;
                default:
                    // do nothing
                    break;
            }
        }
        
        if(key_down)
        {
            if( keyboard_repeat_counter == 0 ) push_event( last_key );
            keyboard_repeat_counter++;
            if(keyboard_repeat_counter == keyboard_repeat_current_max)
            {
                keyboard_repeat_counter = 0;
                keyboard_repeat_current_max = keyboard_repeat_speed;
            }
        }
    }
}

uint8_t E64::cia::read_byte(uint8_t address)
{
    uint8_t return_value = 0x00;
    
    switch( address )
    {
        case 0x00:
            return_value = events_waiting() ? 0x01 : 0x00;
        case 0x01:
            return_value = (generate_key_events ? 0x01 : 0x00);
            break;
        case 0x02:
            return_value = pop_event();
            break;
        default:
            return_value = registers[address];
            break;
    }
    
    return return_value;
}


void E64::cia::write_byte(uint8_t address, uint8_t byte)
{
    switch(address)
    {
        case 0x01:
            generate_key_events = ( byte & 0b00000001 ) ? true : false;
            
            if( byte & 0b10000000 )
            {
                // a write to bit 7 clears the event list
                tail = head;
            }
            break;
        default:
            // all other addresses are not written to
            break;
    }
}
