#pragma once

#include <string>
#include <vector>

class Editor {
    private:
        std::string fileCode;
        std::string oldFileName;
        std::string newFileName;
        std::vector<std::string> rightOrder;

        bool isFunctionLine(const std::string& line);
        void createNewFileName();
        void writeToFile(const std::string& strToWrite);
        void addFunctionToFile(const std::string& functionBody);
        std::string getFileHead();
        void addAllFunctions();
        void setFileCode();
        void writeHeadToFile();
    public:
        Editor(const std::string& fileName) : oldFileName(fileName) {}
        Editor() = default;
        void setOldFileName(const std::string& fileName);
        void edit();
};
