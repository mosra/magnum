#ifndef Magnum_Math_Functions_h
#define Magnum_Math_Functions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Functions usable with scalar and vector types
 */

#include <cmath>
#include <cstdlib> /* std::div() */
#include <type_traits>
#include <utility>

#include "Magnum/visibility.h"
#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt exponent> struct Pow {
        Pow() = delete;

        template<class T> constexpr static T pow(T base) {
            return base*Pow<exponent-1>::pow(base);
        }
    };
    template<> struct Pow<0> {
        Pow() = delete;

        template<class T> constexpr static T pow(T) { return T(1); }
    };

    template<class> struct IsBoolVectorOrScalar: std::false_type {};
    template<> struct IsBoolVectorOrScalar<bool>: std::true_type {};
    template<std::size_t size> struct IsBoolVectorOrScalar<BoolVector<size>>: std::true_type {};

    template<class T> struct IsVectorOrScalar: std::is_arithmetic<T>::type {};
    template<template<class> class Derived, class T> struct IsVectorOrScalar<Unit<Derived, T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Deg<T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Rad<T>>: std::true_type {};
    template<std::size_t size, class T> struct IsVectorOrScalar<Vector<size, T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Vector2<T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Vector3<T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Vector4<T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Color3<T>>: std::true_type {};
    template<class T> struct IsVectorOrScalar<Color4<T>>: std::true_type {};
}

/**
@brief Integral logarithm

Returns integral logarithm of given number with given base.
@see @ref log2(), @ref log(T)
*/
UnsignedInt MAGNUM_EXPORT log(UnsignedInt base, UnsignedInt number);

/**
@brief Base-2 integral logarithm

Returns integral logarithm of given number with base `2`.
@see @ref log(UnsignedInt, UnsignedInt), @ref log(T)
*/
UnsignedInt MAGNUM_EXPORT log2(UnsignedInt number);

/**
@brief Natural logarithm

Returns natural (base @f$ e @f$) logarithm of given number.
@see @ref Constants::e(), @ref log(UnsignedInt, UnsignedInt), @ref log2()
*/
template<class T> inline T log(T number) { return std::log(number); }

/**
@brief Natural exponential

Returns @f$ e^x @f$.
@see @ref Constants::e(), @ref pow(T, T)
*/
template<class T> inline T exp(T exponent) { return std::exp(exponent); }

/**
@brief Integer division with remainder

Example usage:

@snippet MagnumMath.cpp div

Equivalent to the following, but possibly done in a single CPU instruction:

@snippet MagnumMath.cpp div-equivalent
*/
template<class Integral> inline std::pair<Integral, Integral> div(Integral x, Integral y) {
    static_assert(std::is_integral<Integral>{}, "Math::div(): not an integral type");
    const auto result = std::div(x, y);
    return {result.quot, result.rem};
}

/**
@brief If given number is a positive or negative infinity

@see @ref isNan(), @ref Constants::inf()
*/
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, bool>::type isInf(T value) {
    return std::isinf(value);
}

/** @overload */
template<std::size_t size, class T> inline BoolVector<size> isInf(const Vector<size, T>& value) {
    BoolVector<size> out;
    for(std::size_t i = 0; i != size; ++i)
        out.set(i, std::isinf(value[i]));
    return out;
}

/**
@brief If given number is a NaN

Equivalent to @cpp value != value @ce.
@see @ref isInf(), @ref Constants::nan()
*/
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, bool>::type isNan(T value) {
    return std::isnan(value);
}

/** @overload */
template<std::size_t size, class T> inline BoolVector<size> isNan(const Vector<size, T>& value) {
    BoolVector<size> out;
    for(std::size_t i = 0; i != size; ++i)
        out.set(i, std::isnan(value[i]));
    return out;
}

/** @todo Can't trigonometric functions be done with only one overload? */

/* The functions accept Unit instead of Rad to make them working with operator
   products (e.g. 2*35.0_degf, which is of type Unit) */

/**
@brief Sine

@see @ref sincos()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sin(Rad<T> angle);
#else
template<class T> inline T sin(Unit<Rad, T> angle) { return std::sin(T(angle)); }
template<class T> inline T sin(Unit<Deg, T> angle) { return sin(Rad<T>(angle)); }
#endif

/**
@brief Cosine

@see @ref sincos()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T cos(Rad<T> angle);
#else
template<class T> inline T cos(Unit<Rad, T> angle) { return std::cos(T(angle)); }
template<class T> inline T cos(Unit<Deg, T> angle) { return cos(Rad<T>(angle)); }
#endif

/**
@brief Sine and cosine

On some architectures might be faster than doing both computations separately.
@see @ref sin(), @ref cos(), @ref sincos(const Dual<Rad<T>>&)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline std::pair<T, T> sincos(Rad<T> angle);
#else
template<class T> inline std::pair<T, T> sincos(Unit<Rad, T> angle) {
    return {std::sin(T(angle)) ,std::cos(T(angle))};
}
template<class T> inline std::pair<T, T> sincos(Unit<Deg, T> angle) { return sincos(Rad<T>(angle)); }
#endif

/** @brief Tangent */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T tan(Rad<T> angle);
#else
template<class T> inline T tan(Unit<Rad, T> angle) { return std::tan(T(angle)); }
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
@brief Integral power

