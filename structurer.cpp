#include <iostream>
#include <string>
#include <regex>
#include <map>
#include <fstream>

// file with code opens and running trough finding MAIN state before function
// then function places on first place, then my app goes trough this function
// and finding new functions that calls from it.

std::map<std::string, std::vector<std::string>> callGraph;

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

std::vector<std::string> findCalls(const std::string& functionBody) {
    std::vector<std::string> functionsNames;
    std::regex regularCalls(R"((\w+)\s*\()");
    auto foundRegex = std::sregex_iterator(functionBody.begin(), functionBody.end(), regularCalls);
    for (; foundRegex != std::sregex_iterator(); foundRegex++) {
        functionsNames.push_back(std::string((*foundRegex)[1]));
    }
    return functionsNames;
}

size_t getBracePosition(const std::string& code, const std::string& fnName) {
    std::regex re(R"(\b)" + fnName + R"(\s*\([^)]*\)\s*\{)");
    std::smatch match;
    if (!std::regex_search(code, match, re)) {
        return std::string::npos;
    }
    return match.position(0) + match.length(0) - 1;
}

std::string getBody(const std::string& code, const std::string& fnName) {
    size_t functionStart = getBracePosition(code, fnName);
    int depth = 0;
    size_t i = functionStart;
    for (; i < code.size(); i++) {
        if (code[i] == '{') {
            depth += 1;
        } else if (code[i] == '}' && --depth == 0) {
            break;
        }
    }
    std::string functionBody = code.substr(functionStart, i - functionStart + 1);
    return functionBody;
}

int main() {
    std::string fullCode = readCodeFromFile("testCode.cpp");
    std::regex reMain(R"(//\s*MAIN\s*[\r\n]+[^\n(]*?(\w+)\s*\()");
    std::smatch functionNameMatch;
    if (std::regex_search(fullCode, functionNameMatch, reMain)) {
        std::cout << "Main function name: " << functionNameMatch[1] << '\n';
    }
    size_t functionStart = functionNameMatch.position(1) + functionNameMatch.length(0);
    size_t whereBraceOpens = fullCode.find('{', functionStart);
    int depth = 0;
    size_t i = whereBraceOpens;
    for (; i < fullCode.size(); i++) {
        if (fullCode[i] == '{') {
            depth += 1;
        } else if (fullCode[i] == '}' && --depth == 0) {
            break;
        }
    }
    std::string functionBody = fullCode.substr(functionStart, i - functionStart + 1);
    std::cout << "Function body:\n" << functionBody << '\n';
    std::vector<std::string> mainCalls = findCalls(functionBody);
    callGraph[functionNameMatch[1]] = mainCalls;
    for (const auto& call : mainCalls) {
        std::string name = call;
        std::string body = getBody(fullCode, name);
        callGraph[name] = findCalls(body);
    }
    for (const auto& [key, value] : callGraph) {
        std::cout << key << ":\n";
        for (const std::string& call : value) {
            std::cout << "  " << call << '\n';
        }
    }
    return 0;
}
