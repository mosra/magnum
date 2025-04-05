#ifndef Magnum_Math_Functions_h
#define Magnum_Math_Functions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Nghia Truong <nghiatruong.vn@gmail.com>
    Copyright © 2020 Pablo Escobar <mail@rvrs.in>
    Copyright © 2020 janos <janos.meny@googlemail.com>
    Copyright © 2024 John Turner <7strbass@gmail.com>

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

#include <cstdlib> /* std::div() */
#include <type_traits>
#include <Corrade/Utility/Move.h>
#include <Corrade/Utility/StlMath.h>

#include "Magnum/visibility.h"
#include "Magnum/Math/Vector.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs returned std::pair before */
#include <Corrade/Containers/PairStl.h>
#endif

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

    template<class> struct IsBitVectorOrScalar: std::false_type {};
    template<> struct IsBitVectorOrScalar<bool>: std::true_type {};
    template<std::size_t size> struct IsBitVectorOrScalar<BitVector<size>>: std::true_type {};
}

/**
@brief Integer division with remainder

Example usage:

@snippet Math.cpp div

Equivalent to the following, but possibly done in a single CPU instruction:

@snippet Math.cpp div-equivalent
*/
template<class Integral> inline Containers::Pair<Integral, Integral> div(Integral x, Integral y) {
    static_assert(IsIntegral<Integral>::value && IsScalar<Integral>::value,
        "scalar integral type expected");
    const auto result = std::div(x, y);
    return {result.quot, result.rem};
}

/**
@brief [Binomial coefficient](https://en.wikipedia.org/wiki/Binomial_coefficient).
@m_since_latest

Returns the number of combinations of @f$ n @f$ things taken @f$ k @f$ at a
time, with @f$ n \ge k \ge 0 @f$: @f[
    \begin{pmatrix} n \\ k \end{pmatrix} =
        \frac{n! (n - k)!}{k!} =
        \frac{n (n - 1) (n - 2) ~ \cdots ~ (n - (k - 1))}{k (k - 1) ~ \cdots ~ 1} =
        \prod_{i=1}^k \frac{n + 1 - i}{i}
@f]
*/
UnsignedLong MAGNUM_EXPORT binomialCoefficient(UnsignedInt n, UnsignedInt k);

/**
@brief Count of bits set in a number
@m_since_latest

Expands to `__builtin_popcount` / `__builtin_popcountll` on GCC and Clang, uses
the [Counting bits set, in parallel](https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel)
implementation from Sean Eron Anderson Bit Twiddling Hacks page on MSVC and
elsewhere.

See also @relativeref{Corrade,Containers::BitArrayView::count()} for a variant
optimized for running on large sequences of bits.
*/
/* Explicitly checking for Clang in addition to GCC to catch also clang-cl */
#if defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG)
inline UnsignedInt popcount(UnsignedInt number) {
    return __builtin_popcount(number);
}
#else
MAGNUM_EXPORT UnsignedInt popcount(UnsignedInt number);
#endif

/**
@overload
@m_since_latest
*/
#if defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG)
inline UnsignedInt popcount(UnsignedLong number) {
    return __builtin_popcountll(number);
}
#else
MAGNUM_EXPORT UnsignedInt popcount(UnsignedLong number);
#endif

/**
@{ @name Trigonometric functions

Unlike @ref std::sin() and friends, those take or return strongly-typed units
to prevent degrees being accidentally interpreted as radians and such. See
@ref Magnum::Math::Deg "Deg" and @ref Magnum::Math::Rad "Rad" for more
information.
*/

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

#if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG)
namespace Implementation {
    /* GCC builtin since 3.4 (https://stackoverflow.com/a/2742861),
       unfortunately neither Clang nor MSVC have any alternative which wouldn't
       involve inline assembly. */
    inline void sincos(Float rad, Float& sin, Float& cos) {
        __builtin_sincosf(rad, &sin, &cos);
    }
    inline void sincos(Double rad, Double& sin, Double& cos) {
        __builtin_sincos(rad, &sin, &cos);
    }
    inline void sincos(long double rad, long double& sin, long double& cos) {
        __builtin_sincosl(rad, &sin, &cos);
    }
    /* Assuming there's no other floating-point type */
}
#endif

