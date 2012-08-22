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

#include "RectangularMatrix.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<size_t size, class T> class MatrixDeterminant;
}
#endif

/**
@brief Square matrix
@tparam s   %Matrix size

@configurationvalueref{Magnum::Math::Matrix}
@todo @c PERFORMANCE - loop unrolling for Matrix<3, T> and Matrix<4, T>
@todo first col, then row (cache adjacency)
 */
template<size_t s, class T> class Matrix: public RectangularMatrix<s, s, T> {
    public:
        const static size_t size = s;    /**< @brief %Matrix size */

        /** @brief Pass to constructor to create zero-filled matrix */
        enum ZeroType { Zero };

        /**
         * @brief Zero-filled matrix constructor
         *
         * Use this constructor by calling `Matrix m(Matrix::Zero);`.
         */
        inline constexpr explicit Matrix(ZeroType) {}

        /** @brief Pass to constructor to create identity matrix */
        enum IdentityType { Identity };

        /**
         * @brief Default constructor
         *
         * You can also explicitly call this constructor with
         * `Matrix m(Matrix::Identity);`. Optional parameter @p value allows
         * you to specify value on diagonal.
         */
        inline explicit Matrix(IdentityType = Identity, T value = T(1)) {
            for(size_t i = 0; i != size; ++i)
                (*this)(i, i) = value;
        }

        /** @copydoc RectangularMatrix::RectangularMatrix(T, U...) */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix(T first, U... next): RectangularMatrix<size, size, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr Matrix(T first, U... next);
        #endif

        /** @brief Copy constructor */
        inline constexpr Matrix(const RectangularMatrix<size, size, T>& other): RectangularMatrix<size, size, T>(other) {}

        /** @brief Multiply and assign matrix operator */
        inline Matrix<size, T>& operator*=(const RectangularMatrix<size, size, T>& other) {
            return (*this = *this*other);
        }

        /** @brief %Matrix without given column and row */
        Matrix<size-1, T> ij(size_t skipCol, size_t skipRow) const {
            Matrix<size-1, T> out(Matrix<size-1, T>::Zero);

            for(size_t row = 0; row != size-1; ++row)
                for(size_t col = 0; col != size-1; ++col)
                    out(col, row) = (*this)(col + (col >= skipCol),
                                            row + (row >= skipRow));

            return out;
        }

        /**
         * @brief Determinant
         *
         * Computed recursively using Laplace's formula:
         * @f[
         * \det(A) = \sum_{j=1}^n (-1)^{i+j} a_{i,j} \det(A^{i,j})
         * @f]
         * @f$ A^{i, j} @f$ is matrix without i-th row and j-th column, see
         * ij(). The formula is expanded down to 2x2 matrix, where the
         * determinant is computed directly:
         * @f[
         * \det(A) = a_{0, 0} a_{1, 1} - a_{1, 0} a_{0, 1}
         * @f]
         */
        inline T determinant() const { return Implementation::MatrixDeterminant<size, T>()(*this); }

        /**
         * @brief Inverted matrix
         *
         * Computed using Cramer's rule:
         * @f[
         * A^{-1} = \frac{1}{\det(A)} Adj(A)
         * @f]
         */
        Matrix<size, T> inverted() const {
            Matrix<size, T> out(Zero);

            T _determinant = determinant();

            for(size_t row = 0; row != size; ++row)
                for(size_t col = 0; col != size; ++col)
                    out(col, row) = (((row+col) & 1) ? -1 : 1)*ij(row, col).determinant()/_determinant;

            return out;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Reimplementation of functions to return correct type */
        inline Matrix<size, T> operator*(const Matrix<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        template<size_t otherCols> inline RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        inline Vector<size, T> operator*(const Vector<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        #endif
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(size, size, Matrix<size, T>)
};

/** @debugoperator{Magnum::Math::Matrix} */
template<size_t size, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix<size, T>& value) {
    return debug << static_cast<const Magnum::Math::RectangularMatrix<size, size, T>&>(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Type, VectorType, size)       \
    inline constexpr static Type<T>& from(T* data) {                        \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    inline constexpr static const Type<T>& from(const T* data) {            \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
    template<class ...U> inline constexpr static Type<T> from(const Vector<size, T>& first, const U&... next) { \
        return Matrix<size, T>::from(first, next...);                       \
    }                                                                       \
                                                                            \
    inline Type<T>& operator=(const Type<T>& other) {                       \
        Matrix<size, T>::operator=(other);                                  \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline VectorType<T>& operator[](size_t col) {                          \
        return VectorType<T>::from(Matrix<size, T>::data()+col*size);       \
    }                                                                       \
    inline constexpr const VectorType<T>& operator[](size_t col) const {    \
        return VectorType<T>::from(Matrix<size, T>::data()+col*size);       \
    }                                                                       \
                                                                            \
    inline Type<T> operator*(const Matrix<size, T>& other) const {          \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    inline Type<T>& operator*=(const Matrix<size, T>& other) {              \
        Matrix<size, T>::operator*=(other);                                 \
        return *this;                                                       \
    }                                                                       \
    template<size_t otherCols> inline RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const {                                                                 \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    inline VectorType<T> operator*(const Vector<size, T>& other) const {    \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
                                                                            \
    inline Type<T> transposed() const { return Matrix<size, T>::transposed(); } \
    inline Type<T> inverted() const { return Matrix<size, T>::inverted(); }

namespace Implementation {

template<size_t size, class T> class MatrixDeterminant {
    public:
        /** @brief Functor */
        T operator()(const Matrix<size, T>& m) {
            T out(0);

            for(size_t col = 0; col != size; ++col)
                out += ((col & 1) ? -1 : 1)*m(col, 0)*m.ij(col, 0).determinant();

            return out;
        }
};

template<class T> class MatrixDeterminant<2, T> {
    public:
        /** @brief Functor */
        inline constexpr T operator()(const Matrix<2, T>& m) {
            return m(0, 0)*m(1, 1) - m(1, 0)*m(0, 1);
        }
};

template<class T> class MatrixDeterminant<1, T> {
    public:
        /** @brief Functor */
        inline constexpr T operator()(const Matrix<1, T>& m) {
            return m(0, 0);
        }
};

}
#endif

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix} */
    template<size_t size, class T> struct ConfigurationValue<Magnum::Math::Matrix<size, T>>: public ConfigurationValue<Magnum::Math::RectangularMatrix<size, size, T>> {};
}}

#endif
