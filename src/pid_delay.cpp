//  sdl2_pid_delay.cpp
//  E64
//
//  Copyright © 2019 elmerucr. All rights reserved.

#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>
#include "pid_delay.hpp"
#include "sdl2.hpp"
#include "common_defs.hpp"

E64::pid_controller::pid_controller(double k1, double k2, double k3, double setpoint, double initial_output)
{
    // pid process parameters
    this->k1 = k1;
    this->k2 = k2;
    this->k3 = k3;
    this->setpoint = setpoint;
    output = initial_output;
    // internal parameters
    error = 0.0;
    previous_error = 0.0;
    integral = 0.0;
    derivative = 0.0;
}

void E64::pid_controller::change_setpoint(double setpoint)
{
    this->setpoint = setpoint;
}

double E64::pid_controller::process(double input, double interval)
{
    // proportional
    error = setpoint - input;
    // integral
    integral = integral + (error * interval);
    // derivative
    derivative = (error - previous_error) / interval;
    // update previous error
    previous_error = error;
    // calculate new output
    output += (k1 * error) + (k2 * integral) + (k3 * derivative);
    return output;
}

E64::pid_delay::pid_delay(double initial_delay) : fps_pid(-8.0, 0.0, -8.0, FPS, initial_delay), audiobuffer_pid(-0.10, 0.00, -8.00, AUDIO_BUFFER_SIZE, initial_delay)
{
    current_delay = initial_delay;
    framecounter = 0;
    evaluation_interval = 2;

    audio_queue_size = AUDIO_BUFFER_SIZE;
    smoothed_audio_queue_size = audio_queue_size;
    smoothed_framerate = FPS;
    smoothed_mhz = CPU_CLOCK_SPEED/(1024*1024);
    alpha = 0.90f;

    statistics_framecounter = 0;

    then = std::chrono::steady_clock::now();
}

void E64::pid_delay::run()
{
    framecounter++;
    if(!(framecounter & (evaluation_interval - 1) ))
    {
        now = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
        then = now;

        audio_queue_size = E64::sdl2_get_queued_audio_size();
        smoothed_audio_queue_size = (alpha * smoothed_audio_queue_size) + ((1.0 - alpha) * audio_queue_size);

        framerate = (double)(evaluation_interval * 1000) / duration;
        smoothed_framerate = (alpha * smoothed_framerate) + ((1.0 - alpha) * framerate);

        mhz = (double)(framerate * 320 * CPU_CYCLES_PER_SCANLINE)/1000000;
        smoothed_mhz = (alpha * smoothed_mhz) + ((1.0 - alpha) * mhz);

        // run pid's
        //current_delay = fps_pid.process(framerate, evaluation_interval);
        current_delay = audiobuffer_pid.process(smoothed_audio_queue_size, evaluation_interval);
        if (current_delay < 5000)
        {
            std::cout << "[PID Delay] system too slow?" << std::endl;
            current_delay = 5000;
        }
        if (current_delay > 20000) current_delay = 20000;
    }

    statistics_framecounter++;
    if(statistics_framecounter == (FPS / 2) )
    {
        snprintf(statistics_string, 256, "%4.2fMHz  %4.1ffps  %4.1fms  %4.0fbytes", smoothed_mhz, smoothed_framerate, current_delay/1000, smoothed_audio_queue_size);
        statistics_framecounter = 0;
    }

    // call delay
    // c++11 portable version of usleep():
    // see: https://gist.github.com/ngryman/6482577
    // and: https://en.cppreference.com/w/cpp/chrono
    std::this_thread::sleep_for(std::chrono::microseconds((uint32_t)current_delay));
}

char *E64::pid_delay::stats()
{
    return statistics_string;
}
