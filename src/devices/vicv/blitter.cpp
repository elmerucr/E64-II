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

void E64::blitter::add_operation()
{
    //
}

uint8_t E64::blitter::read_byte(uint8_t address)
{
    return 0;
}

void E64::blitter::write_byte(uint8_t address, uint8_t byte)
{
    switch( address )
    {
        case 0x00:
            if( byte & 0b00000001 ) printf("dummy for clearing a framebuffer\n");
            break;
        case 0x01:
            if( byte & 0b00000001 ) printf("dummy for addition of a blit action\n");
            break;
        default:
            break;
    }
}
