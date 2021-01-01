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

E64::fd::fd()
{
	disk_contents = new uint8_t[DISK_SIZE * sizeof(uint8_t)];
	disk_inside = false;
	
	current_fd_state = FD_IDLE;
	current_motor_state = MOTOR_IDLE;
	
	spin_up_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_UP_TIME_MS;
	spin_up_counter = 0;
	
	spin_delay_cycles = (CPU_CLOCK_SPEED / 1000) * SPIN_DELAY_MS;
	spin_delay_counter = 0;
	
	cycle_counter = 0;
}

E64::fd::~fd()
{
	// check for unfinished writing actions?
	// this avoids disk corruption
	if (disk_inside) {
		eject_disk();
	}
	delete disk_contents;
}

void E64::fd::reset()
{
	// a reset doesn't eject a disk
	
	for (int i=0; i<16; i++)
		registers[i] = 0x00;
}

uint8_t E64::fd::read_byte(uint8_t address)
{
	switch(address & 0xf) {
		case 0x00:
			return disk_inside ? 0b00000001 : 0b00000000;
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
			
			if ((byte & 0b11) == 0b10) {
				if (current_fd_state == FD_IDLE) {
					current_fd_state = FD_READING;
				}
			} else if ((byte & 0b11) == 0b11) {
				if (current_fd_state == FD_IDLE) {
					current_fd_state = FD_READING;
				}
			}
			break;
		default:
			registers[address & 0xf] = byte;
	}
}

int E64::fd::insert_disk(const char *path, bool write_protect_disk)
{
	if (disk_inside) {
		debug_console_print("\nerror: already a disk inside\n");
		return FD_ERROR_DISK_INSIDE;
	}
	
	struct stat stats;
	
	if (stat(path, &stats) == 0) {
		if (S_ISDIR(stats.st_mode)) {
			debug_console_print("\nerror: path is directory\n");
			return FD_ERROR_IS_DIRECTORY;
		}
		if (stats.st_size != 1474560) {
			debug_console_print("\nerror: disk image wrong size\n");
			return FD_ERROR_WRONG_SIZE;
		}
		debug_console_print("\ninserting disk: ");
		debug_console_print(path);
		debug_console_put_char('\n');
		current_disk = fopen(path, "r+b");
		fread(disk_contents, DISK_SIZE, 1, current_disk);
		disk_inside = true;
		write_protect = write_protect_disk ? true : false;
		return 0;
	} else {
		debug_console_print("\nerror: file doesn't exist\n");
		return FD_ERROR_WRONG_PATH;
	}
}

int E64::fd::eject_disk()
{
	int return_value = 0;
	if (disk_inside) {
		if (!write_protect) {
			fseek(current_disk, 0, SEEK_SET);
			fwrite(disk_contents, DISK_SIZE, 1, current_disk);
			printf("[fd] writing disk contents\n");
		}
		fclose(current_disk);
		disk_inside = false;
		for (int i=0; i<DISK_SIZE; i++)
			disk_contents[i] = 0;
	} else {
		return_value = 1;
	}
	return return_value;
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
	switch (current_fd_state) {
		case FD_IDLE:
			check_spin_delay();
			break;
		case FD_READING:
			break;
		case FD_WRITING:
			break;
	}
}

void E64::fd::check_spin_delay()
{
	//if (current_motor_state == MOTOR)
}
