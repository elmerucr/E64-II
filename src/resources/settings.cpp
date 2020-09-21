//  settings.cpp
//  E64-II
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "settings.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
//#include <unistd.h>

E64::settings::settings()
{
    snprintf(settings_path, 256, "%s", getenv("HOME"));     // within apple app container this returns a long path
    
    char *iterator = settings_path;         // shorten the path name to home and user dir
    int times = 0;
    while(times < 3)
    {
        if((*iterator == '/') || (*iterator == '\0')) times++;
        iterator++;
    }

    iterator--;
    snprintf(iterator, 256, "/.E64-II");
    
    printf("[Settings] trying to open settings directory: %s\n", settings_path);
    settings_directory = opendir(settings_path);
    if( settings_directory == NULL )
    {
        printf("[Settings] error: directory doesn't exist. Trying to make it...\n");
        mkdir(settings_path, 0777);
        settings_directory = opendir(settings_path);
    }
}

E64::settings::~settings()
{
    if(settings_directory != NULL)
    {
        printf("[Settings] closing settings directory: %s\n", settings_path);
        closedir(settings_directory);
    }
}
