#ifndef Magnum_Math_MathTypeTraits_h
#define Magnum_Math_MathTypeTraits_h
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
 * @brief Class Magnum::Math::MathTypeTraits
 */

#include <cmath>

/** @brief Precision when testing floats for equality */
#ifndef FLOAT_EQUALITY_PRECISION
#define FLOAT_EQUALITY_PRECISION 1.0e-6
#endif

/** @brief Precision when testing doubles for equality */
#ifndef DOUBLE_EQUALITY_PRECISION
#define DOUBLE_EQUALITY_PRECISION 1.0e-12
#endif

namespace Magnum { namespace Math {

/** @ingroup utility
@brief Traits class for numeric types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.

This class and class methods are specialized only for types where it makes
sense, it has empty implementation for unknown types or types which don't
support given feature, thus forcing the compilation stop with an error.

@internal The following values are implemented as inline functions, not as
    static const variables, because the compiler will inline the return values
    instead of referencing to static data and unlike static const variables
    the functions can be overloaded, deleted and hidden.
*/
template<class T> struct MathTypeTraits {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Epsilon value for fuzzy compare
     *
     * Returns minimal difference between numbers to be considered
     * inequal. Returns 1 for integer types and reasonably small value for
     * floating-point types.
     */
    constexpr inline static T epsilon();

    /**
     * @brief Fuzzy compare
     *
     * Uses equality for integer types and fuzzy compare for floating-point
     * types (using @ref epsilon value).
     */
    static bool equals(T a, T b);
    #endif
};

/** @bug Infinity comparison! */

/**
 * @todo Implement better fuzzy comparison algorithm, like at
 * http://floating-point-gui.de/errors/comparison/ or
 * http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> struct _MathTypeTraitsIntegral {
    constexpr inline static T epsilon() { return 1; }

    inline constexpr static bool equals(T a, T b) {
        return a == b;
    }
};
template<> struct MathTypeTraits<unsigned char>: public _MathTypeTraitsIntegral<unsigned char> {};
template<> struct MathTypeTraits<char>: public _MathTypeTraitsIntegral<char> {};

template<> struct MathTypeTraits<unsigned short>: public _MathTypeTraitsIntegral<unsigned short> {};
template<> struct MathTypeTraits<short>: public _MathTypeTraitsIntegral<short> {};

template<> struct MathTypeTraits<unsigned int>: public _MathTypeTraitsIntegral<unsigned int> {};
template<> struct MathTypeTraits<int>: public _MathTypeTraitsIntegral<int> {};

/* long is 32 bits somewhere and 64 bits elsewhere, so it cannot be mapped to
   any of them */
template<> struct MathTypeTraits<long unsigned int>: public _MathTypeTraitsIntegral<long unsigned int> {};
template<> struct MathTypeTraits<long int>: public _MathTypeTraitsIntegral<long int> {};

template<> struct MathTypeTraits<unsigned long long>: public _MathTypeTraitsIntegral<unsigned long long> {};
template<> struct MathTypeTraits<long long>: public _MathTypeTraitsIntegral<long long> {};

template<class T> struct _MathTypeTraitsFloatingPoint {
    inline static bool equals(T a, T b) {
        return std::abs(a - b) < MathTypeTraits<T>::epsilon();
    }
};
template<> struct MathTypeTraits<float>: public _MathTypeTraitsFloatingPoint<float> {
    constexpr inline static float epsilon() { return FLOAT_EQUALITY_PRECISION; }
};
template<> struct MathTypeTraits<double>: public _MathTypeTraitsFloatingPoint<double> {
    constexpr inline static double epsilon() { return DOUBLE_EQUALITY_PRECISION; }
};
#endif

}}

#endif
