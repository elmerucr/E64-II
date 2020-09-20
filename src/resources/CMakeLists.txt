find_package(sdl2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(resources STATIC settings.cpp video.cpp)

target_link_libraries(resources ${SDL2_LIBRARIES})