Returns integral power of base to the exponent.
@see @ref pow(T, T)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt exponent, class T> constexpr T pow(T base);
#else
template<UnsignedInt exponent, class T> constexpr typename std::enable_if<std::is_arithmetic<T>::value, T>::type pow(T base) {
    return Implementation::Pow<exponent>::pow(base);
}
template<UnsignedInt exponent, std::size_t size, class T> inline Vector<size, T> pow(const Vector<size, T>& base) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Implementation::Pow<exponent>::pow(base[i]);
    return out;
}
#endif

/**
@brief Power

Returns power of @p base to the @p exponent.
@see @ref pow(T), @ref exp()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> T pow(T base, T exponent);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type pow(T base, T exponent) {
    return std::pow(base, exponent);
}
template<std::size_t size, class T> inline Vector<size, T> pow(const Vector<size, T>& base, T exponent) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::pow(base[i], exponent);
    return out;
}
#endif

/**
@brief Minimum

<em>NaN</em>s passed in @p value parameter are propagated.
@see @ref max(), @ref minmax(), @ref clamp(),
    @ref min(Corrade::Containers::ArrayView<const T>), @ref Vector::min()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T min(T value, T min);
#else
/* min() for scalars defined in Vector.h */
template<std::size_t size, class T> inline Vector<size, T> min(const Vector<size, T>& value, const Vector<size, T>& min) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::min(value[i], min[i]);
    return out;
}
#endif

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> min(const Vector<size, T>& value, T min) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::min(value[i], min);
    return out;
}

/**
@brief Maximum

<em>NaN</em>s passed in @p value parameter are propagated.
@see @ref min(), @ref minmax(), @ref clamp(),
    @ref max(Corrade::Containers::ArrayView<const T>), @ref Vector::max()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T max(T value, T max);
#else
/* max() for scalars defined in Vector.h */
template<std::size_t size, class T> Vector<size, T> max(const Vector<size, T>& value, const Vector<size, T>& max) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::max(value[i], max[i]);
    return out;
}
#endif

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> max(const Vector<size, T>& value, T max) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::max(value[i], max);
    return out;
}

