#ifndef Magnum_Math_Matrix3_h
#define Magnum_Math_Matrix3_h
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
 * @brief Class Magnum::Math::Matrix3
 */

#include "Matrix.h"
#include "Vector3.h"

namespace Magnum { namespace Math {

/** @brief 3x3 matrix */
template<class T> class Matrix3: public Matrix<T, 3> {
    public:
        /** @copydoc Matrix::from(T*) */
        inline constexpr static Matrix3<T>& from(T* data) {
            return *reinterpret_cast<Matrix3<T>*>(data);
        }

        /** @copydoc Matrix::from(const T*) */
        inline constexpr static const Matrix3<T>& from(const T* data) {
            return *reinterpret_cast<const Matrix3<T>*>(data);
        }

        /** @copydoc Matrix::Matrix(bool) */
        inline constexpr Matrix3(bool identity = true): Matrix<T, 3>{
            /** @todo Make this in Matrix itself, after it will be constexpr */
            identity ? 1.0f : 0.0f, 0.0f, 0.0f,
            0.0f, identity ? 1.0f : 0.0f, 0.0f,
            0.0f, 0.0f, identity ? 1.0f : 0.0f
        } {}

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         *
         * Note that the values are in column-major order.
         */
        /* doxygen: @copydoc Matrix::Matrix(T, U&&...) doesn't work */
        template<class ...U> inline constexpr Matrix3(T first, U&&... next): Matrix<T, 3>(first, std::forward<U>(next)...) {}

        /** @copydoc Matrix::Matrix(const Matrix<T, size>&) */
        inline constexpr Matrix3(const Matrix<T, 3>& other): Matrix<T, 3>(other) {}

        /** @copydoc Matrix::operator=() */
        inline constexpr Matrix3<T>& operator=(const Matrix<T, 3>& other) { return Matrix<T, 3>::operator=(other); }

        /** @copydoc Matrix::at(size_t) const */
        inline constexpr Vector3<T> at(size_t col) const { return Matrix<T, 3>::at(col); }

        /** @copydoc Matrix::at(size_t, size_t) const */
        inline constexpr T at(size_t row, size_t col) const { return Matrix<T, 3>::at(row, col); }

        /** @copydoc Matrix::operator*(const Matrix<T, size>&) const */
        inline Matrix3<T> operator*(const Matrix<T, 3>& other) const { return Matrix<T, 3>::operator*(other); }

        /** @copydoc Matrix::operator*=() */
        inline Matrix3<T>& operator*=(const Matrix<T, 3>& other) {
            Matrix<T, 3>::operator*=(other);
            return *this;
        }

        /** @copydoc Matrix::operator*(const Vector<T, size>&) const */
        inline Vector3<T> operator*(const Vector<T, 3>& other) const { return Matrix<T, 3>::operator*(other); }

        /** @copydoc Matrix::transposed() */
        inline Matrix3<T> transposed() const { return Matrix<T, 3>::transposed(); }

        /** @copydoc Matrix::inversed() */
        inline Matrix3<T> inversed() const { return Matrix<T, 3>::inversed(); }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix3<T>& value) {
    return debug << static_cast<const Magnum::Math::Matrix<T, 3>&>(value);
}
#endif

}}

#endif
