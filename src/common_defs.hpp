//  common_defs.h
//  E64-II
//
//  Copyright © 2017-2020 elmerucr. All rights reserved.
//
//  General definitions for the whole project

#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <cstdint>
#include "video.hpp"
#include "machine.hpp"
#include "pid_delay.hpp"

#define E64_MAJOR_VERSION       0
#define E64_MINOR_VERSION       4
#define E64_BUILD               20200319
#define E64_YEAR                2020

#define CPU_CLOCK_SPEED             5600000
#define IO_CIA_PAGE                 0xfe03
#define IO_VICV_PAGE                0xfe04
#define IO_SND_PAGE                 0xfe05
#define IO_TIMER_PAGE               0xfe06
#define IO_BLITTER_PAGE             0xfe07
#define IO_RESET_VECTOR_MASK        0xfffff8
#define IO_KERNEL_MASK              0xff

// some objects are visible at global level
extern E64::video host_video;
extern E64::pid_delay frame_delay;
extern E64::machine computer;
extern const uint8_t ascii_to_screencode[];
extern const char screencode_to_ascii[];

#define RAM_SIZE                    0x1000000                           // 16mb system

#define FPS                         50
#define VICV_PIXELS_PER_SCANLINE    512
#define VICV_PIXELS_HBLANK          128
#define VICV_SCANLINES              320
#define VICV_SCANLINES_VBLANK       30

#define VICV_DOT_CLOCK_SPEED        (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_SCANLINES_VBLANK)*FPS
#define BLITTER_DOT_CLOCK_SPEED     (8*VICV_DOT_CLOCK_SPEED)
#define CPU_CYCLES_PER_SCANLINE     (CPU_CLOCK_SPEED/((VICV_SCANLINES+VICV_SCANLINES_VBLANK)*FPS))

#define SID_CLOCK_SPEED             985248
#define SAMPLE_RATE                 44100
#define AUDIO_BUFFER_SIZE           8192.0

// VirtualC64 colors
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

#define C64_GREY_00 0x00f0
#define C64_GREY_01 0x11f1
#define C64_GREY_02 0x22f2
#define C64_GREY_03 0x33f3
#define C64_GREY_04 0x44f4
#define C64_GREY_05 0x55f5
#define C64_GREY_06 0x66f6
#define C64_GREY_07 0x77f7
#define C64_GREY_08 0x88f8
#define C64_GREY_09 0x99f9
#define C64_GREY_10 0xaafa
#define C64_GREY_11 0xbbfb
#define C64_GREY_12 0xccfc
#define C64_GREY_13 0xddfd
#define C64_GREY_14 0xeefe
#define C64_GREY_15 0xffff

#define C64_GRN_00  0xff000000
#define C64_GRN_01  0xff0b110a
#define C64_GRN_02  0xff162214
#define C64_GRN_03  0xff21331e
#define C64_GRN_04  0xff2c4428
#define C64_GRN_05  0xff375533
#define C64_GRN_06  0xff42663d
#define C64_GRN_07  0xff4d7747
#define C64_GRN_08  0xff598851
#define C64_GRN_09  0xff64995c
#define C64_GRN_10  0xff6faa66      // This one comes closest to the original C64_GREEN
#define C64_GRN_11  0xff7abb70
#define C64_GRN_12  0xff85cc7a
#define C64_GRN_13  0xff90dd85
#define C64_GRN_14  0xff9bee8f
#define C64_GRN_15  0xffa7ff99

#define COBALT_00   0x00f0
#define COBALT_01   0x12f1
#define COBALT_02   0x24f2
#define COBALT_03   0x36f3
#define COBALT_04   0x48f4
#define COBALT_05   0x5af5
#define COBALT_06   0x6cf6
#define COBALT_07   0x7ef7

// amber
#define AMBER_00   0x00f0
#define AMBER_01   0x11f2
#define AMBER_02   0x22f4
#define AMBER_03   0x33f6
#define AMBER_04   0x44f8
#define AMBER_05   0x55fa
#define AMBER_06   0x66fc
#define AMBER_07   0x77fe

#define ASCII_NULL          0x00    // null
#define ASCII_CR            0x0d    // carriage return
#define ASCII_LF            0x0a    // linefeed
#define ASCII_SPACE         0x20    // space
#define ASCII_UNDERSCORE    0x5f    // _

#endif
