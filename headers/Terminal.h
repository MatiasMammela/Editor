#pragma once

#include <iostream>
#include <ncurses.h>

using namespace std;

class Terminal
{
public:
    WINDOW *editorWindow;
    WINDOW *hudWindow;
    void clearScreen();
    void enableRawMode();
    void disableRawMode();
    Terminal();
};