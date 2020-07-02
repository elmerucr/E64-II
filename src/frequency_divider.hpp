//  frequency_divider.hpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.
//
//  Algorithm based on bresenham line algorithm. Besides very simple multipliers / dividers,
//  it is also possible to build very "complex" ones.

#ifndef frequency_divider_hpp
#define frequency_divider_hpp

#include <cstdint>

namespace E64
{

class frequency_divider
{
private:
    
    uint64_t clock0_frequency;
    uint64_t clock1_frequency;
    uint64_t clock0_cycle_counter;
    uint64_t clock1_cycle_counter;

    uint64_t mult;
    uint64_t mod;
    uint64_t result;

public:

    frequency_divider(uint32_t clock0_freq, uint32_t clock1_freq)
    {
        clock0_frequency = clock0_freq;
        clock1_frequency = clock1_freq;
        reset_cycle_counter();
        mod = 0;
    }
    
    
    inline uint32_t clock(uint32_t delta_clock0)
    {
        mult = (delta_clock0 * clock1_frequency) + mod;
        mod  = mult % clock0_frequency;
        result = mult / clock0_frequency;
        clock0_cycle_counter += delta_clock0;
        clock1_cycle_counter += result;
        return (uint32_t)result;
    }
    
    
    void reset_cycle_counter()
    {
        clock0_cycle_counter = 0;
        clock1_cycle_counter = 0;
    }

};

}

#endif
