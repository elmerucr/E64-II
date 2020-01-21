//  cpu_moira.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "cpu_moira.hpp"
#include "common_defs.hpp"

#include <cstdio>

u8 cpu_moira::read8(u32 addr)
{
    return computer.mmu_ic->read_memory_8(addr);
};

u16 cpu_moira::read16(u32 addr)
{
    return computer.mmu_ic->read_memory_16(addr);
};

void cpu_moira::write8 (u32 addr, u8  val)
{
    computer.mmu_ic->write_memory_8(addr, val);
};

void cpu_moira::write16 (u32 addr, u16 val)
{
    computer.mmu_ic->write_memory_16(addr, val);
};

i64 cpu_moira::run(int no_of_cycles)
{
    i64 start_cycles = getClock();
    do
    {
        execute();
    }
    while( ( (getClock() - start_cycles) < no_of_cycles) && !breakpoint_reached );
    return getClock() - start_cycles;
}

void cpu_moira::breakpointReached(u32 addr)
{
    breakpoint_reached = true;
}

void cpu_moira::dump_registers(char *temp_string)
{
    int n;
    int max = 2048;
    n = snprintf( temp_string,max,"   PC:%08x SSP/USP: %08x %08x\n\n", getPC(), getSSP(), getUSP() );
    temp_string += n;
    max -= n;
    n = snprintf( temp_string,max,"D0-D3:%08x %08x %08x %08x\n", getD(0), getD(1), getD(2), getD(3) );
    temp_string += n;
    max -= n;
    n = snprintf( temp_string,max,"D4-D7:%08x %08x %08x %08x\n", getD(4), getD(5), getD(6), getD(7) );
    temp_string += n;
    max -= n;
    n = snprintf( temp_string,max,"A0-A3:%08x %08x %08x %08x\n", getA(0), getA(1), getA(2), getA(3) );
    temp_string += n;
    max -= n;
    n = snprintf( temp_string,max,"A4-A7:%08x %08x %08x %08x\n\n", getA(4), getA(5), getA(6), getA(7) );
    temp_string += n;
    max -= n;
    n = snprintf( temp_string, max, "   SR: ");
    temp_string += n;
    max -= n;
    disassembleSR(temp_string);
    temp_string += 16;
    max -= 16;
    n = snprintf( temp_string, max, "  ipl pins: %c%c%c", getIPL()&0b100?'1':'0', getIPL()&0b010?'1':'0', getIPL()&0b001?'1':'0');
}
