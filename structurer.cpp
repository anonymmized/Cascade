#include <iostream>
#include <string>
#include <regex>
#include <fstream>

// file with code opens and running trough finding MAIN state before function
// then function places on first place, then my app goes trough this function
// and finding new functions that calls from it.

std::ifstream openFile(const std::string& fileToOpen) {
    std::ifstream openedFile(fileToOpen);
    if (!openedFile.is_open()) {
        std::cerr << "File " << fileToOpen << "wasn't opened\n";
    }
    return openedFile;
}

std::string readCodeFromFile(const std::string& fileToRead) {
    std::ifstream openedFile = openFile(fileToRead);
    std::string fullCode;
    std::string line;
    while (std::getline(openedFile, line)) {
        fullCode += line + '\n';
    }
    return fullCode;
}

int main() {
    std::string fullCode = readCodeFromFile("testCode.cpp");
    std::regex regular(R"(//\s*MAIN\s*[\r\n]+[^\n(]*?(\w+)\s*\()");
    std::smatch regexMatch;
    if (std::regex_search(fullCode, regexMatch, regular)) {
        std::cout << "Main function name: " << regexMatch[1] << '\n';
    }
    return 0;
}
