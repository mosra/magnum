#ifndef Magnum_Math_Matrix_h
#define Magnum_Math_Matrix_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::Matrix
 */

#include "Vector.h"

namespace Magnum { namespace Math {

/**
 * @brief %Matrix
 *
 * @todo @c PERFORMANCE - loop unrolling for Matrix<T, 3> and Matrix<T, 4>
 */
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
            if(&other != this) setData(other.data());
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
         *
         * @bug Creating Matrix<int, 5> from float* ??
         */
        inline void setData(const T* data) {
            memcpy(_data, data, size*size*sizeof(T));
        }

        /** @brief Value at given position */
        inline T at(size_t row, size_t col) const {
            return _data[col*size+row];
        }

        /** @brief %Matrix column */
        inline Vector<T, size> at(size_t col) const {
            return _data+col*size;
        }

        /** @brief Set value at given position */
        inline void set(size_t row, size_t col, T value) {
            _data[col*size+row] = value;
        }

        /** @brief Set matrix column */
        inline void set(size_t col, const Vector<T, size>& value) {
            memcpy(_data+col*size, value.data(), size*sizeof(T));
        }

        /** @brief Add value to given position */
        inline void add(size_t row, size_t col, T value) {
            _data[col*size+row] += value;
        }

        /** @brief Equality operator */
        inline bool operator==(const Matrix<T, size>& other) const {
            for(size_t row = 0; row != size; ++row) {
                for(size_t col = 0; col != size; ++col)
                    if(!TypeTraits<T>::equals(at(row, col), other.at(row, col))) return false;
            }

            return true;
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
                        out.add(row, col, at(row, pos)*other.at(pos, col));
                }
            }

            return out;
        }

        /** @brief Multiply vector operator */
        Vector<T, size> operator*(const Vector<T, size>& other) const {
            Vector<T, size> out;

            for(size_t row = 0; row != size; ++row) {
                for(size_t pos = 0; pos != size; ++pos)
                    out.add(row, at(row, pos)*other.at(pos));
            }

            return out;
        }

        /** @brief Transposed matrix */
        Matrix<T, size> transposed() const {
            Matrix<T, size> out(false);

            for(size_t row = 0; row != size; ++row) {
                for(size_t col = 0; col != size; ++col)
                    out.set(col, row, at(row, col));
            }

            return out;
        }

        /** @brief %Matrix without given row and column */
        Matrix<T, size-1> ij(size_t skipRow, size_t skipCol) const {
            Matrix<T, size-1> out(false);

            for(size_t row = 0; row != size-1; ++row) {
                for(size_t col = 0; col != size-1; ++col)
                    out.set(row, col, at(row + (row >= skipRow),
                                         col + (col >= skipCol)));
            }

            return out;
        }

        /**
         * @brief Determinant
         *
         * Computed recursively using Laplace's formula expanded down to 2x2
         * matrix, where the determinant is computed directly. Complexity is
         * O(n!), the same as when computing the determinant directly.
         */
        T determinant() const {
            T out(0);

            for(size_t col = 0; col != size; ++col)
                out += ((col & 1) ? -1 : 1)*at(0, col)*ij(0, col).determinant();

            return out;
        }

        /**
         * @brief Inverse matrix
         */
        Matrix<T, size> inverse() const {
            Matrix<T, size> out(false);

            T _determinant = determinant();

            for(size_t row = 0; row != size; ++row) {
                for(size_t col = 0; col != size; ++col)
                    out.set(row, col, (((row+col) & 1) ? -1 : 1)*ij(col, row).determinant()/_determinant);
            }

            return out;
        }

    private:
        T _data[size*size];
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Barebone template specialization for 2x2 matrix (only for determinant computation) */
template<class T> class Matrix<T, 2> {
    public:
        inline Matrix(bool identity = true) {
            memset(_data, 0, 4*sizeof(T));
            if(identity) {
                set(0, 0, T(1));
                set(1, 1, T(1));
            }
        }
        inline Matrix<T, 2>& operator=(const Matrix<T, 2>& other) {
            if(&other != this) setData(other.data());
            return *this;
        }
        inline void setData(const T* data) { memcpy(_data, data, 4*sizeof(T)); }
        inline T at(size_t row, size_t col) const { return _data[col*2+row]; }
        inline void set(size_t row, size_t col, T value) { _data[col*2+row] = value; }

        T determinant() const {
            return at(0, 0)*at(1, 1) - at(0, 1)*at(1, 0);
        }

    private:
        T _data[4];
};
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, size_t size> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix<T, size>& value) {
    debug << "Matrix(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(size_t row = 0; row != size; ++row) {
        if(row != 0) debug << ",\n       ";
        for(size_t col = 0; col != size; ++col) {
            if(col != 0) debug << ", ";
            debug << value.at(row, col);
        }
    }
    debug << ')';
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}
#endif

}}

#endif
