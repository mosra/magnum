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

/**
@brief 3x3 matrix

Provides functions for transformations in 2D. See also Matrix4 for 3D
transformations.
*/
template<class T> class Matrix3: public Matrix<3, T> {
    public:
        /**
         * @brief 2D translation matrix
         * @param vec   Translation vector
         *
         * @see Matrix4::translation(), Vector2::xAxis(), Vector2::yAxis()
         */
        inline constexpr static Matrix3<T> translation(const Vector2<T>& vec) {
            return Matrix3<T>( /* Column-major! */
                T(1), T(0), T(0),
                T(0), T(1), T(0),
                vec.x(), vec.y(), T(1)
            );
        }

        /**
         * @brief 2D scaling matrix
         * @param vec   Scaling vector
         *
         * @see Matrix4::scaling(), Vector2::xScale(), Vector2::yScale()
         */
        inline constexpr static Matrix3<T> scaling(const Vector2<T>& vec) {
            return Matrix3<T>( /* Column-major! */
                vec.x(), T(0), T(0),
                T(0), vec.y(), T(0),
                T(0), T(0), T(1)
            );
        }

        /**
         * @brief 3D rotation matrix
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * @see Matrix4::rotation(), deg(), rad()
         */
        static Matrix3<T> rotation(T angle) {
            return Matrix3<T>( /* Column-major! */
                T(cos(angle)), T(sin(angle)), T(0),
                -T(sin(angle)), T(cos(angle)), T(0),
                T(0), T(0), T(1)
            );
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::ZeroType): Matrix<3, T>(Matrix<3, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::IdentityType = (Matrix<3, T>::Identity), T value = T(1)): Matrix<3, T>(
            value, T(0), T(0),
            T(0), value, T(0),
            T(0), T(0), value
        ) {}

        /** @copydoc Matrix::Matrix(T, U...) */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix3(T first, U... next): Matrix<3, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr Matrix3(T first, U... next) {}
        #endif

        /** @copydoc Matrix::Matrix(const Matrix<size, T>&) */
        inline constexpr Matrix3(const Matrix<3, T>& other): Matrix<3, T>(other) {}

        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix3, Vector3, 3)
};

/** @debugoperator{Magnum::Math::Matrix3} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Matrix3<T>& value) {
    return debug << static_cast<const Magnum::Math::Matrix<3, T>&>(value);
}

}}

#endif
