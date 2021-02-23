//  machine.hpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "cia.hpp"
#include "cpu_moira.hpp"
#include "fd.hpp"
#include "frequency_divider.hpp"
#include "mmu.hpp"
#include "sids.hpp"
#include "timer.hpp"
#include "TTL74LS148.hpp"
#include "vicv.hpp"
#include "blitter.hpp"

// output states for run function
#define NO_BREAKPOINT       0b00000000
#define CPU_BREAKPOINT      0b00000001
#define SCANLINE_BREAKPOINT 0b00000010

namespace E64
{

enum machine_mode
{
    MONITOR,
    RUNNING
};
    
class machine_t
{
private:
    frequency_divider *m68k_to_vicv;
    frequency_divider *m68k_to_blitter;
    frequency_divider *m68k_to_sid;
    char machine_help_string[2048];
public:
    enum machine_mode   mode;
    bool on;
    
	mmu_ic		*mmu;
	cpu_moira	*m68k;
	TTL74LS148_ic	*TTL74LS148;
	timer_ic	*timer;
	vicv_ic		*vicv;
	blitter_ic	*blitter;
	sids_ic		*sids;
	cia_ic		*cia;
	fd		*fd0;

	machine_t();
	~machine_t();

	uint8_t  run(uint16_t no_of_cycles);

	void reset();

	void switch_mode(enum machine_mode new_mode);
	void toggle_mode();
};

}

#endif
