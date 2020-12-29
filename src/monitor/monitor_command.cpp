//  monitor_command.cpp
//  E64-II
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <dirent.h>

#include "common.hpp"
#include "monitor_command.hpp"
#include "monitor_console.hpp"
#include "sdl2.hpp"
#include "monitor_status_bar.hpp"

char command_help_string[2048];

void E64::debug_command_execute(char *string_to_parse_and_exec)
{
	bool have_prompt = true;
	
	char *token0, *token1;
	token0 = strtok(string_to_parse_and_exec, " ");
	
	if (token0 == NULL) {
		have_prompt = false;
		debug_console_put_char('\n');
	} else if (token0[0] == ':') {
		have_prompt = false;
		debug_command_enter_monitor_line(string_to_parse_and_exec);
	} else if (token0[0] == ';') {
		have_prompt = false;
		debug_command_enter_monitor_character_line(string_to_parse_and_exec);
	} else if (token0[0] == '\'') {
		have_prompt = false;
		debug_command_enter_monitor_binary_line(string_to_parse_and_exec);
	} else if (token0[0] == '"') {
		have_prompt = false;
		debug_command_enter_monitor_disk_line(string_to_parse_and_exec);
	} else if (strcmp(token0, "b") == 0) {
		token1 = strtok(NULL, " ");
		debug_console_put_char('\n');
		if (token1 == NULL) {
			unsigned int no_of_breakpoints = (unsigned int)pc.m68k->debugger.breakpoints.elements();
			snprintf(command_help_string, 256, "currently %i cpu breakpoint(s) defined\n", no_of_breakpoints);
			debug_console_print(command_help_string);
			if (no_of_breakpoints > 0) {
				snprintf(command_help_string, 256, "\n # address active\n");
				debug_console_print(command_help_string);
				for (int i=0; i<no_of_breakpoints; i++) {
					snprintf(command_help_string, 256, "%2u $%06x  %s\n", i,
						 pc.m68k->debugger.breakpoints.guardAddr(i),
						 pc.m68k->debugger.breakpoints.isEnabled(i) ? "yes" : "no");
					debug_console_print(command_help_string);
				}
			}
		} else {
			uint32_t temp_32bit;
			if (debug_command_hex_string_to_int(token1, &temp_32bit)) {
				temp_32bit &= (RAM_SIZE - 1);
				pc.m68k->debugger.breakpoints.addAt(temp_32bit);
				snprintf(command_help_string, 256,
					 "cpu breakpoint at $%06x added\n",
					 temp_32bit);
				debug_console_print(command_help_string);
			} else {
				debug_console_print("error: invalid address\n");
			}
		}
	} else if (strcmp(token0, "cd") == 0) {
		token1 = strtok(NULL, " ");
		debug_console_put_char('\n');
		if (token1 == NULL) {
			if (chdir(prefs.home_dir)) {
				snprintf(command_help_string, 256,
					 "error: no such file or directory: %s\n",
					 prefs.home_dir);
				debug_console_print(command_help_string);
			}
			getcwd(prefs.current_path, 256);    // update current_path
		} else {
			if (chdir(token1)) {
				snprintf(command_help_string, 256,
					 "error: no such file or directory: %s\n",
					 token1);
				debug_console_print(command_help_string);
			}
			getcwd(prefs.current_path, 256);    // update current_path
		}
	} else if (strcmp(token0, "bar") == 0) {
		debug_console_put_char('\n');
		debug_console_toggle_status_bar();
	} else if (strcmp(token0, "bc") == 0) {
		debug_console_put_char('\n');
		pc.m68k->debugger.breakpoints.removeAll();
		debug_console_print("all cpu breakpoints removed\n");
	} else if (strcmp(token0, "c") == 0) {
		debug_console_put_char('\n');
		E64::sdl2_wait_until_enter_released();
		pc.switch_to_running();
	} else if( strcmp(token0, "clear") == 0 ) {
		debug_console_clear();
	} else if (strcmp(token0, "eject") == 0) {
		debug_console_put_char('\n');
		if (pc.fd0->eject_disk())
			debug_console_print("error: no disk inserted\n");
	} else if (strcmp(token0, "exit") == 0) {
		have_prompt = false;
		E64::sdl2_wait_until_enter_released();
		pc.running = false;
	} else if (strcmp(token0, "full") == 0) {
		debug_console_put_char('\n');
		host_video.toggle_fullscreen();
	} else if (strcmp(token0, "help") == 0) {
		token1 = strtok(NULL, " ");
		if (token1 == NULL) {
			debug_console_put_char('\n');
			debug_console_print(
				"<F1>    run next instruction\n"
				"<F2>    switch system status bar on and off\n"
				"<F3>    switch between readable and hexadecimal disassembly\n"
				"<F9>    switch to monitor and back to running\n"
				"<F10>   toggle runtime stats (running only)\n"
				"<ALT+q> quit application\n"
				"<ALT+r> reset machine\n"
				"<ALT+f> switch between full screen and windowed mode\n");
			debug_console_put_char('\n');
			debug_console_print(
				"other commands:\n"
				"       b      cd     bar      bc       c   clear   eject\n"
				"    exit    full  insert    help      ls       m      mb\n"
				"      mc      md     pwd       r   reset      sb     sbc\n"
				"     ver     win\n");
		}
	} else if (strcmp(token0, "insert") == 0) {
		token1 = strtok(NULL, " ");
		if (token1 == NULL) {
			debug_console_print("\nerror: missing filename\n");
		} else {
			pc.fd0->insert_disk(token1, true);
		}
	} else if (strcmp(token0, "ls") == 0) {
		debug_console_put_char('\n');
		getcwd(command_help_string, 256);
		debug_console_print(command_help_string);
		debug_console_put_char('\n');
	    
		DIR *directory = opendir(prefs.current_path);
		struct dirent *entry;
		int files = 0;

		while ((entry = readdir(directory))) {
			if (*entry->d_name != '.') {
				files++;
				snprintf(command_help_string, 256, "%s%c",
					 entry->d_name,
					 (entry->d_type) & 0b100 ? '/' : '\0');
				debug_console_print(command_help_string);
				debug_console_put_char('\n');
			}
		}
		closedir(directory);
		if (files == 0)
			debug_console_print("empty directory\n");
	} else if (strcmp(token0, "m") == 0) {
		have_prompt = false;
		token1 = strtok(NULL, " ");
		
		uint8_t lines_remaining = VICV_CHAR_ROWS -
			(debug_console.cursor_pos / VICV_CHAR_COLUMNS) - 9;
		if(lines_remaining == 0) lines_remaining = 1;

		uint32_t temp_pc = pc.m68k->getPC();
        
		if (token1 == NULL) {
			for (int i=0; i<lines_remaining; i++) {
				debug_console_put_char('\n');
				debug_command_memory_dump(temp_pc, 1);
				temp_pc = (temp_pc + 8) & 0x00ffffff;
			}
		} else {
			if (!debug_command_hex_string_to_int(token1, &temp_pc)) {
				debug_console_put_char('\n');
				debug_console_print("error: invalid address\n");
			} else {
				for (int i=0; i<lines_remaining; i++) {
					debug_console_put_char('\n');
					debug_command_memory_dump(temp_pc &
						(RAM_SIZE - 1), 1);
					temp_pc = (temp_pc + 8) & 0x00ffffff;
				}
			}
		}
	} else if (strcmp(token0, "mb") == 0) {
		have_prompt = false;
		token1 = strtok(NULL, " ");
		uint8_t lines_remaining = VICV_CHAR_ROWS -
			(debug_console.cursor_pos / VICV_CHAR_COLUMNS) - 9;
		if(lines_remaining == 0) lines_remaining = 1;
	
		uint32_t temp_pc = pc.m68k->getPC();
	
		if (token1 == NULL) {
			for (int i=0; i<lines_remaining; i++) {
				debug_console_put_char('\n');
				debug_command_memory_binary_dump(temp_pc, 1);
				temp_pc = (temp_pc + 1) & 0x00ffffff;
			}
		} else {
			if (!debug_command_hex_string_to_int(token1, &temp_pc)) {
				debug_console_put_char('\n');
				debug_console_print("error: invalid address\n");
			} else {
				for (int i=0; i<lines_remaining; i++) {
					debug_console_put_char('\n');
					debug_command_memory_binary_dump(temp_pc & (RAM_SIZE - 1), 1);
					temp_pc = (temp_pc + 1) & 0x00ffffff;
				}
			}
		}
	} else if (strcmp(token0, "mc") == 0) {
		have_prompt = false;
		token1 = strtok(NULL, " ");
		uint8_t lines_remaining = VICV_CHAR_ROWS -
			(debug_console.cursor_pos / VICV_CHAR_COLUMNS) - 9;
		if(lines_remaining == 0) lines_remaining = 1;
		if (token1 == NULL) {
			debug_console_put_char('\n');
			debug_console_print("error: need address\n");
		} else {
			uint32_t temp_32bit;
			if (!debug_command_hex_string_to_int(token1, &temp_32bit)) {
				debug_console_put_char('\n');
				debug_console_print("error: invalid address\n");
			} else {
				for (int i=0; i<lines_remaining; i++) {
					debug_console_put_char('\n');
					debug_command_memory_character_dump(temp_32bit &
						(RAM_SIZE - 1), 1);
					temp_32bit = (temp_32bit + 16) & 0x00ffffff;
				}
			}
		}
	} else if (strcmp(token0, "md") == 0) {
		have_prompt = false;
		token1 = strtok(NULL, " ");
		
		uint8_t lines_remaining = VICV_CHAR_ROWS -
			(debug_console.cursor_pos / VICV_CHAR_COLUMNS) - 9;
		if (lines_remaining == 0)
			lines_remaining = 1;
		
		uint32_t temp_pos = 0;
		
		if (token1 == NULL) {
			for (int i=0; i<lines_remaining; i++) {
				debug_console_put_char('\n');
				debug_command_fd_dump(temp_pos, 1);
				temp_pos += 0x08;
				if (temp_pos >= pc.fd0->disk_size())
					temp_pos = 0;
			}
		} else {
			if (!debug_command_hex_string_to_int(token1, &temp_pos)) {
				debug_console_put_char('\n');
				debug_console_print("error: invalid sector\n");
			} else {
				temp_pos *= pc.fd0->bytes_per_sector();
				if (temp_pos >= pc.fd0->disk_size())
					temp_pos = pc.fd0->disk_size() -
					pc.fd0->bytes_per_sector();
				for (int i=0; i<lines_remaining; i++) {
					debug_console_put_char('\n');
					debug_command_fd_dump(temp_pos, 1);
					temp_pos += 0x08;
				}
			}
		}
	} else if (strcmp(token0, "pwd") == 0) {
		debug_console_put_char('\n');
		getcwd(command_help_string, 256);
		debug_console_print(command_help_string);
		debug_console_put_char('\n');
	} else if (strcmp(token0, "r") == 0) {
		debug_command_dump_cpu_status();
	} else if (strcmp(token0, "reset") == 0) {
		debug_console_put_char('\n');
		pc.reset();
		statistics.reset();
	} else if (strcmp(token0, "sb") == 0) {
		token1 = strtok(NULL, " ");
		debug_console_put_char('\n');
		if (token1 == NULL) {
			unsigned int no_of_scanline_breakpoints = 0;
			for (int i=0; i<1024; i++) {
				if (pc.vicv->is_scanline_breakpoint(i))
					no_of_scanline_breakpoints++;
			}
			if (no_of_scanline_breakpoints) {
				snprintf(command_help_string, 256,
					 "currently %i scanline breakpoint(s) defined at:\n",
					 no_of_scanline_breakpoints);
				debug_console_print(command_help_string);
				for (int i=0; i<1024; i++) {
					if (pc.vicv->is_scanline_breakpoint(i)) {
						snprintf(command_help_string,
							 256, " %3i\n", i);
						debug_console_print(command_help_string);
					}
				}
			} else {
				debug_console_print("no scanline breakpoints defined\n");
			}
		} else {
			uint32_t temp_32bit = atoi(token1);
			temp_32bit &= 1023;
			if (pc.vicv->is_scanline_breakpoint(temp_32bit)) {
				snprintf(command_help_string, 256,
					 "removing scanline breakpoint %i\n",
					 temp_32bit);
				debug_console_print(command_help_string);
				pc.vicv->remove_scanline_breakpoint(temp_32bit);
			} else {
				snprintf(command_help_string, 256,
					 "adding scanline breakpoint %i\n",
					 temp_32bit);
				debug_console_print(command_help_string);
				pc.vicv->add_scanline_breakpoint(temp_32bit);
			}
		}
	} else if (strcmp(token0, "sbc") == 0) {
		pc.vicv->clear_scanline_breakpoints();
		debug_console_put_char('\n');
		debug_console_print("all scanline breakpoints removed\n");
	} else if (strcmp(token0, "ver") == 0) {
		debug_console_put_char('\n');
		debug_console_version();
	} else if (strcmp(token0, "win") == 0) {
		token1 = strtok(NULL, " ");
		debug_console_put_char('\n');
		if (token1 == NULL) {
			host_video.reset_window_size();
			snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n",
				 host_video.current_window_width(), host_video.current_window_height());
			debug_console_print(command_help_string);
		} else if (strcmp(token1, "+") == 0) {
			host_video.increase_window_size();
			snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n",
				 host_video.current_window_width(), host_video.current_window_height());
			debug_console_print(command_help_string);
		} else if (strcmp(token1, "-") == 0) {
			host_video.decrease_window_size();
			snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n",
				 host_video.current_window_width(), host_video.current_window_height());
			debug_console_print(command_help_string);
		} else {
			snprintf(command_help_string, 256,
				 "error: unknown argument '%s'\n", token1);
			debug_console_print(command_help_string);
		}
	} else {
		debug_console_put_char('\n');
		snprintf(command_help_string, 256,
			 "error: unknown command '%s'\n", token0);
		debug_console_print(command_help_string);
	}
	if (have_prompt)
		debug_console_prompt();
}

