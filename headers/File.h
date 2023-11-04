#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#pragma once
using namespace std;

class File
{
public:
    string FilePath;
    void openFile(string fileName);
    string fileContent;
    void saveToFile(string data, string fileName);

private:
    bool checkFileExists(string fileName);
    void createFile(string fileName);
    void getFileContent(string fileName);
};