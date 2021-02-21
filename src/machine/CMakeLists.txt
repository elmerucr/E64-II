add_subdirectory(cia/)
add_subdirectory(cpu/)
add_subdirectory(fd/)
add_subdirectory(mmu/)
add_subdirectory(sids/)
add_subdirectory(timer/)
add_subdirectory(TTL74LS148/)
add_subdirectory(vicv/)

add_library(machine STATIC machine.cpp)

target_link_libraries(machine cia cpu fd mmu sids timer TTL74LS148 vicv)
