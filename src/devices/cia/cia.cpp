//  cia.cpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "cia.hpp"

E64::cia::cia()
{
    reset();
}

void E64::cia::reset()
{
    for(int i=0; i<256; i++) registers[i] = 0x00;
    for(int i=0; i<128; i++) scancodes_last_known_state[i] = 0x00;
    for(int i=0; i<256; i++) event_queue[i] = 0x00;
    event_stack_pointer_head = 0;
    event_stack_pointer_tail = 0;
    // on reset of cia, line is "high" = 1
    // please note that registers 0 and 1 start empty
    irq_pin = true;
}

void E64::cia::push_event(uint8_t event)
{
    event_queue[event_stack_pointer_head] = event;
    event_stack_pointer_head++;
    // set bit 0 of register 0 => keyboard event is waiting
    registers[0x00] |= 0x01;
}

uint8_t E64::cia::pop_event()
{
    uint8_t result;
    uint8_t no_of_items = event_stack_pointer_head - event_stack_pointer_tail;
    if(no_of_items == 0x00)
    {
        // no events in queue / stack return empty scancode
        result = E64::SCANCODE_EMPTY;
    }
    else
    {
        result = event_queue[event_stack_pointer_tail];
        event_stack_pointer_tail++;
        no_of_items = event_stack_pointer_head - event_stack_pointer_tail;
        // clear the keyboard event bit 0 if there's no value anymore
        if(no_of_items == 0) registers[0x00] &= 0xfe;
    }
    return result;
}

void E64::cia::run()
{
    // register 128 to 255 reflect the current keyboard state
    // shift each register one bit to the left, bit 0 is only set if key is pressed
    // if one of the keys changed its state, push an event
    for(int i=0x00; i<0x80; i++)
    {
        registers[0x80+i] = (registers[0x80+i] << 1) | scancodes_last_known_state[i];

        switch(registers[0x80+i] & 0x03)
        {
            case 0x01:
                // Event: key pressed
                push_event(i);
                if(registers[0x01] & 0x01)
                {
                    irq_pin = false;
                    registers[0x00] |= 0x80;
                }
                break;
            case 0x02:
                // Event: key released
                push_event(0x80 | i);
                if(registers[0x01] & 0x01)
                {
                    irq_pin = false;
                    registers[0x00] |= 0x80;
                }
                break;
            default:
                // do nothing
                break;
        }
    }
}

uint8_t E64::cia::read_byte(uint8_t address)
{
    if(address == 0x02)
    {   // special case is register 0x02 (contains the popped scancode for the last event)
        return pop_event();
    }
    else
    {
        return registers[address];
    }
}


void E64::cia::write_byte(uint8_t address, uint8_t byte)
{
    switch(address)
    {
        case 0x00:
            if(byte & 0x01)
            {
                // a write to bit 0, means acknowledge the interrupt
                irq_pin = true;
                // clear bit 7
                registers[0x00] &= 0x7f;
            }
            break;
        case 0x01:
            registers[0x01] = byte;
            break;
        default:
            // all other addresses are not written to
            break;
    }
}
