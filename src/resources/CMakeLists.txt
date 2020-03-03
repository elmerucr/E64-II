find_package(sdl2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(video STATIC video.cpp)

target_link_libraries(video ${SDL2_LIBRARIES})