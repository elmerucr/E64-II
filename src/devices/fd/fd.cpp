/*
 * fd.hpp
 * E64-II
 *
 * Copyright © 2020 elmerucr. All rights reserved.
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

#define CYCLES_PER_BYTE		407

E64::fd::fd()
{
	disk_contents = new uint8_t[DISK_SIZE * sizeof(uint8_t)];
	disk_attached = false;
}

E64::fd::~fd()
{
	if (disk_attached) {
		detach_disk_image();
	}
	delete disk_contents;
}

void E64::fd::reset()
{
	//
}

uint8_t E64::fd::read_byte(uint8_t address)
{
	return 0x00;
}

void E64::fd::write_byte(uint8_t address, uint8_t byte)
{
	//
}

int E64::fd::attach_disk_image(const char *path, bool write_protect_disk)
{
	if (disk_attached) {
		debug_console_print("\nerror: already disk inside\n");
		return FD_ERROR_DISK_INSIDE;
	}
	
	struct stat stats;
	
	if (stat(path, &stats) == 0) {
		if (S_ISDIR(stats.st_mode)) {
			debug_console_print("\nerror: path is directory\n");
			return FD_ERROR_IS_DIRECTORY;
		}
		if (stats.st_size != 1474560) {
			debug_console_print("\nerror: image wrong size\n");
			return FD_ERROR_WRONG_SIZE;
		}
		debug_console_print("\nattaching disk: ");
		debug_console_print(path);
		debug_console_put_char('\n');
		current_disk = fopen(path, "r+b");
		fread(disk_contents, DISK_SIZE, 1, current_disk);
		//for (int i=0; i<DISK_SIZE; i++)
		//	printf("%04x %02x\n", i, disk_contents[i]);
		disk_attached = true;
		write_protect = write_protect_disk ? true : false;
		return 0;
	} else {
		debug_console_print("\nerror: file doesn't exist\n");
		return FD_ERROR_WRONG_PATH;
	}
}

int E64::fd::detach_disk_image()
{
	int return_value = 0;
	if (disk_attached) {
		if (!write_protect) {
			fseek(current_disk, 0, SEEK_SET);
			fwrite(disk_contents, DISK_SIZE, 1, current_disk);
			printf("[fd] writing disk contents\n");
		}
		fclose(current_disk);
		disk_attached = false;
	} else {
		return_value = 1;
	}
	return return_value;
}
