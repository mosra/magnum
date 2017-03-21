#ifndef Magnum_Math_Vector2_h
#define Magnum_Math_Vector2_h
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
 * @brief Class @ref Magnum::Math::Vector2
 */

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

/**
@brief 2D cross product

2D version of cross product, also called perp-dot product, equivalent to
calling @ref cross(const Vector3<T>&, const Vector3<T>&) with Z coordinate set
to `0` and extracting only Z coordinate from the result (X and Y coordinates
are always zero). Returns `0` either when one of the vectors is zero or they
are parallel or antiparallel and `1` when two *normalized* vectors are
perpendicular. @f[
    \boldsymbol a \times \boldsymbol b = \boldsymbol a_\bot \cdot \boldsymbol b = a_xb_y - a_yb_x
@f]
@see @ref Vector2::perpendicular(),
    @ref dot(const Vector<size, T>&, const Vector<size, T>&)
 */
template<class T> inline T cross(const Vector2<T>& a, const Vector2<T>& b) {
    return dot(a.perpendicular(), b);
}

/**
@brief Two-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see @ref Magnum::Vector2, @ref Magnum::Vector2i, @ref Magnum::Vector2ui,
    @ref Magnum::Vector2d
@configurationvalueref{Magnum::Math::Vector2}
*/
template<class T> class Vector2: public Vector<2, T> {
    public:
        /**
         * @brief Vector in direction of X axis (right)
         *
         * Usable for translation in given axis, for example:
         * @code
         * Matrix3::translation(Vector2::xAxis(5.0f)); // same as Matrix3::translation({5.0f, 0.0f});
         * @endcode
         * @see @ref yAxis(), @ref xScale(), @ref Matrix3::right()
         */
        constexpr static Vector2<T> xAxis(T length = T(1)) { return {length, T(0)}; }

        /**
         * @brief Vector in direction of Y axis (up)
         *
         * See @ref xAxis() for more information.
         * @see @ref yScale(), @ref Matrix3::up()
         */
        constexpr static Vector2<T> yAxis(T length = T(1)) { return {T(0), length}; }

        /**
         * @brief Scaling vector in direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         * @code
         * Matrix3::scaling(Vector2::xScale(-2.0f)); // same as Matrix3::scaling({-2.0f, 1.0f});
         * @endcode
         * @see @ref yScale(), @ref xAxis()
         */
        constexpr static Vector2<T> xScale(T scale) { return {scale, T(1)}; }

        /**
         * @brief Scaling vector in direction of Y axis (height)
         *
         * See @ref xScale() for more information.
         * @see @ref yAxis()
         */
        constexpr static Vector2<T> yScale(T scale) { return {T(1), scale}; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Math::cross(const Vector2<T>&, const Vector2<T>&)
         * @deprecated Use @ref Math::cross(const Vector2<T>&, const Vector2<T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::cross() instead") static T cross(const Vector2<T>& a, const Vector2<T>& b) {
            return Math::cross(a, b);
        }
        #endif

        /** @copydoc Vector::Vector(ZeroInitT) */
        constexpr /*implicit*/ Vector2(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector<2, T>{ZeroInit}
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Vector2(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector<2, T>{NoInit}
            #endif
            {}

        /** @copydoc Vector::Vector(T) */
        constexpr explicit Vector2(T value) noexcept: Vector<2, T>(value) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} x \\ y \end{pmatrix}
         * @f]
         */
        constexpr /*implicit*/ Vector2(T x, T y) noexcept: Vector<2, T>(x, y) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> constexpr explicit Vector2(const Vector<2, U>& other) noexcept: Vector<2, T>(other) {}

        /** @brief Construct vector from external representation */
        template<class U, class V =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::VectorConverter<2, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::VectorConverter<2, T, U>())
            #endif
            >
        constexpr explicit Vector2(const U& other): Vector<2, T>(Implementation::VectorConverter<2, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Vector2(const Vector<2, T>& other) noexcept: Vector<2, T>(other) {}

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
         * @see @ref cross(),
         *      @ref dot(const Vector<size, T>&, const Vector<size, T>&),
         *      @ref operator-() const
         */
        Vector2<T> perpendicular() const { return {-y(), x()}; }

        /**
         * @brief Aspect ratio
         *
         * Returns quotient of the two elements. @f[
         *      a = \frac{v_x}{v_y}
         * @f]
         */
        T aspectRatio() const { return x()/y(); }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(2, Vector2)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(2, Vector2)
#endif

namespace Implementation {
    template<std::size_t, class> struct TypeForSize;
    template<class T> struct TypeForSize<2, T> { typedef Math::Vector2<typename T::Type> Type; };
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector2} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector2<T>>: public ConfigurationValue<Magnum::Math::Vector<2, T>> {};
}}

#endif
