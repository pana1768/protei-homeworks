#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <iostream>
#include <string>

class AppSettings {
public:
    AppSettings(int argc, char* argv[]);

    const std::string& address() const noexcept { return address_; }
    int port() const noexcept { return port_; }
    const std::string& role() const noexcept { return role_; }
    int id() const noexcept { return id_; }
    const std::string& library() const noexcept { return library_; }
    const std::string& userName() const noexcept { return userName_; }

    void setUserName(const std::string& name);

    void print(std::ostream& os) const;

private:
    void parse(int argc, char* argv[]);

    std::string address_{"127.0.0.1"};
    int port_{0};
    std::string role_{"Client"};
    int id_{0};
    std::string library_;
    std::string userName_{"user"};
};

#endif 

