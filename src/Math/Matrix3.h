#ifndef Magnum_Math_Matrix3_h
#define Magnum_Math_Matrix3_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
        constexpr static Matrix3<T> translation(const Vector2<T>& vector) {
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
        constexpr static Matrix3<T> scaling(const Vector2<T>& vector) {
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
        static Matrix3<T> rotation(Rad<T> angle);

        /**
         * @brief 2D reflection matrix
         * @param normal    Normal of the line through which to reflect
         *
         * Expects that the normal is normalized.
         * @see Matrix4::reflection(), Vector::isNormalized()
         */
        static Matrix3<T> reflection(const Vector2<T>& normal) {
            CORRADE_ASSERT(normal.isNormalized(),
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
        constexpr static Matrix3<T> from(const Matrix<2, T>& rotationScaling, const Vector2<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {       translation, T(1)}};
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        constexpr explicit Matrix3(typename Matrix<3, T>::ZeroType): Matrix<3, T>(Matrix<3, T>::Zero) {}

        /**
         * @brief Default constructor
         *
         * Creates identity matrix. You can also explicitly call this
         * constructor with `Matrix3 m(Matrix3::Identity);`. Optional parameter
         * @p value allows you to specify value on diagonal.
         * @todo Use constexpr implementation in Matrix, when done
         */
        constexpr /*implicit*/ Matrix3(typename Matrix<3, T>::IdentityType = (Matrix<3, T>::Identity), T value = T(1)): Matrix<3, T>(
            Vector<3, T>(value,  T(0),  T(0)),
            Vector<3, T>( T(0), value,  T(0)),
            Vector<3, T>( T(0),  T(0), value)
        ) {}

        /** @brief %Matrix from column vectors */
        constexpr /*implicit*/ Matrix3(const Vector3<T>& first, const Vector3<T>& second, const Vector3<T>& third): Matrix<3, T>(first, second, third) {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix3(const RectangularMatrix<3, 3, U>& other): Matrix<3, T>(other) {}

        /** @brief Construct matrix from external representation */
        #ifndef CORRADE_GCC44_COMPATIBILITY
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(std::declval<U>()))> constexpr explicit Matrix3(const U& other): Matrix<3, T>(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(*static_cast<const U*>(nullptr)))> constexpr explicit Matrix3(const U& other): Matrix<3, T>(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(other)) {}
        #endif

        /** @brief Copy constructor */
        constexpr Matrix3(const RectangularMatrix<3, 3, T>& other): Matrix<3, T>(other) {}

        /**
         * @brief Check whether the matrix represents rigid transformation
         *
         * Rigid transformation consists only of rotation and translation (i.e.
         * no scaling or projection).
         * @see isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(2) == Vector3<T>(T(0), T(0), T(1));
        }

        /**
         * @brief 2D rotation and scaling part of the matrix
         *
         * Upper-left 2x2 part of the matrix.
         * @see from(const Matrix<2, T>&, const Vector2&), rotation() const
         *      rotationNormalized(), @ref uniformScaling(), rotation(T),
         *      Matrix4::rotationScaling() const
         */
        constexpr Matrix<2, T> rotationScaling() const {
            return {(*this)[0].xy(),
                    (*this)[1].xy()};
        }

        /**
         * @brief 2D rotation part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotationScaling(), but additionally checks that the
         * base vectors are normalized.
         * @see rotation() const, @ref uniformScaling(),
         *      @ref Matrix4::rotationNormalized()
         * @todo assert also orthogonality or this is good enough?
         */
        Matrix<2, T> rotationNormalized() const {
            CORRADE_ASSERT((*this)[0].xy().isNormalized() && (*this)[1].xy().isNormalized(),
                           "Math::Matrix3::rotationNormalized(): the rotation part is not normalized", {});
            return {(*this)[0].xy(),
                    (*this)[1].xy()};
        }

        /**
         * @brief 2D rotation part of the matrix
         *
         * Normalized upper-left 2x2 part of the matrix. Expects uniform
         * scaling.
         * @see rotationNormalized(), rotationScaling(), @ref uniformScaling(),
         *      rotation(T), Matrix4::rotation() const
         */
        Matrix<2, T> rotation() const {
            CORRADE_ASSERT(TypeTraits<T>::equals((*this)[0].xy().dot(), (*this)[1].xy().dot()),
                           "Math::Matrix3::rotation(): the matrix doesn't have uniform scaling", {});
            return {(*this)[0].xy().normalized(),
                    (*this)[1].xy().normalized()};
        }

        /**
         * @brief Uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 2x2 part of the matrix.
         * Expects that the scaling is the same in all axes. Faster alternative
         * to @ref uniformScaling(), because it doesn't compute the square
         * root.
         * @see @ref rotationScaling(), @ref rotation(),
         *      @ref rotationNormalized(), @ref scaling(const Vector2&),
         *      @ref Matrix4::uniformScaling()
         */
        T uniformScalingSquared() const {
            const T scalingSquared = (*this)[0].xy().dot();
            CORRADE_ASSERT(TypeTraits<T>::equals((*this)[1].xy().dot(), scalingSquared),
                           "Math::Matrix3::uniformScaling(): the matrix doesn't have uniform scaling", {});
            return scalingSquared;
        }

        /**
         * @brief Uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 2x2 part of the matrix. Expects that
         * the scaling is the same in all axes. Use faster alternative
         * @ref uniformScalingSquared() where possible.
         * @see @ref rotationScaling(), @ref rotation(),
         *      @ref rotationNormalized(), @ref scaling(const Vector2&),
         *      @ref Matrix4::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Right-pointing 2D vector
         *
         * First two elements of first column.
         * @see up(), Vector2::xAxis(), Matrix4::right()
         */
        Vector2<T>& right() { return (*this)[0].xy(); }
        constexpr Vector2<T> right() const { return (*this)[0].xy(); } /**< @overload */

        /**
         * @brief Up-pointing 2D vector
         *
         * First two elements of second column.
         * @see right(), Vector2::yAxis(), Matrix4::up()
         */
        Vector2<T>& up() { return (*this)[1].xy(); }
        constexpr Vector2<T> up() const { return (*this)[1].xy(); } /**< @overload */

        /**
         * @brief 2D translation part of the matrix
         *
         * First two elements of third column.
         * @see from(const Matrix<2, T>&, const Vector2&),
         *      translation(const Vector2&), Matrix4::translation()
         */
        Vector2<T>& translation() { return (*this)[2].xy(); }
        constexpr Vector2<T> translation() const { return (*this)[2].xy(); } /**< @overload */

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * Significantly faster than the general algorithm in inverted().
         * @see isRigidTransformation(), invertedOrthogonal(),
         *      rotationScaling() const, translation() const
         */
        Matrix3<T> invertedRigid() const;

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
        Vector2<T> transformVector(const Vector2<T>& vector) const {
            /* Workaround for GCC 4.4 strict-aliasing fascism */
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return ((*this)*Vector3<T>(vector, T(0))).xy();
            #else
            const auto v = (*this)*Vector3<T>(vector, T(0));
            return v.xy();
            #endif
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
        Vector2<T> transformPoint(const Vector2<T>& vector) const {
            /* Workaround for GCC 4.4 strict-aliasing fascism */
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return ((*this)*Vector3<T>(vector, T(1))).xy();
            #else
            const auto v = (*this)*Vector3<T>(vector, T(1));
            return v.xy();
            #endif
        }

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(3, 3, Matrix3<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(3, Matrix3, Vector3)
};

MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(3, Matrix3)

/** @debugoperator{Magnum::Math::Matrix3} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Matrix3<T>& value) {
    return debug << static_cast<const Matrix<3, T>&>(value);
}

template<class T> Matrix3<T> Matrix3<T>::rotation(const Rad<T> angle) {
    const T sine = std::sin(angle.toUnderlyingType());
    const T cosine = std::cos(angle.toUnderlyingType());

    return {{ cosine,   sine, T(0)},
            {  -sine, cosine, T(0)},
            {   T(0),   T(0), T(1)}};
}

template<class T> inline Matrix3<T> Matrix3<T>::invertedRigid() const {
    CORRADE_ASSERT(isRigidTransformation(),
        "Math::Matrix3::invertedRigid(): the matrix doesn't represent rigid transformation", {});

    Matrix<2, T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix3<T>>: public ConfigurationValue<Magnum::Math::Matrix<3, T>> {};
}}

#endif
