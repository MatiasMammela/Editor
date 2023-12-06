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
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GTKSOURCEVIEW REQUIRED gtksourceview-5)
include_directories(${GTKSOURCEVIEW_INCLUDE_DIRS})
include_directories(${GTK4_INCLUDE_DIRS} gui/include)
link_directories(${GTK4_LIBRARY_DIRS})
link_directories(${GTKSOURCEVIEW_LIBRARY_DIRS})
add_definitions(${GTKSOURCEVIEW_CFLAGS_OTHER})
file(GLOB GUI_SOURCES "gui/src/*.c")

file(GLOB UI_FILES "gui/ui/*.ui")
file(COPY ${UI_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(editor_gui ${GUI_SOURCES})
target_link_libraries(editor_gui ${CURSES_LIBRARIES} ${GTK4_LIBRARIES} ${GTKSOURCEVIEW_LIBRARIES} )
