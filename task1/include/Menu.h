#ifndef MENU_H
#define MENU_H

#include "AppSettings.h"
#include "VectorWrapper.h"
#include "DataPool.h"
#include "interfaces.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct AppContext;

class MenuItem {
public:
    explicit MenuItem(const std::string& name) : name_(name) {}
    virtual ~MenuItem() = default;

    const std::string& name() const noexcept { return name_; }
    virtual void execute(AppContext& ctx, const std::vector<std::string>& args) = 0;

private:
    std::string name_;
};

class Menu {
public:
    explicit Menu(AppContext& ctx);
    void registerItem(std::unique_ptr<MenuItem> item);
    void handleLine(const std::string& line);

private:
    AppContext& ctx_;
    std::unordered_map<std::string, std::unique_ptr<MenuItem>> items_;
};

#endif  
