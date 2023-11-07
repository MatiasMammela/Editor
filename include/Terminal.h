#pragma once

#include <iostream>
#include <ncurses.h>
#include <sstream>

using namespace std;

class Terminal {
public:
    WINDOW *editorWindow;
    WINDOW *hudWindow;
    void clearScreen();
    void enableRawMode();
    void disableRawMode();
    Terminal();
    int offset = 0;
};