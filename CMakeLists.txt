cmake_minimum_required(VERSION 3.0)

project(file_manager)

add_executable(file_manager main.c print.c handlers.c)

include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(file_manager ncurses)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")