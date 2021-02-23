//  command.hpp
//  E64-II
//
//  Copyright © 2019-2021 elmerucr. All rights reserved.

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstdint>

namespace E64 {

void monitor_command_execute(char *string_to_parse_and_exec);
void monitor_command_enter_monitor_line(char *string_to_parse_and_exec);
void monitor_command_enter_monitor_character_line(char *string_to_parse_and_exec);
void monitor_command_enter_monitor_binary_line(char *string_to_parse_and_exec);
void monitor_command_enter_monitor_disk_line(char *string_to_parse_and_exec);
void monitor_command_dump_cpu_status(void);
void monitor_command_disassemble(uint8_t number);
void monitor_command_memory_dump(uint32_t address, int rows);
void monitor_command_memory_character_dump(uint32_t address, int rows);
void monitor_command_memory_binary_dump(uint32_t address, int rows);
void monitor_command_fd_dump(uint32_t address, int rows);
void monitor_command_single_step_cpu();

/*
 * Second argument is a pointer to an uint32_t and writes its
 * result to it, only if no errors were found. The return value
 * is TRUE upon success, and FALSE on error.
 */
bool monitor_command_hex_string_to_int(const char *temp_string, uint32_t *return_value);

}

#endif
