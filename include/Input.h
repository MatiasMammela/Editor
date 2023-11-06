#pragma once

#include "File.h"
#include "Terminal.h"
#include <iostream>
#include <ncurses.h>

using namespace std;

class Input {
public:
    Input(File &file, Terminal &terminal);
    void handleInput();
    int cursorY;
    int cursorX;

private:
    File &file_;
    Terminal &terminal_;
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorUp();
    void moveCursorDown();
    void deleteChar();
    void insertChar(int input);
    void newLine();
};