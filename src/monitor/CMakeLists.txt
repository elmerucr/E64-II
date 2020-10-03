add_library(monitor STATIC debug_screen.cpp debug_console.cpp debug_status_bar.cpp debug_command.cpp)

target_link_libraries(monitor char_rom cpu)
