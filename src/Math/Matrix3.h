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

#include "Math/Matrix.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Math {

/**
@brief 3x3 matrix
@tparam T   Underlying data type

Represents 2D transformation. See @ref matrix-vector and @ref transformations
for brief introduction.
@see Magnum::Matrix3, Magnum::Matrix3d, DualComplex,
    SceneGraph::MatrixTransformation2D
@configurationvalueref{Magnum::Math::Matrix3}
*/
template<class T> class Matrix3: public Matrix<3, T> {
    public:
        /**
         * @brief 2D translation matrix
         * @param vector    Translation vector
         *
         * @see translation(), DualComplex::translation(),
         *      Matrix4::translation(const Vector3&), Vector2::xAxis(),
         *      Vector2::yAxis()
         */
        inline constexpr static Matrix3<T> translation(const Vector2<T>& vector) {
            return {{      T(1),       T(0), T(0)},
                    {      T(0),       T(1), T(0)},
                    {vector.x(), vector.y(), T(1)}};
        }

        /**
         * @brief 2D scaling matrix
         * @param vector    Scaling vector
         *
         * @see rotationScaling() const, Matrix4::scaling(const Vector3&),
         *      Vector2::xScale(), Vector2::yScale()
         */
        inline constexpr static Matrix3<T> scaling(const Vector2<T>& vector) {
            return {{vector.x(),       T(0), T(0)},
                    {      T(0), vector.y(), T(0)},
                    {      T(0),       T(0), T(1)}};
        }

        /**
         * @brief 2D rotation matrix
         * @param angle     Rotation angle (counterclockwise)
         *
         * @see rotation() const, Complex::rotation(), DualComplex::rotation(),
         *      Matrix4::rotation(Rad, const Vector3&)
         */
        static Matrix3<T> rotation(Rad<T> angle) {
            T sine = std::sin(T(angle));
            T cosine = std::cos(T(angle));

            return {{ cosine,   sine, T(0)},
                    {  -sine, cosine, T(0)},
                    {   T(0),   T(0), T(1)}};
        }

        /**
         * @brief 2D reflection matrix
         * @param normal    Normal of the line through which to reflect
         *
         * Expects that the normal is normalized.
         * @see Matrix4::reflection()
         */
        static Matrix3<T> reflection(const Vector2<T>& normal) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normal.dot(), T(1)),
                           "Math::Matrix3::reflection(): normal must be normalized", {});
            return from(Matrix<2, T>() - T(2)*normal*RectangularMatrix<1, 2, T>(normal).transposed(), {});
        }

        /**
         * @brief 2D projection matrix
         * @param size      Size of the view
         *
         * @see Matrix4::orthographicProjection(), Matrix4::perspectiveProjection()
         */
        static Matrix3<T> projection(const Vector2<T>& size) {
            return scaling(2.0f/size);
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
        inline constexpr static Matrix3<T> from(const Matrix<2, T>& rotationScaling, const Vector2<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {       translation, T(1)}};
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        inline constexpr explicit Matrix3(typename Matrix<3, T>::ZeroType): Matrix<3, T>(Matrix<3, T>::Zero) {}

        /**
         * @brief Default constructor
         *
         * Creates identity matrix. You can also explicitly call this
         * constructor with `Matrix3 m(Matrix3::Identity);`. Optional parameter
         * @p value allows you to specify value on diagonal.
         * @todo Use constexpr implementation in Matrix, when done
         */
        inline constexpr /*implicit*/ Matrix3(typename Matrix<3, T>::IdentityType = (Matrix<3, T>::Identity), T value = T(1)): Matrix<3, T>(
            Vector<3, T>(value,  T(0),  T(0)),
            Vector<3, T>( T(0), value,  T(0)),
            Vector<3, T>( T(0),  T(0), value)
        ) {}

        /** @brief %Matrix from column vectors */
        inline constexpr /*implicit*/ Matrix3(const Vector3<T>& first, const Vector3<T>& second, const Vector3<T>& third): Matrix<3, T>(first, second, third) {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> inline constexpr explicit Matrix3(const RectangularMatrix<3, 3, U>& other): Matrix<3, T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Matrix3(const RectangularMatrix<3, 3, T>& other): Matrix<3, T>(other) {}

        /**
         * @brief 2D rotation and scaling part of the matrix
         *
         * Upper-left 2x2 part of the matrix.
         * @see from(const Matrix<2, T>&, const Vector2&), rotation() const,
         *      rotation(T), Matrix4::rotationScaling() const
         */
        inline constexpr Matrix<2, T> rotationScaling() const {
            return {(*this)[0].xy(),
                    (*this)[1].xy()};
        }

        /**
         * @brief 2D rotation part of the matrix
         *
         * Normalized upper-left 2x2 part of the matrix.
         * @see rotationScaling() const, rotation(T), Matrix4::rotation() const
         * @todo assert uniform scaling (otherwise this would be garbage)
         */
        inline Matrix<2, T> rotation() const {
            return {(*this)[0].xy().normalized(),
                    (*this)[1].xy().normalized()};
        }

        /** @todo uniform scaling extraction */

        /**
         * @brief Right-pointing 2D vector
         *
         * First two elements of first column.
         * @see up(), Vector2::xAxis(), Matrix4::right()
         */
        inline Vector2<T>& right() { return (*this)[0].xy(); }
        inline constexpr Vector2<T> right() const { return (*this)[0].xy(); } /**< @overload */

        /**
         * @brief Up-pointing 2D vector
         *
         * First two elements of second column.
         * @see right(), Vector2::yAxis(), Matrix4::up()
         */
        inline Vector2<T>& up() { return (*this)[1].xy(); }
        inline constexpr Vector2<T> up() const { return (*this)[1].xy(); } /**< @overload */

        /**
         * @brief 2D translation part of the matrix
         *
         * First two elements of third column.
         * @see from(const Matrix<2, T>&, const Vector2&),
         *      translation(const Vector2&), Matrix4::translation()
         */
        inline Vector2<T>& translation() { return (*this)[2].xy(); }
        inline constexpr Vector2<T> translation() const { return (*this)[2].xy(); } /**< @overload */

        /**
         * @brief Inverted Euclidean transformation matrix
         *
         * Assumes that the matrix represents Euclidean transformation (i.e.
         * only rotation and translation, no scaling) and creates inverted
         * matrix from transposed rotation part and negated translation part.
         * Significantly faster than the general algorithm in inverted().
         * @see rotationScaling() const, translation() const
         */
        inline Matrix3<T> invertedEuclidean() const {
            CORRADE_ASSERT((*this)[0][2] == T(0) && (*this)[1][2] == T(0) && (*this)[2][2] == T(1),
                "Math::Matrix3::invertedEuclidean(): unexpected values on last row", {});
            Matrix<2, T> inverseRotation = rotationScaling().transposed();
            CORRADE_ASSERT((inverseRotation*rotationScaling() == Matrix<2, T>()),
                "Math::Matrix3::invertedEuclidean(): the matrix doesn't represent Euclidean transformation", {});
            return from(inverseRotation, inverseRotation*-translation());
        }

        /**
         * @brief Transform 2D vector with the matrix
         *
         * Unlike in transformPoint(), translation is not involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ 0 \end{pmatrix}
         * @f]
         * @see Complex::transformVector(), Matrix4::transformVector()
         * @todo extract 2x2 matrix and multiply directly? (benchmark that)
         */
        inline Vector2<T> transformVector(const Vector2<T>& vector) const {
            return ((*this)*Vector3<T>(vector, T(0))).xy();
        }

        /**
         * @brief Transform 2D point with the matrix
         *
         * Unlike in transformVector(), translation is also involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ 1 \end{pmatrix}
         * @f]
         * @see DualComplex::transformPoint(), Matrix4::transformPoint()
         */
        inline Vector2<T> transformPoint(const Vector2<T>& vector) const {
            return ((*this)*Vector3<T>(vector, T(1))).xy();
        }

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(3, 3, Matrix3<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(Matrix3, Vector3, 3)
};

MAGNUM_MATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(Matrix3, 3)

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
