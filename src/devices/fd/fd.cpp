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

#define CYCLES_PER_BYTE		512
#define SPIN_UP_TIME_MS		300
#define SPIN_DELAY_MS		2000
#define ERROR_LED_TIME_MS	250

E64::fd::fd()
{
	disk_contents = new uint8_t[DISK_SIZE * sizeof(uint8_t)];
	
	fd_state = FD_EMPTY;
	
	error_led_cycles = (CPU_CLOCK_SPEED / 1000) * ERROR_LED_TIME_MS;
	reset_error_state();
	
	spin_up_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_UP_TIME_MS;
	spin_delay_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_DELAY_MS;
	
	cycle_counter = 0;
}

E64::fd::~fd()
{
	if (fd_state != FD_EMPTY) {
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
	switch(address & 0xf) {
		case 0x00:
			return
				(disk_inside() ? 0b00000001 : 0b00000000) |
				(motor_spinning() ? 0b00000010 : 0b00000000);
		case 0x01:
			break;
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
			sector_number =
				registers[0x2] << 24 |
				registers[0x3] << 16 |
				registers[0x4] <<  8 |
				registers[0x5] <<  0 ;
			memory_address =
				registers[0x6] << 24 |
				registers[0x7] << 16 |
				registers[0x8] <<  8 |
				registers[0x9] <<  0 ;
			memory_address &= 0xfffffffe;
			
			break;
		default:
			registers[address & 0xf] = byte;
	}
}

int E64::fd::insert_disk(const char *path, bool write_protect_disk)
{
	if (fd_state != FD_EMPTY) {
		debug_console_print("\nerror: already a disk inside\n");
		return FD_ERROR_DISK_INSIDE;
	}
	
	struct stat stats;
	
	if (stat(path, &stats) == 0) {
		if (S_ISDIR(stats.st_mode)) {
			debug_console_print("\nerror: path is directory\n");
			return FD_ERROR_IS_DIRECTORY;
		}
		if (stats.st_size != DISK_SIZE) {
			debug_console_print("\nerror: disk image wrong size\n");
			return FD_ERROR_WRONG_SIZE;
		}
		debug_console_print("\ninserting disk: ");
		debug_console_print(path);
		debug_console_put_char('\n');
		current_disk = fopen(path, "r+b");
		fread(disk_contents, DISK_SIZE, 1, current_disk);
		
		fd_state = FD_SPINNING_UP;
		next_state = FD_SPINNING;
		
		write_protect = write_protect_disk ? true : false;
		return 0;
	} else {
		debug_console_print("\nerror: file doesn't exist\n");
		return FD_ERROR_WRONG_PATH;
	}
}

int E64::fd::eject_disk()
{
	switch (fd_state) {
		case FD_EMPTY:
			start_error_state();
			return 1;
		case FD_DISK_LOADED:
			if (!write_protect) {
				fseek(current_disk, 0, SEEK_SET);
				fwrite(disk_contents, DISK_SIZE, 1, current_disk);
				printf("[fd] writing disk contents\n");
			}
			fclose(current_disk);
			fd_state = FD_EMPTY;
			for (int i=0; i<DISK_SIZE; i++)
				disk_contents[i] = 0;
			return 0;
		default:
			// drive motor is spinning
			start_error_state();
			return 2;
	}
//	int return_value = 0;
//	if (fd_state != FD_EMPTY) {
//		if (!write_protect) {
//			fseek(current_disk, 0, SEEK_SET);
//			fwrite(disk_contents, DISK_SIZE, 1, current_disk);
//			printf("[fd] writing disk contents\n");
//		}
//		fclose(current_disk);
//		fd_state = FD_EMPTY;
//		for (int i=0; i<DISK_SIZE; i++)
//			disk_contents[i] = 0;
//	} else {
//		return_value = 1;
//	}
//	return return_value;
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
		case FD_EMPTY:
			break;
		case FD_DISK_LOADED:
			break;
		case FD_SPINNING_UP:
			cycle_counter += number_of_cycles;
			if (cycle_counter > spin_up_cycles) {
				fd_state = next_state;
				cycle_counter -= spin_up_cycles;
			}
			break;
		case FD_SPINNING:
			cycle_counter += number_of_cycles;
			if (cycle_counter > spin_delay_cycles) {
				fd_state = FD_DISK_LOADED;
				cycle_counter -= spin_delay_cycles;
			}
			break;
//		case FD_IDLE:
//			if (current_motor_state == MOTOR_SPINNING)
//				spin_delay_counter += number_of_cycles;
//
//			//check_spin_delay();
//			break;
//		case FD_READING:
//			break;
//		case FD_WRITING:
//			break;
	}
	if (in_error) {
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
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
	COBALT_06, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03, GREEN_03,COBALT_06,
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
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
	COBALT_06, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04, GREEN_04,COBALT_06,
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
		case FD_EMPTY:
			return &disk_icon_data[0];
		case FD_DISK_LOADED:
			return &disk_icon_data[64];
		case FD_SPINNING_UP:
			return &disk_icon_data[128];
		case FD_READING:
			return &disk_icon_data[192];
		case FD_WRITING:
			return &disk_icon_data[256];
		case FD_SPINNING:
			return &disk_icon_data[320];
	}
}

void E64::fd::start_error_state()
{
	in_error = true;
	error_led_on = true;
	error_led_counter = 0;
}

void E64::fd::reset_error_state()
{
	in_error = false;
	error_led_on = false;
}
