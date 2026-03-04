#ifndef CONNECTION_TEST_H
#define CONNECTION_TEST_H

#include "../include/interfaces.h"
#include <vector>
#include <string>

class ConnectionTest : public ITest {
public:
    explicit ConnectionTest(std::vector<std::string> params)
        : params_(std::move(params)) {}

    std::string name() const override;
    bool run() override;

private:
    std::vector<std::string> params_;
};

#endif
