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
    char settings_path[256];
    DIR *settings_directory;
public:
    settings();
    ~settings();
};

}

#endif
