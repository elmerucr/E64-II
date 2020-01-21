//  main.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <iostream>
#include <cstdio>

#include "common_defs.hpp"
#include "sdl2.hpp"
#include "pid_delay.hpp"
#include "vicv.hpp"
#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "debug_status_bar_E64_II.hpp"
#include "cia.hpp"
#include "sound.hpp"
#include "timer.hpp"

// global components of the system
E64::machine computer;
E64::pid_delay frame_delay(12800.0);

int main(int argc, char **argv)
{
    //    std::cout << std::endl << "Command line arguments:" << std::endl;
    //    for(int i=0; i<argc ; i++)
    //    {
    //        std::cout << argv[i] << std::endl;
    //    }
    //    std::cout << std::endl;
    
    printf("E64-II (C)%i by elmerucr - version %i.%i.%i\n", E64_YEAR, E64_MAJOR_VERSION, E64_MINOR_VERSION, E64_BUILD);

    // set up window management, audio and some other stuff
    E64::sdl2_init();

    debug_console_init();

    // Select starting mode of E64-II
    //computer.switch_to_running();
    computer.switch_to_debug();
    
    computer.reset();

    // start of main loop
    computer.running = true;
    
    while(computer.running)
    {
        switch(computer.current_mode)
        {
            case E64::NORMAL_MODE:
                switch( computer.run(64) )
                {
                    case E64::NOTHING:
                        break;
                    case E64::CPU_BREAKPOINT:
                        computer.switch_to_debug();
                        break;
                }
                computer.TTL74LS148_ic->update_interrupt_level();
                // if full frame was drawn call other update functions:
                if(computer.vicv_ic->frame_done == true)
                {
                    computer.vicv_ic->frame_done = false;
                    // process events and catch a possible exit signal
                    if(E64::sdl2_process_events() == E64::QUIT_EVENT) computer.running = false;
                    // updates at 50Hz for cia is ok, it's connected to user input
                    computer.cia_ic->run();
                    E64::sdl2_update_screen();
                    frame_delay.run();
                }
                break;
            case E64::DEBUG_MODE:
                // 10ms is a reasonable delay
                E64::sdl2_delay_10ms();
                if( debug_console_cursor_flash() )
                {
                    debug_status_bar_refresh();
                    debug_console_blit_to_debug_screen();
                    E64::debug_screen_update();
                    E64::sdl2_update_screen();
                }
                switch( E64::sdl2_process_events() )
                {
                    case E64::QUIT_EVENT:
                        computer.running = false;
                        break;
                    case E64::NO_EVENT:
                        // nothing happened, so do nothing
                        break;
                    case E64::KEYPRESS_EVENT:
                        debug_status_bar_refresh();
                        debug_console_blit_to_debug_screen();
                        E64::debug_screen_update();
                        E64::sdl2_update_screen();
                        break;
                }
                break;
        }
    }

    // end of mainloop

    printf("detected quit event\n");

    // cleanup window management
    E64::sdl2_cleanup();

    printf("closing E64-II\n");

    return 0;
}