void E64::debug_command_dump_cpu_status()
{
    debug_console_put_char('\n');
    pc.m68k->dump_registers(command_help_string);
    debug_console_print(command_help_string);
    debug_console_put_char('\n');
}

void E64::debug_command_memory_dump(uint32_t address, int rows)
{
    address = address & 0xfffffffe;  // only even addresses allowed
    
    for(int i=0; i<rows; i++ )
    {
        uint32_t temp_address = address;
        snprintf(command_help_string, 256, "\r:%06x ", temp_address);
        debug_console_print(command_help_string);
        for(int i=0; i<8; i++)
        {
            snprintf(command_help_string, 256, "%02x", pc.mmu->read_memory_8(temp_address));
            debug_console_print(command_help_string);
            if(i & 0b1) debug_console_put_char(' ');
            temp_address ++;
            temp_address &= RAM_SIZE - 1;
        }
        
        debug_console.current_background_color = COBALT_02;
        
        temp_address = address;
        for(int i=0; i<8; i++)
        {
            uint8_t temp_byte = pc.mmu->read_memory_8(temp_address);
            debug_console_put_screencode( temp_byte );
            temp_address++;
        }
        address += 8;
        address &= RAM_SIZE - 1;
        
        debug_console.current_background_color = COBALT_01;
        
        debug_console.cursor_pos -= 28;
    }
}

