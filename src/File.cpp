#include "File.h"
using namespace std;

void File::openFile(string fileName) {
    if (!checkFileExists(fileName)) {
        string file;
        cout << "Luodaanko file [Y/n]";
        getline(cin, file);

        if (file.empty()) {
            file = "Y";
        }

        transform(file.begin(), file.end(), file.begin(), ::toupper);
        if (file != "Y") {
            exit(0);
        }
        createFile(fileName);
    }
    getFileContent(fileName);
};

void File::saveToFile(string data, string fileName) {
    ofstream outputFile(fileName);
    outputFile << data;
}

const vector<string> &File::getTextLines() const {
    return textLines;
}

bool File::checkFileExists(string fileName) {
    ifstream file(fileName);
    if (file) {
        cout << "on olemassa" << endl;
        return true;
    } else {
        cout << "ei ole olemassa" << endl;

        return false;
    }
};

void File::createFile(string fileName) {
    cout << "createFile" << endl;
    ofstream outputFile(fileName);
};

string File::getFileContent(const string &fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        fileContent = content;
        textLines = splitLines(content);
        file.close();
        return content;
    }
    return "";
}
vector<string> File::splitLines(const string &content) {
    vector<string> lines;
    istringstream iss(content);
    string line;
    while (getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
};
