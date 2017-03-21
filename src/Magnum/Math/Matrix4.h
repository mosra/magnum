#ifndef Magnum_Math_Matrix4_h
#define Magnum_Math_Matrix4_h
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
 * @brief Class @ref Magnum::Math::Matrix4
 */

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

See @ref matrix-vector and @ref transformations for brief introduction.
@see @ref Magnum::Matrix4, @ref Magnum::Matrix4d, @ref Matrix4x4,
    @ref DualQuaternion, @ref SceneGraph::MatrixTransformation3D
@configurationvalueref{Magnum::Math::Matrix4}
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
         * @see @ref translation(), @ref DualQuaternion::translation(),
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
         * @see @ref rotationScaling(),
         *      @ref Matrix3::scaling(const Vector2<T>&),
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
         *      @ref DualQuaternion::rotation(), @ref Matrix3::rotation(Rad),
         *      @ref Vector3::xAxis(), @ref Vector3::yAxis(),
         *      @ref Vector3::zAxis(), @ref Vector::isNormalized()
         */
        static Matrix4<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis);

        /**
         * @brief 3D rotation matrix around X axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::xAxis())`. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          1 &          0 &           0 & 0 \\
         *          0 & \cos\theta & -\sin\theta & 0 \\
         *          0 & \sin\theta &  \cos\theta & 0 \\
         *          0 &          0 &           0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad, const Vector3<T>&), @ref rotationY(),
         *      @ref rotationZ(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationX(Rad<T> angle);

        /**
         * @brief 3D rotation matrix around Y axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::yAxis())`. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *           \cos\theta & 0 & \sin\theta & 0 \\
         *                    0 & 1 &          0 & 0 \\
         *          -\sin\theta & 0 & \cos\theta & 0 \\
         *                    0 & 0 &          0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad, const Vector3<T>&), @ref rotationX(),
         *      @ref rotationZ(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationY(Rad<T> angle);

        /**
         * @brief 3D rotation matrix around Z axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::zAxis())`. @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \cos\theta & -\sin\theta & 0 & 0 \\
         *          \sin\theta &  \cos\theta & 0 & 0 \\
         *                   0 &           0 & 1 & 0 \\
         *                   0 &           0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref rotation(Rad, const Vector3<T>&), @ref rotationX(),
         *      @ref rotationY(), @ref rotation() const,
         *      @ref Quaternion::rotation(), @ref Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationZ(Rad<T> angle);

        /**
         * @brief 3D reflection matrix
         * @param normal    Normal of the plane through which to reflect
         *
         * Expects that the normal is normalized. Reflection along axes can be
         * done in a slightly simpler way also using @ref scaling(), e.g.
         * `Matrix4::reflection(Vector3::yAxis())` is equivalent to
         * `Matrix4::scaling(Vector3::yScale(-1.0f))`. @f[
         *      \boldsymbol{A} = \boldsymbol{I} - 2 \boldsymbol{NN}^T ~~~~~ \boldsymbol{N} = \begin{pmatrix} n_x \\ n_y \\ n_z \end{pmatrix}
         * @f]
         * @see @ref Matrix3::reflection(), @ref Vector::isNormalized()
         */
        static Matrix4<T> reflection(const Vector3<T>& normal);

        /**
         * @brief 3D shearing matrix along XY plane
         * @param amountX   Amount of shearing along X axis
         * @param amountY   Amount of shearing along Y axis
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
         * @brief 3D shearing matrix along XZ plane
         * @param amountX   Amount of shearing along X axis
         * @param amountZ   Amount of shearing along Z axis
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
         * @brief 3D shearing matrix along YZ plane
         * @param amountY   Amount of shearing along Y axis
         * @param amountZ   Amount of shearing along Z axis
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
         *          0 & 0 & \frac{2}{n - f} & \frac{2n}{n - f} - 1 \\
         *          0 & 0 & 0 & 1
         *      \end{pmatrix}
         * @f]
         * @see @ref perspectiveProjection(), @ref Matrix3::projection()
         */
        static Matrix4<T> orthographicProjection(const Vector2<T>& size, T near, T far);

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
         * @see @ref orthographicProjection(), @ref Matrix3::projection(),
         *      @ref Constants::inf()
         */
        static Matrix4<T> perspectiveProjection(const Vector2<T>& size, T near, T far);

        /**
         * @brief 3D perspective projection matrix
         * @param fov           Field of view angle (horizontal)
         * @param aspectRatio   Aspect ratio
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         *
         * If @p far is finite, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{1}{\tan{\frac{\theta}{2}}} & 0 & 0 & 0 \\
         *          0 & \frac{a}{\tan{\frac{\theta}{2}}} & 0 & 0 \\
         *          0 & 0 & \frac{n + f}{n - f} & \frac{2nf}{n - f} \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         *
         * For infinite @p far, the result is: @f[
         *      \boldsymbol{A} = \begin{pmatrix}
         *          \frac{1}{\tan{\frac{\theta}{2}}} & 0 & 0 & 0 \\
         *          0 & \frac{a}{\tan{\frac{\theta}{2}}} & 0 & 0 \\
         *          0 & 0 & -1 & -2n \\
         *          0 & 0 & -1 & 0
         *      \end{pmatrix}
         * @f]
         * @see @ref orthographicProjection(), @ref Matrix3::projection(),
         *      @ref Constants::inf()
         */
        static Matrix4<T> perspectiveProjection(Rad<T> fov, T aspectRatio, T near, T far) {
            const T xyScale = 2*std::tan(T(fov)/2)*near;
            return perspectiveProjection(Vector2<T>(xyScale, xyScale/aspectRatio), near, far);
        }

        /**
         * @brief Matrix oriented towards a specific point
         * @param eye       Location to place the matrix
         * @param target    Location towards which the matrix is oriented
         * @param up        Vector as a guide of which way is up (should not be
         *      the same direction as `target - eye`)
         *
         * @attention This function transforms an object so it's at @p eye
         *      position and oriented towards @p target, it does *not* produce
         *      a camera matrix. If you want to get the same what equivalent
         *      call to the well-known `gluLookAt()` would produce, invert the
         *      result using @ref invertedRigid().
         */
        static Matrix4<T> lookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up);

        /**
         * @brief Create matrix from rotation/scaling part and translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 3x3
         *      matrix)
         * @param translation       Translation part (first three elements of
         *      fourth column)
         *
         * @see @ref rotationScaling(), @ref translation() const
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
         * Creates identity matrix. @p value allows you to specify value on
         * diagonal.
         */
        constexpr /*implicit*/ Matrix4(IdentityInitT = IdentityInit, T value = T{1}) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix4x4<T>{IdentityInit, value}
            #endif
            {}

        /** @copydoc Matrix::Matrix(ZeroInitT) */
        constexpr explicit Matrix4(ZeroInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix4x4<T>{ZeroInit}
            #endif
            {}

        /** @copydoc Matrix::Matrix(NoInitT) */
        constexpr explicit Matrix4(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix4x4<T>{NoInit}
            #endif
            {}

        /** @brief Construct matrix from column vectors */
        constexpr /*implicit*/ Matrix4(const Vector4<T>& first, const Vector4<T>& second, const Vector4<T>& third, const Vector4<T>& fourth) noexcept: Matrix4x4<T>(first, second, third, fourth) {}

        /** @brief Construct matrix with one value for all elements */
        constexpr explicit Matrix4(T value) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Matrix4x4<T>{value}
            #endif
            {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix4(const RectangularMatrix<4, 4, U>& other) noexcept: Matrix4x4<T>(other) {}

        /** @brief Construct matrix from external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(std::declval<U>()))> constexpr explicit Matrix4(const U& other): Matrix4x4<T>(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Matrix4(const RectangularMatrix<4, 4, T>& other) noexcept: Matrix4x4<T>(other) {}

        /**
         * @brief Check whether the matrix represents rigid transformation
         *
         * Rigid transformation consists only of rotation and translation (i.e.
         * no scaling or projection).
         * @see @ref isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(3) == Vector4<T>(T(0), T(0), T(0), T(1));
        }

        /**
         * @brief 3D rotation and scaling part of the matrix
         *
         * Upper-left 3x3 part of the matrix.
         * @see @ref from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref rotation() const, @ref rotationNormalized(),
         *      @ref uniformScaling(), @ref rotation(Rad, const Vector3<T>&),
         *      @ref Matrix3::rotationScaling() const
         */
        constexpr Matrix3x3<T> rotationScaling() const {
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
        }

        /**
         * @brief 3D rotation part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotationScaling(), but additionally checks that the
         * base vectors are normalized.
         * @see @ref rotation() const, @ref uniformScaling(),
         *      @ref Matrix3::rotationNormalized()
         * @todo assert also orthogonality or this is good enough?
         */
        Matrix3x3<T> rotationNormalized() const {
            CORRADE_ASSERT((*this)[0].xyz().isNormalized() && (*this)[1].xyz().isNormalized() && (*this)[2].xyz().isNormalized(),
                           "Math::Matrix4::rotationNormalized(): the rotation part is not normalized", {});
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
        }

        /**
         * @brief 3D rotation part of the matrix
         *
         * Normalized upper-left 3x3 part of the matrix. Expects uniform
         * scaling.
         * @see @ref rotationNormalized(), @ref rotationScaling(),
         *      @ref uniformScaling(), @ref rotation(Rad, const Vector3<T>&),
         *      @ref Matrix3::rotation() const
         */
        Matrix3x3<T> rotation() const;

        /**
         * @brief Uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 3x3 part of the matrix.
         * Expects that the scaling is the same in all axes. Faster alternative
         * to @ref uniformScaling(), because it doesn't compute the square
         * root.
         * @see @ref rotationScaling(), @ref rotation() const,
         *      @ref rotationNormalized(), @ref scaling(const Vector3<T>&),
         *      @ref Matrix3::uniformScaling()
         */
        T uniformScalingSquared() const;

        /**
         * @brief Uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 3x3 part of the matrix. Expects that
         * the scaling is the same in all axes. Use faster alternative
         * @ref uniformScalingSquared() where possible.
         * @see @ref rotationScaling(), @ref rotation() const,
         *      @ref rotationNormalized(), @ref scaling(const Vector3<T>&),
         *      @ref Matrix3::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Right-pointing 3D vector
         *
         * First three elements of first column.
         * @see @ref up(), @ref backward(), @ref Vector3::xAxis(),
         *      @ref Matrix3::right()
         */
        Vector3<T>& right() { return (*this)[0].xyz(); }
        constexpr Vector3<T> right() const { return (*this)[0].xyz(); } /**< @overload */

        /**
         * @brief Up-pointing 3D vector
         *
         * First three elements of second column.
         * @see @ref right(), @ref backward(), @ref Vector3::yAxis(),
         *      @ref Matrix3::up()
         */
        Vector3<T>& up() { return (*this)[1].xyz(); }
        constexpr Vector3<T> up() const { return (*this)[1].xyz(); } /**< @overload */

        /**
         * @brief Backward-pointing 3D vector
         *
         * First three elements of third column.
         * @see @ref right(), @ref up(), @ref Vector3::yAxis()
         */
        Vector3<T>& backward() { return (*this)[2].xyz(); }
        constexpr Vector3<T> backward() const { return (*this)[2].xyz(); } /**< @overload */

        /**
         * @brief 3D translation part of the matrix
         *
         * First three elements of fourth column.
         * @see @ref from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref translation(const Vector3<T>&),
         *      @ref Matrix3::translation()
         */
        Vector3<T>& translation() { return (*this)[3].xyz(); }
        constexpr Vector3<T> translation() const { return (*this)[3].xyz(); } /**< @overload */

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * Significantly faster than the general algorithm in @ref inverted(). @f[
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
         * @brief Transform 3D vector with the matrix
         *
         * Unlike in @ref transformVector(), translation is not involved in the
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
         * @brief Transform 3D point with the matrix
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

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(4, 4, Matrix4<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(4, Matrix4, Vector4)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(4, Matrix4)
#endif

template<class T> Matrix4<T> Matrix4<T>::rotation(const Rad<T> angle, const Vector3<T>& normalizedAxis) {
    CORRADE_ASSERT(normalizedAxis.isNormalized(),
                   "Math::Matrix4::rotation(): axis must be normalized", {});

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
    CORRADE_ASSERT(normal.isNormalized(),
                    "Math::Matrix4::reflection(): normal must be normalized", {});
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

template<class T> Matrix4<T> Matrix4<T>::perspectiveProjection(const Vector2<T>& size, const T near, const T far) {
    const Vector2<T> xyScale = 2*near/size;

    if(far == Constants<T>::inf()) {
        return {{xyScale.x(),        T(0),                 T(0),  T(0)},
                {       T(0), xyScale.y(),                 T(0),  T(0)},
                {       T(0),        T(0),                T(-1), T(-1)},
                {       T(0),        T(0),           T(-2)*near,  T(0)}};
    } else {
        const T zScale = T(1.0)/(near-far);
        return {{xyScale.x(),        T(0),                 T(0),  T(0)},
                {       T(0), xyScale.y(),                 T(0),  T(0)},
                {       T(0),        T(0),    (far+near)*zScale, T(-1)},
                {       T(0),        T(0), T(2)*far*near*zScale,  T(0)}};
    }
}

template<class T> Matrix4<T> Matrix4<T>::lookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up) {
    const Vector3<T> backward = (eye - target).normalized();
    const Vector3<T> right = cross(up, backward).normalized();
    const Vector3<T> realUp = cross(backward, right);
    return from({right, realUp, backward}, eye);
}

template<class T> inline Matrix3x3<T> Matrix4<T>::rotation() const {
    CORRADE_ASSERT(TypeTraits<T>::equals((*this)[0].xyz().dot(), (*this)[1].xyz().dot()) &&
                   TypeTraits<T>::equals((*this)[1].xyz().dot(), (*this)[2].xyz().dot()),
        "Math::Matrix4::rotation(): the matrix doesn't have uniform scaling", {});
    return {(*this)[0].xyz().normalized(),
            (*this)[1].xyz().normalized(),
            (*this)[2].xyz().normalized()};
}

template<class T> T Matrix4<T>::uniformScalingSquared() const {
    const T scalingSquared = (*this)[0].xyz().dot();
    CORRADE_ASSERT(TypeTraits<T>::equals((*this)[1].xyz().dot(), scalingSquared) &&
                   TypeTraits<T>::equals((*this)[2].xyz().dot(), scalingSquared),
        "Math::Matrix4::uniformScaling(): the matrix doesn't have uniform scaling", {});
    return scalingSquared;
}

template<class T> Matrix4<T> Matrix4<T>::invertedRigid() const {
    CORRADE_ASSERT(isRigidTransformation(),
        "Math::Matrix4::invertedRigid(): the matrix doesn't represent rigid transformation", {});

    Matrix3x3<T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix4<T>>: public ConfigurationValue<Magnum::Math::Matrix4x4<T>> {};
}}

#endif
