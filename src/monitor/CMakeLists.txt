add_library(monitor STATIC monitor.cpp screen.cpp tty.cpp status_bar.cpp command.cpp)

target_link_libraries(monitor rom cpu)
