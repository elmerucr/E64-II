add_library(monitor STATIC screen.cpp monitor_console.cpp monitor_status_bar.cpp monitor_command.cpp)

target_link_libraries(monitor rom cpu)
