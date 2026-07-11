#include <iostream>
#include <algorithm>
#include <string>

#include "editor.hpp"
// #include "structurer.hpp"


struct Options {
    std::string fileName = "";
    bool inPlace = false;
    bool help = false;
    std::string errorOutput;
};

Options parseArgs(int argc, char **argv) {
    Options opts;
    for (int i = 1; i < argc; i++) {
        std::string currentArg = argv[i];
        if (currentArg == "--in-place" || currentArg == "-i") {
            opts.inPlace = true;
        }
        else if (currentArg == "--help" || currentArg == "-h") {
            opts.help = true;
        }
        else if (!currentArg.empty() && currentArg[0] != '-') {
            opts.fileName = currentArg;
        }
        else {
            opts.errorOutput = "The unknown argument " + currentArg + " check --help (-h) for actual arguments\n";
            return opts;
        }
    }
    return opts;
}

void printHelp() {
    std::cout <<
        "Cascade — reorder C++ functions in call order (stepdown rule)\n"
        "and sort includes.\n"
        "\n"
        "Usage:\n"
        "  cascade <file> [options]\n"
        "  cascade --help\n"
        "\n"
        "Arguments:\n"
        "  <file>            Path to a C++ source file (e.g. src/main.cpp).\n"
        "\n"
        "Options:\n"
        "  -i, --in-place   Overwrite the original file instead of writing a copy.\n"
        "  -h,  --help       Show this help and exit.\n"
        "\n"
        "Output:\n"
        "  By default writes the result next to the original with \"_new\"\n"
        "  before the extension (main.cpp -> main_new.cpp). The original\n"
        "  is left untouched unless --in-place is given.\n"
        "\n"
        "Example:\n"
        "  cascade src/parser.cpp\n";
}

int main(int argc, char **argv) {
    Options foundOpts = parseArgs(argc, argv);
    if (!foundOpts.errorOutput.empty()) {
        std::cout << foundOpts.errorOutput << '\n';
        return 1;
    }
    if (foundOpts.help) {
        printHelp();
        return 0;
    }
    if (foundOpts.fileName.empty()) {
        std::cout << "There is no file to work with. Plese select target file\n";
        return 1;
    }
    Editor editor;
    editor.setOldFileName(foundOpts.fileName);
    editor.edit(foundOpts.inPlace);
    return 0;
}
