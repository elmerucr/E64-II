//  mmu.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "mmu.hpp"
#include "common.hpp"

extern uint8_t kernel[];

E64::mmu::mmu()
{
    // allocate main ram and fill with a pattern
    ram = new uint8_t[RAM_SIZE * sizeof(uint8_t)];
    ram_as_words = (uint16_t *)ram;
    reset();
}

E64::mmu::~mmu()
{
    delete ram;
    ram = nullptr;
}

void E64::mmu::reset()
{
    // fill alternating blocks with 0x00 and 0x80
    for(int i=0; i<RAM_SIZE; i++) ram[i] = (i & 64) ? 0x10 : 0x00;
}

unsigned int E64::mmu::read_memory_8(unsigned int address)
{
    uint32_t page = address >> 8;
    if( page == IO_VICV_PAGE )
    {
        return pc.vicv_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_SND_PAGE )
    {
        return pc.sids_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_BLITTER_PAGE )
    {
        return pc.blitter_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_TIMER_PAGE )
    {
        return pc.timer_ic->read_byte(address & 0x000000ff);
    }
    else if( page == IO_CIA_PAGE )
    {
        return pc.cia_ic->read_byte(address & 0x000000ff);
    }
    else if( ( (address & 0x00fc0000) >> 16) == IO_KERNEL_MASK )
    {
        return kernel[address & 0x0000ffff];
    }
    else if( (address & IO_RESET_VECTOR_MASK) == 0 )
    {
        return kernel[address & 0x0000ffff];
    }
    else if( (address & IO_PATCHED_CHAR_ROM_MASK) == IO_PATCHED_CHAR_ROM_MASK)
    {
        return patched_char_rom[address & 0x000007ff];
    }
    else
    {
        // ram  access
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
        pc.vicv_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_SND_PAGE )
    {
        pc.sids_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_BLITTER_PAGE )
    {
        pc.blitter_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_TIMER_PAGE )
    {
        pc.timer_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else if( page == IO_CIA_PAGE )
    {
        pc.cia_ic->write_byte(address & 0x000000ff, value & 0x000000ff);
    }
    else
    {
        // ram access
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
