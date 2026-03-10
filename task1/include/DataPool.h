#ifndef DATA_POOL_H
#define DATA_POOL_H

#include "VectorWrapper.h"
#include <deque>
#include <memory>

class DataPool {
public:
    void push(std::unique_ptr<IVectorWrapper> vec);
    bool pop(std::unique_ptr<IVectorWrapper>& out);
    bool empty() const noexcept { return storage_.empty(); }

private:
    std::deque<std::unique_ptr<IVectorWrapper>> storage_;
};

#endif