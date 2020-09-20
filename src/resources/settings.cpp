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
    snprintf(settings_path, 256, "%s", getenv("HOME"));     // within app bundle returns too much
    
    char *iterator = settings_path;
    int times = 0;
    while(*iterator != '\0')                    // shorten the path name to home and user dir
    {
        if(*iterator == '/') times++;
        if(times == 3) *iterator = '\0';
        iterator++;
    }
    
    snprintf(settings_path, 256, "%s/.E64-II", settings_path);
    
    printf("[Settings] trying to open settings directory: %s\n",settings_path);
}

E64::settings::~settings()
{
    
}
