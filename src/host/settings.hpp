//  settings.hpp
//  E64-II
//
//  Copyright © 2020 elmerucr. All rights reserved.

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <dirent.h>

namespace E64 {

class settings_t
{
private:
	DIR *settings_directory;
    
	void update_current_path_from_settings();
	void write_current_path_to_settings();
public:
	settings_t();
	~settings_t();

	char home_dir[256];
	char settings_path[256];
	char path_to_kernel[256];
	char current_path[256];
};

}

#endif
