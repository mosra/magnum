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

#include "Vector.h"

namespace Magnum { namespace Math {

/** @brief Three-component vector */
template<class T> class Vector3: public Vector<T, 3> {
    public:
        /** @copydoc Vector::from(T*) */
        inline constexpr static Vector3<T>& from(T* data) {
            return *reinterpret_cast<Vector3<T>*>(data);
        }

        /** @copydoc Vector::from(const T*) */
        inline constexpr static const Vector3<T>& from(const T* data) {
            return *reinterpret_cast<const Vector3<T>*>(data);
        }

        /** @brief Vector in direction of X axis */
        inline constexpr static Vector3<T> xAxis(T length = 1) { return Vector3<T>(length, 0, 0); }

        /** @brief Vector in direction of Y axis */
        inline constexpr static Vector3<T> yAxis(T length = 1) { return Vector3<T>(0, length, 0); }

        /** @brief Vector in direction of Z axis */
        inline constexpr static Vector3<T> zAxis(T length = 1) { return Vector3<T>(0, 0, length); }

        /** @brief Cross product */
        constexpr static Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
            return Vector3<T>(a[1]*b[2]-a[2]*b[1],
                              a[2]*b[0]-a[0]*b[2],
                              a[0]*b[1]-a[1]*b[0]);
        }

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector3(T value = T()): Vector<T, 3>(value, value, value) {}

        /** @copydoc Vector::Vector(const Vector&)  */
        inline constexpr Vector3(const Vector<T, 3>& other): Vector<T, 3>(other) {}

        /**
         * @brief Constructor
         * @param x     X / R value
         * @param y     Y / G value
         * @param z     Z / B value
         */
        inline constexpr Vector3(T x, T y, T z): Vector<T, 3>(x, y, z) {}

        /**
         * @brief Constructor
         * @param other     Two component vector
         * @param z         Z / B value
         */
        inline constexpr Vector3(const Vector<T, 2>& other, T z = T(0)): Vector<T, 3>(other[0], other[1], z) {}

        inline constexpr T x() const { return Vector<T, 3>::at(0); } /**< @brief X component */
        inline constexpr T y() const { return Vector<T, 3>::at(1); } /**< @brief Y component */
        inline constexpr T z() const { return Vector<T, 3>::at(2); } /**< @brief Z component */

        inline void setX(T value) { this->set(0, value); } /**< @brief Set X component */
        inline void setY(T value) { this->set(1, value); } /**< @brief Set Y component */
        inline void setZ(T value) { this->set(2, value); } /**< @brief Set Z component */

        inline constexpr T r() const { return x(); } /**< @brief R component */
        inline constexpr T g() const { return x(); } /**< @brief G component */
        inline constexpr T b() const { return z(); } /**< @brief B component */

        inline void setR(T value) { setX(value); } /**< @brief Set R component */
        inline void setG(T value) { setY(value); } /**< @brief Set G component */
        inline void setB(T value) { setZ(value); } /**< @brief Set B component */

        /** @copydoc Vector::operator=() */
        inline Vector3<T>& operator=(const Vector<T, 3>& other) { return Vector<T, 3>::operator=(other); }

        /** @copydoc Vector::operator*(const Vector<T, size>&) const */
        inline T operator*(const Vector<T, 3>& other) const { return Vector<T, 3>::operator*(other); }

        /** @copydoc Vector::operator*(T) const */
        inline Vector3<T> operator*(T number) const { return Vector<T, 3>::operator*(number); }

        /** @copydoc Vector::operator*=() */
        inline Vector3<T>& operator*=(T number) {
            Vector<T, 3>::operator*=(number);
            return *this;
        }

        /** @copydoc Vector::operator/() */
        inline Vector3<T> operator/(T number) const { return Vector<T, 3>::operator/(number); }

        /** @copydoc Vector::operator/=() */
        inline Vector3<T>& operator/=(T number) {
            Vector<T, 3>::operator/=(number);
            return *this;
        }

        /** @copydoc Vector::operator+() */
        inline Vector3<T> operator+(const Vector<T, 3>& other) const { return Vector<T, 3>::operator+(other); }

        /** @copydoc Vector::operator+=() */
        inline Vector3<T>& operator+=(const Vector<T, 3>& other) {
            Vector<T, 3>::operator+=(other);
            return *this;
        }

        /** @copydoc Vector::operator-(const Vector<T, size>&) const */
        inline Vector3<T> operator-(const Vector<T, 3>& other) const { return Vector<T, 3>::operator-(other); }

        /** @copydoc Vector::operator-=() */
        inline Vector3<T>& operator-=(const Vector<T, 3>& other) {
            Vector<T, 3>::operator-=(other);
            return *this;
        }

        /** @copydoc Vector::operator-() */
        inline Vector3<T> operator-() const { return Vector<T, 3>::operator-(); }

        /** @copydoc Vector::normalized() */
        inline Vector3<T> normalized() const { return Vector<T, 3>::normalized(); }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector3<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<T, 3>&>(value);
}
#endif

}}

#endif
