#ifndef Magnum_Math_Matrix4_h
#define Magnum_Math_Matrix4_h
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
 * @brief Class @ref Magnum::Math::Matrix4
 */

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"

#ifdef CORRADE_TARGET_WINDOWS /* I so HATE windef.h */
#undef near
#undef far
#endif

namespace Magnum { namespace Math {

/**
@brief 3D transformation matrix
@tparam T   Underlying data type

Expands upon a generic @ref Matrix4x4 with functionality for 3D
transformations. A 3D transformation matrix consists of a upper-left 3x3 part
describing a combined scaling, rotation and shear, and the three top-right
components specifying a translation: @f[
    \boldsymbol{T} = \begin{pmatrix}
        \color{m-danger} a_x & \color{m-success} b_x & \color{m-info} c_x & \color{m-warning} t_x \\
        \color{m-danger} a_y & \color{m-success} b_y & \color{m-info} c_y & \color{m-warning} t_y \\
        \color{m-danger} a_z & \color{m-success} b_z & \color{m-info} c_z &  \color{m-warning} t_z \\
        \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
    \end{pmatrix}
@f]

The @f$ \color{m-danger} \boldsymbol{a} @f$,
@f$ \color{m-success} \boldsymbol{b} @f$ and @f$ \color{m-info} \boldsymbol{c} @f$
vectors can be also thought of as the three basis vectors describing the
coordinate system the matrix converts to. In case of an affine transformation,
the bottom row is always
@f$ \begin{pmatrix} \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1 \end{pmatrix} @f$. A (pure) 3D perspective projection matrix, however,
can look for example like this: @f[
    \boldsymbol{P} = \begin{pmatrix}
        \color{m-danger} s_x & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 \\
        \color{m-dim} 0 & \color{m-success} s_y & \color{m-dim} 0 & \color{m-dim} 0 \\
        \color{m-dim} 0 & \color{m-dim} 0 & \color{m-info} s_z & \color{m-warning} t_z \\
        \color{m-primary} 0 & \color{m-primary} 0 & \color{m-primary} -1 & \color{m-primary} 0
    \end{pmatrix}
@f]

The bottom row having the non-zero value in the third column instead of the
fourth is, simply put, what makes perspective shortening happening along the
Z axis. While perspective shortening along X or Y is *technically* also
possible, it doesn't really have a common use, neither it is a thing in case of
a 2D transformation with @ref Matrix3.

@section Math-Matrix4-usage Usage

See @ref types, @ref matrix-vector and @ref transformations first for an
introduction into using transformation matrices.

While it's possible to create the matrix directly from the components, the
recommended usage is by creating elementary transformation matrices with
@ref translation(const Vector3<T>&) "translation()",
@ref rotation(Rad<T>, const Vector3<T>&) "rotation()" and variants,
@ref scaling(const Vector3<T>&) "scaling()", @ref reflection(),
@ref shearingXY() and variants, @ref lookAt() and @ref orthographicProjection()
/ @ref perspectiveProjection() and multiplying them together to form the final
transformation --- the rightmost transformation is applied first, leftmost
last:

@snippet Math.cpp Matrix4-usage

Conversely, the transformation parts can be extracted back using the member
@ref rotation() const "rotation()", @ref scaling() const "scaling()" and their
variants, and @ref translation(). The basis vectors can be accessed using
@ref right(), @ref up() and @ref backward(). Matrices that combine non-uniform
scaling and/or shear with rotation can't be trivially decomposed back, for
these you might want to consider using @ref Algorithms::qr() or
@ref Algorithms::svd().

When a lot of transformations gets composed together over time (for example
with a camera movement), a floating-point drift accumulates, causing the
rotation part to no longer be orthogonal. This can be accounted for using
@ref Algorithms::gramSchmidtOrthonormalizeInPlace() and variants.

@see @ref Magnum::Matrix4, @ref Magnum::Matrix4d, @ref Matrix4x4,
    @ref DualQuaternion, @ref SceneGraph::MatrixTransformation3D
 */
template<class T> class Matrix4: public Matrix4x4<T> {
    public:
        /**
         * @brief 3D translation matrix
         * @param vector    Translation vector
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 & 0 & 0 & v_x \\
         *          0 & 1 & 0 & v_y \\
         *          0 & 0 & 1 & v_z \\
         *          0 & 0 & 0 &   1
         *      \end{pmatrix}
         * @f]
         * @see @ref translation() const, @ref DualQuaternion::translation(),
         *      @ref Matrix3::translation(const Vector2<T>&),
         *      @ref Vector3::xAxis(), @ref Vector3::yAxis(),
         *      @ref Vector3::zAxis()
         */
        constexpr static Matrix4<T> translation(const Vector3<T>& vector) {
            return {{      T(1),       T(0),       T(0), T(0)},
                    {      T(0),       T(1),       T(0), T(0)},
                    {      T(0),       T(0),       T(1), T(0)},
                    {vector.x(), vector.y(), vector.z(), T(1)}};
        }

        /**
         * @brief 3D scaling matrix
         * @param vector    Scaling vector
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          v_x &   0 &   0 & 0 \\
         *            0 & v_y &   0 & 0 \\
         *            0 &   0 & v_z & 0 \\
         *            0 &   0 &   0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref scaling() const, @ref Matrix3::scaling(const Vector2<T>&),
         *      @ref Vector3::xScale(), @ref Vector3::yScale(),
         *      @ref Vector3::zScale()
         */
        constexpr static Matrix4<T> scaling(const Vector3<T>& vector) {
            return {{vector.x(),       T(0),       T(0), T(0)},
                    {      T(0), vector.y(),       T(0), T(0)},
                    {      T(0),       T(0), vector.z(), T(0)},
                    {      T(0),       T(0),       T(0), T(1)}};
        }

        /**
         * @brief 3D rotation matrix around arbitrary axis
         * @param angle             Rotation angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. If possible, use
         * faster alternatives like @ref rotationX(), @ref rotationY() and
         * @ref rotationZ(). @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          v_{x}v_{x}(1 - \cos\theta) + \cos\theta & v_{y}v_{x}(1 - \cos\theta) - v_{z}\sin \theta & v_{z}v_{x}(1 - \cos\theta) + v_{y}\sin\theta & 0 \\
         *          v_{x}v_{y}(1 - \cos\theta) + v_{z}\sin\theta & v_{y}v_{y}(1 - \cos\theta) + \cos\theta & v_{z}v_{y}(1 - \cos\theta) - v_{x}\sin\theta & 0 \\
         *          v_{x}v_{z}(1 - \cos\theta) - v_{y}\sin\theta & v_{y}v_{z}(1 - \cos\theta)+v_{x}\sin\theta & v_{z}v_{z}(1 - \cos\theta) + \cos\theta & 0 \\
         *          0 & 0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation() const, @ref Quaternion::rotation(),
         *      @ref DualQuaternion::rotation(), @ref Matrix3::rotation(Rad<T>),
         *      @ref Vector3::xAxis(), @ref Vector3::yAxis(),
         *      @ref Vector3::zAxis(), @ref Vector::isNormalized()
         */
        static Matrix4<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis);

