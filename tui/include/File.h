#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class File {
public:
    string FilePath;
    void openFile(string fileName);
    string fileContent;
    void saveToFile(string data, string fileName);
    vector<string> textLines;
    const vector<string> &getTextLines() const;
    void constructFileContent();

private:
    bool checkFileExists(string fileName);
    void createFile(string fileName);
    string getFileContent(const std::string &fileName);
    vector<string> splitLines(const string &content);
};