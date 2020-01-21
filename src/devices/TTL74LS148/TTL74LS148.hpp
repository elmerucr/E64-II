//  TTL74LS148.hpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.
//
// exception collector and
// priority encoder
// 8 input lines, 3 bit output

#ifndef TTL74LS148_HPP
#define TTL74LS148_HPP

#include <cstdint>

namespace E64
{
    class TTL74LS148
    {
    private:
        struct device
        {
            bool *device_pin;
            int level;
        };
        struct device devices[256];
        uint8_t number_of_devices;
        bool default_device = true;
        int output_level;
    public:
        TTL74LS148();
        
        // when connecting a device, both a pointer to a pin and an interrupt level (1-6) must be supplied
        void connect_device(bool *device_pin, int level);
        
        
        void update_interrupt_level();
    };
}

#endif
