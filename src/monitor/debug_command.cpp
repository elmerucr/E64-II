//  debug_command.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <dirent.h>

#include "common.hpp"
#include "debug_command.hpp"
#include "debug_console.hpp"
#include "sdl2.hpp"
#include "debug_status_bar.hpp"

char command_help_string[2048];

void E64::debug_command_execute(char *string_to_parse_and_exec)
{
    // get tokens, if commands could take more arguments, make more ....
    char *token0, *token1, *token2, *token3;
    token0 = strtok( string_to_parse_and_exec, " ");
    token1 = strtok( NULL, " ");
    token2 = strtok( NULL, " ");
    token3 = strtok( NULL, " ");
    if(token0 == NULL)
    {
        debug_console_put_char('\n');
    }
    else if( strcmp(token0, "b") == 0 )
    {
        debug_console_put_char('\n');
        if(token1 == NULL)
        {
            unsigned int no_of_breakpoints = (unsigned int)pc.m68k_ic->debugger.breakpoints.elements();
            snprintf(command_help_string, 256, "currently %i cpu breakpoint(s) defined\n", no_of_breakpoints);
            debug_console_print(command_help_string);
            if( no_of_breakpoints > 0 )
            {
                snprintf(command_help_string, 256, "\n # address active\n");
                debug_console_print(command_help_string);
                for(int i=0; i<no_of_breakpoints; i++)
                {
                    snprintf(command_help_string, 256, "%2u $%06x  %s\n", i, pc.m68k_ic->debugger.breakpoints.guardAddr(i), pc.m68k_ic->debugger.breakpoints.isEnabled(i) ? "yes" : "no");
                    debug_console_print(command_help_string);
                }
            }
        }
        else
        {
            uint32_t temp_32bit;
            if( debug_command_hex_string_to_int(token1, &temp_32bit) )
            {
                temp_32bit &= (RAM_SIZE - 1);
                pc.m68k_ic->debugger.breakpoints.addAt(temp_32bit);
                snprintf(command_help_string, 256, "cpu breakpoint at $%06x added\n", temp_32bit);
                debug_console_print(command_help_string);
            }
            else
            {
                debug_console_print("error: invalid address\n");
            }
        }
    }
    else if( strcmp(token0, "cd") == 0 )
    {
        debug_console_put_char('\n');
        if( token1 == NULL )
        {
            if( chdir(prefs.home_dir) )
            {
                snprintf(command_help_string, 256, "error: no such file or directory: %s\n", prefs.home_dir);
                debug_console_print(command_help_string);
            }
            getcwd(prefs.current_path, 256);    // update current_path
        }
        else
        {
            if( chdir(token1) )
            {
                snprintf(command_help_string, 256, "error: no such file or directory: %s\n", token1);
                debug_console_print(command_help_string);
            }
            getcwd(prefs.current_path, 256);    // update current_path
        }
    }
    else if( strcmp(token0, "bar") == 0 )
    {
        debug_console_put_char('\n');
        debug_console_toggle_status_bar();
    }
    else if( strcmp(token0, "bc") == 0 )
    {
        debug_console_put_char('\n');
        pc.m68k_ic->debugger.breakpoints.removeAll();
        debug_console_print("all cpu breakpoints removed\n");
    }
    else if( strcmp(token0, "c") == 0 )
    {
        debug_console_put_char('\n');
        E64::sdl2_wait_until_enter_released();
        pc.switch_to_running();
    }
    else if( strcmp(token0, "clear") == 0 )
    {
        debug_console_clear();
    }
    else if( strcmp(token0, "exit") == 0 )
    {
        E64::sdl2_wait_until_enter_released();
        pc.running = false;
    }
    else if( strcmp(token0, "full") == 0 )
    {
        debug_console_put_char('\n');
        host_video.toggle_fullscreen();
    }
//    else if( strcmp(token0, "help") == 0 )
//    {
//        if(token1 == NULL)
//        {
//            debug_console_print("Debugger                        Running Mode\n");
//            debug_console_print("========                        ============\n");
//            debug_console_print("F1    next cpu instruction      F9    switch mode\n");
//            debug_console_print("b     cpu breakpoints           F10   toggle runtime stats\n");
//            debug_console_print("bc    clear all cpu breakpoints\n");
//            debug_console_print("c     continue execution\n");
//            debug_console_print("d     disassemble\n");
//            debug_console_print("m     memory dump\n");
//            debug_console_print("r     cpu registers\n");
//            debug_console_print("sb    scanline breakpoints\n);
//            debug_console_print("sbc   clear all scanline breakpoints\n);
//            debug_console_print("bar   debug status bar on/off\n");
//            debug_console_print("clear clear screen\n");
//            debug_console_print("exit  exit application\n");
//            debug_console_print("full  toggle window/fullscreen\n");
//            debug_console_print("help  print this help message\n");
//            debug_console_print("irq   irq related commands\n");
//            debug_console_print("ver   version information\n");
//            debug_console_print("win   window options\n\n");
//            debug_console_print("<help> <command name> for more info on some commands\n");
//        }
//        else if( strcmp(token1, "b") == 0 )
//        {
//            debug_console_print("<b>      current breakpoint settings\n");
//            debug_console_print("<b on>   enable defined breakpoints\n");
//            debug_console_print("<b off>  disable defined breakpoints\n");
//            debug_console_print("<b xxxx> enable or disable a breakpoint at $xxxx\n");
//        }
//        else if( strcmp(token1, "g") == 0 )
//        {
//            debug_console_print("<g xxxx> start execution at $xxxx\n");
//        }
//        else if( strcmp(token1, "n") == 0 )
//        {
//            debug_console_print("<n>        executes the next instruction on the cpu\n");
//            debug_console_print("<n number> executes the next <number> instructions on the cpu\n");
//        }
//        else
//        {
//            snprintf(c256_string2, 256, "error: unrecognized option '%s'\n", token1);
//            debug_console_print(c256_string2);
//        }
//    }
    else if( strcmp(token0, "ls") == 0 )
    {
        debug_console_put_char('\n');
        debug_console_put_char('\n');
        
        DIR *directory = opendir(prefs.current_path);
        struct dirent *entry;
        int files = 0;
        
        while( (entry = readdir(directory)) )
        {
            if( *entry->d_name != '.' )
            {
                files++;
                snprintf(command_help_string, 256, "%s%c",
                         entry->d_name,
                         (entry->d_type) & 0b100 ? '/' : '\0'
                         );
                debug_console_print(command_help_string);
                debug_console_put_char('\n');
            }
        }
        closedir(directory);
        if( files == 0 ) debug_console_print("empty directory\n");
    }
    else if( strcmp(token0, "m") == 0 )
    {
        debug_console_put_char('\n');
        if( token1 == NULL )
        {
            debug_command_memory_dump(pc.m68k_ic->getPC(), 1);
        }
        else
        {
            uint32_t temp_32bit;
            if ( !debug_command_hex_string_to_int(token1, &temp_32bit) )
            {
                debug_console_print("error: invalid address\n");
            }
            else
            {
                debug_command_memory_dump(temp_32bit & (RAM_SIZE - 1), 1);
            }
        }
    }
    else if( strcmp(token0, "mc") == 0 )
    {
        debug_console_put_char('\n');
        if( token1 == NULL )
        {
            debug_console_print("error: need address\n");
        }
        else
        {
            uint32_t temp_32bit;
            if ( !debug_command_hex_string_to_int(token1, &temp_32bit) )
            {
                debug_console_print("error: invalid address\n");
            }
            else
            {
                debug_command_memory_character_dump(temp_32bit & (RAM_SIZE - 1), 1);
            }
        }
    }
    else if( strcmp(token0, "pwd") == 0 )
    {
        debug_console_put_char('\n');
        getcwd(command_help_string, 256);
        debug_console_print(command_help_string);
        debug_console_put_char('\n');
    }
    else if( strcmp(token0, "r") == 0 )
    {
        debug_command_dump_cpu_status();
    }
    else if( strcmp(token0, "reset") == 0)
    {
        debug_console_put_char('\n');
        pc.reset();
        statistics.reset();
    }
    else if( strcmp(token0, "sb") == 0)
    {
        debug_console_put_char('\n');
        if(token1 == NULL)
        {
            unsigned int no_of_scanline_breakpoints = 0;
            for(int i=0; i<1024; i++)
            {
                if(pc.vicv_ic->is_scanline_breakpoint(i)) no_of_scanline_breakpoints++;
            }
            if(no_of_scanline_breakpoints)
            {
                snprintf(command_help_string, 256, "currently %i scanline breakpoint(s) defined at:\n", no_of_scanline_breakpoints);
                debug_console_print(command_help_string);
                for(int i=0; i<1024; i++)
                {
                    if(pc.vicv_ic->is_scanline_breakpoint(i))
                    {
                        snprintf(command_help_string, 256, " %3i\n", i);
                        debug_console_print(command_help_string);
                    }
                }
            }
            else
            {
                debug_console_print("no scanline breakpoints defined\n");
            }
        }
        else
        {
            uint32_t temp_32bit = atoi(token1);
            temp_32bit &= 1023;
            if(pc.vicv_ic->is_scanline_breakpoint(temp_32bit))
            {
                snprintf(command_help_string, 256, "removing scanline breakpoint %i\n", temp_32bit);
                debug_console_print(command_help_string);
                pc.vicv_ic->remove_scanline_breakpoint(temp_32bit);
            }
            else
            {
                snprintf(command_help_string, 256, "adding scanline breakpoint %i\n", temp_32bit);
                debug_console_print(command_help_string);
                pc.vicv_ic->add_scanline_breakpoint(temp_32bit);
            }
        }
    }
    else if( strcmp(token0, "sbc") == 0 )
    {
        pc.vicv_ic->clear_scanline_breakpoints();
        debug_console_put_char('\n');
        debug_console_print("all scanline breakpoints removed\n");
    }
    else if( strcmp(token0, "ver") == 0 )
    {
        debug_console_put_char('\n');
        debug_console_version();
    }
    else if( strcmp(token0, "win") == 0 )
    {
        debug_console_put_char('\n');
        if(token1 == NULL)
        {
            host_video.reset_window_size();
            snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n", host_video.current_window_width(), host_video.current_window_height());
            debug_console_print(command_help_string);
        }
        else if( strcmp(token1, "+") == 0 )
        {
            host_video.increase_window_size();
            snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n", host_video.current_window_width(), host_video.current_window_height());
            debug_console_print(command_help_string);
        }
        else if( strcmp(token1, "-") == 0 )
        {
            host_video.decrease_window_size();
            snprintf(command_help_string, 256, "host system window size is %u x %u pixels\n", host_video.current_window_width(), host_video.current_window_height());
            debug_console_print(command_help_string);
        }
        else
        {
            snprintf(command_help_string, 256, "error: unknown argument '%s'\n", token1);
            debug_console_print(command_help_string);
        }
    }
    else
    {
        debug_console_put_char('\n');
        snprintf(command_help_string, 256, "error: unknown command '%s'\n", token0);
        debug_console_print(command_help_string);
    }
}

