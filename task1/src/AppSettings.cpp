#include "../include/AppSettings.h"

#include <stdexcept>
#include <ostream>

namespace {

bool isUnsignedNumber(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

}

AppSettings::AppSettings(int argc, char* argv[]) {
    parse(argc, argv);
}

void AppSettings::setUserName(const std::string& name) {
    if (name.empty()) {
        return;
    }
    userName_ = name;
}

void AppSettings::print(std::ostream& os) const {
    os << "Current configuration:\n";
    os << "  Address : " << address_  << "\n";
    os << "  Port    : " << port_     << "\n";
    os << "  Role    : " << role_     << "\n";
    os << "  ID      : " << id_       << "\n";
    os << "  Library : " << (library_.empty() ? "<empty>" : library_) << "\n";
    os << "  User    : " << userName_ << "\n";
}

void AppSettings::parse(int argc, char* argv[]) {
    if (argc <= 1) {
        return;
    }

    for (int i = 1; i < argc; ++i) {
        const char* raw = argv[i];
        std::string arg = raw ? raw : "";

        if (arg == "-a" && i + 1 < argc) {
            address_ = argv[++i];
        } else if (arg == "-p" && i + 1 < argc) {
            std::string value = argv[++i];
            if (!isUnsignedNumber(value)) {
                throw std::runtime_error("invalid value for -p (port)");
            }
            port_ = std::stoi(value);
        } else if (arg == "-r" && i + 1 < argc) {
            role_ = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            std::string value = argv[++i];
            if (!isUnsignedNumber(value)) {
                throw std::runtime_error("invalid value for -i (id)");
            }
            id_ = std::stoi(value);
        } else if (arg == "-L" && i + 1 < argc) {
            library_ = argv[++i];
        } else if (arg == "-u" && i + 1 < argc) {
            userName_ = argv[++i];
        } else {
            throw std::runtime_error("unknown or incomplete argument: " + arg);
        }
    }
}

