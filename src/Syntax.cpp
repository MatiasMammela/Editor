#include "Syntax.h"
Syntax::Syntax() {
    start_color();
    init_pair(BRACKET, COLOR_YELLOW, COLOR_BLACK);
    init_pair(DECLARATION, COLOR_BLUE, COLOR_BLACK);
    init_pair(STRING, COLOR_GREEN, COLOR_BLACK);
    init_pair(DEFAULT_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(COMMENT, COLOR_RED, COLOR_BLACK);
    init_pair(RETURN, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(INCLUDE, COLOR_CYAN, COLOR_BLACK);
    insideString = false;
    insideComment = false;
}
void Syntax::printColorPair(int pair, WINDOW *window, int x, int y, std::string word) {
    wattron(window, COLOR_PAIR(pair));
    mvwprintw(window, y, x, word.c_str());
    wattroff(window, COLOR_PAIR(pair));
}

void Syntax::drawSyntaxHighlighting(WINDOW *window, int y, int x, std::string line) {
    insideString = false;
    insideComment = false;

    // Järkyttävää

    while (x < line.size()) {
        if (line.substr(x, 1) == "{" || line.substr(x, 1) == "}" || line.substr(x, 1) == "(" || line.substr(x, 1) == ")" || line.substr(x, 1) == "[" || line.substr(x, 1) == "]") {
            printColorPair(BRACKET, window, x, y, line.substr(x, 1));
            x++;
        } else if (line.substr(x, 3) == "int" && (x == 0 || line.substr(x - 1, 1) == " ")) {
            printColorPair(DECLARATION, window, x, y, line.substr(x, 3));
            x += 3;
        } else if (line.substr(x, 6) == "string") {
            printColorPair(DECLARATION, window, x, y, line.substr(x, 6));
            x += 6;
        } else if (line.substr(x, 4) == "char" || line.substr(x, 4) == "bool") {
            printColorPair(DECLARATION, window, x, y, line.substr(x, 4));
            x += 4;
        } else if (line.substr(x, 4) == "void") {
            printColorPair(DECLARATION, window, x, y, "void");
            x += 4;
        } else if (line.substr(x, 5) == "while") {
            printColorPair(DECLARATION, window, x, y, "while");
            x += 5;
        } else if (line.substr(x, 3) == "for") {
            printColorPair(DECLARATION, window, x, y, "for");
            x += 3;
        } else if (line.substr(x, 2) == "if") {
            printColorPair(DECLARATION, window, x, y, "if");
            x += 2;
        } else if (line.substr(x, 4) == "else") {
            printColorPair(DECLARATION, window, x, y, "else");
            x += 4;
        } else if (line.substr(x, 6) == "return") {
            printColorPair(RETURN, window, x, y, "return");
            x += 6;
        } else if (line.substr(x, 1) == "\"") {
            insideString = !insideString;
            printColorPair(STRING, window, x, y, "\"");
            x++;
        } else if (line.substr(x, 6) == "public") {
            printColorPair(DECLARATION, window, x, y, "public");
            x += 6;
        } else if (line.substr(x, 7) == "private") {
            printColorPair(DECLARATION, window, x, y, "private");
            x += 7;
        } else if (line.substr(x, 8) == "protected") {
            printColorPair(DECLARATION, window, x, y, "protected");
            x += 8;
        } else if (line.substr(x, 5) == "class") {
            printColorPair(DECLARATION, window, x, y, "class");
            x += 5;
        } else if (line.substr(x, 6) == "return") {
            printColorPair(RETURN, window, x, y, "return");
            x += 6;
        } else if (line.substr(x, 8) == "#include") {
            printColorPair(INCLUDE, window, x, y, "#include");
            x += 8;
        } else if (line.substr(x, 1) == "/" && line.substr(x + 1, 1) == "*") {
            insideComment = !insideComment;
            printColorPair(COMMENT, window, x, y, "/");
            printColorPair(COMMENT, window, x + 1, y, "*");
            x += 2;
        } else if (line.substr(x, 1) == "/" && line.substr(x + 1, 1) == "/") {
            insideComment = !insideComment;
            printColorPair(COMMENT, window, x, y, "/");
            printColorPair(COMMENT, window, x + 1, y, "/");
            x += 2;
        } else {
            if (insideString) {
                printColorPair(STRING, window, x, y, line.substr(x, 1));
            } else if (insideComment) {
                printColorPair(COMMENT, window, x, y, line.substr(x, 1));
            } else {
                printColorPair(DEFAULT_PAIR, window, x, y, line.substr(x, 1));
            }
            x++;
        }
    }
}