void E64::debug_command_memory_character_dump(uint32_t address, int rows)
{
    for(int i=0; i<rows; i++ )
    {
        uint32_t temp_address = address;
        snprintf(command_help_string, 256, "\r;%06x", temp_address);
        debug_console_print(command_help_string);
        for(int i=0; i<16; i++)
        {
            if( (i & 1) == 0 ) debug_console_put_char(' ');
            // important: vicv and blitter are not able to see roms and i/o adresses, so don't use read_memory_8
            snprintf(command_help_string, 256, "%02x", pc.mmu->ram[temp_address]);
            debug_console_print(command_help_string);
            temp_address++;
            temp_address &= RAM_SIZE - 1;
        }

        debug_console_put_char(' ');
        
        temp_address = address;
        for(int i=0; i<8; i++)
        {
            debug_console.current_background_color = *(uint16_t *)(&(pc.mmu->ram[temp_address & 0x00ffffff]));
            debug_console_put_char(' ');
            temp_address += 2;
        }

        debug_console.current_background_color = COBALT_01;
        
        address += 16;
        address &= RAM_SIZE - 1;
        debug_console.cursor_pos -= 48;
    }
}

void E64::debug_command_memory_binary_dump(uint32_t address, int rows)
{
	for (int i=0; i<rows; i++) {
		uint8_t temp_byte = pc.mmu->read_memory_8(address);
		
		snprintf(command_help_string, 256, "\r'%06x ", address);
		debug_console_print(command_help_string);

		snprintf(command_help_string, 256, "%02x ", temp_byte);
		debug_console_print(command_help_string);
		
		for (int i=0; i<8; i++) {
			if (temp_byte & 0x80)
				debug_console_put_char('*');
			else
				debug_console_put_char('.');
			temp_byte = temp_byte << 1;
		}

		debug_console_put_char(' ');

		debug_console.current_background_color = COBALT_02;
		debug_console_put_screencode( pc.mmu->read_memory_8(address));
		debug_console.current_background_color = COBALT_01;
		
		if (address & 0b1)
			snprintf(command_help_string, 256, "      ");
		else
			snprintf(command_help_string, 256, " %04x ",
				 pc.mmu->read_memory_16(address));
		debug_console_print(command_help_string);
		
		debug_console.current_background_color =
			pc.mmu->read_memory_8(address & 0xfffffe) |
			pc.mmu->read_memory_8((address & 0xfffffe)+1) << 8;
		
		debug_console_print("  ");
		
		debug_console.current_background_color = COBALT_01;
		
		address++;
		address &= RAM_SIZE - 1;
	}
	debug_console.cursor_pos -= 21;
}


