#include "../include/AppSettings.h"
#include "../include/AppContext.h"
#include "../include/utils.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    try {
        AppSettings settings(argc, argv);

        std::cout << "Application started.\n";
        settings.print(std::cout);
        std::cout << "\n";

        AppContext ctx(std::move(settings));
        mainLoop(ctx);
    } catch (const std::exception& ex) {
        LOG_ERROR(ex.what());
        std::cerr << "Failed: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}