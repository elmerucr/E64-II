//  debug_command.cpp
//  E64-II
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <cstdio>
#include <cstring>

#include "common_defs.hpp"
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
        // do nothing
    }
    else if( strcmp(token0, "b") == 0 )
    {
        if(token1 == NULL)
        {
            unsigned int no_of_breakpoints = (unsigned int)computer.m68k_ic->debugger.breakpoints.elements();
            snprintf(command_help_string, 256, "currently %i breakpoint(s) defined\n", no_of_breakpoints);
            debug_console_print(command_help_string);
            if( no_of_breakpoints > 0 )
            {
                snprintf(command_help_string, 256, "\n # address\n");
                debug_console_print(command_help_string);
                for(int i=0; i<no_of_breakpoints; i++)
                {
                    snprintf(command_help_string, 256, "%2u $%06x %1u\n", i, computer.m68k_ic->debugger.breakpoints.guardAddr(i), computer.m68k_ic->debugger.breakpoints.isEnabled(i) ? 1 : 0);
                    debug_console_print(command_help_string);
                }
            }
        }
        else
        {
            uint32_t temp_32bit = debug_command_hex_string_to_int(token1) & (RAM_SIZE - 1);
            computer.m68k_ic->debugger.breakpoints.addAt(temp_32bit);
            snprintf(command_help_string, 256, "breakpoint at $%06x added\n", temp_32bit);
            debug_console_print(command_help_string);
//            if( computer.cpu_ic->is_breakpoint(temp_32bit) )
//            {
//                snprintf(command_help_string, 256, "breakpoint at $%06x removed\n", temp_32bit);
//                debug_console_print(command_help_string);
//                computer.cpu_ic->remove_breakpoint(temp_32bit);
//            }
//            else
//            {
//                snprintf(command_help_string, 256, "breakpoint at $%06x added\n", temp_32bit);
//                debug_console_print(command_help_string);
//                computer.cpu_ic->add_breakpoint(temp_32bit);
//            }
        }
    }
    else if( strcmp(token0, "bar") == 0 )
    {
        debug_console_toggle_status_bar();
    }
    else if( strcmp(token0, "bc") == 0 )
    {
        computer.m68k_ic->debugger.breakpoints.removeAll();
        debug_console_print("all breakpoints removed\n");
    }
    else if( strcmp(token0, "c") == 0 )
    {
        E64::sdl2_wait_until_enter_released();
        computer.switch_to_running();
        // NEEDS WORK
        //computer.cpu_ic->force_next_instruction();
    }
    else if( strcmp(token0, "clear") == 0 )
    {
        debug_console_clear();
    }
//    else if( strcmp(token0, "d") == 0 )
//    {
//        if(token1 == NULL)
//        {
//            debug_command_disassemble(16);
//        }
//        else
//        {
//            debug_console_print("missing functionality: argument takes no. of instr to disassemble\n");
//        }
//    }
    else if( strcmp(token0, "exit") == 0 )
    {
        E64::sdl2_wait_until_enter_released();
        computer.running = false;
    }
    else if( strcmp(token0, "full") == 0 )
    {
        E64::sdl2_toggle_fullscreen();
    }
//    else if( strcmp(token0, "help") == 0 )
//    {
//        if(token1 == NULL)
//        {
//            debug_console_print("Debugger                        Running Mode\n");
//            debug_console_print("========                        ============\n");
//            debug_console_print("F1    next cpu instruction      F9    switch mode\n");
//            debug_console_print("b     cpu breakpoints           F10   toggle runtime stats\n");
//            debug_console_print("c     continue execution\n");
//            debug_console_print("d     disassemble\n");
//            debug_console_print("m     memory dump\n");
//            debug_console_print("r     cpu registers\n");
//            debug_console_print("t     show top of the stack\n");
//            debug_console_print("bar   debug status bar on/off\n");
//            debug_console_print("clear clear screen\n");
//            debug_console_print("exit  exit application\n");
//            debug_console_print("full  toggle window/fullscreen\n");
//            debug_console_print("help  print this help message\n");
//            debug_console_print("irq   irq related commands\n");
//            debug_console_print("nmi   nmi related commands\n");
//            debug_console_print("ver   version information\n");
//            debug_console_print("win   window options\n\n");
//            debug_console_print("<help> <command name> for more info\n");
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
    else if( strcmp(token0, "irq") == 0 )
    {
        snprintf(command_help_string, 256, "debugger irq pin status: %1u\n", computer.toggle_debugger_irq_pin() ? 1 : 0);
        debug_console_print(command_help_string);
        computer.TTL74LS148_ic->update_interrupt_level();
    }
    else if( strcmp(token0, "m") == 0 )
    {
        if( token1 == NULL )
        {
            debug_command_memory_dump(computer.m68k_ic->getPC(), 8);
        }
        else
        {
            uint32_t temp_32bit;
            temp_32bit = debug_command_hex_string_to_int(token1);
            if( token2 == NULL)
            {
                debug_command_memory_dump(temp_32bit & (RAM_SIZE - 1), 8);
            }
        }
    }
    else if( strcmp(token0, "r") == 0 )
    {
        debug_command_dump_cpu_status();
    }
