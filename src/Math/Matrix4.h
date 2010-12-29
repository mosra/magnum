#ifndef Magnum_Math_Matrix4_h
#define Magnum_Math_Matrix4_h
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
 * @brief Class Magnum::Math::Matrix4
 */

#include "Matrix.h"

#include "Vector3.h"

namespace Magnum { namespace Math {

/**
 * @brief Matrix 4x4
 *
 * @todo @c PERFORMANCE - make (T,T,T) - (Vec3&lt;T&gt;) alternatives calling
 *      direction based on statistics, which is used more frequently
 * @todo Rotation with Euler angles
 * @todo Shearing
 * @todo Reflection
 */
template<class T> class Matrix4: public Matrix<T, 4> {
    public:
        /**
         * @brief Translation matrix
         * @param x     Translation along X
         * @param y     Translation along Y
         * @param z     Translation along Z
         */
        inline static Matrix4 translation(T x, T y, T z) {
            return translation(Vector3<T>(x, y, z));
        }

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
         * @param x     Scaling along X
         * @param y     Scaling along Y
         * @param z     Scaling along Z
         */
        inline static Matrix4 scaling(T x, T y, T z) {
            return scaling(Vector3<T>(x, y, z));
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
         * @param x     Rotation axis X coordinates
         * @param y     Rotation axis Y coordinates
         * @param z     Rotation axis Z coordinates
         */
        inline static Matrix4 rotation(T angle, T x, T y, T z) {
            return rotation(angle, Vector3<T>(x, y, z));
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

            /* Not creating identity matrix, as nearly all ones would be
               overwritten */
            Matrix4 out(false);
            out.set(3, 3, T(1));

            out.set(0, 0, cosine + xx*oneMinusCosine);
            out.set(0, 1, xy*oneMinusCosine - vn.z()*sine);
            out.set(0, 2, xz*oneMinusCosine + vn.y()*sine);
            out.set(1, 0, xy*oneMinusCosine + vn.z()*sine);
            out.set(1, 1, cosine + yy*oneMinusCosine);
            out.set(1, 2, yz*oneMinusCosine - vn.x()*sine);
            out.set(2, 0, xz*oneMinusCosine - vn.y()*sine);
            out.set(2, 1, yz*oneMinusCosine + vn.x()*sine);
            out.set(2, 2, cosine + zz*oneMinusCosine);

            return out;
        }

        /** @copydoc Matrix::Matrix(bool) */
        inline Matrix4(bool identity = true): Matrix<T, 4>(identity) {}

        /** @copydoc Matrix::Matrix(const T*) */
        inline Matrix4(const T* data): Matrix<T, 4>(data) {}

        /** @copydoc Matrix::Matrix(const Matrix<T, size>&) */
        inline Matrix4(const Matrix<T, 4>& other): Matrix<T, 4>(other) {}
};

}}

#endif
