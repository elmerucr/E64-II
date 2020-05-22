//  pid_controller.hpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include <cstdint>
#include <chrono>

#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

namespace E64
{

class pid_controller
{
private:
    // pid process parameters
    double k1;
    double k2;
    double k3;

    double setpoint;
    double output;

    // internal
    double error;
    double previous_error;
    double integral;
    double derivative;
    
public:
    pid_controller(double _k1, double _k2, double _k3, double _setpoint, double initial_output);
    void change_setpoint(double _setpoint);
    double process(double input, double interval);
};

}

#endif
