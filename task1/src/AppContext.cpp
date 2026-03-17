#include "../include/AppContext.h"
#include "../include/ConnectionTest.h"
#include "../include/ResourceTest.h"
#include "../include/Menu.h"
#include "../include/utils.hpp"
#include "../include/NetworkAddress.h"


#include <cctype>
#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

class AliasMenuItem : public MenuItem {
public:
    AliasMenuItem() : MenuItem("alias") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class TypeMenuItem : public MenuItem {
public:
    TypeMenuItem() : MenuItem("type") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class VectorMenuItem : public MenuItem {
public:
    VectorMenuItem() : MenuItem("vector") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class PrintMenuItem : public MenuItem {
public:
    PrintMenuItem() : MenuItem("print") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class ExitMenuItem : public MenuItem {
public:
    ExitMenuItem() : MenuItem("exit") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class HelpMenuItem : public MenuItem {
public:
    HelpMenuItem() : MenuItem("help") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class AddressMenuItem : public MenuItem {
public:
    AddressMenuItem() : MenuItem("address") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class PushPoolMenuItem : public MenuItem {
public:
    PushPoolMenuItem() : MenuItem("push") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class QueueMenuItem : public MenuItem {
public:
    QueueMenuItem() : MenuItem("queue") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

class SendMenuItem : public MenuItem {
public:
    SendMenuItem() : MenuItem("send") {}
    void execute(AppContext& ctx, const std::vector<std::string>& args) override;
};

namespace {

std::string toLower(const std::string& s) {
    std::string r = s;
    for (char& c : r) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return r;
}

std::string stripNonPrintable(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (unsigned char c : s) {
        if (std::isprint(c) || std::isspace(c)) {
            r += static_cast<char>(c);
        }
    }
    return r;
}

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) result.push_back(token);
    return result;
}

} 

namespace {

using VectorFactory = std::function<std::unique_ptr<IVectorWrapper>()>;

std::unordered_map<std::string, VectorFactory>& getVectorRegistry() {
    static std::unordered_map<std::string, VectorFactory> reg;
    if (reg.empty()) {
        reg["int"]    = [] { return std::make_unique<TypedVectorWrapper<int>>(); };
        reg["double"] = [] { return std::make_unique<TypedVectorWrapper<double>>(); };
        reg["float"]  = [] { return std::make_unique<TypedVectorWrapper<float>>(); };
    }
    return reg;
}

} 

AppContext::AppContext(AppSettings s)
    : settings(std::move(s)),
      currentVector(nullptr),
      dataPool(),
      tests(),
      shouldExit(false) {
    tests.push_back(std::make_unique<ConnectionTest>(
        std::vector<std::string>{settings.address()}));
    tests.push_back(std::make_unique<ResourceTest>(
        std::vector<std::string>{"config.txt"}));
}

std::unique_ptr<IVectorWrapper> AppContext::createVectorByType(
    const std::string& typeName) const {
    auto& reg = getVectorRegistry();
    auto it = reg.find(toLower(typeName));
    return (it != reg.end()) ? it->second() : nullptr;
}

void AliasMenuItem::execute(AppContext& ctx,
                            const std::vector<std::string>& args) {
    if (args.empty()) {
        LOG_WARNING("alias: missing name");
        return;
    }
    ctx.settings.setUserName(args[0]);
    std::cout << "Alias set to: " << ctx.settings.userName() << "\n";
}

void TypeMenuItem::execute(AppContext& ctx,
                           const std::vector<std::string>& args) {
    if (args.empty()) {
        LOG_WARNING("type: missing type name");
        return;
    }
    auto vec = ctx.createVectorByType(args[0]);
    if (!vec) {
        LOG_WARNING("unsupported type requested");
        return;
    }
    ctx.currentVector = std::move(vec);
    std::cout << "Vector type set to: " << args[0] << "\n";
}

void VectorMenuItem::execute(AppContext& ctx,
                             const std::vector<std::string>& args) {
    if (!ctx.currentVector) {
        LOG_WARNING("vector: type not set");
        return;
    }
    try {
        if (!args.empty()) {
            std::string line;
            for (size_t i = 0; i < args.size(); ++i) {
                if (i > 0) line += " ";
                line += args[i];
            }
            if (!ctx.currentVector->inputFromString(line)) {
                LOG_WARNING("vector: failed to parse from string");
                return;
            }
        } else {
            ctx.currentVector->input(std::cin);
        }
    } catch (const std::exception& ex) {
        LOG_ERROR(ex.what());
        std::cout << "vector input error: " << ex.what() << "\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

void PrintMenuItem::execute(AppContext& ctx,
                            const std::vector<std::string>&) {
    if (!ctx.currentVector) {
        LOG_WARNING("print: no vector");
        return;
    }
    ctx.currentVector->print(std::cout);
}

void ExitMenuItem::execute(AppContext& ctx,
                           const std::vector<std::string>&) {
    ctx.shouldExit = true;
    std::cout << "Exit\n";
}

void HelpMenuItem::execute(AppContext&,
                           const std::vector<std::string>&) {
    std::cout <<
        "Commands:\n"
        "  alias <name>        - set user alias\n"
        "  type <int|double|float> - set vector type\n"
        "  vector [n1 n2 ...]  - input 4D vector (stdin or from args)\n"
        "  print               - print current vector\n"
        "  push                - push current vector to DataPool (FIFO)\n"
        "  address             - show network address and port\n"
        "  queue               - add current 4D vector to send queue\n"
        "  send [host] [port]  - send queued vectors to server\n"
        "  exit, quit          - exit\n";
}

void AddressMenuItem::execute(AppContext& ctx,
                              const std::vector<std::string>&) {
    std::string addrStr = ctx.settings.address();
    if (ctx.settings.port() != 0) {
        addrStr += ":" + std::to_string(ctx.settings.port());
    }
    NetworkAddress na(addrStr);
    na.print(std::cout);
}

void PushPoolMenuItem::execute(AppContext& ctx,
                               const std::vector<std::string>&) {
    if (!ctx.currentVector) {
        LOG_WARNING("push: no vector");
        return;
    }
    ctx.dataPool.push(ctx.currentVector->clone());
    std::cout << "Vector pushed to DataPool\n";
}


void QueueMenuItem::execute(AppContext& ctx,
                            const std::vector<std::string>&) {
    if (!ctx.currentVector) {
        LOG_WARNING("queue: no vector");
        return;
    }
    std::array<double, 4> v{};
    if (!ctx.currentVector->tryGetVec4(v)) {
        LOG_WARNING("queue: current vector is not 4D");
        return;
    }
    ctx.queuedVectors.push_back(v);
    LOG_INFO("queued vector");
    std::cout << "Queued vectors: " << ctx.queuedVectors.size() << "\n";
}

void SendMenuItem::execute(AppContext& ctx,
                           const std::vector<std::string>& args) {
    if (ctx.queuedVectors.empty()) {
        LOG_WARNING("send: queue is empty");
        return;
    }

    std::string host = ctx.settings.address();
    std::uint16_t port = static_cast<std::uint16_t>(ctx.settings.port() ? ctx.settings.port() : 8080);
    if (args.size() >= 1) host = args[0];
    if (args.size() >= 2) {
        try {
            const int p = std::stoi(args[1]);
            if (p > 0 && p < 65536) port = static_cast<std::uint16_t>(p);
        } catch (...) {
            LOG_WARNING("send: invalid port");
            return;
        }
    }

    LOG_INFO(("connecting to " + host + ":" + std::to_string(port)).c_str());
    const int fd = tcpjson::connectTo(host, port);
    if (fd < 0) {
        LOG_ERROR("send: connect failed");
        return;
    }
    LOG_INFO("connected");

    const std::string req = tcpjson::encodeVectors(ctx.queuedVectors);
    if (!tcpjson::sendFrame(fd, req)) {
        LOG_ERROR("send: failed to send");
        tcpjson::closeFd(fd);
        return;
    }
    LOG_INFO("request sent");

    std::string resp;
    if (!tcpjson::recvFrame(fd, resp)) {
        LOG_ERROR("send: failed to receive");
        tcpjson::closeFd(fd);
        return;
    }
    tcpjson::closeFd(fd);
    LOG_INFO("response received");

    std::vector<tcpjson::Vec4> out;
    if (!tcpjson::decodeVectors(resp, out)) {
        LOG_WARNING("send: response is not vectors json");
        std::cout << resp << "\n";
        return;
    }

    ctx.queuedVectors = out;
    std::cout << "Server returned " << ctx.queuedVectors.size() << " vectors:\n";
    for (size_t i = 0; i < ctx.queuedVectors.size(); ++i) {
        const auto& v = ctx.queuedVectors[i];
        std::cout << "v" << (i + 1) << ": " << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << "\n";
    }
    LOG_INFO("queue updated from server response");
}


void Menu::handleLine(const std::string& line) {
    std::string sanitized = trim(stripNonPrintable(line));
    auto tokens = split(sanitized);
    if (tokens.empty()) return;

    const std::string cmd = toLower(tokens[0]);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    auto it = items_.find(cmd);
    if (it == items_.end()) {
        LOG_WARNING("unknown command: " + cmd);
        return;
    }
    it->second->execute(ctx_, args);
}

void Menu::registerItem(std::unique_ptr<MenuItem> item) {
    const std::string key = toLower(item->name());
    items_[key] = std::move(item);
}

Menu::Menu(AppContext& ctx) : ctx_(ctx) {
    registerItem(std::make_unique<HelpMenuItem>());
    registerItem(std::make_unique<AliasMenuItem>());
    registerItem(std::make_unique<TypeMenuItem>());
    registerItem(std::make_unique<VectorMenuItem>());
    registerItem(std::make_unique<PrintMenuItem>());
    registerItem(std::make_unique<PushPoolMenuItem>());
    registerItem(std::make_unique<QueueMenuItem>());
    registerItem(std::make_unique<SendMenuItem>());
    registerItem(std::make_unique<AddressMenuItem>());
    auto exitItem = std::make_unique<ExitMenuItem>();
    registerItem(std::move(exitItem));
    items_["quit"] = std::make_unique<ExitMenuItem>();
}

void mainLoop(AppContext& ctx) {
    Menu menu(ctx);
    std::cout << "Type 'help' for commands.\n";

    std::string line;
    while (!ctx.shouldExit) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        menu.handleLine(line);
    }
}
