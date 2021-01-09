/*
 * fd.cpp
 * E64-II
 *
 * Copyright © 2020-2021 elmerucr. All rights reserved.
 */

#include "common.hpp"
#include "monitor_console.hpp"
#include "fd.hpp"
#include <unistd.h>
#include <sys/stat.h>

E64::fd::fd()
{
	disk_contents = new uint8_t[FD_DISK_SIZE * sizeof(uint8_t)];
	
	fd_state = FD_STATE_EMPTY;
	
	error_led_cycles = (CPU_CLOCK_SPEED / 1000) * FD_ERROR_LED_TIME_MS;
	reset_error_state();
	
	spin_up_cycles = (CPU_CLOCK_SPEED / 1000) * FD_SPIN_UP_TIME_MS;
	spin_delay_cycles = (CPU_CLOCK_SPEED / 1000) * FD_SPIN_DELAY_MS;
}

E64::fd::~fd()
{
	if (fd_state != FD_STATE_EMPTY) {
		// finish actions if any such as writing a sector
		// NEEDS WORK
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
	cycles_done = 0;
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
		case 0x01:
			return 0x00;
		case 0x02:
			return get_error_state();
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
		case 0x0b:
			// smallest byte of buffer address, must be even
			registers[0x0b] = byte & 0b11111110;
			break;
		default:
			registers[address & 0xf] = byte;
	}
}

void E64::fd::attempt_start_reading()
{
	switch (fd_state) {
		case FD_STATE_EMPTY:
			set_error_state(FD_ERROR_NO_DISK_INSIDE);
			break;
		case FD_STATE_DISK_LOADED:
			if (sector >= FD_SECTORS) {
				set_error_state(FD_ERROR_ILLEGAL_SECTOR);
				break;
			}
			get_sector_and_buffer();
			fd_state = FD_STATE_SPINNING_UP;
			next_state = FD_STATE_READING;
			cycles_done = 0;
			bytes_done = 0;
			break;
		case FD_STATE_SPINNING_UP:
			switch (next_state) {
				case FD_STATE_SPINNING:
					if (sector >= FD_SECTORS) {
						set_error_state(FD_ERROR_ILLEGAL_SECTOR);
						break;
					}
					get_sector_and_buffer();
					next_state = FD_STATE_READING;
					cycles_done = 0;
					bytes_done = 0;
					break;
				case FD_STATE_READING:
					set_error_state(FD_ERROR_READING_PLANNED);
					break;
				case FD_STATE_WRITING:
					set_error_state(FD_ERROR_WRITING_PLANNED);
					break;
				default:
					break;
			}
			break;
		case FD_STATE_READING:
			set_error_state(FD_ERROR_READING);
			break;
		case FD_STATE_WRITING:
			set_error_state(FD_ERROR_WRITING);
			break;
		case FD_STATE_SPINNING:
			if (sector >= FD_SECTORS) {
				set_error_state(FD_ERROR_ILLEGAL_SECTOR);
				break;
			}
			get_sector_and_buffer();
			fd_state = FD_STATE_READING;
			cycles_done = 0;
			bytes_done = 0;
			break;
	}
}

