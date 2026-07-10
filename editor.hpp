#pragma once

#include <string>
#include <vector>

class Editor {
    private:
        std::string fileCode;
        std::string oldFileName;
        std::string newFileName;
        std::vector<std::string> rightOrder;
        size_t definedCount;

        void createNewFileName();
        void writeToFile(const std::string& strToWrite);
        void addFunctionToFile(const std::string& functionBody);
        void addAllFunctions();
        //void setFileCode();
        void writeHeadToFile();
        void renameFile();
    public:
        Editor(const std::string& fileName) : oldFileName(fileName) {}
        Editor() = default;
        std::string getFileHead();
        void setOldFileName(const std::string& fileName);
        void setFileCode();
        void edit();
};
