#include "Terminal.h"

void Terminal::clearScreen()
{
    clear();
    refresh();
}
Terminal::Terminal()
{
    initscr();
    keypad(editorWindow, TRUE);
    editorWindow = newwin(LINES, COLS, 0, 0);
    wrefresh(editorWindow);
}
void Terminal::enableRawMode()
{
    raw();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
}

void Terminal::disableRawMode()
{
    endwin();
}
