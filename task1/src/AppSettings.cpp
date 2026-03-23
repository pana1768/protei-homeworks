#include "../include/AppSettings.h"

#include <stdexcept>
#include <ostream>
#include <limits>

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

bool isValidIpv4(const std::string& ip) {
    if (ip.empty()) return false;
    int parts = 0;
    size_t start = 0;
    while (start < ip.size()) {
        const size_t dot = ip.find('.', start);
        const size_t end = (dot == std::string::npos) ? ip.size() : dot;
        const std::string part = ip.substr(start, end - start);
        if (part.empty() || part.size() > 3 || !isUnsignedNumber(part)) return false;
        const int value = std::stoi(part);
        if (value < 0 || value > 255) return false;
        ++parts;
        if (dot == std::string::npos) break;
        start = dot + 1;
    }
    return parts == 4;
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
    os << "  Server IP : " << endpoint_.ipv4 << "\n";
    os << "  Port      : " << endpoint_.port << "\n";
    os << "  Role    : " << role_     << "\n";
    os << "  ID      : " << id_       << "\n";
    os << "  Library : " << (library_.empty() ? "<empty>" : library_) << "\n";
    os << "  User    : " << userName_ << "\n";
}

void AppSettings::parse(int argc, char* argv[]) {
    endpoint_.ipv4.clear();
    endpoint_.port = 0;

    for (int i = 1; i < argc; ++i) {
        const char* raw = argv[i];
        std::string arg = raw ? raw : "";

        if (arg == "--help") {
            helpRequested_ = true;
            return;
        } else if (arg == "-i" && i + 1 < argc) {
            std::string ip = argv[++i];
            if (!isValidIpv4(ip)) {
                throw std::runtime_error("invalid value for -i (ipv4)");
            }
            endpoint_.ipv4 = std::move(ip);
        } else if (arg == "-p" && i + 1 < argc) {
            std::string value = argv[++i];
            if (!isUnsignedNumber(value)) {
                throw std::runtime_error("invalid value for -p (port)");
            }
            const int port = std::stoi(value);
            if (port <= 0 || port > std::numeric_limits<std::uint16_t>::max()) {
                throw std::runtime_error("port out of range");
            }
            endpoint_.port = static_cast<std::uint16_t>(port);
        } else if (arg == "-u" && i + 1 < argc) {
            userName_ = argv[++i];
        } else {
            throw std::runtime_error("unknown or incomplete argument: " + arg);
        }
    }
    if (endpoint_.ipv4.empty() || endpoint_.port == 0) {
        throw std::runtime_error("required flags: -i <server_ipv4> -p <server_port>");
    }
}