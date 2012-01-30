#ifndef Magnum_Math_TypeTraits_h
#define Magnum_Math_TypeTraits_h
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
 * @brief Class Magnum::Math::TypeTraits
 */

#include <cmath>

namespace Magnum { namespace Math {

/**
@brief Traits class for numeric types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.

This class and class methods are specialized only for types where it makes
sense, it has empty implementation for unknown types or types which don't
support given feature, thus forcing the compilation stop with an error.
*/
template<class T> struct TypeTraits {
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /* Development note: the following values are implemented as inline
       functions, not as static const variables, because the compiler will
       inline the return values instead of referencing to static data and
       unlike static const variables these functions can return floats. */

    /**
     * @brief Epsilon value for fuzzy compare
     *
     * Returns 0 for integer types and reasonably small value for
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
template<class T> struct _TypeTraitsIntegral {
    constexpr inline static T epsilon() { return 0; }

    inline constexpr static bool equals(T a, T b) {
        return a == b;
    }
};
template<> struct TypeTraits<unsigned char>: public _TypeTraitsIntegral<unsigned char> {};
template<> struct TypeTraits<char>: public _TypeTraitsIntegral<char> {};

template<> struct TypeTraits<unsigned short>: public _TypeTraitsIntegral<unsigned short> {};
template<> struct TypeTraits<short>: public _TypeTraitsIntegral<short> {};

template<> struct TypeTraits<unsigned int>: public _TypeTraitsIntegral<unsigned int> {};
template<> struct TypeTraits<int>: public _TypeTraitsIntegral<int> {};

/* long is 32 bits somewhere and 64 bits elsewhere, so it cannot be mapped to
   any of them */
template<> struct TypeTraits<long unsigned int>: public _TypeTraitsIntegral<long unsigned int> {};
template<> struct TypeTraits<long int>: public _TypeTraitsIntegral<long int> {};

template<> struct TypeTraits<unsigned long long>: public _TypeTraitsIntegral<unsigned long long> {};
template<> struct TypeTraits<long long>: public _TypeTraitsIntegral<long long> {};

template<class T> struct _TypeTraitsFloatingPoint {
    inline static bool equals(T a, T b) {
        return std::abs(a - b) < TypeTraits<T>::epsilon();
    }
};
template<> struct TypeTraits<float>: public _TypeTraitsFloatingPoint<float> {
    constexpr inline static float epsilon() { return 1.0e-6f; }
};
template<> struct TypeTraits<double>: public _TypeTraitsFloatingPoint<double> {
    constexpr inline static double epsilon() { return 1.0e-12; }
};
#endif

}}

#endif