/*
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 * from: https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
 *
 * This function is slightly adopted to check for true values. It returns false
 * when there's wrong input.
 */
bool E64::debug_command_hex_string_to_int(const char *temp_string, uint32_t *return_value)
{
    uint32_t val = 0;
    while (*temp_string)
    {
        // get current character then increment
        uint8_t byte = *temp_string++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <='f')
        {
            byte = byte - 'a' + 10;
        }
        else if (byte >= 'A' && byte <='F')
        {
            byte = byte - 'A' + 10;
        }
        else
        {
            // we have a problem, return false and do not write the return value
            return false;
        }
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xf);
    }
    *return_value = val;
    return true;
}

void E64::debug_command_single_step_cpu()
{
	debug_console_cursor_deactivate();
	pc.run(0);
	debug_console_cursor_activate();
}

void E64::debug_command_enter_monitor_line(char *string_to_parse_and_exec)
{
    uint32_t address;
    uint32_t arg0, arg1, arg2, arg3;
    
    string_to_parse_and_exec[7]  = '\0';
    string_to_parse_and_exec[12] = '\0';
    string_to_parse_and_exec[17] = '\0';
    string_to_parse_and_exec[22] = '\0';
    string_to_parse_and_exec[27] = '\0';
    
    if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[1], &address) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 1;
        debug_console_print("??????\n");
    }
    else if( address & 0b1 )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 1;
        debug_console_print("??????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[8], &arg0) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 8;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[13], &arg1) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 13;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[18], &arg2) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 18;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[23], &arg3) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 23;
        debug_console_print("????\n");
    }
    else
    {
        uint32_t original_address = address;
        
        arg0 &= 0xffff;
        arg1 &= 0xffff;
        arg2 &= 0xffff;
        arg3 &= 0xffff;
        
        pc.mmu->write_memory_16(address, (uint16_t)arg0); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg1); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg2); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg3); address +=2; address &= 0xffffff;

        debug_console_put_char('\r');
        
        debug_command_memory_dump(original_address, 1);
        
        original_address += 8;
        original_address &= 0xffffff;
        
        snprintf(command_help_string, 256, "\n:%06x ", original_address);
        debug_console_print(command_help_string);
    }
}

