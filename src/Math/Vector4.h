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

/** @brief Four-component vector */
template<class T> class Vector4: public Vector<4, T> {
    public:
        /**
         * @copydoc Vector::Vector
         *
         * W / A component is set to one.
         */
        inline constexpr Vector4(): Vector<4, T>(T(0), T(0), T(0), T(1)) {}

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector4(T value): Vector<4, T>(value, value, value, value) {}

        /** @copydoc Vector::Vector(const Vector&) */
        inline constexpr Vector4(const Vector<4, T>& other): Vector<4, T>(other) {}

        /**
         * @brief Constructor
         * @param x     X / R value
         * @param y     Y / G value
         * @param z     Z / B value
         * @param w     W / A value
         */
        inline constexpr Vector4(T x, T y, T z, T w = T(1)): Vector<4, T>(x, y, z, w) {}

        /**
         * @brief Constructor
         * @param other     Three component vector
         * @param w         W / A value
         */
        /* Not marked as explicit, because conversion from Vector3 to Vector4
           is fairly common, nearly always with W set to 1 */
        inline constexpr Vector4(const Vector<3, T>& other, T w = T(1)): Vector<4, T>(other[0], other[1], other[2], w) {}

        inline constexpr T x() const { return (*this)[0]; } /**< @brief X component */
        inline constexpr T y() const { return (*this)[1]; } /**< @brief Y component */
        inline constexpr T z() const { return (*this)[2]; } /**< @brief Z component */
        inline constexpr T w() const { return (*this)[3]; } /**< @brief W component */

        inline void setX(T value) { (*this)[0] = value; }   /**< @brief Set X component */
        inline void setY(T value) { (*this)[1] = value; }   /**< @brief Set Y component */
        inline void setZ(T value) { (*this)[2] = value; }   /**< @brief Set Z component */
        inline void setW(T value) { (*this)[3] = value; }   /**< @brief Set W component */

        /**
         * @brief XYZ part of the vector
         * @return First three components of the vector
         */
        inline constexpr Vector3<T> xyz() const { return Vector3<T>::from(Vector<4, T>::data()); }

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         */
        inline constexpr Vector2<T> xy() const { return Vector2<T>::from(Vector<4, T>::data()); }

        inline constexpr T r() const { return x(); }    /**< @brief R component */
        inline constexpr T g() const { return y(); }    /**< @brief G component */
        inline constexpr T b() const { return z(); }    /**< @brief B component */
        inline constexpr T a() const { return w(); }    /**< @brief A component */

        inline void setR(T value) { setX(value); }      /**< @brief Set R component */
        inline void setG(T value) { setY(value); }      /**< @brief Set G component */
        inline void setB(T value) { setZ(value); }      /**< @brief Set B component */
        inline void setA(T value) { setA(value); }      /**< @brief Set A component */

        /**
         * @brief RGB part of the vector
         * @return First three components of the vector
         */
        inline constexpr Vector3<T> rgb() const { return xyz(); }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector4, 4)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector4<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<4, T>&>(value);
}
#endif

}}

#endif
