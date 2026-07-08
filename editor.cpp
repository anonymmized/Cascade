#include "editor.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

/*
std::string getFileHead(const std::string& fileName) {
    std::ifstream fileWithCode(fileName);
    if (!fileWithCode.is_open()) {

    }
}
*/

void Editor::setFileCode() {
    std::ifstream fileWithCode(oldFileName);
    if (!fileWithCode.is_open()) {
        std::cerr << "The file " << oldFileName << " was not opened\n";
        return;
    }

    std::string line;
    while (std::getline(fileWithCode, line)) {
        fileCode += line + '\n';
    }
}

void Editor::setOldFileName(const std::string& fileName) {
    oldFileName = fileName;
}

void Editor::createNewFileName() {
    size_t dot = oldFileName.find_last_of('.');

    if (dot == std::string::npos) {
        newFileName = oldFileName + "_new";
        return;
    }

    std::string oldName = oldFileName.substr(0, dot);
    std::string oldExt = oldFileName.substr(dot);

    newFileName = oldName + "_new" + oldExt;
}

std::string Editor::getFileHead() {
    std::istringstream stream(fileCode);
    std::string finalHead;
    std::string line;
    while (std::getline(stream, line)) {
        if (isFunctionLine(line)) {
            break;
        }
        finalHead += line + '\n';
    }
    return finalHead;
}

bool Editor::isFunctionLine(const std::string& line) {
    static const std::regex re(R"(^[A-Za-z_][^\n(]*?(\w+)\s*\([^)]*\)\s*\{?\s*$)");
    return std::regex_search(line, re);
}

void Editor::writeToFile(const std::string& strToWrite) {
    std::ofstream newFile(newFileName);
    if (!newFile.is_open()) {
        std::cerr << "The file " << newFileName << " was not opened\n";
        return;
    }

    newFile << strToWrite << '\n';
}

void Editor::addFunctionToFile(const std::string& functionBody) {
    std::ofstream fileToWrite(newFileName, std::ios::app);
    if (!fileToWrite.is_open()) {
        std::cerr << "The file " << newFileName << " was not opened\n";
        return;
    }

    fileToWrite << functionBody << '\n';
}

int main() {
    Editor editor;
    editor.setOldFileName("./structurer.cpp");
    editor.createNewFileName();
    editor.writeToFile("hello");
    editor.writeToFile("world");
}
