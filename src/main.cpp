//  main.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <iostream>
#include <cstdio>

#include "common.hpp"
#include "sdl2.hpp"
#include "stats.hpp"
#include "vicv.hpp"
#include "monitor_console.hpp"
#include "monitor_screen.hpp"
#include "monitor_status_bar.hpp"
#include "cia.hpp"
#include "sids.hpp"
#include "timer.hpp"
#include <chrono>
#include <thread>

// global components
E64::settings   prefs;
E64::machine    pc;
E64::video      host_video;
E64::stats      statistics;

int main(int argc, char **argv)
{
	printf("E64-II (C)%i by elmerucr - version %i.%i.%i\n",
	       E64_II_YEAR, E64_II_MAJOR_VERSION, E64_II_MINOR_VERSION,
	       E64_II_BUILD);

	// place this call into machine class?
	pc.vicv->set_stats(statistics.stats_info());

	// set up window management, audio and some other stuff
	E64::sdl2_init();

	E64::monitor_screen_init();
	debug_console_init();

	// Select starting mode of E64-II
	pc.switch_to_running();
	//pc.switch_to_debug();

	pc.reset();

	pc.running = true;

	std::chrono::time_point<std::chrono::steady_clock> refresh_time = std::chrono::steady_clock::now();

	statistics.reset();

	while (pc.running) {
		if (pc.current_mode == E64::NORMAL_MODE) {
			if (pc.run(0))
				pc.switch_to_debug();
			if (pc.vicv->frame_done) {
				pc.vicv->frame_done = false;
				if (E64::sdl2_process_events() == E64::QUIT_EVENT)
					pc.running = false;
				statistics.process_parameters();

				/*
				 * If vsync is enabled, the update screen function
				 * takes more time, i.e. it will return after a few
				 * milliseconds, exactly when vertical refresh is
				 * done. This will avoid tearing.
				 * Moreover, there's no need then to let the system sleep
				 * with a calculated value. But we will still have to do
				 * a time measurement for estimation of idle time.
				 */
				statistics.start_idle();

				if (host_video.vsync_disabled()) {
					refresh_time +=
						std::chrono::microseconds(statistics.time_per_frame);

					/*
					 * Check if the next update is in the past,
					 * this can be the result of a debug session.
					 * If so, calculate a new update moment. This will
					 * avoid "playing catch-up" by the virtual machine.
					 */
					if (refresh_time < std::chrono::steady_clock::now())
						refresh_time = std::chrono::steady_clock::now() + std::chrono::microseconds(statistics.time_per_frame);
					std::this_thread::sleep_until(refresh_time);
				}
				host_video.update_screen();
				statistics.done_idle();
			}
		} else {
			if (debug_console_cursor_flash()) {
				debug_status_bar_refresh();
				debug_console_blit_to_debug_screen();
				E64::monitor_screen_update();
				host_video.update_screen();
			}
			std::this_thread::sleep_for(std::chrono::microseconds(10000));

			switch (E64::sdl2_process_events()) {
			case E64::QUIT_EVENT:
				pc.running = false;
				break;
			case E64::NO_EVENT:
				break;
			case E64::KEYPRESS_EVENT:
				debug_status_bar_refresh();
				debug_console_blit_to_debug_screen();
				E64::monitor_screen_update();
				host_video.update_screen();
				break;
			}
		}
	}

	printf("detected quit event\n");
	E64::sdl2_cleanup();
	printf("closing E64-II\n");
	return 0;
}
