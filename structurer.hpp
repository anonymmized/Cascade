#pragma once

#include <string>
#include <utility>
#include <vector>
#include <fstream>

class Structurer {
    private:
        std::vector<std::pair<std::string, std::vector<std::string>>> callGraph;
        std::string targetFile;
        std::string fileCode;
        size_t mainBracePos;
        std::string mainName;
    public:
        Structurer(const std::string& _targetFile) : targetFile(_targetFile) {}
        Structurer() = default;
        void setFile(const std::string& _targetFile);
        std::ifstream openFile();
        void readCodeFromFile();
        std::vector<std::string> findCalls(const std::string& functionBody);
        std::string getBody(const std::string& fnName);
        size_t getBracePosition(const std::string& fnName);
        void addToGraph(const std::string& fnName);
        void setMainBraceAndName();
        std::string getMainName();
        std::vector<std::pair<std::string, std::vector<std::string>>> getGraph();
};
