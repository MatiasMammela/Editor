#include "File.h"
#include "Input.h"
#include "Terminal.h"
#include "ncurses.h"
#include <csignal>
#include <iostream>
using namespace std;
void sigintHandler(int signum) {
    // varataan ctrl+c kopiointiin
    // ctrl + q lopettaa ohjelman
}
void renderEditor(Terminal &terminal, File &file) {
    wclear(terminal.editorWindow);

    for (int i = terminal.offset; i < file.textLines.size(); i++) {
        mvwprintw(terminal.editorWindow, i - terminal.offset, 0, file.textLines[i].c_str());
    }

    wrefresh(terminal.editorWindow);
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>";
        getch();
        return 1;
    }
    // signal(SIGINT, sigintHandler);
    string filename = argv[1];
    File file;
    file.openFile(filename);

    Terminal terminal;
    terminal.enableRawMode();
    terminal.clearScreen();
    wclear(terminal.editorWindow);
    Input input(file, terminal);

    renderEditor(terminal, file);

    while (true) {
        input.handleInput();
        renderEditor(terminal, file);
    }

    terminal.disableRawMode();
    return 0;
}
