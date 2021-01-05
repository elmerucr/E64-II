/*
 * fd.hpp
 * E64-II
 *
 * Copyright © 2020-2021 elmerucr. All rights reserved.
 */

/*
 * Register 0 - FD Status Register (READ ONLY)
 *
 * 7 6 5 4 3 2 1 0
 * |       | | | |
 * |       | | | +-- Empty (0) / Disk inside (1)
 * |       | | +---- Motor idle (0) / Motor spinning (1)
 * |       | +------ Reading: idle (0) / busy (1)
 * |       +-------- Writing: idle (0) / Busy (1)
 * +---------------- Error (1)
 *
 * bits 4-6: Reserved
 *
 *
 * Register 1 - FD Control Register
 *
 * 7 6 5 4 3 2 1 0
 * |           | |
 * |           | +-- Data direction: Reading (0) / Writing (1)
 * |           +---- Start operation
 * +---------------- Reset (and do not eject disk)
 *
 * Registers 2 - 5: Form a sector number (big endian) for read/write operation.
 *
 * Registers 6 - 9: Form an address (big endian) for read/write operation.
 *                  Uneven address will be truncated to even address.
 *
 */

#ifndef FD_HPP
#define FD_HPP

#define	FD_HOST_ERROR_DISK_INSIDE	0b00000001
#define FD_HOST_ERROR_WRONG_PATH	0b00000010
#define FD_HOST_ERROR_IS_DIRECTORY	0b00000100
#define FD_HOST_ERROR_WRONG_SIZE	0b00001000
#define FD_HOST_ERROR_EXTENSION		0b00010000

namespace E64
{

enum fd_state_list {
	FD_STATE_EMPTY,
	FD_STATE_DISK_LOADED,
	FD_STATE_SPINNING_UP,
	FD_STATE_READING,
	FD_STATE_WRITING,
	FD_STATE_SPINNING
};

enum fd_error_state_list {
	FD_ERROR_NONE = 0,
	FD_ERROR_NO_DISK_INSIDE,
	FD_ERROR_MOTOR_IS_SPINNING,
	FD_ERROR_READING_PLANNED,
	FD_ERROR_READING,
	FD_ERROR_WRITING_PLANNED,
	FD_ERROR_WRITING,
	FD_ERROR_WRONG_SECTOR
};

class fd {
private:
	bool write_protect;
	FILE *current_disk;
	
	uint8_t registers[16];
	
	enum fd_error_state_list current_error_state;
	bool error_led_on;
	uint32_t error_led_cycles;
	uint32_t error_led_counter;
	
	enum fd_state_list fd_state;
	
	uint32_t spin_up_cycles;
	uint32_t spin_delay_cycles;
	uint32_t cycle_counter;
	
	// finite state machine parameters
	uint32_t sector_number;
	uint32_t memory_address;
	uint32_t byte_count;		// used for read/write counting
	
	enum fd_state_list next_state;
	
	void start_error_state(enum fd_error_state_list new_error);
	void reset_error_state();
	
	void attempt_start_reading();
	void attempt_start_writing();
	
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
	
	int	read_error_state() { return current_error_state; }
	
	uint16_t bytes_per_sector();
	uint32_t disk_size();
	uint16_t *icon_data();
	
	inline bool disk_inside() { return fd_state != FD_STATE_EMPTY; }
	inline bool motor_spinning() { return (fd_state != FD_STATE_EMPTY) &&
		(fd_state != FD_STATE_DISK_LOADED); }
	inline bool reading() { return fd_state == FD_STATE_READING; }
	inline bool writing() { return fd_state == FD_STATE_WRITING; }
	inline bool in_error() { return current_error_state != FD_ERROR_NONE; }
};

}

#endif