/**
@brief Minimum and maximum of two values

@see @ref min(), @ref max(), @ref clamp(),
    @ref minmax(Corrade::Containers::ArrayView<const T>), @ref Vector::minmax(),
    @ref Range::Range(const std::pair<VectorType, VectorType>&)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline std::pair<T, T> minmax(const T& a, const T& b);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, std::pair<T, T>>::type minmax(T a, T b) {
    return a < b ? std::make_pair(a, b) : std::make_pair(b, a);
}
template<std::size_t size, class T> inline std::pair<Vector<size, T>, Vector<size, T>> minmax(const Vector<size, T>& a, const Vector<size, T>& b) {
    using std::swap;
    std::pair<Vector<size, T>, Vector<size, T>> out{a, b};
    for(std::size_t i = 0; i != size; ++i)
        if(out.first[i] > out.second[i]) swap(out.first[i], out.second[i]);
    return out;
}
#endif

/**
@brief Clamp value

Values smaller than @p min are set to @p min, values larger than @p max are
set to @p max. Equivalent to:

@snippet MagnumMath.cpp clamp

<em>NaN</em>s passed in @p value parameter are propagated.
@see @ref min(), @ref max()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class U> inline T clamp(const T& value, const T& min, const T& max);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type clamp(T value, T min, T max) {
    return Math::min(Math::max(value, min), max);
}
template<std::size_t size, class T> inline Vector<size, T> clamp(const Vector<size, T>& value, const Vector<size, T>& min, const Vector<size, T>& max) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = clamp(value[i], min[i], max[i]);
    return out;
}
#endif

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> clamp(const Vector<size, T>& value, T min, T max) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = clamp(value[i], min, max);
    return out;
}

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
template<std::size_t size, class T> inline Vector<size, T> sign(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
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
template<std::size_t size, class T> inline Vector<size, T> abs(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::abs(a[i]);
    return out;
}
#endif

/** @brief Nearest not larger integer */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T floor(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type floor(T a) {
    return std::floor(a);
}
template<std::size_t size, class T> inline Vector<size, T> floor(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::floor(a[i]);
    return out;
}
#endif

/** @brief Round value to nearest integer */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T round(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type round(T a) {
    return std::round(a);
}
template<std::size_t size, class T> inline Vector<size, T> round(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::round(a[i]);
    return out;
}
#endif

/** @brief Nearest not smaller integer */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T ceil(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type ceil(T a) {
    return std::ceil(a);
}
template<std::size_t size, class T> inline Vector<size, T> ceil(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = std::ceil(a[i]);
    return out;
}
#endif

/**
@brief Square root

@see @ref sqrtInverted(), @ref Vector::length(), @ref sqrt(const Dual<T>&)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sqrt(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type sqrt(T a) {
    return T(std::sqrt(a));
}
template<std::size_t size, class T> inline Vector<size, T> sqrt(const Vector<size, T>& a) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = T(std::sqrt(a[i]));
    return out;
}
#endif

/**
@brief Inverse square root

@see @ref sqrt(), @ref Vector::lengthInverted()
@m_keyword{inversesqrt(),GLSL inversesqrt(),}
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T sqrtInverted(const T& a);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type sqrtInverted(T a) {
    return T(1)/std::sqrt(a);
}
template<std::size_t size, class T> inline Vector<size, T> sqrtInverted(const Vector<size, T>& a) {
    return Vector<size, T>(T(1))/sqrt(a);
}
#endif

/**
@brief Linear interpolation of two values
@param a     First value
@param b     Second value
@param t     Interpolation phase (from range @f$ [0; 1] @f$)

The interpolation for vectors is done as in following, similarly for scalars: @f[
    \boldsymbol{v_{LERP}} = (1 - t) \boldsymbol{v_A} + t \boldsymbol{v_B}
@f]

See @ref select() for constant interpolation using the same API and
@ref splerp() for spline interpolation.
@see @ref lerpInverted(), @ref lerp(const Complex<T>&, const Complex<T>&, T),
    @ref lerp(const Quaternion<T>&, const Quaternion<T>&, T),
    @ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
@m_keyword{mix(),GLSL mix(),}
*/
template<class T, class U> inline
    #ifndef DOXYGEN_GENERATING_OUTPUT
    typename std::enable_if<Implementation::IsVectorOrScalar<T>::value && !Implementation::IsBoolVectorOrScalar<U>::value, T>::type
    #else
    T
    #endif
lerp(const T& a, const T& b, U t) {
    return Implementation::lerp(a, b, t);
}

/** @overload
@m_keyword{mix(),GLSL mix(),}
*/
template<class T> inline T lerp(const T& a, const T& b, bool t) {
    return t ? b : a;
}

/** @overload
Similar to the above, but instead of multiplication and addition it just does
component-wise selection from either @p a or @p b based on values in @p t.
@m_keyword{mix(),GLSL mix(),}
*/
template<std::size_t size, class T> inline Vector<size, T> lerp(const Vector<size, T>& a, const Vector<size, T>& b, const BoolVector<size>& t) {
    Vector<size, T> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = t[i] ? b[i] : a[i];
    return out;
}

/** @overload
@m_keyword{mix(),GLSL mix(),}
*/
template<std::size_t size> inline BoolVector<size> lerp(const BoolVector<size>& a, const BoolVector<size>& b, const BoolVector<size>& t) {
    /* Not using NoInit because it causes some compilers to report unitialized
       value */
    BoolVector<size> out;
    for(std::size_t i = 0; i != size; ++i)
        out.set(i, t[i] ? b[i] : a[i]);
    return out;
}

/**
@brief Inverse linear interpolation of two values
@param a    First value
@param b    Second value
@param lerp Interpolated value

Returns interpolation phase *t*: @f[
    t = \frac{\boldsymbol{v_{LERP}} - \boldsymbol{v_A}}{\boldsymbol{v_B} - \boldsymbol{v_A}}
@f]
@see @ref lerp(), @ref select()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T lerpInverted(const T& a, const T& b, const T& lerp);
#else
template<class T> inline T lerpInverted(T a, T b, T lerp) {
    return (lerp - a)/(b - a);
}
template<std::size_t size, class T, class U> inline Vector<size, T> lerpInverted(const Vector<size, T>& a, const Vector<size, T>& b, const Vector<size, T>& lerp) {
    return (lerp - a)/(b - a);
}
#endif

/**
@brief Constant interpolation of two values
@param a     First value
@param b     Second value
@param t     Interpolation phase

A constant interpolation counterpart to @ref lerp(): @f[
    \boldsymbol{v}_i = \begin{cases}
        \boldsymbol{v_A}_i, & t_i < 1 \\
        \boldsymbol{v_B}_i, & t_i \ge 1
    \end{cases}
@f]

Equivalent to calling @cpp Math::lerp(a, b, t >= U(1)) @ce.
*/
template<class T, class U> constexpr T select(const T& a, const T& b, U t) {
    return lerp(a, b, t >= U(1));
}

/**
@brief Fused multiply-add

Computes and returns @f$ ab + c @f$. On some architectures might be faster than
doing the computation manually.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline T fma(const T& a, const T& b, const T& c);
#else
template<class T> inline typename std::enable_if<std::is_arithmetic<T>::value, T>::type fma(T a, T b, T c) {
    /* On Emscripten it works with -O2 but not with -O1 (function not defined).
       I guess that's only because -O2 optimizes it out, so disabling it there. */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return std::fma(a, b, c);
    #else
    return a*b + c;
    #endif
}
template<std::size_t size, class T> inline Vector<size, T> fma(const Vector<size, T>& a, const Vector<size, T>& b, const Vector<size, T>& c) {
    return a*b + c;
}
#endif

/*@}*/

}}

#endif
