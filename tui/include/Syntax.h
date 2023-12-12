// Syntax.h
#pragma once
#include <ncurses.h>
#include <regex>
#include <string>
class Syntax {
public:
    enum ColorPairs {
        BRACKET = 1,
        DECLARATION = 2,
        STRING = 3,
        DEFAULT_PAIR = 4,
        COMMENT = 5,
        RETURN = 6,
        INCLUDE = 7,
    };

    Syntax();
    void printColoredString(WINDOW *window, int x, int y, std::string word, int colorPair);
    void handleDeclaration(WINDOW *window, int &x, int y, std::string line, const std::string &keyword, int colorPair);
    void drawSyntaxHighlighting(WINDOW *window, int y, int x, std::string line);

private:
    bool insideComment;
    bool insideString; /* A flag to track whether we are inside a string*/

    void printColorPair(int pair, WINDOW *window, int x, int y, std::string word);
};
