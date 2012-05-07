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
template<class T> class Matrix3: public Matrix<3, T> {
    public:
        /** @copydoc Matrix::from(T*) */
        inline constexpr static Matrix3<T>& from(T* data) {
            return *reinterpret_cast<Matrix3<T>*>(data);
        }

        /** @copydoc Matrix::from(const T*) */
        inline constexpr static const Matrix3<T>& from(const T* data) {
            return *reinterpret_cast<const Matrix3<T>*>(data);
        }

        /** @copydoc Matrix::from(const Vector<size, T>&, const U&...) */
        template<class ...U> inline constexpr static Matrix3<T> from(const Vector<3, T>& first, const U&... next) {
            return Matrix<3, T>::from(first, next...);
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::ZeroType): Matrix<3, T>(Matrix<3, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::IdentityType = Matrix<3, T>::Identity, T value = T(1)): Matrix<3, T>(
            value, 0.0f, 0.0f,
            0.0f, value, 0.0f,
            0.0f, 0.0f, value
        ) {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> explicit Matrix3(U) = delete;
        #endif

        /** @copydoc Matrix::Matrix(T, U...) */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix3(T first, U... next): Matrix<3, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr Matrix3(T first, U... next) {}
        #endif

        /** @copydoc Matrix::Matrix(const Matrix<size, T>&) */
        inline constexpr Matrix3(const Matrix<3, T>& other): Matrix<3, T>(other) {}

        /** @copydoc Matrix::operator=() */
        inline Matrix3<T>& operator=(const Matrix3<T>& other) {
            Matrix<3, T>::operator=(other);
            return *this;
        }

        /** @copydoc Matrix::operator[](size_t) */
        inline Vector3<T>& operator[](size_t col) { return Vector3<T>::from(Matrix<3, T>::data()+col*3); }

        /** @copydoc Matrix::operator[](size_t) const */
        inline constexpr const Vector3<T>& operator[](size_t col) const { return Vector3<T>::from(Matrix<3, T>::data()+col*3); }

        /** @copydoc Matrix::operator*(const Matrix<size, T>&) const */
        inline Matrix3<T> operator*(const Matrix<3, T>& other) const { return Matrix<3, T>::operator*(other); }

        /** @copydoc Matrix::operator*=() */
        inline Matrix3<T>& operator*=(const Matrix<3, T>& other) {
            Matrix<3, T>::operator*=(other);
            return *this;
        }

        /** @copydoc Matrix::operator*(const Vector<size, T>&) const */
        inline Vector3<T> operator*(const Vector<3, T>& other) const { return Matrix<3, T>::operator*(other); }

        /** @copydoc Matrix::transposed() */
        inline Matrix3<T> transposed() const { return Matrix<3, T>::transposed(); }

        /** @copydoc Matrix::inverted() */
        inline Matrix3<T> inverted() const { return Matrix<3, T>::inverted(); }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix3<T>& value) {
    return debug << static_cast<const Magnum::Math::Matrix<3, T>&>(value);
}
#endif

}}

#endif
