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
    // timers, for calculation of fps
    std::chrono::time_point<std::chrono::steady_clock> now, then, done;
    int64_t total_time;
    int64_t idle_time;

    uint8_t framecounter;               // keeps track of no of frames since last evaluation
    uint8_t evaluation_interval;        // amount of frames between two evaluations

    uint8_t statistics_framecounter;    // the status bar on the bottom is not refreshed every few frames, but once every few frames
    char statistics_string[256];

    double framerate;
    double smoothed_framerate;
    
    double mhz;
    double smoothed_mhz;
    
    double audio_queue_size;
    double smoothed_audio_queue_size;
    
    double idle_per_frame;
    double smoothed_idle_per_frame;

    // exp smoothing constant for average_audio_queue_size
    double alpha;

public:
    // constructor
    stats();
    
    // process calculations on parameters (fps/mhz/buffersize)
    void process_parameters();

    // for time measurement during the main cycle
    void start_idle();
    void done_idle();

    // getting info
    inline double get_current_framerate() { return framerate; }
    inline double get_current_smoothed_framerate() { return smoothed_framerate; }
    inline double get_current_audio_queue_size() { return audio_queue_size; }
    inline double get_current_smoothed_audio_queue_size() { return smoothed_audio_queue_size; }
    inline char *stats_info() { return statistics_string; }
};

}

#endif
