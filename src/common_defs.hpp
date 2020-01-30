//  common_defs.h
//  E64-II
//
//  Copyright © 2017 elmerucr. All rights reserved.
//
//  General definitions for the whole project

#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <cstdint>
#include "machine.hpp"
#include "pid_delay.hpp"

#define E64_MAJOR_VERSION       0
#define E64_MINOR_VERSION       4
#define E64_BUILD               20200131
#define E64_YEAR                2020

#define CPU_CLOCK_SPEED             5600000     // 8MHz system
#define IO_CIA_PAGE                 0xfe03
#define IO_VICV_PAGE                0xfe04
#define IO_SND_PAGE                 0xfe05
#define IO_TIMER_PAGE               0xfe06
#define IO_RESET_VECTOR_MASK        0xfffff8
#define IO_KERNEL_MASK              0xff
#define DEBUGGER_FOREGROUND_COLOR   0x3b
#define DEBUGGER_BACKGROUND_COLOR   0x34

// some objects must be visible at global level
extern E64::pid_delay frame_delay;
extern E64::machine computer;
extern const uint8_t ascii_to_screencode[];
extern const char screencode_to_ascii[];

#define RAM_SIZE                    0x1000000                           // 16mb system

#define FPS                         50
#define VICV_PIXELS_PER_SCANLINE    512
#define VICV_PIXELS_HBLANK          128
#define VICV_SCANLINES              320
#define VICV_PIXELS_VBLANK          30

#define VICV_DOT_CLOCK_SPEED        (VICV_PIXELS_PER_SCANLINE+VICV_PIXELS_HBLANK)*(VICV_SCANLINES+VICV_PIXELS_VBLANK)*FPS
#define CPU_CYCLES_PER_SCANLINE     (CPU_CLOCK_SPEED/( (VICV_SCANLINES+VICV_PIXELS_VBLANK) * FPS) )

#define SID_CLOCK_SPEED             985248
#define SAMPLE_RATE                 44100
#define AUDIO_BUFFER_SIZE           8192.0

// E64 elmerucr colors
//#define C64_BLACK       0xff000000  // color 0 - argb8888
//#define C64_BLACK       0xff161616  // color 0 - argb8888 ==> A more 'realistic' variant of black
//#define C64_WHITE       0xffffffff  // color 1
//#define C64_RED         0xff965247  // color 2
//#define C64_CYAN        0xff80bfc7  // color 3
//#define C64_PURPLE      0xff985aa1  // color 4
//#define C64_GREEN       0xff70ab67  // color 5
//#define C64_BLUE        0xff50449c  // color 6
//#define C64_YELLOW      0xffcdd592  // color 7
//#define C64_ORANGE      0xff9a6b42  // color 8
//#define C64_BROWN       0xff6a551d  // color 9
//#define C64_LIGHTRED    0xffc18279  // color 10
//#define C64_DARKGREY    0xff636363  // color 11
//#define C64_GREY        0xff8b8b8b  // color 12
//#define C64_LIGHTGREEN  0xffabe2a4  // color 13
//#define C64_LIGHTBLUE   0xff8a7fce  // color 14
//#define C64_LIGHTGREY   0xffafafaf  // color 15

// VirtualC64 colors
#define C64_BLACK       0xff000000  // color 0
#define C64_WHITE       0xffffffff  // color 1
#define C64_RED         0xff783839  // color 2
#define C64_CYAN        0xff89cbc7  // color 3
#define C64_PURPLE      0xff834292  // color 4
#define C64_GREEN       0xff6ba959  // color 5
#define C64_BLUE        0xff2b2f95  // color 6
#define C64_YELLOW      0xffeeef82  // color 7
#define C64_ORANGE      0xff855231  // color 8
#define C64_BROWN       0xff50390e  // color 9
#define C64_LIGHTRED    0xffb87072  // color 10
#define C64_DARKGREY    0xff494949  // color 11
#define C64_GREY        0xff7b7b7b  // color 12
#define C64_LIGHTGREEN  0xffbcfca9  // color 13
#define C64_LIGHTBLUE   0xff6c6fe3  // color 14
#define C64_LIGHTGREY   0xffb1b1b1  // color 15

#define C64_GREY_00 0xff000000
#define C64_GREY_01 0xff111111
#define C64_GREY_02 0xff222222
#define C64_GREY_03 0xff333333
#define C64_GREY_04 0xff444444
#define C64_GREY_05 0xff555555
#define C64_GREY_06 0xff666666
#define C64_GREY_07 0xff777777
#define C64_GREY_08 0xff888888
#define C64_GREY_09 0xff999999
#define C64_GREY_10 0xffaaaaaa
#define C64_GREY_11 0xffbbbbbb
#define C64_GREY_12 0xffcccccc
#define C64_GREY_13 0xffdddddd
#define C64_GREY_14 0xffeeeeee
#define C64_GREY_15 0xffffffff

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

#define C64_COBALT_00  0xff000000
#define C64_COBALT_01  0xff070811
#define C64_COBALT_02  0xff0e1022
#define C64_COBALT_03  0xff151833
#define C64_COBALT_04  0xff1c2044
#define C64_COBALT_05  0xff232855
#define C64_COBALT_06  0xff2a3066
#define C64_COBALT_07  0xff313877
#define C64_COBALT_08  0xff384088
#define C64_COBALT_09  0xff3f4899
#define C64_COBALT_10  0xff4650aa
#define C64_COBALT_11  0xff4d58bb
#define C64_COBALT_12  0xff5460cc
#define C64_COBALT_13  0xff5b68dd
#define C64_COBALT_14  0xff6270ee
#define C64_COBALT_15  0xff6978ff

#define C64_AMBER_00 0xff000000      // kimbie dark from Visual Studio Code
#define C64_AMBER_01 0xff110c08
#define C64_AMBER_02 0xff221911      // background
#define C64_AMBER_03 0xff33261a
#define C64_AMBER_04 0xff443323
#define C64_AMBER_05 0xff553f2c
#define C64_AMBER_06 0xff664c35      // background select
#define C64_AMBER_07 0xff77593e
#define C64_AMBER_08 0xff886647
#define C64_AMBER_09 0xff99724f
#define C64_AMBER_10 0xffaa7f58
#define C64_AMBER_11 0xffbb8c61
#define C64_AMBER_12 0xffcc996a
#define C64_AMBER_13 0xffdda573      // text
#define C64_AMBER_14 0xffeeb27c
#define C64_AMBER_15 0xffffbf85

#define ASCII_NULL          0x00    // null
#define ASCII_CR            0x0d    // carriage return
#define ASCII_LF            0x0a    // linefeed
#define ASCII_SPACE         0x20    // space
#define ASCII_UNDERSCORE    0x5f    // _

#endif
