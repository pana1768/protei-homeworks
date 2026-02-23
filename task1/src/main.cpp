#include <iostream>
#include "../include/utils.hpp"

int main(int argc, char* argv[]) {
    AppConfig config;
    if (!parseCommandLine(argc, argv, config)) {
        std::cerr << "Failed to parse command line arguments.\n";
        return 1;
    }

    std::cout << "Application started.\n";
    printConfig(config);
    std::cout << "\n";

    mainLoop();

    return 0;
}