#include "File.h"
#include "Input.h"
#include "Syntax.h"
#include "Terminal.h"
#include "ncurses.h"
#include <csignal>
#include <iostream>
using namespace std;
void sigintHandler(int signum) {
    // varataan ctrl+c kopiointiin
    // ctrl + q lopettaa ohjelman
}
void renderEditor(Terminal &terminal, File &file, Syntax &syntax) {
    wclear(terminal.editorWindow);
    wclear(terminal.lineNumbersWindow);
    for (int i = terminal.offset; i < file.textLines.size(); i++) {

        string line = file.textLines[i];
        syntax.drawSyntaxHighlighting(terminal.editorWindow, i - terminal.offset, 0, line);
    }

    terminal.drawLineNumbers(terminal.offset);
    wrefresh(terminal.lineNumbersWindow);
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
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    Terminal terminal(file.textLines.size());
    terminal.enableRawMode();
    terminal.clearScreen();
    wclear(terminal.editorWindow);
    Input input(file, terminal);
    Syntax syntax;
    renderEditor(terminal, file, syntax);

    while (true) {
        input.handleInput();
        renderEditor(terminal, file, syntax);
    }

    terminal.disableRawMode();
    return 0;
}
