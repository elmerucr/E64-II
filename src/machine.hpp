//  machine.hpp
//  E64
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "cia.hpp"
#include "cpu_moira.hpp"
#include "frequency_divider.hpp"
#include "mmu.hpp"
#include "sids.hpp"
#include "timer.hpp"
#include "TTL74LS148.hpp"
#include "vicv.hpp"
#include "blitter.hpp"

// output states for run member function
#define NO_BREAKPOINT       0b00000000
#define CPU_BREAKPOINT      0b00000001
#define SCANLINE_BREAKPOINT 0b00000010

namespace E64
{

enum machine_mode
{
    DEBUG_MODE,
    NORMAL_MODE
};
    
class machine
{
private:
    frequency_divider *m68k_to_vicv;
    frequency_divider *m68k_to_blitter;
    frequency_divider *m68k_to_sid;
    frequency_divider *m68k_to_timer;
    char machine_help_string[2048];
public:
    enum machine_mode   current_mode;
    bool running;
    
    mmu                 *mmu_ic;
    cpu_moira           *m68k_ic;
    TTL74LS148          *TTL74LS148_ic;
    timer               *timer_ic;
    vicv                *vicv_ic;
    blitter             *blitter_ic;
    sids                *sids_ic;
    cia                 *cia_ic;

    machine();
    ~machine();
    
    uint8_t  run(uint16_t no_of_cycles);
    
    void reset();
    
    void switch_to_running();
    void switch_to_debug();
    void switch_mode();
};

}

#endif
