//  frequency_divider.hpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.
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
        frequency_divider(uint32_t clock0_freq, uint32_t clock1_freq);
        uint32_t clock(uint32_t delta_clock0);
        void reset_cycle_counter();
    };
}

#endif
