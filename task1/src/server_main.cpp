#include "../include/TcpJsonProtocol.h"
#include "../include/utils.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) {
    initLogging("server.log");

    std::uint16_t port = 8080;
    if (argc >= 2) {
        const int p = std::atoi(argv[1]);
        if (p > 0 && p < 65536) port = static_cast<std::uint16_t>(p);
    }

    const int listenFd = tcpjson::listenOn(port);
    if (listenFd < 0) {
        LOG_ERROR("server: listen failed");
        return 1;
    }

    LOG_INFO(("server listening on port " + std::to_string(port)).c_str());
    std::cout << "Server listening on port " << port << "\n";

    while (true) {
        const int clientFd = tcpjson::acceptClient(listenFd);
        if (clientFd < 0) continue;
        LOG_INFO("client accepted");

        std::string req;
        if (!tcpjson::recvFrame(clientFd, req)) {
            LOG_WARNING("failed to read request");
            tcpjson::closeFd(clientFd);
            continue;
        }
        LOG_INFO("request received");
        std::vector<tcpjson::Vec4> vectors;
        if (!tcpjson::decodeVectors(req, vectors)) {
            LOG_WARNING("bad request json");
            tcpjson::sendFrame(clientFd, "{\"error\":\"bad_request\"}");
            tcpjson::closeFd(clientFd);
            continue;
        }
        LOG_INFO(("processing vectors: " + std::to_string(vectors.size())).c_str());
        const auto out = tcpjson::multiplyByMatrix(vectors);
        const std::string resp = tcpjson::encodeVectors(out);
        tcpjson::sendFrame(clientFd, resp);
        LOG_INFO("response sent");
        tcpjson::closeFd(clientFd);
    }

    tcpjson::closeFd(listenFd);
    shutdownLogging();
    return 0;
}

