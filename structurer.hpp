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

        void collectDefinedFunctions();
        std::ifstream openFile();
        void readCodeFromFile();
        std::vector<std::string> findCalls(const std::string& functionBody);
        std::string getBody(const std::string& fnName);
        void addToGraph(const std::string& fnName);
        void addToOrder(const std::string& fnName);
        size_t getFunctionEnd(size_t functionStart);
        std::pair<size_t, size_t> findPositions(const std::string& fnName);
    public:
        Structurer(const std::string& _targetFile) : targetFile(_targetFile) {}
        Structurer() = default;

        void setFile(const std::string& _targetFile);
        void analyze();
        const std::vector<std::string>& getOrder() const;
        std::string getFullDefinition(const std::string& fnName);
};
