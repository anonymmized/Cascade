#include "./structurer.hpp"

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <set>
#include <algorithm>

// file with code opens and running trough finding MAIN state before function
// then function places on first place, then my app goes trough this function
// and finding new functions that calls from it.

void Structurer::setDefinedFunctions() {
    std::regex re(R"(\w+\s+(\w+)\s*\([^)]*\)\s*\{)");
    auto foundRegex = std::sregex_iterator(fileCode.begin(), fileCode.end(), re);
    for (; foundRegex != std::sregex_iterator(); foundRegex++) {
        definedFunctions.insert(std::string((*foundRegex)[1]));
    }
}

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
        std::string name = (*foundRegex)[1];
        if (definedFunctions.count(name)) {
            functionsNames.push_back(name);
        }
    }
    return functionsNames;
}

std::string Structurer::getBody(const std::string& fnName) {
    size_t functionStart = getBracePosition(fnName);
    if (functionStart == std::string::npos) {
        return "";
    }
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

std::vector<std::pair<std::string, std::vector<std::string>>> Structurer::getGraph() {
    return callGraph;
}

std::set<std::string> Structurer::getDefinedFunctions() {
    return definedFunctions;
}

void Structurer::addToResult(const std::string& fnName) {
    if (std::find(result.begin(), result.end(), fnName) != result.end()) {
        return;
    }
    std::string body = getBody(fnName);
    result.push_back(fnName);
    for (auto func : findCalls(body)) {
        addToResult(func);
    }
}

std::vector<std::string> Structurer::getResult() {
    return result;
}

void Structurer::analyze() {
    readCodeFromFile();
    setDefinedFunctions();

    for (auto& fn : definedFunctions) {
        addToGraph(fn);
    }

}


int main() {
    Structurer structer;
    structer.setFile("./structurer.cpp");
    structer.readCodeFromFile();
    structer.setDefinedFunctions();

    for (auto& fn : structer.getSet()) {
        structer.addToGraph(fn);
    }

    std::set<std::string> called;
    for (auto& [fn, calls] : structer.getGraph()) {
        for (auto& c : calls) {
            called.insert(c);
        }
    }

    for (auto& fn : structer.getSet()) {
        if (!called.count(fn)) {
            structer.addToResult(fn);
        }
    }

    for (auto& fn : structer.getResult()) {
        std::cout << fn << '\n';
    }

    return 0;
}
