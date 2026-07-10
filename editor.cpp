#include "editor.hpp"
#include "structurer.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

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
    std::regex re(R"((?:^[ \t]*(?://[^\n]*|/\*[\s\S]*?\*/)[ \t]*\n)*^[A-Za-z_][^\n(]*?\w+\s*\([^)]*\)\s*\{)", std::regex::multiline);
    std::smatch match;
    if (!std::regex_search(fileCode, match, re)) {
        return fileCode;
    }
    size_t firstFunction = match.position(0);
    return fileCode.substr(0, firstFunction);
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

    fileToWrite << functionBody<< "\n\n";
}

void Editor::addAllFunctions() {
    Structurer structurer;
    structurer.setFile(oldFileName);
    structurer.analyze();
    rightOrder = structurer.getOrder();
    for (auto& fn : rightOrder) {
        if (fn != "main") {
            std::string fnDefinition = structurer.getFullDefinition(fn);
            addFunctionToFile(fnDefinition);
        }
    }
    std::string mainDefinition = structurer.getFullDefinition("main");
    addFunctionToFile(mainDefinition);
    definedCount = structurer.getDefinedCount();
}

void Editor::writeHeadToFile() {
    std::string fileHead = getFileHead();
    std::vector<std::string> sortedHead = sortFileHead(fileHead);
    std::string result;
    for (auto& str : sortedHead) {
        result += str + '\n';
    }
    writeToFile(result);
}

std::vector<std::string> Editor::sortFileHead(const std::string& fileHead) {
    std::vector<std::string> finalVector;
    std::istringstream stream(fileHead);
    std::string line;
    while (std::getline(stream, line)) {
        finalVector.push_back(line);
    }
    std::sort(begin(finalVector), end(finalVector));
    return finalVector;
}

void Editor::edit() {
    if (oldFileName.empty()) {
        std::cerr << "The filename was not set\n";
        return;
    }
    createNewFileName();
    setFileCode();
    writeHeadToFile();
    addAllFunctions();
    if (definedCount != rightOrder.size()) {
        std::cerr << "Added functions do not match initial quantity\n";
        return;
    }
    renameFile();
}

void Editor::renameFile() {
    std::error_code ec;
    std::filesystem::rename(newFileName, oldFileName, ec);
    if (ec) {
        std::cerr << "Rename failed: " << ec.message() << '\n';
        return;
    }
}

int main() {
    Editor editor;
    editor.setOldFileName("./testCode.cpp");
    editor.edit();
}

