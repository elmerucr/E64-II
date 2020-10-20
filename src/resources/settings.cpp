//  settings.cpp
//  E64-II
//
//  Copyright © 2020 elmerucr. All rights reserved.

#include "settings.hpp"
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

extern uint8_t kernel[];

E64::settings::settings()
{
    snprintf(settings_path, 256, "%s", getenv("HOME"));     // within apple app container this returns a long path
    
    // shorten the path name to home and user dir
    char *iterator = settings_path;
    int times = 0;
    while(times < 3)
    {
        if((*iterator == '/') || (*iterator == '\0')) times++;
        iterator++;
    }
    iterator--;
    *iterator = '\0';   // force '\0' character to terminate string to copy to 'home_dir'
    
    strcpy(home_dir, settings_path);
    strcpy(current_path, home_dir);    // current path defaults to homedir
    printf("[Settings] user home directory: %s\n", home_dir);
    
    snprintf(iterator, 256, "/.E64-II");
    
    printf("[Settings] opening settings directory: %s\n", settings_path);
    settings_directory = opendir(settings_path);
    if( settings_directory == NULL )
    {
        printf("[Settings] error: directory doesn't exist. Trying to make it...\n");
        mkdir(settings_path, 0777);
        settings_directory = opendir(settings_path);
    }
    
    // switch to settings path and try to update current_path from settings
    chdir(settings_path);
    
    // call this before update_current_...
    find_and_update_kernel();
    
    update_current_path_from_settings();
}

E64::settings::~settings()
{
    write_current_path_to_settings();
    
    if(settings_directory != NULL)
    {
        printf("[Settings] closing settings directory: %s\n", settings_path);
        closedir(settings_directory);
    }
}

void E64::settings::update_current_path_from_settings()
{
    FILE *temp_file = fopen("PATH", "r");
    if( temp_file )
    {
        fgets(current_path, 256, temp_file);
        size_t ln = strlen(current_path) - 1;
        if (*current_path && current_path[ln] == '\n') current_path[ln] = '\0';
        
        printf("[Settings] current path is now: %s\n", current_path);
        fclose(temp_file);
        chdir(current_path);
    }
    else
    {
        printf("[Settings] current path not found in settings, defaulting to: %s\n", current_path);
        //write_current_path_to_settings();
    }
}

void E64::settings::write_current_path_to_settings()
{
    chdir(settings_path);
    FILE *temp_file = fopen("PATH", "w");
    
    char *temp_char = current_path;
    
    while( *temp_char != '\0' )
    {
        fwrite(temp_char, 1, 1, temp_file);
        temp_char++;
    }
    
    fclose(temp_file);
}

void E64::settings::find_and_update_kernel()
{
    FILE *temp_file = fopen("kernel.bin", "r");
    
    if( temp_file )
    {
        printf("[Settings] found 'kernel.bin' in settings dir, using it\n");
        fread(kernel, 65536, 1, temp_file);
        fclose(temp_file);
    }
    else
    {
        printf("[Settings] no 'kernel.bin' in settings dir, using enclosed version\n");
    }
}
