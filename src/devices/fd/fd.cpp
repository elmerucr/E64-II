/*
 * fd.hpp
 * E64-II
 *
 * Copyright © 2020-2021 elmerucr. All rights reserved.
 */

#include "common.hpp"
#include "monitor_console.hpp"
#include "fd.hpp"
#include <unistd.h>
#include <sys/stat.h>

#define SIDES			2
#define TRACKS			80
#define SECTORS_PER_TRACK	18
#define SECTORS			SIDES*TRACKS*SECTORS_PER_TRACK
#define BYTES_PER_SECTOR	512
#define	DISK_SIZE		SECTORS*BYTES_PER_SECTOR

#define CYCLES_PER_BYTE		8192
#define SPIN_UP_TIME_MS		300
#define SPIN_DELAY_MS		2000
#define ERROR_LED_TIME_MS	250

E64::fd::fd()
{
	disk_contents = new uint8_t[DISK_SIZE * sizeof(uint8_t)];
	
	fd_state = FD_STATE_EMPTY;
	
	error_led_cycles = (CPU_CLOCK_SPEED / 1000) * ERROR_LED_TIME_MS;
	reset_error_state();
	
	spin_up_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_UP_TIME_MS;
	spin_delay_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_DELAY_MS;
	
	cycle_counter = 0;
}

E64::fd::~fd()
{
	if (fd_state != FD_STATE_EMPTY) {
		// finish actions if any such as writing a sector
		eject_disk();
	}
	delete disk_contents;
}

void E64::fd::reset()
{
	// a reset doesn't eject a disk
	
	for (int i=0; i<16; i++)
		registers[i] = 0x00;
	
	reset_error_state();
	
	cycle_counter = 0;
}

uint8_t E64::fd::read_byte(uint8_t address)
{
	switch (address & 0xf) {
		case 0x00:
			return
				(disk_inside()    ? 0b00000001 : 0b00000000) |
				(motor_spinning() ? 0b00000010 : 0b00000000) |
				(reading()        ? 0b00000100 : 0b00000000) |
				(writing()        ? 0b00001000 : 0b00000000) |
				(in_error()       ? 0b10000000 : 0b00000000) ;
		case 0x02:
			return read_error_state();
		default:
			return registers[address & 0xf];
	}
	return 0x00;
}

void E64::fd::write_byte(uint8_t address, uint8_t byte)
{
	switch (address & 0xf) {
		case 0x00:
			break;
		case 0x01:
			potential_sector_number =
				registers[0x4] << 24 |
				registers[0x5] << 16 |
				registers[0x6] <<  8 |
				registers[0x7] <<  0 ;
			potential_memory_address =
				registers[0x8] << 24 |
				registers[0x9] << 16 |
				registers[0xa] <<  8 |
				registers[0xb] <<  0 ;
			memory_address &= 0xfffffffe;
			switch (byte) {
				case 0b00000100:
					attempt_start_reading();
					break;
				case 0b00001000:
					attempt_start_writing();
					break;
				case 0b10000000:
					reset_error_state();
					break;
				default:
					break;
			}
			break;
		default:
			registers[address & 0xf] = byte;
	}
}

void E64::fd::attempt_start_reading()
{
	switch (fd_state) {
		case FD_STATE_EMPTY:
			start_error_state(FD_ERROR_NO_DISK_INSIDE);
			break;
		case FD_STATE_DISK_LOADED:
			if (sector_number >= SECTORS) {
				start_error_state(FD_ERROR_WRONG_SECTOR);
				break;
			}
			sector_number = potential_sector_number;
			memory_address = potential_memory_address;
			fd_state = FD_STATE_SPINNING_UP;
			next_state = FD_STATE_READING;
			byte_count = 0;
			break;
		case FD_STATE_SPINNING_UP:
			switch (next_state) {
				case FD_STATE_SPINNING:
					if (sector_number >= SECTORS) {
						start_error_state(FD_ERROR_WRONG_SECTOR);
						break;
					}
					sector_number = potential_sector_number;
					memory_address = potential_memory_address;
					next_state = FD_STATE_READING;
					byte_count = 0;
					break;
				case FD_STATE_READING:
					start_error_state(FD_ERROR_READING_PLANNED);
					break;
				case FD_STATE_WRITING:
					start_error_state(FD_ERROR_WRITING_PLANNED);
					break;
				default:
					break;
			}
			break;
		case FD_STATE_READING:
			start_error_state(FD_ERROR_READING);
			break;
		case FD_STATE_WRITING:
			start_error_state(FD_ERROR_WRITING);
			break;
		case FD_STATE_SPINNING:
			if (sector_number >= SECTORS) {
				start_error_state(FD_ERROR_WRONG_SECTOR);
				break;
			}
			sector_number = potential_sector_number;
			memory_address = potential_memory_address;
			fd_state = FD_STATE_READING;
			//next_state = FD_STATE_SPINNING
			byte_count = 0;
			break;
	}
}

