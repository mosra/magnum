#ifndef Magnum_Math_Math_h
#define Magnum_Math_Math_h
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

#include <cstddef>

#include "utilities.h"

/** @file
 * @brief Math constants and utilities
 */

namespace Magnum { namespace Math {

/**
@todo Quaternions:
 - interpolation between rotations (=> animation, continuous collision detection)
 - better rotation representation (4 floats instead of 9/16 floats when using
   matrices)
*/

/** @brief Numeric constants */
template<class T> struct Constants {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    static constexpr T Pi;      /**< @brief Pi */
    static constexpr T Sqrt2;   /**< @brief Square root of 2 */
    static constexpr T Sqrt3;   /**< @brief Square root of 3 */
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct Constants<double> {
    static constexpr double Pi = 3.14159265359;
    static constexpr double Sqrt2 = 1.41421356237;
    static constexpr double Sqrt3 = 1.73205080757;
};
template<> struct Constants<float> {
    static constexpr float Pi = 3.14159265359f;
    static constexpr float Sqrt2 = 1.41421356237f;
    static constexpr float Sqrt3 = 1.73205080757f;
};

namespace Implementation {
    template<size_t exponent> struct Pow {
        template<class T> inline constexpr T operator()(T base) const {
            return base*Pow<exponent-1>()(base);
        }
    };
    template<> struct Pow<0> {
        template<class T> inline constexpr T operator()(T base) const { return 1; }
    };
}
#endif

/**
 * @brief Integral power
 *
 * Returns integral power of base to the exponent.
 */
template<size_t exponent, class T> inline constexpr T pow(T base) {
    return Implementation::Pow<exponent>()(base);
}

/**
 * @brief Integral logarithm
 *
 * Returns integral logarithm of given number with given base.
 */
size_t MAGNUM_EXPORT log(size_t base, size_t number);

/**
 * @brief Angle in degrees
 *
 * Function to make angle entering less error-prone. Converts the value to
 * radians at compile time. For example `deg(180.0f)` is converted to `3.14f`.
 */
template<class T> inline constexpr T deg(T value) { return value*Constants<T>::Pi/180; }

/**
 * @brief Angle in radians
 *
 * See also deg().
 */
template<class T> inline constexpr T rad(T value) { return value; }

}}

#endif
