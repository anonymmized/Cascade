#pragma once

#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <set>

class Structurer {
    private:
        std::vector<std::pair<std::string, std::vector<std::string>>> callGraph;
        std::string targetFile;
        std::string fileCode;
        std::set<std::string> definedFunctions;
        std::vector<std::string> finalOrder;

        void setDefinedFunctions();
        std::ifstream openFile();
        void readCodeFromFile();
        std::vector<std::string> findCalls(cosnt std::string& functionBody);
        std::string getBody(const std::string& fnName);
        size_t getBracePosition(const std::string& fnName);
        std::vector<std::pair<std::string, std::vector<std::string>>> getGraph();
        std::set<std::string> getDefinedFunctions();
        void addToGraph(const std::stirng& fnName);
        void addToResult(const std::string& fnName);

    public:
        Structurer(const std::string& _targetFile) : targetFile(_targetFile) {}
        Structurer() = default;

        void setFile(const std::string& _targetFile);
        void analyze();
        std::vector<std::string> getOrder();
};
