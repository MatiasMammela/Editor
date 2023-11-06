cmake_minimum_required(VERSION 3.0)

project(editor)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS} include)

file(GLOB SOURCES "src/*.cpp")

add_executable(editor ${SOURCES})
target_link_libraries(editor ${CURSES_LIBRARIES})