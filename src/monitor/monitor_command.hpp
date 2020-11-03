//  monitor_command.hpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#ifndef MONITOR_COMMAND_HPP
#define MONITOR_COMMAND_HPP

#include <cstdint>

namespace E64
{

void debug_command_execute(char *string_to_parse_and_exec);
void debug_command_enter_monitor_line(char *string_to_parse_and_exec);
void debug_command_dump_cpu_status(void);
void debug_command_disassemble(uint8_t number);
void debug_command_memory_dump(uint32_t address, int rows);
void debug_command_memory_character_dump(uint32_t address, int rows);
void debug_command_single_step_cpu(void);

/*
 * Second argument is a pointer to an uint32_t and writes its
 * result to it, only if no errors were found. The return value
 * is TRUE upon success, and FALSE on error.
 */
bool debug_command_hex_string_to_int(const char *temp_string, uint32_t *return_value);

}

#endif
