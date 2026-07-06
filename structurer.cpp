#include "./structurer.hpp"

#include <iostream>
#include <string>
#include <regex>
#include <map>
#include <fstream>

// file with code opens and running trough finding MAIN state before function
// then function places on first place, then my app goes trough this function
// and finding new functions that calls from it.

void Structurer::setFile(const std::string& _targetFile) {
    targetFile = _targetFile;
}

std::ifstream Structurer::openFile() {
    std::ifstream openedFile(targetFile);
    if (!openedFile.is_open()) {
        std::cerr << "File " << targetFile << "wasn't opened\n";
    }
    return openedFile;
}

void Structurer::readCodeFromFile() {
    std::ifstream openedFile = openFile();
    std::string line;
    while (std::getline(openedFile, line)) {
        fileCode += line + '\n';
    }
}

std::vector<std::string> Structurer::findCalls(const std::string& functionBody) {
    std::vector<std::string> functionsNames;
    std::regex regularCalls(R"((\w+)\s*\()");
    auto foundRegex = std::sregex_iterator(functionBody.begin(), functionBody.end(), regularCalls);
    for (; foundRegex != std::sregex_iterator(); foundRegex++) {
        functionsNames.push_back(std::string((*foundRegex)[1]));
    }
    return functionsNames;
}

std::string Structurer::getBody(const std::string& fnName) {
    size_t functionStart = getBracePosition(fnName);
    int depth = 0;
    size_t i = functionStart;
    for (; i < fileCode.size(); i++) {
        if (fileCode[i] == '{') {
            depth += 1;
        } else if (fileCode[i] == '}' && --depth == 0) {
            break;
        }
    }

    std::string functionBody = fileCode.substr(functionStart, i - functionStart + 1);
    return functionBody;
}

size_t Structurer::getBracePosition(const std::string& fnName) {
    std::regex re(R"(\b)" + fnName + R"(\s*\([^)]*\)\s*\{)");
    std::smatch match;
    if (!std::regex_search(fileCode, match, re)) {
        return std::string::npos;
    }
    return match.position(0) + match.length(0) - 1;
}

void Structurer::addToGraph(const std::string& fnName) {
    std::string body = getBody(fnName);
    std::vector<std::string> calls = findCalls(body);
    callGraph.push_back({fnName, calls});
}

void Structurer::setMainBraceAndName() {
    std::regex re(R"(//\s*MAIN\s*[\r\n]+[^\n(]*?(\w+)\s*\()");
    std::smatch match;
    if (std::regex_search(fileCode, match, re)) {
        mainName = match[1];
    }
    size_t functionStart = match.position(1) + match.length(0);
    mainBracePos = fileCode.find('{', functionStart);
}

std::string Structurer::getMainName() {
    return mainName;
}

std::vector<std::pair<std::string, std::vector<std::string>>> Structurer::getGraph() {
    return callGraph;
}

int main() {
    Structurer structer;
    structer.setFile("./testCode.cpp");
    structer.readCodeFromFile();
    std::string mainName = structer.getMainName();
    std::vector<std::pair<std::string, std::vector<std::string>>> callGraph = structer.getGraph();
    structer.addToGraph(mainName);
    for (const auto& fnName : callGraph[0].second) {
        structer.addToGraph(fnName);
    }
    for (const auto& [key, value] : callGraph) {
        std::cout << key << ":\n";
        for (const std::string& call : value) {
            std::cout << "  " << call << '\n';
        }
    }
    return 0;
}
