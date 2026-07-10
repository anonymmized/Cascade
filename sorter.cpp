#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include "./editor.hpp"

std::vector<std::string> stringToVector(const std::string& stringToConvert) {
    std::vector<std::string> finalVector;
    std::istringstream stream(stringToConvert);
    std::string line;
    while (std::getline(stream, line)) {
        finalVector.push_back(line);
    }
    return finalVector;
}

int main() {
    Editor editor;
    editor.setOldFileName("./editor.cpp");
    editor.setFileCode();
    std::string headString = editor.getFileHead();
    auto vectorWithHead = stringToVector(headString);
    std::sort(begin(vectorWithHead), end(vectorWithHead));
    for (auto& str : vectorWithHead) {
        std::cout << str << '\n';
    }
    return 0;
}
