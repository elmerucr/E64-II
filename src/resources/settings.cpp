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
    
    char *iterator = settings_path;         // shorten the path name to home and user dir
    int times = 0;
    while(times < 3)
    {
        if((*iterator == '/') || (*iterator == '\0')) times++;
        iterator++;
    }
    
    iterator--;
    snprintf(iterator, 256, "/.E64-II");
    
    printf("[Settings] trying to open settings directory: %s\n",settings_path);
}

E64::settings::~settings()
{
    
}
