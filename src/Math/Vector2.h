#ifndef Magnum_Math_Vector2_h
#define Magnum_Math_Vector2_h
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
 * @brief Class Magnum::Math::Vector2
 */

#include "Vector.h"

namespace Magnum { namespace Math {

/**
@brief Two-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see Magnum::Vector2, Magnum::Vector2i, Magnum::Vector2ui, Magnum::Vector2d
@configurationvalueref{Magnum::Math::Vector2}
*/
template<class T> class Vector2: public Vector<2, T> {
    public:
        /**
         * @brief %Vector in direction of X axis (right)
         *
         * Usable for translation in given axis, for example:
         * @code
         * Matrix3::translation(Vector2::xAxis(5.0f)); // same as Matrix3::translation({5.0f, 0.0f});
         * @endcode
         * @see yAxis(), xScale(), Matrix3::right()
         */
        constexpr static Vector2<T> xAxis(T length = T(1)) { return Vector2<T>(length, T()); }

        /**
         * @brief %Vector in direction of Y axis (up)
         *
         * See xAxis() for more information.
         * @see yScale(), Matrix3::up()
         */
        constexpr static Vector2<T> yAxis(T length = T(1)) { return Vector2<T>(T(), length); }

        /**
         * @brief Scaling vector in direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         * @code
         * Matrix3::scaling(Vector2::xScale(-2.0f)); // same as Matrix3::scaling({-2.0f, 1.0f});
         * @endcode
         * @see yScale(), xAxis()
         */
        constexpr static Vector2<T> xScale(T scale) { return Vector2<T>(scale, T(1)); }

        /**
         * @brief Scaling vector in direction of Y axis (height)
         *
         * See xScale() for more information.
         * @see yAxis()
         */
        constexpr static Vector2<T> yScale(T scale) { return Vector2<T>(T(1), scale); }

        /**
         * @brief 2D cross product
         *
         * 2D version of cross product, also called perp-dot product,
         * equivalent to calling Vector3::cross() with Z coordinate set to `0`
         * and extracting only Z coordinate from the result (X and Y
         * coordinates are always zero). @f[
         *      \boldsymbol a \times \boldsymbol b = \boldsymbol a_\bot \cdot \boldsymbol b = a_xb_y - a_yb_x
         * @f]
         * @see perpendicular(), dot(const Vector&, const Vector&)
         */
        static T cross(const Vector2<T>& a, const Vector2<T>& b) {
            return Vector<2, T>::dot(a.perpendicular(), b);
        }

        /** @copydoc Vector::Vector() */
        constexpr /*implicit*/ Vector2() {}

        /** @copydoc Vector::Vector(T) */
        constexpr explicit Vector2(T value): Vector<2, T>(value) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} x \\ y \end{pmatrix}
         * @f]
         */
        constexpr /*implicit*/ Vector2(T x, T y): Vector<2, T>(x, y) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> constexpr explicit Vector2(const Vector<2, U>& other): Vector<2, T>(other) {}

        /** @brief Construct vector from external representation */
        #ifndef CORRADE_GCC44_COMPATIBILITY
        template<class U, class V = decltype(Implementation::VectorConverter<2, T, U>::from(std::declval<U>()))> constexpr explicit Vector2(const U& other): Vector<2, T>(Implementation::VectorConverter<2, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::VectorConverter<2, T, U>::from(*static_cast<const U*>(nullptr)))> constexpr explicit Vector2(const U& other): Vector<2, T>(Implementation::VectorConverter<2, T, U>::from(other)) {}
        #endif

        /** @brief Copy constructor */
        constexpr Vector2(const Vector<2, T>& other): Vector<2, T>(other) {}

        T& x() { return (*this)[0]; }                   /**< @brief X component */
        constexpr T x() const { return (*this)[0]; }    /**< @overload */
        T& y() { return (*this)[1]; }                   /**< @brief Y component */
        constexpr T y() const { return (*this)[1]; }    /**< @overload */

        /**
         * @brief Perpendicular vector
         *
         * Returns vector rotated 90° counterclockwise. @f[
         *      \boldsymbol v_\bot = \begin{pmatrix} -v_y \\ v_x \end{pmatrix}
         * @f]
         * @see cross(), dot(const Vector&, const Vector&), operator-() const
         */
        Vector2<T> perpendicular() const { return {-y(), x()}; }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector2, 2)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Vector2, 2)

/** @debugoperator{Magnum::Math::Vector2} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector2<T>& value) {
    return debug << static_cast<const Vector<2, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector2} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector2<T>>: public ConfigurationValue<Magnum::Math::Vector<2, T>> {};
}}

#endif
