#ifndef Magnum_Math_Color_h
#define Magnum_Math_Color_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Math::Color3, @ref Magnum::Math::Color4, literal @link Magnum::Math::Literals::ColorLiterals::operator""_rgb() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_rgba() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_rgbf() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_rgbaf() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_srgb() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_srgba() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_srgbf() @endlink, @link Magnum::Math::Literals::ColorLiterals::operator""_srgbaf() @endlink
 */

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Vector4.h"

#if defined(CORRADE_MSVC2017_COMPATIBILITY) && !defined(CORRADE_MSVC2015_COMPATIBILITY)
/* Needed by the fullChannel() workaround */
#include "Magnum/Math/Half.h"
#endif

namespace Magnum { namespace Math {

namespace Implementation {

/* Convert color from HSV */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> Color3<T> fromHsv(ColorHsv<T> hsv) {
    /* Remove repeats */
    hsv.hue -= floor(T(hsv.hue)/T(360))*Deg<T>(360);
    if(hsv.hue < Deg<T>(0)) hsv.hue += Deg<T>(360);

    int h = int(T(hsv.hue)/T(60)) % 6;
    T f = T(hsv.hue)/T(60) - h;

    T p = hsv.value * (T(1) - hsv.saturation);
    T q = hsv.value * (T(1) - f*hsv.saturation);
    T t = hsv.value * (T(1) - (T(1) - f)*hsv.saturation);

    switch(h) {
        case 0: return {hsv.value, t, p};
        case 1: return {q, hsv.value, p};
        case 2: return {p, hsv.value, t};
        case 3: return {p, q, hsv.value};
        case 4: return {t, p, hsv.value};
        case 5: return {hsv.value, p, q};
        default: CORRADE_INTERNAL_DEBUG_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color3<T> fromHsv(const ColorHsv<typename TypeTraits<T>::FloatingPointType>& hsv) {
    return pack<Color3<T>>(fromHsv<typename TypeTraits<T>::FloatingPointType>(hsv));
}

/* Internal hue computing function */
template<class T> Deg<T> hue(const Color3<T>& color, T max, T delta) {
    T deltaInv60 = T(60)/delta;

    T hue(0);
    if(delta != T(0)) {
        if(max == color.r())
            hue = (color.g()-color.b())*deltaInv60 + (color.g() < color.b() ? T(360) : T(0));
        else if(max == color.g())
            hue = (color.b()-color.r())*deltaInv60 + T(120);
        else /* max == color.b() */
            hue = (color.r()-color.g())*deltaInv60 + T(240);
    }

    return Deg<T>(hue);
}

/* Hue, saturation, value for floating-point types */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline Deg<T> hue(const Color3<T>& color) {
    T max = color.max();
    T delta = max - color.min();
    return hue(color, max, delta);
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline T saturation(const Color3<T>& color) {
    T max = color.max();
    T delta = max - color.min();
    return max != T(0) ? delta/max : T(0);
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline T value(const Color3<T>& color) {
    return color.max();
}

/* Hue, saturation, value for integral types */
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Deg<typename Color3<T>::FloatingPointType> hue(const Color3<T>& color) {
    return hue<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline typename Color3<T>::FloatingPointType saturation(const Color3<T>& color) {
    return saturation<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline typename Color3<T>::FloatingPointType value(const Color3<T>& color) {
    return unpack<typename Color3<T>::FloatingPointType>(color.max());
}

/* Convert color to HSV */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline ColorHsv<T> toHsv(const Color3<T>& color) {
    T max = color.max();
    T delta = max - color.min();

    return ColorHsv<T>{hue<typename Color3<T>::FloatingPointType>(color, max, delta), max != T(0) ? delta/max : T(0), max};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline ColorHsv<typename TypeTraits<T>::FloatingPointType> toHsv(const Color3<T>& color) {
    return toHsv<typename TypeTraits<T>::FloatingPointType>(unpack<Color3<typename TypeTraits<T>::FloatingPointType>>(color));
}

/* sRGB -> RGB conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline Color3<T> fromSrgb(const Vector3<T>& srgb) {
    constexpr const T a(T(0.055));
    return lerp(srgb/T(12.92), pow((srgb + Vector3<T>{a})/(T(1.0) + a), T(2.4)), srgb > Vector3<T>(T(0.04045)));
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline Color4<T> fromSrgbAlpha(const Vector4<T>& srgbAlpha) {
    return {fromSrgb<T>(srgbAlpha.rgb()), srgbAlpha.a()};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color3<T> fromSrgb(const Vector3<typename Color3<T>::FloatingPointType>& srgb) {
    return pack<Color3<T>>(fromSrgb<typename Color3<T>::FloatingPointType>(srgb));
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color4<T> fromSrgbAlpha(const Vector4<typename Color4<T>::FloatingPointType>& srgbAlpha) {
    return {fromSrgb<T>(srgbAlpha.rgb()), pack<T>(srgbAlpha.a())};
}
template<class T, class Integral> inline Color3<T> fromSrgbIntegral(const Vector3<Integral>& srgb) {
    static_assert(IsIntegral<Integral>::value, "only conversion from different integral type is supported");
    return fromSrgb<T>(unpack<Vector3<typename Color3<T>::FloatingPointType>>(srgb));
}
template<class T, class Integral> inline Color4<T> fromSrgbAlphaIntegral(const Vector4<Integral>& srgbAlpha) {
    static_assert(IsIntegral<Integral>::value, "only conversion from different integral type is supported");
    return fromSrgbAlpha<T>(unpack<Vector4<typename Color4<T>::FloatingPointType>>(srgbAlpha));
}

/* Integer linear RGB -> linear RGB conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline Color3<T> fromLinearRgbInt(UnsignedInt linear) {
    return {unpack<T>(UnsignedByte(linear >> 16)),
            unpack<T>(UnsignedByte(linear >> 8)),
            unpack<T>(UnsignedByte(linear))};
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline Color4<T> fromLinearRgbaInt(UnsignedInt linear) {
    return {unpack<T>(UnsignedByte(linear >> 24)),
            unpack<T>(UnsignedByte(linear >> 16)),
            unpack<T>(UnsignedByte(linear >> 8)),
            unpack<T>(UnsignedByte(linear))};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color3<T> fromLinearRgbInt(UnsignedInt linear) {
    return {pack<T>(unpack<Float>(UnsignedByte(linear >> 16))),
            pack<T>(unpack<Float>(UnsignedByte(linear >> 8))),
            pack<T>(unpack<Float>(UnsignedByte(linear)))};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color4<T> fromLinearRgbaInt(UnsignedInt linear) {
    return {pack<T>(unpack<Float>(UnsignedByte(linear >> 24))),
            pack<T>(unpack<Float>(UnsignedByte(linear >> 16))),
            pack<T>(unpack<Float>(UnsignedByte(linear >> 8))),
            pack<T>(unpack<Float>(UnsignedByte(linear)))};
}

/* RGB -> sRGB conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> Vector3<typename Color3<T>::FloatingPointType> toSrgb(const Color3<T>& rgb) {
    constexpr const T a = T(0.055);
    return lerp(rgb*T(12.92), (T(1.0) + a)*pow(rgb, T(1.0)/T(2.4)) - Vector3<T>{a}, rgb > Vector3<T>(T(0.0031308)));
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> Vector4<typename Color4<T>::FloatingPointType> toSrgbAlpha(const Color4<T>& rgba) {
    return {toSrgb<T>(rgba.rgb()), rgba.a()};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Vector3<typename Color3<T>::FloatingPointType> toSrgb(const Color3<T>& rgb) {
    return toSrgb<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(rgb));
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Vector4<typename Color4<T>::FloatingPointType> toSrgbAlpha(const Color4<T>& rgba) {
    return {toSrgb<T>(rgba.rgb()), unpack<typename Color3<T>::FloatingPointType>(rgba.a())};
}
template<class T, class Integral> inline Vector3<Integral> toSrgbIntegral(const Color3<T>& rgb) {
    static_assert(IsIntegral<Integral>::value, "only conversion from different integral type is supported");
    return pack<Vector3<Integral>>(toSrgb<T>(rgb));
}
template<class T, class Integral> inline Vector4<Integral> toSrgbAlphaIntegral(const Color4<T>& rgba) {
    static_assert(IsIntegral<Integral>::value, "only conversion from different integral type is supported");
    return pack<Vector4<Integral>>(toSrgbAlpha<T>(rgba));
}

/* Linear RGB -> integer linear RGB conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline UnsignedInt toLinearRgbInt(const Color3<T>& linear) {
    return (pack<UnsignedByte>(linear[0]) << 16) |
           (pack<UnsignedByte>(linear[1]) << 8) |
            pack<UnsignedByte>(linear[2]);
}
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> inline UnsignedInt toLinearRgbaInt(const Color4<T>& linear) {
    return (pack<UnsignedByte>(linear[0]) << 24) |
           (pack<UnsignedByte>(linear[1]) << 16) |
           (pack<UnsignedByte>(linear[2]) << 8) |
            pack<UnsignedByte>(linear[3]);
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline UnsignedInt toLinearRgbInt(const Color3<T>& linear) {
    return (pack<UnsignedByte>(unpack<Float>(linear[0])) << 16) |
           (pack<UnsignedByte>(unpack<Float>(linear[1])) << 8) |
            pack<UnsignedByte>(unpack<Float>(linear[2]));
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline UnsignedInt toLinearRgbaInt(const Color4<T>& linear) {
    return (pack<UnsignedByte>(unpack<Float>(linear[0])) << 24) |
           (pack<UnsignedByte>(unpack<Float>(linear[1])) << 16) |
           (pack<UnsignedByte>(unpack<Float>(linear[2])) << 8) |
            pack<UnsignedByte>(unpack<Float>(linear[3]));
}

/* CIE XYZ -> RGB conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> Color3<T> fromXyz(const Vector3<T>& xyz) {
    /* Taken from https://en.wikipedia.org/wiki/Talk:SRGB#Rounded_vs._Exact,
       the rounded matrices from the main article don't round-trip perfectly */
    return Matrix3x3<T>{
        Vector3<T>{T(12831)/T(3959), T(-851781)/T(878810), T(705)/T(12673)},
        Vector3<T>{T(-329)/T(214), T(1648619)/T(878810), T(-2585)/T(12673)},
        Vector3<T>{T(-1974)/T(3959), T(36519)/T(878810), T(705)/T(667)}}*xyz;
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Color3<T> fromXyz(const Vector3<typename Color3<T>::FloatingPointType>& xyz) {
    return pack<Color3<T>>(fromXyz<typename Color3<T>::FloatingPointType>(xyz));
}

/* RGB -> CIE XYZ conversion */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> Vector3<typename Color3<T>::FloatingPointType> toXyz(const Color3<T>& rgb) {
    /* Taken from https://en.wikipedia.org/wiki/Talk:SRGB#Rounded_vs._Exact,
       the rounded matrices from the main article don't round-trip perfectly */
    return (Matrix3x3<T>{
        Vector3<T>{T(506752)/T(1228815), T(87098)/T(409605), T(7918)/T(409605)},
        Vector3<T>{T(87881)/T(245763), T(175762)/T(245763), T(87881)/T(737289)},
        Vector3<T>{T(12673)/T(70218), T(12673)/T(175545), T(1001167)/T(1053270)}})*rgb;
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> inline Vector3<typename Color3<T>::FloatingPointType> toXyz(const Color3<T>& rgb) {
    return toXyz<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(rgb));
}

/* Alpha (un)premultiplication */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> constexpr Color4<T> premultiplied(const Color4<T>& color) {
    return {color.rgb()*color.a(), color.a()};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> constexpr Color4<T> premultiplied(const Color4<T>& color) {
    /* The + 0.5 is to round the value to nearest integer instead of flooring.
       Not using round() to have this constexpr. See premultipliedRoundtrip()
       for a verification this exactly matches pack()/unpack() behavior. */
    return {
        T(typename Color4<T>::FloatingPointType(color.r())*color.a()/bitMax<T>() + typename Color4<T>::FloatingPointType(0.5)),
        T(typename Color4<T>::FloatingPointType(color.g())*color.a()/bitMax<T>() + typename Color4<T>::FloatingPointType(0.5)),
        T(typename Color4<T>::FloatingPointType(color.b())*color.a()/bitMax<T>() + typename Color4<T>::FloatingPointType(0.5)),
        color.a()
    };
}

template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> constexpr Color4<T> unpremultiplied(const Color4<T>& color) {
    /* If alpha is zero, zero the RGB channels. Could keep them unchanged, but
       that would add unnecessary variation to the output. */
    return {color.a() == T(0) ? Color3<T>{} : color.rgb()/color.a(), color.a()};
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> constexpr Color4<T> unpremultiplied(const Color4<T>& color) {
    /* Additionally also clamp the RGB channels so the division doesn't go over
       1, as with the packed type it would result in overflow. The + 0.5 is to
       round the value to nearest integer instead of flooring. Not using
       round() to have this constexpr. Unlike premultiplied(), this does *not*
       match pack()/unpack() behavior as this leads to better precision,
       statistically speaking. See the unpremultipliedRoundtrip() test for
       details. */
    return color.a() == T(0) ? Color4<T>{} : Color4<T>{
        T(typename Color4<T>::FloatingPointType(min(color.r(), color.a()))*bitMax<T>()/color.a() + typename Color4<T>::FloatingPointType(0.5)),
        T(typename Color4<T>::FloatingPointType(min(color.g(), color.a()))*bitMax<T>()/color.a() + typename Color4<T>::FloatingPointType(0.5)),
        T(typename Color4<T>::FloatingPointType(min(color.b(), color.a()))*bitMax<T>()/color.a() + typename Color4<T>::FloatingPointType(0.5)),
        color.a()
    };
}

/* Value for full channel (1.0f for floats, 255 for unsigned byte) */
#if !defined(CORRADE_MSVC2017_COMPATIBILITY) || defined(CORRADE_MSVC2015_COMPATIBILITY)
/* MSVC 2017 since 15.8 crashes with the following at a constructor line that
   calls this function via a default parameter. This happens only when the
   /permissive- (yes, there's a dash at the end) flag is specified, which is
   projects created directly using VS (enabled by default since 15.5) but not
   projects using CMake. Not using SFINAE in this case makes it work. Minimal
   repro case here: https://twitter.com/czmosra/status/1039446378248896513 */
template<class T, typename std::enable_if<IsFloatingPoint<T>::value, int>::type = 0> constexpr T fullChannel() {
    return T(1.0);
}
template<class T, typename std::enable_if<IsIntegral<T>::value, int>::type = 0> constexpr T fullChannel() {
    return Implementation::bitMax<T>();
}
#else
template<class T> constexpr T fullChannel() { return bitMax<T>(); }
template<> constexpr float fullChannel<float>() { return 1.0f; }
template<> constexpr Half fullChannel<Half>() {
    /* This is 1.0_h, but expressible in a constexpr context */
    return Half{UnsignedShort{0x3c00}};
}
template<> constexpr double fullChannel<double>() { return 1.0; }
template<> constexpr long double fullChannel<long double>() { return 1.0l; }
#endif

}

/**
@brief Color in linear RGB color space

The class can store either a floating-point or an integral representation of a
linear RGB color. Colors in sRGB color space should not be used directly in
calculations --- they should be converted to linear RGB using @ref fromSrgb()
/ @ref fromSrgbInt(), calculation done on the linear representation and then
converted back to sRGB using @ref toSrgb() / @ref toSrgbInt().

Integral colors are assumed to be in a packed representation where the
@f$ [0.0, 1.0] @f$ range is mapped to @f$ [0, 2^b - 1] @f$ with @f$ b @f$ being
bit count of given integer type. Note that constructor conversion between
different types (like in @ref Vector classes) doesn't do any (un)packing, you
need to use either @ref pack() / @ref unpack(), the integer variants of
@ref toSrgb() / @ref fromSrgb() or @ref toSrgbInt() / @ref fromSrgbInt()
instead. For convenience, conversion from and to 8bpp representation without
sRGB conversion is possible with @ref fromLinearRgbInt() and
@ref toLinearRgbInt().

@snippet Math.cpp Color3

Conversion from and to HSV is done always using floating-point types, so hue
is always in range in range @f$ [0.0\degree, 360.0\degree] @f$, saturation and
value in range @f$ [0.0, 1.0] @f$.

@see @link Literals::ColorLiterals::operator""_rgb() @endlink,
    @link Literals::ColorLiterals::operator""_rgbf() @endlink,
    @link Literals::ColorLiterals::operator""_srgb() @endlink,
    @link Literals::ColorLiterals::operator""_srgbf() @endlink,
    @ref Color4, @ref Magnum::Color3, @ref Magnum::Color3h,
    @ref Magnum::Color3ub, @ref Magnum::Color3us
*/
/* Not using template specialization because some internal functions are
   impossible to explicitly instantiate */
template<class T> class Color3: public Vector3<T> {
    public:
        /**
         * @brief Corresponding floating-point type
         *
         * For HSV and other color spaces.
         */
        typedef typename TypeTraits<T>::FloatingPointType FloatingPointType;

        /**
         * @brief Red color
         *
         * Convenience alternative to e.g. @cpp Color3{red, 0.0f, 0.0f} @ce.
         * With floating-point underlying type equivalent to @ref Vector3::xAxis().
         * @see @ref green(), @ref blue(), @ref cyan()
         */
        constexpr static Color3<T> red(T red = Implementation::fullChannel<T>()) {
            return Vector3<T>::xAxis(red);
        }

        /**
         * @brief Green color
         *
         * Convenience alternative to e.g. @cpp Color3(0.0f, green, 0.0f) @ce.
         * With floating-point underlying type equivalent to @ref Vector3::yAxis().
         * @see @ref red(), @ref blue(), @ref magenta()
         */
        constexpr static Color3<T> green(T green = Implementation::fullChannel<T>()) {
            return Vector3<T>::yAxis(green);
        }

        /**
         * @brief Blue color
         *
         * Convenience alternative to e.g. @cpp Color3{0.0f, 0.0f, blue} @ce.
         * With a floating-point underlying type it's equivalent to
         * @ref Vector3::zAxis().
         * @see @ref red(), @ref green(), @ref yellow()
         */
        constexpr static Color3<T> blue(T blue = Implementation::fullChannel<T>()) {
            return Vector3<T>::zAxis(blue);
        }

        /**
         * @brief Cyan color
         *
         * Convenience alternative to e.g. @cpp Color3{red, 1.0f, 1.0f} @ce.
         * With a floating-point underlying type it's equivalent to
         * @ref Vector3::xScale().
         * @see @ref magenta(), @ref yellow(), @ref red()
         */
        constexpr static Color3<T> cyan(T red = T(0)) {
            return {red, Implementation::fullChannel<T>(), Implementation::fullChannel<T>()};
        }

        /**
         * @brief Magenta color
         *
         * Convenience alternative to e.g. @cpp Color3{1.0f, green, 1.0f} @ce.
         * With a floating-point underlying type it's equivalent to
         * @ref Vector3::yScale().
         * @see @ref cyan(), @ref yellow(), @ref green()
         */
        constexpr static Color3<T> magenta(T green = T(0)) {
            return {Implementation::fullChannel<T>(), green, Implementation::fullChannel<T>()};
        }

        /**
         * @brief Yellow color
         *
         * Convenience alternative to e.g. @cpp Color3{1.0f, 1.0f, blue} @ce.
         * With a floating-point underlying type it's equivalent to
         * @ref Vector3::zScale().
         * @see @ref cyan(), @ref magenta(), @ref red()
         */
        constexpr static Color3<T> yellow(T blue = T(0)) {
            return {Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), blue};
        }

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv   Color in HSV color space
         *
         * Hue is allowed to overflow the range @f$ [0.0\degree, 360.0\degree] @f$,
         * in which case it will be wrapped back to this range.
         * @see @ref toHsv()
         */
        static Color3<T> fromHsv(const ColorHsv<FloatingPointType>& hsv) {
            return Implementation::fromHsv<T>(hsv);
        }

        /**
         * @brief Create linear RGB color from sRGB representation
         * @param srgb  Color in sRGB color space
         *
         * Applies an inverse [sRGB curve](https://en.wikipedia.org/wiki/SRGB)
         * onto input, returning the input in linear RGB color space with D65
         * illuminant and 2° standard colorimetric observer. @f[
         *      \boldsymbol{c}_\mathrm{linear} = \begin{cases}
         *          \dfrac{\boldsymbol{c}_\mathrm{sRGB}}{12.92}, & \boldsymbol{c}_\mathrm{sRGB} \le 0.04045 \\
         *          \left( \dfrac{\boldsymbol{c}_\mathrm{sRGB} + 0.055}{1 + 0.055} \right)^{2.4}, & \boldsymbol{c}_\mathrm{sRGB} > 0.04045
         *      \end{cases}
         * @f]
         * @see @ref fromSrgb(const Vector3<Integral>&), @ref fromSrgbInt(),
         *      @link operator""_srgbf() @endlink, @ref toSrgb(),
         *      @ref Color4::fromSrgbAlpha()
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        static Color3<T> fromSrgb(const Vector3<FloatingPointType>& srgb) {
            return Implementation::fromSrgb<T>(srgb);
        }

        /** @overload
         * @brief Create linear RGB color from integral sRGB representation
         * @param srgb  Color in sRGB color space
         *
         * Useful in cases where you have for example an 8-bit sRGB
         * representation and want to create a floating-point linear RGB color
         * out of it:
         *
         * @snippet Math.cpp Color3-fromSrgb
         *
         * For conversion from a *linear* 24-bit representation (i.e, without
         * applying the sRGB curve), use @ref unpack():
         *
         * @snippet Math.cpp Color3-unpack
         *
         * @see @ref fromSrgbInt(), @link operator""_srgbf() @endlink,
         *      @ref Color4::fromSrgbAlpha(const Vector4<Integral>&)
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color3<T> fromSrgb(const Vector3<Integral>& srgb) {
            return Implementation::fromSrgbIntegral<T, Integral>(srgb);
        }

        /**
         * @brief Create linear RGB color from 24-bit sRGB representation
         * @param srgb  24-bit sRGB color
         * @m_since_latest
         *
         * See @ref fromSrgb() for more information and @ref toSrgbInt() for an
         * inverse operation. There's also a @link operator""_srgbf() @endlink
         * that does this conversion directly from hexadecimal literals. The
         * following two statements are equivalent:
         *
         * @snippet Math.cpp Color3-fromSrgbInt
         *
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * from endian-independent sRGB / linear representation use
         * @ref fromSrgb(const Vector3<Integral>&).
         * @see @ref fromLinearRgbInt(), @ref Color4::fromSrgbAlphaInt()
         */
        static Color3<T> fromSrgbInt(UnsignedInt srgb) {
            return fromSrgb<UnsignedByte>({UnsignedByte(srgb >> 16),
                                           UnsignedByte(srgb >> 8),
                                           UnsignedByte(srgb)});
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief fromSrgbInt()
         * @m_deprecated_since_latest Use @ref fromSrgbInt() instead.
         */
        CORRADE_DEPRECATED("use fromSrgInt() instead") static Color3<T> fromSrgb(UnsignedInt srgb) {
            return fromSrgbInt(srgb);
        }
        #endif

        /**
         * @brief Create linear RGB color from 24-bit linear representation
         * @param linear    24-bit linear RGB color
         * @m_since_latest
         *
         * Compared to @ref fromSrgbInt() *does not* peform a sRGB conversion
         * on the input. See @ref toLinearRgbInt() for an inverse operation,
         * there's also a @link operator""_rgbf() @endlink that does this
         * conversion directly from hexadecimal literals. The following two
         * statements are equivalent:
         *
         * @snippet Math.cpp Color3-fromLinearRgbInt
         *
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * from endian-independent linear RGB representation use
         * @ref unpack() "unpack<Color3>()" on a @ref Color3ub input.
         * @see @ref Color4::fromLinearRgbaInt()
         */
        static Color3<T> fromLinearRgbInt(UnsignedInt linear) {
            return Implementation::fromLinearRgbInt<T>(linear);
        }

        /**
         * @brief Create RGB color from [CIE XYZ representation](https://en.wikipedia.org/wiki/CIE_1931_color_space)
         * @param xyz   Color in CIE XYZ color space
         *
         * Applies transformation matrix, returning the input in linear
         * RGB color space with D65 illuminant and 2° standard colorimetric
         * observer. @f[
         *      \begin{bmatrix} R_\mathrm{linear} \\ G_\mathrm{linear} \\ B_\mathrm{linear} \end{bmatrix} =
         *      \begin{bmatrix}
         *          3.2406 & -1.5372 & -0.4986 \\
         *          -0.9689 & 1.8758 & 0.0415 \\
         *          0.0557 & -0.2040 & 1.0570
         *      \end{bmatrix} \begin{bmatrix} X \\ Y \\ Z \end{bmatrix}
         * @f]
         * @see @ref toXyz(), @ref toSrgb(), @ref xyYToXyz()
         */
        static Color3<T> fromXyz(const Vector3<FloatingPointType>& xyz) {
            return Implementation::fromXyz<T>(xyz);
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Color3(ZeroInitT).
         */
        constexpr /*implicit*/ Color3() noexcept: Vector3<T>{ZeroInit} {}

        /**
         * @brief Construct a zero color
         *
         * All components are set to zero.
         */
        constexpr explicit Color3(ZeroInitT) noexcept: Vector3<T>{ZeroInit} {}

        /** @copydoc Vector::Vector(Magnum::NoInitT) */
        explicit Color3(Magnum::NoInitT) noexcept: Vector3<T>{Magnum::NoInit} {}

        /**
         * @brief Gray constructor
         * @param rgb   RGB value
         */
        constexpr explicit Color3(T rgb) noexcept: Vector3<T>(rgb) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         */
        constexpr /*implicit*/ Color3(T r, T g, T b) noexcept: Vector3<T>(r, g, b) {}

        /**
         * @brief Constructor
         * @param rg    RG value
         * @param b     B value
         * @m_since_latest
         */
        constexpr /*implicit*/ Color3(const Vector<2, T>& rg, T b) noexcept: Vector3<T>{rg, b} {}

        /** @copydoc Vector::Vector(const T(&)[size_]) */
        #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
        template<std::size_t size_> constexpr explicit Color3(const T(&data)[size_]) noexcept: Vector3<T>{data} {}
        #else
        /* GCC 4.8 workaround, see the Vector base for details */
        constexpr explicit Color3(const T(&data)[3]) noexcept: Vector3<T>{data} {}
        #endif

        /**
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (un)packing, use either
         *      @ref pack() / @ref unpack() or the integer variants of
         *      @ref toSrgb() / @ref fromSrgb() instead. See class
         *      documentation for more information.
         */
        template<class U> constexpr explicit Color3(const Vector<3, U>& other) noexcept: Vector3<T>(other) {}

        /** @copydoc Vector::Vector(const BitVector<size>&) */
        constexpr explicit Color3(const BitVector3& other) noexcept: Vector3<T>{other} {}

        /** @brief Construct a color from external representation */
        template<class U, class =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::VectorConverter<3, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::VectorConverter<3, T, U>())
            #endif
            >
        constexpr explicit Color3(const U& other): Vector3<T>(Implementation::VectorConverter<3, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Color3(const Vector<3, T>& other) noexcept: Vector3<T>(other) {}

        /**
         * @brief Convert to HSV representation
         *
         * @see @ref hue(), @ref saturation(), @ref value(), @ref fromHsv()
         */
        ColorHsv<FloatingPointType> toHsv() const {
            return Implementation::toHsv<T>(*this);
        }

        /**
         * @brief Hue
         * @return Hue in range @f$ [0.0\degree, 360.0\degree] @f$.
         *
         * @see @ref saturation(), @ref value(), @ref toHsv(), @ref fromHsv()
         */
        Deg<FloatingPointType> hue() const {
            return Deg<FloatingPointType>(Implementation::hue<T>(*this));
        }

        /**
         * @brief Saturation
         * @return Saturation in range @f$ [0.0, 1.0] @f$.
         *
         * @see @ref hue(), @ref value(), @ref toHsv(), @ref fromHsv()
         */
        FloatingPointType saturation() const {
            return Implementation::saturation<T>(*this);
        }

        /**
         * @brief Value
         * @return Value in range @f$ [0.0, 1.0] @f$.
         *
         * @see @ref hue(), @ref saturation(), @ref toHsv(), @ref fromHsv()
         */
        FloatingPointType value() const {
            return Implementation::value<T>(*this);
        }

        /**
         * @brief Convert to sRGB representation
         *
         * Assuming the color is in linear RGB with D65 illuminant and 2°
         * standard colorimetric observer, applies a
         * [sRGB curve](https://en.wikipedia.org/wiki/SRGB) onto it, returning
         * the color represented in sRGB color space: @f[
         *      \boldsymbol{c}_\mathrm{sRGB} = \begin{cases}
         *          12.92 \boldsymbol{c}_\mathrm{linear}, & \boldsymbol{c}_\mathrm{linear} \le 0.0031308 \\
         *          (1 + 0.055) \boldsymbol{c}_\mathrm{linear}^{1/2.4} - 0.055, & \boldsymbol{c}_\mathrm{linear} > 0.0031308
         *      \end{cases}
         * @f]
         * @see @ref fromSrgb(), @ref toSrgbInt(), @ref Color4::toSrgbAlpha()
         */
        Vector3<FloatingPointType> toSrgb() const {
            return Implementation::toSrgb<T>(*this);
        }

        /** @overload
         * @brief Convert to integral sRGB representation
         *
         * Useful in cases where you have a floating-point linear RGB color and
         * want to create for example an 8-bit sRGB representation out of it:
         *
         * @snippet Math.cpp Color3-toSrgb
         *
         * For conversion to a *linear* 24-bit representation (i.e, without
         * applying the sRGB curve), use @ref pack():
         *
         * @snippet Math.cpp Color3-pack
         *
         * @see @ref toSrgbInt(), @ref Color4::toSrgbAlpha(),
         *      @ref toLinearRgbInt()
         */
        template<class Integral> Vector3<Integral> toSrgb() const {
            return Implementation::toSrgbIntegral<T, Integral>(*this);
        }

        /**
         * @brief Convert to 24-bit integral sRGB representation
         *
         * See @ref toSrgb() const for more information and @ref fromSrgbInt()
         * for an inverse operation. Note that the integral value is
         * endian-dependent (the red channel being in the *last* byte on
         * little-endian platforms), for conversion to an endian-independent
         * sRGB representation use @ref toSrgb() const "toSrgb<UnsignedByte>() const".
         * @see @ref toLinearRgbInt(), @ref Color4::toSrgbAlphaInt()
         */
        UnsignedInt toSrgbInt() const {
            const auto srgb = toSrgb<UnsignedByte>();
            return (srgb[0] << 16) | (srgb[1] << 8) | srgb[2];
        }

        /**
         * @brief Convert to 24-bit integral linear RGB representation
         *
         * Compared to @ref toSrgbInt() *does not* perform a sRGB conversion on
         * the output. See @ref fromLinearRgbInt() for an inverse operation.
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * to an endian-independent linear representation use
         * @ref pack() "pack<Color3ub>()".
         * @see @ref Color4::toLinearRgbaInt()
         */
        UnsignedInt toLinearRgbInt() const {
            return Implementation::toLinearRgbInt(*this);
        }

        /**
         * @brief Convert to [CIE XYZ representation](https://en.wikipedia.org/wiki/CIE_1931_color_space)
         *
         * Assuming the color is in linear RGB with D65 illuminant and 2°
         * standard colorimetric observer, applies transformation matrix,
         * returning the color in CIE XYZ color space. @f[
         *      \begin{bmatrix} X \\ Y \\ Z \end{bmatrix} =
         *      \begin{bmatrix}
         *          0.4124 & 0.3576 & 0.1805 \\
         *          0.2126 & 0.7152 & 0.0722 \\
         *          0.0193 & 0.1192 & 0.9505
         *      \end{bmatrix}
         *      \begin{bmatrix} R_\mathrm{linear} \\ G_\mathrm{linear} \\ B_\mathrm{linear} \end{bmatrix}
         * @f]
         *
         * Please note that @ref x(), @ref y() and @ref z() *do not* correspond
         * to primaries in CIE XYZ color space, but are rather aliases to
         * @ref r(), @ref g() and @ref b().
         * @see @ref fromXyz(), @ref fromSrgb(), @ref xyzToXyY()
         */
        Vector3<FloatingPointType> toXyz() const {
            return Implementation::toXyz<T>(*this);
        }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(3, Color3)
};

#ifdef CORRADE_MSVC2015_COMPATIBILITY
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(3, Color3)
#endif

/**
@brief Color in linear RGBA color space

See @ref Color3 for more information.
@see @link Literals::ColorLiterals::operator""_rgba() @endlink,
    @link Literals::ColorLiterals::operator""_rgbaf() @endlink,
    @link Literals::ColorLiterals::operator""_srgba() @endlink,
    @link Literals::ColorLiterals::operator""_srgbaf() @endlink,
    @ref Magnum::Color4, @ref Magnum::Color4h, @ref Magnum::Color4ub,
    @ref Magnum::Color4us
*/
/* Not using template specialization because some internal functions are
   impossible to explicitly instantiate */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class Color4: public Vector4<T> {
    public:
        /** @copydoc Color3::FloatingPointType */
        typedef typename Color3<T>::FloatingPointType FloatingPointType;

        /**
         * @brief Red color
         *
         * Convenience alternative to e.g. @cpp Color4{red, 0.0f, 0.0f, alpha} @ce.
         * @see @ref green(), @ref blue(), @ref cyan()
         */
        constexpr static Color4<T> red(T red = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {red, T(0), T(0), alpha};
        }

        /**
         * @brief Green color
         *
         * Convenience alternative to e.g. @cpp Color4{0.0f, green, 0.0f, alpha} @ce.
         * @see @ref red(), @ref blue(), @ref magenta()
         */
        constexpr static Color4<T> green(T green = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {T(0), green, T(0), alpha};
        }

        /**
         * @brief Blue color
         *
         * Convenience alternative to e.g. @cpp Color4{0.0f, 0.0f, blue, alpha} @ce.
         * @see @ref red(), @ref green(), @ref yellow()
         */
        constexpr static Color4<T> blue(T blue = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {T(0), T(0), blue, alpha};
        }

        /**
         * @brief Cyan color
         *
         * Convenience alternative to e.g. @cpp Color4{red, 1.0f, 1.0f, alpha} @ce.
         * @see @ref magenta(), @ref yellow(), @ref red()
         */
        constexpr static Color4<T> cyan(T red = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {red, Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), alpha};
        }

        /**
         * @brief Magenta color
         *
         * Convenience alternative to e.g. @cpp Color4{1.0f, green, 1.0f, alpha} @ce.
         * @see @ref cyan(), @ref yellow(), @ref green()
         */
        constexpr static Color4<T> magenta(T green = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {Implementation::fullChannel<T>(), green, Implementation::fullChannel<T>(), alpha};
        }

        /**
         * @brief Yellow color
         *
         * Convenience alternative to e.g. @cpp Color4{1.0f, 1.0f, blue, alpha} @ce.
         * @see @ref cyan(), @ref magenta(), @ref red()
         */
        constexpr static Color4<T> yellow(T blue = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), blue, alpha};
        }

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv   Color in HSV color space
         * @param a     Alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         *
         * Hue can overflow the range @f$ [0.0, 360.0] @f$.
         * @see @ref toHsv()
         */
        static Color4<T> fromHsv(const ColorHsv<FloatingPointType>& hsv, T a = Implementation::fullChannel<T>()) {
            return Color4<T>(Implementation::fromHsv<T>(hsv), a);
        }

        /**
         * @brief Create linear RGBA color from sRGB + alpha representation
         * @param srgbAlpha     Color in sRGB color space with linear alpha
         *
         * Applies inverse sRGB curve onto RGB channels of the input, alpha
         * channel is assumed to be linear. See @ref Color3::fromSrgb() for
         * more information.
         * @see @link operator""_srgbaf @endlink, @ref toSrgbAlpha()
         */
        /* Input is a Vector4 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        static Color4<T> fromSrgbAlpha(const Vector4<FloatingPointType>& srgbAlpha) {
            return {Implementation::fromSrgbAlpha<T>(srgbAlpha)};
        }

        /** @overload
         * @brief Create linear RGB color from integral sRGB + alpha representation
         * @param srgbAlpha Color in sRGB color space with linear alpha
         *
         * Useful in cases where you have for example an 8-bit sRGB + alpha
         * representation and want to create a floating-point linear RGBA color
         * out of it:
         *
         * @snippet Math.cpp Color4-fromSrgbAlpha
         *
         * For conversion from a *linear* 32-bit representation (i.e, without
         * applying the sRGB curve), use @ref unpack():
         *
         * @snippet Math.cpp Color4-unpack
         *
         * @see @ref fromSrgbAlphaInt(UnsignedInt)
         */
        /* Input is a Vector4 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color4<T> fromSrgbAlpha(const Vector4<Integral>& srgbAlpha) {
            return {Implementation::fromSrgbAlphaIntegral<T, Integral>(srgbAlpha)};
        }

        /**
         * @brief Create linear RGBA color from sRGB representation
         * @param srgb      Color in sRGB color space
         * @param a         Alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         *
         * Applies inverse sRGB curve onto RGB channels of the input. Alpha
         * value is taken as-is. See @ref Color3::fromSrgb() for more
         * information.
         * @see @link operator""_srgbaf @endlink, @ref toSrgbAlpha()
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        static Color4<T> fromSrgb(const Vector3<FloatingPointType>& srgb, T a = Implementation::fullChannel<T>()) {
            return {Implementation::fromSrgb<T>(srgb), a};
        }

        /** @overload
         * @brief Create linear RGB color from integral sRGB representation
         * @param srgb      Color in sRGB color space
         * @param a         Linear alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         *
         * Same as above, but with alpha as a separate parameter.
         * @see @ref fromSrgbInt(UnsignedInt, T)
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color4<T> fromSrgb(const Vector3<Integral>& srgb, T a = Implementation::fullChannel<T>()) {
            return {Implementation::fromSrgbIntegral<T, Integral>(srgb), a};
        }

        /**
         * @brief Create linear RGBA color from 32-bit sRGB + alpha representation
         * @param srgbAlpha     32-bit sRGB color with linear alpha
         * @m_since_latest
         *
         * See @ref Color3::fromSrgbInt() for more information and
         * @ref toSrgbAlphaInt() for an inverse operation. There's also a
         * @link operator""_srgbaf() @endlink that does this conversion
         * directly from hexadecimal literals. The following two statements are
         * equivalent:
         *
         * @snippet Math.cpp Color4-fromSrgbAlphaInt
         *
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * from an endian-independent sRGB / linear representation use
         * @ref fromSrgbAlpha(const Vector4<Integral>&) / @ref unpack().
         */
        static Color4<T> fromSrgbAlphaInt(UnsignedInt srgbAlpha) {
            return fromSrgbAlpha<UnsignedByte>({UnsignedByte(srgbAlpha >> 24),
                                                UnsignedByte(srgbAlpha >> 16),
                                                UnsignedByte(srgbAlpha >> 8),
                                                UnsignedByte(srgbAlpha)});
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief fromSrgbAlphaInt()
         * @m_deprecated_since_latest Use @ref fromSrgbAlphaInt() instead.
         */
        CORRADE_DEPRECATED("use fromSrgInt() instead") static Color4<T> fromSrgbAlpha(UnsignedInt srgb) {
            return fromSrgbAlphaInt(srgb);
        }
        #endif

        /**
         * @brief Create linear RGBA color from 24-bit sRGB + alpha representation
         * @param srgb      24-bit sRGB color
         * @param a         Linear alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         *
         * Same as above, but with alpha as a separate parameter.
         */
        static Color4<T> fromSrgbInt(UnsignedInt srgb, T a = Implementation::fullChannel<T>()) {
            return fromSrgb<UnsignedByte>({UnsignedByte(srgb >> 16),
                                           UnsignedByte(srgb >> 8),
                                           UnsignedByte(srgb)}, a);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief fromSrgbInt()
         * @m_deprecated_since_latest Use @ref fromSrgbInt() instead.
         */
        CORRADE_DEPRECATED("use fromSrgInt() instead") static Color4<T> fromSrgb(UnsignedInt srgb, T a = Implementation::fullChannel<T>()) {
            return fromSrgbInt(srgb, a);
        }
        #endif

        /**
         * @brief Create linear RGBA color from 32-bit linear representation
         * @param linear    32-bit linear RGBA color
         * @m_since_latest
         *
         * Compared to @ref fromSrgbAlphaInt() *does not* peform a sRGB
         * conversion on the input. See @ref toLinearRgbaInt() for an inverse
         * operation, there's also a @link operator""_rgbaf() @endlink that
         * does this conversion directly from hexadecimal literals. The
         * following two statements are equivalent:
         *
         * @snippet Math.cpp Color4-fromLinearRgbaInt
         *
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * from endian-independent linear RGBA representation use
         * @ref unpack() "unpack<Color4>()" on a @ref Color4ub input.
         * @see @ref Color3::fromLinearRgbInt()
         */
        static Color4<T> fromLinearRgbaInt(UnsignedInt linear) {
            return Implementation::fromLinearRgbaInt<T>(linear);
        }

        /**
         * @brief Create linear RGBA color from 24-bit linear RGB + alpha representation
         * @param linear    24-bit linear RGB color
         * @param a         Linear alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         * @m_since_latest
         *
         * Same as above, but with alpha as a separate parameter.
         */
        static Color4<T> fromLinearRgbInt(UnsignedInt linear, T a = Implementation::fullChannel<T>()) {
            return {Implementation::fromLinearRgbInt<T>(linear), a};
        }

        /**
         * @brief Create RGBA color from [CIE XYZ representation](https://en.wikipedia.org/wiki/CIE_1931_color_space)
         * @param xyz   Color in CIE XYZ color space
         * @param a     Alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         *
         * Applies transformation matrix, returning the input in linear RGB
         * color space. See @ref Color3::fromXyz() for more information.
         * @see @ref toXyz(), @ref toSrgbAlpha(), @ref xyYToXyz()
         */
        static Color4<T> fromXyz(const Vector3<FloatingPointType> xyz, T a = Implementation::fullChannel<T>()) {
            return {Implementation::fromXyz<T>(xyz), a};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Color4(ZeroInitT).
         */
        constexpr /*implicit*/ Color4() noexcept: Vector4<T>{ZeroInit} {}

        /**
         * @brief Construct a zero color
         *
         * All components are set to zero.
         */
        constexpr explicit Color4(ZeroInitT) noexcept: Vector4<T>{ZeroInit} {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Color4(Magnum::NoInitT) noexcept: Vector4<T>{Magnum::NoInit} {}

        /**
         * @copydoc Color3::Color3(T)
         * @param alpha Alpha value, defaults to @cpp 1.0 @ce for
         *      floating-point types and maximum positive value for integral
         *      types
         */
        constexpr explicit Color4(T rgb, T alpha = Implementation::fullChannel<T>()) noexcept: Vector4<T>(rgb, rgb, rgb, alpha) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         * @param a     A value, defaults to @cpp 1.0 @ce for floating-point
         *      types and maximum positive value for integral types.
         */
        constexpr /*implicit*/ Color4(T r, T g, T b, T a = Implementation::fullChannel<T>()) noexcept: Vector4<T>(r, g, b, a) {}

        /**
         * @brief Constructor
         * @param rgb   Three-component color
         * @param a     A value
         */
        /* Not marked as explicit, because conversion from Color3 to Color4
           is fairly common, nearly always with A set to 1 */
        constexpr /*implicit*/ Color4(const Vector3<T>& rgb, T a = Implementation::fullChannel<T>()) noexcept: Vector4<T>(rgb[0], rgb[1], rgb[2], a) {}

        /** @copydoc Vector::Vector(const T(&)[size_]) */
        #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
        template<std::size_t size_> constexpr explicit Color4(const T(&data)[size_]) noexcept: Vector4<T>{data} {}
        #else
        /* GCC 4.8 workaround, see the Vector base for details */
        constexpr explicit Color4(const T(&data)[4]) noexcept: Vector4<T>{data} {}
        #endif

        /**
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (un)packing, use either
         *      @ref pack() / @ref unpack() or the integer variants of
         *      @ref toSrgbAlpha() / @ref fromSrgbAlpha() instead. See
         *      @ref Color3 class documentation for more information.
         */
        template<class U> constexpr explicit Color4(const Vector<4, U>& other) noexcept: Vector4<T>(other) {}

        /** @copydoc Vector::Vector(const BitVector<size>&) */
        constexpr explicit Color4(const BitVector4& other) noexcept: Vector4<T>{other} {}

        /** @brief Construct a color from external representation */
        template<class U, class =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::VectorConverter<4, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::VectorConverter<4, T, U>())
            #endif
            >
        constexpr explicit Color4(const U& other): Vector4<T>(Implementation::VectorConverter<4, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Color4(const Vector<4, T>& other) noexcept: Vector4<T>(other) {}

        /**
         * @brief Convert to HSV representation
         *
         * The alpha channel is not subject to any conversion, so it is
         * ignored.
         * @see @ref hue(), @ref saturation(), @ref value(), @ref a(),
         *      @ref fromHsv()
         */
        ColorHsv<FloatingPointType> toHsv() const {
            return Implementation::toHsv<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::hue() */
        Deg<FloatingPointType> hue() const {
            return Implementation::hue<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::saturation() */
        FloatingPointType saturation() const {
            return Implementation::saturation<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::value() */
        FloatingPointType value() const {
            return Implementation::value<T>(Vector4<T>::rgb());
        }

        /**
         * @brief Convert to sRGB + alpha representation
         *
         * Assuming the color is in linear RGB, applies sRGB curve onto the RGB
         * channels, returning the color represented in sRGB color space. Alpha
         * channel is kept linear. See @ref Color3::toSrgb() for more
         * information.
         *
         * @see @ref fromSrgbAlpha(), @ref toSrgbAlphaInt()
         */
        Vector4<FloatingPointType> toSrgbAlpha() const {
            return Implementation::toSrgbAlpha<T>(*this);
        }

        /** @overload
         * @brief Convert to integral sRGB + alpha representation
         *
         * Useful in cases where you have a floating-point linear RGBA color
         * and want to create for example an 8-bit sRGB + alpha representation
         * out of it:
         *
         * @snippet Math.cpp Color4-toSrgbAlpha
         *
         * For conversion to a *linear* 32-bit representation (i.e, without
         * applying the sRGB curve), use @ref pack():
         *
         * @snippet Math.cpp Color4-pack
         *
         * @see @ref toSrgbAlphaInt(), @ref toLinearRgbaInt()
         */
        template<class Integral> Vector4<Integral> toSrgbAlpha() const {
            return Implementation::toSrgbAlphaIntegral<T, Integral>(*this);
        }

        /**
         * @brief Convert to 32-bit integral sRGB + linear alpha representation
         *
         * See @ref Color3::toSrgb() const for more information and
         * @ref fromSrgbAlphaInt() for an inverse operation. Use @ref rgb()
         * together with @ref Color3::toSrgbInt() to output a 24-bit sRGB
         * color. Note that the integral value is endian-dependent (the red
         * channel being in the *last* byte on little-endian platforms), for
         * conversion to an endian-independent sRGB representation use
         * @ref toSrgbAlpha() const "toSrgbAlpha<UnsignedByte>() const".
         * @see @ref toLinearRgbaInt()
         */
        UnsignedInt toSrgbAlphaInt() const {
            const auto srgbAlpha = toSrgbAlpha<UnsignedByte>();
            return (srgbAlpha[0] << 24) | (srgbAlpha[1] << 16) | (srgbAlpha[2] << 8) | srgbAlpha[3];
        }

        /**
         * @brief Convert to 32-bit integral linear RGBA representation
         *
         * Compared to @ref toSrgbAlphaInt() *does not* perform a sRGB
         * conversion on the output. See @ref fromLinearRgbaInt() for an
         * inverse operation. Use @ref rgb() together with
         * @ref Color3::toLinearRgbInt() to output a 24-bit linear RGBA color.
         * Note that the integral value is endian-dependent (the red channel
         * being in the *last* byte on little-endian platforms), for conversion
         * to an endian-independent linear representation use
         * @ref pack() "pack<Color4ub>()".
         */
        UnsignedInt toLinearRgbaInt() const {
            return Implementation::toLinearRgbaInt(*this);
        }

        /**
         * @brief Convert to [CIE XYZ representation](https://en.wikipedia.org/wiki/CIE_1931_color_space)
         *
         * Assuming the color is in linear RGB, applies transformation matrix,
         * returning the color in CIE XYZ color space. The alpha channel is not
         * subject to any conversion, so it is ignored. See @ref Color3::toXyz()
         * for more information.
         *
         * Please note that @ref xyz(), @ref x(), @ref y() and @ref z() *do not*
         * correspond to primaries in CIE XYZ color space, but are rather
         * aliases to @ref rgb(), @ref r(), @ref g() and @ref b().
         * @see @ref fromXyz(), @ref xyzToXyY()
         */
        Vector3<FloatingPointType> toXyz() const {
            return Implementation::toXyz<T>(rgb());
        }

        /**
         * @brief Color with premultiplied alpha
         * @m_since_latest
         *
         * The resulting color has RGB channels always less than or equal to
         * alpha. Note that premultiplication isn't a reversible operation ---
         * if alpha is zero, RGB channels become zero as well and
         * @ref unpremultiplied() won't be able to recover the original values
         * back. @f[
         *  \boldsymbol{c_\mathrm{premult}} = (\boldsymbol{c_{rgb}} c_a, c_a)
         * @f]
         */
        constexpr Color4<T> premultiplied() const {
            return Implementation::premultiplied(*this);
        }

        /**
         * @brief Color with unpremultiplied alpha
         * @m_since_latest
         *
         * Assuming the input has premultiplied alpha, such as coming from
         * @ref premultiplied(), returns an unpremultiplied color. Note that
         * premultiplication isn't a reversible operation --- if alpha is zero,
         * the RGB channels will be set to zero as well. @f[
         *      \boldsymbol{c} = \begin{cases}
         *          \boldsymbol{0}, & {c_\mathrm{premult}}_a = 0 \\
         *          (\cfrac{\boldsymbol{{c_\mathrm{premult}}_{rgb}}}{c_a}, {c_\mathrm{premult}}_a) & {c_\mathrm{premult}}_a > 0
         *      \end{cases}
         * @f]
         *
         * Additionally, with packed types such as @ref Color4ub, RGB channels
         * are clamped to avoid overflow: @f[
         *      \boldsymbol{c} = \begin{cases}
         *          \boldsymbol{0}, & {c_\mathrm{premult}}_a = 0 \\
         *          (\cfrac{\min(\boldsymbol{{c_\mathrm{premult}}_{rgb}}, {c_\mathrm{premult}}_a)}{c_a}, {c_\mathrm{premult}}_a) & {c_\mathrm{premult}}_a > 0
         *      \end{cases}
         * @f]
         */
        constexpr Color4<T> unpremultiplied() const {
            return Implementation::unpremultiplied(*this);
        }

        /* Overloads to remove WTF-factor from return types */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Color3<T>& xyz() { return Color3<T>::from(Vector4<T>::data()); }
        constexpr const Color3<T> xyz() const { return Vector4<T>::xyz(); }

        Color3<T>& rgb() { return xyz(); }
        constexpr const Color3<T> rgb() const { return xyz(); }
        #endif

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(4, Color4)
};

#ifdef CORRADE_MSVC2015_COMPATIBILITY
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(4, Color4)
#endif

/** @relatesalso Color3
@brief Convert color from [CIE xyY representation](https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space) to CIE XYZ

@f[
    \begin{array}{rcl}
        X & = & \dfrac{Y}{y}x \\
        Z & = & \dfrac{Y}{y}(1 - x - y)
    \end{array}
@f]
@see @ref xyzToXyY(), @ref Color3::fromXyz(), @ref Color3::toXyz()
*/
template<class T> inline Vector3<T> xyYToXyz(const Vector3<T>& xyY) {
    return {xyY[0]*xyY[2]/xyY[1], xyY[2], (T(1) - xyY[0] - xyY[1])*xyY[2]/xyY[1]};
}

/** @relatesalso Color3
@brief Convert color from CIE XYZ representation to [CIE xyY](https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space)

@f[
    \begin{array}{rcl}
        x & = & \dfrac{X}{X + Y + Z} \\
        y & = & \dfrac{Y}{X + Y + Z}
    \end{array}
@f]
@see @ref xyYToXyz(), @ref Color3::fromXyz(), @ref Color3::toXyz()
*/
template<class T> inline Vector3<T> xyzToXyY(const Vector3<T>& xyz) {
    return {xyz.xy()/xyz.sum(), xyz.y()};
}

/**
@brief HSV color
@m_since{2019,10}

Storage-only type with just the usual constructors and (non-)equality
comparison.
@see @ref Color3::fromHsv(), @ref Color3::toHsv(), @ref Color4::fromHsv(),
    @ref Color4::toHsv()
*/
template<class T> struct ColorHsv {
    /**
     * @brief Default constructor
     *
     * Equivalent to @ref ColorHsv(ZeroInitT).
     */
    constexpr /*implicit*/ ColorHsv() noexcept: hue{}, saturation{}, value{} {}

    /**
     * @brief Construct a zero color
     *
     * All members are set to zero.
     */
    constexpr explicit ColorHsv(ZeroInitT) noexcept: hue{}, saturation{}, value{} {}

    /** @brief Construct without initializing the contents */
    explicit ColorHsv(Magnum::NoInitT) noexcept: hue{Magnum::NoInit} /* and the others not */ {}

    /** @brief Constructor */
    constexpr /*implicit*/ ColorHsv(Deg<T> hue, T saturation, T value) noexcept: hue{hue}, saturation{saturation}, value{value} {}

    /**
     * @brief Construct from different type
     *
     * Performs only default casting on the values, no rounding or
     * anything else.
     */
    template<class U> constexpr explicit ColorHsv(const ColorHsv<U>& other) noexcept: hue{other.hue}, saturation{T(other.saturation)}, value{T(other.value)} {}

    /** @brief Equality comparison */
    bool operator==(const ColorHsv<T>& other) const {
        return hue == other.hue &&
            TypeTraits<T>::equals(saturation, other.saturation) &&
            TypeTraits<T>::equals(value, other.value);
    }

    /** @brief Non-equality comparison */
    bool operator!=(const ColorHsv<T>& other) const {
        return !operator==(other);
    }

    /** @brief Hue, in range @f$ [0.0, 360.0] @f$ */
    Deg<T> hue;

    /** @brief Saturation, in range @f$ [0.0, 1.0] @f$ */
    T saturation;

    /** @brief Value, in range @f$ [0.0, 1.0] @f$ */
    T value;
};

#ifndef CORRADE_SINGLES_NO_DEBUG
/** @debugoperator{ColorHsv} */
template<class T> Debug& operator<<(Debug& debug, const ColorHsv<T>& value) {
    return debug << "ColorHsv(" << Debug::nospace << value.hue
        << Debug::nospace << "," << value.saturation
        << Debug::nospace << "," << value.value
        << Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const ColorHsv<Float>&);
#endif
#endif

/* Unlike STL, where there's e.g. std::literals::string_literals with both
   being inline, here's just the second inline because making both would cause
   the literals to be implicitly available to all code in Math. Which isn't
   great if there are eventually going to be conflicts. In case of STL the
   expected use case was that literals are available to anybody who does
   `using namespace std;`, that doesn't apply here as most APIs are in
   subnamespaces that *should not* be pulled in via `using` as a whole. */
namespace Literals {
    /** @todoc The inline causes "error: non-const getClassDef() called on
        aliased member. Please report as a bug." on Doxygen 1.8.18, plus the
        fork I have doesn't even mark them as inline in the XML output yet. And
        it also duplicates the literal reference to parent namespace, adding
        extra noise. Revisit once upgrading to a newer version. */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    inline
    #endif
    namespace ColorLiterals {

/* According to https://wg21.link/CWG2521, space between "" and literal name is
   deprecated because _Uppercase or __double names could be treated as reserved
   depending on whether the space was present or not, and whitespace is not
   load-bearing in any other contexts. Clang 17+ adds an off-by-default warning
   for this; GCC 4.8 however *requires* the space there, so until GCC 4.8
   support is dropped, we suppress this warning instead of removing the
   space. */
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
#endif
/** @relatesalso Magnum::Math::Color3
@brief 8bit-per-channel linear RGB literal

Unpacks the literal into three 8-bit values. Example usage:

@snippet Math.cpp _rgb

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. To convey such meaning, use the @link operator""_srgb() @endlink
    literal instead.

@see @link operator""_rgba() @endlink, @link operator""_rgbf() @endlink
@m_keywords{_rgb rgb}
*/
constexpr Color3<UnsignedByte> operator"" _rgb(unsigned long long value) {
    return {UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color3
@brief 8bit-per-channel sRGB literal

Unpacks the literal into three 8-bit values without any colorspace conversion.
Behaves identically to @link operator""_rgb() @endlink though it doesn't
return a @ref Color3 type to indicate that the resulting value is not linear
RGB. Use this literal to document that given value is in sRGB. Example usage:

@snippet Math.cpp _srgb

@attention Note that colors in sRGB representation should not be used directly
    in calculations --- they should be converted to linear RGB, calculation
    done on the linear representation and then converted back to sRGB. Use the
    @link operator""_srgbf() @endlink literal if you want to get a linear RGB
    representation directly or convert the value using @ref Color3::fromSrgb()
    / @ref Color3::fromSrgbInt().

@see @link operator""_srgba() @endlink, @link operator""_rgb() @endlink
@m_keywords{_srgb srgb}
*/
/* Output is a Vector3 to hint that it doesn't have any (additive,
   multiplicative) semantics of a linear RGB color */
constexpr Vector3<UnsignedByte> operator"" _srgb(unsigned long long value) {
    return {UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color4
@brief 8bit-per-channel linear RGBA literal

Unpacks the literal into four 8-bit values. Example usage:

@snippet Math.cpp _rgba

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. To convey such meaning, use the @link operator""_srgba() @endlink
    literal instead.

@see @link operator""_rgb() @endlink, @link operator""_rgbaf() @endlink
@m_keywords{_rgba rgba}
*/
constexpr Color4<UnsignedByte> operator"" _rgba(unsigned long long value) {
    return {UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color4
@brief 8bit-per-channel sRGB + alpha literal

Unpacks the literal into four 8-bit values without any colorspace conversion.
Behaves identically to @link operator""_rgba() @endlink though it doesn't
return a @ref Color4 type to indicate that the resulting value is not linear
RGBA. Use this literal to document that given value is in sRGB + alpha. Example
usage:

@snippet Math.cpp _srgba

@attention Note that colors in sRGB representation should not be used directly
    in calculations --- they should be converted to linear RGB, calculation
    done on the linear representation and then converted back to sRGB. Use the
    @link operator""_srgbaf() @endlink literal if you want to get a linear RGBA
    representation directly or convert the value using
    @ref Color4::fromSrgbAlpha() / @ref Color4::fromSrgbAlphaInt().

@see @link operator""_srgb() @endlink, @link operator""_rgba() @endlink
@m_keywords{_srgba srgba}
*/
/* Output is a Vector3 to hint that it doesn't have any (additive,
   multiplicative) semantics of a linear RGB color */
constexpr Vector4<UnsignedByte> operator"" _srgba(unsigned long long value) {
    return {UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color3
@brief Float linear RGB literal

Equivalent to calling @ref Color3::fromLinearRgbInt() on the literal value.
Example usage:

@snippet Math.cpp _rgbf

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. In that case use the @link operator""_srgbf() @endlink
    literal instead.

@see @link operator""_rgbaf() @endlink, @link operator""_rgb() @endlink
@m_keywords{_rgbf rgbf}
*/
constexpr Color3<Float> operator"" _rgbf(unsigned long long value) {
    return {((value >> 16) & 0xff)/255.0f,
            ((value >>  8) & 0xff)/255.0f,
            ((value >>  0) & 0xff)/255.0f};
}

/** @relatesalso Magnum::Math::Color3
@brief Float sRGB literal

Calls @ref Color3::fromSrgbInt() on the literal value. Example usage:

@snippet Math.cpp _srgbf

@see @link operator""_srgbaf() @endlink, @link operator""_srgb() @endlink,
    @link operator""_rgbf() @endlink
@m_keywords{_srgbf srgbf}
*/
inline Color3<Float> operator"" _srgbf(unsigned long long value) {
    return Color3<Float>::fromSrgbInt(UnsignedInt(value));
}

/** @relatesalso Magnum::Math::Color4
@brief Float linear RGBA literal

Equivalent to calling @ref Color4::fromLinearRgbaInt() on the literal value.
Example usage:

@snippet Math.cpp _rgbaf

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. In that case use the @link operator""_srgbaf() @endlink
    literal instead.

@see @link operator""_rgbf() @endlink, @link operator""_rgba() @endlink
@m_keywords{_rgbaf rgbaf}
*/
constexpr Color4<Float> operator"" _rgbaf(unsigned long long value) {
    return {((value >> 24) & 0xff)/255.0f,
            ((value >> 16) & 0xff)/255.0f,
            ((value >>  8) & 0xff)/255.0f,
            ((value >>  0) & 0xff)/255.0f};
}

/** @relatesalso Magnum::Math::Color4
@brief Float sRGB + alpha literal

Calls @ref Color4::fromSrgbAlphaInt() on the literal value. Example usage:

@snippet Math.cpp _srgbaf

@see @link operator""_srgbf() @endlink, @link operator""_srgba() @endlink,
    @link operator""_rgbaf() @endlink
@m_keywords{_srgbaf srgbaf}
*/
inline Color4<Float> operator"" _srgbaf(unsigned long long value) {
    return Color4<Float>::fromSrgbAlphaInt(UnsignedInt(value));
}
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic pop
#endif

}}

#ifndef CORRADE_SINGLES_NO_DEBUG
/**
@debugoperator{Color3}

If @ref Debug::Flag::Color is enabled or @ref Debug::color was set immediately
before, prints the value as an ANSI 24bit color escape sequence using two
successive Unicode block characters (to have it roughly square). To preserve at
least some information when text is copied, the square consists of one of the
five @cb{.shell-session} ░▒▓█ @ce shades, however the color is set for both
foreground and background so the actual block character is indistinguishable
when seen on a terminal.

If @ref Debug::Flag::Color is enabled and @ref Debug::Flag::DisableColors is
set, only the shaded character is used, without any ANSI color escape sequence.

If @ref Debug::Flag::Color is not enabled, the value is printed as a hex color
(e.g. @cb{.shell-session} #ff33aa @ce). Other underlying types are handled by
@ref operator<<(Debug&, const Vector<size, T>&).

For example, the following snippet:

@snippet Math.cpp Color3-debug

<b></b>

@m_class{m-noindent}

prints the following on terminals that support it:

@include MathColor3-debug.ansi
*/
MAGNUM_EXPORT Debug& operator<<(Debug& debug, const Color3<UnsignedByte>& value);

/**
@debugoperator{Color4}

If @ref Debug::Flag::Color is enabled or @ref Debug::color was set immediately
before, prints the value as an ANSI 24bit color escape sequence using two
successive Unicode block characters (to have it roughly square). To preserve at
least some information when text is copied, the square consists of one of the
five @cb{.shell-session} ░▒▓█ @ce shades. The square shade is calculated as a
product of @ref Color4::value() and @ref Color4::a(). If calculated color value
is less than alpha, the colored square has the color set for both background
and foreground, otherwise the background is left at the default.

If @ref Debug::Flag::Color is enabled and @ref Debug::Flag::DisableColors is
set, only the shaded character is used, without any ANSI color escape sequence.

If @ref Debug::Flag::Color is not enabled, the value is printed as a hex color
(e.g. @cb{.shell-session} #ff33aaff @ce). Other underlying types are handled by
@ref operator<<(Debug&, const Vector<size, T>&).

For example, the following snippet:

@snippet Math.cpp Color4-debug

<b></b>

@m_class{m-noindent}

prints the following on terminals that support it:

@include MathColor4-debug.ansi
*/
MAGNUM_EXPORT Debug& operator<<(Debug& debug, const Color4<UnsignedByte>& value);
#endif

namespace Implementation {
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };
    #ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
    template<class T> struct StrictWeakOrdering<Color3<T>>: StrictWeakOrdering<Vector<3, T>> {};
    template<class T> struct StrictWeakOrdering<Color4<T>>: StrictWeakOrdering<Vector<4, T>> {};
    #endif
}

}}

#if !defined(CORRADE_NO_TWEAKABLE) && (defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN))
namespace Corrade { namespace Utility {

/**
@tweakableliteral{Magnum::Math::Color3}

Parses the @link Magnum::Math::Literals::ColorLiterals::operator""_rgb @endlink
and @link Magnum::Math::Literals::ColorLiterals::operator""_srgb @endlink
literals.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Color3<Magnum::UnsignedByte>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Color3<Magnum::UnsignedByte>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Vector3<Magnum::UnsignedByte>>: TweakableParser<Magnum::Math::Color3<Magnum::UnsignedByte>> {};
#endif

/**
@tweakableliteral{Magnum::Math::Color4}

Parses the @link Magnum::Math::Literals::ColorLiterals::operator""_rgba @endlink
and @link Magnum::Math::Literals::ColorLiterals::operator""_srgba @endlink literals.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Color4<Magnum::UnsignedByte>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Color4<Magnum::UnsignedByte>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Vector4<Magnum::UnsignedByte>>: TweakableParser<Magnum::Math::Color4<Magnum::UnsignedByte>> {};
#endif

/**
@tweakableliteral{Magnum::Math::Color3}

Parses the @link Magnum::Math::Literals::ColorLiterals::operator""_rgbf @endlink
and @link Magnum::Math::Literals::ColorLiterals::operator""_srgbf @endlink
literals.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Color3<Magnum::Float>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Color3<Magnum::Float>> parse(Containers::StringView value);
};

/**
@tweakableliteral{Magnum::Math::Color4}

Parses the @link Magnum::Math::Literals::ColorLiterals::operator""_rgbaf @endlink
and @link Magnum::Math::Literals::ColorLiterals::operator""_srgbaf @endlink
literals.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Color4<Magnum::Float>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Color4<Magnum::Float>> parse(Containers::StringView value);
};

}}
#endif

#endif
