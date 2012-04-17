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

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class T, size_t size> class MatrixDeterminant;

    template<size_t ...> struct Sequence {};

    /* E.g. GenerateSequence<3>::Type is Sequence<0, 1, 2> */
    template<size_t N, size_t ...sequence> struct GenerateSequence:
        GenerateSequence<N-1, N-1, sequence...> {};

    template<size_t ...sequence> struct GenerateSequence<0, sequence...> {
        typedef Sequence<sequence...> Type;
    };
}
#endif

/**
 * @brief %Matrix
 *
 * @todo @c PERFORMANCE - loop unrolling for Matrix<T, 3> and Matrix<T, 4>
 * @todo first col, then row (cache adjacency)
 */
template<class T, size_t size> class Matrix {
    public:
        /**
         * @brief %Matrix from array
         * @return Reference to the data as if it was Matrix, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        inline constexpr static Matrix<T, size>& from(T* data) {
            return *reinterpret_cast<Matrix<T, size>*>(data);
        }

        /** @copydoc from(T*) */
        inline constexpr static const Matrix<T, size>& from(const T* data) {
            return *reinterpret_cast<const Matrix<T, size>*>(data);
        }

        /**
         * @brief %Matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         */
        template<class ...U> inline constexpr static Matrix<T, size> from(const Vector<T, size>& first, const U&... next) {
            return from(typename Implementation::GenerateSequence<size>::Type(), first, next...);
        }

        /** @brief Pass to constructor to create zero-filled matrix */
        enum ZeroType { Zero };

        /**
         * @brief Zero-filled matrix constructor
         *
         * Use this constructor by calling `Matrix m(Matrix::Zero);`.
         */
        inline constexpr explicit Matrix(ZeroType): _data() {}

        /** @brief Pass to constructor to create identity matrix */
        enum IdentityType { Identity };

        /**
         * @brief Default constructor
         *
         * You can also explicitly call this constructor with
         * `Matrix m(Matrix::Identity);`.
         */
        inline explicit Matrix(IdentityType = Identity): _data() {
            /** @todo constexpr how? */
            for(size_t i = 0; i != size; ++i)
                _data[size*i+i] = static_cast<T>(1);
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> explicit Matrix(U) = delete;
        #endif

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         *
         * Note that the values are in column-major order.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix(T first, U... next): _data{first, next...} {}
        #else
        template<class ...U> inline constexpr Matrix(T first, U... next);
        #endif

        /** @brief Copy constructor */
        inline constexpr Matrix(const Matrix<T, size>& other) = default;

        /** @brief Assignment operator */
        inline Matrix<T, size>& operator=(const Matrix<T, size>& other) = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of `size*size` length in column-major
         *      order.
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @copydoc data() */

        /** @brief %Matrix column */
        inline Vector<T, size>& operator[](size_t col) {
            return Vector<T, size>::from(_data+col*size);
        }

        /** @copydoc operator[]() */
        inline constexpr const Vector<T, size>& operator[](size_t col) const {
            return Vector<T, size>::from(_data+col*size);
        }

        /** @brief Equality operator */
        inline bool operator==(const Matrix<T, size>& other) const {
            for(size_t row = 0; row != size; ++row)
                for(size_t col = 0; col != size; ++col)
                    if(!TypeTraits<T>::equals((*this)[col][row], other[col][row])) return false;

            return true;
        }

        /** @brief Non-equality operator */
        inline constexpr bool operator!=(const Matrix<T, size>& other) const {
            return !operator==(other);
        }

        /** @brief Multiply matrix operator */
        Matrix<T, size> operator*(const Matrix<T, size>& other) const {
            Matrix<T, size> out(Zero);

            for(size_t row = 0; row != size; ++row)
                for(size_t col = 0; col != size; ++col)
                    for(size_t pos = 0; pos != size; ++pos)
                        out[col][row] += (*this)[pos][row]*other[col][pos];

            return out;
        }

        /** @brief Multiply and assign matrix operator */
        inline Matrix<T, size>& operator*=(const Matrix<T, size>& other) {
            return (*this = *this*other);
        }

        /** @brief Multiply vector operator */
        Vector<T, size> operator*(const Vector<T, size>& other) const {
            Vector<T, size> out;

            for(size_t row = 0; row != size; ++row)
                for(size_t pos = 0; pos != size; ++pos)
                    out[row] += (*this)[pos][row]*other[pos];

            return out;
        }

        /** @brief Transposed matrix */
        Matrix<T, size> transposed() const {
            Matrix<T, size> out(Zero);

            for(size_t row = 0; row != size; ++row)
                for(size_t col = 0; col != size; ++col)
                    out[row][col] = (*this)[col][row];

            return out;
        }

        /** @brief %Matrix without given column and row */
        Matrix<T, size-1> ij(size_t skipCol, size_t skipRow) const {
            Matrix<T, size-1> out(Matrix<T, size-1>::Zero);

            for(size_t row = 0; row != size-1; ++row)
                for(size_t col = 0; col != size-1; ++col)
                    out[col][row] = (*this)[col + (col >= skipCol)]
                                           [row + (row >= skipRow)];

            return out;
        }

        /**
         * @brief Determinant
         *
         * Computed recursively using Laplace's formula expanded down to 2x2
         * matrix, where the determinant is computed directly. Complexity is
         * O(n!), the same as when computing the determinant directly.
         */
        inline T determinant() const { return Implementation::MatrixDeterminant<T, size>()(*this); }

        /**
         * @brief Inverse matrix
         */
        Matrix<T, size> inversed() const {
            Matrix<T, size> out(Zero);

            T _determinant = determinant();

            for(size_t row = 0; row != size; ++row)
                for(size_t col = 0; col != size; ++col)
                    out[col][row] = (((row+col) & 1) ? -1 : 1)*ij(row, col).determinant()/_determinant;

            return out;
        }

    private:
        template<size_t ...sequence, class ...U> inline constexpr static Matrix<T, size> from(Implementation::Sequence<sequence...> s, const Vector<T, size>& first, U... next) {
            return from(s, next..., first[sequence]...);
        }

        template<size_t ...sequence, class ...U> inline constexpr static Matrix<T, size> from(Implementation::Sequence<sequence...> s, T first, U... next) {
            return Matrix<T, size>(first, next...);
        }

        T _data[size*size];
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class T, size_t size> class MatrixDeterminant {
    public:
        /** @brief Functor */
        T operator()(const Matrix<T, size>& m) {
            T out(0);

            for(size_t col = 0; col != size; ++col)
                out += ((col & 1) ? -1 : 1)*m[col][0]*m.ij(col, 0).determinant();

            return out;
        }
};

template<class T> class MatrixDeterminant<T, 2> {
    public:
        /** @brief Functor */
        inline constexpr T operator()(const Matrix<T, 2>& m) {
            return m[0][0]*m[1][1] - m[1][0]*m[0][1];
        }
};

template<class T> class MatrixDeterminant<T, 1> {
    public:
        /** @brief Functor */
        inline constexpr T operator()(const Matrix<T, 1>& m) {
            return m[0][0];
        }
};

}

template<class T, size_t size> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix<T, size>& value) {
    debug << "Matrix(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(size_t row = 0; row != size; ++row) {
        if(row != 0) debug << ",\n       ";
        for(size_t col = 0; col != size; ++col) {
            if(col != 0) debug << ", ";
            debug << value[col][row];
        }
    }
    debug << ')';
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}
#endif

}}

#endif
