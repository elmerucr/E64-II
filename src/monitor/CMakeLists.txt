add_library(monitor STATIC monitor_screen.cpp monitor_console.cpp monitor_status_bar.cpp monitor_command.cpp)

target_link_libraries(monitor roms cpu)
