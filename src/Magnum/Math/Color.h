#ifndef Magnum_Math_Color_h
#define Magnum_Math_Color_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Math::Color3, @ref Magnum::Math::Color4
 */

#include <tuple>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math {

namespace Implementation {

/* Convert color from HSV */
template<class T> typename std::enable_if<std::is_floating_point<T>::value, Color3<T>>::type fromHSV(typename Color3<T>::HSV hsv) {
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
        default: CORRADE_ASSERT_UNREACHABLE();
    }
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color3<T>>::type fromHSV(typename Color3<T>::HSV hsv) {
    return denormalize<Color3<T>>(fromHSV<typename Color3<T>::FloatingPointType>(hsv));
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
    return hue<typename Color3<T>::FloatingPointType>(normalize<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T> inline typename Color3<T>::FloatingPointType saturation(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type& color) {
    return saturation<typename Color3<T>::FloatingPointType>(normalize<Color3<typename Color3<T>::FloatingPointType>>(color));
}
template<class T> inline typename Color3<T>::FloatingPointType value(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return normalize<typename Color3<T>::FloatingPointType>(color.max());
}

/* Convert color to HSV */
template<class T> inline typename Color3<T>::HSV toHSV(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    T max = color.max();
    T delta = max - color.min();

    return typename Color3<T>::HSV(hue<typename Color3<T>::FloatingPointType>(color, max, delta), max != T(0) ? delta/max : T(0), max);
}
template<class T> inline typename Color3<T>::HSV toHSV(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return toHSV<typename Color3<T>::FloatingPointType>(normalize<Color3<typename Color3<T>::FloatingPointType>>(color));
}

/* Value for full channel (1.0f for floats, 255 for unsigned byte) */
template<class T> constexpr typename std::enable_if<std::is_floating_point<T>::value, T>::type fullChannel() {
    return T(1);
}
template<class T> constexpr typename std::enable_if<std::is_integral<T>::value, T>::type fullChannel() {
    return std::numeric_limits<T>::max();
}

}

/**
@brief Three-component (RGB) color

The class can store either floating-point (normalized) or integral
(denormalized) representation of color. Note that constructor conversion
between different types (like in @ref Vector classes) doesn't do any
(de)normalization, you should use @ref normalize() and
@ref denormalize() instead, for example:
@code
typedef Color3<UnsignedByte> Color3ub;
Color3 a(1.0f, 0.5f, 0.75f);
auto b = denormalize<Color3ub>(a); // b == {255, 127, 191}
@endcode

Conversion from and to HSV is done always using floating-point types, so hue
is always in range in range @f$ [0.0, 360.0] @f$, saturation and value in
range @f$ [0.0, 1.0] @f$.

@see @ref Color4, @ref Magnum::Color3, @ref Magnum::Color3ub
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

        /** @brief Corresponding floating-point type for HSV computation */
        typedef typename TypeTraits<T>::FloatingPointType FloatingPointType;

        /**
         * @brief Type for storing HSV values
         *
         * Hue in range @f$ [0.0, 360.0] @f$, saturation and value in
         * range @f$ [0.0, 1.0] @f$.
         */
        typedef std::tuple<Deg<FloatingPointType>, FloatingPointType, FloatingPointType> HSV;

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv Hue, saturation and value
         *
         * Hue can overflow the range @f$ [0.0, 360.0] @f$.
         */
        constexpr static Color3<T> fromHSV(HSV hsv) {
            return Implementation::fromHSV<T>(hsv);
        }
        /** @overload */
        constexpr static Color3<T> fromHSV(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value) {
            return fromHSV(std::make_tuple(hue, saturation, value));
        }

        /**
         * @brief Default constructor
         *
         * All components are set to zero.
         */
        constexpr /*implicit*/ Color3(ZeroInitT = ZeroInit)
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* MSVC 2015 can't handle {} here */
            : Vector3<T>(ZeroInit)
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Color3(NoInitT)
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* MSVC 2015 can't handle {} here */
            : Vector3<T>(NoInit)
            #endif
            {}

        /**
         * @brief Gray constructor
         * @param rgb   RGB value
         */
        constexpr explicit Color3(T rgb): Vector3<T>(rgb) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         */
        constexpr /*implicit*/ Color3(T r, T g, T b): Vector3<T>(r, g, b) {}

        /**
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (de)normalization, use
         *      @ref normalize() and @ref denormalize() instead.
         *      See class documentation for more information.
         */
        template<class U> constexpr explicit Color3(const Vector<3, U>& other): Vector3<T>(other) {}

        /** @brief Copy constructor */
        constexpr Color3(const Vector<3, T>& other): Vector3<T>(other) {}

        /**
         * @brief Convert to HSV
         *
         * Example usage:
         * @code
         * T hue, saturation, value;
         * std::tie(hue, saturation, value) = color.toHSV();
         * @endcode
         *
         * @see @ref hue(), @ref saturation(), @ref value(), @ref fromHSV()
         */
        constexpr HSV toHSV() const {
            return Implementation::toHSV<T>(*this);
        }

        /**
         * @brief Hue
         * @return Hue in range @f$ [0.0, 360.0] @f$.
         *
         * @see @ref saturation(), @ref value(), @ref toHSV(), @ref fromHSV()
         */
        constexpr Deg<FloatingPointType> hue() const {
            return Deg<FloatingPointType>(Implementation::hue<T>(*this));
        }

        /**
         * @brief Saturation
         * @return Saturation in range @f$ [0.0, 1.0] @f$.
         *
         * @see @ref hue(), @ref value(), @ref toHSV(), @ref fromHSV()
         */
        constexpr FloatingPointType saturation() const {
            return Implementation::saturation<T>(*this);
        }

        /**
         * @brief Value
         * @return Value in range @f$ [0.0, 1.0] @f$.
         *
         * @see @ref hue(), @ref saturation(), @ref toHSV(), @ref fromHSV()
         */
        constexpr FloatingPointType value() const {
            return Implementation::value<T>(*this);
        }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(3, Color3)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(3, Color3)
#endif

/**
@brief Four-component (RGBA) color

See @ref Color3 for more information.
@see @ref Magnum::Color4, @ref Magnum::Color4ub
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

        /** @copydoc Color3::HSV */
        typedef typename Color3<T>::HSV HSV;

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
         * @copydoc Color3::fromHSV()
         * @param a     Alpha value, defaults to `1.0` for floating-point types
         *      and maximum positive value for integral types.
         */
        constexpr static Color4<T> fromHSV(HSV hsv, T a = Implementation::fullChannel<T>()) {
            return Color4<T>(Implementation::fromHSV<T>(hsv), a);
        }
        /** @overload */
        constexpr static Color4<T> fromHSV(Deg<FloatingPointType> hue, FloatingPointType saturation, FloatingPointType value, T alpha = Implementation::fullChannel<T>()) {
            return fromHSV(std::make_tuple(hue, saturation, value), alpha);
        }

        /**
         * @brief Default constructor
         *
         * All components are set to zero.
         */
        constexpr /*implicit*/ Color4(): Vector4<T>(T(0), T(0), T(0), T(0)) {}

        /** @copydoc Vector::Vector(ZeroInitT) */
        constexpr explicit Color4(ZeroInitT)
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* MSVC 2015 can't handle {} here */
            : Vector4<T>(ZeroInit)
            #endif
            {}

        /** @copydoc Vector::Vector(NoInitT) */
        explicit Color4(NoInitT)
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* MSVC 2015 can't handle {} here */
            : Vector4<T>(NoInit)
            #endif
            {}

        /**
         * @copydoc Color3::Color3(T)
         * @param alpha Alpha value, defaults to `1.0` for floating-point types
         *      and maximum positive value for integral types.
         */
        constexpr explicit Color4(T rgb, T alpha = Implementation::fullChannel<T>()): Vector4<T>(rgb, rgb, rgb, alpha) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         * @param a     A value, defaults to `1.0` for floating-point types and
         *      maximum positive value for integral types.
         */
        constexpr /*implicit*/ Color4(T r, T g, T b, T a = Implementation::fullChannel<T>()): Vector4<T>(r, g, b, a) {}

        /**
         * @brief Constructor
         * @param rgb   Three-component color
         * @param a     A value
         */
        /* Not marked as explicit, because conversion from Color3 to Color4
           is fairly common, nearly always with A set to 1 */
        constexpr /*implicit*/ Color4(const Vector3<T>& rgb, T a = Implementation::fullChannel<T>()): Vector4<T>(rgb[0], rgb[1], rgb[2], a) {}

        /**
         * @copydoc Vector::Vector(const Vector<size, U>&)
         *
         * @attention This function doesn't do any (de)normalization, use
         *      @ref normalize() and @ref denormalize() instead.
         *      See @ref Color3 class documentation for more information.
         */
        template<class U> constexpr explicit Color4(const Vector<4, U>& other): Vector4<T>(other) {}

        /** @brief Copy constructor */
        constexpr Color4(const Vector<4, T>& other): Vector4<T>(other) {}

        /** @copydoc Color3::toHSV() */
        constexpr HSV toHSV() const {
            return Implementation::toHSV<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::hue() */
        constexpr Deg<FloatingPointType> hue() const {
            return Implementation::hue<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::saturation() */
        constexpr FloatingPointType saturation() const {
            return Implementation::saturation<T>(Vector4<T>::rgb());
        }

        /** @copydoc Color3::value() */
        constexpr FloatingPointType value() const {
            return Implementation::value<T>(Vector4<T>::rgb());
        }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(4, Color4)
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(4, Color4)
#endif

/** @debugoperator{Magnum::Math::Color3} */
template<class T> inline Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color3<T>& value) {
    return debug << static_cast<const Vector3<T>&>(value);
}

/** @debugoperator{Magnum::Math::Color4} */
template<class T> inline Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Color4<T>& value) {
    return debug << static_cast<const Vector4<T>&>(value);
}

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
