//  mmu_E64_II.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "mmu_E64_II.hpp"
#include "common_defs.hpp"

extern uint8_t kernel[];

E64::mmu::mmu()
{
    // allocate main ram and fill with a pattern
    ram = new uint8_t[RAM_SIZE * sizeof(uint8_t)];
    // fill alternating blocks with 0x00 and 0x80
    for(int i=0; i< RAM_SIZE; i++) ram[i] = (i & 64) ? 0x80 : 0x00;
}

E64::mmu::~mmu()
{
    delete ram;
    ram = nullptr;
}

unsigned int E64::mmu::read_memory_8(unsigned int address)
{
    uint32_t page = address >> 8;
    if( page == IO_VICV_PAGE )
    {
        return computer.vicv_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_SND_PAGE )
    {
        return computer.sound_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_TIMER_PAGE )
    {
        return computer.timer_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_CIA_PAGE )
    {
        return computer.cia_ic->read_byte(address & 0x000000ff);
    }
    else if( (address >> 16) == IO_KERNEL_MASK )
    {
        return kernel[address & 0x0000ffff];
    }
    else if( (address & IO_RESET_VECTOR_MASK) == 0 )
    {
        return kernel[address & 0x0000ffff];
    }
    else
    {
        // normal memory access
        return ram[address & 0x00ffffff];
    }
}
    
unsigned int E64::mmu::read_memory_16(unsigned int address)
{
    unsigned int result;
    uint32_t temp_address = address;
    result = read_memory_8(temp_address);
    temp_address++;
    result = read_memory_8(temp_address) | (result << 8);
    return result;
}

unsigned int E64::mmu::read_disassembler_8(unsigned int address)
{
    return read_memory_8(address);
}

unsigned int E64::mmu::read_disassembler_16(unsigned int address)
{
    return read_memory_16(address);
}

void E64::mmu::write_memory_8(unsigned int address, unsigned int value)
{
    uint32_t page = address >> 8;
    if( page == IO_VICV_PAGE )
    {
        computer.vicv_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_SND_PAGE )
    {
        computer.sound_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_TIMER_PAGE )
    {
        computer.timer_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_CIA_PAGE )
    {
        computer.cia_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else
    {
        // normal memory access
        ram[address & 0x00ffffff] = value & 0x000000ff;
    }
}

void E64::mmu::write_memory_16(unsigned int address, unsigned int value)
{
    uint32_t temp_address = address;
    write_memory_8(temp_address, value >> 8);
    temp_address++;
    write_memory_8(temp_address, value & 0xff);
}
