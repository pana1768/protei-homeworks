#ifndef INTERFACES_H
#define INTERFACES_H

#include <string>

class ITest {
public:
    virtual ~ITest() = default;

    virtual std::string name() const = 0;
    virtual bool run() = 0;
};

#endif