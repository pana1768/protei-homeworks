#ifndef NETWORK_ADDRESS_H
#define NETWORK_ADDRESS_H

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

class NetworkAddress {
public:
    explicit NetworkAddress(const std::string& addrAndPort);
    explicit NetworkAddress(const std::vector<int>& octets, int port = 0);
    explicit NetworkAddress(std::uint32_t hexAddr, int port = 0);

    const std::string& address() const noexcept { return address_; }
    int port() const noexcept { return port_; }
    void print(std::ostream& out) const;

private:
    std::string address_;
    int port_ = 0;
};

#endif