/*
 * fd.hpp
 * E64-II
 *
 * Copyright © 2020 elmerucr. All rights reserved.
 */

/*
 * Register 0 - FD Status Register (READ ONLY)
 *
 * 7 6 5 4 3 2 1 0
 * |       | | | |
 * |       | | | +-- Empty (0) / Disk inside (1)
 * |       | | +---- Motor idle (0) / Motor spinning (1)
 * |       | +------ Reading: Idle (0) / Busy (1)
 * |       +-------- Writing: Idle (0) / Busy (1)
 * +---------------- Error (1)
 *
 * bits 4-6: Reserved
 *
 *
 * Register 1 - FD Control Register
 *
 * 7 6 5 4 3 2 1 0
 *             | |
 *             | +-- Data direction: Reading (0) / Writing (1)
 *             +---- Start operation
 *
 * Registers 2 - 5: Form a sector number (big endian) for read/write operation.
 *
 * Registers 6 - 9: Form an address (big endian) for read/write operation.
 *                  Uneven address will be truncated to even address.
 *
 */

#ifndef FD_HPP
#define FD_HPP

#define	FD_ERROR_DISK_INSIDE	0b00000001
#define FD_ERROR_WRONG_PATH	0b00000010
#define FD_ERROR_IS_DIRECTORY	0b00000100
#define FD_ERROR_WRONG_SIZE	0b00001000
#define FD_ERROR_EXTENSION	0b00010000

namespace E64
{

enum fd_state {
	FD_IDLE,
	FD_READING,
	FD_WRITING
};

enum motor_state {
	MOTOR_IDLE,
	MOTOR_SPINNING_UP,
	MOTOR_SPINNING
};

class fd {
private:
	bool		disk_attached;
	bool		write_protect;
	
	FILE		*current_disk;
	
	enum fd_state	current_fd_state;
	enum motor_state	current_motor_state;
	
	uint32_t	spin_up_cycles;
	uint32_t	spin_delay_cycles;
	uint32_t	cycle_counter;
	
public:
	fd();
	~fd();
	
	uint8_t	*disk_contents;
	
	int	insert_disk(const char *path, bool write_protect_disk);
	int	eject_disk();
	
	void	reset();
	uint8_t	read_byte(uint8_t address);
	void	write_byte(uint8_t address, uint8_t byte);
	void	run(uint32_t number_of_cycles);
	
	uint16_t bytes_per_sector();
	uint32_t disk_size();
};

}

#endif
