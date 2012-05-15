#ifndef Magnum_Math_Matrix4_h
#define Magnum_Math_Matrix4_h
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
 * @brief Class Magnum::Math::Matrix4
 */

#include "Matrix3.h"
#include "Vector4.h"

namespace Magnum { namespace Math {

/**
@brief 4x4 matrix

@todo Shearing
@todo Reflection
 */
template<class T> class Matrix4: public Matrix<4, T> {
    public:
        /**
         * @brief Translation matrix
         * @param vec   Translation vector
         */
        static constexpr Matrix4<T> translation(const Vector3<T>& vec) {
            return Matrix4( /* Column-major! */
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                vec.x(), vec.y(), vec.z(), 1.0f
            );
        }

        /**
         * @brief Scaling matrix
         * @param vec   Scaling vector
         */
        static constexpr Matrix4 scaling(const Vector3<T>& vec) {
            return Matrix4( /* Column-major! */
                vec.x(), 0.0f, 0.0f, 0.0f,
                0.0f, vec.y(), 0.0f, 0.0f,
                0.0f, 0.0f, vec.z(), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }

        /**
         * @brief Rotation matrix
         * @param angle Rotation angle (counterclockwise, in radians)
         * @param vec   Rotation vector
         */
        static Matrix4 rotation(T angle, const Vector3<T>& vec) {
            Vector3<T> vn = vec.normalized();

            T sine = sin(angle);
            T cosine = cos(angle);
            T oneMinusCosine = T(1) - cosine;

            T xx = vn.x()*vn.x();
            T xy = vn.x()*vn.y();
            T xz = vn.x()*vn.z();
            T yy = vn.y()*vn.y();
            T yz = vn.y()*vn.z();
            T zz = vn.z()*vn.z();

            return Matrix4( /* Column-major! */
                cosine + xx*oneMinusCosine,
                    xy*oneMinusCosine + vn.z()*sine,
                        xz*oneMinusCosine - vn.y()*sine,
                           T(0),
                xy*oneMinusCosine - vn.z()*sine,
                    cosine + yy*oneMinusCosine,
                        yz*oneMinusCosine + vn.x()*sine,
                           T(0),
                xz*oneMinusCosine + vn.y()*sine,
                    yz*oneMinusCosine - vn.x()*sine,
                        cosine + zz*oneMinusCosine,
                           T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix4(typename Matrix<4, T>::ZeroType): Matrix<4, T>(Matrix<4, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        inline constexpr explicit Matrix4(typename Matrix<4, T>::IdentityType = Matrix<4, T>::Identity, T value = T(1)): Matrix<4, T>(
            value, 0.0f, 0.0f, 0.0f,
            0.0f, value, 0.0f, 0.0f,
            0.0f, 0.0f, value, 0.0f,
            0.0f, 0.0f, 0.0f, value
        ) {}

        /** @copydoc Matrix::Matrix(T, U...) */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix4(T first, U... next): Matrix<4, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr Matrix4(T first, U... next) {}
        #endif

        /** @copydoc Matrix::Matrix(const Matrix<size, T>&) */
        inline constexpr Matrix4(const Matrix<4, T>& other): Matrix<4, T>(other) {}

        /** @copydoc Matrix::ij() */
        inline Matrix3<T> ij(size_t skipRow, size_t skipCol) const { return Matrix<4, T>::ij(skipRow, skipCol); }

        /** @brief Rotation and scaling part of the matrix */
        inline Matrix3<T> rotationScaling() const {
            return Matrix3<T>::from(
                (*this)[0].xyz(),
                (*this)[1].xyz(),
                (*this)[2].xyz());
        }

        /** @brief Rotation part of the matrix */
        inline Matrix3<T> rotation() const {
            return Matrix3<T>::from(
                (*this)[0].xyz().normalized(),
                (*this)[1].xyz().normalized(),
                (*this)[2].xyz().normalized());
        }

        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix4, Vector4, 4)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix4<T>& value) {
    return debug << static_cast<const Magnum::Math::Matrix<4, T>&>(value);
}
#endif

}}

#endif
