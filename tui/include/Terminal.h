#pragma once

#include <iostream>
#include <ncurses.h>
#include <sstream>

using namespace std;

class Terminal {
public:
    WINDOW *editorWindow;
    WINDOW *hudWindow;
    WINDOW *lineNumbersWindow;
    void drawLineNumbers(int offset);
    void clearScreen();
    void drawWindow(int maxLines);
    void enableRawMode();
    void disableRawMode();
    Terminal(int maxLines);
    int offset = 0;
};