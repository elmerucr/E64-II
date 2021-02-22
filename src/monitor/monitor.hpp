#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "screen.hpp"

namespace E64 {

class monitor_t {
public:
	monitor_t();
	~monitor_t();
	
	screen_t *screen;
};

}

#endif
