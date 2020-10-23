add_subdirectory(char_rom/)

add_library(kernel STATIC kernel/kernel.cpp)

add_library(roms STATIC rom.cpp)
