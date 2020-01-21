find_package(sdl2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(E64_sdl2 STATIC sdl2.cpp)

target_link_libraries(E64_sdl2 ${SDL2_LIBRARIES})