        /**
         * @brief 3D rotation matrix around the X axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling @cpp Matrix4::rotation(angle, Vector3::xAxis()) @ce. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 &          0 &           0 & 0 \\
         *          0 & \cos\theta & -\sin\theta & 0 \\
         *          0 & \sin\theta &  \cos\theta & 0 \\
         *          0 &          0 &           0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad<T>, const Vector3<T>&), @ref rotationY(),
         *      @ref rotationZ(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad<T>)
         */
        static Matrix4<T> rotationX(Rad<T> angle);

        /**
         * @brief 3D rotation matrix around the Y axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling @cpp Matrix4::rotation(angle, Vector3::yAxis()) @ce. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *           \cos\theta & 0 & \sin\theta & 0 \\
         *                    0 & 1 &          0 & 0 \\
         *          -\sin\theta & 0 & \cos\theta & 0 \\
         *                    0 & 0 &          0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad<T>, const Vector3<T>&), @ref rotationX(),
         *      @ref rotationZ(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad<T>)
         */
        static Matrix4<T> rotationY(Rad<T> angle);

        /**
         * @brief 3D rotation matrix around the Z axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling @cpp Matrix4::rotation(angle, Vector3::zAxis()) @ce. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \cos\theta & -\sin\theta & 0 & 0 \\
         *          \sin\theta &  \cos\theta & 0 & 0 \\
         *                   0 &           0 & 1 & 0 \\
         *                   0 &           0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad<T>, const Vector3<T>&), @ref rotationX(),
         *      @ref rotationY(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad<T>)
         */
        static Matrix4<T> rotationZ(Rad<T> angle);

        /**
         * @brief 3D reflection matrix
         * @param normal    Normal of the plane through which to reflect
         *
         * Expects that the normal is normalized. Reflection along axes can be
         * done in a slightly simpler way also using @ref scaling(), e.g.
         * @cpp Matrix4::reflection(Vector3::yAxis()) @ce is equivalent to
         * @cpp Matrix4::scaling(Vector3::yScale(-1.0f)) @ce. @f[
         *      \boldsymbol{A} = \boldsymbol{I} - 2 \boldsymbol{NN}^T ~~~~~ \boldsymbol{N} = \begin{pmatrix} n_x \\ n_y \\ n_z \end{pmatrix}
         * @f]
         * @see @ref Quaternion::reflection(), @ref Matrix3::reflection(),
         *      @ref Vector::isNormalized(), @ref reflect()
         */
        static Matrix4<T> reflection(const Vector3<T>& normal);

        /**
         * @brief 3D shearing matrix along the XY plane
         * @param amountX   Amount of shearing along the X axis
         * @param amountY   Amount of shearing along the Y axis
         *
         * Z axis remains unchanged. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 & 0 & v_x & 0 \\
         *          0 & 1 & v_y & 0 \\
         *          0 & 0 &   1 & 0 \\
         *          0 & 0 &   0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref shearingXZ(), @ref shearingYZ(), @ref Matrix3::shearingX(),
         *      @ref Matrix3::shearingY()
         */
        constexpr static Matrix4<T> shearingXY(T amountX, T amountY) {
            return {{    (1),    T(0), T(0), T(0)},
                    {    (0),    T(1), T(0), T(0)},
                    {amountX, amountY, T(1), T(0)},
                    {    (0),    T(0), T(0), T(1)}};
        }

        /**
         * @brief 3D shearing matrix along the XZ plane
         * @param amountX   Amount of shearing along the X axis
         * @param amountZ   Amount of shearing along the Z axis
         *
         * Y axis remains unchanged. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 & v_x & 0 & 0 \\
         *          0 &   1 & 0 & 0 \\
         *          0 & v_z & 1 & 0 \\
         *          0 &   0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref shearingXY(), @ref shearingYZ(), @ref Matrix3::shearingX(),
         *      @ref Matrix3::shearingY()
         */
        constexpr static Matrix4<T> shearingXZ(T amountX, T amountZ) {
            return {{   T(1), T(0),    T(0), T(0)},
                    {amountX, T(1), amountZ, T(0)},
                    {   T(0), T(0),    T(1), T(0)},
                    {   T(0), T(0),    T(0), T(1)}};
        }

        /**
         * @brief 3D shearing matrix along the YZ plane
         * @param amountY   Amount of shearing along the Y axis
         * @param amountZ   Amount of shearing along the Z axis
         *
         * X axis remains unchanged. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *            1 & 0 & 0 & 0 \\
         *          v_y & 1 & 0 & 0 \\
         *          v_z & 0 & 1 & 0 \\
         *            0 & 0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref shearingXY(), @ref shearingXZ(), @ref Matrix3::shearingX(),
         *      @ref Matrix3::shearingY()
         */
        constexpr static Matrix4<T> shearingYZ(T amountY, T amountZ) {
            return {{T(1), amountY, amountZ, T(0)},
                    {T(0),    T(1),    T(0), T(0)},
                    {T(0),    T(0),    T(1), T(0)},
                    {T(0),    T(0),    T(0), T(1)}};
        }

