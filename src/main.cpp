//  main.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <iostream>
#include <cstdio>

#include "common.hpp"
#include "sdl2.hpp"
#include "stats.hpp"
#include "delay.hpp"
#include "vicv.hpp"
#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "debug_status_bar.hpp"
#include "cia.hpp"
#include "sids.hpp"
#include "timer.hpp"

// global component host video
E64::video host_video;

// global component the machine
E64::machine computer;

// global component stats (keeps track of system performance parameters)
E64::stats  statistics;


int main(int argc, char **argv)
{
    //    std::cout << std::endl << "Command line arguments:" << std::endl;
    //    for(int i=0; i<argc ; i++)
    //    {
    //        std::cout << argv[i] << std::endl;
    //    }
    //    std::cout << std::endl;
    
    printf("E64-II (C)%i by elmerucr - version %i.%i.%i\n", E64_YEAR, E64_MAJOR_VERSION, E64_MINOR_VERSION, E64_BUILD);

    // delay (with PID controller, constructed with the assumption of 50% CPU usage on start)
    E64::delay frame_delay((1000000/FPS)/2);
    computer.vicv_ic->set_stats(frame_delay.stats_info());
    
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
                
                // if a full frame is done, call other update functions:
                if( computer.vicv_ic->frame_done )
                {
                    computer.vicv_ic->frame_done = false;
                    
                    // process events and catch a possible exit signal
                    if(E64::sdl2_process_events() == E64::QUIT_EVENT) computer.running = false;
                    
                    computer.cia_ic->run();
                    
                    /*  Next function starts with a time measurement that
                     *  is used for the calculation of the average frametime
                     *  and FPS.
                     *  Also, when vsync is not enabled the system uses a
                     *  delay (sleep) function to reasonably approach a stable
                     *  FPS. It is possible to calculate CPU usage by
                     *  performing the next calculation:
                     *
                     *  %CPU = (nominal_frame_time - delay) / nominal_frame_time
                     */
                    
                    statistics.process_parameters();
                    frame_delay.process();
                    
                    /*  If we have vsync enabled, the update screen function
                     *  will take more time, i.e. it will return after a few
                     *  milliseconds, exactly when vertical refresh can be
                     *  done. This will avoid tearing.
                     *  Moreover, there's no need to let the system delay
                     *  with a calculated value. But we will have to do
                     *  a time measurement for estimation of %CPU usage.
                     *
                     *  Of course, if there's no vsync, we don't have to do
                     *  a time measurement, but we have to let the system
                     *  sleep the calculated delay.
                     */
                    
                    statistics.start_idle();
                    
                    if( host_video.vsync_disabled() ) frame_delay.sleep();
                    
                    host_video.update_screen();
                    
                    statistics.done_idle();
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
