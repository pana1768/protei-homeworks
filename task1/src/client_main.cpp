#include "TcpJsonProtocol.h"
#include "utils.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

static bool readVec4(tcpjson::Vec4& v) {
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    std::vector<double> nums;
    if (!parseVectorFromString(line, nums)) return false;
    if (nums.size() != 4) return false;
    for (size_t i = 0; i < 4; ++i) v[i] = nums[i];
    return true;
}

int main(int argc, char* argv[]) {
    initLogging("client.log");

    std::string host = "127.0.0.1";
    std::uint16_t port = 8080;
    if (argc >= 2) host = argv[1];
    if (argc >= 3) {
        const int p = std::atoi(argv[2]);
        if (p > 0 && p < 65536) port = static_cast<std::uint16_t>(p);
    }

    const int fd = tcpjson::connectTo(host, port);
    if (fd < 0) {
        LOG_ERROR("client: connect failed");
        return 1;
    }
    LOG_INFO(("connected to " + host + ":" + std::to_string(port)).c_str());

    std::cout << "Connected to " << host << ":" << port << "\n";
    std::cout << "Enter count: ";
    int n = 0;
    std::cin >> n;
    std::cin.ignore(10000, '\n');
    if (n <= 0) {
        tcpjson::closeFd(fd);
        return 0;
    }

    std::vector<tcpjson::Vec4> vectors;
    vectors.reserve(static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) {
        std::cout << "v" << (i + 1) << " (4 numbers): ";
        tcpjson::Vec4 v{};
        if (!readVec4(v)) {
            LOG_ERROR("client: bad vector input");
            tcpjson::closeFd(fd);
            return 1;
        }
        vectors.push_back(v);
    }

    const std::string req = tcpjson::encodeVectors(vectors);
    if (!tcpjson::sendFrame(fd, req)) {
        LOG_ERROR("client: send failed");
        tcpjson::closeFd(fd);
        return 1;
    }
    LOG_INFO(("sent vectors: " + std::to_string(vectors.size())).c_str());
    std::string resp;
    if (!tcpjson::recvFrame(fd, resp)) {
        LOG_ERROR("client: recv failed");
        tcpjson::closeFd(fd);
        return 1;
    }
    tcpjson::closeFd(fd);
    LOG_INFO("response received");

    std::vector<tcpjson::Vec4> out;
    if (!tcpjson::decodeVectors(resp, out)) {
        std::cout << resp << "\n";
        return 0;
    }

    for (size_t i = 0; i < out.size(); ++i) {
        std::cout << "out" << (i + 1) << ": "
                  << out[i][0] << " "
                  << out[i][1] << " "
                  << out[i][2] << " "
                  << out[i][3] << "\n";
    }

    shutdownLogging();
    return 0;
}

