cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

project(E64-II)

find_package(sdl2 REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    src/
    src/host/
    src/machine/
    src/machine/cpu/
    src/machine/cpu/Moira
    src/machine/fd
    src/machine/mmu/
    src/machine/cia/
    src/machine/sids/
    src/machine/sids/resid-0.16/
    src/machine/timer/
    src/machine/TTL74LS148/
    src/machine/vicv/
    src/monitor/
    src/rom/
)

add_subdirectory(src/)

add_executable(E64-II src/main.cpp)

target_link_libraries(E64-II host machine monitor rom ${SDL2_LIBRARIES})
