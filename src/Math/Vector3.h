#ifndef Magnum_Math_Vector3_h
#define Magnum_Math_Vector3_h
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
 * @brief Class Magnum::Math::Vector3
 */

#include "Vector2.h"
#include "Swizzle.h"

namespace Magnum { namespace Math {

/**
@brief Three-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction. See also Point2D for
homogeneous two-dimensional coordinates.
@see Magnum::Vector3, Magnum::Vector3i, Magnum::Vector3ui
@configurationvalueref{Magnum::Math::Vector3}
*/
template<class T> class Vector3: public Vector<3, T> {
    public:
        /**
         * @brief %Vector in direction of X axis (right)
         *
         * Usable for translation or rotation along given axis, for example:
         * @code
         * Matrix4::translation(Vector3::xAxis(5.0f)); // same as Matrix4::translation({5.0f, 0.0f, 0.0f});
         * Matrix4::rotation(30.0_degf, Vector3::xAxis()); // same as Matrix::rotation(30.0_degf, {1.0f, 0.0f, 0.0f});
         * @endcode
         * @see yAxis(), zAxis(), xScale(), Matrix4::right()
         */
        inline constexpr static Vector3<T> xAxis(T length = T(1)) { return Vector3<T>(length, T(), T()); }

        /**
         * @brief %Vector in direction of Y axis (up)
         *
         * See xAxis() for more information.
         * @see yScale(), Matrix4::up()
         */
        inline constexpr static Vector3<T> yAxis(T length = T(1)) { return Vector3<T>(T(), length, T()); }

        /**
         * @brief %Vector in direction of Z axis (backward)
         *
         * See xAxis() for more information.
         * @see zScale(), Matrix4::backward()
         */
        inline constexpr static Vector3<T> zAxis(T length = T(1)) { return Vector3<T>(T(), T(), length); }

        /**
         * @brief Scaling vector in direction of X axis (width)
         *
         * Usable for scaling along given direction, for example:
         * @code
         * Matrix4::scaling(Vector3::xScale(-2.0f)); // same as Matrix4::scaling({-2.0f, 1.0f, 1.0f});
         * @endcode
         * @see yScale(), zScale(), xAxis()
         */
        inline constexpr static Vector3<T> xScale(T scale) { return Vector3<T>(scale, T(1), T(1)); }

        /**
         * @brief Scaling vector in direction of Y axis (height)
         *
         * See xScale() for more information.
         * @see yAxis()
         */
        inline constexpr static Vector3<T> yScale(T scale) { return Vector3<T>(T(1), scale, T(1)); }

        /**
         * @brief Scaling vector in direction of Z axis (depth)
         *
         * See xScale() for more information.
         * @see zAxis()
         */
        inline constexpr static Vector3<T> zScale(T scale) { return Vector3<T>(T(1), T(1), scale); }

        /**
         * @brief Cross product
         *
         * @f[
         * \boldsymbol a \times \boldsymbol b =
         * \begin{pmatrix} c_0 \\ c_1 \\ c_2 \end{pmatrix} =
         * \begin{pmatrix}a_1b_2 - a_2b_1 \\ a_2b_0 - a_0b_2 \\ a_0b_1 - a_1b_0 \end{pmatrix}
         * @f]
         */
        inline constexpr static Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
            return swizzle<'y', 'z', 'x'>(a)*swizzle<'z', 'x', 'y'>(b) -
                   swizzle<'z', 'x', 'y'>(a)*swizzle<'y', 'z', 'x'>(b);
        }

        /** @copydoc Vector::Vector() */
        inline constexpr /*implicit*/ Vector3() {}

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector3(T value): Vector<3, T>(value) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = (x, y, z)^T
         * @f]
         */
        inline constexpr /*implicit*/ Vector3(T x, T y, T z): Vector<3, T>(x, y, z) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = (v_x, v_y, z)^T
         * @f]
         */
        inline constexpr /*implicit*/ Vector3(const Vector2<T>& xy, T z): Vector<3, T>(xy[0], xy[1], z) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> inline constexpr explicit Vector3(const Vector<3, U>& other): Vector<3, T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Vector3(const Vector<3, T>& other): Vector<3, T>(other) {}

        inline T& x() { return (*this)[0]; }                /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; } /**< @overload */
        inline T& y() { return (*this)[1]; }                /**< @brief Y component */
        inline constexpr T y() const { return (*this)[1]; } /**< @overload */
        inline T& z() { return (*this)[2]; }                /**< @brief Z component */
        inline constexpr T z() const { return (*this)[2]; } /**< @overload */

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see swizzle()
         */
        inline Vector2<T>& xy() { return Vector2<T>::from(Vector<3, T>::data()); }
        inline constexpr Vector2<T> xy() const { return Vector2<T>::from(Vector<3, T>::data()); } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector3, 3)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Vector3, 3)

/** @debugoperator{Magnum::Math::Vector3} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector3<T>& value) {
    return debug << static_cast<const Vector<3, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector3<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};
}}

#endif
