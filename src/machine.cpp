//  machine.cpp
//  E64
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "machine.hpp"
#include "sdl2.hpp"
#include "common.hpp"

E64::machine::machine()
{
    // defaults to normal mode, but can be changed later by application
    current_mode = NORMAL_MODE;
    
    mmu_ic = new mmu();
    
    m68k_ic = new cpu_moira();
    
    TTL74LS148_ic = new TTL74LS148();
    
    timer_ic = new timer();
    timer_ic->interrupt_device_no = TTL74LS148_ic->connect_device(4);
    
    vicv_ic = new vicv();
    vicv_ic->interrupt_device_no_vblank = TTL74LS148_ic->connect_device(2);
    
    blitter_ic = new blitter();
    
    sids_ic = new sids(true);
    
    cia_ic = new cia();
    
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
    delete sids_ic;
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
    host_video.update_title();
    // audio starts "automatically" when buffer reaches a minimum size
}

void E64::machine::switch_to_debug()
{
    current_mode = DEBUG_MODE;
    debug_console_cursor_activate();
    host_video.update_title();
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

uint8_t E64::machine::run(uint16_t no_of_cycles)
{
    // default exit_code of the function is 0, no breakpoints have occurred
    uint8_t output_state = NO_BREAKPOINT;
    
    // run cycles on the cpu and check for breakpoints
    unsigned int processed_cycles = (unsigned int)computer.m68k_ic->run(no_of_cycles);
    if( m68k_ic->breakpoint_reached )
    {
        // cpu breakpoint encountered
        snprintf(machine_help_string, 256, "cpu breakpoint occurred at $%06x\n", m68k_ic->getPC());
        debug_console_print(machine_help_string);
        m68k_ic->breakpoint_reached = false;
        output_state |= CPU_BREAKPOINT;
    }
    
    // run cycles on vicv and check for breakpoints
    vicv_ic->run(m68k_to_vicv->clock(processed_cycles));
    if( vicv_ic->breakpoint_reached )
    {
        snprintf(machine_help_string, 256, "scanline breakpoint occurred at line %i\n", vicv_ic->get_current_scanline());
        debug_console_print(machine_help_string);
        vicv_ic->breakpoint_reached = false;
        output_state |= SCANLINE_BREAKPOINT;
    }
    
    // run cycles on blitter
    blitter_ic->run(m68k_to_blitter->clock(processed_cycles));
    
    // run cycles on timer
    timer_ic->run(m68k_to_timer->clock(processed_cycles));
    
    // run cycles on sound device & start audio if buffer is large enough
    // some cheating by adjustment of cycles to run depending on current
    // audio queue size
    unsigned int audio_queue_size = E64::sdl2_get_queued_audio_size();
    
    if(audio_queue_size < 0.9 * AUDIO_BUFFER_SIZE)
    {
        sids_ic->run(m68k_to_sid->clock(1.2 * processed_cycles));
    }
    else if(audio_queue_size > 1.1 * AUDIO_BUFFER_SIZE)
    {
        sids_ic->run(m68k_to_sid->clock(0.8 * processed_cycles));
    }
    else
    {
        sids_ic->run(m68k_to_sid->clock(processed_cycles));
    }
    
    if(audio_queue_size > (AUDIO_BUFFER_SIZE/2)) E64::sdl2_start_audio();
    
    return output_state;
}

void E64::machine::reset()
{
    host_video.reset();
    mmu_ic->reset();
    m68k_ic->reset();
    sids_ic->reset();
    vicv_ic->reset();
    blitter_ic->reset();
    timer_ic->reset();
    TTL74LS148_ic->update_interrupt_level();
}
