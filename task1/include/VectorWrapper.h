#ifndef VECTOR_WRAPPER_H
#define VECTOR_WRAPPER_H

#include "vector.hpp"
#include <memory>
#include <istream>
#include <ostream>
#include <type_traits>
#include <sstream>
#include <string>

class IVectorWrapper {
public:
    virtual ~IVectorWrapper() = default;

    virtual Types type() const noexcept = 0;
    virtual void input(std::istream& in) = 0;
    virtual void print(std::ostream& out) const = 0;
    virtual bool inputFromString(const std::string& str) = 0;
    virtual std::unique_ptr<IVectorWrapper> clone() const = 0;
};

template <typename T>
class TypedVectorWrapper : public IVectorWrapper {
public:
    TypedVectorWrapper() = default;

    Types type() const noexcept override;
    void input(std::istream& in) override;
    void print(std::ostream& out) const override;
    bool inputFromString(const std::string& str) override;
    std::unique_ptr<IVectorWrapper> clone() const override;

private:
    Vector<T> vector_;
};

template <typename T>
Types TypedVectorWrapper<T>::type() const noexcept {
    if (std::is_same<T, int>::value) return Types::INT;
    if (std::is_same<T, double>::value) return Types::DOUBLE;
    if (std::is_same<T, float>::value) return Types::FLOAT;
    return Types::None;
}

template <>
inline Types TypedVectorWrapper<int>::type() const noexcept { return Types::INT; }

template <>
inline Types TypedVectorWrapper<double>::type() const noexcept { return Types::DOUBLE; }

template <>
inline Types TypedVectorWrapper<float>::type() const noexcept { return Types::FLOAT; }

template <typename T>
bool TypedVectorWrapper<T>::inputFromString(const std::string& str) {
    try {
        std::istringstream iss(str);
        input(iss);
        return iss.eof();
    } catch (...) {
        return false;
    }
}

template <typename T>
void TypedVectorWrapper<T>::input(std::istream& in) {
    for (size_t i = 0; i < vector_.x; ++i) {
        for (size_t j = 0; j < vector_.y; ++j) {
            for (size_t m = 0; m < vector_.w; ++m) {
                for (size_t n = 0; n < vector_.z; ++n) {
                    if (!(in >> vector_.arr[i][j][m][n])) {
                        throw std::runtime_error("non-numeric value in numeric vector");
                    }
                }
            }
        }
    }
    if (vector_.x > 0 && vector_.y > 0 && vector_.w > 0 && vector_.z > 0) {
        if (vector_.arr[0][0][0][vector_.z - 1] == static_cast<T>(0)) {
            throw std::runtime_error("w component of vector must be non-zero");
        }
    }
}

template <typename T>
void TypedVectorWrapper<T>::print(std::ostream& out) const {
    for (size_t i = 0; i < vector_.x; ++i) {
        for (size_t j = 0; j < vector_.y; ++j) {
            for (size_t m = 0; m < vector_.w; ++m) {
                for (size_t n = 0; n < vector_.z; ++n) {
                    out << vector_.arr[i][j][m][n] << " ";
                }
                out << "\n";
            }
            out << "\n";
        }
    }
}

template <typename T>
std::unique_ptr<IVectorWrapper> TypedVectorWrapper<T>::clone() const {
    return std::make_unique<TypedVectorWrapper<T>>();
}

#endif 
