#include "../include/TcpJsonProtocol.h"
#include "../include/utils.hpp"


#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

struct ServerOptions {
    std::uint16_t port{0};
    bool help{false};
};

bool isUnsignedNumber(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

ServerOptions parseServerOptions(int argc, char* argv[]) {
    ServerOptions opt{};
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i] ? argv[i] : "";
        if (arg == "--help") {
            opt.help = true;
            return opt;
        }
        if (arg == "-p" && i + 1 < argc) {
            const std::string value = argv[++i];
            if (!isUnsignedNumber(value)) throw std::runtime_error("invalid -p");
            const int p = std::stoi(value);
            if (p <= 0 || p > 65535) throw std::runtime_error("port out of range");
            opt.port = static_cast<std::uint16_t>(p);
            continue;
        }
        throw std::runtime_error("unknown or incomplete argument: " + arg);
    }
    if (opt.port == 0) throw std::runtime_error("required: -p <port>");
    return opt;
}

void printHelp() {
    std::cout
        << "MyServer\n"
        << "Usage:\n"
        << "  MyServer -p <listen_port> [--help]\n"
        << "Required:\n"
        << "  -p   TCP port for incoming client connections\n";
}

void processClient(int clientFd) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    LOG_INFO("client accepted");

    while (true) {
        std::string req;
        if (!tcpjson::recvFrame(clientFd, req)) {
            LOG_INFO("client disconnected");
            break;
        }
        LOG_INFO("request received");

        std::vector<tcpjson::Vec4> vectors;
        if (!tcpjson::decodeVectors(req, vectors)) {
            LOG_WARNING("bad request json");
            tcpjson::sendFrame(clientFd, "{\"error\":\"bad_request\"}");
            continue;
        }

        const double delta = dist(gen);
        LOG_INFO(("processing vectors: " + std::to_string(vectors.size()) +
                  ", delta=" + std::to_string(delta)).c_str());
        for (auto& vec : vectors) {
            for (double& x : vec) x += delta;
        }
        const std::string resp = tcpjson::encodeVectors(vectors);
        if (!tcpjson::sendFrame(clientFd, resp)) {
            LOG_WARNING("response send failed");
            break;
        }
        LOG_INFO("response sent");
    }
    tcpjson::closeFd(clientFd);
}

}

int main(int argc, char* argv[]) {
    initLogging("server.log");

    ServerOptions opt{};
    try {
        opt = parseServerOptions(argc, argv);
    } catch (const std::exception& ex) {
        LOG_ERROR(ex.what());
        std::cerr << "Failed: " << ex.what() << "\n";
        printHelp();
        shutdownLogging();
        return 1;
    }
    if (opt.help) {
        printHelp();
        shutdownLogging();
        return 0;
    }

    const int listenFd = tcpjson::listenOn(opt.port);
    if (listenFd < 0) {
        LOG_ERROR("server: listen failed");
        shutdownLogging();
        return 1;
    }

    LOG_INFO(("server listening on port " + std::to_string(opt.port)).c_str());
    std::cout << "Server listening on port " << opt.port << "\n";

    std::mutex qMutex;
    std::condition_variable qCv;
    std::queue<int> accepted;
    bool stop = false;
    std::vector<std::thread> workers;
    workers.reserve(4);
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back([&]() {
            while (true) {
                int fd = -1;
                {
                    std::unique_lock<std::mutex> lock(qMutex);
                    qCv.wait(lock, [&]() { return stop || !accepted.empty(); });
                    if (stop && accepted.empty()) return;
                    fd = accepted.front();
                    accepted.pop();
                }
                processClient(fd);
            }
        });
    }

    while (true) {
        const int clientFd = tcpjson::acceptClient(listenFd);
        if (clientFd < 0) continue;
        {
            std::lock_guard<std::mutex> lock(qMutex);
            accepted.push(clientFd);
        }
        qCv.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(qMutex);
        stop = true;
    }
    qCv.notify_all();
    for (auto& t : workers) t.join();
    tcpjson::closeFd(listenFd);
    shutdownLogging();
    return 0;
}

