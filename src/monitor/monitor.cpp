#include "monitor.hpp"

E64::monitor_t::monitor_t()
{
	screen = new screen_t();
	tty = new tty_t();
	status_bar = new status_bar_t();
	command = new command_t();
}

E64::monitor_t::~monitor_t()
{
	delete screen;
	delete tty;
	delete status_bar;
	delete command;
}
