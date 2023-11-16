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
    int margin = 1;
    int temp = maxLines;
    while (temp != 0) {
        temp /= 10;
        digits++;
    }
    if (digits < 2)
        digits = 2;
    editorWindow = newwin(LINES, COLS - digits - margin, 0, digits + margin);
    box(editorWindow, 0, 0);
    lineNumbersWindow = newwin(LINES, 0, 0, 0);
    box(lineNumbersWindow, 0, 0);
}

void Terminal::drawLineNumbers(int offset) {
    wclear(lineNumbersWindow);

    int max_y, max_x;
    getmaxyx(lineNumbersWindow, max_y, max_x);
    wattron(lineNumbersWindow, COLOR_PAIR(1));
    for (int i = 0; i < max_y; ++i) {
        int lineNumber = i + offset + 1;
        mvwprintw(lineNumbersWindow, i, 0, "%d", lineNumber);
    }
    wattroff(lineNumbersWindow, COLOR_PAIR(1));
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
