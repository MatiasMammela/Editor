cmake_minimum_required(VERSION 3.0)

project(editor)

# TUI version
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS} tui/include)

file(GLOB TUI_SOURCES "tui/src/*.cpp")

add_executable(editor_tui ${TUI_SOURCES})
target_link_libraries(editor_tui ${CURSES_LIBRARIES})

# GUI version
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
include_directories(${GTKMM_INCLUDE_DIRS} gui/include)
link_directories(${GTKMM_LIBRARY_DIRS})

file(GLOB GUI_SOURCES "gui/src/*.cpp")

add_executable(editor_gui ${GUI_SOURCES})
target_link_libraries(editor_gui ${CURSES_LIBRARIES} ${GTKMM_LIBRARIES})
