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
#include "Point2D.h"

namespace Magnum { namespace Math {

/**
@brief 3x3 matrix for affine transformations in 2D
@tparam T   Data type

Provides functions for transformations in 2D. See Matrix4 for 3D
transformations. See also @ref matrix-vector for brief introduction.
@see Magnum::Matrix3, SceneGraph::MatrixTransformation2D
@configurationvalueref{Magnum::Math::Matrix3}
*/
template<class T> class Matrix3: public Matrix<3, T> {
    public:
        /**
         * @brief 2D translation matrix
         * @param vector    Translation vector
         *
         * @see translation(), Matrix4::translation(const Vector3&),
         *      Vector2::xAxis(), Vector2::yAxis()
         */
        inline constexpr static Matrix3<T> translation(const Vector2<T>& vector) {
            return Matrix3<T>( /* Column-major! */
                T(1), T(0), T(0),
                T(0), T(1), T(0),
                vector.x(), vector.y(), T(1)
            );
        }

        /**
         * @brief 2D scaling matrix
         * @param vector    Scaling vector
         *
         * @see rotationScaling() const, Matrix4::scaling(const Vector3&),
         *      Vector2::xScale(), Vector2::yScale()
         */
        inline constexpr static Matrix3<T> scaling(const Vector2<T>& vector) {
            return Matrix3<T>( /* Column-major! */
                vector.x(), T(0), T(0),
                T(0), vector.y(), T(0),
                T(0), T(0), T(1)
            );
        }

        /**
         * @brief 2D rotation matrix
         * @param angle     Rotation angle (counterclockwise, in radians)
         *
         * @see rotation() const, Matrix4::rotation(T, const Vector3&), deg(),
         *      rad()
         */
        static Matrix3<T> rotation(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return Matrix3<T>( /* Column-major! */
                cosine, sine, T(0),
                -sine, cosine, T(0),
                T(0), T(0), T(1)
            );
        }

        /**
         * @brief Create matrix from rotation/scaling part and translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 2x2
         *      matrix)
         * @param translation       Translation part (first two elements of
         *      third column)
         *
         * @see rotationScaling() const, translation() const
         */
        static Matrix3<T> from(const Matrix<2, T>& rotationScaling, const Vector2<T>& translation = Vector2<T>()) {
            return from(
                Vector3<T>(rotationScaling[0], T(0)),
                Vector3<T>(rotationScaling[1], T(0)),
                Vector3<T>(translation, T(1))
            );
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::ZeroType): Matrix<3, T>(Matrix<3, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        inline constexpr /*implicit*/ Matrix3(typename Matrix<3, T>::IdentityType = (Matrix<3, T>::Identity), T value = T(1)): Matrix<3, T>(
            value, T(0), T(0),
            T(0), value, T(0),
            T(0), T(0), value
        ) {}

        /** @copydoc Matrix::Matrix */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr /*implicit*/ Matrix3(T first, U... next): Matrix<3, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr /*implicit*/ Matrix3(T first, U... next) {}
        #endif

        /** @brief Copy constructor */
        inline constexpr Matrix3(const RectangularMatrix<3, 3, T>& other): Matrix<3, T>(other) {}

        /**
         * @brief 2D rotation and scaling part of the matrix
         *
         * Upper-left 2x2 part of the matrix.
         * @see from(const Matrix<2, T>&, const Vector2&), rotation() const,
         *      rotation(T), Matrix4::rotationScaling() const
         */
        inline Matrix<2, T> rotationScaling() const {
            return Matrix<2, T>::from(
                (*this)[0].xy(),
                (*this)[1].xy());
        }

        /**
         * @brief 2D rotation part of the matrix
         *
         * Normalized upper-left 2x2 part of the matrix.
         * @see rotationScaling() const, rotation(T), Matrix4::rotation() const
         */
        inline Matrix<2, T> rotation() const {
            return Matrix<2, T>::from(
                (*this)[0].xy().normalized(),
                (*this)[1].xy().normalized());
        }

        /**
         * @brief Right-pointing 2D vector
         *
         * First two elements of first column.
         * @see Vector2::xAxis()
         */
        inline Vector2<T>& right() { return (*this)[0].xy(); }
        inline constexpr Vector2<T> right() const { return (*this)[0].xy(); } /**< @overload */

        /**
         * @brief Up-pointing 2D vector
         *
         * First two elements of second column.
         * @see Vector2::yAxis()
         */
        inline Vector2<T>& up() { return (*this)[1].xy(); }
        inline constexpr Vector2<T> up() const { return (*this)[1].xy(); } /**< @overload */

        /**
         * @brief 2D translation part of the matrix
         *
         * First two elements of third column.
         * @see translation(const Vector2&), Matrix4::translation()
         */
        inline Vector2<T>& translation() { return (*this)[2].xy(); }
        inline constexpr Vector2<T> translation() const { return (*this)[2].xy(); } /**< @overload */

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Point2D<T> operator*(const Point2D<T>& other) const {
            return Matrix<3, T>::operator*(other);
        }
        #endif

        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix3, Vector3, 3)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(3, 3, Matrix3<T>)
};

/** @debugoperator{Magnum::Math::Matrix3} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Matrix3<T>& value) {
    return debug << static_cast<const Matrix<3, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix3<T>>: public ConfigurationValue<Magnum::Math::Matrix<3, T>> {};
}}

#endif
