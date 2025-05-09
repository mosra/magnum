#ifndef Magnum_Math_Matrix3_h
#define Magnum_Math_Matrix3_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math {

/**
@brief 2D transformation matrix
@tparam T   Underlying data type

Expands upon a generic @ref Matrix3x3 with functionality for 2D
transformations. A 2D transformation matrix consists of a upper-left 2x2 part
describing a combined scaling, rotation and shear, and the two top-right
components specifying a translation: @f[
    \boldsymbol{T} = \begin{pmatrix}
        \color{m-danger} a_x & \color{m-success} b_x & \color{m-warning} t_x \\
        \color{m-danger} a_y & \color{m-success} b_y & \color{m-warning} t_y \\
        \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
    \end{pmatrix}
@f]

The @f$ \color{m-danger} \boldsymbol{a} @f$ and
@f$ \color{m-success} \boldsymbol{b} @f$ vectors can be also thought of as the
two basis vectors describing the coordinate system the matrix converts to. The
bottom row is always
@f$ \begin{pmatrix} \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1 \end{pmatrix} @f$
as, unlike with @ref Matrix4 in 3D, perspective shortening happening along the
X or Y axis isn't really a thing.

@section Math-Matrix3-usage Usage

See @ref types, @ref matrix-vector and @ref transformations first for an
introduction into using transformation matrices.

While it's possible to create the matrix directly from the components, the
recommended usage is by creating elementary transformation matrices with
@ref translation(const Vector2<T>&) "translation()",
@ref rotation(Rad<T>) "rotation()", @ref scaling(const Vector2<T>&) "scaling()",
@ref reflection(), @ref shearingX(), @ref shearingY(), and @ref projection()
and multiplying them together to form the final transformation --- the
rightmost transformation is applied first, leftmost last:

@snippet Math.cpp Matrix3-usage

Conversely, the transformation parts can be extracted back using the member
@ref rotation() const "rotation()", @ref scaling() const "scaling()" and their
variants, and @ref translation(). The basis vectors can be accessed using
@ref right() and @ref up(). Matrices that combine non-uniform scaling and/or
shear with rotation can't be trivially decomposed back, for these you might
want to consider using @ref Algorithms::qr() or @ref Algorithms::svd().

When a lot of transformations gets composed together over time (for example
with a camera movement), a floating-point drift accumulates, causing the
rotation part to no longer be orthogonal. This can be accounted for using
@ref Algorithms::gramSchmidtOrthonormalizeInPlace() and variants.

@see @ref Magnum::Matrix3, @ref Magnum::Matrix3d, @ref Matrix3x3,
    @ref DualComplex, @ref SceneGraph::MatrixTransformation2D
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
         * @see @ref scaling() const, @ref Matrix4::scaling(const Vector3<T>&),
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
         *      @ref Matrix4::rotation(Rad<T>, const Vector3<T>&)
         */
        static Matrix3<T> rotation(Rad<T> angle);

        /**
         * @brief 2D reflection matrix
         * @param normal    Normal of the line through which to reflect
         *
         * Expects that the normal is normalized. Reflection along axes can be
         * done in a slightly simpler way also using @ref scaling(), e.g.
         * @cpp Matrix3::reflection(Vector2::yAxis()) @ce is equivalent to
         * @cpp Matrix3::scaling(Vector2::yScale(-1.0f)) @ce. @f[
         *      \boldsymbol{A} = \boldsymbol{I} - 2 \boldsymbol{NN}^T ~~~~~ \boldsymbol{N} = \begin{pmatrix} n_x \\ n_y \end{pmatrix}
         * @f]
         * @see @ref Matrix4::reflection(), @ref Vector::isNormalized(),
         *      @ref reflect()
         */
        static Matrix3<T> reflection(const Vector2<T>& normal) {
            CORRADE_DEBUG_ASSERT(normal.isNormalized(),
                "Math::Matrix3::reflection(): normal" << normal << "is not normalized", {});
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
         *
         * If you need an off-center projection (as with the classic
         * @m_class{m-doc-external} [gluOrtho2D()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluOrtho2D.xml)
         * function, use @ref projection(const Vector2<T>&, const Vector2<T>&).
         * @see @ref Matrix4::orthographicProjection(),
         *      @ref Matrix4::perspectiveProjection()
         */
        static Matrix3<T> projection(const Vector2<T>& size) {
            return scaling(T(2.0)/size);
        }

        /**
         * @brief 2D off-center orthographic projection matrix
         * @param bottomLeft    Bottom left corner of the clipping plane
         * @param topRight      Top right corner of the clipping plane
         * @m_since_latest
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2}{r - l} & 0 & - \frac{r + l}{r - l} \\
         *          0 & \frac{2}{t - b} & - \frac{t + b}{t - b} \\
         *          0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         *
         * Equivalent to the classic @m_class{m-doc-external} [gluOrtho2D()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluOrtho2D.xml)
         * function. If @p bottomLeft and @p topRight are a negation of each
         * other, this function is equivalent to
         * @ref projection(const Vector2<T>&).
         *
         * @see @ref Matrix4::orthographicProjection(const Vector2<T>&, const Vector2<T>&, T, T),
         *      @ref Matrix4::perspectiveProjection(const Vector2<T>&, const Vector2<T>&, T, T)
         * @m_keywords{gluOrtho2D()}
         */
        static Matrix3<T> projection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight);

        /**
         * @brief Create a matrix from a rotation/scaling part and a translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 2x2
         *      matrix)
         * @param translation       Translation part (first two elements of
         *      third column)
         *
         * @see @ref rotationScaling(), @ref translation() const,
         *      @ref Matrix4::from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref DualComplex::from(const Complex<T>&, const Vector2<T>&),
         *      @ref DualQuaternion::from(const Quaternion<T>&, const Vector3<T>&)
         */
        constexpr static Matrix3<T> from(const Matrix2x2<T>& rotationScaling, const Vector2<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {       translation, T(1)}};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Matrix3(IdentityInitT, T).
         */
        constexpr /*implicit*/ Matrix3() noexcept: Matrix3x3<T>{IdentityInit, T(1)} {}

        /**
         * @brief Construct an identity matrix
         *
         * The @p value allows you to specify value on diagonal.
         */
        constexpr explicit Matrix3(IdentityInitT, T value = T{1}) noexcept: Matrix3x3<T>{IdentityInit, value} {}

        /** @copydoc Matrix::Matrix(ZeroInitT) */
        constexpr explicit Matrix3(ZeroInitT) noexcept: Matrix3x3<T>{ZeroInit} {}

        /** @copydoc Matrix::Matrix(Magnum::NoInitT) */
        constexpr explicit Matrix3(Magnum::NoInitT) noexcept: Matrix3x3<T>{Magnum::NoInit} {}

        /** @brief Construct from column vectors */
        constexpr /*implicit*/ Matrix3(const Vector3<T>& first, const Vector3<T>& second, const Vector3<T>& third) noexcept: Matrix3x3<T>(first, second, third) {}

        /** @brief Construct with one value for all elements */
        constexpr explicit Matrix3(T value) noexcept: Matrix3x3<T>{value} {}

        /** @copydoc Matrix::Matrix(const T(&)[cols_][rows_]) */
        #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
        template<std::size_t cols_, std::size_t rows_> constexpr explicit Matrix3(const T(&data)[cols_][rows_]) noexcept: Matrix3x3<T>{data} {}
        #else
        /* GCC 4.8 workaround, see the RectangularMatrix base for details */
        constexpr explicit Matrix3(const T(&data)[3][3]) noexcept: Matrix3x3<T>{data} {}
        #endif

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix3(const RectangularMatrix<3, 3, U>& other) noexcept: Matrix3x3<T>(other) {}

        /** @brief Construct a matrix from external representation */
        template<class U, class = decltype(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(std::declval<U>()))> constexpr explicit Matrix3(const U& other) noexcept: Matrix3x3<T>(Implementation::RectangularMatrixConverter<3, 3, T, U>::from(other)) {}

        /** @copydoc RectangularMatrix::RectangularMatrix(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>&, T) */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix3(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>& other, T value = T(1)) noexcept: Matrix3x3<T>{IdentityInit, other, value} {}

        /** @copydoc RectangularMatrix::RectangularMatrix(ZeroInitT, const RectangularMatrix<otherCols, otherRows, T>&) */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix3(ZeroInitT, const RectangularMatrix<otherCols, otherRows, T>& other) noexcept: Matrix3x3<T>{ZeroInit, other} {}

        /**
         * @brief Construct by slicing or expanding a matrix of different size
         * @m_since_latest
         *
         * Equivalent to @ref Matrix3(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>&, T).
         * Note that this default is different from @ref RectangularMatrix,
         * where it's equivalent to the @ref ZeroInit variant instead.
         */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix3(const RectangularMatrix<otherCols, otherRows, T>& other, T value = T(1)) noexcept: Matrix3x3<T>{IdentityInit, other, value} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Matrix3(const RectangularMatrix<3, 3, T>& other) noexcept: Matrix3x3<T>(other) {}

        /**
         * @brief Check whether the matrix represents a rigid transformation
         *
         * A [rigid transformation](https://en.wikipedia.org/wiki/Rigid_transformation)
         * consists only of rotation, reflection and translation (i.e., no
         * scaling, skew or projection).
         * @see @ref isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(2) == Vector3<T>(T(0), T(0), T(1));
        }

        /**
         * @brief 2D rotation and scaling part of the matrix
         *
         * Unchanged upper-left 2x2 part of the matrix. @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & \color{m-success} b_x & t_x \\
         *          \color{m-danger} a_y & \color{m-success} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * Note that an arbitrary combination of rotation and scaling can also
         * represent shear and reflection. Especially when non-uniform scaling
         * is involved, decomposition of the result into primary linear
         * transformations may have multiple equivalent solutions. See
         * @ref rotation() const, @ref Algorithms::svd() and
         * @ref Algorithms::qr() for further info. See also
         * @ref rotationShear() and @ref scaling() const for extracting further
         * properties.
         *
         * @see @ref from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref rotation(Rad<T>), @ref Matrix4::rotationScaling()
         */
        constexpr Matrix2x2<T> rotationScaling() const {
            return {(*this)[0].xy(),
                    (*this)[1].xy()};
        }

        /**
         * @brief 2D rotation, reflection and shear part of the matrix
         *
         * Normalized upper-left 2x2 part of the matrix. Assuming the following
         * matrix, with the upper-left 2x2 part represented by column vectors
         * @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & \color{m-success} b_x & t_x \\
         *          \color{m-danger} a_y & \color{m-success} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|}
         *      \end{pmatrix}
         * @f]
         *
         * This function is a counterpart to @ref rotation() const that does
         * not require orthogonal input. See also @ref rotationScaling() and
         * @ref scaling() const for extracting other properties. The
         * @ref Algorithms::svd() and @ref Algorithms::qr() can be used to
         * separate the rotation / shear components; see @ref rotation() const
         * for an example of decomposing a rotation + reflection matrix into a
         * pure rotation and signed scaling.
         *
         * @see @ref from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref rotation(Rad), @ref Matrix4::rotationShear() const
         */
        Matrix2x2<T> rotationShear() const {
            return {(*this)[0].xy().normalized(),
                    (*this)[1].xy().normalized()};
        }

        /**
         * @brief 2D rotation and reflection part of the matrix
         *
         * Normalized upper-left 2x2 part of the matrix. Expects that the
         * normalized part is orthogonal. Assuming the following matrix, with
         * the upper-left 2x2 part represented by column vectors
         * @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|}
         *      \end{pmatrix}
         * @f]
         *
         * This function is equivalent to @ref rotationShear() but with the
         * added orthogonality requirement. See also @ref rotationScaling() and
         * @ref scaling() const for extracting other properties.
         *
         * There's usually several solutions for decomposing the matrix into a
         * rotation @f$ \boldsymbol{R} @f$ and a scaling @f$ \boldsymbol{S} @f$
         * that satisfy @f$ \boldsymbol{R} \boldsymbol{S} = \boldsymbol{M} @f$.
         * One possibility that gives you always a pure rotation matrix without
         * reflections (which can then be fed to @ref Complex::fromMatrix(),
         * for example) is to flip an arbitrary column of the 2x2 part if its
         * @ref determinant() is negative, and apply the sign flip to the
         * corresponding scaling component instead:
         *
         * @snippet Math.cpp Matrix3-rotation-extract-reflection
         *
         * @note Extracting rotation part of a matrix with this function may
         *      cause assertions in case you have unsanitized input (for
         *      example a model transformation loaded from an external source)
         *      or when you accumulate many transformations together (for
         *      example when controlling a FPS camera). To mitigate this,
         *      either first reorthogonalize the matrix using
         *      @ref Algorithms::gramSchmidtOrthogonalize(), decompose it to
         *      basic linear transformations using @ref Algorithms::svd() or
         *      @ref Algorithms::qr() or use a different transformation
         *      representation that suffers less floating point error and can
         *      be easier renormalized such as @ref DualComplex. Another
         *      possibility is to ignore the error and extract combined
         *      rotation and scaling / shear with @ref rotationScaling() /
         *      @ref rotationShear().
         *
         * @see @ref rotationNormalized(), @ref scaling() const,
         *      @ref rotation(Rad<T>), @ref Matrix4::rotation() const
         */
        Matrix2x2<T> rotation() const;

        /**
         * @brief 2D rotation and reflection part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotation() const, but expects that the rotation part
         * is orthogonal, saving the extra renormalization. Assuming the
         * following matrix, with the upper-left 2x2 part represented by column
         * vectors @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & \color{m-success} b_x & t_x \\
         *          \color{m-danger} a_y & \color{m-success} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|}
         *      \end{pmatrix} = \begin{pmatrix}
         *          \boldsymbol{a} &
         *          \boldsymbol{b}
         *      \end{pmatrix}
         * @f]
         *
         * In particular, for an orthogonal matrix, @ref rotationScaling(),
         * @ref rotationShear(), @ref rotation() const and
         * @ref rotationNormalized() all return the same value.
         *
         * @see @ref isOrthogonal(), @ref uniformScaling(),
         *      @ref Matrix4::rotationNormalized()
         */
        Matrix2x2<T> rotationNormalized() const;

        /**
         * @brief Non-uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 2x2 part of the matrix.
         * Faster alternative to @ref scaling() const, because it doesn't
         * calculate the square root. Assuming the following matrix, with the
         * upper-left 2x2 part represented by column vectors
         * @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting scaling vector, squared, is: @f[
         *      \boldsymbol{s}^2 = \begin{pmatrix}
         *          \boldsymbol{a} \cdot \boldsymbol{a} \\
         *          \boldsymbol{b} \cdot \boldsymbol{b}
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref scaling() const, @ref uniformScalingSquared(),
         *      @ref rotation() const, @ref Matrix4::scalingSquared()
         */
        Vector2<T> scalingSquared() const {
            return {(*this)[0].xy().dot(),
                    (*this)[1].xy().dot()};
        }

        /**
         * @brief Non-uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 2x2 part of the matrix. Use the
         * faster alternative @ref scalingSquared() where possible. Assuming
         * the following matrix, with the upper-left 2x2 part represented by
         * column vectors @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$:
         * @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting scaling vector is: @f[
         *      \boldsymbol{s} = \begin{pmatrix}
         *          | \boldsymbol{a} | \\
         *          | \boldsymbol{b} |
         *      \end{pmatrix}
         * @f]
         *
         * Note that the returned vector is sign-less and the signs are instead
         * contained in @ref rotation() const / @ref rotationShear() const,
         * meaning these contain rotation together with a potential reflection.
         * See @ref rotation() const for an example of decomposing a rotation +
         * reflection matrix into a pure rotation and signed scaling.
         * @see @ref scalingSquared(), @ref uniformScaling(),
         *      @ref rotation() const, @ref Matrix4::scaling() const
         */
        Vector2<T> scaling() const {
            return {(*this)[0].xy().length(),
                    (*this)[1].xy().length()};
        }

        /**
         * @brief Uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 2x2 part of the matrix.
         * Expects that the scaling is the same in all axes. Faster alternative
         * to @ref uniformScaling(), because it doesn't compute the square
         * root. Assuming the following matrix, with the upper-left 2x2 part
         * represented by column vectors @f$ \boldsymbol{a} @f$ and
         * @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting uniform scaling, squared, is: @f[
         *      s^2 = \boldsymbol{a} \cdot \boldsymbol{a}
         *          = \boldsymbol{b} \cdot \boldsymbol{b}
         * @f]
         *
         * @note Extracting uniform scaling of a matrix this way may cause
         *      assertions in case you have unsanitized input (for example a
         *      model transformation loaded from an external source) or when
         *      you accumulate many transformations together (for example when
         *      controlling a FPS camera). To mitigate this, either first
         *      reorthogonalize the matrix using
         *      @ref Algorithms::gramSchmidtOrthogonalize(), decompose it to
         *      basic linear transformations using @ref Algorithms::svd() or
         *      @ref Algorithms::qr() or extract a non-uniform scaling using
         *      @ref scalingSquared().
         *
         * @see @ref rotation() const, @ref scaling() const,
         *      @ref scaling(const Vector2<T>&),
         *      @ref Matrix4::uniformScalingSquared()
         */
        T uniformScalingSquared() const;

        /**
         * @brief Uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 2x2 part of the matrix. Expects that
         * the scaling is the same in all axes. Use faster alternative
         * @ref uniformScalingSquared() where possible. Assuming the following
         * matrix, with the upper-left 3x3 part represented by column vectors
         * @f$ \boldsymbol{a} @f$ and  @f$ \boldsymbol{b} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting uniform scaling is: @f[
         *      s = | \boldsymbol{a} | = | \boldsymbol{b} |
         * @f]
         *
         * @note Extracting uniform scaling of a matrix this way may cause
         *      assertions in case you have unsanitized input (for example a
         *      model transformation loaded from an external source) or when
         *      you accumulate many transformations together (for example when
         *      controlling a FPS camera). To mitigate this, either first
         *      reorthogonalize the matrix using
         *      @ref Algorithms::gramSchmidtOrthogonalize(), decompose it to
         *      basic linear transformations using @ref Algorithms::svd() or
         *      @ref Algorithms::qr() or extract a non-uniform scaling using
         *      @ref scaling() const.
         *
         * @see @ref rotation() const, @ref scalingSquared() const,
         *      @ref scaling(const Vector2<T>&), @ref Matrix4::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Right-pointing 2D vector
         *
         * First two elements of first column. @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & b_x & t_x \\
         *          \color{m-danger} a_y & b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref up(), @ref Vector2::xAxis(), @ref Matrix4::right()
         */
        Vector2<T>& right() { return (*this)[0].xy(); }
        constexpr Vector2<T> right() const { return (*this)[0].xy(); } /**< @overload */

        /**
         * @brief Up-pointing 2D vector
         *
         * First two elements of second column. @f[
         *      \begin{pmatrix}
         *          a_x & \color{m-success} b_x & t_x \\
         *          a_y & \color{m-success} b_y & t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref right(), @ref Vector2::yAxis(), @ref Matrix4::up()
         */
        Vector2<T>& up() { return (*this)[1].xy(); }
        constexpr Vector2<T> up() const { return (*this)[1].xy(); } /**< @overload */

        /**
         * @brief 2D translation part of the matrix
         *
         * First two elements of third column. @f[
         *      \begin{pmatrix}
         *          a_x & b_x & \color{m-warning} t_x \\
         *          a_y & b_y & \color{m-warning} t_y \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref translation(const Vector2<T>&),
         *      @ref Matrix4::translation()
         */
        Vector2<T>& translation() { return (*this)[2].xy(); }
        constexpr Vector2<T> translation() const { return (*this)[2].xy(); } /**< @overload */

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents a [rigid transformation](https://en.wikipedia.org/wiki/Rigid_transformation)
         * (i.e., no scaling, skew or projection). Significantly faster than
         * the general algorithm in @ref inverted(). @f[
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
         * @brief Transform a 2D vector with the matrix
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
         * @brief Transform a 2D point with the matrix
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

        _MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(3, 3, Matrix3<T>)
        _MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(3, Matrix3, Vector3)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
_MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(3, Matrix3)
#endif

template<class T> Matrix3<T> Matrix3<T>::rotation(const Rad<T> angle) {
    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));

    return {{ cosine,   sine, T(0)},
            {  -sine, cosine, T(0)},
            {   T(0),   T(0), T(1)}};
}

template<class T> Matrix3<T> Matrix3<T>::projection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight) {
    const Vector2<T> difference = topRight - bottomLeft;
    const Vector2<T> scale = T(2.0)/difference;
    const Vector2<T> offset = (topRight + bottomLeft)/difference;

    return {{  scale.x(),        T(0), T(0)},
            {       T(0),   scale.y(), T(0)},
            {-offset.x(), -offset.y(), T(1)}};
}

