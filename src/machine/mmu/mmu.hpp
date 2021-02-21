/*
 * mmu.hpp
 * E64-II
 *
 * Copyright © 2019-2020 elmerucr. All rights reserved.
*/

#ifndef MMU_HPP
#define MMU_HPP

#include <cstdint>

#define IO_FD0_PAGE		0xfb00
#define IO_CIA_PAGE		0xfb03
#define IO_VICV_PAGE		0xfb04
#define IO_SND_PAGE		0xfb05
#define IO_TIMER_PAGE		0xfb06
#define IO_BLITTER_PAGE		0xfb07
#define IO_FONT			0xfb8000
#define IO_KERNEL		0xfc
#define IO_RESET_VECTOR_MASK	0xfffff8

namespace E64
{

class mmu_ic
{
public:
	mmu_ic();
	~mmu_ic();
	
	uint8_t  *ram;          // make this private and work with friend class?
	uint16_t *ram_as_words; // make this private and work with friend class?
	uint8_t  current_kernel_image[262144];
	
	void reset();
	
	unsigned int read_memory_8(unsigned int address);
	unsigned int read_memory_16(unsigned int address);
	
	unsigned int read_disassembler_8(unsigned int address);
	unsigned int read_disassembler_16(unsigned int address);
	
	void write_memory_8(unsigned int address, unsigned int value);
	void write_memory_16(unsigned int address, unsigned int value);
	
	void find_and_update_kernel_image();
};

}

#endif
