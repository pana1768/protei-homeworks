#ifndef RESOURCE_TEST_H
#define RESOURCE_TEST_H

#include "interfaces.h"
#include <vector>
#include <string>

class ResourceTest : public ITest {
public:
    explicit ResourceTest(std::vector<std::string> paths)
        : paths_(std::move(paths)) {}

    std::string name() const override;
    bool run() override;

private:
    std::vector<std::string> paths_;
};

#endif 