//  rewrite this / merge this with the other enter line function?
void E64::debug_command_enter_monitor_character_line(char *string_to_parse_and_exec)
{
    uint32_t address;
    uint32_t arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7;
    
    string_to_parse_and_exec[7]  = '\0';
    string_to_parse_and_exec[12] = '\0';
    string_to_parse_and_exec[17] = '\0';
    string_to_parse_and_exec[22] = '\0';
    string_to_parse_and_exec[27] = '\0';
    string_to_parse_and_exec[32] = '\0';
    string_to_parse_and_exec[37] = '\0';
    string_to_parse_and_exec[42] = '\0';
    string_to_parse_and_exec[47] = '\0';
    
    if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[1], &address) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 1;
        debug_console_print("??????\n");
    }
    else if( address & 0b1 )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 1;
        debug_console_print("??????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[8], &arg0) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 8;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[13], &arg1) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 13;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[18], &arg2) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 18;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[23], &arg3) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 23;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[28], &arg4) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 28;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[33], &arg5) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 33;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[38], &arg6) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 38;
        debug_console_print("????\n");
    }
    else if( !debug_command_hex_string_to_int(&string_to_parse_and_exec[43], &arg7) )
    {
        debug_console_put_char('\r');
        debug_console.cursor_pos += 43;
        debug_console_print("????\n");
    }
    else
    {
        uint32_t original_address = address;
        
        arg0 &= 0xffff;
        arg1 &= 0xffff;
        arg2 &= 0xffff;
        arg3 &= 0xffff;
        arg4 &= 0xffff;
        arg5 &= 0xffff;
        arg6 &= 0xffff;
        arg7 &= 0xffff;
        
        pc.mmu->write_memory_16(address, (uint16_t)arg0); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg1); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg2); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg3); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg4); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg5); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg6); address +=2; address &= 0xffffff;
        pc.mmu->write_memory_16(address, (uint16_t)arg7); address +=2; address &= 0xffffff;

        debug_console_put_char('\r');
        
        debug_command_memory_character_dump(original_address, 1);
        
        original_address += 16;
        original_address &= 0xffffff;
        
        snprintf(command_help_string, 256, "\n;%06x ", original_address);
        debug_console_print(command_help_string);
    }
}

