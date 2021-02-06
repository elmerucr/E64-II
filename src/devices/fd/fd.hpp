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
 * Register 1 - FD Control Register (WRITE ONLY)
 *
 * 7 6 5 4 3 2 1 0
 * |       | |
 * |       | +------ Start reading (1)
 * |       +-------- Start writing (1)
 * +---------------- Reset error state
 *
 * Register 2 - Error status of drive (READ ONLY)
 * --> Enumerated list
 *
 * Register 3 - Reserved for future IRQ status etc...
 * --> Probably an enumerated list
 *
 * Registers 4 - 7: Form a sector number (big endian) for read/write operations
 *
 * Registers 8 - 11: Form an address (big endian) for read/write operations.
 *                   Uneven addresses will be truncated to even addresses.
 *
 */

#ifndef FD_HPP
#define FD_HPP

#define FD_SIDES		2
#define FD_TRACKS		80
#define FD_SECTORS_PER_TRACK	18
#define FD_SECTORS		FD_SIDES*FD_TRACKS*FD_SECTORS_PER_TRACK
#define FD_BYTES_PER_SECTOR	512
#define	FD_DISK_SIZE		FD_SECTORS*FD_BYTES_PER_SECTOR

#define FD_CYCLES_PER_BYTE	1024
#define FD_SPIN_UP_TIME_MS	300
#define FD_SPIN_DELAY_MS	2000
#define FD_ERROR_LED_TIME_MS	250
#define FD_TRACK_CHANGE_TIME_MS	100

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

enum fd_error_list {
	FD_ERROR_NONE = 0,
	FD_ERROR_NO_DISK_INSIDE,
	FD_ERROR_MOTOR_IS_SPINNING,
	FD_ERROR_READING_PLANNED,
	FD_ERROR_READING,
	FD_ERROR_WRITING_PLANNED,
	FD_ERROR_WRITING,
	FD_ERROR_ILLEGAL_SECTOR,
	FD_ERROR_WRITE_PROTECT
};

class fd {
private:
	/*
	 * write_protect is equal to the switch that could be found on real
	 * floppy disks. An attempt to write on the disk will result in a
	 * hardware error if the disk is write-protected.
	 */
	bool write_protect;
	
	/*
	 * save_on_eject works separately from write_protect and makes it
	 * possible to insert a disk writeable (having all functionality), but
	 * not changing its contents permanently after ejecting the disk
	 */
	bool save_on_eject;
	
	FILE *current_disk;
	
	uint8_t registers[16];
	
	enum fd_error_list current_error_state;
	bool error_led_on;
	
	/*
	 * Number of cycles led is on or off when fd is in an error state. This
	 * number is calculated from the amount of milliseconds and the clock
	 * speed by the constructor.
	 */
	uint32_t error_led_cycles;
	uint32_t error_led_counter;
	
	enum fd_state_list fd_state;
	enum fd_state_list next_state;
	
	uint32_t sector;
	uint32_t buffer;
	
	uint32_t track;
	uint32_t previous_track;
	
	/*
	 * On a read or write action, this variable keeps track of the number of
	 * bytes that have been transferred. The total number is always equal to
	 * FD_BYTES_PER_SECTOR, which is 512 normally.
	 */
	uint32_t bytes_done;
	
	/*
	 * The number of cycles needed for spinning up  the motor. The constructor
	 * calculates this number based on the amount of milliseconds and the
	 * clock speed.
	 */
	uint32_t spin_up_cycles;
	
	/*
	 * Number of cycles motor keeps running after an activity is finished.
	 * Constructor calculates this based on ms and clock speed.
	 */
	uint32_t spin_delay_cycles;
	
	/*
	 * Number of cycles for track change. Calculated by constructor.
	 */
	uint32_t track_change_cycles;
	
	int32_t track_change_cycle_counter;
	
	/*
	 * Internal counter that keeps track of the amount of cycles since
	 * the start of an activity. After a state change, this number will be
	 * set to 0.
	 */
	uint32_t cycles_done;
	
	void set_error_state(enum fd_error_list new_error);
	void reset_error_state();
	
	void attempt_start_reading();
	void attempt_start_writing();
	
	inline void get_sector_and_buffer()
	{
		sector = registers[0x4] << 24 |
			 registers[0x5] << 16 |
			 registers[0x6] <<  8 |
			 registers[0x7] <<  0 ;
		buffer = registers[0x8] << 24 |
			 registers[0x9] << 16 |
			 registers[0xa] <<  8 |
			 registers[0xb] <<  0 ;
		
		previous_track = track;
		track  = sector / (FD_SIDES * FD_SECTORS_PER_TRACK);
	}
	
	uint16_t sample_no;
	bool previous_sample_motor_on;
	bool playing_spinning_down;
	bool playing_track_change;
	uint32_t track_last_sample;
	uint16_t spinning_down_sample_no;
	uint16_t track_change_sample_no;
public:
	fd();
	~fd();
	
	uint8_t	*disk_contents;
	
	int insert_disk(const char *path, bool write_protect_disk, bool save_on_eject_disk);
	int eject_disk();
	
	void reset();
	uint8_t read_byte(uint8_t address);
	void write_byte(uint8_t address, uint8_t byte);
	void run(uint32_t cycles);
	
	inline int get_error_state() { return current_error_state; }
	
	uint16_t bytes_per_sector();
	uint32_t disk_size();
	uint16_t *icon_data();
	
	inline bool disk_inside() { return fd_state != FD_STATE_EMPTY; }
	
	inline bool motor_spinning()
	{
		return	(fd_state != FD_STATE_EMPTY) &&
			(fd_state != FD_STATE_DISK_LOADED);
	}
	
	inline bool reading()
	{
		return	(fd_state == FD_STATE_READING) ||
			(next_state == FD_STATE_READING);
	}
	
	inline bool writing()
	{
		return	(fd_state == FD_STATE_WRITING) ||
			(next_state == FD_STATE_WRITING);
	}
	
	inline bool in_error() { return current_error_state != FD_ERROR_NONE; }
	
	inline uint8_t current_track() { return track; }
	
	int16_t sound_sample();
};

}

#endif
