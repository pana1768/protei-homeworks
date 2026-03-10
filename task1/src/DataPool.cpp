#include "../include/DataPool.h"

void DataPool::push(std::unique_ptr<IVectorWrapper> vec) {
    if (vec) {
        storage_.push_back(std::move(vec));
    }
}

bool DataPool::pop(std::unique_ptr<IVectorWrapper>& out) {
    if (storage_.empty()) return false;
    out = std::move(storage_.front());
    storage_.pop_front();
    return true;
}