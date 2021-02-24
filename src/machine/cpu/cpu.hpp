/*
 * cpu.hpp
 * E64-II
 *
 * Copyright © 2019-2020 elmerucr. All rights reserved.
 */

#ifndef	CPU_MOIRA
#define	CPU_MOIRA

#include "Moira.h"

using namespace moira;

class cpu : public Moira
{
	// must be implemented
	u8 read8(u32 addr) override;
	u16 read16(u32 addr) override;
	void write8 (u32 addr, u8  val) override;
	void write16 (u32 addr, u16 val) override;
	//
	void breakpointReached(u32 addr) override;
	//
public:
	inline i64 run(int no_of_cycles)
	{
		i64 start_cycles = getClock();
		do
		{
			execute();
		}
		while( ( (getClock() - start_cycles) < no_of_cycles) && !breakpoint_reached );
		return getClock() - start_cycles;
	}
	
	void dump_registers(char *temp_string);
	bool breakpoint_reached;
};

#endif