void E64::debug_command_dump_cpu_status()
{
    debug_console_put_char('\n');
    pc.m68k_ic->dump_registers(command_help_string);
    debug_console_print(command_help_string);
    debug_console_put_char('\n');
}

void E64::debug_command_memory_dump(uint32_t address, int rows)
{
    for(int i=0; i<rows; i++ )
    {
        uint32_t temp_address = address;
        snprintf(command_help_string, 256, "\r:%06x  ", temp_address);
        debug_console_print(command_help_string);
        for(int i=0; i<8; i++)
        {
            snprintf(command_help_string, 256, "%02x ", pc.mmu_ic->read_memory_8(temp_address));
            debug_console_print(command_help_string);
            temp_address ++;
            temp_address &= RAM_SIZE - 1;
        }
        
        debug_console_put_char(' ');
        
        temp_address = address;
        for(int i=0; i<8; i++)
        {
            uint8_t temp_byte = pc.mmu_ic->read_memory_8(temp_address);
            if( (( temp_byte & 0x7f) == ASCII_LF) || ( (temp_byte & 0x7f) == ASCII_CR) ) temp_byte = 0x80;
            debug_console_put_char( temp_byte );
            temp_address++;
        }
        
        debug_console_put_char(' ');
        debug_console_put_char(' ');
        
        temp_address = address;
        for(int i=0; i<8; i++)
        {
            uint8_t temp_byte = pc.mmu_ic->read_memory_8(temp_address);
            debug_console_put_screencode( temp_byte );
            temp_address++;
        }
        address += 8;
        address &= RAM_SIZE - 1;
        debug_console.cursor_pos -= 43;
    }
}

