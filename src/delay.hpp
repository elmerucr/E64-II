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

    uint8_t framecounter;               // keeps track of no of frames since last evaluation
    uint8_t evaluation_interval;        // amount of frames between two evaluations

    pid_controller fps_pid;

public:
    // constructor
    delay(double initial_delay);
    
    // process pid and sleep
    void process(double measured_framerate);
};

}

#endif
