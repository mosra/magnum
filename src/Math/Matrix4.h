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
@brief 4x4 matrix for transformations in 3D
@tparam T   Data type

Provides functions for transformations in 3D. See Matrix3 for 2D
transformations. See also @ref matrix-vector for brief introduction.
@see Magnum::Matrix4, SceneGraph::MatrixTransformation3D
@configurationvalueref{Magnum::Math::Matrix4}
 */
template<class T> class Matrix4: public Matrix<4, T> {
    public:
        /**
         * @brief 3D translation
         * @param vector    Translation vector
         *
         * @see translation(), DualQuaternion::translation(),
         *      Matrix3::translation(const Vector2&), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis()
         */
        inline constexpr static Matrix4<T> translation(const Vector3<T>& vector) {
            return {{      T(1),       T(0),       T(0), T(0)},
                    {      T(0),       T(1),       T(0), T(0)},
                    {      T(0),       T(0),       T(1), T(0)},
                    {vector.x(), vector.y(), vector.z(), T(1)}};
        }

        /**
         * @brief 3D scaling
         * @param vector    Scaling vector
         *
         * @see rotationScaling() const, Matrix3::scaling(const Vector2&),
         *      Vector3::xScale(), Vector3::yScale(), Vector3::zScale()
         */
        inline constexpr static Matrix4<T> scaling(const Vector3<T>& vector) {
            return {{vector.x(),       T(0),       T(0), T(0)},
                    {      T(0), vector.y(),       T(0), T(0)},
                    {      T(0),       T(0), vector.z(), T(0)},
                    {      T(0),       T(0),       T(0), T(1)}};
        }

        /**
         * @brief 3D rotation around arbitrary axis
         * @param angle             Rotation angle (counterclockwise, in radians)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. If possible, use
         * faster alternatives like rotationX(), rotationY() and rotationZ().
         * @see rotation() const, DualQuaternion::rotation(),
         *      Quaternion::rotation(), Matrix3::rotation(T), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis(), deg(), rad()
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

            return {
                {cosine + xx*oneMinusCosine,
                    xy*oneMinusCosine + normalizedAxis.z()*sine,
                        xz*oneMinusCosine - normalizedAxis.y()*sine,
                           T(0)},
                {xy*oneMinusCosine - normalizedAxis.z()*sine,
                    cosine + yy*oneMinusCosine,
                        yz*oneMinusCosine + normalizedAxis.x()*sine,
                           T(0)},
                {xz*oneMinusCosine + normalizedAxis.y()*sine,
                    yz*oneMinusCosine - normalizedAxis.x()*sine,
                        cosine + zz*oneMinusCosine,
                           T(0)},
                {T(0), T(0), T(0), T(1)}
            };
        }

        /**
         * @brief 3D rotation around X axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::xAxis())`.
         * @see rotation(T, const Vector3&), rotationY(), rotationZ(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(T),
         *      deg(), rad()
         */
        static Matrix4<T> rotationX(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return {{T(1),   T(0),   T(0), T(0)},
                    {T(0), cosine,   sine, T(0)},
                    {T(0),  -sine, cosine, T(0)},
                    {T(0),   T(0),   T(0), T(1)}};
        }

        /**
         * @brief 3D rotation around Y axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::yAxis())`.
         * @see rotation(T, const Vector3&), rotationX(), rotationZ(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(T),
         *      deg(), rad()
         */
        static Matrix4<T> rotationY(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return {{cosine, T(0),  -sine, T(0)},
                    {  T(0), T(1),   T(0), T(0)},
                    {  sine, T(0), cosine, T(0)},
                    {  T(0), T(0),   T(0), T(1)}};
        }

        /**
         * @brief 3D rotation matrix around Z axis
         * @param angle Rotation angle (counterclockwise, in radians)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::zAxis())`.
         * @see rotation(T, const Vector3&), rotationX(), rotationY(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(T),
         *      deg(), rad()
         */
        static Matrix4<T> rotationZ(T angle) {
            T sine = std::sin(angle);
            T cosine = std::cos(angle);

            return {{cosine,   sine, T(0), T(0)},
                    { -sine, cosine, T(0), T(0)},
                    {  T(0),   T(0), T(1), T(0)},
                    {  T(0),   T(0), T(0), T(1)}};
        }

        /**
         * @brief 3D reflection matrix
         * @param normal    Normal of the plane through which to reflect
         *
         * Expects that the normal is normalized.
         * @see Matrix3::reflection()
         */
        static Matrix4<T> reflection(const Vector3<T>& normal) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normal.dot(), T(1)),
                           "Math::Matrix4::reflection(): normal must be normalized", {});
            return from(Matrix<3, T>() - T(2)*normal*RectangularMatrix<1, 3, T>(normal).transposed(), {});
        }

        /**
         * @brief 3D orthographic projection matrix
         * @param size      Size of the view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * @see perspectiveProjection(), Matrix3::projection()
         */
        static Matrix4<T> orthographicProjection(const Vector2<T>& size, T near, T far) {
            Vector2<T> xyScale = T(2.0)/size;
            T zScale = T(2.0)/(near-far);

            return {{xyScale.x(),        T(0),             T(0), T(0)},
                    {       T(0), xyScale.y(),             T(0), T(0)},
                    {       T(0),        T(0),           zScale, T(0)},
                    {       T(0),        T(0), near*zScale-T(1), T(1)}};
        }

        /**
         * @brief 3D perspective projection matrix
         * @param size      Size of near clipping plane
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * @see orthographicProjection(), Matrix3::projection()
         */
        static Matrix4<T> perspectiveProjection(const Vector2<T>& size, T near, T far) {
            Vector2<T> xyScale = 2*near/size;
            T zScale = T(1.0)/(near-far);

            return {{xyScale.x(),        T(0),                 T(0),  T(0)},
                    {       T(0), xyScale.y(),                 T(0),  T(0)},
                    {       T(0),        T(0),    (far+near)*zScale, T(-1)},
                    {       T(0),        T(0), T(2)*far*near*zScale,  T(0)}};
        }

        /**
         * @brief 3D perspective projection matrix
         * @param fov           Field of view angle (horizontal, in radians)
         * @param aspectRatio   Aspect ratio
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         *
         * @see orthographicProjection(), Matrix3::projection()
         */
        static Matrix4<T> perspectiveProjection(T fov, T aspectRatio, T near, T far) {
            T xyScale = 2*std::tan(fov/2)*near;

            return perspectiveProjection(Vector2<T>(xyScale, xyScale/aspectRatio), near, far);
        }

        /**
         * @brief Create matrix from rotation/scaling part and translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 3x3
         *      matrix)
         * @param translation       Translation part (first three elements of
         *      fourth column)
         *
         * @see rotationScaling() const, translation() const
         */
        static Matrix4<T> from(const Matrix<3, T>& rotationScaling, const Vector3<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {rotationScaling[2], T(0)},
                    {       translation, T(1)}};
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix4(typename Matrix<4, T>::ZeroType): Matrix<4, T>(Matrix<4, T>::Zero) {}

        /** @copydoc Matrix::Matrix(IdentityType, T) */
        /** @todo Use constexpr implementation in Matrix, when done */
        inline constexpr /*implicit*/ Matrix4(typename Matrix<4, T>::IdentityType = (Matrix<4, T>::Identity), T value = T(1)): Matrix<4, T>(
            Vector<4, T>(value,  T(0),  T(0),  T(0)),
            Vector<4, T>( T(0), value,  T(0),  T(0)),
            Vector<4, T>( T(0),  T(0), value,  T(0)),
            Vector<4, T>( T(0),  T(0),  T(0), value)
        ) {}

        /** @brief %Matrix from column vectors */
        inline constexpr /*implicit*/ Matrix4(const Vector4<T>& first, const Vector4<T>& second, const Vector4<T>& third, const Vector4<T>& fourth): Matrix<4, T>(first, second, third, fourth) {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> inline constexpr explicit Matrix4(const RectangularMatrix<4, 4, U>& other): Matrix<4, T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Matrix4(const RectangularMatrix<4, 4, T>& other): Matrix<4, T>(other) {}

        /**
         * @brief 3D rotation and scaling part of the matrix
         *
         * Upper-left 3x3 part of the matrix.
         * @see from(const Matrix<3, T>&, const Vector3&), rotation() const,
         *      rotation(T, const Vector3&), Matrix3::rotationScaling() const
         */
        inline Matrix<3, T> rotationScaling() const {
            /* Not Matrix3, because it is for affine 2D transformations */
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
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
            return {(*this)[0].xyz().normalized(),
                    (*this)[1].xyz().normalized(),
                    (*this)[2].xyz().normalized()};
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
         * @see from(const Matrix<3, T>&, const Vector3&),
         *      translation(const Vector3&), Matrix3::translation()
         */
        inline Vector3<T>& translation() { return (*this)[3].xyz(); }
        inline constexpr Vector3<T> translation() const { return (*this)[3].xyz(); } /**< @overload */

        /**
         * @brief Inverted Euclidean transformation matrix
         *
         * Expects that the matrix represents Euclidean transformation (i.e.
         * only rotation and translation, no scaling) and creates inverted
         * matrix from transposed rotation part and negated translation part.
         * Significantly faster than the general algorithm in inverted().
         * @see rotationScaling() const, translation() const
         */
        inline Matrix4<T> invertedEuclidean() const {
            CORRADE_ASSERT((*this)[0][3] == T(0) && (*this)[1][3] == T(0) && (*this)[2][3] == T(0) && (*this)[3][3] == T(1),
                "Math::Matrix4::invertedEuclidean(): unexpected values on last row", {});
            Matrix<3, T> inverseRotation = rotationScaling().transposed();
            CORRADE_ASSERT((inverseRotation*rotationScaling() == Matrix<3, T>()),
                "Math::Matrix4::invertedEuclidean(): the matrix doesn't represent Euclidean transformation", {});
            return from(inverseRotation, inverseRotation*-translation());
        }

        /**
         * @brief Transform 3D vector with the matrix
         *
         * Translation is not involved in the transformation. @f[
         *      \boldsymbol v' = \boldsymbol M (v_x, v_y, v_z, 0)^T
         * @f]
         * @see transformPoint(), Quaternion::rotateVector(),
         *      Matrix3::transformVector()
         */
        inline Vector3<T> transformVector(const Vector3<T>& vector) const {
            return ((*this)*Vector4<T>(vector, T(0))).xyz();
        }

        /**
         * @brief Transform 3D point with the matrix
         *
         * Unlike in transformVector(), translation is also involved. @f[
         *      \boldsymbol v' = \boldsymbol M (v_x, v_y, v_z, 1)^T
         * @f]
         * @see DualQuaternion::transformPoint(), Matrix3::transformPoint()
         */
        inline Vector3<T> transformPoint(const Vector3<T>& vector) const {
            return ((*this)*Vector4<T>(vector, T(1))).xyz();
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Point3D<T> operator*(const Point3D<T>& other) const {
            return Matrix<4, T>::operator*(other);
        }
        #endif

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(4, 4, Matrix4<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix4, Vector4, 4)
};

MAGNUM_MATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(Matrix4, 4)

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