/**
@brief Sine and cosine

On GCC, this uses the `__builtin_sincos` intrinsic (or its `f` / `l` suffixed
variants), which may be faster than calculating sine and cosine separately. On
other compilers this *might* result in the optimizer picking up the combined
instruction as well.
@see @ref sin(), @ref cos(), @ref sincos(const Dual<Rad<T>>&)
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> inline Containers::Pair<T, T> sincos(Rad<T> angle);
#else
template<class T> inline Containers::Pair<T, T> sincos(Unit<Rad, T> angle) {
    #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG)
    Containers::Pair<T, T> out{Magnum::NoInit};
    Implementation::sincos(T(angle), out.first(), out.second());
    return out;
    #else
    return {std::sin(T(angle)), std::cos(T(angle))};
    #endif
}
template<class T> inline Containers::Pair<T, T> sincos(Unit<Deg, T> angle) { return sincos(Rad<T>(angle)); }
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

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

/**
@{ @name Scalar/vector functions

These functions are overloaded for both scalar and vector types, including
@ref Magnum::Math::Deg "Deg" and @ref Magnum::Math::Rad "Rad". Scalar versions
function exactly as their possible STL equivalents, vector overloads perform
the operations component-wise.
*/

/**
@brief If given number is a positive or negative infinity
@m_since{2019,10}

@see @ref isNan(), @ref Constants::inf(),
    @ref isInf(const Containers::StridedArrayView1D<const T>&)
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline bool isInf(T value) {
    return std::isinf(UnderlyingTypeOf<T>(value));
}

/**
@overload
@m_since{2019,10}
*/
template<std::size_t size, class T> inline BitVector<size> isInf(const Vector<size, T>& value) {
    BitVector<size> out;
    for(std::size_t i = 0; i != size; ++i)
        if(Math::isInf(value[i])) out.set(i);
    return out;
}

/**
@brief If given number is a NaN
@m_since{2019,10}

Equivalent to @cpp value != value @ce.
@see @ref isInf(), @ref Constants::nan(),
    @ref isNan(const Containers::StridedArrayView1D<const T>&)
*/
/* defined in Vector.h */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type
    #endif
> bool isNan(T value);

/**
@overload
@m_since{2019,10}
*/
template<std::size_t size, class T> inline BitVector<size> isNan(const Vector<size, T>& value) {
    BitVector<size> out;
    for(std::size_t i = 0; i != size; ++i)
        if(Math::isNan(value[i])) out.set(i);
    return out;
}

/**
@brief Minimum

<em>NaN</em>s passed in the @p value parameter are propagated.
@see @ref max(), @ref minmax(), @ref clamp(),
    @ref min(const Containers::StridedArrayView1D<const T>&),
    @ref Vector::min(), @ref Utility::min()
*/
/* defined in Vector.h */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type
    #endif
> constexpr T min(T value, T min);

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> min(const Vector<size, T>& value, const Vector<size, T>& min) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::min(value[i], min[i]);
    return out;
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> min(const Vector<size, T>& value, T min) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::min(value[i], min);
    return out;
}

/**
@brief Maximum

<em>NaN</em>s passed in the @p value parameter are propagated.
@see @ref min(), @ref minmax(), @ref clamp(),
    @ref max(const Containers::StridedArrayView1D<const T>&),
    @ref Vector::max(), @ref Utility::max()
*/
/* defined in Vector.h */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type
    #endif
> constexpr T max(T a, T b);

/** @overload */
template<std::size_t size, class T> Vector<size, T> max(const Vector<size, T>& value, const Vector<size, T>& max) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::max(value[i], max[i]);
    return out;
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> max(const Vector<size, T>& value, T max) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::max(value[i], max);
    return out;
}

