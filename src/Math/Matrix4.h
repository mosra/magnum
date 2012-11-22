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

#include "Matrix.h"
#include "Point3D.h"

namespace Magnum { namespace Math {

/**
@brief 4x4 matrix for affine transformations in 3D
@tparam T   Data type

Provides functions for transformations in 3D. See Matrix3 for 2D
transformations. See also @ref matrix-vector for brief introduction.
@see Magnum::Matrix4
@configurationvalueref{Magnum::Math::Matrix4}
@todo Shearing
@todo Reflection
 */
template<class T> class Matrix4: public Matrix<4, T> {
    public:
        /**
         * @brief 3D translation
         * @param vector    Translation vector
         *
         * @see translation(), Matrix3::translation(const Vector2&),
         *      Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()
         */
        inline constexpr static Matrix4<T> translation(const Vector3<T>& vector) {
            return Matrix4<T>( /* Column-major! */
                T(1), T(0), T(0), T(0),
                T(0), T(1), T(0), T(0),
                T(0), T(0), T(1), T(0),
                vector.x(), vector.y(), vector.z(), T(1)
            );
        }

        /**
         * @brief 3D scaling
         * @param vector    Scaling vector
         *
         * @see rotationScaling() const, Matrix3::scaling(const Vector2&),
         *      Vector3::xScale(), Vector3::yScale(), Vector3::zScale()
         */
        inline constexpr static Matrix4<T> scaling(const Vector3<T>& vector) {
            return Matrix4<T>( /* Column-major! */
                vector.x(), T(0), T(0), T(0),
                T(0), vector.y(), T(0), T(0),
                T(0), T(0), vector.z(), T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /**
         * @brief 3D rotation around arbitrary axis
         * @param angle             Rotation angle (counterclockwise, in radians)
         * @param normalizedAxis    Normalized rotation axis
         *
         * If possible, use faster alternatives like rotationX(), rotationY()
         * and rotationZ().
         * @see rotation() const, Matrix3::rotation(T), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis(), deg(), rad()
         * @attention Assertion fails on non-normalized rotation vector and
         *      identity matrix is returned.
         */
        static Matrix4<T> rotation(T angle, const Vector3<T>& normalizedAxis) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normalizedAxis.dot(), T(1)),
                           "Math::Matrix4::rotation(): axis must be normalized", {});

            T sine = std::sin(angle);
            T cosine = std::cos(angle);
            T oneMinusCosine = T(1) - cosine;

            T xx = normalizedAxis.x()*normalizedAxis.x();
            T xy = normalizedAxis.x()*normalizedAxis.y();
            T xz = normalizedAxis.x()*normalizedAxis.z();
            T yy = normalizedAxis.y()*normalizedAxis.y();
            T yz = normalizedAxis.y()*normalizedAxis.z();
            T zz = normalizedAxis.z()*normalizedAxis.z();

            return Matrix4<T>( /* Column-major! */
                cosine + xx*oneMinusCosine,
                    xy*oneMinusCosine + normalizedAxis.z()*sine,
                        xz*oneMinusCosine - normalizedAxis.y()*sine,
                           T(0),
                xy*oneMinusCosine - normalizedAxis.z()*sine,
                    cosine + yy*oneMinusCosine,
                        yz*oneMinusCosine + normalizedAxis.x()*sine,
                           T(0),
                xz*oneMinusCosine + normalizedAxis.y()*sine,
                    yz*oneMinusCosine - normalizedAxis.x()*sine,
                        cosine + zz*oneMinusCosine,
                           T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /**
         * @brief 3D rotation around X axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::xAxis())`.
         * @see rotation(T, const Vector3&), rotationY(), rotationZ(),
         *      rotation() const, Matrix3::rotation(T), deg(), rad()
         */
        static Matrix4<T> rotationX(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return Matrix4<T>( /* Column-major! */
                T(1), T(0), T(0), T(0),
                T(0), cosine, sine, T(0),
                T(0), -sine, cosine, T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /**
         * @brief 3D rotation around Y axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::yAxis())`.
         * @see rotation(T, const Vector3&), rotationX(), rotationZ(),
         *      rotation() const, Matrix3::rotation(T), deg(), rad()
         */
        static Matrix4<T> rotationY(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return Matrix4<T>( /* Column-major! */
                cosine, T(0), -sine, T(0),
                T(0), T(1), T(0), T(0),
                sine, T(0), cosine, T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /**
         * @brief 3D rotation matrix around Z axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::zAxis())`.
         * @see rotation(T, const Vector3&), rotationX(), rotationY(),
         *      rotation() const, Matrix3::rotation(T), deg(), rad()
         */
        static Matrix4<T> rotationZ(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return Matrix4<T>( /* Column-major! */
                cosine, sine, T(0), T(0),
                -sine, cosine, T(0), T(0),
                T(0), T(0), T(1), T(0),
                T(0), T(0), T(0), T(1)
            );
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix4(typename Matrix<4, T>::ZeroType): Matrix<4, T>(Matrix<4, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        inline constexpr Matrix4(typename Matrix<4, T>::IdentityType = (Matrix<4, T>::Identity), T value = T(1)): Matrix<4, T>(
            value, T(0), T(0), T(0),
            T(0), value, T(0), T(0),
            T(0), T(0), value, T(0),
            T(0), T(0), T(0), value
        ) {}

        /** @copydoc Matrix::Matrix */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr Matrix4(T first, U... next): Matrix<4, T>(first, next...) {}
        #else
        template<class ...U> inline constexpr Matrix4(T first, U... next) {}
        #endif

        /** @brief Copy constructor */
        inline constexpr Matrix4(const RectangularMatrix<4, 4, T>& other): Matrix<4, T>(other) {}

        /**
         * @brief 3D rotation and scaling part of the matrix
         *
         * Upper-left 3x3 part of the matrix.
         * @see rotation() const, rotation(T, const Vector3&),
         *      Matrix3::rotationScaling() const
         */
        inline Matrix<3, T> rotationScaling() const {
            /* Not Matrix3, because it is for affine 2D transformations */
            #ifndef CORRADE_GCC45_COMPATIBILITY /* GCC 4.5 badly optimizes this */
            return Matrix<3, T>::from(
                (*this)[0].xyz(),
                (*this)[1].xyz(),
                (*this)[2].xyz());
            #else
            return Matrix<3, T>(
                (*this)(0, 0), (*this)(0, 1), (*this)(0, 2),
                (*this)(1, 0), (*this)(1, 1), (*this)(1, 2),
                (*this)(2, 0), (*this)(2, 1), (*this)(2, 2));
            #endif
        }

        /**
         * @brief 3D rotation part of the matrix
         *
         * Normalized upper-left 3x3 part of the matrix.
         * @see rotationScaling() const, rotation(T, const Vector3&),
         *      Matrix3::rotation() const
         */
        inline Matrix<3, T> rotation() const {
            /* Not Matrix3, because it is for affine 2D transformations */
            return Matrix<3, T>::from(
                #ifndef CORRADE_GCC45_COMPATIBILITY /* GCC 4.5 badly optimizes this */
                (*this)[0].xyz().normalized(),
                (*this)[1].xyz().normalized(),
                (*this)[2].xyz().normalized());
                #else
                Vector3<T>((*this)(0, 0), (*this)(0, 1), (*this)(0, 2)).normalized(),
                Vector3<T>((*this)(1, 0), (*this)(1, 1), (*this)(1, 2)).normalized(),
                Vector3<T>((*this)(2, 0), (*this)(2, 1), (*this)(2, 2)).normalized());
                #endif
        }


        /**
         * @brief Right-pointing 3D vector
         *
         * First three elements of first column.
         * @see Vector3::xAxis()
         */
        inline Vector3<T>& right() { return (*this)[0].xyz(); }
        inline constexpr Vector3<T> right() const { return (*this)[0].xyz(); } /**< @overload */

        /**
         * @brief Up-pointing 3D vector
         *
         * First three elements of second column.
         * @see Vector3::yAxis()
         */
        inline Vector3<T>& up() { return (*this)[1].xyz(); }
        inline constexpr Vector3<T> up() const { return (*this)[1].xyz(); } /**< @overload */

        /**
         * @brief Backward-pointing 3D vector
         *
         * First three elements of third column.
         * @see Vector3::yAxis()
         */
        inline Vector3<T>& backward() { return (*this)[2].xyz(); }
        inline constexpr Vector3<T> backward() const { return (*this)[2].xyz(); } /**< @overload */

        /**
         * @brief 3D translation part of the matrix
         *
         * First three elements of fourth column.
         * @see translation(const Vector3&), Matrix3::translation()
         */
        inline Vector3<T>& translation() { return (*this)[3].xyz(); }
        inline constexpr Vector3<T> translation() const { return (*this)[3].xyz(); } /**< @overload */

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Point3D<T> operator*(const Point3D<T>& other) const {
            return Matrix<4, T>::operator*(other);
        }
        #endif

        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix4, Vector4, 4)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(4, 4, Matrix4<T>)
};

/** @debugoperator{Magnum::Math::Matrix4} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Matrix4<T>& value) {
    return debug << static_cast<const Matrix<4, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix4<T>>: public ConfigurationValue<Magnum::Math::Matrix<4, T>> {};
}}

#endif
