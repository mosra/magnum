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

@configurationvalueref{Magnum::Math::Vector4}
*/
template<class T> class Vector4: public Vector<4, T> {
    public:
        /**
         * @copydoc Vector::Vector
         *
         * W component is set to one.
         */
        inline constexpr Vector4(): Vector<4, T>(T(0), T(0), T(0), T(1)) {}

        /** @copydoc Vector::Vector(T) */
        inline constexpr explicit Vector4(T value): Vector<4, T>(value, value, value, value) {}

        /** @brief Copy constructor */
        inline constexpr Vector4(const RectangularMatrix<1, 4, T>& other): Vector<4, T>(other) {}

        /**
         * @brief Constructor
         * @param x     X value
         * @param y     Y value
         * @param z     Z value
         * @param w     W value
         */
        inline constexpr Vector4(T x, T y, T z, T w = T(1)): Vector<4, T>(x, y, z, w) {}

        /**
         * @brief Constructor
         * @param xyz   Three component vector
         * @param w     W value
         */
        /* Not marked as explicit, because conversion from Vector3 to Vector4
           is fairly common, nearly always with W set to 1 */
        inline constexpr Vector4(const Vector<3, T>& xyz, T w = T(1)): Vector<4, T>(xyz[0], xyz[1], xyz[2], w) {}

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
         *
         * @see swizzle()
         */
        inline constexpr Vector3<T> xyz() const { return Vector3<T>::from(Vector<4, T>::data()); }

        /**
         * @brief XY part of the vector
         * @return First two components of the vector
         *
         * @see swizzle()
         */
        inline constexpr Vector2<T> xy() const { return Vector2<T>::from(Vector<4, T>::data()); }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Vector4, 4)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Vector4, 4)

/** @debugoperator{Magnum::Math::Vector4} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Vector4<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<4, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Vector4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Vector4<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