        /**
         * @brief 3D orthographic projection matrix
         * @param size      Size of the view
         * @param near      Distance to near clipping plane, positive is ahead
         * @param far       Distance to far clipping plane, positive is ahead
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2}{s_x} & 0 & 0 & 0 \\
         *          0 & \frac{2}{s_y} & 0 & 0 \\
         *          0 & 0 & \frac{2}{n - f} & \frac{n + f}{n - f} \\
         *          0 & 0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         *
         * If you need an off-center projection (as with the classic
         * @m_class{m-doc-external} [glOrtho()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml)
         * function), use @ref orthographicProjection(const Vector2<T>&, const Vector2<T>&, T, T).
         * @see @ref perspectiveProjection(),
         *      @ref orthographicProjectionNear() const,
         *      @ref orthographicProjectionFar() const,
         *      @ref Matrix3::projection()
         */
        static Matrix4<T> orthographicProjection(const Vector2<T>& size, T near, T far);

        /**
         * @brief 3D off-center orthographic projection matrix
         * @param bottomLeft    Bottom left corner of the clipping plane
         * @param topRight      Top right corner of the clipping plane
         * @param near          Distance to near clipping plane, positive is
         *      ahead
         * @param far           Distance to far clipping plane, positive is
         *      ahead
         * @m_since_latest
         *
         * @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2}{r - l} & 0 & 0 & - \frac{r + l}{r - l} \\
         *          0 & \frac{2}{t - b} & 0 & - \frac{t + b}{t - b} \\
         *          0 & 0 & \frac{2}{n - f} & \frac{n + f}{n - f} \\
         *          0 & 0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         *
         * Equivalent to the classic @m_class{m-doc-external} [glOrtho()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml)
         * function. If @p bottomLeft and @p topRight are a negation of each
         * other, this function is equivalent to
         * @ref orthographicProjection(const Vector2<T>&, T, T).
         *
         * @see @ref perspectiveProjection(),
         *      @ref orthographicProjectionNear() const,
         *      @ref orthographicProjectionFar() const,
         *      @ref Matrix3::projection(const Vector2<T>&, const Vector2<T>&)
         * @m_keywords{glOrtho()}
         */
        static Matrix4<T> orthographicProjection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight, T near, T far);

        /**
         * @brief 3D perspective projection matrix
         * @param size      Size of near clipping plane
         * @param near      Distance to near clipping plane, positive is ahead
         * @param far       Distance to far clipping plane, positive is ahead
         *
         * If @p far is finite, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2n}{s_x} & 0 & 0 & 0 \\
         *          0 & \frac{2n}{s_y} & 0 & 0 \\
         *          0 & 0 & \frac{n + f}{n - f} & \frac{2nf}{n - f} \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * For infinite @p far, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2n}{s_x} & 0 & 0 & 0 \\
         *          0 & \frac{2n}{s_y} & 0 & 0 \\
         *          0 & 0 & -1 & -2n \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * If you need an off-center projection, use
         * @ref perspectiveProjection(const Vector2<T>&, const Vector2<T>&, T, T)
         * instead.
         * @see @ref perspectiveProjection(Rad<T>, T, T, T),
         *      @ref orthographicProjection(),
         *      @ref perspectiveProjectionNear() const,
         *      @ref perspectiveProjectionFar() const,
         *      @ref Matrix3::projection(), @ref Constants::inf()
         */
        static Matrix4<T> perspectiveProjection(const Vector2<T>& size, T near, T far);

        /**
         * @brief 3D perspective projection matrix
         * @param fov           Horizontal field of view angle @f$ \theta @f$
         * @param aspectRatio   Horizontal:vertical aspect ratio @f$ a @f$
         * @param near          Near clipping plane @f$ n @f$
         * @param far           Far clipping plane @f$ f @f$
         *
         * If @p far is finite, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{1}{\tan \left(\frac{\theta}{2} \right)} & 0 & 0 & 0 \\
         *          0 & \frac{a}{\tan \left(\frac{\theta}{2} \right)} & 0 & 0 \\
         *          0 & 0 & \frac{n + f}{n - f} & \frac{2nf}{n - f} \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * For infinite @p far, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{1}{\tan \left( \frac{\theta}{2} \right) } & 0 & 0 & 0 \\
         *          0 & \frac{a}{\tan \left( \frac{\theta}{2} \right) } & 0 & 0 \\
         *          0 & 0 & -1 & -2n \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * This function is equivalent to calling
         * @ref perspectiveProjection(const Vector2<T>&, T, T) with the
         * @p size parameter calculated as @f[
         *      \boldsymbol{s} = 2 n \tan \left(\tfrac{\theta}{2} \right)
         *      \begin{pmatrix}
         *          1 \\
         *          \frac{1}{a}
         *      \end{pmatrix}
         * @f]
         *
         * This function is similar to the classic @m_class{m-doc-external} [gluPerspective()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml),
         * with the difference that @p fov is *horizontal* instead of vertical.
         * If you need an off-center projection (as with the classic
         * @m_class{m-doc-external} [glFrustum()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml)
         * function), use @ref perspectiveProjection(const Vector2<T>&, const Vector2<T>&, T, T).
         * @see @ref orthographicProjection(),
         *      @ref perspectiveProjectionNear() const,
         *      @ref perspectiveProjectionFar() const,
         *      @ref Matrix3::projection(), @ref Constants::inf()
         * @m_keywords{gluPerspective()}
         */
        static Matrix4<T> perspectiveProjection(Rad<T> fov, T aspectRatio, T near, T far) {
            return perspectiveProjection(T(2)*near*std::tan(T(fov)*T(0.5))*Vector2<T>::yScale(T(1)/aspectRatio), near, far);
        }

