#ifndef Magnum_Math_Color_h
#define Magnum_Math_Color_h
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
 * @brief Class @ref Magnum::Math::Color3, @ref Magnum::Math::Color4, literal @link Magnum::Math::Literals::operator""_rgb() @endlink, @link Magnum::Math::Literals::operator""_rgba() @endlink, @link Magnum::Math::Literals::operator""_rgbf() @endlink, @link Magnum::Math::Literals::operator""_rgbaf() @endlink, @link Magnum::Math::Literals::operator""_srgb() @endlink, @link Magnum::Math::Literals::operator""_srgba() @endlink, @link Magnum::Math::Literals::operator""_srgbf() @endlink, @link Magnum::Math::Literals::operator""_srgbaf() @endlink
 */

#include <tuple>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math {

namespace Implementation {

/* Convert color from HSV */
template<class T> typename std::enable_if<std::is_floating_point<T>::value, Color3<T>>::type fromHsv(const typename Color3<T>::Hsv& hsv) {
    Deg<T> hue;
    T saturation, value;
    std::tie(hue, saturation, value) = hsv;

    /* Remove repeats */
    hue -= floor(T(hue)/T(360))*Deg<T>(360);
    if(hue < Deg<T>(0)) hue += Deg<T>(360);

    int h = int(T(hue)/T(60)) % 6;
    T f = T(hue)/T(60) - h;

    T p = value * (T(1) - saturation);
    T q = value * (T(1) - f*saturation);
    T t = value * (T(1) - (T(1) - f)*saturation);

    switch(h) {
        case 0: return {value, t, p};
        case 1: return {q, value, p};
        case 2: return {p, value, t};
        case 3: return {p, q, value};
        case 4: return {t, p, value};
        case 5: return {value, p, q};
        default: CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color3<T>>::type fromHsv(const typename Color3<T>::Hsv& hsv) {
    return pack<Color3<T>>(fromHsv<typename Color3<T>::FloatingPointType>(hsv));
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
template<class T> inline Deg<T> hue(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    T max = color.max();
    T delta = max - color.min();
    return hue(color, max, delta);
}
template<class T> inline T saturation(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    T max = color.max();
    T delta = max - color.min();
    return max != T(0) ? delta/max : T(0);
}
template<class T> inline T value(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    return color.max();
}

/* Hue, saturation, value for integral types */
template<class T> inline Deg<typename Color3<T>::FloatingPointType> hue(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return hue<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T> inline typename Color3<T>::FloatingPointType saturation(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type& color) {
    return saturation<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T> inline typename Color3<T>::FloatingPointType value(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return unpack<typename Color3<T>::FloatingPointType>(color.max());
}

/* Convert color to HSV */
template<class T> inline typename Color3<T>::Hsv toHsv(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    T max = color.max();
    T delta = max - color.min();

    return typename Color3<T>::Hsv(hue<typename Color3<T>::FloatingPointType>(color, max, delta), max != T(0) ? delta/max : T(0), max);
}
template<class T> inline typename Color3<T>::Hsv toHsv(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return toHsv<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(color));
}

/* sRGB -> RGB conversion */
template<class T> typename std::enable_if<std::is_floating_point<T>::value, Color3<T>>::type fromSrgb(const Vector3<T>& srgb) {
    constexpr const T a(T(0.055));
    return lerp(srgb/T(12.92), pow((srgb + Vector3<T>{a})/(T(1.0) + a), T(2.4)), srgb > Vector3<T>(T(0.04045)));
}
template<class T> typename std::enable_if<std::is_floating_point<T>::value, Color4<T>>::type fromSrgbAlpha(const Vector4<T>& srgbAlpha) {
    return {fromSrgb<T>(srgbAlpha.rgb()), srgbAlpha.a()};
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color3<T>>::type fromSrgb(const Vector3<typename Color3<T>::FloatingPointType>& srgb) {
    return pack<Color3<T>>(fromSrgb<typename Color3<T>::FloatingPointType>(srgb));
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color4<T>>::type fromSrgbAlpha(const Vector4<typename Color4<T>::FloatingPointType>& srgbAlpha) {
    return {fromSrgb<T>(srgbAlpha.rgb()), pack<T>(srgbAlpha.a())};
}
template<class T, class Integral> inline Color3<T> fromSrgbIntegral(const Vector3<Integral>& srgb) {
    static_assert(std::is_integral<Integral>::value, "only conversion from different integral type is supported");
    return fromSrgb<T>(unpack<Vector3<typename Color3<T>::FloatingPointType>>(srgb));
}
template<class T, class Integral> inline Color4<T> fromSrgbAlphaIntegral(const Vector4<Integral>& srgbAlpha) {
    static_assert(std::is_integral<Integral>::value, "only conversion from different integral type is supported");
    return fromSrgbAlpha<T>(unpack<Vector4<typename Color4<T>::FloatingPointType>>(srgbAlpha));
}

/* RGB -> sRGB conversion */
template<class T> Vector3<typename Color3<T>::FloatingPointType> toSrgb(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type rgb) {
    constexpr const T a(0.055);
    return lerp(rgb*T(12.92), (T(1.0) + a)*pow(rgb, T(1.0)/T(2.4)) - Vector3<T>{a}, rgb > Vector3<T>(0.0031308));
}
template<class T> Vector4<typename Color4<T>::FloatingPointType> toSrgbAlpha(typename std::enable_if<std::is_floating_point<T>::value, const Color4<T>&>::type rgba) {
    return {toSrgb<T>(rgba.rgb()), rgba.a()};
}
template<class T> inline Vector3<typename Color3<T>::FloatingPointType> toSrgb(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type rgb) {
    return toSrgb<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(rgb));
}
template<class T> inline Vector4<typename Color4<T>::FloatingPointType> toSrgbAlpha(typename std::enable_if<std::is_integral<T>::value, const Color4<T>&>::type rgba) {
    return {toSrgb<T>(rgba.rgb()), unpack<typename Color3<T>::FloatingPointType>(rgba.a())};
}
template<class T, class Integral> inline Vector3<Integral> toSrgbIntegral(const Color3<T>& rgb) {
    static_assert(std::is_integral<Integral>::value, "only conversion from different integral type is supported");
    return pack<Vector3<Integral>>(toSrgb<T>(rgb));
}
template<class T, class Integral> inline Vector4<Integral> toSrgbAlphaIntegral(const Color4<T>& rgba) {
    static_assert(std::is_integral<Integral>::value, "only conversion from different integral type is supported");
    return pack<Vector4<Integral>>(toSrgbAlpha<T>(rgba));
}

/* CIE XYZ -> RGB conversion */
template<class T> typename std::enable_if<std::is_floating_point<T>::value, Color3<T>>::type fromXyz(const Vector3<T>& xyz) {
    /* Taken from https://en.wikipedia.org/wiki/Talk:SRGB#Rounded_vs._Exact,
       the rounded matrices from the main article don't round-trip perfectly */
    return Matrix3x3<T>{
        Vector3<T>{T(12831)/T(3959), T(-851781)/T(878810), T(705)/T(12673)},
        Vector3<T>{T(-329)/T(214), T(1648619)/T(878810), T(-2585)/T(12673)},
        Vector3<T>{T(-1974)/T(3959), T(36519)/T(878810), T(705)/T(667)}}*xyz;
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color3<T>>::type fromXyz(const Vector3<typename Color3<T>::FloatingPointType>& xyz) {
    return pack<Color3<T>>(fromXyz<typename Color3<T>::FloatingPointType>(xyz));
}

/* RGB -> CIE XYZ conversion */
template<class T> Vector3<typename Color3<T>::FloatingPointType> toXyz(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type rgb) {
    /* Taken from https://en.wikipedia.org/wiki/Talk:SRGB#Rounded_vs._Exact,
       the rounded matrices from the main article don't round-trip perfectly */
    return (Matrix3x3<T>{
        Vector3<T>{T(506752)/T(1228815), T(87098)/T(409605), T(7918)/T(409605)},
        Vector3<T>{T(87881)/T(245763), T(175762)/T(245763), T(87881)/T(737289)},
        Vector3<T>{T(12673)/T(70218), T(12673)/T(175545), T(1001167)/T(1053270)}})*rgb;
}
template<class T> inline Vector3<typename Color3<T>::FloatingPointType> toXyz(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type rgb) {
    return toXyz<typename Color3<T>::FloatingPointType>(unpack<Color3<typename Color3<T>::FloatingPointType>>(rgb));
}

/* Value for full channel (1.0f for floats, 255 for unsigned byte) */
template<class T> constexpr typename std::enable_if<std::is_floating_point<T>::value, T>::type fullChannel() {
    return T(1);
}
template<class T> constexpr typename std::enable_if<std::is_integral<T>::value, T>::type fullChannel() {
    return Implementation::bitMax<T>();
}

}

/**
@brief Color in linear RGB color space

The class can store either floating-point (normalized) or integral
(denormalized) representation of linear RGB color. Colors in sRGB color space
should not be used directly in calculations -- they should be converted to
linear RGB using @ref fromSrgb(), calculation done on the linear representation
and then converted back to sRGB using @ref toSrgb().

Note that constructor conversion between different types (like in @ref Vector
classes) doesn't do any (de)normalization, you should use @ref pack) and
@ref unpack() instead, for example:
@code
Color3 a(1.0f, 0.5f, 0.75f);
auto b = pack<Color3ub>(a); // b == {255, 127, 191}
@endcode

Conversion from and to HSV is done always using floating-point types, so hue
is always in range in range @f$ [0.0, 360.0] @f$, saturation and value in
range @f$ [0.0, 1.0] @f$.

@see @link operator""_rgb() @endlink, @link operator""_rgbf() @endlink,
    @link operator""_srgb() @endlink, @link operator""_srgbf() @endlink,
    @ref Color4, @ref Magnum::Color3, @ref Magnum::Color3ub
*/
/* Not using template specialization because some internal functions are
   impossible to explicitly instantiate */
template<class T> class Color3: public Vector3<T> {
    public:
        /**
         * @brief Red color
         *
         * Convenience alternative to e.g. `Color3(red, 0.0f, 0.0f)`. With
         * floating-point underlying type equivalent to @ref Vector3::xAxis().
         * @see @ref green(), @ref blue(), @ref cyan()
         */
        constexpr static Color3<T> red(T red = Implementation::fullChannel<T>()) {
            return Vector3<T>::xAxis(red);
        }

        /**
         * @brief Green color
         *
         * Convenience alternative to e.g. `Color3(0.0f, green, 0.0f)`. With
         * floating-point underlying type equivalent to @ref Vector3::yAxis().
         * @see @ref red(), @ref blue(), @ref magenta()
         */
        constexpr static Color3<T> green(T green = Implementation::fullChannel<T>()) {
            return Vector3<T>::yAxis(green);
        }

        /**
         * @brief Blue color
         *
         * Convenience alternative to e.g. `Color3(0.0f, 0.0f, blue)`. With
         * floating-point underlying type equivalent to @ref Vector3::zAxis().
         * @see @ref red(), @ref green(), @ref yellow()
         */
        constexpr static Color3<T> blue(T blue = Implementation::fullChannel<T>()) {
            return Vector3<T>::zAxis(blue);
        }

        /**
         * @brief Cyan color
         *
         * Convenience alternative to e.g. `Color3(red, 1.0f, 1.0f)`. With
         * floating-point underlying type equivalent to @ref Vector3::xScale().
         * @see @ref magenta(), @ref yellow(), @ref red()
         */
        constexpr static Color3<T> cyan(T red = T(0)) {
            return {red, Implementation::fullChannel<T>(), Implementation::fullChannel<T>()};
        }

        /**
         * @brief Magenta color
         *
         * Convenience alternative to e.g. `Color3(0.0f, green, 0.0f)`. With
         * floating-point underlying type equivalent to @ref Vector3::yScale().
         * @see @ref cyan(), @ref yellow(), @ref green()
         */
        constexpr static Color3<T> magenta(T green = T(0)) {
            return {Implementation::fullChannel<T>(), green, Implementation::fullChannel<T>()};
        }

        /**
         * @brief Yellow color
         *
         * Convenience alternative to `Color3(0.0f, 0.0f, yellow)`. With
         * floating-point underlying type equivalent to @ref Vector3::zScale().
         * @see @ref cyan(), @ref magenta(), @ref red()
         */
        constexpr static Color3<T> yellow(T blue = T(0)) {
            return {Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), blue};
        }

        /**
         * @brief Corresponding floating-point type
         *
         * For HSV and other color spaces.
         */
        typedef typename TypeTraits<T>::FloatingPointType FloatingPointType;

        /**
         * @brief Type for storing HSV color space values
         *
         * Hue in range @f$ [0.0, 360.0] @f$, saturation and value in
         * range @f$ [0.0, 1.0] @f$.
         */
        typedef std::tuple<Deg<FloatingPointType>, FloatingPointType, FloatingPointType> Hsv;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief Hsv
         * @deprecated Use @ref Hsv instead.
         */
        typedef CORRADE_DEPRECATED("use Hsv instead") Hsv HSV;
        #endif

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv   Color in HSV color space
         *
         * Hue can overflow the range @f$ [0.0, 360.0] @f$.
         * @see @ref toHsv()
         */
        static Color3<T> fromHsv(const Hsv& hsv) {
            return Implementation::fromHsv<T>(hsv);
        }
        /** @overload */
        static Color3<T> fromHsv(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value) {
            return fromHsv(std::make_tuple(hue, saturation, value));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief fromHsv(const Hsv&)
         * @deprecated Use @ref fromHsv(const Hsv&) instead.
         */
        CORRADE_DEPRECATED("use fromHsv() instead") static Color3<T> fromHSV(const Hsv& hsv) {
            return fromHsv(hsv);
        }
        /** @copybrief fromHsv(Deg<FloatingPointType>, FloatingPointType, FloatingPointType)
         * @deprecated Use @ref fromHsv(Deg<FloatingPointType>, FloatingPointType, FloatingPointType)
         *      instead.
         */
        CORRADE_DEPRECATED("use fromHsv() instead") static Color3<T> fromHSV(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value) {
            return fromHsv(hue, saturation, value);
        }
        #endif

        /**
         * @brief Create linear RGB color from sRGB representation
         * @param srgb  Color in sRGB color space
         *
         * Applies inverse sRGB curve onto input, returning the input in linear
         * RGB color space with D65 illuminant and 2° standard colorimetric
         * observer. @f[
         *      \boldsymbol{c}_\mathrm{linear} = \begin{cases}
         *          \dfrac{\boldsymbol{c}_\mathrm{sRGB}}{12.92}, & \boldsymbol{c}_\mathrm{sRGB} \le 0.04045 \\
         *          \left( \dfrac{\boldsymbol{c}_\mathrm{sRGB} + a}{1 + a} \right)^{2.4}, & \boldsymbol{c}_\mathrm{sRGB} > 0.04045
         *      \end{cases}
         * @f]
         * @see @link operator""_srgbf() @endlink, @ref toSrgb()
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
         * @code
         * Math::Vector3<UnsignedByte> srgb;
         * auto rgb = Color3::fromSrgb(srgb);
         * @endcode
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color3<T> fromSrgb(const Vector3<Integral>& srgb) {
            return Implementation::fromSrgbIntegral<T, Integral>(srgb);
        }

        /**
         * @brief Create RGB color from CIE XYZ representation
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
         * All components are set to zero.
         */
        constexpr /*implicit*/ Color3(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector3<T>{ZeroInit}
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Color3(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector3<T>{NoInit}
            #endif
            {}

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
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (un)packing, use @ref pack()
         *      and @ref unpack() instead. See class documentation for more
         *      information.
         */
        template<class U> constexpr explicit Color3(const Vector<3, U>& other) noexcept: Vector3<T>(other) {}

        /** @brief Construct color from external representation */
        template<class U, class V =
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
         * Example usage:
         * @code
         * Deg hue;
         * Float saturation, value;
         * std::tie(hue, saturation, value) = color.toHsv();
         * @endcode
         *
         * @see @ref hue(), @ref saturation(), @ref value(), @ref fromHsv()
         */
        Hsv toHsv() const {
            return Implementation::toHsv<T>(*this);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief toHsv()
         * @deprecated Use @ref toHsv() instead.
         */
        CORRADE_DEPRECATED("use toHsv() instead") Hsv toHSV() const { return toHsv(); }
        #endif

        /**
         * @brief Hue
         * @return Hue in range @f$ [0.0, 360.0] @f$.
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
         * standard colorimetric observer, applies sRGB curve onto it,
         * returning the color represented in sRGB color space: @f[
         *      \boldsymbol{c}_\mathrm{sRGB} = \begin{cases}
         *          12.92C_\mathrm{linear}, & \boldsymbol{c}_\mathrm{linear} \le 0.0031308 \\
         *          (1 + a) \boldsymbol{c}_\mathrm{linear}^{1/2.4}-a, & \boldsymbol{c}_\mathrm{linear} > 0.0031308
         *      \end{cases}
         * @f]
         * @see @ref fromSrgb()
         */
        Vector3<FloatingPointType> toSrgb() const {
            return Implementation::toSrgb<T>(*this);
        }

        /** @overload
         * @brief Convert to integral sRGB representation
         *
         * Useful in cases where you have a floating-point linear RGB color and
         * want to create for example an 8-bit sRGB representation out of it:
         * @code
         * Color3 color;
         * Math::Vector3<UnsignedByte> srgb = color.toSrgb<UnsignedByte>();
         * @endcode
         */
        template<class Integral> Vector3<Integral> toSrgb() const {
            return Implementation::toSrgbIntegral<T, Integral>(*this);
        }

        /**
         * @brief Convert to CIE XYZ representation
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

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(3, Color3)
#endif

/**
@brief Color in linear RGBA color space

See @ref Color3 for more information.
@see @link operator""_rgba() @endlink, @link operator""_rgbaf() @endlink,
    @link operator""_srgba() @endlink, @link operator""_srgbaf() @endlink,
    @ref Magnum::Color4, @ref Magnum::Color4ub
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

        /** @copydoc Color3::Hsv */
        typedef typename Color3<T>::Hsv Hsv;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief Hsv
         * @deprecated Use @ref Hsv instead.
         */
        typedef CORRADE_DEPRECATED("use Hsv instead") Hsv HSV;
        #endif

        /**
         * @brief Red color
         *
         * Convenience alternative to e.g. `Color4(red, 0.0f, 0.0f, alpha)`.
         * @see @ref green(), @ref blue(), @ref cyan()
         */
        constexpr static Color4<T> red(T red = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {red, T(0), T(0), alpha};
        }

        /**
         * @brief Green color
         *
         * Convenience alternative to e.g. `Color4(0.0f, green, 0.0f, alpha)`.
         * @see @ref red(), @ref blue(), @ref magenta()
         */
        constexpr static Color4<T> green(T green = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {T(0), green, T(0), alpha};
        }

        /**
         * @brief Blue color
         *
         * Convenience alternative to e.g. `Color4(0.0f, 0.0f, blue, alpha)`.
         * @see @ref red(), @ref green(), @ref yellow()
         */
        constexpr static Color4<T> blue(T blue = Implementation::fullChannel<T>(), T alpha = Implementation::fullChannel<T>()) {
            return {T(0), T(0), blue, alpha};
        }

        /**
         * @brief Cyan color
         *
         * Convenience alternative to e.g. `Color4(red, 1.0f, 1.0f, alpha)`.
         * @see @ref magenta(), @ref yellow(), @ref red()
         */
        constexpr static Color4<T> cyan(T red = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {red, Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), alpha};
        }

        /**
         * @brief Magenta color
         *
         * Convenience alternative to e.g. `Color4(1.0f, green, 1.0f, alpha)`.
         * @see @ref cyan(), @ref yellow(), @ref green()
         */
        constexpr static Color4<T> magenta(T green = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {Implementation::fullChannel<T>(), green, Implementation::fullChannel<T>(), alpha};
        }

        /**
         * @brief Yellow color
         *
         * Convenience alternative to e.g. `Color4(1.0f, 1.0f, blue, alpha)`.
         * @see @ref cyan(), @ref magenta(), @ref red()
         */
        constexpr static Color4<T> yellow(T blue = T(0), T alpha = Implementation::fullChannel<T>()) {
            return {Implementation::fullChannel<T>(), Implementation::fullChannel<T>(), blue, alpha};
        }

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv   Color in HSV color space
         * @param a     Alpha value, defaults to `1.0` for floating-point types
         *      and maximum positive value for integral types.
         *
         * Hue can overflow the range @f$ [0.0, 360.0] @f$.
         * @see @ref toHsv()
         */
        static Color4<T> fromHsv(const Hsv& hsv, T a = Implementation::fullChannel<T>()) {
            return Color4<T>(Implementation::fromHsv<T>(hsv), a);
        }
        /** @overload */
        static Color4<T> fromHsv(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value, T alpha = Implementation::fullChannel<T>()) {
            return fromHsv(std::make_tuple(hue, saturation, value), alpha);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief fromHsv(const Hsv&, T)
         * @deprecated Use @ref fromHsv(const Hsv&, T) instead.
         */
        CORRADE_DEPRECATED("use fromHsv() instead") static Color4<T> fromHSV(const Hsv& hsv, T a = Implementation::fullChannel<T>()) {
            return fromHsv(hsv, a);
        }
        /** @copybrief fromHsv(Deg<FloatingPointType>, FloatingPointType, FloatingPointType, T)
         * @deprecated Use @ref fromHsv(Deg<FloatingPointType>, FloatingPointType, FloatingPointType, T)
         *      instead.
         */
        CORRADE_DEPRECATED("use fromHsv() instead") static Color4<T> fromHSV(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value, T a = Implementation::fullChannel<T>()) {
            return fromHsv(hue, saturation, value, a);
        }
        #endif

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

        /**
         * @brief Create linear RGBA color from sRGB representation
         * @param srgb      Color in sRGB color space
         * @param a         Alpha value, defaults to `1.0` for floating-point
         *      types and maximum positive value for integral types.
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
         * @brief Create linear RGB color from integral sRGB + alpha representation
         * @param srgbAlpha Color in sRGB color space with linear alpha
         *
         * Useful in cases where you have for example an 8-bit sRGB + alpha
         * representation and want to create a floating-point linear RGBA color
         * out of it:
         * @code
         * Math::Vector4<UnsignedByte> srgbAlpha;
         * auto rgba = Color4::fromSrgbAlpha(srgbAlpha);
         * @endcode
         */
        /* Input is a Vector4 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color4<T> fromSrgbAlpha(const Vector4<Integral>& srgbAlpha) {
            return {Implementation::fromSrgbAlphaIntegral<T, Integral>(srgbAlpha)};
        }

        /** @overload
         * @brief Create linear RGB color from integral sRGB representation
         * @param srgb      Color in sRGB color space
         * @param a         Alpha value, defaults to `1.0` for floating-point
         *      types and maximum positive value for integral types.
         *
         * Useful in cases where you have for example an 8-bit sRGB
         * representation and want to create a floating-point linear RGBA color
         * out of it:
         * @code
         * Math::Vector3<UnsignedByte> srgb;
         * auto rgba = Color4::fromSrgb(srgb);
         * @endcode
         */
        /* Input is a Vector3 to hint that it doesn't have any (additive,
           multiplicative) semantics of a linear RGB color */
        template<class Integral> static Color4<T> fromSrgb(const Vector3<Integral>& srgb, T a = Implementation::fullChannel<T>()) {
            return {Implementation::fromSrgbIntegral<T, Integral>(srgb), a};
        }

        /**
         * @brief Create RGBA color from CIE XYZ representation
         * @param xyz   Color in CIE XYZ color space
         * @param a     Alpha value, defaults to `1.0` for floating-point types
         *      and maximum positive value for integral types.
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
         * All components are set to zero.
         */
        constexpr /*implicit*/ Color4() noexcept: Vector4<T>(T(0), T(0), T(0), T(0)) {}

        /** @copydoc Vector::Vector(ZeroInitT) */
        constexpr explicit Color4(ZeroInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector4<T>{ZeroInit}
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Color4(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Vector4<T>{NoInit}
            #endif
            {}

        /**
         * @copydoc Color3::Color3(T)
         * @param alpha Alpha value, defaults to `1.0` for floating-point types
         *      and maximum positive value for integral types.
         */
        constexpr explicit Color4(T rgb, T alpha = Implementation::fullChannel<T>()) noexcept: Vector4<T>(rgb, rgb, rgb, alpha) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         * @param a     A value, defaults to `1.0` for floating-point types and
         *      maximum positive value for integral types.
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

        /**
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (un)packing, use @ref pack)
         *      and @ref unpack() instead. See @ref Color3 class documentation
         *      for more information.
         */
        template<class U> constexpr explicit Color4(const Vector<4, U>& other) noexcept: Vector4<T>(other) {}

        /** @brief Construct color from external representation */
        template<class U, class V =
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
         * ignored. Example usage:
         * @code
         * Deg hue;
         * Float saturation, value;
         * std::tie(hue, saturation, value) = color.toHsv();
         * @endcode
         *
         * @see @ref hue(), @ref saturation(), @ref value(), @ref fromHsv()
         */
        Hsv toHsv() const {
            return Implementation::toHsv<T>(Vector4<T>::rgb());
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief toHsv()
         * @deprecated Use @ref toHsv() instead.
         */
        CORRADE_DEPRECATED("use toHsv() instead") Hsv toHSV() const { return toHsv(); }
        #endif

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
         * @see @ref fromSrgbAlpha()
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
         * @code
         * Color4 color;
         * Math::Vector4<UnsignedByte> srgbAlpha = color.toSrgbAlpha<UnsignedByte>();
         * @endcode
         */
        template<class Integral> Vector4<Integral> toSrgbAlpha() const {
            return Implementation::toSrgbAlphaIntegral<T, Integral>(*this);
        }

        /**
         * @brief Convert to CIE XYZ representation
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

        /* Overloads to remove WTF-factor from return types */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Color3<T>& xyz() { return Color3<T>::from(Vector4<T>::data()); }
        constexpr const Color3<T> xyz() const { return Vector4<T>::xyz(); }

        Color3<T>& rgb() { return xyz(); }
        constexpr const Color3<T> rgb() const { return xyz(); }
        #endif

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(4, Color4)
};

/** @relatesalso Color3
@brief Convert color from CIE xyY representation to CIE XYZ

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
@brief Convert color from CIE XYZ representation to CIE xyY

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

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(4, Color4)
#endif

namespace Literals {

/** @relatesalso Magnum::Math::Color3
@brief 8bit-per-channel linear RGB literal

Unpacks the literal into three 8-bit values. Example usage:
@code
Color3ub a = 0x33b27f_rgb;  // {0x33, 0xb2, 0x7f}
@endcode

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. To convey such meaning, use the @link operator""_srgb() @endlink
    literal instead.

@see @link operator""_rgba() @endlink, @link operator""_rgbf() @endlink
*/
constexpr Color3<UnsignedByte> operator "" _rgb(unsigned long long value) {
    return {UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color3
@brief 8bit-per-channel sRGB literal

Unpacks the literal into three 8-bit values without any colorspace conversion.
Behaves identically to @link operator""_rgb() @endlink though it doesn't
return a @ref Color3 type to indicate that the resulting value is not linear
RGB. Use this literal to document that given value is in sRGB. Example usage:
@code
Math::Vector3<UnsignedByte> a = 0x33b27f_srgb;  // {0x33, 0xb2, 0x7f}
@endcode

@attention Note that colors in sRGB representation should not be used directly
    in calculations -- they should be converted to linear RGB, calculation done
    on the linear representation and then converted back to sRGB. Use the
    @link operator""_srgbf() @endlink literal if you want to get a linear RGB
    representation directly or convert the value using @ref Color3::fromSrgb().

@see @link operator""_srgba() @endlink, @link operator""_rgb() @endlink
*/
/* Output is a Vector3 to hint that it doesn't have any (additive,
   multiplicative) semantics of a linear RGB color */
constexpr Vector3<UnsignedByte> operator "" _srgb(unsigned long long value) {
    return {UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color4
@brief 8bit-per-channel linear RGBA literal

Unpacks the literal into four 8-bit values. Example usage:
@code
Color4ub a = 0x33b27fcc_rgba;   // {0x33, 0xb2, 0x7f, 0xcc}
@endcode

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. To convey such meaning, use the @link operator""_srgba() @endlink
    literal instead.

@see @link operator""_rgb() @endlink, @link operator""_rgbaf() @endlink
*/
constexpr Color4<UnsignedByte> operator "" _rgba(unsigned long long value) {
    return {UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color4
@brief 8bit-per-channel sRGB + alpha literal

Unpacks the literal into four 8-bit values without any colorspace conversion.
Behaves identically to @link operator""_rgba() @endlink though it doesn't
return a @ref Color4 type to indicate that the resulting value is not linear
RGBA. Use this literal to document that given value is in sRGB + alpha. Example
usage:
@code
Math::Vector4<UnsignedByte> a = 0x33b27fcc_srgba;   // {0x33, 0xb2, 0x7f, 0xcc}
@endcode

@attention Note that colors in sRGB representation should not be used directly
    in calculations -- they should be converted to linear RGB, calculation done
    on the linear representation and then converted back to sRGB. Use the
    @link operator""_srgbaf() @endlink literal if you want to get a linear RGBA
    representation directly or convert the value using @ref Color4::fromSrgbAlpha().

@see @link operator""_srgb() @endlink, @link operator""_rgba() @endlink
*/
/* Output is a Vector3 to hint that it doesn't have any (additive,
   multiplicative) semantics of a linear RGB color */
constexpr Vector4<UnsignedByte> operator "" _srgba(unsigned long long value) {
    return {UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)};
}

/** @relatesalso Magnum::Math::Color3
@brief Float linear RGB literal

Unpacks the 8-bit values into three floats. Example usage:
@code
Color3 a = 0x33b27f_rgbf;   // {0.2f, 0.698039f, 0.498039f}
@endcode

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. In that case use the @link operator""_srgbf() @endlink
    literal instead.

@see @link operator""_rgbaf() @endlink, @link operator""_rgb() @endlink
*/
inline Color3<Float> operator "" _rgbf(unsigned long long value) {
    return Math::unpack<Color3<Float>>(Color3<UnsignedByte>{UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)});
}

/** @relatesalso Magnum::Math::Color3
@brief Float sRGB literal

Unpacks the 8-bit values into three floats and converts the color space from
sRGB to linear RGB. See @ref Color3::fromSrgb() for more information. Example
usage:
@code
Color3 a = 0x33b27f_srgbf;  // {0.0331048f, 0.445201f, 0.212231f}
@endcode
@see @link operator""_srgbaf() @endlink, @link operator""_srgb() @endlink,
    @link operator""_rgbf() @endlink
*/
inline Color3<Float> operator "" _srgbf(unsigned long long value) {
    return Color3<Float>::fromSrgb<UnsignedByte>({UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)});
}

/** @relatesalso Magnum::Math::Color4
@brief Float linear RGBA literal

Unpacks the 8-bit values into four floats. Example usage:
@code
Color4 a = 0x33b27fcc_rgbaf;    // {0.2f, 0.698039f, 0.498039f, 0.8f}
@endcode

@attention 8bit-per-channel colors are commonly treated as being in sRGB color
    space, which is not directly usable in calculations and has to be converted
    to linear RGB first. In that case use the @link operator""_srgbaf() @endlink
    literal instead.

@see @link operator""_rgbf() @endlink, @link operator""_rgba() @endlink
*/
inline Color4<Float> operator "" _rgbaf(unsigned long long value) {
    return Math::unpack<Color4<Float>>(Color4<UnsignedByte>{UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)});
}

/** @relatesalso Magnum::Math::Color4
@brief Float sRGB + alpha literal

Unpacks the 8-bit values into four floats and converts the color space from
sRGB + alpha to linear RGBA. See @ref Color4::fromSrgbAlpha() for more
information. Example usage:
@code
Color4 a = 0x33b27fcc_srgbaf;  // {0.0331048f, 0.445201f, 0.212231f, 0.8f}
@endcode
@see @link operator""_srgbf() @endlink, @link operator""_srgba() @endlink,
    @link operator""_rgbaf() @endlink
*/
inline Color4<Float> operator "" _srgbaf(unsigned long long value) {
    return Color4<Float>::fromSrgbAlpha<UnsignedByte>({UnsignedByte(value >> 24), UnsignedByte(value >> 16), UnsignedByte(value >> 8), UnsignedByte(value)});
}

}

/**
@debugoperator{Magnum::Math::Color3}

Prints the value as hex color (e.g. `#ff33aa`). Other underlying types are
handled by @ref operator<<(Corrade::Utility::Debug&, const Vector<size, T>&).
*/
MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color3<UnsignedByte>& value);

/**
@debugoperator{Magnum::Math::Color4}

Prints the value as hex color (e.g. `#9933aaff`). Other underlying types are
handled by @ref operator<<(Corrade::Utility::Debug&, const Vector<size, T>&).
*/
MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color4<UnsignedByte>& value);

namespace Implementation {
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Color3} */
    template<class T> struct ConfigurationValue<Magnum::Math::Color3<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};

    /** @configurationvalue{Magnum::Color4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Color4<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
