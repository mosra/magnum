#ifndef Magnum_Math_TypeTraits_h
#define Magnum_Math_TypeTraits_h
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
 * @brief Class Magnum::Math::TypeTraits
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

namespace Implementation {
    template<class T> struct TypeTraitsDefault {
        TypeTraitsDefault() = delete;

        constexpr static bool equals(T a, T b) {
            return a == b;
        }
    };
}

/**
@brief Traits class for numeric types

Traits classes are usable for detecting type features at compile time without
the need for repeated code such as method overloading or template
specialization for given types.
*/
template<class T> struct TypeTraits: Implementation::TypeTraitsDefault<T> {
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
    constexpr static T epsilon();

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

/* Integral scalar types */
namespace Implementation {
    template<class T> struct TypeTraitsIntegral: TypeTraitsDefault<T> {
        constexpr static T epsilon() { return T(1); }
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct TypeTraits<UnsignedByte>: Implementation::TypeTraitsIntegral<UnsignedByte> {
    typedef Float FloatingPointType;
};
template<> struct TypeTraits<Byte>: Implementation::TypeTraitsIntegral<Byte> {
    typedef Float FloatingPointType;
};
template<> struct TypeTraits<UnsignedShort>: Implementation::TypeTraitsIntegral<UnsignedShort> {
    typedef Float FloatingPointType;
};
template<> struct TypeTraits<Short>: Implementation::TypeTraitsIntegral<Short> {
    typedef Float FloatingPointType;
};
template<> struct TypeTraits<UnsignedInt>: Implementation::TypeTraitsIntegral<UnsignedInt> {
    #ifndef MAGNUM_TARGET_GLES
    typedef Double FloatingPointType;
    #endif
};
template<> struct TypeTraits<Int>: Implementation::TypeTraitsIntegral<Int> {
    #ifndef MAGNUM_TARGET_GLES
    typedef Double FloatingPointType;
    #endif
};
template<> struct TypeTraits<UnsignedLong>: Implementation::TypeTraitsIntegral<UnsignedLong> {
    #ifndef MAGNUM_TARGET_GLES
    typedef long double FloatingPointType;
    #endif
};
template<> struct TypeTraits<Long>: Implementation::TypeTraitsIntegral<Long> {
    #ifndef MAGNUM_TARGET_GLES
    typedef long double FloatingPointType;
    #endif
};

/* Floating-point scalar types */
namespace Implementation {
    template<class T> struct TypeTraitsFloatingPoint {
        TypeTraitsFloatingPoint() = delete;

        static bool equals(T a, T b) {
            return std::abs(a - b) < TypeTraits<T>::epsilon();
        }
    };
}

template<> struct TypeTraits<Float>: Implementation::TypeTraitsFloatingPoint<Float> {
    typedef Float FloatingPointType;

    constexpr static Float epsilon() { return FLOAT_EQUALITY_PRECISION; }
};
#ifndef MAGNUM_TARGET_GLES
template<> struct TypeTraits<Double>: Implementation::TypeTraitsFloatingPoint<Double> {
    typedef Double FloatingPointType;

    constexpr static Double epsilon() { return DOUBLE_EQUALITY_PRECISION; }
};
#endif
template<> struct TypeTraits<long double>: Implementation::TypeTraitsFloatingPoint<long double> {
    typedef long double FloatingPointType;

    constexpr static long double epsilon() { return LONG_DOUBLE_EQUALITY_PRECISION; }
};

namespace Implementation {

/* Proper comparison should be with epsilon^2, but the value is not
   representable in given precision. Comparing to epsilon instead. */
template<class T> inline bool isZeroSquared(T lengthSquared) {
    return std::abs(lengthSquared) < TypeTraits<T>::epsilon();
}

/* Comparing squared length to 1 is not sufficient to compare within range
   [1 - epsilon, 1 + epsilon], as e.g. Quaternion with dot() = 1 + 1e-7 when
   converted to matrix has column vectors with dot() = 1 + 1e-6, which is just
   above 1 + epsilon. Thus it's needed to compare sqrt(dot()) in range
   [1 - epsilon, 1 + epsilon] or dot() in range [1 - 2*epsilon + epsilon^2,
   1 + 2*epsilon + epsilon^2]. Because epsilon^2 is way off machine precision,
   it's omitted. */
template<class T> inline bool isNormalizedSquared(T lengthSquared) {
    return std::abs(lengthSquared - T(1)) < T(2)*TypeTraits<T>::epsilon();
}

}
#endif

}}

#endif
