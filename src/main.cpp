#include "File.h"
#include "Input.h"
#include "Terminal.h"
#include "ncurses.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << ("Usage: %s <filename>", argv[0]);
        getch();
        return 1;
    }

    string filename = argv[1];
    File file;
    file.openFile(filename);

    Terminal terminal;
    terminal.enableRawMode();
    terminal.clearScreen();
    wclear(terminal.editorWindow);

    Input input(file, terminal);

    for (int i = 0; i < file.textLines.size(); i++) {
        wprintw(terminal.editorWindow, file.textLines[i].c_str());
        wprintw(terminal.editorWindow, "\n");
    }

    while (true) {
        input.handleInput();
    }

    terminal.disableRawMode();
    return 0;
}