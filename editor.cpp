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
    if (std::find(rightOrder.begin(), rightOrder.end(), "main") != rightOrder.end()) {
        std::string mainDefinition = structurer.getFullDefinition("main");
        addFunctionToFile(mainDefinition);
    }
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
    std::vector<std::string> includes;
    std::vector<std::string> others;
    std::istringstream stream(fileHead);
    std::string line;
    while (std::getline(stream, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        if (trimmed.rfind("#include", 0) == 0) {
            includes.push_back(line);
        } else if (!trimmed.empty()) {
            others.push_back(line);
        }
    }
    std::sort(begin(includes), end(includes));

    std::vector<std::string> finalVector = others;
    finalVector.insert(finalVector.end(), includes.begin(), includes.end());
    return finalVector;
}

void Editor::edit(bool inPlace) {
    if (oldFileName.empty()) {
        std::cerr << "The filename was not set\n";
        return;
    }
    createNewFileName();
    setFileCode();
    if (checkForHeader()) {
        std::cout << "Found a header file. Continue process\n";
    } else {
        char warn;
        std::cout <<
            "WARNING: no self-header include found — reordering free functions may break compilation.\n"
            "The header may exist under a different name; include it before reordering.\nContinue program? [y/N] ";
        std::cin >> warn;
        if (warn != 'Y' && warn != 'y') {
            std::cout << "Exiting...\n";
            return;
        }
    }
    writeHeadToFile();
    addAllFunctions();
    if (definedCount != rightOrder.size()) {
        std::cerr << "Added functions do not match initial quantity\n";
        return;
    }
    if (inPlace) {
        if (warnUser() == 0) {
            renameFile();
        }
    }
}

std::string Editor::getFileName(const std::string& fileToGet) {
    size_t rightSlash = fileToGet.rfind('/');
    size_t dotPos = fileToGet.rfind('.');
    return fileToGet.substr(rightSlash + 1, dotPos - rightSlash - 1);
}

bool Editor::checkForHeader() {
    std::istringstream stream(fileCode);
    std::string line;
    while (std::getline(stream, line)) {
        if (std::count(line.begin(), line.end(), '"') == 2) {
            size_t leftQuote = line.find('"');
            size_t rightQuote = line.rfind('"');
            std::string includePath = line.substr(leftQuote + 1, rightQuote - leftQuote - 1);

            if (getFileName(includePath) == getFileName(oldFileName)) {
                return true;
            }
        }
    }
    return false;
}

void Editor::renameFile() {
    std::error_code ec;
    std::filesystem::rename(newFileName, oldFileName, ec);
    if (ec) {
        std::cerr << "Rename failed: " << ec.message() << '\n';
        return;
    }
}

int Editor::warnUser() {
    char warn;
    std::cout << "WARNING: --in-place will OVERWRITE the original file:\n" << oldFileName << "\n\n" <<
        "Cascade reorders functions and may produce an order that does not\n"
        "compile (e.g. free functions in a headerless file). In --in-place\n"
        "mode the original is replaced, so a bad reorder cannot be undone.\n\n"
        "Recommended: run without --in-place first and review <name>_new.<ext>.\n"
        "No backup is made.\n"
        "Continue and overwrite the original? [y/N]: ";
    std::cin >> warn;
    if (warn == 'y' || warn == 'Y') {
        return 0;
    }
    std::cout << "Exiting...\n";
    return 1;
}
