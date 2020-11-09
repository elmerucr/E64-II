//  TTL74LS148.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "TTL74LS148.hpp"
#include "common.hpp"

E64::TTL74LS148_ic::TTL74LS148_ic()
{
    number_of_devices = 0;
    
    for(int i=0; i<256; i++)
    {
        devices[i] = { true, 0 };
    }
}

uint8_t E64::TTL74LS148_ic::connect_device(int level)
{
    devices[number_of_devices].level = level;
    
    number_of_devices++;
    
    return (number_of_devices - 1);
}

void E64::TTL74LS148_ic::pull_line(uint8_t handler)
{
    devices[handler].state = false;
    update_interrupt_level();
}

void E64::TTL74LS148_ic::release_line(uint8_t handler)
{
    devices[handler].state = true;
    update_interrupt_level();
}

void E64::TTL74LS148_ic::update_interrupt_level()
{
    unsigned int temp_level = 0;
    for(int i=0; i<number_of_devices; i++)
    {
        if( (devices[i].state == false) && (devices[i].level > temp_level) ) temp_level = devices[i].level;
    }
    pc.m68k->setIPL(temp_level);
}
