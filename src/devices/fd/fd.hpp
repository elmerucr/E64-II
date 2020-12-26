/*
 * fd.hpp
 * E64-II
 *
 * Copyright © 2020 elmerucr. All rights reserved.
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

class fd {
private:
	bool	disk_attached;
	bool	write_protect;
	
	FILE	*current_disk;
	
	uint8_t	*disk_contents;
	
public:
	fd();
	~fd();
	
	int	attach_disk_image(const char *path, bool write_on_detach);
	int	detach_disk_image();
	
	void	reset();
	uint8_t	read_byte(uint8_t address);
	void	write_byte(uint8_t address, uint8_t byte);
	void	run(uint32_t number_of_cycles);
};

}

#endif
