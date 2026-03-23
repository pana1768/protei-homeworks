#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <iostream>
#include <string>
#include <cstdint>

struct ClientEndpoint {
    std::string ipv4;
    std::uint16_t port = 0;
};

class AppSettings {
public:
    AppSettings(int argc, char* argv[]);

    const std::string& address() const noexcept { return endpoint_.ipv4; }
    int port() const noexcept { return static_cast<int>(endpoint_.port); }
    const ClientEndpoint& endpoint() const noexcept { return endpoint_; }
    bool helpRequested() const noexcept { return helpRequested_; }
    const std::string& role() const noexcept { return role_; }
    int id() const noexcept { return id_; }
    const std::string& library() const noexcept { return library_; }
    const std::string& userName() const noexcept { return userName_; }

    void setUserName(const std::string& name);

    void print(std::ostream& os) const;

private:
    void parse(int argc, char* argv[]);

    ClientEndpoint endpoint_{};
    bool helpRequested_{false};
    std::string role_{"Client"};
    int id_{0};
    std::string library_;
    std::string userName_{"user"};
};

#endif 