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

namespace Magnum { namespace Math {

/** @brief Three-component vector */
template<class T> class Vector3: public Vector<3, T> {
    public:
        /** @brief %Vector in direction of X axis */
        inline constexpr static Vector3<T> xAxis(T length = T(1)) { return Vector3<T>(length, T(), T()); }

        /** @brief %Vector in direction of Y axis */
        inline constexpr static Vector3<T> yAxis(T length = T(1)) { return Vector3<T>(T(), length, T()); }

        /** @brief %Vector in direction of Z axis */
        inline constexpr static Vector3<T> zAxis(T length = T(1)) { return Vector3<T>(T(), T(), length); }

        /**
         * @brief Cross product
         *
         * @f[
         * \begin{pmatrix} c_0 \\ c_1 \\ c_2 \end{pmatrix} =
         * \begin{pmatrix}a_1b_2 - a_2b_1 \\ a_2b_0 - a_0b_2 \\ a_0b_1 - a_1b_0 \end{pmatrix}
         * @f]
         */
        inline constexpr static Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
            return Vector3<T>(a[1]*b[2]-a[2]*b[1],
                              a[2]*b[0]-a[0]*b[2],
                              a[0]*b[1]-a[1]*b[0]);
        }

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector3(T value = T()): Vector<3, T>(value, value, value) {}

        /** @copydoc Vector::Vector(const Vector&) */
        inline constexpr Vector3(const Vector<3, T>& other): Vector<3, T>(other) {}

        /**
         * @brief Constructor
         * @param x     X / R value
         * @param y     Y / G value
         * @param z     Z / B value
         */
        inline constexpr Vector3(T x, T y, T z): Vector<3, T>(x, y, z) {}

        /**
         * @brief Constructor
         * @param other     Two component vector
         * @param z         Z / B value
         */
        inline constexpr Vector3(const Vector<2, T>& other, T z): Vector<3, T>(other[0], other[1], z) {}

        inline constexpr T x() const { return (*this)[0]; } /**< @brief X component */
        inline constexpr T y() const { return (*this)[1]; } /**< @brief Y component */
        inline constexpr T z() const { return (*this)[2]; } /**< @brief Z component */

        inline void setX(T value) { (*this)[0] = value; }   /**< @brief Set X component */
        inline void setY(T value) { (*this)[1] = value; }   /**< @brief Set Y component */
        inline void setZ(T value) { (*this)[2] = value; }   /**< @brief Set Z component */

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see swizzle()
         */
        inline constexpr Vector2<T> xy() const { return Vector2<T>::from(Vector<3, T>::data()); }

        inline constexpr T r() const { return x(); }        /**< @brief R component */
        inline constexpr T g() const { return y(); }        /**< @brief G component */
        inline constexpr T b() const { return z(); }        /**< @brief B component */

        inline void setR(T value) { setX(value); }          /**< @brief Set R component */
        inline void setG(T value) { setY(value); }          /**< @brief Set G component */
        inline void setB(T value) { setZ(value); }          /**< @brief Set B component */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector3, 3)
};

/** @debugoperator{Vector3} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector3<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<3, T>&>(value);
}

}}

#endif
