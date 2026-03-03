#include <iostream>
#include <exception>

#include "../include/utils.hpp"

int main(int argc, char* argv[]) {
    AppConfig config;

    try {
        parseCommandLine(argc, argv, config);
    } catch (const std::exception& ex) {
        LOG_ERROR(ex.what());
        return 1;
    }

    std::cout << "Application started.\n";
    printConfig(config);
    std::cout << "\n";

    mainLoop();

    return 0;
}