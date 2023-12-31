#pragma once

#include "File.h"
#include "Terminal.h"
#include <cstring>
#include <iostream>
#include <ncurses.h>
using namespace std;

class Input {
public:
    int findWordIndex = 0;
    Input(File &file, Terminal &terminal);
    void handleInput();
    string copiedLine;
    int cursorY;
    int cursorX;
    string searchWord;

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
    void JoinLines();
    void findWord();
    void ifFileEmpty();
};