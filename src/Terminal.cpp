#include "Terminal.h"

void Terminal::clearScreen() {
    clear();
    refresh();
}

Terminal::Terminal() {
    initscr();
    editorWindow = newwin(LINES, COLS, 0, 0);
    keypad(editorWindow, TRUE);
    wrefresh(editorWindow);
}

void Terminal::enableRawMode() {
    raw();
    noecho();
    keypad(stdscr, TRUE);
    // cbreak();
}

void Terminal::disableRawMode() {
    endwin();
}
