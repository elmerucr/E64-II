//  command.hpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstdint>

namespace E64 {

class command_t {
public:
	void execute(char *string_to_parse_and_exec);
	void enter_monitor_line(char *string_to_parse_and_exec);
	void enter_monitor_character_line(char *string_to_parse_and_exec);
	void enter_monitor_binary_line(char *string_to_parse_and_exec);
	void enter_monitor_disk_line(char *string_to_parse_and_exec);
	void dump_cpu_status(void);
	void disassemble(uint8_t number);
	void memory_dump(uint32_t address, int rows);
	void memory_character_dump(uint32_t address, int rows);
	void memory_binary_dump(uint32_t address, int rows);
	void fd_dump(uint32_t address, int rows);
	void single_step_cpu();

	/*
	 * Second argument is a pointer to an uint32_t and writes its
	 * result to it, only if no errors were found. The return value
	 * is TRUE upon success, and FALSE on error.
	 */
	bool hex_string_to_int(const char *temp_string, uint32_t *return_value);
};

}

#endif
