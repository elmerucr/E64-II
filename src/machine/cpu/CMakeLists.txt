add_subdirectory(Moira/)

add_library(cpu STATIC cpu.cpp)

target_link_libraries(cpu Moira)
