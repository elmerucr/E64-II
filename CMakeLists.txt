cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

project(E64-II)

find_package(sdl2 REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    src/
    src/devices/
    src/devices/cpu/
    src/devices/cpu/Moira
    src/devices/mmu/
    src/devices/kernel/
    src/devices/cia/
    src/devices/sids/
    src/devices/sids/resid-0.16/
    src/devices/timer/
    src/devices/TTL74LS148/
    src/devices/vicv/
    src/monitor/
    src/resources/
    src/rom/
    src/sdl2/
)

add_subdirectory(src/)

add_executable(
    E64-II
    src/main.cpp
    src/machine.cpp
    src/frequency_divider.cpp
	src/stats.cpp)

target_link_libraries(E64-II char_rom cia cpu E64_sdl2 kernel mmu monitor resid resources roms sids timer TTL74LS148 vicv ${SDL2_LIBRARIES})