void E64::fd::attempt_start_writing()
{
	if (write_protect) {
		start_error_state(FD_ERROR_WRITE_PROTECT);
	} else {
		switch (fd_state) {
			case FD_STATE_EMPTY:
				start_error_state(FD_ERROR_NO_DISK_INSIDE);
				break;
			case FD_STATE_DISK_LOADED:
				if (sector_number >= SECTORS) {
					start_error_state(FD_ERROR_WRONG_SECTOR);
					break;
				}
				sector_number = potential_sector_number;
				memory_address = potential_memory_address;
				fd_state = FD_STATE_SPINNING_UP;
				next_state = FD_STATE_WRITING;
				byte_count = 0;
				break;
			case FD_STATE_SPINNING_UP:
				switch (next_state) {
					case FD_STATE_SPINNING:
						if (sector_number >= SECTORS) {
							start_error_state(FD_ERROR_WRONG_SECTOR);
							break;
						}
						sector_number = potential_sector_number;
						memory_address = potential_memory_address;
						next_state = FD_STATE_WRITING;
						byte_count = 0;
						break;
					case FD_STATE_READING:
						start_error_state(FD_ERROR_READING_PLANNED);
						break;
					case FD_STATE_WRITING:
						start_error_state(FD_ERROR_WRITING_PLANNED);
						break;
					default:
						break;
				}
				break;
			case FD_STATE_READING:
				start_error_state(FD_ERROR_READING);
				break;
			case FD_STATE_WRITING:
				start_error_state(FD_ERROR_WRITING);
				break;
			case FD_STATE_SPINNING:
				if (sector_number >= SECTORS) {
					start_error_state(FD_ERROR_WRONG_SECTOR);
					break;
				}
				sector_number = potential_sector_number;
				memory_address = potential_memory_address;
				fd_state = FD_STATE_WRITING;
				byte_count = 0;
				break;
		}
	}
}

int E64::fd::insert_disk(const char *path, bool write_protect_disk, bool save_on_eject_disk)
{
	if (fd_state != FD_STATE_EMPTY) {
		debug_console_print("\nerror: already a disk inside\n");
		return 1;
	}
	
	struct stat stats;
	
	if (stat(path, &stats) == 0) {
		if (S_ISDIR(stats.st_mode)) {
			debug_console_print("\nerror: path is directory\n");
			return 1;
		}
		if (stats.st_size != DISK_SIZE) {
			debug_console_print("\nerror: disk image wrong size\n");
			return 1;
		}
		debug_console_print("\ninserting disk: ");
		debug_console_print(path);
		debug_console_put_char('\n');
		current_disk = fopen(path, "r+b");
		fread(disk_contents, DISK_SIZE, 1, current_disk);
		
		fd_state = FD_STATE_SPINNING_UP;
		next_state = FD_STATE_SPINNING;
		
		write_protect = write_protect_disk;
		save_on_eject = save_on_eject_disk;
		
		if (write_protect) {
			debug_console_print("\ndisk is write protected\n");
		} else {
			debug_console_print("\ndisk is writable\n");
		}
		if (save_on_eject) {
			debug_console_print("disk contents will be saved on eject\n");
		} else {
			debug_console_print("disk contents will not be saved on eject\n");
		}
		
		return 0;
	} else {
		debug_console_print("\nerror: no such file\n");
		return 1;
	}
}