void E64::debug_command_memory_character_dump(uint32_t address, int rows)
{
    for(int i=0; i<rows; i++ )
    {
        uint32_t temp_address = address;
        snprintf(command_help_string, 256, "\r;%06x ", temp_address);
        debug_console_print(command_help_string);
        for(int i=0; i<16; i++)
        {
            if( (i & 1) == 0 ) debug_console_put_char(' ');
            //snprintf(command_help_string, 256, "%02x", pc.mmu_ic->read_memory_8(temp_address));
            // important: vicv and blitter are not able to see roms and i/o adresses, so don't use read_memory_8
            snprintf(command_help_string, 256, "%02x", pc.mmu_ic->ram[temp_address]);
            debug_console_print(command_help_string);
            temp_address++;
            temp_address &= RAM_SIZE - 1;
        }
        
        debug_console_put_char(' ');
        debug_console_put_char(' ');
        
        temp_address = address;
        for(int i=0; i<8; i++)
        {
            debug_console.current_background_color = *(uint16_t *)(&(pc.mmu_ic->ram[temp_address & 0x00ffffff]));
            debug_console_put_char(' ');
            //if( (( temp_byte & 0x7f) == ASCII_LF) || ( (temp_byte & 0x7f) == ASCII_CR) ) temp_byte = 0x80;
            //debug_console_put_char( temp_byte );
            temp_address += 2;
        }

        debug_console.current_background_color = COBALT_01;
        
        address += 16;
        address &= RAM_SIZE - 1;
        debug_console.cursor_pos -= 49;
    }
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
    pc.run(0);
}
