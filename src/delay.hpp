//  delay.hpp
//  E64
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <cstdint>
#include <chrono>
#include "pid_controller.hpp"

#ifndef DELAY_HPP
#define DELAY_HPP

namespace E64
{

class delay
{
private:
    double current_delay;           // delay per frame in microsec
    
    /*  1000000/FPS is the expected time that passes per frame. This is
     *  also the maximum allowed delay time in the case vsync is not
     *  enabled.
     */
    double nominal_frame_time;
    
    // timers, for calculation of fps
    std::chrono::time_point<std::chrono::steady_clock> now, then;
    int64_t duration;

    uint8_t framecounter;               // keeps track of no of frames since last evaluation
    uint8_t evaluation_interval;        // amount of frames between two evaluations, must be a power of 2!


    uint8_t statistics_framecounter;    // the status bar on the bottom is not refreshed every few frames, but once every few frames
    char statistics_string[256];

    double framerate;
    double smoothed_framerate;
    double mhz;
    double smoothed_mhz;
    double audio_queue_size;
    double smoothed_audio_queue_size;
    double cpu_usage;
    double smoothed_cpu_usage;

    // exp smoothing constant for average_audio_queue_size
    double alpha;

    pid_controller fps_pid;

public:
    // constructor
    delay(double initial_delay);
    
    // process calculations on parameters (fps/mhz/buffersize) and run pid
    void process();
    
    // perform the delay for stable fps (not called when vsync is enabled)
    void sleep();
    
    char *stats_info();
};


}

#endif
