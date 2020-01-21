//  mmu_E64_II.hpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#ifndef MMU_E64_II_HPP
#define MMU_E64_II_HPP

#include <cstdint>

extern uint8_t patched_char_rom[];

namespace E64
{
    class mmu
    {
    private:        
    public:
        mmu();
        ~mmu();
        uint8_t *ram;
        
        unsigned int read_memory_8(unsigned int address);
        unsigned int read_memory_16(unsigned int address);
        
        unsigned int read_disassembler_8(unsigned int address);
        unsigned int read_disassembler_16(unsigned int address);
        
        void write_memory_8(unsigned int address, unsigned int value);
        void write_memory_16(unsigned int address, unsigned int value);
    };
}

#endif