void E64::debug_command_enter_monitor_binary_line(char *string_to_parse_and_exec)
{
	uint32_t address;
	uint32_t arg0;
	
	string_to_parse_and_exec[7]  = '\0';
	string_to_parse_and_exec[10] = '\0';
	
	if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[1], &address)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 1;
		debug_console_print("??????\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[8], &arg0)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 8;
		debug_console_print("??\n");
	} else {
		arg0 &= 0xff;
		pc.mmu->write_memory_8(address, (uint8_t)arg0);
		debug_console_put_char('\r');
		debug_command_memory_binary_dump(address, 1);
		address++;
		address &= 0xffffff;
		snprintf(command_help_string, 256, "\n\'%06x ", address);
		debug_console_print(command_help_string);
	}
}

void E64::debug_command_fd_dump(uint32_t address, int rows)
{
	address &= 0xfffffff8;

	if (address == 0xfffffff8)
		address = pc.fd0->disk_size() - 0x08;
	if (address >= pc.fd0->disk_size())
		address = 0;
	
	for (int i=0; i<rows; i++ ) {
		uint16_t sector = address / pc.fd0->bytes_per_sector();
		uint16_t offset = address - (sector * pc.fd0->bytes_per_sector());
		snprintf(command_help_string, 256, "\r\"%1x:%08x:%04x ", 0, sector, offset);
		debug_console_print(command_help_string);
		for (int i=0; i<8; i++) {
			snprintf(command_help_string, 256, "%02x ",
				 pc.fd0->disk_contents[address+i]);
			debug_console_print(command_help_string);
		}
		debug_console.current_background_color = COBALT_02;
		for (int i=0; i<8; i++)
			debug_console_put_screencode(pc.fd0->disk_contents[address+i]);
		debug_console.current_background_color = COBALT_01;
		address += 0x8;
	}
	debug_console.cursor_pos -= 32;
}

