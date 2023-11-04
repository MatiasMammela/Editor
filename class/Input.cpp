#include "Input.h"

Input::Input(File &file, Terminal &terminal) : file_(file), terminal_(terminal)
{
    cursorX = 0;
    cursorY = 0;
}

void Input::handleInput()
{
    wmove(terminal_.editorWindow, cursorY, cursorX);
    wrefresh(terminal_.editorWindow);
    int input = getch();
    switch (input)
    {
    case KEY_LEFT:
        if (cursorX > 0)
        {
            cursorX--;
        }
        else
        {
            if (cursorY > 0)
            {
                cursorY--;
                int lastCharPos = COLS - 1;
                while (lastCharPos >= 0 && mvwinch(terminal_.editorWindow, cursorY, lastCharPos) == ' ')
                {
                    lastCharPos--;
                }
                if (lastCharPos < 0)
                {
                    lastCharPos = 0;
                }
                cursorX = lastCharPos;
            }
        }
        break;
    case KEY_RIGHT:
        if (cursorX < COLS - 1)
        {
            cursorX++;
        }
        break;
    case KEY_UP:
        if (cursorY > 0)
        {
            cursorY--;
        }
        break;
    case KEY_DOWN:
        if (cursorY < LINES - 1)
        {
            cursorY++;
        }
        break;
    case KEY_BACKSPACE:
        if (cursorX == 0)
        {
            mvwdelch(terminal_.editorWindow, cursorY, cursorX);
        }
        if (cursorX > 0)
        {
            cursorX--;
            mvwdelch(terminal_.editorWindow, cursorY, cursorX);
        }
        else
        {
            if (cursorY > 0)
            {
                cursorY--;
                int lastCharPos = COLS - 1;
                while (lastCharPos >= 0 && mvwinch(terminal_.editorWindow, cursorY, lastCharPos) == ' ')
                {
                    lastCharPos--;
                }
                if (lastCharPos < 0)
                {
                    lastCharPos = 0;
                }
                cursorX = lastCharPos;
            }
        }
        break;
    case '\n':
        // enter
        winch(terminal_.editorWindow);
        cursorX = 0;
        cursorY++;
        break;
    default:
        // lisää kirjain :DD
        winsch(terminal_.editorWindow, input);
        cursorX++;
        break;
    }
    wrefresh(terminal_.editorWindow);
}