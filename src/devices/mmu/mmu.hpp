//  mmu.hpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#ifndef MMU_HPP
#define MMU_HPP

#include <cstdint>

#define IO_CIA_PAGE                 0xfa03
#define IO_VICV_PAGE                0xfa04
#define IO_SND_PAGE                 0xfa05
#define IO_TIMER_PAGE               0xfa06
#define IO_BLITTER_PAGE             0xfa07
#define IO_RESET_VECTOR_MASK        0xfffff8
#define IO_PATCHED_CHAR_ROM_MASK    0xfb
#define IO_KERNEL_MASK              0xfc

extern uint8_t patched_char_rom[];

namespace E64
{

class mmu
{
private:
public:
    mmu();
    ~mmu();
    
    uint8_t  *ram;                  // make this private and work with friend class?
    uint16_t *ram_as_words;         // make this private and work with friend class?
    
    void reset();
    
    unsigned int read_memory_8(unsigned int address);
    unsigned int read_memory_16(unsigned int address);
    
    unsigned int read_disassembler_8(unsigned int address);
    unsigned int read_disassembler_16(unsigned int address);
    
    void write_memory_8(unsigned int address, unsigned int value);
    void write_memory_16(unsigned int address, unsigned int value);
};

}

#endif
