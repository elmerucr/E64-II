//  pid_controller.cpp
//  E64
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "pid_controller.hpp"

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