/**
@brief Minimum and maximum of two values

@see @ref min(), @ref max(), @ref clamp(),
    @ref minmax(const Containers::StridedArrayView1D<const T>&),
    @ref Vector::minmax(),
    @ref Range::Range(const Containers::Pair<VectorType, VectorType>&)
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline Containers::Pair<T, T> minmax(T a, T b) {
    return a < b ? Containers::pair(a, b) : Containers::pair(b, a);
}

/** @overload */
template<std::size_t size, class T> inline Containers::Pair<Vector<size, T>, Vector<size, T>> minmax(const Vector<size, T>& a, const Vector<size, T>& b) {
    using Utility::swap;
    Containers::Pair<Vector<size, T>, Vector<size, T>> out{a, b};
    for(std::size_t i = 0; i != size; ++i)
        if(out.first()[i] > out.second()[i]) swap(out.first()[i], out.second()[i]);
    return out;
}

/**
@brief Clamp value

Values smaller than @p min are set to @p min, values larger than @p max are
set to @p max. Equivalent to:

@snippet Math.cpp clamp

<em>NaN</em>s passed in @p value parameter are propagated.
@see @ref min(), @ref max()
*/
/* defined in Vector.h */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type
    #endif
> constexpr T clamp(T value, T min, T max);

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> clamp(const Vector<size, T>& value, const Vector<size, T>& min, const Vector<size, T>& max) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::clamp(value[i], min[i], max[i]);
    return out;
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> clamp(const Vector<size, T>& value, T min, T max) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::clamp(value[i], min, max);
    return out;
}

/**
@brief Sign

Returns `1` if @p x > 0, `0` if @p x = 0 and `-1` if @p x < 0.
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline UnderlyingTypeOf<T> sign(T scalar) {
    if(scalar > T(0)) return UnderlyingTypeOf<T>(1);
    if(scalar < T(0)) return UnderlyingTypeOf<T>(-1);
    return UnderlyingTypeOf<T>(0);
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, UnderlyingTypeOf<T>> sign(const Vector<size, T>& a) {
    Vector<size, UnderlyingTypeOf<T>> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::sign(a[i]);
    return out;
}

/** @brief Absolute value */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T abs(T a) {
    return T(std::abs(UnderlyingTypeOf<T>(a)));
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> abs(const Vector<size, T>& a) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::abs(a[i]);
    return out;
}

/** @brief Nearest not larger integer */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T floor(T a) {
    return T(std::floor(UnderlyingTypeOf<T>(a)));
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> floor(const Vector<size, T>& a) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::floor(a[i]);
    return out;
}

/** @brief Round value to nearest integer */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T round(T a) {
    return T(std::round(UnderlyingTypeOf<T>(a)));
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> round(const Vector<size, T>& a) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::round(a[i]);
    return out;
}

/** @brief Nearest not smaller integer */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T ceil(T a) {
    return T(std::ceil(UnderlyingTypeOf<T>(a)));
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> ceil(const Vector<size, T>& a) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::ceil(a[i]);
    return out;
}

/**
@brief Floating point division remainder
@param a     Numerator
@param b     Denumerator
@m_since_latest

Calculates the remainder @f$ r @f$ of a floating point division: @f[
    r = a - b ~ \operatorname{trunc}(\frac{a}{b})
@f]

@attention This function differs from the GLSL @glsl mod() @ce function when
    @f$ \frac{a}{b} @f$ is negative. The return value has the same sign as the
    numerator, whereas @glsl mod() @ce keeps the denumerator's sign.

@m_keyword{mod(),GLSL mod(),}
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T fmod(T a, T b) {
    return T(std::fmod(UnderlyingTypeOf<T>(a), UnderlyingTypeOf<T>(b)));
}

/**
@overload
@m_since_latest
*/
template<std::size_t size, class T> inline Vector<size, T> fmod(const Vector<size, T>& a, const Vector<size, T>& b) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::fmod(a[i], b[i]);
    return out;
}

