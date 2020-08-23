#ifndef Magnum_Math_Vector3_h
#define Magnum_Math_Vector3_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::Vector3, function @ref Magnum::Math::cross()
 */

#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace Math {

/**
@brief Cross product

Result has length of @cpp 0 @ce either when one of them is zero or they are
parallel or antiparallel and length of @cpp 1 @ce when two *normalized* vectors
are perpendicular. @f[
    \boldsymbol a \times \boldsymbol b = \begin{pmatrix}a_yb_z - a_zb_y \\ a_zb_x - a_xb_z \\ a_xb_y - a_yb_x \end{pmatrix}
@f]

If @f$ \boldsymbol{a} @f$, @f$ \boldsymbol{b} @f$ and @f$ \boldsymbol{c} @f$
are corners of a triangle in a counterclockwise order,
@f$ (\boldsymbol{c} - \boldsymbol{b}) \times (\boldsymbol{a} - \boldsymbol{b}) @f$
gives the direction of its normal.
@see @ref cross(const Vector2<T>&, const Vector2<T>&), @ref planeEquation()
*/
template<class T> inline Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
    return {
        a._data[1]*b._data[2] - b._data[1]*a._data[2],
        a._data[2]*b._data[0] - b._data[2]*a._data[0],
        a._data[0]*b._data[1] - b._data[0]*a._data[1]
    };
}

/**
@brief Three-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see @ref Magnum::Vector3, @ref Magnum::Vector3h, @ref Magnum::Vector3d,
    @ref Magnum::Vector3ub, @ref Magnum::Vector3b, @ref Magnum::Vector3us,
    @ref Magnum::Vector3s, @ref Magnum::Vector3ui, @ref Magnum::Vector3i
@configurationvalueref{Magnum::Math::Vector3}
*/
template<class T> class Vector3: public Vector<3, T> {
    public:
        /**
         * @brief Vector in a direction of X axis (right)
         *
         * Usable for translation or rotation along given axis, for example:
         *
         * @snippet MagnumMath.cpp Vector3-xAxis
         *
         * @see @ref yAxis(), @ref zAxis(), @ref xScale(), @ref Color3::red(),
         *      @ref Matrix4::right()
         */
        constexpr static Vector3<T> xAxis(T length = T(1)) { return {length, T(0), T(0)}; }

        /**
         * @brief Vector in a direction of Y axis (up)
         *
         * See @ref xAxis() for more information.
         * @see @ref yScale(), @ref Color3::green(), @ref Matrix4::up()
         */
        constexpr static Vector3<T> yAxis(T length = T(1)) { return {T(0), length, T(0)}; }

        /**
         * @brief Vector in a direction of Z axis (backward)
         *
         * See @ref xAxis() for more information.
         * @see @ref zScale(), @ref Color3::blue(), @ref Matrix4::backward()
         */
        constexpr static Vector3<T> zAxis(T length = T(1)) { return {T(0), T(0), length}; }

        /**
         * @brief Scaling vector in a direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         *
         * @snippet MagnumMath.cpp Vector3-xScale
         *
         * @see @ref yScale(), @ref zScale(), @ref Color3::cyan(), @ref xAxis()
         */
        constexpr static Vector3<T> xScale(T scale) { return {scale, T(1), T(1)}; }

        /**
         * @brief Scaling vector in a direction of Y axis (height)
         *
         * See @ref xScale() for more information.
         * @see @ref yAxis(), @ref Color3::magenta()
         */
        constexpr static Vector3<T> yScale(T scale) { return {T(1), scale, T(1)}; }

        /**
         * @brief Scaling vector in a direction of Z axis (depth)
         *
         * See @ref xScale() for more information.
         * @see @ref zAxis(), @ref Color3::yellow()
         */
        constexpr static Vector3<T> zScale(T scale) { return {T(1), T(1), scale}; }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Vector3(ZeroInitT).
         */
        constexpr /*implicit*/ Vector3() noexcept: Vector<3, T>{ZeroInit} {}

        /** @copydoc Vector::Vector(ZeroInitT) */
        constexpr explicit Vector3(ZeroInitT) noexcept: Vector<3, T>{ZeroInit} {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Vector3(Magnum::NoInitT) noexcept: Vector<3, T>{Magnum::NoInit} {}

        /** @copydoc Vector::Vector(T) */
        constexpr explicit Vector3(T value) noexcept: Vector<3, T>(value) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} x \\ y \\ z \end{pmatrix}
         * @f]
         */
        constexpr /*implicit*/ Vector3(T x, T y, T z) noexcept: Vector<3, T>(x, y, z) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} v_x \\ v_y \\ z \end{pmatrix}
         * @f]
         */
        constexpr /*implicit*/ Vector3(const Vector2<T>& xy, T z) noexcept: Vector<3, T>(xy[0], xy[1], z) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> constexpr explicit Vector3(const Vector<3, U>& other) noexcept: Vector<3, T>(other) {}

        /** @brief Construct vector from external representation */
        template<class U, class V =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::VectorConverter<3, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::VectorConverter<3, T, U>())
            #endif
            >
        constexpr explicit Vector3(const U& other): Vector<3, T>(Implementation::VectorConverter<3, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Vector3(const Vector<3, T>& other) noexcept: Vector<3, T>(other) {}

        /**
         * @brief X component
         *
         * @see @ref r()
         */
        T& x() { return Vector<3, T>::_data[0]; }
        constexpr T x() const { return Vector<3, T>::_data[0]; } /**< @overload */

        /**
         * @brief Y component
         *
         * @see @ref g()
         */
        T& y() { return Vector<3, T>::_data[1]; }
        constexpr T y() const { return Vector<3, T>::_data[1]; } /**< @overload */

        /**
         * @brief Z component
         *
         * @see @ref b()
         */
        T& z() { return Vector<3, T>::_data[2]; }
        constexpr T z() const { return Vector<3, T>::_data[2]; } /**< @overload */

        /**
         * @brief R component
         *
         * Equivalent to @ref x().
         */
        T& r() { return Vector<3, T>::_data[0]; }
        constexpr T r() const { return Vector<3, T>::_data[0]; } /**< @overload */

        /**
         * @brief G component
         *
         * Equivalent to @ref y().
         */
        T& g() { return Vector<3, T>::_data[1]; }
        constexpr T g() const { return Vector<3, T>::_data[1]; } /**< @overload */

        /**
         * @brief B component
         *
         * Equivalent to @ref z().
         */
        T& b() { return Vector<3, T>::_data[2]; }
        constexpr T b() const { return Vector<3, T>::_data[2]; } /**< @overload */

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see @ref gather(), @ref scatter()
         */
        Vector2<T>& xy() { return Vector2<T>::from(Vector<3, T>::data()); }
        constexpr const Vector2<T> xy() const {
            return {Vector<3, T>::_data[0], Vector<3, T>::_data[1]};
        } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(3, Vector3)

    private:
        template<class U> friend Vector3<U> cross(const Vector3<U>&, const Vector3<U>&);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(3, Vector3)
#endif

namespace Implementation {
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };

    template<class T> struct StrictWeakOrdering<Vector3<T>>: StrictWeakOrdering<Vector<3, T>> {};
}

}}

#endif
