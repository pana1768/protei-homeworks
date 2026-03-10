#ifndef INCLUDE_VECTOR_HPP_
#define INCLUDE_VECTOR_HPP_

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

enum class Types {
    INT,
    DOUBLE,
    FLOAT,
    None
};

template <typename T>
struct Vector {
    std::vector<T> data_;
    size_t x;
    size_t y;
    size_t w;
    size_t z;

    Vector(const Vector&) = delete;
    Vector& operator=(const Vector&) = delete;

    size_t index(size_t i, size_t j, size_t m, size_t n) const {
        return i * (y * w * z) + j * (w * z) + m * z + n;
    }

    T& at(size_t i, size_t j, size_t m, size_t n) {
        return data_[index(i, j, m, n)];
    }
    const T& at(size_t i, size_t j, size_t m, size_t n) const {
        return data_[index(i, j, m, n)];
    }

    explicit Vector(const size_t x_ = 1, const size_t y_ = 1,
                    const size_t w_ = 1, const size_t z_ = 4)
        : x(x_), y(y_), w(w_), z(z_) {
        data_.resize(x * y * w * z, static_cast<T>(0));
    }
};

template <typename T>
void input_vector(Vector<T>& vector) {
    for (size_t i = 0; i < vector.x; ++i) {
        for (size_t j = 0; j < vector.y; ++j) {
            for (size_t m = 0; m < vector.w; ++m) {
                for (size_t n = 0; n < vector.z; ++n) {
                    if (!(std::cin >> vector.at(i, j, m, n))) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        throw std::runtime_error("non-numeric value in numeric vector");
                    }
                }
            }
        }
    }
    if (vector.x > 0 && vector.y > 0 && vector.w > 0 && vector.z > 0) {
        if (vector.at(0, 0, 0, vector.z - 1) == static_cast<T>(0)) {
            throw std::runtime_error("w component of vector must be non-zero");
        }
    }
}

template <typename T>
void print_vector(const Vector<T>& vector) {
    for (size_t i = 0; i < vector.x; ++i) {
        for (size_t j = 0; j < vector.y; ++j) {
            for (size_t m = 0; m < vector.w; ++m) {
                for (size_t n = 0; n < vector.z; ++n) {
                    std::cout << vector.at(i, j, m, n) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

#endif 