        /**
         * @brief 3D off-center perspective projection matrix
         * @param bottomLeft    Bottom left corner of the near clipping plane
         * @param topRight      Top right corner of the near clipping plane
         * @param near          Distance to near clipping plane, positive is
         *      ahead
         * @param far           Distance to far clipping plane, positive is
         *      ahead
         * @m_since{2019,10}
         *
         * If @p far is finite, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2n}{r - l} & 0 & \frac{r + l}{r - l} & 0 \\
         *          0 & \frac{2n}{t - b} & \frac{t + b}{t - b} & 0 \\
         *          0 & 0 & \frac{n + f}{n - f} & \frac{2nf}{n - f} \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * For infinite @p far, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{2n}{r - l} & 0 & \frac{r + l}{r - l} & 0 \\
         *          0 & \frac{2n}{t - b} & \frac{t + b}{t - b} & 0 \\
         *          0 & 0 & -1 & -2n \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * Equivalent to the classic @m_class{m-doc-external} [glFrustum()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml)
         * function. If @p bottomLeft and @p topRight are a negation of each
         * other, this function is equivalent to
         * @ref perspectiveProjection(const Vector2<T>&, T, T).
         *
         * @see @ref perspectiveProjection(Rad<T> fov, T, T, T),
         *      @ref orthographicProjection(const Vector2<T>&, const Vector2<T>&, T, T),
         *      @ref perspectiveProjectionNear() const,
         *      @ref perspectiveProjectionFar() const,
         *      @ref Matrix3::projection(), @ref Constants::inf()
         * @m_keywords{glFrustum()}
         */
        static Matrix4<T> perspectiveProjection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight, T near, T far);

        /**
         * @brief Matrix oriented towards a specific point
         * @param eye       Location to place the matrix
         * @param target    Location towards which the matrix is oriented
         * @param up        Vector as a guide of which way is up (should not be
         *      the same direction as @cpp target - eye @ce)
         *
         * @attention This function transforms an object so it's at @p eye
         *      position and oriented towards @p target, it does *not* produce
         *      a camera matrix. If you want to get the same what equivalent
         *      call to the well-known @m_class{m-doc-external} [gluLookAt()](https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml)
         *      would produce, invert the result using @ref invertedRigid().
         * @m_keywords{gluLookAt()}
         */
        static Matrix4<T> lookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up);

        /**
         * @brief Create a matrix from a rotation/scaling part and a translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 3x3
         *      matrix)
         * @param translation       Translation part (first three elements of
         *      fourth column)
         *
         * @see @ref rotationScaling(), @ref translation() const,
         *      @ref Matrix3::from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref DualComplex::from(const Complex<T>&, const Vector2<T>&),
         *      @ref DualQuaternion::from(const Quaternion<T>&, const Vector3<T>&)
         */
        constexpr static Matrix4<T> from(const Matrix3x3<T>& rotationScaling, const Vector3<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {rotationScaling[2], T(0)},
                    {       translation, T(1)}};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Matrix4(IdentityInitT, T).
         */
        constexpr /*implicit*/ Matrix4() noexcept: Matrix4x4<T>{IdentityInit, T(1)} {}

        /**
         * @brief Construct an identity matrix
         *
         * The @p value allows you to specify value on diagonal.
         */
        constexpr explicit Matrix4(IdentityInitT, T value = T{1}) noexcept: Matrix4x4<T>{IdentityInit, value} {}

        /** @copydoc Matrix::Matrix(ZeroInitT) */
        constexpr explicit Matrix4(ZeroInitT) noexcept: Matrix4x4<T>{ZeroInit} {}

        /** @copydoc Matrix::Matrix(Magnum::NoInitT) */
        constexpr explicit Matrix4(Magnum::NoInitT) noexcept: Matrix4x4<T>{Magnum::NoInit} {}

        /** @brief Construct from column vectors */
        constexpr /*implicit*/ Matrix4(const Vector4<T>& first, const Vector4<T>& second, const Vector4<T>& third, const Vector4<T>& fourth) noexcept: Matrix4x4<T>(first, second, third, fourth) {}

        /** @brief Construct with one value for all elements */
        constexpr explicit Matrix4(T value) noexcept: Matrix4x4<T>{value} {}

        /** @copydoc Matrix::Matrix(const T(&)[cols_][rows_]) */
        #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
        template<std::size_t cols_, std::size_t rows_> constexpr explicit Matrix4(const T(&data)[cols_][rows_]) noexcept: Matrix4x4<T>{data} {}
        #else
        /* GCC 4.8 workaround, see the RectangularMatrix base for details */
        constexpr explicit Matrix4(const T(&data)[4][4]) noexcept: Matrix4x4<T>{data} {}
        #endif

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix4(const RectangularMatrix<4, 4, U>& other) noexcept: Matrix4x4<T>(other) {}

        /** @brief Construct a matrix from external representation */
        template<class U, class = decltype(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(std::declval<U>()))> constexpr explicit Matrix4(const U& other): Matrix4x4<T>(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(other)) {}

        /** @copydoc RectangularMatrix::RectangularMatrix(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>&, T) */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix4(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>& other, T value = T(1)) noexcept: Matrix4x4<T>{IdentityInit, other, value} {}

        /** @copydoc RectangularMatrix::RectangularMatrix(ZeroInitT, const RectangularMatrix<otherCols, otherRows, T>&) */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix4(ZeroInitT, const RectangularMatrix<otherCols, otherRows, T>& other) noexcept: Matrix4x4<T>{ZeroInit, other} {}

        /**
         * @brief Construct by slicing or expanding a matrix of different size
         * @m_since_latest
         *
         * Equivalent to @ref Matrix4(IdentityInitT, const RectangularMatrix<otherCols, otherRows, T>&, T).
         * Note that this default is different from @ref RectangularMatrix,
         * where it's equivalent to the @ref ZeroInit variant instead.
         */
        template<std::size_t otherCols, std::size_t otherRows> constexpr explicit Matrix4(const RectangularMatrix<otherCols, otherRows, T>& other, T value = T(1)) noexcept: Matrix4x4<T>{IdentityInit, other, value} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Matrix4(const RectangularMatrix<4, 4, T>& other) noexcept: Matrix4x4<T>(other) {}

        /**
         * @brief Check whether the matrix represents a rigid transformation
         *
         * A [rigid transformation](https://en.wikipedia.org/wiki/Rigid_transformation)
         * consists only of rotation, reflection and translation (i.e., no
         * scaling, skew or projection).
         * @see @ref isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(3) == Vector4<T>(T(0), T(0), T(0), T(1));
        }

        /**
         * @brief 3D rotation and scaling part of the matrix
         *
         * Unchanged upper-left 3x3 part of the matrix. @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & \color{m-success} b_x & \color{m-info} c_x & t_x \\
         *          \color{m-danger} a_y & \color{m-success} b_y & \color{m-info} c_y & t_y \\
         *          \color{m-danger} a_z & \color{m-success} b_z & \color{m-info} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
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
         * @see @ref from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref rotation(Rad, const Vector3<T>&),
         *      @ref Matrix3::rotationScaling() const,
         *      @ref normalMatrix()
         */
        constexpr Matrix3x3<T> rotationScaling() const {
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
        }

        /**
         * @brief 3D rotation, reflection and shear part of the matrix
         *
         * Normalized upper-left 3x3 part of the matrix. Assuming the following
         * matrix, with the upper-left 3x3 part represented by column vectors
         * @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|} &
         *          \cfrac{\boldsymbol{c}}{|\boldsymbol{c}|}
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
         * @see @ref from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref rotation(Rad, const Vector3<T>&),
         *      @ref Matrix3::rotationShear() const,
         *      @ref normalMatrix()
         */
        Matrix3x3<T> rotationShear() const {
            return {(*this)[0].xyz().normalized(),
                    (*this)[1].xyz().normalized(),
                    (*this)[2].xyz().normalized()};
        }

        /**
         * @brief 3D rotation and reflection part of the matrix
         *
         * Normalized upper-left 3x3 part of the matrix. Expects that the
         * normalized part is orthogonal. Assuming the following matrix, with
         * the upper-left 3x3 part represented by column vectors
         * @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|} &
         *          \cfrac{\boldsymbol{c}}{|\boldsymbol{c}|}
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
         * reflections (which can then be fed to @ref Quaternion::fromMatrix(),
         * for example) is to flip an arbitrary column of the 3x3 part if its
         * @ref determinant() is negative, and apply the sign flip to the
         * corresponding scaling component instead:
         *
         * @snippet Math.cpp Matrix4-rotation-extract-reflection
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
         *      be easier renormalized such as @ref DualQuaternion. Another
         *      possibility is to ignore the error and extract combined
         *      rotation and scaling / shear with @ref rotationScaling() /
         *      @ref rotationShear().
         *
         * @see @ref rotationNormalized(), @ref scaling() const,
         *      @ref rotation(Rad, const Vector3<T>&),
         *      @ref Matrix3::rotation() const,
         *      @ref normalMatrix()
         */
        Matrix3x3<T> rotation() const;

        /**
         * @brief 3D rotation and reflection part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotation() const, but expects that the rotation part
         * is orthogonal, saving the extra renormalization. Assuming the
         * following matrix, with the upper-left 3x3 part represented by column
         * vectors @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & \color{m-success} b_x & \color{m-info} c_x & t_x \\
         *          \color{m-danger} a_y & \color{m-success} b_y & \color{m-info} c_y & t_y \\
         *          \color{m-danger} a_z & \color{m-success} b_z & \color{m-info} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting rotation is extracted as: @f[
         *      \boldsymbol{R} = \begin{pmatrix}
         *          \cfrac{\boldsymbol{a}}{|\boldsymbol{a}|} &
         *          \cfrac{\boldsymbol{b}}{|\boldsymbol{b}|} &
         *          \cfrac{\boldsymbol{c}}{|\boldsymbol{c}|}
         *      \end{pmatrix} = \begin{pmatrix}
         *          \boldsymbol{a} &
         *          \boldsymbol{b} &
         *          \boldsymbol{c}
         *      \end{pmatrix}
         * @f]
         *
         * In particular, for an orthogonal matrix, @ref rotationScaling(),
         * @ref rotationShear(), @ref rotation() const and
         * @ref rotationNormalized() all return the same value.
         *
         * @see @ref isOrthogonal(), @ref uniformScaling(),
         *      @ref Matrix3::rotationNormalized()
         */
        Matrix3x3<T> rotationNormalized() const;

        /**
         * @brief Non-uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 3x3 part of the matrix.
         * Faster alternative to @ref scaling() const, because it doesn't
         * calculate the square root. Assuming the following matrix, with the
         * upper-left 3x3 part represented by column vectors
         * @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting scaling vector, squared, is: @f[
         *      \boldsymbol{s}^2 = \begin{pmatrix}
         *          \boldsymbol{a} \cdot \boldsymbol{a} \\
         *          \boldsymbol{b} \cdot \boldsymbol{b} \\
         *          \boldsymbol{c} \cdot \boldsymbol{c}
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref scaling() const, @ref uniformScalingSquared(),
         *      @ref rotation() const, @ref Matrix3::scalingSquared()
         */
        Vector3<T> scalingSquared() const {
            return {(*this)[0].xyz().dot(),
                    (*this)[1].xyz().dot(),
                    (*this)[2].xyz().dot()};
        }

        /**
         * @brief Non-uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 3x3 part of the matrix. Use the
         * faster alternative @ref scalingSquared() where possible. Assuming
         * the following matrix, with the upper-left 3x3 part represented by
         * column vectors @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting scaling vector is: @f[
         *      \boldsymbol{s} = \begin{pmatrix}
         *          | \boldsymbol{a} | \\
         *          | \boldsymbol{b} | \\
         *          | \boldsymbol{c} |
         *      \end{pmatrix}
         * @f]
         *
         * Note that the returned vector is sign-less and the signs are instead
         * contained in @ref rotation() const / @ref rotationShear() const,
         * meaning these contain rotation together with a potential reflection.
         * See @ref rotation() const for an example of decomposing a rotation +
         * reflection matrix into a pure rotation and signed scaling.
         * @see @ref scalingSquared(), @ref uniformScaling(),
         *      @ref rotation() const, @ref Matrix3::scaling() const
         */
        Vector3<T> scaling() const {
            return {(*this)[0].xyz().length(),
                    (*this)[1].xyz().length(),
                    (*this)[2].xyz().length()};
        }

        /**
         * @brief Uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 3x3 part of the matrix.
         * Expects that the scaling is the same in all axes. Faster alternative
         * to @ref uniformScaling(), because it doesn't calculate the square
         * root. Assuming the following matrix, with the upper-left 3x3 part
         * represented by column vectors @f$ \boldsymbol{a} @f$,
         * @f$ \boldsymbol{b} @f$ and @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting uniform scaling, squared, is: @f[
         *      s^2 = \boldsymbol{a} \cdot \boldsymbol{a}
         *          = \boldsymbol{b} \cdot \boldsymbol{b}
         *          = \boldsymbol{c} \cdot \boldsymbol{c}
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
         *      @ref scaling(const Vector3<T>&),
         *      @ref Matrix3::uniformScalingSquared()
         */
        T uniformScalingSquared() const;

        /**
         * @brief Uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 3x3 part of the matrix. Expects that
         * the scaling is the same in all axes. Use the faster alternative
         * @ref uniformScalingSquared() where possible. Assuming the following
         * matrix, with the upper-left 3x3 part represented by column vectors
         * @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and
         * @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the resulting uniform scaling is: @f[
         *      s = | \boldsymbol{a} | = | \boldsymbol{b} | = | \boldsymbol{c} |
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
         * @see @ref rotation() const, @ref scaling() const,
         *      @ref scaling(const Vector3<T>&),
         *      @ref Matrix3::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Normal matrix
         * @m_since{2019,10}
         *
         * Shorthand for taking an inverse transpose of the upper-left 3x3 part
         * of the matrix. Assuming the following matrix, with the upper-left
         * 3x3 part represented by column vectors @f$ \boldsymbol{a} @f$,
         * @f$ \boldsymbol{b} @f$ and @f$ \boldsymbol{c} @f$: @f[
         *      \begin{pmatrix}
         *          \color{m-warning} a_x & \color{m-warning} b_x & \color{m-warning} c_x & t_x \\
         *          \color{m-warning} a_y & \color{m-warning} b_y & \color{m-warning} c_y & t_y \\
         *          \color{m-warning} a_z & \color{m-warning} b_z & \color{m-warning} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * the normal matrix is extracted as: @f[
         *      \boldsymbol{N} = \left(\begin{pmatrix}
         *          \boldsymbol{a} &
         *          \boldsymbol{b} &
         *          \boldsymbol{c}
         *      \end{pmatrix}^{-1}\right)^T
         * @f]
         *
         * The inverse transpose guarantees that the normals stay perpendicular
         * to the surface and point in the original direction even in presence
         * of shear, non-uniform scaling and reflection.
         * @see @ref rotationScaling(), @ref inverted(), @ref transposed()
         */
        Matrix3x3<T> normalMatrix() const {
            return rotationScaling().inverted().transposed();
        }

        /**
         * @brief Right-pointing 3D vector
         *
         * First three elements of first column. @f[
         *      \begin{pmatrix}
         *          \color{m-danger} a_x & b_x & c_x & t_x \\
         *          \color{m-danger} a_y & b_y & c_y & t_y \\
         *          \color{m-danger} a_z & b_z & c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref up(), @ref backward(), @ref Vector3::xAxis(),
         *      @ref Matrix3::right()
         */
        Vector3<T>& right() { return (*this)[0].xyz(); }
        constexpr Vector3<T> right() const { return (*this)[0].xyz(); } /**< @overload */

        /**
         * @brief Up-pointing 3D vector
         *
         * First three elements of second column. @f[
         *      \begin{pmatrix}
         *          a_x & \color{m-success} b_x & c_x & t_x \\
         *          a_y & \color{m-success} b_y & c_y & t_y \\
         *          a_z & \color{m-success} b_z & c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref right(), @ref backward(), @ref Vector3::yAxis(),
         *      @ref Matrix3::up()
         */
        Vector3<T>& up() { return (*this)[1].xyz(); }
        constexpr Vector3<T> up() const { return (*this)[1].xyz(); } /**< @overload */

        /**
         * @brief Backward-pointing 3D vector
         *
         * First three elements of third column. @f[
         *      \begin{pmatrix}
         *          a_x & b_x & \color{m-info} c_x & t_x \\
         *          a_y & b_y & \color{m-info} c_y & t_y \\
         *          a_z & b_z & \color{m-info} c_z & t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref right(), @ref up(), @ref Vector3::yAxis()
         */
        Vector3<T>& backward() { return (*this)[2].xyz(); }
        constexpr Vector3<T> backward() const { return (*this)[2].xyz(); } /**< @overload */

        /**
         * @brief 3D translation part of the matrix
         *
         * First three elements of fourth column. @f[
         *      \begin{pmatrix}
         *          a_x & b_x & c_x & \color{m-warning} t_x \\
         *          a_y & b_y & c_y & \color{m-warning} t_y \\
         *          a_z & b_z & c_z & \color{m-warning} t_z \\
         *          \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 0 & \color{m-dim} 1
         *      \end{pmatrix}
         * @f]
         *
         * @see @ref from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref translation(const Vector3<T>&),
         *      @ref Matrix3::translation()
         */
        Vector3<T>& translation() { return (*this)[3].xyz(); }
        constexpr Vector3<T> translation() const { return (*this)[3].xyz(); } /**< @overload */

        /**
         * @brief Distance to near plane of an orthographic projection matrix
         * @m_since_latest
         *
         * Assuming a matrix @f$ \boldsymbol{A} @f$ constructed with
         * @ref orthographicProjection(), returns the distance to its near
         * plane: @f[
         *      \frac{\boldsymbol{A}_{3,2} + 1}{\boldsymbol{A}_{2,2}} =
         *      \frac{\frac{n + f}{n - f} + 1}{\frac{2}{n - f}} =
         *      \frac{\frac{n + f + n - f}{n - f}}{\frac{2}{n - f}} =
         *      \frac{2n}{2} = n
         * @f]
         * @see @ref orthographicProjectionFar() const,
         *      @ref perspectiveProjectionNear() const
         */
        Float orthographicProjectionNear() const {
            return ((*this)[3][2] + T(1))/(*this)[2][2];
        }

        /**
         * @brief Distance to far plane of an orthographic projection matrix
         * @m_since_latest
         *
         * Assuming a matrix @f$ \boldsymbol{A} @f$ constructed with
         * @ref orthographicProjection(), returns the distance to its far
         * plane: @f[
         *      \frac{\boldsymbol{A}_{3,2} - 1}{\boldsymbol{A}_{2,2}} =
         *      \frac{\frac{n + f}{n - f} - 1}{\frac{2}{n - f}} =
         *      \frac{\frac{n + f - n + f}{n - f}}{\frac{2}{n - f}} =
         *      \frac{2f}{2} = f
         * @f]
         * @see @ref orthographicProjectionNear() const,
         *      @ref perspectiveProjectionFar() const
         */
        Float orthographicProjectionFar() const {
            return ((*this)[3][2] - T(1))/(*this)[2][2];
        }

        /**
         * @brief Distance to near plane of a perspective projection matrix
         * @m_since_latest
         *
         * Assuming a matrix @f$ \boldsymbol{A} @f$ constructed with
         * @ref perspectiveProjection(), returns the distance to its near
         * plane: @f[
         *      \frac{\boldsymbol{A}_{3,2}}{\boldsymbol{A}_{2,2} - 1} =
         *      \frac{\frac{2nf}{n - f}}{\frac{n + f}{n - f} - 1} =
         *      \frac{\frac{2nf}{n - f}}{\frac{n + f - n + f}{n - f}} =
         *      \frac{2nf}{2f} = n
         * @f]
         *
         * The same equation works for a perspective projection with an
         * infinite far plane: @f[
         *      \frac{\boldsymbol{A}_{3,2}}{\boldsymbol{A}_{2,2} - 1} =
         *      \frac{-2n}{-1 - 1} =
         *      \frac{-2n}{-2} = n
         * @f]
         * @see @ref perspectiveProjectionFar() const,
         *      @ref orthographicProjectionNear() const
         */
        Float perspectiveProjectionNear() const {
            return (*this)[3][2]/((*this)[2][2] - T(1));
        }

        /**
         * @brief Distance to far plane of a perspective projection matrix
         * @m_since_latest
         *
         * Assuming a matrix @f$ \boldsymbol{A} @f$ constructed with
         * @ref perspectiveProjection() with a positive far value, returns the
         * distance to its far plane: @f[
         *      \left\lvert \frac{\boldsymbol{A}_{3,2}}{\boldsymbol{A}_{2,2} + 1}\right\rvert =
         *      \left\lvert \frac{\frac{2nf}{n - f}}{\frac{n + f}{n - f} + 1}\right\rvert =
         *      \left\lvert \frac{\frac{2nf}{n - f}}{\frac{n + f + n - f}{n - f}}\right\rvert =
         *      \left\lvert \frac{2nf}{2n} \right\rvert = f
         * @f]
         *
         * The same equation works for a perspective projection with an
         * infinite far plane: @f[
         *      \left\lvert \frac{\boldsymbol{A}_{3,2}}{\boldsymbol{A}_{2,2} + 1} \right\rvert =
         *      \left\lvert \frac{-2n}{-1 + 1} \right\rvert =
         *      \left\lvert \frac{-2n}{0} \right\rvert = \infty
         * @f]
         * @see @ref perspectiveProjectionNear() const,
         *      @ref orthographicProjectionFar() const
         */
        Float perspectiveProjectionFar() const {
            return std::abs((*this)[3][2]/((*this)[2][2] + T(1)));
        }

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents a [rigid transformation](https://en.wikipedia.org/wiki/Rigid_transformation)
         * (i.e., no scaling, skew or projection). Significantly faster than
         * the general algorithm in @ref inverted(). @f[
         *      A^{-1} = \begin{pmatrix} (A^{3,3})^T & (A^{3,3})^T \begin{pmatrix} a_{3,0} \\ a_{3,1} \\ a_{3,2} \\ \end{pmatrix} \\ \begin{array}{ccc} 0 & 0 & 0 \end{array} & 1 \end{pmatrix}
         * @f]
         * @f$ A^{i, j} @f$ is matrix without i-th row and j-th column, see
         * @ref ij()
         * @see @ref isRigidTransformation(), @ref invertedOrthogonal(),
         *      @ref rotationScaling(), @ref translation() const,
         *      @ref Matrix3::invertedRigid()
         */
        Matrix4<T> invertedRigid() const;

        /**
         * @brief Transform a 3D vector with the matrix
         *
         * Unlike in @ref transformPoint(), translation is not involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ v_z \\ 0 \end{pmatrix}
         * @f]
         * @see @ref Quaternion::transformVector(),
         *      @ref Matrix3::transformVector()
         * @todo extract 3x3 matrix and multiply directly? (benchmark that)
         */
        Vector3<T> transformVector(const Vector3<T>& vector) const {
            return ((*this)*Vector4<T>(vector, T(0))).xyz();
        }

        /**
         * @brief Transform a 3D point with the matrix
         *
         * Unlike in @ref transformVector(), translation is also involved in
         * the transformation. @f[
         *      \boldsymbol v' = \boldsymbol v''_{xyz} / v''_w ~~~~~~~~~~ \boldsymbol v'' = \begin{pmatrix} v''_x \\ v''_y \\ v''_z \\ v''_w \end{pmatrix} = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ v_z \\ 1 \end{pmatrix} \\
         * @f]
         * @see @ref DualQuaternion::transformPoint(),
         *      @ref Matrix3::transformPoint()
         */
        Vector3<T> transformPoint(const Vector3<T>& vector) const {
            const Vector4<T> transformed{(*this)*Vector4<T>(vector, T(1))};
            return transformed.xyz()/transformed.w();
        }

        _MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(4, 4, Matrix4<T>)
        _MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(4, Matrix4, Vector4)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
_MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(4, Matrix4)
#endif

template<class T> Matrix4<T> Matrix4<T>::rotation(const Rad<T> angle, const Vector3<T>& normalizedAxis) {
    CORRADE_DEBUG_ASSERT(normalizedAxis.isNormalized(),
        "Math::Matrix4::rotation(): axis" << normalizedAxis << "is not normalized", {});

    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));
    const T oneMinusCosine = T(1) - cosine;

    const T xx = normalizedAxis.x()*normalizedAxis.x();
    const T xy = normalizedAxis.x()*normalizedAxis.y();
    const T xz = normalizedAxis.x()*normalizedAxis.z();
    const T yy = normalizedAxis.y()*normalizedAxis.y();
    const T yz = normalizedAxis.y()*normalizedAxis.z();
    const T zz = normalizedAxis.z()*normalizedAxis.z();

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

