#ifndef Magnum_Math_Vector3_h
#define Magnum_Math_Vector3_h
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
 * @brief Class @ref Magnum::Math::Vector3
 */

#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Swizzle.h"

namespace Magnum { namespace Math {

/**
@brief Cross product

Result has length of `0` either when one of them is zero or they are parallel
or antiparallel and length of `1` when two *normalized* vectors are
perpendicular. Done using the following equation: @f[
     \boldsymbol a \times \boldsymbol b = \begin{pmatrix} c_y \\ c_z \\ c_x \end{pmatrix} ~~~~~
     \boldsymbol c = \boldsymbol a \begin{pmatrix} b_y \\ b_z \\ b_x \end{pmatrix} -
                     \boldsymbol b \begin{pmatrix} a_y \\ a_z \\ a_x \end{pmatrix}
@f]
Which is equivalent to the common one (source:
https://twitter.com/sjb3d/status/563640846671953920): @f[
     \boldsymbol a \times \boldsymbol b = \begin{pmatrix}a_yb_z - a_zb_y \\ a_zb_x - a_xb_z \\ a_xb_y - a_yb_x \end{pmatrix}
@f]
@see @ref cross(const Vector2<T>&, const Vector2<T>&)
*/
template<class T> inline Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
    return swizzle<'y', 'z', 'x'>(a*swizzle<'y', 'z', 'x'>(b) -
                                  b*swizzle<'y', 'z', 'x'>(a));
}

/**
@brief Three-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see @ref Magnum::Vector3, @ref Magnum::Vector3i, @ref Magnum::Vector3ui,
    @ref Magnum::Vector3d
@configurationvalueref{Magnum::Math::Vector3}
*/
template<class T> class Vector3: public Vector<3, T> {
    public:
        /**
         * @brief Vector in direction of X axis (right)
         *
         * Usable for translation or rotation along given axis, for example:
         * @code
         * Matrix4::translation(Vector3::xAxis(5.0f)); // same as Matrix4::translation({5.0f, 0.0f, 0.0f});
         * Matrix4::rotation(30.0_degf, Vector3::xAxis()); // same as Matrix::rotation(30.0_degf, {1.0f, 0.0f, 0.0f});
         * @endcode
         * @see @ref yAxis(), @ref zAxis(), @ref xScale(), @ref Color3::red(),
         *      @ref Matrix4::right()
         */
        constexpr static Vector3<T> xAxis(T length = T(1)) { return {length, T(0), T(0)}; }

        /**
         * @brief Vector in direction of Y axis (up)
         *
         * See @ref xAxis() for more information.
         * @see @ref yScale(), @ref Color3::green(), @ref Matrix4::up()
         */
        constexpr static Vector3<T> yAxis(T length = T(1)) { return {T(0), length, T(0)}; }

        /**
         * @brief Vector in direction of Z axis (backward)
         *
         * See @ref xAxis() for more information.
         * @see @ref zScale(), @ref Color3::blue(), @ref Matrix4::backward()
         */
        constexpr static Vector3<T> zAxis(T length = T(1)) { return {T(0), T(0), length}; }

        /**
         * @brief Scaling vector in direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         * @code
         * Matrix4::scaling(Vector3::xScale(-2.0f)); // same as Matrix4::scaling({-2.0f, 1.0f, 1.0f});
         * @endcode
         * @see @ref yScale(), @ref zScale(), @ref Color3::cyan(), @ref xAxis()
         */
        constexpr static Vector3<T> xScale(T scale) { return {scale, T(1), T(1)}; }

        /**
         * @brief Scaling vector in direction of Y axis (height)
         *
         * See @ref xScale() for more information.
         * @see @ref yAxis(), @ref Color3::magenta()
         */
        constexpr static Vector3<T> yScale(T scale) { return {T(1), scale, T(1)}; }

        /**
         * @brief Scaling vector in direction of Z axis (depth)
         *
         * See @ref xScale() for more information.
         * @see @ref zAxis(), @ref Color3::yellow()
         */
        constexpr static Vector3<T> zScale(T scale) { return {T(1), T(1), scale}; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Math::cross(const Vector3<T>&, const Vector3<T>&)
         * @deprecated Use @ref Math::cross(const Vector3<T>&, const Vector3<T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::cross() instead") static Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
            return Math::cross(a, b);
        }
        #endif

        /** @copydoc Vector::Vector(ZeroInitT) */
        constexpr /*implicit*/ Vector3(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector<3, T>{ZeroInit}
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Vector3(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector<3, T>{NoInit}
            #endif
            {}

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
        T& x() { return (*this)[0]; }
        constexpr T x() const { return (*this)[0]; }    /**< @overload */

        /**
         * @brief Y component
         *
         * @see @ref g()
         */
        T& y() { return (*this)[1]; }
        constexpr T y() const { return (*this)[1]; }    /**< @overload */

        /**
         * @brief Z component
         *
         * @see @ref b()
         */
        T& z() { return (*this)[2]; }
        constexpr T z() const { return (*this)[2]; }    /**< @overload */

        /**
         * @brief R component
         *
         * Equivalent to @ref x().
         */
        T& r() { return x(); }
        constexpr T r() const { return x(); }           /**< @overload */

        /**
         * @brief G component
         *
         * Equivalent to @ref y().
         */
        T& g() { return y(); }
        constexpr T g() const { return y(); }           /**< @overload */

        /**
         * @brief B component
         *
         * Equivalent to @ref z().
         */
        T& b() { return z(); }
        constexpr T b() const { return z(); }           /**< @overload */

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see @ref swizzle()
         */
        Vector2<T>& xy() { return Vector2<T>::from(Vector<3, T>::data()); }
        constexpr const Vector2<T> xy() const { return {x(), y()}; } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(3, Vector3)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(3, Vector3)
#endif

namespace Implementation {
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector3<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};
}}

#endif
