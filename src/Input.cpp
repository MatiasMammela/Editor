#include "Input.h"

Input::Input(File &file, Terminal &terminal)
    : file_(file), terminal_(terminal) {
    cursorX = 0;
    cursorY = 0;
}

void Input::handleInput() {
    wmove(terminal_.editorWindow, cursorY, cursorX);
    wrefresh(terminal_.editorWindow);
    int input = getch();
    switch (input) {
    case KEY_LEFT:
        moveCursorLeft();
        break;
    case KEY_RIGHT:
        moveCursorRight();
        break;
    case KEY_UP:
        moveCursorUp();
        break;
    case KEY_DOWN:
        moveCursorDown();
        break;
    case KEY_BACKSPACE:
        deleteChar();
        break;
    case '\n':
        newLine();
        break;
    default:
        insertChar(input);
        break;
    }
    wrefresh(terminal_.editorWindow);
}

void Input::moveCursorLeft() {
    if (cursorX > 0) {
        cursorX--;
    }
}

void Input::moveCursorRight() {
    if (cursorX < file_.textLines[cursorY].length()) {
        cursorX++;
    }
}

void Input::moveCursorUp() {
    if (cursorY > 0) {
        cursorY--;
        if (cursorX > file_.textLines[cursorY].length()) {
            cursorX = file_.textLines[cursorY].length();
        }
    }
}

void Input::moveCursorDown() {
    if (cursorY < file_.textLines.size() - 1) {
        cursorY++;
        if (cursorX > file_.textLines[cursorY].length()) {
            cursorX = file_.textLines[cursorY].length();
        }
    }
}

void Input::deleteChar() {
    if (cursorX > 0) {
        mvwdelch(terminal_.editorWindow, cursorY, cursorX);
        file_.textLines[cursorY].erase(cursorX - 1, 1);
        moveCursorLeft();
    }
}

void Input::insertChar(int input) {
    winsch(terminal_.editorWindow, input);
    moveCursorRight();
    file_.textLines[cursorY].insert(cursorX - 1, 1, input);
}

void Input::newLine() {
    // todo
}
