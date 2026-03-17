#ifndef TCP_JSON_PROTOCOL_H
#define TCP_JSON_PROTOCOL_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace tcpjson {

using Vec4 = std::array<double, 4>;

int connectTo(const std::string& host, std::uint16_t port);
int listenOn(std::uint16_t port, int backlog = 16);
int acceptClient(int listenFd);
void closeFd(int fd);

bool sendFrame(int fd, const std::string& payload);
bool recvFrame(int fd, std::string& payload);

std::string encodeVectors(const std::vector<Vec4>& vectors);
bool decodeVectors(const std::string& json, std::vector<Vec4>& vectors);

std::vector<Vec4> multiplyByMatrix(const std::vector<Vec4>& vectors);

}

#endif

