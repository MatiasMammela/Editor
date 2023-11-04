#include "../headers/File.h"
#include "File.h"
using namespace std;

void File::openFile(string fileName)
{

    if (!checkFileExists(fileName))
    {
        string file;
        cout << "Luodaanko file [Y/n]";
        getline(cin, file);

        if (file.empty())
        {
            file = "Y";
        }

        transform(file.begin(), file.end(), file.begin(), ::toupper);
        if (file != "Y")
        {
            exit(0);
        }
        createFile(fileName);
    }
    getFileContent(fileName);
};

void File::saveToFile(string data, string fileName)
{
    ofstream outputFile(fileName);
    outputFile << data;
}
bool File::checkFileExists(string fileName)
{
    ifstream file(fileName);
    if (file)
    {
        cout << "on olemassa" << endl;
        return true;
    }
    else
    {
        cout << "ei ole olemassa" << endl;

        return false;
    }
};

void File::createFile(string fileName)
{
    cout << "createFile" << endl;
    ofstream outputFile(fileName);
};

void File::getFileContent(string fileName)
{
    cout << "getFileContent" << endl;
    ifstream file(fileName);
    file.open(fileName, std::ifstream::in);

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    fileContent = content;
};
