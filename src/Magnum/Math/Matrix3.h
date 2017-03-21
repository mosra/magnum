#ifndef Magnum_Math_Matrix3_h
#define Magnum_Math_Matrix3_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::Matrix3
 */

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math {

/**
@brief 2D transformation matrix
@tparam T   Underlying data type

See @ref matrix-vector and @ref transformations for brief introduction.
@see @ref Magnum::Matrix3, @ref Magnum::Matrix3d, @ref Matrix3x3,
    @ref DualComplex, @ref SceneGraph::MatrixTransformation2D
@configurationvalueref{Magnum::Math::Matrix3}
*/
template<class T> class Matrix3: public Matrix3x3<T> {
    public:
        /**
         * @brief 2D translation matrix
         * @param vector    Translation vector
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 & 0 & v_x \\
         *          0 & 1 & v_y \\
         *          0 & 0 &   1
         *      \end{pmatrix}
         * @f]
         * @see @ref translation() const, @ref DualComplex::translation(),
         *      @ref Matrix4::translation(const Vector3<T>&),
         *      @ref Vector2::xAxis(), @ref Vector2::yAxis()
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
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          v_x &   0 & 0 \\
         *            0 & v_y & 0 \\
         *            0 &   0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotationScaling(),
         *      @ref Matrix4::scaling(const Vector3<T>&),
         *      @ref Vector2::xScale(), @ref Vector2::yScale()
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
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \cos\theta & -\sin\theta & 0 \\
         *          \sin\theta &  \cos\theta & 0 \\
         *                   0 &           0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation() const, @ref Complex::rotation(),
         *      @ref DualComplex::rotation(),
         *      @ref Matrix4::rotation(Rad, const Vector3<T>&)
         */
        static Matrix3<T> rotation(Rad<T> angle);

        /**
         * @brief 2D reflection matrix
         * @param normal    Normal of the line through which to reflect
         *
         * Expects that the normal is normalized. Reflection along axes can be
         * done in a slightly simpler way also using @ref scaling(), e.g.
         * `Matrix3::reflection(Vector2::yAxis())` is equivalent to
         * `Matrix3::scaling(Vector2::yScale(-1.0f))`. @f[
         *      \boldsymbol{A} = \boldsymbol{I} - 2 \boldsymbol{NN}^T ~~~~~ \boldsymbol{N} = \begin{pmatrix} n_x \\ n_y \end{pmatrix}
         * @f]
         * @see @ref Matrix4::reflection(), @ref Vector::isNormalized()
         */
        static Matrix3<T> reflection(const Vector2<T>& normal) {
            CORRADE_ASSERT(normal.isNormalized(),
                           "Math::Matrix3::reflection(): normal must be normalized", {});
            return from(Matrix2x2<T>() - T(2)*normal*RectangularMatrix<1, 2, T>(normal).transposed(), {});
        }

        /**
         * @brief 2D shearing matrix along X axis
         * @param amount    Shearing amount
         *
         * Y axis remains unchanged. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 & v_x & 0 \\
         *          0 &   1 & 0 \\
         *          0 &   0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref shearingY(), @ref Matrix4::shearingXY(),
         *      @ref Matrix4::shearingXZ(), @ref Matrix4::shearingYZ()
         */
        constexpr static Matrix3<T> shearingX(T amount) {
            return {{  T(1), T(0), T(0)},
                    {amount, T(1), T(0)},
                    {  T(0), T(0), T(1)}};
        }

        /**
         * @brief 2D shearing matrix along Y axis
         * @param amount    Shearing amount
         *
         * X axis remains unchanged. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *            1 & 0 & 0 \\
         *          v_y & 1 & 0 \\
         *            0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref shearingX(), @ref Matrix4::shearingXY(),
         *      @ref Matrix4::shearingXZ(), @ref Matrix4::shearingYZ()
         */
        constexpr static Matrix3<T> shearingY(T amount) {
            return {{T(1), amount, T(0)},
                    {T(0),   T(1), T(0)},
                    {T(0),   T(0), T(1)}};
        }

        /**
         * @brief 2D projection matrix
         * @param size      Size of the view
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2}{s_x} & 0 & 0 \\
         *          0 & \frac{2}{s_y} & 0 \\
         *          0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref Matrix4::orthographicProjection(),
         *      @ref Matrix4::perspectiveProjection()
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
         * @see @ref rotationScaling(), @ref translation() const
         */
        constexpr static Matrix3<T> from(const Matrix2x2<T>& rotationScaling, const Vector2<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {       translation, T(1)}};
        }

        /**
         * @brief Default constructor
         *
         * Creates identity matrix. @p value allows you to specify value on
         * diagonal.
         */
        constexpr /*implicit*/ Matrix3(IdentityInitT = IdentityInit, T value = T{1}) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix3x3<T>{IdentityInit, value}
            #endif
            {}

        /** @copydoc Matrix::Matrix(ZeroInitT) */
        constexpr explicit Matrix3(ZeroInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix3x3<T>{ZeroInit}
            #endif
            {}

        /** @copydoc Matrix::Matrix(NoInitT) */
        constexpr explicit Matrix3(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix3x3<T>{NoInit}
            #endif
            {}

        /** @brief Construct matrix from column vectors */
        constexpr /*implicit*/ Matrix3(const Vector3<T>& first, const Vector3<T>& second, const Vector3<T>& third) noexcept: Matrix3x3<T>(first, second, third) {}

        /** @brief Construct matrix with one value for all elements */
        constexpr explicit Matrix3(T value) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix3x3<T>{value}
            #endif
            {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix3(const RectangularMatrix<3, 3, U>& other) noexcept: Matrix3x3<T>(other) {}

        /** @brief Construct matrix from external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(std::declval<U>()))> constexpr explicit Matrix3(const U& other) noexcept: Matrix3x3<T>(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Matrix3(const RectangularMatrix<3, 3, T>& other) noexcept: Matrix3x3<T>(other) {}

        /**
         * @brief Check whether the matrix represents rigid transformation
         *
         * Rigid transformation consists only of rotation and translation (i.e.
         * no scaling or projection).
         * @see @ref isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(2) == Vector3<T>(T(0), T(0), T(1));
        }

        /**
         * @brief 2D rotation and scaling part of the matrix
         *
         * Upper-left 2x2 part of the matrix.
         * @see @ref from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref rotation() const, @ref rotationNormalized(),
         *      @ref uniformScaling(), @ref rotation(Rad<T>),
         *      @ref Matrix4::rotationScaling()
         */
        constexpr Matrix2x2<T> rotationScaling() const {
            return {(*this)[0].xy(),
                    (*this)[1].xy()};
        }

        /**
         * @brief 2D rotation part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotationScaling(), but additionally checks that the
         * base vectors are normalized.
         * @see @ref rotation() const, @ref uniformScaling(),
         *      @ref Matrix4::rotationNormalized()
         * @todo assert also orthogonality or this is good enough?
         */
        Matrix2x2<T> rotationNormalized() const {
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
         * @see @ref rotationNormalized(), @ref rotationScaling(),
         *      @ref uniformScaling(), @ref rotation(Rad<T>),
         *      @ref Matrix4::rotation() const
         */
        Matrix2x2<T> rotation() const {
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
         * @see @ref rotationScaling(), @ref rotation() const,
         *      @ref rotationNormalized(), @ref scaling(const Vector2<T>&),
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
         * @see @ref rotationScaling(), @ref rotation() const,
         *      @ref rotationNormalized(), @ref scaling(const Vector2<T>&),
         *      @ref Matrix4::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Right-pointing 2D vector
         *
         * First two elements of first column.
         * @see @ref up(), @ref Vector2::xAxis(), @ref Matrix4::right()
         */
        Vector2<T>& right() { return (*this)[0].xy(); }
        constexpr Vector2<T> right() const { return (*this)[0].xy(); } /**< @overload */

        /**
         * @brief Up-pointing 2D vector
         *
         * First two elements of second column.
         * @see @ref right(), @ref Vector2::yAxis(), @ref Matrix4::up()
         */
        Vector2<T>& up() { return (*this)[1].xy(); }
        constexpr Vector2<T> up() const { return (*this)[1].xy(); } /**< @overload */

        /**
         * @brief 2D translation part of the matrix
         *
         * First two elements of third column.
         * @see @ref from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref translation(const Vector2<T>&),
         *      @ref Matrix4::translation()
         */
        Vector2<T>& translation() { return (*this)[2].xy(); }
        constexpr Vector2<T> translation() const { return (*this)[2].xy(); } /**< @overload */

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * Significantly faster than the general algorithm in @ref inverted(). @f[
         *      A^{-1} = \begin{pmatrix} (A^{2,2})^T & (A^{2,2})^T \begin{pmatrix} a_{2,0} \\ a_{2,1} \end{pmatrix} \\ \begin{array}{cc} 0 & 0 \end{array} & 1 \end{pmatrix}
         * @f]
         * @f$ A^{i, j} @f$ is matrix without i-th row and j-th column, see
         * @ref ij()
         * @see @ref isRigidTransformation(), @ref invertedOrthogonal(),
         *      @ref rotationScaling(), @ref translation() const,
         *      @ref Matrix4::invertedRigid()
         */
        Matrix3<T> invertedRigid() const;

        /**
         * @brief Transform 2D vector with the matrix
         *
         * Unlike in @ref transformPoint(), translation is not involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ 0 \end{pmatrix}
         * @f]
         * @see @ref Complex::transformVector(),
         *      @ref Matrix4::transformVector()
         * @todo extract 2x2 matrix and multiply directly? (benchmark that)
         */
        Vector2<T> transformVector(const Vector2<T>& vector) const {
            return ((*this)*Vector3<T>(vector, T(0))).xy();
        }

        /**
         * @brief Transform 2D point with the matrix
         *
         * Unlike in @ref transformVector(), translation is also involved in
         * the transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ 1 \end{pmatrix}
         * @f]
         * @see @ref DualComplex::transformPoint(),
         *      @ref Matrix4::transformPoint()
         */
        Vector2<T> transformPoint(const Vector2<T>& vector) const {
            return ((*this)*Vector3<T>(vector, T(1))).xy();
        }

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(3, 3, Matrix3<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(3, Matrix3, Vector3)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(3, Matrix3)
#endif

template<class T> Matrix3<T> Matrix3<T>::rotation(const Rad<T> angle) {
    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));

    return {{ cosine,   sine, T(0)},
            {  -sine, cosine, T(0)},
            {   T(0),   T(0), T(1)}};
}

template<class T> inline Matrix3<T> Matrix3<T>::invertedRigid() const {
    CORRADE_ASSERT(isRigidTransformation(),
        "Math::Matrix3::invertedRigid(): the matrix doesn't represent rigid transformation", {});

    Matrix2x2<T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix3<T>>: public ConfigurationValue<Magnum::Math::Matrix3x3<T>> {};
}}

#endif
