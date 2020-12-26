//  machine.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include "monitor_console.hpp"
#include "monitor_screen.hpp"
#include "machine.hpp"
#include "sdl2.hpp"
#include "common.hpp"

E64::machine::machine()
{
	// defaults to normal mode, but can be changed later by application
	current_mode = NORMAL_MODE;
	
	mmu = new mmu_ic();
	
	m68k = new cpu_moira();
	
	TTL74LS148 = new TTL74LS148_ic();
	
	timer = new timer_ic();
	timer->interrupt_device_number = TTL74LS148->connect_device(4);
	
	vicv = new vicv_ic();
	vicv->vblank_interrupt_device_number = TTL74LS148->connect_device(2);
	
	blitter = new blitter_ic();
	
	sids = new sids_ic();
	
	cia = new cia_ic();
	
	fd0 = new fd();
	
	// init frequency dividers (make sure the right amount of cycles will run on different ic's)
	m68k_to_vicv  = new frequency_divider(CPU_CLOCK_SPEED, VICV_DOT_CLOCK_SPEED);
	m68k_to_blitter = new frequency_divider(CPU_CLOCK_SPEED, BLITTER_DOT_CLOCK_SPEED);
	m68k_to_sid   = new frequency_divider(CPU_CLOCK_SPEED, SID_CLOCK_SPEED );
//	m68k_to_timer = new frequency_divider(CPU_CLOCK_SPEED, CPU_CLOCK_SPEED );
//	m68k_to_cia = new frequency_divider(CPU_CLOCK_SPEED, CPU_CLOCK_SPEED );
//	m68k_to_fd0 = new frequency_divider(CPU_CLOCK_SPEED, CPU_CLOCK_SPEED );
	
	m68k->configDasm(true, false);   // output numbers in hex, use small case for mnemonics
}

E64::machine::~machine()
{
	delete m68k_to_sid;
	delete m68k_to_blitter;
	delete m68k_to_vicv;
	
	delete fd0;
	delete cia;
	delete sids;
	delete blitter;
	delete vicv;
	delete timer;
	delete TTL74LS148;
	delete m68k;
	delete mmu;
}

void E64::machine::switch_to_running()
{
	current_mode = NORMAL_MODE;
	debug_console_cursor_deactivate();
	host_video.update_title();
	// audio starts automatically when buffer reaches a minimum size
}

void E64::machine::switch_to_debug()
{
	current_mode = MONITOR_MODE;
	debug_console_cursor_activate();
	host_video.update_title();
	E64::sdl2_stop_audio();
}

void E64::machine::switch_mode()
{
	switch (current_mode) {
	case NORMAL_MODE:
		switch_to_debug();
		break;
        case MONITOR_MODE:
		switch_to_running();
		break;
	}
}

/*
 * Note: using run(0) function causes the cpu to run only 1 instruction per
 * call. This will increase the overall host cpu load, but also increases
 * accuracy of the system as a whole. Most importantly, SID and VICV emulation
 * will be very realistic. Instant changes to registers should be reflected in
 * audio output.
 * However, run(63) significantly reduces host cpu load, once we have some music
 * running in the virtual machine, test this.
 */
uint8_t E64::machine::run(uint16_t no_of_cycles)
{
	// default exit_code of the function is 0, no breakpoints have occurred
	uint8_t output_state = NO_BREAKPOINT;
	
	// run cycles on the cpu and check for breakpoints
	unsigned int processed_cycles = (unsigned int)pc.m68k->run(no_of_cycles);
	if (m68k->breakpoint_reached) {
		snprintf(machine_help_string, 256,
			 "cpu breakpoint occurred at $%06x\n", m68k->getPC());
		debug_console_print(machine_help_string);
		m68k->breakpoint_reached = false;
		output_state |= CPU_BREAKPOINT;
	}
	
	// run cycles on vicv and check for breakpoints
	vicv->run(m68k_to_vicv->clock(processed_cycles));
	if (vicv->breakpoint_reached) {
		snprintf(machine_help_string, 256, "scanline breakpoint occurred at line %i\n", vicv->get_current_scanline());
		debug_console_print(machine_help_string);
		vicv->breakpoint_reached = false;
		output_state |= SCANLINE_BREAKPOINT;
	}
    
	// run cycles on blitter
	blitter->run(m68k_to_blitter->clock(processed_cycles));
	
	// run cycles on timer
	timer->run(processed_cycles);
	
	// run cycles on cia
	cia->run(processed_cycles);
	
	// run cycles on sound device & start audio if buffer is large enough
	// some cheating by adjustment of cycles to run depending on current
	// audio queue size
	unsigned int audio_queue_size = statistics.get_current_audio_queue_size();
	
	if (audio_queue_size < 0.9 * AUDIO_BUFFER_SIZE) {
		sids->run(m68k_to_sid->clock(1.2 * processed_cycles));
	} else if (audio_queue_size > 1.1 * AUDIO_BUFFER_SIZE) {
		sids->run(m68k_to_sid->clock(0.8 * processed_cycles));
	} else {
		sids->run(m68k_to_sid->clock(processed_cycles));
	}
	
	if (audio_queue_size > (AUDIO_BUFFER_SIZE/2))
		E64::sdl2_start_audio();
	
	return output_state;
}

void E64::machine::reset()
{
	host_video.reset();
	mmu->reset();
	m68k->reset();
	sids->reset();
	vicv->reset();
	blitter->reset();    // sometimes, when resetting there's the warning message blitter not finished
	timer->reset();
	cia->reset();
	fd0->reset();
	TTL74LS148->update_interrupt_level();
	printf("[machine] system reset\n");
}
