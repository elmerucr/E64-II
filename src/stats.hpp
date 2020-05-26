//  stats.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include <cstdint>
#include <chrono>

#ifndef STATS_HPP
#define STATS_HPP

namespace E64
{

class stats
{
private:
    std::chrono::time_point<std::chrono::steady_clock> now, then, done;
    int64_t total_time;
    int64_t idle_time;

    uint8_t framecounter;               // keeps track of no of frames since last evaluation
    uint8_t framecounter_interval;      // amount of frames between two evaluations

    uint8_t status_bar_framecounter;    // the status bar on the bottom is refreshed every few frames
    uint8_t status_bar_framecounter_interval;
    
    double alpha;                       // exponential smoothing constant

    double framerate;
    double smoothed_framerate;
    
    double mhz;
    double smoothed_mhz;
    
    double audio_queue_size;
    double smoothed_audio_queue_size;
    
    double idle_per_frame;
    double smoothed_idle_per_frame;
    
    char statistics_string[256];
    
public:
    void reset_measurement();
    
    uint32_t nominal_time_per_frame;      // in microseconds
    
    // process calculations on parameters (fps/mhz/buffersize)
    void process_parameters();

    // for time measurement during the main cycle
    void start_idle();
    void done_idle();

    // getters
    inline double get_current_framerate() { return framerate; }
    inline double get_current_smoothed_framerate() { return smoothed_framerate; }
    inline double get_current_audio_queue_size() { return audio_queue_size; }
    inline double get_current_smoothed_audio_queue_size() { return smoothed_audio_queue_size; }
    inline char *stats_info() { return statistics_string; }
};

}

#endif
