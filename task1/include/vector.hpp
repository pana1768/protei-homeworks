#ifndef INCLUDE_VECTOR_HPP_
#define INCLUDE_VECTOR_HPP_

#include <iostream>
#include <stdexcept>

enum class Types {
    INT,
    DOUBLE,
    FLOAT,
    None
};

template <typename T>
struct Vector {
    T**** arr;
    size_t x;
    size_t y;
    size_t w;
    size_t z;

    explicit Vector(const size_t x_ = 1, const size_t y_ = 1,
                    const size_t w_ = 1, const size_t z_ = 4)
        : arr(0), x(x_), y(y_), w(w_), z(z_) {
        arr = new T***[x];
        for (size_t i = 0; i < x; ++i) {
            arr[i] = new T**[y];
            for (size_t j = 0; j < y; ++j) {
                arr[i][j] = new T*[w];
                for (size_t m = 0; m < w; ++m) {
                    arr[i][j][m] = new T[z];
                    for (size_t n = 0; n < z; ++n) {
                        arr[i][j][m][n] = static_cast<T>(0);
                    }
                }
            }
        }
    }

    ~Vector() {
        if (!arr) {
            return;
        }
        for (size_t i = 0; i < x; ++i) {
            for (size_t j = 0; j < y; ++j) {
                for (size_t m = 0; m < w; ++m) {
                    delete[] arr[i][j][m];
                }
                delete[] arr[i][j];
            }
            delete[] arr[i];
        }
        delete[] arr;
    }
};

template <typename T>
void input_vector(Vector<T>& vector) {
    for (size_t i = 0; i < vector.x; ++i) {
        for (size_t j = 0; j < vector.y; ++j) {
            for (size_t m = 0; m < vector.w; ++m) {
                for (size_t n = 0; n < vector.z; ++n) {
                    if (!(std::cin >> vector.arr[i][j][m][n])) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        throw std::runtime_error("non-numeric value in numeric vector");
                    }
                }
            }
        }
    }

    if (vector.x > 0 && vector.y > 0 && vector.w > 0 && vector.z > 0) {
        if (vector.arr[0][0][0][vector.z - 1] == static_cast<T>(0)) {
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
                    std::cout << vector.arr[i][j][m][n] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

template <typename func_t>
void dispatch(const Types type, void* vectorPtr, func_t func) {
    switch (type) {
        case Types::INT:
            func(static_cast<Vector<int>*>(vectorPtr));
            break;
        case Types::DOUBLE:
            func(static_cast<Vector<double>*>(vectorPtr));
            break;
        case Types::FLOAT:
            func(static_cast<Vector<float>*>(vectorPtr));
            break;
        case Types::None:
        default:
            break;
    }
}

#endif  