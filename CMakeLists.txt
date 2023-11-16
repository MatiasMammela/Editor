cmake_minimum_required(VERSION 3.0)

project(editor)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS} include)

file(GLOB SOURCES "src/*.cpp")

add_executable(editor ${SOURCES})
target_link_libraries(editor ${CURSES_LIBRARIES})

# GUI version
file(GLOB GUI_SOURCES "src/gui/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_executable(editor_gui ${GUI_SOURCES})
target_link_libraries(editor_gui ${CURSES_LIBRARIES} ${GTKMM_LIBRARIES})