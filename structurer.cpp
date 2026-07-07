#include "./structurer.hpp"

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <set>
#include <algorithm>

void Structurer::setFile(const std::string& _targetFile) {
    targetFile = _targetFile;
}

void Structurer::analyze() {
    readCodeFromFile();
    collectDefinedFunctions();

    for (auto& fn : definedFunctions) {
        addToGraph(fn);
    }

    std::set<std::string> calledFunctions;
    for (auto& [fn, calls] : callGraph) {
        for (auto& c : calls) {
            calledFunctions.insert(c);
        }
    }

    for (auto& fn : definedFunctions) {
        if (!calledFunctions.count(fn)) {
            addToOrder(fn);
        }
    }
}

void Structurer::readCodeFromFile() {
    std::ifstream openedFile = openFile();
    std::string line;
    while (std::getline(openedFile, line)) {
        fileCode += line + '\n';
    }
}

std::ifstream Structurer::openFile() {
    std::ifstream openedFile(targetFile);
    if (!openedFile.is_open()) {
        std::cerr << "File: " << targetFile << " wasn't opened\n";
    }
    return openedFile;
}

void Structurer::collectDefinedFunctions() {
    std::regex re(R"(^[A-Za-z_][^\n(]*?(\w+)\s*\([^)]*\)\s*\{)", std::regex::multiline);
    static const std::set<std::string> keywords =
        {"if", "else", "for", "while", "switch", "catch", "do", "return", "sizeof"};
    auto foundRegex = std::sregex_iterator(fileCode.begin(), fileCode.end(), re);
    for (; foundRegex != std::sregex_iterator(); foundRegex++) {
        std::string name = (*foundRegex)[1];
        if (keywords.count(name)) {
            continue;
        }
        definedFunctions.insert(name);
    }
}

void Structurer::addToGraph(const std::string& fnName) {
    std::string body = getBody(fnName);
    std::vector<std::string> calls = findCalls(body);
    callGraph.push_back({fnName, calls});
}

std::string Structurer::getBody(const std::string& fnName) {
    size_t functionStart = getBracePosition(fnName);
    if (functionStart == std::string::npos) {
        return "";
    }

    int depth = 0;
    size_t i = functionStart;
    for (; i < fileCode.size(); i++) {
        char curr = fileCode[i];
        if (curr == '"') {
            i += 1;
            while (i < fileCode.size() && fileCode[i] != '"') {
                if (fileCode[i] == '\\') {
                    i += 1;
                }
                i += 1;
            }
            continue;
        }
        if (curr == '\'') {
            i += 1;
            while (i < fileCode.size() && fileCode[i] != '\'') {
                if (fileCode[i] == '\\') {
                     i += 1;
                }
                i += 1;
            }
            continue;
        }
        if (curr == '/' && i + 1 < fileCode.size() && fileCode[i+1] == '/') {
            while (i < fileCode.size() && fileCode[i] != '\n') {
                i += 1;
            }
            continue;
        }
        if (curr == '/' && i + 1 < fileCode.size() && fileCode[i+1] == '*') {
            i += 2;
            while (i + 1 < fileCode.size() && !(fileCode[i] == '*' && fileCode[i+1] == '/')) {
                i += 1;
            }
            continue;
        }
        if (curr == '{') {
            depth += 1;
        } else if (curr == '}' && --depth == 0) {
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

std::vector<std::string> Structurer::findCalls(const std::string& functionBody) {
    std::vector<std::string> functionsNames;
    std::regex re(R"((\w+)\s*\()");
    auto foundRegex = std::sregex_iterator(functionBody.begin(), functionBody.end(), re);
    for (; foundRegex != std::sregex_iterator(); foundRegex++) {
        std::string name = (*foundRegex)[1];
        if (definedFunctions.count(name)) {
            functionsNames.push_back(name);
        }
    }
    return functionsNames;
}

void Structurer::addToOrder(const std::string& fnName) {
    if (std::find(finalOrder.begin(), finalOrder.end(), fnName) != finalOrder.end()) {
        return;
    }
    std::string body = getBody(fnName);
    finalOrder.push_back(fnName);
    for (auto func : findCalls(body)) {
        addToOrder(func);
    }
}

const std::vector<std::string>& Structurer::getOrder() const {
    return finalOrder;
}



int main() {
    Structurer structer;
    structer.setFile("./structurer.cpp");
    structer.analyze();
    const auto& finalOrder = structer.getOrder();
    for (auto& fn : finalOrder) {
        std::cout << "Name: " << fn << '\n';
    }
    return 0;
}
