#include "Input.h"

Input::Input(File &file, Terminal &terminal)
    : file_(file), terminal_(terminal) {
    cursorX = 0;
    cursorY = 0;
}
void Input::handleInput() {
    string line;
    wmove(terminal_.editorWindow, cursorY, cursorX);
    wrefresh(terminal_.editorWindow);
    int input = getch();
    // mvprintw(0,0, "Input: %d", input);
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
        if (cursorX == 0 && cursorY > 0) {
            JoinLines();
        } else {
            deleteChar();
        }
        break;
    case '\n':
        newLine();
        break;
    // ctrl + s
    case 19:
        file_.constructFileContent();
        file_.saveToFile(file_.fileContent, file_.FilePath);
        break;
    // ctrl + k
    case 11:
        if (cursorY >= 0 && cursorY < file_.textLines.size()) {
            if (!file_.textLines[cursorY+terminal_.offset].empty()) {
                string deletedLine = file_.textLines[cursorY+terminal_.offset];

                wmove(terminal_.editorWindow, cursorY, 0);
                wclrtoeol(terminal_.editorWindow);

                file_.textLines.erase(file_.textLines.begin() + cursorY+terminal_.offset);

                if (cursorY >= file_.textLines.size()) {
                    cursorY = file_.textLines.size() - 1;
                }

                cursorX = 0;

                wrefresh(terminal_.editorWindow);

                std::cout << "Deleted Line: " << deletedLine << std::endl;
            }
        }
        break;
    // ctrl + q
    case 17:
        terminal_.disableRawMode();
        exit(0);
        break;
    // ctrl + c
    case 3:
        copiedLine = file_.textLines[cursorY+terminal_.offset];
        // mvprintw(0, 0, "Copied Line: %s", copiedLine.c_str());
        break;
    // ctrl + v
    case 22:
        if (!copiedLine.empty()) {
            file_.textLines.insert(file_.textLines.begin() + cursorY + terminal_.offset + 1, copiedLine);
            moveCursorDown();
            cursorX = 0;
        } else {
            mvprintw(0, 0, "Nothing to paste!");
        }
        break;
    // ctrl + h
    case 8:
        terminal_.clearScreen();
        terminal_.hudWindow = newwin(LINES - 10, COLS - 10, 5, 5);
        box(terminal_.hudWindow, 0, 0);
        mvwprintw(terminal_.hudWindow, 0, 1, "Help");
        mvwprintw(terminal_.hudWindow, 1, 1, "Ctrl + S: Save and exit");
        mvwprintw(terminal_.hudWindow, 2, 1, "Ctrl + Q: Exit without saving");
        mvwprintw(terminal_.hudWindow, 3, 1, "Ctrl + H: Show help");
        mvwprintw(terminal_.hudWindow, 4, 1, "Ctrl + K: Delete line");
        mvwprintw(terminal_.hudWindow, 5, 1, "Ctrl + C: Copy line");
        mvwprintw(terminal_.hudWindow, 6, 1, "Ctrl + V: Paste line");
        wrefresh(terminal_.hudWindow);
        getch();
        terminal_.clearScreen();
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
    } else {
        if (cursorY > 0) {
            cursorY--;
            cursorX = file_.textLines[cursorY+terminal_.offset].length();
        }
    }
}

void Input::moveCursorRight() {
    if (cursorX < file_.textLines[cursorY+terminal_.offset].length()) {
        cursorX++;
    }
}
void Input::JoinLines() {
    string currentLine = file_.textLines[cursorY+terminal_.offset];
    file_.textLines.erase(file_.textLines.begin() + cursorY+terminal_.offset);
    cursorY--;
    cursorX = file_.textLines[cursorY+terminal_.offset].length();
    file_.textLines[cursorY+terminal_.offset] += currentLine;
}

void Input::moveCursorUp() {
    if (cursorY > 0) {
        cursorY--;
        if (cursorX > file_.textLines[cursorY+terminal_.offset].length()) {
            cursorX = file_.textLines[cursorY+terminal_.offset].length();
        }
    }

    if (cursorY == 0 && terminal_.offset > 0) {
        terminal_.offset--;
    }

}

void Input::moveCursorDown() {
    if (cursorY+terminal_.offset < file_.textLines.size()-1) {
        
        if (cursorX > file_.textLines[cursorY+terminal_.offset].length()) {
            cursorX = file_.textLines[cursorY+terminal_.offset].length();
        }

        if (cursorY == LINES - 1 && cursorY + terminal_.offset < file_.textLines.size()) {
            terminal_.offset++;
            cursorY--;

        }
        cursorY++;

    }

}

void Input::deleteChar() {
    if (cursorX > 0) {
        mvwdelch(terminal_.editorWindow, cursorY, cursorX);
        file_.textLines[cursorY+terminal_.offset].erase(cursorX - 1, 1);
        moveCursorLeft();
    }
}

void Input::insertChar(int input) {
    file_.textLines[cursorY + terminal_.offset].insert(cursorX, 1, input);
    moveCursorRight();
}

void Input::newLine() {
    string currentLine = file_.textLines[cursorY+terminal_.offset].substr(cursorX);
    file_.textLines[cursorY+terminal_.offset].erase(cursorX);
    file_.textLines.insert(file_.textLines.begin() + cursorY+terminal_.offset + 1, currentLine);
    moveCursorDown();
    cursorX = 0;
}
