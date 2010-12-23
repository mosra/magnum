#ifndef Magnum_Matrix_h
#define Magnum_Matrix_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Matrix
 */

#include <cstring>

namespace Magnum {

/** @brief Matrix */
template<class T, size_t size> class Matrix {
    public:
        /**
         * @brief Default constructor
         * @param identity Create identity matrix instead of zero matrix.
         */
        inline Matrix(bool identity = true) {
            memset(_data, 0, size*size*sizeof(T));

            if(identity) for(size_t i = 0; i != size; ++i)
                _data[size*i+i] = static_cast<T>(1);
        }

        /**
         * @brief Constructor
         * @param data  One-dimensional array of @c size*size length in column-major order.
         */
        inline Matrix(const T* data) { setData(data); }

        /** @brief Copy constructor */
        inline Matrix(const Matrix<T, size>& other) {
            setData(other.data());
        }

        /** @brief Assignment operator */
        inline Matrix<T, size>& operator=(const Matrix<T, size>& other) {
            setData(other.data());
            return *this;
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of @c size*size length in column-major order.
         */
        inline const T* data() const { return _data; }

        /**
         * @brief Set raw data
         * @param data One-dimensional array of @c size*size length in column-major order.
         */
        inline void setData(const T* data) {
            memcpy(_data, data, size*size*sizeof(T));
        }

        /** @brief Value at given position */
        inline T at(size_t col, size_t row) const {
            return _data[col*size+row];
        }

        /** @brief Set value at given position */
        inline void set(size_t col, size_t row, T value) {
            _data[col*size+row] = value;
        }

        /** @brief Add value to given position */
        inline void add(size_t col, size_t row, T value) {
            _data[col*size+row] += value;
        }

        /** @brief Equality operator */
        inline bool operator==(const Matrix<T, size>& other) const {
            return memcmp(_data, other.data(), size*size*sizeof(T)) == 0;
        }

        /** @brief Non-equality operator */
        inline bool operator!=(const Matrix<T, size>& other) const {
            return !operator==(other);
        }

        /** @brief Multiply matrix operator */
        Matrix<T, size> operator*(const Matrix<T, size>& other) const {
            Matrix<T, size> out(false);

            for(size_t row = 0; row != size; ++row) {
                for(size_t col = 0; col != size; ++col) {
                    for(size_t pos = 0; pos != size; ++pos)
                        out.add(col, row, at(pos, row)*other.at(col, pos));
                }
            }

            return out;
        }

        /** @brief Transposed matrix */
        Matrix<T, size> transposed() const {
            Matrix<T, size> out;

            for(size_t row = 0; row != size; ++row) {
                for(size_t col = 0; col != size; ++col)
                    out.set(col, row, at(row, col));
            }

            return out;
        }

    private:
        T _data[size*size];
};

}

#endif
