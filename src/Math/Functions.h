#ifndef Magnum_Math_Functions_h
#define Magnum_Math_Functions_h
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

#include <cmath>
#include <type_traits>
#include <limits>

#include "Math/Vector.h"

#include "magnumVisibility.h"

/** @file
 * @brief Functions usable with scalar and vector types
 */

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<UnsignedInt exponent> struct Pow {
        Pow() = delete;

        template<class T> inline constexpr static T pow(T base) {
            return base*Pow<exponent-1>::pow(base);
        }
    };
    template<> struct Pow<0> {
        Pow() = delete;

        template<class T> inline constexpr static T pow(T) { return 1; }
    };
}
#endif

/**
 * @brief Integral power
 *
 * Returns integral power of base to the exponent.
 */
template<UnsignedInt exponent, class T> inline constexpr T pow(T base) {
    return Implementation::Pow<exponent>::pow(base);
}

/**
 * @brief Base-2 integral logarithm
 *
 * Returns integral logarithm of given number with base `2`.
 * @see log()
 */
UnsignedInt MAGNUM_EXPORT log2(UnsignedInt number);

/**
 * @brief Integral logarithm
 *
 * Returns integral logarithm of given number with given base.
 * @see log2()
 */
UnsignedInt MAGNUM_EXPORT log(UnsignedInt base, UnsignedInt number);

/** @todo Can't trigonometric functions be done with only one overload? */

/** @brief Sine */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sin(Rad<T> angle);
#else
template<class T> inline T sin(Unit<Rad, T> angle) { return std::sin(angle.toUnderlyingType()); }
template<class T> inline T sin(Unit<Deg, T> angle) { return sin(Rad<T>(angle)); }
#endif

/** @brief Cosine */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T cos(Rad<T> angle);
#else
template<class T> inline T cos(Unit<Rad, T> angle) { return std::cos(angle.toUnderlyingType()); }
template<class T> inline T cos(Unit<Deg, T> angle) { return cos(Rad<T>(angle)); }
#endif

/** @brief Tangent */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T tan(Rad<T> angle);
#else
template<class T> inline T tan(Unit<Rad, T> angle) { return std::tan(angle.toUnderlyingType()); }
template<class T> inline T tan(Unit<Deg, T> angle) { return tan(Rad<T>(angle)); }
#endif

/** @brief Arc sine */
template<class T> inline Rad<T> asin(T value) { return Rad<T>(std::asin(value)); }

/** @brief Arc cosine */
template<class T> inline Rad<T> acos(T value) { return Rad<T>(std::acos(value)); }

/** @brief Arc tangent */
template<class T> inline Rad<T> atan(T value) { return Rad<T>(std::atan(value)); }

/**
@{ @name Scalar/vector functions

These functions are overloaded for both scalar and vector types. Scalar
versions function exactly as their possible STL equivalents, vector overloads
perform the operations component-wise.
*/

/**
@brief Minimum

@see min(), clamp(), Vector::min()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T min(T a, T b);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type min(T a, T b) {
    return std::min(a, b);
}
template<std::size_t size, class T> inline Vector<size, T> min(const Vector<size, T>& a, const Vector<size, T>& b) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::min(a[i], b[i]);
    return out;
}
#endif

/**
@brief Maximum

@see max(), clamp(), Vector::max()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T max(const T& a, const T& b);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type max(T a, T b) {
    return std::max(a, b);
}
template<std::size_t size, class T> Vector<size, T> max(const Vector<size, T>& a, const Vector<size, T>& b) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::max(a[i], b[i]);
    return out;
}
#endif

/**
@brief Sign

Returns `1` if @p x > 0, `0` if @p x = 0 and `-1` if @p x < 0.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sign(const T scalar);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type sign(const T& scalar) {
    if(scalar > T(0)) return T(1);
    if(scalar < T(0)) return T(-1);
    return T(0);
}
template<std::size_t size, class T> Vector<size, T> sign(const Vector<size, T>& a) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = sign(a[i]);
    return out;
}
#endif

/** @brief Absolute value */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T abs(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type abs(T a) {
    return std::abs(a);
}
template<std::size_t size, class T> Vector<size, T> abs(const Vector<size, T>& a) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::abs(a[i]);
    return out;
}
#endif

