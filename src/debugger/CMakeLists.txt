add_library(debugger STATIC debug_screen.cpp debug_console.cpp)

target_link_libraries(debugger char_rom cpu)
