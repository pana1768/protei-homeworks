#include "../include/NetworkAddress.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

std::string octetsToString(const std::vector<int>& octets) {
    if (octets.size() < 4) {
        throw std::runtime_error("NetworkAddress: need 4 octets");
    }
    std::ostringstream os;
    os << octets[0] << "." << octets[1] << "." << octets[2] << "." << octets[3];
    return os.str();
}

std::string hexToString(std::uint32_t hexAddr) {
    std::ostringstream os;
    os << ((hexAddr >> 24) & 0xFF) << "."
       << ((hexAddr >> 16) & 0xFF) << "."
       << ((hexAddr >> 8) & 0xFF) << "."
       << (hexAddr & 0xFF);
    return os.str();
}

} 

NetworkAddress::NetworkAddress(const std::string& addrAndPort) {
    auto colon = addrAndPort.find(':');
    if (colon == std::string::npos) {
        address_ = addrAndPort;
        port_ = 0;
        return;
    }
    address_ = addrAndPort.substr(0, colon);
    try {
        port_ = std::stoi(addrAndPort.substr(colon + 1));
    } catch (...) {
        throw std::runtime_error("NetworkAddress: invalid port in string");
    }
}

NetworkAddress::NetworkAddress(const std::vector<int>& octets, int port) {
    address_ = octetsToString(octets);
    port_ = port;
}

NetworkAddress::NetworkAddress(std::uint32_t hexAddr, int port) {
    address_ = hexToString(hexAddr);
    port_ = port;
}

void NetworkAddress::print(std::ostream& out) const {
    out << "Address: " << address_ << "\n"
        << "Port: " << port_ << "\n";
}
