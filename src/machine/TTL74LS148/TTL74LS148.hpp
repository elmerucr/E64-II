//  TTL74LS148.hpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.
//
// exception collector and
// priority encoder
// 8 input lines, 3 bit output

#ifndef TTL74LS148_HPP
#define TTL74LS148_HPP

#include <cstdint>

namespace E64
{

class TTL74LS148_ic
{
private:
	struct device {
		bool state;
		int level;
	};
	struct device devices[256];
	uint8_t number_of_devices;
	int output_level;
public:
	TTL74LS148_ic();
	
	void pull_line(uint8_t handler);
	void release_line(uint8_t handler);
	
	/* Recalculates interrupt level based on individual connections */
	void update_interrupt_level();
	
	/*
	 * When connecting a device, both a pointer to a pin and an interrupt
	 * level (1-6) must be supplied. Returns a unique interrupt_device_no
	 */
	uint8_t connect_device(int level);
};

}

#endif