/**
@overload
@m_since_latest
*/
template<std::size_t size, class T> inline Vector<size, T> fmod(const Vector<size, T>& a, T b) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::fmod(a[i], b);
    return out;
}

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
template<class T, class U
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<(IsVector<T>::value || IsScalar<T>::value) && !Implementation::IsBitVectorOrScalar<U>::value, int>::type = 0
    #endif
> inline T lerp(const T& a, const T& b, U t) {
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
@see @ref Vector::Vector(const BitVector<size>&)
*/
template<std::size_t size, class T> inline Vector<size, T> lerp(const Vector<size, T>& a, const Vector<size, T>& b, const BitVector<size>& t) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = t[i] ? b[i] : a[i];
    return out;
}

/** @overload
@m_keyword{mix(),GLSL mix(),}
*/
template<std::size_t size> inline BitVector<size> lerp(const BitVector<size>& a, const BitVector<size>& b, const BitVector<size>& t) {
    /* Not using NoInit because it causes some compilers to report unitialized
       value */
    BitVector<size> out;
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

Useful in combination with @ref lerp() for mapping values from one range to
another --- for example, the following snippet maps `a` from a range
@f$ [ -1; +1 ] @f$ to a range @f$ [ 5\degree; 15\degree ] @f$; the second
expression combines that with @ref clamp() to ensure the value is in bounds:

@snippet Math.cpp lerpInverted-map

@see @ref select()
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline UnderlyingTypeOf<T> lerpInverted(T a, T b, T lerp) {
    return (lerp - a)/(b - a);
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, UnderlyingTypeOf<T>> lerpInverted(const Vector<size, T>& a, const Vector<size, T>& b, const Vector<size, T>& lerp) {
    return (lerp - a)/(b - a);
}

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
    static_assert(IsUnitless<U>::value, "expecting a unitless type for the interpolation phase");
    return lerp(a, b, t >= U(1));
}

/**
@brief Fused multiply-add

Computes and returns @f$ ab + c @f$. On some architectures might be faster than
doing the computation manually. Works only on types that satisfy
@ref IsUnitless.
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T fma(T a, T b, T c) {
    static_assert(IsUnitless<T>::value, "expecting a unitless type");
    /* On Emscripten it works with -O2 but not with -O1 (function not defined).
       I guess that's only because -O2 optimizes it out, so disabling it there. */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return std::fma(a, b, c);
    #else
    return a*b + c;
    #endif
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> fma(const Vector<size, T>& a, const Vector<size, T>& b, const Vector<size, T>& c) {
    static_assert(IsUnitless<T>::value, "expecting a unitless type");
    return a*b + c;
}

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

/**
@{ @name Exponential and power functions

Unlike @m_class{m-doc} <a href="#scalarvector-functions">scalar/vector functions</a>
these don't work on @ref Magnum::Math::Deg "Deg" / @ref Magnum::Math::Rad "Rad"
as the resulting unit can't be easily expressed.
*/

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
@brief Integral power

Returns integral power of base to the exponent. Works only on types that
satisfy @ref IsUnitless.
@see @ref pow(T, T)
*/
template<UnsignedInt exponent, class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> constexpr T pow(T base) {
    static_assert(IsUnitless<T>::value, "expected a unitless type");
    return Implementation::Pow<exponent>::pow(base);
}

/** @overload */
template<UnsignedInt exponent, std::size_t size, class T> inline Vector<size, T> pow(const Vector<size, T>& base) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::pow<exponent>(base[i]);
    return out;
}

/**
@brief Power

Returns power of @p base to the @p exponent. Works only on types that satisfy
@ref IsUnitless.
@see @ref pow(T), @ref exp()
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T pow(T base, T exponent) {
    static_assert(IsUnitless<T>::value, "expected a unitless type");
    return std::pow(base, exponent);
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> pow(const Vector<size, T>& base, T exponent) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::pow(base[i], exponent);
    return out;
}

/**
@brief Square root

Works only on types that satisfy @ref IsUnitless.
@see @ref sqrtInverted(), @ref Vector::length(), @ref sqrt(const Dual<T>&)
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T sqrt(T a) {
    static_assert(IsUnitless<T>::value, "expecting a unitless type");
    return std::sqrt(a);
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> sqrt(const Vector<size, T>& a) {
    Vector<size, T> out{Magnum::NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = Math::sqrt(a[i]);
    return out;
}

/**
@brief Inverse square root

Works only on types that satisfy @ref IsUnitless.
@see @ref sqrt(), @ref Vector::lengthInverted()
@m_keyword{inversesqrt(),GLSL inversesqrt(),}
*/
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<IsScalar<T>::value, int>::type = 0
    #endif
