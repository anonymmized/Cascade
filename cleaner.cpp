#include <iostream>
#include <algorithm>
#include <string>
/*
#include "editor.hpp"
#include "structurer.hpp"
*/

struct Options {
    std::string fileName = "";
    bool inPlace = false;
    bool help = false;
};

Options parseArgs(int argc, char **argv) {
    if (argc == 1) {
        return {};
    }
    Options opts;
    for (int i = 0; i < argc; i++) {
        std::string currentArg = argv[i];
        if (currentArg == "--in-place" || currentArg == "-pl") {
            opts.inPlace = true;
        }
        else if (currentArg == "--help" || currentArg == "-h") {
            opts.help = true;
        }
        else if (currentArg[0] != '-') {
            opts.fileName = currentArg;
        }
        else {
            continue;
        }
    }
    return opts;
}


int main(int argc, char **argv) {
    Options foundOpts = parseArgs(argc, argv);
    if (!foundOpts.fileName.empty()) {
        std::cout << "File name: " << foundOpts.fileName << '\n';
    }
    std::cout << "In place: " << std::to_string(foundOpts.inPlace) << '\n';
    std::cout << "Help: " << std::to_string(foundOpts.help) << '\n';
    return 0;
}
