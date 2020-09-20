//  settings.cpp
//  E64-II
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "settings.hpp"
#include <cstdio>
#include <cstdlib>
//#include <sys/stat.h>
//#include <unistd.h>

E64::settings::settings()
{
    snprintf(settings_path, 256, "%s/.E64-II", getenv("HOME"));
    printf("trying to open settings directory: %s\n",settings_path);
}

E64::settings::~settings()
{
    
}
