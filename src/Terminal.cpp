#include "Terminal.h"

void Terminal::clearScreen() {
    clear();
    refresh();
}

Terminal::Terminal(int maxLines) {
    initscr();

    drawWindow(maxLines);
    keypad(editorWindow, TRUE);
    wrefresh(editorWindow);
    wrefresh(lineNumbersWindow);
}

void Terminal::drawWindow(int maxLines) {

    int digits = 0;
    int temp = maxLines;
    while (temp != 0) {
        temp /= 10;
        digits++;
    }
    if (digits < 2)
        digits = 2;
    editorWindow = newwin(LINES, COLS - digits, 0, digits);
    box(editorWindow, 0, 0);
    lineNumbersWindow = newwin(LINES, 0, 0, 0);
    box(lineNumbersWindow, 0, 0);
    wbkgd(lineNumbersWindow, COLOR_PAIR(1) | A_REVERSE);
}

void Terminal::drawLineNumbers(int offset) {
    wclear(lineNumbersWindow);

    int max_y, max_x;
    getmaxyx(lineNumbersWindow, max_y, max_x);

    for (int i = 0; i < max_y; ++i) {
        int lineNumber = i + offset + 1;
        mvwprintw(lineNumbersWindow, i, 0, "%d", lineNumber);
    }

    wrefresh(lineNumbersWindow);
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