void E64::debug_command_enter_monitor_disk_line(char *string_to_parse_and_exec)
{
	uint32_t sector = 0;
	uint32_t offset = 0;

	string_to_parse_and_exec[11] = '\0';
	string_to_parse_and_exec[16] = '\0';

	uint32_t arg0 = 0;
	uint32_t arg1 = 0;
	uint32_t arg2 = 0;
	uint32_t arg3 = 0;
	uint32_t arg4 = 0;
	uint32_t arg5 = 0;
	uint32_t arg6 = 0;
	uint32_t arg7 = 0;

	string_to_parse_and_exec[19] = '\0';
	string_to_parse_and_exec[22] = '\0';
	string_to_parse_and_exec[25] = '\0';
	string_to_parse_and_exec[28] = '\0';
	string_to_parse_and_exec[31] = '\0';
	string_to_parse_and_exec[34] = '\0';
	string_to_parse_and_exec[37] = '\0';
	string_to_parse_and_exec[40] = '\0';

	
	if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[3], &sector)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 3;
		debug_console_print("????????\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[12], &offset)) {
	       debug_console_put_char('\r');
	       debug_console.cursor_pos += 12;
	       debug_console_print("????\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[17], &arg0)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 17;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[20], &arg1)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 20;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[23], &arg2)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 23;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[26], &arg3)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 26;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[29], &arg4)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 29;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[32], &arg5)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 32;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[35], &arg6)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 35;
		debug_console_print("??\n");
	} else if (!debug_command_hex_string_to_int(&string_to_parse_and_exec[38], &arg7)) {
		debug_console_put_char('\r');
		debug_console.cursor_pos += 38;
		debug_console_print("??\n");
	} else {
		uint32_t temp_pos = ((sector * pc.fd0->bytes_per_sector()) + offset);
		if ((temp_pos & 0b111) || (temp_pos >= pc.fd0->disk_size())) {
			debug_console_print("\nerror: illegal sector:offset\n");
		} else {
			pc.fd0->disk_contents[temp_pos + 0x0] = (arg0 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x1] = (arg1 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x2] = (arg2 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x3] = (arg3 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x4] = (arg4 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x5] = (arg5 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x6] = (arg6 & 0xff);
			pc.fd0->disk_contents[temp_pos + 0x7] = (arg7 & 0xff);
			
			debug_console_put_char('\r');
			debug_command_fd_dump(temp_pos, 1);
			temp_pos += 0x8;
			if (temp_pos >= pc.fd0->disk_size())
				temp_pos = 0;
			
			sector = temp_pos / pc.fd0->bytes_per_sector();
			offset = temp_pos - (sector * pc.fd0->bytes_per_sector());
			
			snprintf(command_help_string, 256, "\n\"%1x:%08x:%04x ", 0, sector, offset);
			debug_console_print(command_help_string);
		}
	}
}
