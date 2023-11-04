#pragma once

#include <iostream>
#include <ncurses.h>
#include "File.h"
#include "Terminal.h"
using namespace std;

class Input
{
public:
    Input(File &file, Terminal &terminal);
    void handleInput();
    int cursorY;
    int cursorX;

private:
    File &file_;
    Terminal &terminal_;
};