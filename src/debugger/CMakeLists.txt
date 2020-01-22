add_library(debugger STATIC debug_screen.cpp debug_console.cpp debug_status_bar_E64_II.cpp debug_command_E64_II.cpp)

target_link_libraries(debugger char_rom cpu)
