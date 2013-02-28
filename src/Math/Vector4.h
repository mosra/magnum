#ifndef Magnum_Math_Vector4_h
#define Magnum_Math_Vector4_h
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
 * @brief Class Magnum::Math::Vector4
 */

#include "Vector3.h"

namespace Magnum { namespace Math {

/**
@brief Four-component vector
@tparam T   Data type

See @ref matrix-vector for brief introduction.
@see Magnum::Vector4, Magnum::Vector4i, Magnum::Vector4ui, Magnum::Vector4d
@configurationvalueref{Magnum::Math::Vector4}
*/
template<class T> class Vector4: public Vector<4, T> {
    public:
        /** @copydoc Vector::Vector() */
        inline constexpr /*implicit*/ Vector4() {}

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector4(T value): Vector<4, T>(value) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} x \\ y \\ z \\ w \end{pmatrix}
         * @f]
         */
        inline constexpr /*implicit*/ Vector4(T x, T y, T z, T w): Vector<4, T>(x, y, z, w) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} v_x \\ v_y \\ v_z \\ w \end{pmatrix}
         * @f]
         */
        inline constexpr /*implicit*/ Vector4(const Vector3<T>& xyz, T w): Vector<4, T>(xyz[0], xyz[1], xyz[2], w) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> inline constexpr explicit Vector4(const Vector<4, U>& other): Vector<4, T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Vector4(const Vector<4, T>& other): Vector<4, T>(other) {}

        inline T& x() { return (*this)[0]; }                /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; } /**< @overload */
        inline T& y() { return (*this)[1]; }                /**< @brief Y component */
        inline constexpr T y() const { return (*this)[1]; } /**< @overload */
        inline T& z() { return (*this)[2]; }                /**< @brief Z component */
        inline constexpr T z() const { return (*this)[2]; } /**< @overload */
        inline T& w() { return (*this)[3]; }                /**< @brief W component */
        inline constexpr T w() const { return (*this)[3]; } /**< @overload */

        /**
         * @brief XYZ part of the vector
         * @return First three components of the vector
         *
         * @see swizzle()
         */
        inline Vector3<T>& xyz() { return Vector3<T>::from(Vector<4, T>::data()); }
        inline constexpr const Vector3<T> xyz() const { return {x(), y(), z()}; } /**< @overload */

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see swizzle()
         */
        inline Vector2<T>& xy() { return Vector2<T>::from(Vector<4, T>::data()); }
        inline constexpr const Vector2<T> xy() const { return {x(), y()}; } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector4, 4)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Vector4, 4)

/** @debugoperator{Magnum::Math::Vector4} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector4<T>& value) {
    return debug << static_cast<const Vector<4, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector4<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