void E64::fd::attempt_start_writing()
{
	if (write_protect) {
		set_error_state(FD_ERROR_WRITE_PROTECT);
	} else {
		switch (fd_state) {
			case FD_STATE_EMPTY:
				set_error_state(FD_ERROR_NO_DISK_INSIDE);
				break;
			case FD_STATE_DISK_LOADED:
				if (sector >= FD_SECTORS) {
					set_error_state(FD_ERROR_ILLEGAL_SECTOR);
					break;
				}
				get_sector_and_buffer();
				fd_state = FD_STATE_SPINNING_UP;
				next_state = FD_STATE_WRITING;
				cycles_done = 0;
				bytes_done = 0;
				break;
			case FD_STATE_SPINNING_UP:
				switch (next_state) {
					case FD_STATE_SPINNING:
						if (sector >= FD_SECTORS) {
							set_error_state(FD_ERROR_ILLEGAL_SECTOR);
							break;
						}
						get_sector_and_buffer();
						next_state = FD_STATE_WRITING;
						cycles_done = 0;
						bytes_done = 0;
						break;
					case FD_STATE_READING:
						set_error_state(FD_ERROR_READING_PLANNED);
						break;
					case FD_STATE_WRITING:
						set_error_state(FD_ERROR_WRITING_PLANNED);
						break;
					default:
						break;
				}
				break;
			case FD_STATE_READING:
				set_error_state(FD_ERROR_READING);
				break;
			case FD_STATE_WRITING:
				set_error_state(FD_ERROR_WRITING);
				break;
			case FD_STATE_SPINNING:
				if (sector >= FD_SECTORS) {
					set_error_state(FD_ERROR_ILLEGAL_SECTOR);
					break;
				}
				get_sector_and_buffer();
				fd_state = FD_STATE_WRITING;
				cycles_done = 0;
				bytes_done = 0;
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
		if (stats.st_size != FD_DISK_SIZE) {
			debug_console_print("\nerror: disk image wrong size\n");
			return 1;
		}
		debug_console_print("\ninserting disk: ");
		debug_console_print(path);
		debug_console_put_char('\n');
		current_disk = fopen(path, "r+b");
		fread(disk_contents, FD_DISK_SIZE, 1, current_disk);
		
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
			set_error_state(FD_ERROR_NO_DISK_INSIDE);
			return 1;
		case FD_STATE_DISK_LOADED:
			if (save_on_eject) {
				fseek(current_disk, 0, SEEK_SET);
				fwrite(disk_contents, FD_DISK_SIZE, 1, current_disk);
				printf("[fd] writing disk contents\n");
			}
			fclose(current_disk);
			fd_state = FD_STATE_EMPTY;
			for (int i=0; i < FD_DISK_SIZE; i++)
				disk_contents[i] = 0;
			return 0;
		default:
			// drive motor is spinning
			// that's true, but make difference between read/write
			// etc???
			// NEEDS WORK
			set_error_state(FD_ERROR_MOTOR_IS_SPINNING);
			return 1;
	}
}

uint16_t E64::fd::bytes_per_sector()
{
	return FD_BYTES_PER_SECTOR;
}

uint32_t E64::fd::disk_size()
{
	return FD_DISK_SIZE;
}


void E64::fd::run(uint32_t cycles)
{
	switch (fd_state) {
		case FD_STATE_EMPTY:
			break;
		case FD_STATE_DISK_LOADED:
			break;
		case FD_STATE_SPINNING_UP:
			cycles_done += cycles;
			if (cycles_done > spin_up_cycles) {
				fd_state = next_state;
				next_state = FD_STATE_SPINNING;
				cycles_done -= spin_up_cycles;
			}
			break;
		case FD_STATE_READING:
			cycles_done += cycles;
			if (cycles_done > FD_CYCLES_PER_BYTE) {
				pc.mmu->ram[(buffer+bytes_done) & 0xffffff] =
					pc.fd0->disk_contents[(sector * FD_BYTES_PER_SECTOR) + bytes_done];
				cycles_done -= FD_CYCLES_PER_BYTE;
				bytes_done++;
				if (bytes_done == FD_BYTES_PER_SECTOR) {
					fd_state = FD_STATE_SPINNING;
					cycles_done = 0;
				}
			}
			break;
		case FD_STATE_WRITING:
			cycles_done += cycles;
			if (cycles_done > FD_CYCLES_PER_BYTE) {
				pc.fd0->disk_contents[(sector * FD_BYTES_PER_SECTOR) + bytes_done] =
					pc.mmu->ram[(buffer+bytes_done) & 0xffffff];
				cycles_done -= FD_CYCLES_PER_BYTE;
				bytes_done++;
				if (bytes_done == FD_BYTES_PER_SECTOR) {
					fd_state = FD_STATE_SPINNING;
					cycles_done = 0;
				}
			}
			break;
		case FD_STATE_SPINNING:
			cycles_done += cycles;
			if (cycles_done > spin_delay_cycles) {
				fd_state = FD_STATE_DISK_LOADED;
				cycles_done -= spin_delay_cycles;
			}
			break;
	}
	if (current_error_state != FD_ERROR_NONE) {
		if (error_led_counter > error_led_cycles) {
			error_led_on = !error_led_on;
			error_led_counter -= error_led_cycles;
		}
		error_led_counter += cycles;
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
	
	// error led flashes
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
		return &disk_icon_data[0x180];
	switch (fd_state) {
		case FD_STATE_EMPTY:
			return &disk_icon_data[0x000];
		case FD_STATE_DISK_LOADED:
			return &disk_icon_data[0x040];
		case FD_STATE_SPINNING_UP:
			return &disk_icon_data[0x080];
		case FD_STATE_READING:
			return &disk_icon_data[0x0c0];
		case FD_STATE_WRITING:
			return &disk_icon_data[0x100];
		case FD_STATE_SPINNING:
			return &disk_icon_data[0x140];
	}
}

void E64::fd::set_error_state(enum fd_error_list new_error)
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
