#ifndef Magnum_Math_MathTypeTraits_h
#define Magnum_Math_MathTypeTraits_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class Magnum::Math::MathTypeTraits
 */

#include <cmath>

#include "Types.h"

/** @brief Precision when testing floats for equality */
#ifndef FLOAT_EQUALITY_PRECISION
#define FLOAT_EQUALITY_PRECISION 1.0e-6
#endif

/** @brief Precision when testing doubles for equality */
#ifndef DOUBLE_EQUALITY_PRECISION
#define DOUBLE_EQUALITY_PRECISION 1.0e-12
#endif

/**
@brief Precision when testing long doubles for equality
@todo some proper value please
*/
#ifndef LONG_DOUBLE_EQUALITY_PRECISION
#define LONG_DOUBLE_EQUALITY_PRECISION 1.0e-18
#endif

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class T> struct MathTypeTraitsDefault {
        MathTypeTraitsDefault() = delete;

        inline constexpr static bool equals(T a, T b) {
            return a == b;
        }
    };
}
#endif

/**
@brief Traits class for numeric types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.
*/
template<class T> struct MathTypeTraits: Implementation::MathTypeTraitsDefault<T> {
    /*
     * The following values are implemented as inline functions, not as
     * static const variables, because the compiler will inline the return
     * values instead of referencing to static data and unlike static const
     * variables the functions can be overloaded, deleted and hidden.
     */

    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Corresponding floating-point type for normalization
     *
     * If the type is not already floating-point, defines smallest larger
     * floating-point type.
     */
    typedef U FloatingPointType;

    /**
     * @brief Epsilon value for fuzzy compare
     *
     * Returns minimal difference between numbers to be considered
     * inequal. Returns 1 for integer types and reasonably small value for
     * floating-point types. Not implemented for arbitrary types.
     */
    inline constexpr static T epsilon();

    /**
     * @brief Fuzzy compare
     *
     * Uses fuzzy compare for floating-point types (using epsilon() value),
     * pure equality comparison everywhere else.
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
/* Integral scalar types */
namespace Implementation {
    template<class T> struct MathTypeTraitsIntegral: MathTypeTraitsDefault<T> {
        inline constexpr static T epsilon() { return T(1); }
    };
}

template<> struct MathTypeTraits<UnsignedByte>: Implementation::MathTypeTraitsIntegral<UnsignedByte> {
    typedef Float FloatingPointType;
};
template<> struct MathTypeTraits<Byte>: Implementation::MathTypeTraitsIntegral<Byte> {
    typedef Float FloatingPointType;
};
template<> struct MathTypeTraits<UnsignedShort>: Implementation::MathTypeTraitsIntegral<UnsignedShort> {
    typedef Float FloatingPointType;
};
template<> struct MathTypeTraits<Short>: Implementation::MathTypeTraitsIntegral<Short> {
    typedef Float FloatingPointType;
};
template<> struct MathTypeTraits<UnsignedInt>: Implementation::MathTypeTraitsIntegral<UnsignedInt> {
    typedef Double FloatingPointType;
};
template<> struct MathTypeTraits<Int>: Implementation::MathTypeTraitsIntegral<Int> {
    typedef Double FloatingPointType;
};
template<> struct MathTypeTraits<UnsignedLong>: Implementation::MathTypeTraitsIntegral<UnsignedLong> {
    typedef long double FloatingPointType;
};
template<> struct MathTypeTraits<Long>: Implementation::MathTypeTraitsIntegral<Long> {
    typedef long double FloatingPointType;
};

/* Floating-point scalar types */
namespace Implementation {
    template<class T> struct MathTypeTraitsFloatingPoint {
        MathTypeTraitsFloatingPoint() = delete;

        inline static bool equals(T a, T b) {
            return std::abs(a - b) < MathTypeTraits<T>::epsilon();
        }
    };
}

template<> struct MathTypeTraits<Float>: Implementation::MathTypeTraitsFloatingPoint<Float> {
    typedef Float FloatingPointType;

    inline constexpr static Float epsilon() { return FLOAT_EQUALITY_PRECISION; }
};
template<> struct MathTypeTraits<Double>: Implementation::MathTypeTraitsFloatingPoint<Double> {
    typedef Double FloatingPointType;

    inline constexpr static Double epsilon() { return DOUBLE_EQUALITY_PRECISION; }
};
template<> struct MathTypeTraits<long double>: Implementation::MathTypeTraitsFloatingPoint<long double> {
    typedef long double FloatingPointType;

    inline constexpr static long double epsilon() { return LONG_DOUBLE_EQUALITY_PRECISION; }
};
#endif

}}

#endif
