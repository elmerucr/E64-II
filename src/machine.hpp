//  machine.hpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "cia.hpp"
#include "cpu_moira.hpp"
#include "frequency_divider.hpp"
#include "mmu_E64_II.hpp"
#include "sound.hpp"
#include "timer.hpp"
#include "TTL74LS148.hpp"
#include "vicv.hpp"

namespace E64
{
    enum machine_mode
    {
        DEBUG_MODE,
        NORMAL_MODE
    };
    
    enum output_states
    {
        NOTHING = 0,
        CPU_BREAKPOINT = 1
    };
    
    class machine
    {
    private:
        frequency_divider *m68k_to_vicv;
        frequency_divider *m68k_to_sid;
        frequency_divider *m68k_to_timer;
        bool debugger_irq_pin;
        char machine_help_string[2048];
    public:
        enum machine_mode   current_mode;
        bool running;
        
        mmu                 *mmu_ic;
        cpu_moira           *m68k_ic;
        TTL74LS148          *TTL74LS148_ic;
        timer               *timer_ic;
        vicv                *vicv_ic;
        sound               *sound_ic;
        cia                 *cia_ic;

        machine();
        ~machine();
        
//        void force_next_instruction();
        int  run(uint16_t no_of_cycles);
        
        bool toggle_debugger_irq_pin();
        
        void reset();
        
        void switch_to_running();
        void switch_to_debug();
        void switch_mode();
    };
}

#endif