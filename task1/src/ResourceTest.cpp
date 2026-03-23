#include "../include/ResourceTest.h"
#include "../include/utils.hpp"

#include <fstream>

std::string ResourceTest::name() const {
    return "ResourceTest";
}

bool ResourceTest::run() {
    for (const auto& path : paths_) {
        std::ifstream in(path);
        if (!in.good()) {
            LOG_WARNING("ResourceTest: missing resource " + path);
            return false;
        }
    }
    LOG_INFO("ResourceTest passed");
    return true;
}