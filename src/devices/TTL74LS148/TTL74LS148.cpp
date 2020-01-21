//  TTL74LS148.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "TTL74LS148.hpp"
#include "common_defs.hpp"

E64::TTL74LS148::TTL74LS148()
{
    number_of_devices = 0;
    for(int i=0; i<256; i++)
    {
        devices[i] = { &default_device, 0 };
    }
}

void E64::TTL74LS148::connect_device(bool *device_pin, int level)
{
    devices[number_of_devices] = { device_pin, level };
    number_of_devices++;
}

void E64::TTL74LS148::update_interrupt_level()
{
    unsigned int temp_level = 0;
    for(int i=0; i<number_of_devices; i++)
    {
        if( (*devices[i].device_pin == false) && (devices[i].level > temp_level) ) temp_level = devices[i].level;
    }
    computer.m68k_ic->setIPL(temp_level);
}