int E64::fd::eject_disk()
{
	switch (fd_state) {
		case FD_STATE_EMPTY:
			start_error_state(FD_ERROR_NO_DISK_INSIDE);
			return 1;
		case FD_STATE_DISK_LOADED:
			if (save_on_eject) {
				fseek(current_disk, 0, SEEK_SET);
				fwrite(disk_contents, DISK_SIZE, 1, current_disk);
				printf("[fd] writing disk contents\n");
			}
			fclose(current_disk);
			fd_state = FD_STATE_EMPTY;
			for (int i=0; i<DISK_SIZE; i++)
				disk_contents[i] = 0;
			return 0;
		default:
			// drive motor is spinning
			start_error_state(FD_ERROR_MOTOR_IS_SPINNING);
			return 1;
	}
}

uint16_t E64::fd::bytes_per_sector()
{
	return BYTES_PER_SECTOR;
}

uint32_t E64::fd::disk_size()
{
	return DISK_SIZE;
}


void E64::fd::run(uint32_t number_of_cycles)
{
	switch (fd_state) {
		case FD_STATE_EMPTY:
			break;
		case FD_STATE_DISK_LOADED:
			break;
		case FD_STATE_SPINNING_UP:
			cycle_counter += number_of_cycles;
			if (cycle_counter > spin_up_cycles) {
				fd_state = next_state;
				next_state = FD_STATE_SPINNING;
				cycle_counter -= spin_up_cycles;
			}
			break;
		case FD_STATE_READING:
			cycle_counter += number_of_cycles;
			if (cycle_counter > CYCLES_PER_BYTE) {
				pc.mmu->ram[(memory_address+byte_count) & 0xffffff] =
					pc.fd0->disk_contents[(sector_number * BYTES_PER_SECTOR) + byte_count];
				cycle_counter -= CYCLES_PER_BYTE;
				byte_count++;
				if (byte_count == BYTES_PER_SECTOR) {
					fd_state = FD_STATE_SPINNING;
					cycle_counter = 0;
				}
			}
			break;
		case FD_STATE_WRITING:
			cycle_counter += number_of_cycles;
			if (cycle_counter > CYCLES_PER_BYTE) {
				pc.fd0->disk_contents[(sector_number * BYTES_PER_SECTOR) + byte_count] =
					pc.mmu->ram[(memory_address+byte_count) & 0xffffff];
				cycle_counter -= CYCLES_PER_BYTE;
				byte_count++;
				if (byte_count == BYTES_PER_SECTOR) {
					fd_state = FD_STATE_SPINNING;
					cycle_counter = 0;
				}
			}
			break;
		case FD_STATE_SPINNING:
			cycle_counter += number_of_cycles;
			if (cycle_counter > spin_delay_cycles) {
				fd_state = FD_STATE_DISK_LOADED;
				cycle_counter -= spin_delay_cycles;
			}
			break;
	}
	if (current_error_state != FD_ERROR_NONE) {
		if (error_led_counter > error_led_cycles) {
			error_led_on = !error_led_on;
			error_led_counter -= error_led_cycles;
		}
		error_led_counter += number_of_cycles;
	}
}

uint16_t disk_icon_data[448] = {
	// FD_EMPTY
	COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_03,
	COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,COBALT_03,

	// FD_DISK_LOADED
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_02,COBALT_06,
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	
	// FD_SPINNING_UP
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,   0x44f3,COBALT_06,
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	
	// FD_READING
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06, GREEN_06,COBALT_06,
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	
	// FD_WRITING
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,   0x1ff1,COBALT_06,
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	
	// FD_SPINNING
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,COBALT_06,
	
	// error led
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,
	C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED,C64_RED
};

uint16_t *E64::fd::icon_data()
{
	if (error_led_on)
		return &disk_icon_data[384];
	switch (fd_state) {
		case FD_STATE_EMPTY:
			return &disk_icon_data[0];
		case FD_STATE_DISK_LOADED:
			return &disk_icon_data[64];
		case FD_STATE_SPINNING_UP:
			return &disk_icon_data[128];
		case FD_STATE_READING:
			return &disk_icon_data[192];
		case FD_STATE_WRITING:
			return &disk_icon_data[256];
		case FD_STATE_SPINNING:
			return &disk_icon_data[320];
	}
}

void E64::fd::start_error_state(enum fd_error_state_list new_error)
{
	current_error_state = new_error;
	error_led_on = true;
	error_led_counter = 0;
}

void E64::fd::reset_error_state()
{
	current_error_state = FD_ERROR_NONE;
	error_led_on = false;
}
