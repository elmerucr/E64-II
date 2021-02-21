add_subdirectory(Moira/)

add_library(cpu STATIC cpu_moira.cpp)

target_link_libraries(cpu Moira)