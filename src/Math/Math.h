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
#include <cmath>
#include <type_traits>
#include <limits>

#include "magnumVisibility.h"

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

/**
@brief Numeric constants

@internal See MathTypeTraits class for implementation notes.
*/
template<class T> struct Constants {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    static inline constexpr T pi();     /**< @brief Pi */
    static inline constexpr T sqrt2();  /**< @brief Square root of 2 */
    static inline constexpr T sqrt3();  /**< @brief Square root of 3 */
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct Constants<double> {
    static inline constexpr double pi()    { return 3.141592653589793; }
    static inline constexpr double sqrt2() { return 1.414213562373095; }
    static inline constexpr double sqrt3() { return 1.732050807568877; }
};
template<> struct Constants<float> {
    static inline constexpr float pi()    { return 3.141592654f; }
    static inline constexpr float sqrt2() { return 1.414213562f; }
    static inline constexpr float sqrt3() { return 1.732050808f; }
};

namespace Implementation {
    template<size_t exponent> struct Pow {
        template<class T> inline constexpr T operator()(T base) const {
            return base*Pow<exponent-1>()(base);
        }
    };
    template<> struct Pow<0> {
        template<class T> inline constexpr T operator()(T) const { return 1; }
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
@brief Normalize floating-point value

Converts integral value from full range of given (signed/unsigned) integral
type to value in range @f$ [0, 1] @f$.

@attention To ensure the integral type is correctly detected when using
literals, this function should be called with both template parameters
explicit, e.g.:
@code
// Even if this is char literal, integral type is `int`, thus a = 0.1f
float a = normalize<float>('\127');

// b = 1.0f
float b = normalize<float, char>('\127');
@endcode

@todo Signed normalization to [-1.0, 1.0] like in OpenGL?
*/
template<class FloatingPoint, class Integral> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value, FloatingPoint>::type normalize(Integral value) {
    return (FloatingPoint(value)-FloatingPoint(std::numeric_limits<Integral>::min()))/
        (FloatingPoint(std::numeric_limits<Integral>::max()) - FloatingPoint(std::numeric_limits<Integral>::min()));
}

/**
@brief Denormalize floating-point value

Converts floating-point value in range @f$ [0, 1] @f$ to full range of given
integral type.

@note For best precision, `FloatingPoint` type should be always larger that
resulting `Integral` type (e.g. `double` to `int`, `long double` to `long long`).

@todo Signed normalization to [-1.0, 1.0] like in OpenGL?
*/
template<class Integral, class FloatingPoint> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value, Integral>::type denormalize(FloatingPoint value) {
    return             std::numeric_limits<Integral>::min() +
        round(FloatingPoint(value*std::numeric_limits<Integral>::max()) -
        FloatingPoint(value*std::numeric_limits<Integral>::min()));
}

/**
 * @brief Angle in degrees
 *
 * Function to make angle entering less error-prone. Converts the value to
 * radians at compile time. For example `deg(180.0f)` is converted to `3.14f`.
 */
template<class T> inline constexpr T deg(T value) { return value*Constants<T>::pi()/180; }

/**
 * @brief Angle in radians
 *
 * See also deg().
 */
template<class T> inline constexpr T rad(T value) { return value; }

}}

#endif
