#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "AppSettings.h"
#include "DataPool.h"
#include "VectorWrapper.h"
#include "interfaces.h"

#include <memory>
#include <string>
#include <vector>

struct AppContext {
    AppSettings settings;
    std::unique_ptr<IVectorWrapper> currentVector;
    DataPool dataPool;
    std::vector<std::unique_ptr<ITest>> tests;
    bool shouldExit = false;

    explicit AppContext(AppSettings s);

    std::unique_ptr<IVectorWrapper> createVectorByType(const std::string& typeName) const;
};

void mainLoop(AppContext& ctx);

#endif 