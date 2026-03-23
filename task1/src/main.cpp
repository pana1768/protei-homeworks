#include "../include/AppSettings.h"
#include "../include/AppContext.h"
#include "../include/utils.hpp"

#include <iostream>

namespace {
void printHelp() {
    std::cout
        << "MyApp client\n"
        << "Usage:\n"
        << "  MyApp -i <server_ipv4> -p <server_port> [-u <user>] [--help]\n"
        << "Required:\n"
        << "  -i   IPv4 address of server\n"
        << "  -p   TCP server port\n"
        << "Optional:\n"
        << "  -u   user alias\n"
        << "  --help  show this help\n";
}
}

int main(int argc, char* argv[]) {
    try {
        initLogging("app.log");
        AppSettings settings(argc, argv);
        if (settings.helpRequested()) {
            printHelp();
            shutdownLogging();
            return 0;
        }

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
    shutdownLogging();
    return 0;
}