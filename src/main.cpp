//  main.cpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#include <cstdio>
#include <chrono>
#include <thread>

#include "common.hpp"
#include "host.hpp"
#include "sdl2.hpp"
#include "stats.hpp"
#include "vicv.hpp"
#include "monitor_console.hpp"
#include "monitor_screen.hpp"
#include "monitor_status_bar.hpp"
#include "cia.hpp"
#include "sids.hpp"
#include "timer.hpp"

#include "lua.hpp"

// global components
E64::host_t	host;
E64::machine    pc;
E64::stats      statistics;

std::chrono::time_point<std::chrono::steady_clock> refresh_time;

static void run_loop()
{
	if (pc.run(63)) pc.switch_mode(E64::MONITOR);
	if (pc.vicv->frame_done) {
		pc.vicv->frame_done = false;
		if (E64::sdl2_process_events() == E64::QUIT_EVENT)
			pc.on = false;
		statistics.process_parameters();
		/*
		 * If vsync is enabled, the update screen function takes more
		 * time, i.e. it will return after a few milliseconds, exactly
		 * when vertical refresh is done. This will avoid tearing.
		 * Moreover, there's no need then to let the system sleep with a
		 * calculated value. But we will still have to do a time
		 * measurement for estimation of idle time.
		 */
		statistics.start_idle_time();
		if (host.video.vsync_disabled()) {
			refresh_time +=
				std::chrono::microseconds(statistics.frametime);
			/*
			 * Check if the next update is in the past,
			 * this can be the result of a debug session.
			 * If so, calculate a new update moment. This will
			 * avoid "playing catch-up" by the virtual machine.
			 */
			if (refresh_time < std::chrono::steady_clock::now())
				refresh_time = std::chrono::steady_clock::now() +
					std::chrono::microseconds(statistics.frametime);
			std::this_thread::sleep_until(refresh_time);
		}
		host.video.update_screen();
		statistics.end_idle_time();
	}
}

static void monitor_loop()
{
	if (debug_console_cursor_flash()) {
		debug_status_bar_refresh();
		debug_console_blit_to_debug_screen();
		E64::monitor_screen_update();
		host.video.update_screen();
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000));

	switch (E64::sdl2_process_events()) {
	case E64::QUIT_EVENT:
		pc.on = false;
		break;
	case E64::NO_EVENT:
		break;
	case E64::KEYPRESS_EVENT:
		debug_status_bar_refresh();
		debug_console_blit_to_debug_screen();
		E64::monitor_screen_update();
		host.video.update_screen();
		break;
	}
}

int main(int argc, char **argv)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	debug_console_init();
	// call inits to global components? messages can be in console

	// place this call into machine class?
	pc.vicv->set_stats(statistics.stats_info());

	// set up window management, audio and some other stuff
	E64::sdl2_init();

	E64::monitor_screen_init();

	// Select starting mode of E64-II
	pc.switch_mode(E64::RUNNING);
	pc.reset();
	pc.on = true;

	refresh_time = std::chrono::steady_clock::now();

	statistics.reset();

	while (pc.on) {
		switch (pc.mode) {
			case E64::RUNNING:
				run_loop();
				break;
			case E64::MONITOR:
				monitor_loop();
				break;
		}
	}

	printf("detected quit event\n");
	E64::sdl2_cleanup();
	
	lua_close(L);
	return 0;
}
