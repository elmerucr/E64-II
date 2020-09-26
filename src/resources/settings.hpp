//  settings.hpp
//  E64-II
//
//  Copyright © 2020 elmerucr. All rights reserved.

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <dirent.h>

namespace E64 {

class settings
{
private:
    char home_dir[256];
    char settings_path[256];
    char current_path[256];
    DIR *settings_directory;
    
    void update_current_path_from_settings();
    void write_current_path_to_settings();
public:
    settings();
    ~settings();
};

}

#endif
