add_library(debugger STATIC debug_screen.cpp debug_console.cpp debug_status_bar.cpp debug_command.cpp)

target_link_libraries(debugger char_rom cpu)
