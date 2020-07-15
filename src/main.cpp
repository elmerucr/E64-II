//  main.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <iostream>
#include <cstdio>

#include "common.hpp"
#include "sdl2.hpp"
#include "stats.hpp"
#include "vicv.hpp"
#include "debug_console.hpp"
#include "debug_screen.hpp"
#include "debug_status_bar.hpp"
#include "cia.hpp"
#include "sids.hpp"
#include "timer.hpp"
#include <chrono>
#include <thread>

// global components
E64::machine    pc;
E64::video      host_video;
E64::stats      statistics;

int main(int argc, char **argv)
{
    //    std::cout << std::endl << "Command line arguments:" << std::endl;
    //    for(int i=0; i<argc ; i++)
    //    {
    //        std::cout << argv[i] << std::endl;
    //    }
    //    std::cout << std::endl;
    
    printf("E64-II (C)%i by elmerucr - version %i.%i.%i\n", E64_II_YEAR, E64_II_MAJOR_VERSION, E64_II_MINOR_VERSION, E64_II_BUILD);

    // place this call into machine class?
    pc.vicv_ic->set_stats( statistics.stats_info() );
    
    // set up window management, audio and some other stuff
    E64::sdl2_init();

    E64::debug_screen_init();
    debug_console_init();

    // Select starting mode of E64-II
    pc.switch_to_running();
    //pc.switch_to_debug();
    
    pc.reset();

    // start of main loop
    pc.running = true;
    
    std::chrono::time_point<std::chrono::steady_clock> screen_update_moment = std::chrono::steady_clock::now();
    
    statistics.reset_measurement();
    
    while(pc.running)
    {
        switch(pc.current_mode)
        {
            case E64::NORMAL_MODE:
                
                /*  Note: using run(0) function causes the cpu to run only
                 *  1 instruction per call. This will increase the overall
                 *  host cpu load, but also increases accuracy of the
                 *  system as a whole. Most importantly, SID emulation will
                 *  be very realistic. Instant changes to SID's registers
                 *  might be reflected in audio output.
                 *  However, run(63) significantly reduces host cpu load,
                 *  once we have some music running in the virtual machine,
                 *  test this.
                 */
                
                if( pc.run(511) != 0 ) pc.switch_to_debug();
                
                // if a full frame is done, call other update functions:
                if( pc.vicv_ic->frame_done )
                {
                    pc.vicv_ic->frame_done = false;
                    
                    // process events and check for possible exit signal
                    if(E64::sdl2_process_events() == E64::QUIT_EVENT) pc.running = false;
                    
                    statistics.process_parameters();
                    
                    /*  If vsync is enabled, the update screen function
                     *  takes more time, i.e. it will return after a few
                     *  milliseconds, exactly when vertical refresh is
                     *  done. This will avoid tearing.
                     *
                     *  Moreover, there's no need then to let the system sleep
                     *  with a calculated value. But we will still have to do
                     *  a time measurement for estimation of idle time.
                     */
                    
                    statistics.start_idle();
                    
                    if( host_video.vsync_disabled() )
                    {
                        
                        screen_update_moment += std::chrono::microseconds(statistics.nominal_time_per_frame);
                        
                        /*  Check if the next update lies in the past,
                         *  this can be a result of a debug session.
                         *
                         *  If so, calculate a new update moment. This will
                         *  avoid "playing catch-up" by the virtual machine.
                         */

                        if( screen_update_moment < std::chrono::steady_clock::now() ) screen_update_moment = std::chrono::steady_clock::now() + std::chrono::microseconds(statistics.nominal_time_per_frame);
                        //if( (std::chrono::steady_clock::now() - screen_update_moment) > std::chrono::microseconds(250000) ) screen_update_moment = std::chrono::steady_clock::now() + std::chrono::microseconds(statistics.nominal_time_per_frame);
                        
                        std::this_thread::sleep_until(screen_update_moment);
                        
                        /*  general info about sleep function / chrono / time:
                         *  c++11 portable version of usleep():
                         *  see: https://gist.github.com/ngryman/6482577
                         *  and: https://en.cppreference.com/w/cpp/chrono
                         */
                    }
                    
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
                std::this_thread::sleep_for(std::chrono::microseconds(10000));
                
                switch( E64::sdl2_process_events() )
                {
                    case E64::QUIT_EVENT:
                        pc.running = false;
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
