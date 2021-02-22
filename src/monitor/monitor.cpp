#include "monitor.hpp"

E64::monitor_t::monitor_t()
{
	screen = new screen_t();
}

E64::monitor_t::~monitor_t()
{
	delete screen;
}