//    else if( strcmp(token0, "t") == 0 )
//    {
//        debug_command_memory_dump(computer.cpu_ic->sp, 1);
//    }
    else if( strcmp(token0, "reset") == 0)
    {
        computer.reset();
    }
    else if( strcmp(token0, "ver") == 0 )
    {
        debug_console_version();
    }
    else if( strcmp(token0, "win") == 0 )
    {
        if(token1 == NULL)
        {
            E64::sdl2_reset_window_size();
        }
        else if( strcmp(token1, "+") == 0 )
        {
            E64::sdl2_increase_window_size();
        }
        else if( strcmp(token1, "-") == 0 )
        {
            E64::sdl2_decrease_window_size();
        }
        else
        {
            snprintf(command_help_string, 256, "error: unknown argument '%s'\n", token1);
            debug_console_print(command_help_string);
        }
    }
    else
    {
        snprintf(command_help_string, 256, "error: unknown command '%s'\n", token0);
        debug_console_print(command_help_string);
    }
}

void E64::debug_command_dump_cpu_status()
{
    computer.m68k_ic->dump_registers(command_help_string);
    debug_console_print(command_help_string);
    
    computer.m68k_ic->disassembleSR(command_help_string);
    debug_console_print(command_help_string);
    
//    debug_console_print("\n\n");
//    debug_console_print(".,");
//    csg65ce02_dasm(computer.cpu_ic->pc, c256_string2, 256);
//    debug_console_print(c256_string2);
    debug_console_putchar('\n');
}

//void E64::debug_command_disassemble(uint8_t number)
//{
//    uint16_t temp_pc = computer.cpu_ic->pc;
//    for(int i = 0; i<number; i++)
//    {
//        debug_console_print(".,");
//        temp_pc += csg65ce02_dasm(temp_pc, c256_string2, 256);
//        debug_console_print(c256_string2);
//        debug_console_putchar('\n');
//    }
//}

void E64::debug_command_memory_dump(uint32_t address, int rows)
{
    for(int i=0; i<rows; i++ )
    {
        uint32_t temp_address = address;
        snprintf(command_help_string, 256, ":%06x", temp_address);
        debug_console_print(command_help_string);
        for(int i=0; i<16; i += 4)
        {
            //if((i & 3) == 0) debug_console_print(" ");
            snprintf(command_help_string, 256, " %04x%04x", computer.mmu_ic->read_memory_16(temp_address), computer.mmu_ic->read_memory_16(temp_address+2));
            debug_console_print(command_help_string);
            temp_address += 4;
            temp_address &= RAM_SIZE - 1;
        }
        //debug_console_print(" ");
        temp_address = address;
        for(int i=0; i<16; i++)
        {
            if((i & 3) == 0) debug_console_putchar(' ');
            uint8_t temp_byte = computer.mmu_ic->read_memory_8(temp_address);
            if( temp_byte == ASCII_LF ) temp_byte = 0x80;
            debug_console_putchar( temp_byte );
            temp_address++;
        }
        address += 16;
        address &= RAM_SIZE - 1;
        debug_console_print("\n");
    }
}

// hex2int
// take a hex string and convert it to a 32bit number (max 8 hex digits)
// from: https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
uint32_t E64::debug_command_hex_string_to_int(const char *temp_string)
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
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

void E64::debug_command_single_step_cpu()
{
    // NEEDS WORK
    //computer.cpu_ic->force_next_instruction();
    computer.run(0);
    computer.TTL74LS148_ic->update_interrupt_level();
}
