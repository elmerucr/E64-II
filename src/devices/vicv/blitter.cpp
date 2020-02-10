//  blitter.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "blitter.hpp"
#include "common_defs.hpp"

/* dummy function
 * replace it!
 */
void E64::blitter::run(int no_of_cycles)
{
    while(no_of_cycles > 0)
    {
        computer.mmu_ic->ram[0x00d001] = computer.mmu_ic->ram[0x00eff2];
        no_of_cycles--;
    }
}
