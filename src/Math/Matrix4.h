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
 * @brief 4x4 matrix
 *
 * @todo Rotation with Euler angles
 * @todo Shearing
 * @todo Reflection
 */
template<class T> class Matrix4: public Matrix<T, 4> {
    public:
        /**
         * @brief Translation matrix
         * @param vec   Translation vector
         */
        static Matrix4<T> translation(const Vector3<T>& vec) {
            Matrix4 out; /* (Identity matrix) */
            out.set(0, 3, vec.x());
            out.set(1, 3, vec.y());
            out.set(2, 3, vec.z());
            return out;
        }

        /**
         * @brief Scaling matrix
         * @param vec   Scaling vector
         */
        static Matrix4 scaling(const Vector3<T>& vec) {
            Matrix4 out; /* (Identity matrix) */
            out.set(0, 0, vec.x());
            out.set(1, 1, vec.y());
            out.set(2, 2, vec.z());
            return out;
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

        /** @copydoc Matrix::Matrix(bool) */
        inline Matrix4(bool identity = true): Matrix<T, 4>(identity) {}

        /** @copydoc Matrix::Matrix(T, U&&...) */
        template<class ...U> inline Matrix4(T first, U&&... next): Matrix<T, 4>(first, std::forward<U>(next)...) {}

        /** @copydoc Matrix::Matrix(const T*) */
        inline Matrix4(const T* data): Matrix<T, 4>(data) {}

        /** @copydoc Matrix::Matrix(const Matrix<T, size>&) */
        inline Matrix4(const Matrix<T, 4>& other): Matrix<T, 4>(other) {}

        /** @copydoc Matrix::operator=() */
        inline Matrix4<T>& operator=(const Matrix<T, 4>& other) { return Matrix<T, 4>::operator=(other); }

        /** @copydoc Matrix::at(size_t) const */
        inline Vector4<T> at(size_t col) const { return Matrix<T, 4>::at(col); }

        /** @copydoc Matrix::at(size_t, size_t) const */
        inline T at(size_t row, size_t col) const { return Matrix<T, 4>::at(row, col); }

        /** @copydoc Matrix::operator*(const Matrix<T, size>&) const */
        inline Matrix4<T> operator*(const Matrix<T, 4>& other) const { return Matrix<T, 4>::operator*(other); }

        /** @copydoc Matrix::operator*(const Vector<T, size>&) const */
        inline Vector4<T> operator*(const Vector<T, 4>& other) const { return Matrix<T, 4>::operator*(other); }

        /** @copydoc Matrix::transposed() */
        inline Matrix4<T> transposed() const { return Matrix<T, 4>::transposed(); }

        /** @copydoc Matrix::ij() */
        inline Matrix3<T> ij(size_t skipRow, size_t skipCol) const { return Matrix<T, 4>::ij(skipRow, skipCol); }

        /** @copydoc Matrix::inverse() */
        inline Matrix4<T> inverse() const { return Matrix<T, 4>::inverse(); }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix4<T>& value) {
    return debug << static_cast<const Magnum::Math::Matrix<T, 4>&>(value);
}
#endif

}}

#endif
