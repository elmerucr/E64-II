//  common.h
//  E64-II
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.
//
//  General definitions for the whole project

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include "video.hpp"
#include "machine.hpp"
#include "stats.hpp"

#define E64_II_MAJOR_VERSION    0
#define E64_II_MINOR_VERSION    5
#define E64_II_BUILD            20200705
#define E64_II_YEAR             2020

//  Some global objects
extern E64::video       host_video;
extern E64::machine     pc;
extern E64::stats       statistics;
extern const uint8_t    ascii_to_screencode[];
extern const char       screencode_to_ascii[];

#define RAM_SIZE                    0x1000000   // 16mb system, don't change this number

/*  System uses standard 720p60 HDMI output @ half the dimensions.
 *  In a 'distant' future, this will come in handy with real hardware.
 *  See also:
 *  https://timetoexplore.net/blog/video-timings-vga-720p-1080p
 *
 *  Name         1280x720p60
 *  Standard       CTA-770.3
 *  VIC                    4
 *  Short Name          720p
 *  Aspect Ratio        16:9
 *
 *  Pixel Clock       74.250 MHz
 *  TMDS Clock       742.500 MHz
 *  Pixel Time          13.5 ns ±0.5%
 *  Horizontal Freq.  45.000 kHz
 *  Line Time           22.2 μs
 *  Vertical Freq.    60.000 Hz
 *  Frame Time          16.7 ms
 *
 *  Horizontal Timings
 *  Active Pixels       1280
 *  Front Porch          110
 *  Sync Width            40
 *  Back Porch           220
 *  Blanking Total       370
 *  Total Pixels        1650
 *  Sync Polarity        pos
 *
 *  Vertical Timings
 *  Active Lines         720
 *  Front Porch            5
 *  Sync Width             5
 *  Back Porch            20
 *  Blanking Total        30
 *  Total Lines          750
 *  Sync Polarity        pos
 *
 *  Active Pixels    921,600
 *  Data Rate           1.78 Gbps
 *
 *  Frame Memory (Kbits)
 *   8-bit Memory      7,200
 *  12-bit Memory     10,800
 *  24-bit Memory     21,600
 *  32-bit Memory     28,800
 *
 */
#define VICV_PIXELS_PER_SCANLINE    640
#define VICV_PIXELS_HBLANK          185
#define VICV_SCANLINES              360
#define VICV_SCANLINES_VBLANK       15

//  These two macros are defined for use within the debugger.
#define VICV_CHAR_COLUMNS           (VICV_PIXELS_PER_SCANLINE / 8)
#define VICV_CHAR_ROWS              (VICV_SCANLINES / 8)


#define FPS                         60

#define VICV_DOT_CLOCK_SPEED        (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK)*FPS
#define BLITTER_DOT_CLOCK_SPEED     (4*VICV_DOT_CLOCK_SPEED)
#define CPU_CLOCK_SPEED             (VICV_DOT_CLOCK_SPEED/2)

#define SID_CLOCK_SPEED             985248
#define SAMPLE_RATE                 44100
#define AUDIO_BUFFER_SIZE           8192.0

//  C64 (VirtualC64)
#define C64_BLACK       0x00f0
#define C64_WHITE       0xffff
#define C64_RED         0x33f7
#define C64_CYAN        0xccf8
#define C64_PURPLE      0x49f8
#define C64_GREEN       0xa5f6
#define C64_BLUE        0x39f3
#define C64_YELLOW      0xe8fe
#define C64_ORANGE      0x53f8
#define C64_BROWN       0x31f5
#define C64_LIGHTRED    0x77fb
#define C64_DARKGREY    0x44f4
#define C64_GREY        0x77f7
#define C64_LIGHTGREEN  0xfafb
#define C64_LIGHTBLUE   0x7df6
#define C64_LIGHTGREY   0xaafa

//  Grey
#define GREY_00 0x00f0
#define GREY_01 0x11f1
#define GREY_02 0x22f2
#define GREY_03 0x33f3
#define GREY_04 0x44f4
#define GREY_05 0x55f5
#define GREY_06 0x66f6
#define GREY_07 0x77f7
#define GREY_08 0x88f8
#define GREY_09 0x99f9
#define GREY_10 0xaafa
#define GREY_11 0xbbfb
#define GREY_12 0xccfc
#define GREY_13 0xddfd
#define GREY_14 0xeefe
#define GREY_15 0xffff

//  Green
#define GREEN_00  0x00f0
#define GREEN_01  0x21f1
#define GREEN_02  0x42f2
#define GREEN_03  0x63f3
#define GREEN_04  0x84f4
#define GREEN_05  0xa5f5
#define GREEN_06  0xc6f6
#define GREEN_07  0xe7f7

//  Cobalt
#define COBALT_00   0x00f0
#define COBALT_01   0x12f1
#define COBALT_02   0x24f2
#define COBALT_03   0x36f3
#define COBALT_04   0x48f4
#define COBALT_05   0x5af5
#define COBALT_06   0x6cf6
#define COBALT_07   0x7ef7

//  Amber
#define AMBER_00   0x00f0
#define AMBER_01   0x11f2
#define AMBER_02   0x22f4
#define AMBER_03   0x33f6
#define AMBER_04   0x44f8
#define AMBER_05   0x55fa
#define AMBER_06   0x66fc
#define AMBER_07   0x77fe

//  Ascii values
#define ASCII_NULL          0x00    // null
#define ASCII_CR            0x0d    // carriage return
#define ASCII_LF            0x0a    // linefeed
#define ASCII_SPACE         0x20    // space
#define ASCII_UNDERSCORE    0x5f    // _

#endif
