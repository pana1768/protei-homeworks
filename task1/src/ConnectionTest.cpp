#include "../include/ConnectionTest.h"
#include "../include/utils.hpp"

std::string ConnectionTest::name() const {
    return "ConnectionTest";
}

bool ConnectionTest::run() {
    for (const auto& p : params_) {
        if (p.empty()) {
            LOG_WARNING("ConnectionTest: empty parameter");
            return false;
        }
    }
    LOG_INFO("ConnectionTest passed");
    return true;
}
