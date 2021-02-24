#include "screen.hpp"
#include "tty.hpp"
#include "status_bar.hpp"
#include "command.hpp"

#ifndef MONITOR_HPP
#define MONITOR_HPP

namespace E64 {

class monitor_t {
public:
	monitor_t();
	~monitor_t();
	
	screen_t *screen;
	tty_t *tty;
	status_bar_t *status_bar;
	command_t *command;
};

}

#endif
