//  main.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <iostream>
#include <cstdio>

#include "common.hpp"
#include "sdl2.hpp"
#include "pid_delay.hpp"
#include "vicv.hpp"
#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "debug_status_bar.hpp"
#include "cia.hpp"
#include "sids.hpp"
#include "timer.hpp"

// global components of the system
E64::video host_video;
E64::machine computer;
E64::pid_delay frame_delay(7000.0);

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

    E64::debug_screen_init();
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
                
                /*  Note: using run(0) function causes the cpu to run only
                 *  1 instruction per call. This will increase the overall
                 *  host cpu load, but also increases accuracy of the
                 *  system as a whole. Most importantly, SID emulation will
                 *  be more realistic. Instant changes to SID's registers
                 *  will be reflected in audio output.
                 *  However, run(63) significantly reduces host cpu load,
                 *  once we have some music running in the virtual machine,
                 *  test drive!
                 */
                
                if( computer.run(63) != 0 ) computer.switch_to_debug();
                
                // if full frame was drawn call other update functions:
                if( computer.vicv_ic->frame_done )
                {
                    computer.vicv_ic->frame_done = false;
                    
                    // process events and catch a possible exit signal
                    if(E64::sdl2_process_events() == E64::QUIT_EVENT) computer.running = false;
                    
                    computer.cia_ic->run();
                    
                    host_video.update_screen();
                    
                    frame_delay.run();
                }
                break;
            case E64::DEBUG_MODE:
                if( debug_console_cursor_flash() )
                {
                    debug_status_bar_refresh();
                    debug_console_blit_to_debug_screen();
                    E64::debug_screen_update();
                    host_video.update_screen();
                }
                
                // 10ms is a reasonable delay
                E64::sdl2_delay_10ms();
                
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
                        host_video.update_screen();
                        break;
                }
                break;
        }
    }

    // end of mainloop

    printf("detecting quit event\n");

    // cleanup window management
    E64::sdl2_cleanup();

    printf("closing E64-II\n");

    return 0;
}