> inline T sqrtInverted(T a) {
    static_assert(IsUnitless<T>::value, "expecting a unitless type");
    return T(1)/std::sqrt(a);
}

/** @overload */
template<std::size_t size, class T> inline Vector<size, T> sqrtInverted(const Vector<size, T>& a) {
    return Vector<size, T>(T(1))/Math::sqrt(a);
}

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

/**
@brief Reflect a vector
@m_since{2020,06}

Reflects the vector off a surface given the surface outward normal. Expects
that the normal vector is normalized. For a vector @f$ \boldsymbol{v} @f$ and a
normal @f$ \boldsymbol{n} @f$, the reflection vector @f$ \boldsymbol{r} @f$ is
calculated as: @f[
    \boldsymbol{r} = \boldsymbol{v} - 2 (\boldsymbol{n} \cdot \boldsymbol{v}) \boldsymbol{n}
@f]
@see @ref dot(const Vector<size, T>&, const Vector<size, T>&), @ref refract(),
    @ref Vector::isNormalized(), @ref Matrix3::reflection(),
    @ref Matrix4::reflection()
*/
template<std::size_t size, class T> inline Vector<size, T> reflect(const Vector<size, T>& vector, const Vector<size, T>& normal) {
    CORRADE_DEBUG_ASSERT(normal.isNormalized(),
        "Math::reflect(): normal" << normal << "is not normalized", {});
    return vector - T(2.0)*dot(vector, normal)*normal;
}

/**
@brief Refract a vector
@m_since{2020,06}

Refracts a vector through a medium given the surface outward normal and ratio
of indices of refraction eta. Expects that both @p vector and @p normal is
normalized. For a vector @f$ \boldsymbol{v} @f$, normal @f$ \boldsymbol{n} @f$
and a ratio of indices of refraction @f$ \eta @f$, the refraction vector
@f$ \boldsymbol{r} @f$ is calculated as: @f[
    \begin{array}{rcl}
        \eta & = & \cfrac{\text{IOR}_\text{source}}{\text{IOR}_\text{destination}} \\[10pt]
        k & = & 1 - \eta^2 (1 - (\boldsymbol{n} \cdot \boldsymbol{v})^2) \\
        \boldsymbol{r} & = & \begin{cases}
            \boldsymbol{0}, & \text{if} ~ k < 0 \\
            \eta \boldsymbol{v} - (\eta (\boldsymbol{n} \cdot \boldsymbol{v}) + \sqrt{k}) \boldsymbol{n}, & \text{if} ~ k \ge 0
        \end{cases}
    \end{array}
@f]

Wikipedia has a [List of refractive indices](https://en.wikipedia.org/wiki/List_of_refractive_indices).
@see @ref dot(const Vector<size, T>&, const Vector<size, T>&), @ref reflect(),
    @ref Vector::isNormalized()
*/
template<std::size_t size, class T> inline Vector<size, T> refract(const Vector<size, T>& vector, const Vector<size, T>& normal, T eta) {
    CORRADE_DEBUG_ASSERT(vector.isNormalized() && normal.isNormalized(),
        "Math::refract(): vectors" << vector << "and" << normal << "are not normalized", {});
    const T dot = Math::dot(vector, normal);
    const T k  = T(1.0) - eta*eta*(T(1.0) - dot*dot);
    if(k < T(0.0)) return {};
    return eta*vector - (eta*dot + std::sqrt(k))*normal;
}

}}

#endif
