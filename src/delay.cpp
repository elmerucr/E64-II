//  delay.cpp
//  E64
//
//  Copyright © 2019-2020 elmerucr. All rights reserved.

#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>
#include "delay.hpp"
#include "common.hpp"


E64::delay::delay(double initial_delay) : fps_pid(-8.0, 0.0, -8.0, FPS, initial_delay)
{
    current_delay = initial_delay;
    
    nominal_frame_time = (double)1000000/FPS;
    
    framecounter = 0;
    evaluation_interval = 8;        // must be a power of 2!
}

void E64::delay::process(double measured_framerate)
{
    framecounter++;
    if( framecounter == evaluation_interval )
    {
        // run pid
        current_delay = fps_pid.process(measured_framerate, evaluation_interval);
        if (current_delay < 500)
        {
            std::cout << "[PID Delay] system too slow?" << std::endl;
            current_delay = 500;
        }
        if (current_delay > nominal_frame_time ) current_delay = nominal_frame_time;
    }

    // call delay
    // c++11 portable version of usleep():
    // see: https://gist.github.com/ngryman/6482577
    // and: https://en.cppreference.com/w/cpp/chrono
    std::this_thread::sleep_for(std::chrono::microseconds((uint32_t)current_delay));
}
