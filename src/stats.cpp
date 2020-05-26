//  stats.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include <cstdint>
#include <chrono>

#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>
#include "stats.hpp"
#include "sdl2.hpp"
#include "common.hpp"


E64::stats::stats()
{
    total_time = 0;
    idle_time = 0;
    
    framecounter = 0;
    
    evaluation_interval = 4;

    smoothed_audio_queue_size = AUDIO_BUFFER_SIZE;
    
    smoothed_framerate = FPS;
    
    smoothed_mhz = CPU_CLOCK_SPEED/(1000*1000);
    
    smoothed_idle_per_frame = 1000000 / (FPS * 2);
    
    alpha = 0.90f;

    statistics_framecounter = 0;
    
    nominal_time_per_frame = 1000000 / FPS;

    now = then = std::chrono::steady_clock::now();
}

void E64::stats::process_parameters()
{
    framecounter++;
    if(framecounter == evaluation_interval)
    {
        framecounter = 0;
        
        audio_queue_size = E64::sdl2_get_queued_audio_size();
        smoothed_audio_queue_size = (alpha * smoothed_audio_queue_size) + ((1.0 - alpha) * audio_queue_size);

        // framerate is no. of frames in one measurement interval divided by the duration
        framerate = (double)(evaluation_interval * 1000000) / total_time;
        smoothed_framerate = (alpha * smoothed_framerate) + ((1.0 - alpha) * framerate);

        mhz = (double)(framerate * (CPU_CLOCK_SPEED / FPS) )/1000000;
        smoothed_mhz = (alpha * smoothed_mhz) + ((1.0 - alpha) * mhz);
        
        idle_per_frame = idle_time / (evaluation_interval);
        smoothed_idle_per_frame = (alpha * smoothed_idle_per_frame) + ((1.0 - alpha) * idle_per_frame);
        
        total_time = idle_time = 0;
    }

    statistics_framecounter++;
    if(statistics_framecounter == (FPS / 2) )
    {
        snprintf(statistics_string, 256, "%5.2fMHz  %5.2ffps  %5.2fms %5.0fbytes", smoothed_mhz, smoothed_framerate, smoothed_idle_per_frame/1000, smoothed_audio_queue_size);
        statistics_framecounter = 0;
    }
}

void E64::stats::start_idle()
{
    // here we pinpoint done, because we're done with the "work"
    done = std::chrono::steady_clock::now();
}

void E64::stats::done_idle()
{
    now = std::chrono::steady_clock::now();
    idle_time += std::chrono::duration_cast<std::chrono::microseconds>(now - done).count();
    total_time += std::chrono::duration_cast<std::chrono::microseconds>(now - then).count();
    then = now;
}