template<class T> Matrix2x2<T> Matrix3<T>::rotation() const {
    Matrix2x2<T> rotationShear = this->rotationShear();
    CORRADE_DEBUG_ASSERT(rotationShear.isOrthogonal(),
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:" << Debug::newline << rotationShear, {});
    return rotationShear;
}

template<class T> Matrix2x2<T> Matrix3<T>::rotationNormalized() const {
    Matrix2x2<T> rotationScaling = this->rotationScaling();
    CORRADE_DEBUG_ASSERT(rotationScaling.isOrthogonal(),
        "Math::Matrix3::rotationNormalized(): the rotation part is not orthogonal:" << Debug::newline << rotationScaling, {});
    return rotationScaling;
}

template<class T> T Matrix3<T>::uniformScalingSquared() const {
    const T scalingSquared = (*this)[0].xy().dot();
    CORRADE_DEBUG_ASSERT(TypeTraits<T>::equals((*this)[1].xy().dot(), scalingSquared),
        "Math::Matrix3::uniformScaling(): the matrix doesn't have uniform scaling:" << Debug::newline << rotationScaling(), {});
    return scalingSquared;
}

template<class T> inline Matrix3<T> Matrix3<T>::invertedRigid() const {
    CORRADE_DEBUG_ASSERT(isRigidTransformation(),
        "Math::Matrix3::invertedRigid(): the matrix doesn't represent a rigid transformation:" << Debug::newline << *this, {});

    Matrix2x2<T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
namespace Implementation {
    template<class T> struct StrictWeakOrdering<Matrix3<T>>: StrictWeakOrdering<RectangularMatrix<3, 3, T>> {};
}
#endif

}}

#endif
