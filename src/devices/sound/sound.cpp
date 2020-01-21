//  sound.cpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include "sound.hpp"
#include "sdl2.hpp"
#include "common_defs.hpp"

E64::sound::sound(bool big_endian)
{
    for(int i=0; i<32; i++) register_index[i] = i;
    if( big_endian == true )
    {
        // voice 1
        register_index[0] = 1;      // freq
        register_index[1] = 0;

        register_index[2] = 3;      // pulse width
        register_index[3] = 2;

        // voice 2
        register_index[7] = 8;      // freq
        register_index[8] = 7;
        
        register_index[9] = 10;     // pulse width
        register_index[10] = 9;
        
        // voice 3
        register_index[14] = 15;    // freq
        register_index[15] = 14;
        
        register_index[16] = 17;    // pulse width
        register_index[17] = 16;
        
        // filter
        register_index[21] = 22;    // fc
        register_index[22] = 21;
    }
    
    for(int i = 0; i<4; i++)
    {
        // set chip model
        sid[i].set_chip_model(MOS6581);
        // In order to make SID produce the same pitch as it would in a PAL C64,
        // we must use the exact same clock speed (985248Hz). We'll have the same
        // frequencies, envelope, etc... as in the real thing.
        // Using a sort of Bresenham algorithm it will be possible to "connect" the
        // clock of SID to any other clock. One condition: the other one, usually
        // the cpu clock, must be faster. See C256_bresenham_clocks source files.
        sid[i].set_sampling_parameters(SID_CLOCK_SPEED, SAMPLE_FAST, SAMPLE_RATE);
        sid[i].enable_filter(true);
        sid[i].reset();
        
        // reset cycle counters
        delta_t_sid0 = 0;
        delta_t_sid1 = 0;
        
        // sid 0 balance reset;
        balance_registers[0] = 0x00;    // left
        balance_registers[1] = 0x00;    // right
        // sid 1 balance reset;
        balance_registers[2] = 0x00;    // left
        balance_registers[3] = 0x00;    // right
        // sid 2 balance reset;
        balance_registers[4] = 0x00;    // left
        balance_registers[5] = 0x00;    // right
        // sid 3 balance reset;
        balance_registers[6] = 0x00;    // left
        balance_registers[7] = 0x00;    // right
    }
}

E64::sound::~sound()
{
    // nothing
}

uint8_t E64::sound::read_byte(uint8_t address)
{
    if(address & 0x80)
    {
        return balance_registers[address & 0x07];
    }
    else
    {
        return sid[(address & 0x60) >> 5].read( register_index[address & 0x1f] ); // NEEDS CHECKING!!!
    }
}

void E64::sound::write_byte(uint8_t address, uint8_t byte)
{
    // each sid requires 32 addresses (of which 29 are used)
    // bit 7 of address determines if a sid chip should be addressed
    // bits 5 and 6 determine which sid chip of the four
    // bits 0 to 4 are the actual address within one sid chip
    if(address & 0x80)
    {
        balance_registers[address & 0x07] = byte;
    }
    else
    {
        sid[(address & 0x60) >> 5].write( register_index[address & 0x1f], byte); // NEEDS CHECKING!!!
    }
}

void E64::sound::run(uint32_t number_of_cycles)
{
    delta_t_sid0 += number_of_cycles;
    delta_t_sid1 = delta_t_sid0;
    // clock(delta_t, buf, maxNoOfSamples) function:
    //   This function returns the number of samples written by the SID chip.
    //   delta_t is a REFERENCE to the number of cycles to be processed
    //   buf is the memory area in which data should be written
    //   maxNoOfSamples (internal size of the presented buffer)
    int n = sid[0].clock(delta_t_sid0, sample_buffer_mono_sid0, 65536);
    sid[1].clock(delta_t_sid1, sample_buffer_mono_sid1, 65536);
    
    for(int i=0; i<n; i++)
    {
        // left channel
        sample_buffer_stereo[2*i] = (sample_buffer_mono_sid0[i] * balance_registers[0]) / 255;
        sample_buffer_stereo[2*i] += (sample_buffer_mono_sid1[i] * balance_registers[2]) / 255;
        // right channel
        sample_buffer_stereo[(2*i)+1] = (sample_buffer_mono_sid0[i] * balance_registers[1]) / 255;
        sample_buffer_stereo[(2*i)+1] += (sample_buffer_mono_sid1[i] * balance_registers[3]) / 255;
    }
    E64::sdl2_queue_audio((void *)sample_buffer_stereo, 2 * n * sizeof(int16_t));
}

void E64::sound::reset()
{
    sid[0].reset();
    sid[1].reset();
    sid[2].reset();
    sid[3].reset();
}
