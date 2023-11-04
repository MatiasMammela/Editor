cmake_minimum_required(VERSION 3.0)

project(editor)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS} headers)

add_executable(editor main.cpp class/File.cpp class/Terminal.cpp class/Input.cpp)
target_link_libraries(editor ${CURSES_LIBRARIES})