/**
@brief Square root

@see sqrtInverted(), Vector::length()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sqrt(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type sqrt(T a) {
    return std::sqrt(a);
}
template<std::size_t size, class T> Vector<size, T> sqrt(const Vector<size, T>& a) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::sqrt(a[i]);
    return out;
}
#endif

/**
@brief Inverse square root

@see sqrt(), Vector::lengthInverted()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sqrtInverted(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type sqrtInverted(T a) {
    return T(1)/std::sqrt(a);
}
template<std::size_t size, class T> Vector<size, T> sqrtInverted(const Vector<size, T>& a) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = T(1)/std::sqrt(a[i]);
    return out;
}
#endif

/**
@brief Clamp value

Values smaller than @p min are set to @p min, values larger than @p max are
set to @p max.
@see min(), max()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class U> inline T clamp(const T& value, U min, U max);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type clamp(T value, T min, T max) {
    return std::min(std::max(value, min), max);
}
template<std::size_t size, class T> Vector<size, T> clamp(const Vector<size, T>& value, T min, T max) {
    Vector<size, T> out;
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::min(std::max(value[i], min), max);
    return out;
}
#endif

/**
@brief Linear interpolation of two values
@param a     First value
@param b     Second value
@param t     Interpolation phase (from range @f$ [0; 1] @f$)

The interpolation for vectors is done as in following, similarly for scalars: @f[
    \boldsymbol v_{LERP} = (1 - t) \boldsymbol v_A + t \boldsymbol v_B
@f]
@see Quaternion::lerp()
@todo http://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
    (when SIMD is in place)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class U> inline T lerp(const T& a, const T& b, U t);
#else
template<class T, class U> inline T lerp(T a, T b, U t) {
    return (U(1) - t)*a + t*b;
}
template<std::size_t size, class T, class U> inline Vector<size, T> lerp(const Vector<size, T>& a, const Vector<size, T>& b, U t) {
    return (U(1) - t)*a + t*b;
}
#endif

/**
@brief Fused multiply-add

Computes and returns @f$ ab + c @f$.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T fma(const T& a, const T& b, const T& c);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type fma(T a, T b, T c) {
    return std::fma(a, b, c);
}
template<std::size_t size, class T> inline Vector<size, T> fma(const Vector<size, T>& a, const Vector<size, T>& b, const Vector<size, T>& c) {
    return a*b + c;
}
#endif

/**
@brief Normalize integral value

Converts integral value from full range of given *unsigned* integral type to
value in range @f$ [0, 1] @f$ or from *signed* integral to range @f$ [-1, 1] @f$.

@note For best precision, resulting `FloatingPoint` type should be always
    larger that `Integral` type (e.g. Double from Int, LongDouble from Long and
    similarly for vector types).

@attention To ensure the integral type is correctly detected when using
    literals, this function should be called with both template parameters
    explicit, e.g.:
@code
// Literal type is (signed) char, but we assumed unsigned char, a != 1.0f
Float a = normalize<Float>('\xFF');

// b = 1.0f
Float b = normalize<Float, UnsignedByte>('\xFF');
@endcode

@see denormalize()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class FloatingPoint, class Integral> inline FloatingPoint normalize(const Integral& value);
#else
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_unsigned<Integral>::value, FloatingPoint>::type normalize(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "Math::normalize(): normalization must be done from integral to floating-point type");
    return value/FloatingPoint(std::numeric_limits<Integral>::max());
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_signed<Integral>::value, FloatingPoint>::type normalize(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "Math::normalize(): normalization must be done from integral to floating-point type");
    return Math::max(value/FloatingPoint(std::numeric_limits<Integral>::max()), FloatingPoint(-1));
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_unsigned<typename Integral::Type>::value, FloatingPoint>::type normalize(const Integral& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "Math::normalize(): normalization must be done from integral to floating-point type");
    return FloatingPoint(value)/typename FloatingPoint::Type(std::numeric_limits<typename Integral::Type>::max());
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_signed<typename Integral::Type>::value, FloatingPoint>::type normalize(const Integral& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "Math::normalize(): normalization must be done from integral to floating-point type");
    return Math::max(FloatingPoint(value)/typename FloatingPoint::Type(std::numeric_limits<typename Integral::Type>::max()), FloatingPoint(-1));
}
#endif

/**
@brief Denormalize floating-point value

Converts floating-point value in range @f$ [0, 1] @f$ to full range of given
*unsigned* integral type or range @f$ [-1, 1] @f$ to full range of given *signed*
integral type.

@note For best precision, `FloatingPoint` type should be always larger that
    resulting `Integral` type (e.g. Double to Int, LongDouble to Long and
    similarly for vector types).

@attention Return value for floating point numbers outside the normalized
    range is undefined.

@see normalize()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Integral, class FloatingPoint> inline Integral denormalize(const FloatingPoint& value);
#else
template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_arithmetic<FloatingPoint>::value, Integral>::type denormalize(FloatingPoint value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "Math::denormalize(): denormalization must be done from floating-point to integral type");
    return value*std::numeric_limits<Integral>::max();
}
template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_arithmetic<typename Integral::Type>::value, Integral>::type denormalize(const FloatingPoint& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "Math::denormalize(): denormalization must be done from floating-point to integral type");
    return Integral(value*std::numeric_limits<typename Integral::Type>::max());
}
#endif

/*@}*/

}}

#endif
