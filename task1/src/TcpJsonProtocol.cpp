#include "../include/TcpJsonProtocol.h"
#include "../include/utils.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <sstream>

namespace tcpjson {

static bool writeAll(int fd, const void* buf, size_t len) {
    const auto* p = static_cast<const unsigned char*>(buf);
    size_t off = 0;
    while (off < len) {
        const ssize_t n = ::send(fd, p + off, len - off, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        if (n == 0) return false;
        off += static_cast<size_t>(n);
    }
    return true;
}

static bool readAll(int fd, void* buf, size_t len) {
    auto* p = static_cast<unsigned char*>(buf);
    size_t off = 0;
    while (off < len) {
        const ssize_t n = ::recv(fd, p + off, len - off, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        if (n == 0) return false;
        off += static_cast<size_t>(n);
    }
    return true;
}

int connectTo(const std::string& host, std::uint16_t port) {
    struct addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res = nullptr;
    const std::string portStr = std::to_string(port);
    if (::getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res) != 0) {
        return -1;
    }

    int fd = -1;
    for (auto* p = res; p; p = p->ai_next) {
        fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) continue;
        if (::connect(fd, p->ai_addr, p->ai_addrlen) == 0) break;
        ::close(fd);
        fd = -1;
    }
    ::freeaddrinfo(res);
    return fd;
}

int listenOn(std::uint16_t port, int backlog) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int yes = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(fd);
        return -1;
    }
    if (::listen(fd, backlog) < 0) {
        ::close(fd);
        return -1;
    }
    return fd;
}

int acceptClient(int listenFd) {
    sockaddr_in client {};
    socklen_t len = sizeof(client);
    return ::accept(listenFd, reinterpret_cast<sockaddr*>(&client), &len);
}

void closeFd(int fd) {
    if (fd >= 0) ::close(fd);
}

bool sendFrame(int fd, const std::string& payload) {
    const std::uint32_t n = static_cast<std::uint32_t>(payload.size());
    const std::uint32_t be = htonl(n);
    return writeAll(fd, &be, sizeof(be)) && (n == 0 || writeAll(fd, payload.data(), n));
}

bool recvFrame(int fd, std::string& payload) {
    std::uint32_t be = 0;
    if (!readAll(fd, &be, sizeof(be))) return false;
    const std::uint32_t n = ntohl(be);
    if (n > (64u * 1024u * 1024u)) return false;
    payload.assign(n, '\0');
    if (n == 0) return true;
    return readAll(fd, payload.data(), n);
}

static void skipWs(const char*& p) {
    while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
}

static bool consume(const char*& p, char c) {
    skipWs(p);
    if (*p != c) return false;
    ++p;
    return true;
}

static bool parseString(const char*& p, std::string& out) {
    skipWs(p);
    if (*p != '"') return false;
    ++p;
    out.clear();
    while (*p && *p != '"') {
        const char ch = *p++;
        if (ch == '\\') {
            if (!*p) return false;
            out += *p++;
        } else {
            out += ch;
        }
    }
    if (*p != '"') return false;
    ++p;
    return true;
}

static bool parseNumber(const char*& p, double& out) {
    skipWs(p);
    char* end = nullptr;
    errno = 0;
    const double v = std::strtod(p, &end);
    if (end == p) return false;
    if (errno == ERANGE) return false;
    out = v;
    p = end;
    return std::isfinite(out);
}

std::string encodeVectors(const std::vector<Vec4>& vectors) {
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os.precision(10);
    os << "{\"vectors\":[";
    for (size_t i = 0; i < vectors.size(); ++i) {
        if (i) os << ",";
        os << "[";
        for (size_t j = 0; j < 4; ++j) {
            if (j) os << ",";
            os << vectors[i][j];
        }
        os << "]";
    }
    os << "]}";
    return os.str();
}

bool decodeVectors(const std::string& json, std::vector<Vec4>& vectors) {
    vectors.clear();
    const char* p = json.c_str();
    if (!consume(p, '{')) return false;

    bool found = false;
    while (true) {
        std::string key;
        if (!parseString(p, key)) return false;
        if (!consume(p, ':')) return false;
        if (key == "vectors") {
            if (!consume(p, '[')) return false;
            found = true;
            skipWs(p);
            if (*p == ']') {
                ++p;
            } else {
                while (true) {
                    if (!consume(p, '[')) return false;
                    Vec4 v{};
                    for (int i = 0; i < 4; ++i) {
                        double n = 0.0;
                        if (!parseNumber(p, n)) return false;
                        v[static_cast<size_t>(i)] = n;
                        if (i < 3) {
                            if (!consume(p, ',')) return false;
                        }
                    }
                    if (!consume(p, ']')) return false;
                    vectors.push_back(v);
                    skipWs(p);
                    if (*p == ',') {
                        ++p;
                        continue;
                    }
                    if (*p == ']') {
                        ++p;
                        break;
                    }
                    return false;
                }
            }
        } else {
            return false;
        }

        skipWs(p);
        if (*p == ',') {
            ++p;
            continue;
        }
        if (*p == '}') {
            ++p;
            break;
        }
        return false;
    }

    skipWs(p);
    return found && *p == '\0';
}

std::vector<Vec4> multiplyByMatrix(const std::vector<Vec4>& vectors) {
    const double M[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 2.0, 0.0, 0.0},
        {0.0, 0.0, 3.0, 0.0},
        {0.0, 0.0, 0.0, 4.0},
    };
    std::vector<Vec4> out;
    out.reserve(vectors.size());
    for (const auto& v : vectors) {
        Vec4 r{};
        for (int i = 0; i < 4; ++i) {
            double s = 0.0;
            for (int j = 0; j < 4; ++j) {
                s += M[i][j] * v[static_cast<size_t>(j)];
            }
            r[static_cast<size_t>(i)] = s;
        }
        out.push_back(r);
    }
    return out;
}

} 

