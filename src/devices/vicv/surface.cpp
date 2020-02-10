//  surface.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "surface.hpp"

E64::surface::surface()
{
    set_width(5);
    set_height(2);
    x_position = 8;
    y_position = 64;
}

uint8_t E64::surface::read(uint8_t address)
{
    switch(address & 0b00001111)
    {
        case SURFACE_REG_WIDTH:
            
            break;
        case SURFACE_REG_HEIGHT:
            //
            break;
    }
    return registers[address & 0b00001111];
}

void    write(uint8_t address, uint8_t byte);

void E64::surface::set_width(uint8_t width)
{
    switch(width & 0b00000111)
    {
        case 0b000: width_mask = 0b1111111111111000; break;
        case 0b001: width_mask = 0b1111111111110000; break;
        case 0b010: width_mask = 0b1111111111100000; break;
        case 0b011: width_mask = 0b1111111111000000; break;
        case 0b100: width_mask = 0b1111111110000000; break;
        case 0b101: width_mask = 0b1111111100000000; break;
        case 0b110: width_mask = 0b1111111000000000; break;
        case 0b111: width_mask = 0b1111110000000000; break;
    }
}

void E64::surface::set_height(uint8_t height)
{
    switch(height & 0b00000111)
    {
        case 0b000: height_mask = 0b1111111111111000; break;
        case 0b001: height_mask = 0b1111111111110000; break;
        case 0b010: height_mask = 0b1111111111100000; break;
        case 0b011: height_mask = 0b1111111111000000; break;
        case 0b100: height_mask = 0b1111111110000000; break;
        case 0b101: height_mask = 0b1111111100000000; break;
        case 0b110: height_mask = 0b1111111000000000; break;
        case 0b111: height_mask = 0b1111110000000000; break;
    }
}

#define X_RELATIVE  (x - x_position)
#define Y_RELATIVE  (y - y_position)
#define X_IN_SURFACE ((width_mask & X_RELATIVE) == 0)
#define Y_IN_SURFACE ((height_mask & Y_RELATIVE) == 0)

bool E64::surface::render_pixel(int16_t x, int16_t y)
{
    if( X_IN_SURFACE && Y_IN_SURFACE ) return true;
    return false;
}
