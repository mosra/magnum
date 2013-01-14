#ifndef Magnum_Math_Functions_h
#define Magnum_Math_Functions_h
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

#include <cmath>
#include <type_traits>
#include <limits>
#include <algorithm>

#include "magnumVisibility.h"

/** @file
 * @brief Functions usable with scalar and vector types
 */

namespace Magnum { namespace Math {

/**
@todo Vector/scalar overloaded functions (...SIMD):
 - min(), max() (component-wise)
 - abs()
 - sin(), cos()...
 - clamp(), lerp()
*/

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::uint32_t exponent> struct Pow {
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
template<std::uint32_t exponent, class T> inline constexpr T pow(T base) {
    return Implementation::Pow<exponent>()(base);
}

/**
 * @brief Base-2 integral logarithm
 *
 * Returns integral logarithm of given number with base `2`.
 * @see log()
 */
std::uint32_t MAGNUM_EXPORT log2(std::uint32_t number);

/**
 * @brief Integral logarithm
 *
 * Returns integral logarithm of given number with given base.
 * @see log2()
 */
std::uint32_t MAGNUM_EXPORT log(std::uint32_t base, std::uint32_t number);

/**
@brief Normalize integral value

Converts integral value from full range of given *unsigned* integral type to
value in range @f$ [0, 1] @f$ or from *signed* integral to range @f$ [-1, 1] @f$.

@note For best precision, `FloatingPoint` type should be always larger that
    resulting `Integral` type (e.g. `double` to `std::int32_t`, `long double`
    to `std::int64_t`).

@attention To ensure the integral type is correctly detected when using
    literals, this function should be called with both template parameters
    explicit, e.g.:
@code
// Even if this is character literal, integral type is 32bit, thus a != 1.0f
float a = normalize<float>('\127');

// b = 1.0f
float b = normalize<float, std::int8_t>('\127');
@endcode

@see denormalize()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class FloatingPoint, class Integral> inline constexpr FloatingPoint normalize(Integral value);
#else
template<class FloatingPoint, class Integral> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value && std::is_unsigned<Integral>::value, FloatingPoint>::type normalize(Integral value) {
    return value/FloatingPoint(std::numeric_limits<Integral>::max());
}

template<class FloatingPoint, class Integral> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value && std::is_signed<Integral>::value, FloatingPoint>::type normalize(Integral value) {
    return std::max(value/FloatingPoint(std::numeric_limits<Integral>::max()), FloatingPoint(-1));
}
#endif

/**
@brief Denormalize floating-point value

Converts floating-point value in range @f$ [0, 1] @f$ to full range of given
unsigned integral type or range @f$ [-1, 1] @f$ to full range of given signed
integral type.

@note For best precision, `FloatingPoint` type should be always larger that
    resulting `Integral` type (e.g. `double` to `std::int32_t`, `long double`
    to `std::int64_t`).

@see normalize()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Integral, class FloatingPoint> inline constexpr typename Integral denormalize(FloatingPoint value);
#else
template<class Integral, class FloatingPoint> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value && std::is_unsigned<Integral>::value, Integral>::type denormalize(FloatingPoint value) {
    return value*std::numeric_limits<Integral>::max();
}
template<class Integral, class FloatingPoint> inline constexpr typename std::enable_if<std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value && std::is_signed<Integral>::value, Integral>::type denormalize(FloatingPoint value) {
    return value*std::numeric_limits<Integral>::max();
}
#endif

/** @brief Clamp value */
template<class T> inline T clamp(T value, T min, T max) {
    return std::min(std::max(value, min), max);
}

}}

#endif