template<class T> Matrix4<T> Matrix4<T>::rotationX(const Rad<T> angle) {
    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));

    return {{T(1),   T(0),   T(0), T(0)},
            {T(0), cosine,   sine, T(0)},
            {T(0),  -sine, cosine, T(0)},
            {T(0),   T(0),   T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::rotationY(const Rad<T> angle) {
    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));

    return {{cosine, T(0),  -sine, T(0)},
            {  T(0), T(1),   T(0), T(0)},
            {  sine, T(0), cosine, T(0)},
            {  T(0), T(0),   T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::rotationZ(const Rad<T> angle) {
    const T sine = std::sin(T(angle));
    const T cosine = std::cos(T(angle));

    return {{cosine,   sine, T(0), T(0)},
            { -sine, cosine, T(0), T(0)},
            {  T(0),   T(0), T(1), T(0)},
            {  T(0),   T(0), T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::reflection(const Vector3<T>& normal) {
    CORRADE_DEBUG_ASSERT(normal.isNormalized(),
        "Math::Matrix4::reflection(): normal" << normal << "is not normalized", {});
    return from(Matrix3x3<T>() - T(2)*normal*RectangularMatrix<1, 3, T>(normal).transposed(), {});
}

template<class T> Matrix4<T> Matrix4<T>::orthographicProjection(const Vector2<T>& size, const T near, const T far) {
    const Vector2<T> xyScale = T(2.0)/size;
    const T zScale = T(2.0)/(near-far);

    return {{xyScale.x(),        T(0),             T(0), T(0)},
            {       T(0), xyScale.y(),             T(0), T(0)},
            {       T(0),        T(0),           zScale, T(0)},
            {       T(0),        T(0), near*zScale-T(1), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::orthographicProjection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight, const T near, const T far) {
    const Vector3<T> difference{topRight - bottomLeft, near - far};
    const Vector3<T> scale = T(2.0)/difference;
    const Vector3<T> offset = Vector3<T>{topRight + bottomLeft, near + far}/difference;

    return {{  scale.x(),        T(0),       T(0), T(0)},
            {       T(0),   scale.y(),       T(0), T(0)},
            {       T(0),        T(0),  scale.z(), T(0)},
            {-offset.x(), -offset.y(), offset.z(), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::perspectiveProjection(const Vector2<T>& size, const T near, const T far) {
    const Vector2<T> xyScale = 2*near/size;

    T m22, m32;
    if(far == Constants<T>::inf()) {
        m22 = T(-1);
        m32 = T(-2)*near;
    } else {
        const T zScale = T(1.0)/(near-far);
        m22 = (far+near)*zScale;
        m32 = T(2)*far*near*zScale;
    }

    return {{xyScale.x(),        T(0), T(0),  T(0)},
            {       T(0), xyScale.y(), T(0),  T(0)},
            {       T(0),        T(0), m22,  T(-1)},
            {       T(0),        T(0), m32,  T(0)}};
}

template<class T> Matrix4<T> Matrix4<T>::perspectiveProjection(const Vector2<T>& bottomLeft, const Vector2<T>& topRight, const T near, const T far) {
    const Vector2<T> xyDifference = topRight - bottomLeft;
    const Vector2<T> xyScale = 2*near/xyDifference;
    const Vector2<T> xyOffset = (topRight + bottomLeft)/xyDifference;

    T m22, m32;
    if(far == Constants<T>::inf()) {
        m22 = T(-1);
        m32 = T(-2)*near;
    } else {
        const T zScale = T(1.0)/(near-far);
        m22 = (far+near)*zScale;
        m32 = T(2)*far*near*zScale;
    }

    return {{ xyScale.x(),         T(0), T(0),  T(0)},
            {        T(0),  xyScale.y(), T(0),  T(0)},
            {xyOffset.x(), xyOffset.y(), m22,  T(-1)},
            {        T(0),         T(0), m32,  T(0)}};
}

template<class T> Matrix4<T> Matrix4<T>::lookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up) {
    const Vector3<T> backward = (eye - target).normalized();
    const Vector3<T> right = cross(up, backward).normalized();
    const Vector3<T> realUp = cross(backward, right);
    return from({right, realUp, backward}, eye);
}

template<class T> Matrix3x3<T> Matrix4<T>::rotation() const {
    Matrix3x3<T> rotationShear = this->rotationShear();
    CORRADE_DEBUG_ASSERT(rotationShear.isOrthogonal(),
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:" << Debug::newline << rotationShear, {});
    return rotationShear;
}

template<class T> Matrix3x3<T> Matrix4<T>::rotationNormalized() const {
    Matrix3x3<T> rotationScaling = this->rotationScaling();
    CORRADE_DEBUG_ASSERT(rotationScaling.isOrthogonal(),
        "Math::Matrix4::rotationNormalized(): the rotation part is not orthogonal:" << Debug::newline << rotationScaling, {});
    return rotationScaling;
}

template<class T> T Matrix4<T>::uniformScalingSquared() const {
    const T scalingSquared = (*this)[0].xyz().dot();
    CORRADE_DEBUG_ASSERT(TypeTraits<T>::equals((*this)[1].xyz().dot(), scalingSquared) &&
                   TypeTraits<T>::equals((*this)[2].xyz().dot(), scalingSquared),
        "Math::Matrix4::uniformScaling(): the matrix doesn't have uniform scaling:" << Debug::newline << rotationScaling(), {});
    return scalingSquared;
}

template<class T> Matrix4<T> Matrix4<T>::invertedRigid() const {
    CORRADE_DEBUG_ASSERT(isRigidTransformation(),
        "Math::Matrix4::invertedRigid(): the matrix doesn't represent a rigid transformation:" << Debug::newline << *this, {});

    Matrix3x3<T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
namespace Implementation {
    template<class T> struct StrictWeakOrdering<Matrix4<T>>: StrictWeakOrdering<RectangularMatrix<4, 4, T>> {};
}
#endif

}}

#endif
