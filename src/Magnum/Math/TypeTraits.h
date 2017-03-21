#ifndef Magnum_Math_TypeTraits_h
#define Magnum_Math_TypeTraits_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::TypeTraits
 */

#include <cmath>

#include "Magnum/Types.h"

/**
@brief Precision when testing floats for equality

They have "at least" 6 significant digits of precision, taking one digit less
for more headroom.
*/
#ifndef FLOAT_EQUALITY_PRECISION
#define FLOAT_EQUALITY_PRECISION 1.0e-5f
#endif

/**
@brief Precision when testing doubles for equality

They have "at least" 15 significant digits of precision, taking one digit less
for more headroom.
*/
#ifndef DOUBLE_EQUALITY_PRECISION
#define DOUBLE_EQUALITY_PRECISION 1.0e-14
#endif

/**
@brief Precision when testing long doubles for equality

They have "at least" 18 significant digits of precision, taking one digit less
for more headroom.

@attention On MSVC the precision is the same as for doubles, because they are
    internally the same type (source: https://msdn.microsoft.com/en-us/library/9cx8xs15.aspx).
    The same is apparently for @ref CORRADE_TARGET_ANDROID "Android", but I
    couldn't find any source for that.
*/
#ifndef LONG_DOUBLE_EQUALITY_PRECISION
#if !defined(_MSC_VER) && !defined(CORRADE_TARGET_ANDROID)
#define LONG_DOUBLE_EQUALITY_PRECISION 1.0e-17l
#else
#define LONG_DOUBLE_EQUALITY_PRECISION 1.0e-14l
#endif
#endif

namespace Magnum { namespace Math {

namespace Implementation {
    template<class T> struct TypeTraitsDefault {
        TypeTraitsDefault() = delete;

        constexpr static bool equals(T a, T b) {
            return a == b;
        }

        constexpr static bool equalsZero(T a, T) {
            return !a;
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
     * @brief Type name
     *
     * Returns a string representation of type name, such as `"UnsignedInt"`
     * for @ref Magnum::UnsignedInt "UnsignedInt".
     */
    constexpr static const char* name();

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
     * Uses fuzzy compare for floating-point types (using @ref epsilon()
     * value), pure equality comparison everywhere else.
     */
    static bool equals(T a, T b);

    /**
     * @brief Fuzzy compare to zero with magnitude
     *
     * Uses fuzzy compare for floating-point types (using @ref epsilon()
     * value), pure equality comparison everywhere else. Use this function when
     * comparing e.g. a calculated nearly-zero difference with zero, knowing
     * the magnitude of original values so the epsilon can be properly scaled.
     * In other words, the following lines are equivalent:
     * @code
     * Float a, b;
     * Math::TypeTraits<Float>::equals(a, b);
     * Math::TypeTraits<Float>::equalsZero(a - b, Math::max(Math::abs(a), Math::abs(b)));
     * @endcode
     */
    static bool equalsZero(T a, T magnitude);
    #endif
};

/* Integral scalar types */
namespace Implementation {
    template<class> struct TypeTraitsName;
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type) template<> struct TypeTraitsName<type> { \
        constexpr static const char* name() { return #type; } \
    };
    _c(UnsignedByte)
    _c(Byte)
    _c(UnsignedShort)
    _c(Short)
    _c(UnsignedInt)
    _c(Int)
    #ifndef MAGNUM_TARGET_WEBGL
    _c(UnsignedLong)
    _c(Long)
    #endif
    _c(Float)
    _c(Double)
    _c(long double)
    #undef _c
    #endif

    template<class T> struct TypeTraitsIntegral: TypeTraitsDefault<T>, TypeTraitsName<T> {
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
    typedef Double FloatingPointType;
};
template<> struct TypeTraits<Int>: Implementation::TypeTraitsIntegral<Int> {
    typedef Double FloatingPointType;
};
#ifndef MAGNUM_TARGET_WEBGL
template<> struct TypeTraits<UnsignedLong>: Implementation::TypeTraitsIntegral<UnsignedLong> {
    typedef long double FloatingPointType;
};
template<> struct TypeTraits<Long>: Implementation::TypeTraitsIntegral<Long> {
    typedef long double FloatingPointType;
};
#endif

/* Floating-point scalar types */
namespace Implementation {

template<class T> struct TypeTraitsFloatingPoint: TypeTraitsName<T> {
    TypeTraitsFloatingPoint() = delete;

    static bool equals(T a, T b);
    static bool equalsZero(T a, T epsilon);
};

/* Adapted from http://floating-point-gui.de/errors/comparison/ */
template<class T> bool TypeTraitsFloatingPoint<T>::equals(const T a, const T b) {
    /* Shortcut for binary equality (also infinites) */
    if(a == b) return true;

    const T absA = std::abs(a);
    const T absB = std::abs(b);
    const T difference = std::abs(a - b);

    /* One of the numbers is zero or both are extremely close to it, relative
       error is meaningless */
    if(a == T{} || b == T{} || difference < TypeTraits<T>::epsilon())
        return difference < TypeTraits<T>::epsilon();

    /* Relative error */
    return difference/(absA + absB) < TypeTraits<T>::epsilon();
}

template<class T> bool TypeTraitsFloatingPoint<T>::equalsZero(const T a, const T magnitude) {
    /* Shortcut for binary equality */
    if(a == T(0.0)) return true;

    const T absA = std::abs(a);

    /* The value is extremely close to zero, relative error is meaningless */
    if(absA < TypeTraits<T>::epsilon())
        return absA < TypeTraits<T>::epsilon();

    /* Relative error */
    return absA*T(0.5)/magnitude < TypeTraits<T>::epsilon();
}

}

template<> struct TypeTraits<Float>: Implementation::TypeTraitsFloatingPoint<Float> {
    typedef Float FloatingPointType;

    constexpr static Float epsilon() { return FLOAT_EQUALITY_PRECISION; }
};
template<> struct TypeTraits<Double>: Implementation::TypeTraitsFloatingPoint<Double> {
    typedef Double FloatingPointType;

    constexpr static Double epsilon() { return DOUBLE_EQUALITY_PRECISION; }
};
template<> struct TypeTraits<long double>: Implementation::TypeTraitsFloatingPoint<long double> {
    typedef long double FloatingPointType;

    constexpr static long double epsilon() { return LONG_DOUBLE_EQUALITY_PRECISION; }
};

namespace Implementation {

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
