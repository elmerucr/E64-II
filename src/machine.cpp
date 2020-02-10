//  machine.cpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "debug_console.hpp"
#include "machine.hpp"
#include "sdl2.hpp"
#include "common_defs.hpp"

E64::machine::machine()
{
    // defaults to normal mode, but can be changed later by application
    current_mode = NORMAL_MODE;
    
    mmu_ic = new mmu();
    
    m68k_ic = new cpu_moira();
    
    TTL74LS148_ic = new TTL74LS148();
    
    timer_ic = new timer();
    TTL74LS148_ic->connect_device(&timer_ic->irq_pin, 2);
    
    vicv_ic = new vicv();
    
    blitter_ic = new blitter();
    
    sound_ic = new sound(true);
    
    cia_ic = new cia();
    
    debugger_irq_pin = true;
    TTL74LS148_ic->connect_device(&debugger_irq_pin, 5);
    
    // init frequency dividers (make sure the right amount of cycles will run on different ic's)
    m68k_to_vicv  = new frequency_divider(CPU_CLOCK_SPEED, VICV_DOT_CLOCK_SPEED);
    m68k_to_blitter = new frequency_divider(CPU_CLOCK_SPEED, BLITTER_DOT_CLOCK_SPEED);
    m68k_to_sid   = new frequency_divider(CPU_CLOCK_SPEED, SID_CLOCK_SPEED );
    m68k_to_timer = new frequency_divider(CPU_CLOCK_SPEED, CPU_CLOCK_SPEED );
    
    m68k_ic->configDasm(true, false);   // output numbers in hex, use small case for mnemonics
}

E64::machine::~machine()
{
    delete m68k_to_timer;
    delete m68k_to_sid;
    delete m68k_to_blitter;
    delete m68k_to_vicv;
    
    delete cia_ic;
    delete sound_ic;
    delete blitter_ic;
    delete vicv_ic;
    delete timer_ic;
    delete TTL74LS148_ic;
    delete m68k_ic;
    delete mmu_ic;
}

void E64::machine::switch_to_running()
{
    current_mode = NORMAL_MODE;
    debug_console_cursor_deactivate();
    E64::sdl2_update_title();
    // audio starts "automatically" when buffer reaches a minimum size
}

void E64::machine::switch_to_debug()
{
    current_mode = DEBUG_MODE;
    debug_console_cursor_activate();
    E64::sdl2_update_title();
    E64::sdl2_stop_audio();
}

void E64::machine::switch_mode()
{
    switch(current_mode)
    {
        case NORMAL_MODE:
            switch_to_debug();
            break;
        case DEBUG_MODE:
            switch_to_running();
            break;
    }
}

//void E64::machine::force_next_instruction()
//{
//    cpu_ic->force_next_instruction();
//}

int E64::machine::run(uint16_t no_of_cycles)
{
    // default exit_code of the function is 0, no breakpoints have occurred
    int exit_code = NOTHING;
    
    unsigned int processed_cycles = (unsigned int)computer.m68k_ic->run(no_of_cycles);
    if( m68k_ic->breakpoint_reached )
    {
        // cpu breakpoint encountered
        snprintf(machine_help_string, 256, "cpu breakpoint occurred at $%06x\n", m68k_ic->getPC());
        debug_console_print(machine_help_string);
        m68k_ic->breakpoint_reached = false;
        exit_code = CPU_BREAKPOINT;
    }
    // run cycles on vicv
    vicv_ic->run(m68k_to_vicv->clock(processed_cycles));
    // run cycles on blitter
    blitter_ic->run(m68k_to_blitter->clock(processed_cycles));
    // run cycles on timer
    timer_ic->run(m68k_to_timer->clock(processed_cycles));
    // calculate no. of cycles to run on sound device & start audio if buffer large enough
    sound_ic->run(m68k_to_sid->clock(processed_cycles));
    if(E64::sdl2_get_queued_audio_size() > (AUDIO_BUFFER_SIZE/2)) E64::sdl2_start_audio();
    
    TTL74LS148_ic->update_interrupt_level();
    
    return exit_code;
}

void E64::machine::reset()
{
    m68k_ic->reset();
    TTL74LS148_ic->update_interrupt_level();
    sound_ic->reset();
    vicv_ic->reset();
}

bool E64::machine::toggle_debugger_irq_pin()
{
    debugger_irq_pin = !debugger_irq_pin;
    return debugger_irq_pin;
}
