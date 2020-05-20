//  delay.cpp
//  E64
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>
#include "delay.hpp"
#include "sdl2.hpp"
#include "common.hpp"


E64::delay::delay(double initial_delay) : fps_pid(-8.0, 0.0, -8.0, FPS, initial_delay)
{
    current_delay = initial_delay;
    
    nominal_frame_time = (double)1000000/FPS;
    
    framecounter = 0;
    evaluation_interval = 8;        // must be a power of 2!

    audio_queue_size = AUDIO_BUFFER_SIZE;
    smoothed_audio_queue_size = audio_queue_size;
    smoothed_framerate = FPS;
    smoothed_mhz = CPU_CLOCK_SPEED/(1000*1000);
    alpha = 0.90f;

    statistics_framecounter = 0;

    then = std::chrono::steady_clock::now();
}

void E64::delay::process()
{
    framecounter++;
    if(!(framecounter & (evaluation_interval - 1) ))
    {
        // get current time stamp
        now = std::chrono::steady_clock::now();
        
        // calculate time passed since last measurement (in milliseconds)
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
        then = now;

        audio_queue_size = E64::sdl2_get_queued_audio_size();
        smoothed_audio_queue_size = (alpha * smoothed_audio_queue_size) + ((1.0 - alpha) * audio_queue_size);

        // framerate is no. of frames in one measurement interval divided by the duration
        framerate = (double)(evaluation_interval * 1000) / duration;
        smoothed_framerate = (alpha * smoothed_framerate) + ((1.0 - alpha) * framerate);

        mhz = (double)(framerate * (VICV_SCANLINES+VICV_SCANLINES_VBLANK) * CPU_CYCLES_PER_SCANLINE)/1000000;
        smoothed_mhz = (alpha * smoothed_mhz) + ((1.0 - alpha) * mhz);

        // run pid
        current_delay = fps_pid.process(framerate, evaluation_interval);
        if (current_delay < 500)
        {
            std::cout << "[PID Delay] system too slow?" << std::endl;
            current_delay = 500;
        }
        if (current_delay > nominal_frame_time ) current_delay = nominal_frame_time;
    }

    statistics_framecounter++;
    if(statistics_framecounter == (FPS / 2) )
    {
        snprintf(statistics_string, 256, "%4.2fMHz  %4.1ffps  %4.1fms  %4.0fbytes", smoothed_mhz, smoothed_framerate, current_delay/1000, smoothed_audio_queue_size);
        statistics_framecounter = 0;
    }
}

void E64::delay::sleep()
{
    // call delay
    // c++11 portable version of usleep():
    // see: https://gist.github.com/ngryman/6482577
    // and: https://en.cppreference.com/w/cpp/chrono
    std::this_thread::sleep_for(std::chrono::microseconds((uint32_t)current_delay));
}

char *E64::delay::stats_info()
{
    return statistics_string;
}
