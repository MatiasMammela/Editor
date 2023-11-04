#include "ncurses.h"
#include "headers/File.h"
#include "headers/Terminal.h"
#include "headers/Input.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cout << "Usage blaah blaah blaah" << endl;
        return 1;
    }

    string filename = argv[1];
    cout << filename << endl;

    File file;
    file.openFile(filename);

    Terminal terminal;

    terminal.enableRawMode();
    terminal.clearScreen();
    wclear(terminal.editorWindow);
    wrefresh(terminal.editorWindow);
    const char *content = file.fileContent.c_str();
    wprintw(terminal.editorWindow, content);
    wrefresh(terminal.editorWindow);
    Input input(file, terminal);

    while (true)
    {
        input.handleInput();
    }
    terminal.disableRawMode();
    return 0;
}