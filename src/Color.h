#ifndef Magnum_Color_h
#define Magnum_Color_h
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
 * @brief Class Magnum::Color3, Magnum::Color4
 */

#include <tuple>

#include "Math/MathTypeTraits.h"
#include "Math/Math.h"
#include "Math/Vector4.h"
#include "Magnum.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

/* Convert color from HSV */
template<class T> inline typename std::enable_if<std::is_floating_point<T>::value, Color3<T>>::type fromHSV(typename Color3<T>::HSV hsv) {
    T hue, saturation, value;
    std::tie(hue, saturation, value) = hsv;

    /* Remove repeats */
    hue -= int(hue/T(360))*T(360);
    if(hue < T(0)) hue += T(360);

    int h = int(hue/T(60)) % 6;
    T f = hue/T(60) - h;

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
        default:
            CORRADE_ASSERT(false, "It shouldn't get here.", {});
    }
}
template<class T> inline typename std::enable_if<std::is_integral<T>::value, Color3<T>>::type fromHSV(typename Color3<T>::HSV hsv) {
    return Color3<T>::fromNormalized(fromHSV<typename Color3<T>::FloatingPointType>(hsv));
}

/* Internal hue computing function */
template<class T> T hue(const Color3<T>& color, T max, T delta) {
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

    return hue;
}

/* Hue, saturation, value for floating-point types */
template<class T> inline T hue(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
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
template<class T> inline typename Color3<T>::FloatingPointType hue(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return hue<typename Color3<T>::FloatingPointType>(Color3<typename Color3<T>::FloatingPointType>::fromDenormalized(color));
}
template<class T> inline typename Color3<T>::FloatingPointType saturation(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type& color) {
    return saturation<typename Color3<T>::FloatingPointType>(Color3<typename Color3<T>::FloatingPointType>::fromDenormalized(color));
}
template<class T> inline typename Color3<T>::FloatingPointType value(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return Math::normalize<typename Color3<T>::FloatingPointType>(color.max());
}

/* Convert color to HSV */
template<class T> inline typename Color3<T>::HSV toHSV(typename std::enable_if<std::is_floating_point<T>::value, const Color3<T>&>::type color) {
    T max = color.max();
    T delta = max - color.min();

    return typename Color3<T>::HSV(hue<typename Color3<T>::FloatingPointType>(color, max, delta), max != T(0) ? delta/max : T(0), max);
}
template<class T> inline typename Color3<T>::HSV toHSV(typename std::enable_if<std::is_integral<T>::value, const Color3<T>&>::type color) {
    return toHSV<typename Color3<T>::FloatingPointType>(Color3<typename Color3<T>::FloatingPointType>::fromDenormalized(color));
}

/* Default alpha value */
template<class T> inline constexpr typename std::enable_if<std::is_floating_point<T>::value, T>::type defaultAlpha() {
    return T(1);
}
template<class T> inline constexpr typename std::enable_if<std::is_integral<T>::value, T>::type defaultAlpha() {
    return std::numeric_limits<T>::max();
}

}
#endif

/**
@brief Three-component (RGB) color

The class can store both floating-point (normalized) and integral
(denormalized) representation of color. You can convert between these two
representations using fromNormalized() and fromDenormalized().

Conversion from and to HSV is done always using floating-point types, so hue
is always in range in range @f$ [0.0, 360.0] @f$, saturation and value in
range @f$ [0.0, 1.0] @f$.

@see Color4

@todo Hue in degrees so users can use deg()
@todo Signed normalization to [-1.0, 1.0] like in OpenGL?
*/
/* Not using template specialization because some internal functions are
   impossible to explicitly instantiate */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class Color3: public Math::Vector3<T> {
    public:
        /** @brief Corresponding floating-point type for HSV computation */
        typedef typename Math::MathTypeTraits<T>::FloatingPointType FloatingPointType;

        /**
         * @brief Type for storing HSV values
         *
         * Hue in range @f$ [0.0, 360.0] @f$, saturation and value in
         * range @f$ [0.0, 1.0] @f$.
         */
        typedef std::tuple<FloatingPointType, FloatingPointType, FloatingPointType> HSV;

        /**
         * @brief Create integral color from floating-point color
         *
         * E.g. `{0.294118, 0.45098, 0.878431}` is converted to
         * `{75, 115, 224}`, if resulting type is `uint8_t`.
         *
         * @note This function is enabled only if source type is floating-point
         *      and destination type is integral.
         */
        template<class U> inline constexpr static typename std::enable_if<std::is_integral<T>::value && std::is_floating_point<U>::value, Color3<T>>::type fromNormalized(const Color3<U>& color) {
            return Color3<T>(Math::denormalize<T>(color.r()),
                             Math::denormalize<T>(color.g()),
                             Math::denormalize<T>(color.b()));
        }

        /**
         * @brief Create floating-point color from integral color
         *
         * E.g. `{75, 115, 224}` is converted to
         * `{0.294118, 0.45098, 0.878431}`, if source type is `uint8_t`.
         *
         * @note This function is enabled only if source type is integral
         *      and destination type is floating-point.
         */
        template<class U> inline constexpr static typename std::enable_if<std::is_floating_point<T>::value && std::is_integral<U>::value, Color3<T>>::type fromDenormalized(const Color3<U>& color) {
            return Color3<T>(Math::normalize<T>(color.r()),
                             Math::normalize<T>(color.g()),
                             Math::normalize<T>(color.b()));
        }

        /**
         * @brief Create RGB color from HSV representation
         * @param hsv Hue, saturation and value
         *
         * Hue can overflow the range @f$ [0.0, 360.0] @f$.
         */
        inline constexpr static Color3<T> fromHSV(HSV hsv) {
            return Implementation::fromHSV<T>(hsv);
        }
        /** @overload */
        inline constexpr static Color3<T> fromHSV(FloatingPointType hue, FloatingPointType saturation, FloatingPointType value) {
            return fromHSV(std::make_tuple(hue, saturation, value));
        }

        /**
         * @brief Default constructor
         *
         * All components are set to zero.
         */
        inline constexpr Color3() {}

        /**
         * @brief Gray constructor
         * @param rgb   RGB value
         */
        inline constexpr explicit Color3(T rgb): Math::Vector3<T>(rgb) {}

        /** @brief Copy constructor */
        inline constexpr Color3(const Math::RectangularMatrix<1, 3, T>& other): Math::Vector3<T>(other) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         */
        inline constexpr Color3(T r, T g, T b): Math::Vector3<T>(r, g, b) {}

        inline T& r() { return Math::Vector3<T>::x(); }                 /**< @brief R component */
        inline constexpr T r() const { return Math::Vector3<T>::x(); }  /**< @overload */
        inline T& g() { return Math::Vector3<T>::y(); }                 /**< @brief G component */
        inline constexpr T g() const { return Math::Vector3<T>::y(); }  /**< @overload */
        inline T& b() { return Math::Vector3<T>::z(); }                 /**< @brief B component */
        inline constexpr T b() const { return Math::Vector3<T>::z(); }  /**< @overload */

        /**
         * @brief Convert to HSV
         *
         * Example usage:
         * @code
         * T hue, saturation, value;
         * std::tie(hue, saturation, value) = color.toHSV();
         * @endcode
         *
         * @see hue(), saturation(), value(), fromHSV()
         */
        inline constexpr HSV toHSV() const {
            return Implementation::toHSV<T>(*this);
        }

        /**
         * @brief Hue
         * @return Hue in range @f$ [0.0, 360.0] @f$.
         *
         * @see saturation(), value(), toHSV(), fromHSV()
         */
        inline constexpr FloatingPointType hue() const {
            return Implementation::hue<T>(*this);
        }

        /**
         * @brief Saturation
         * @return Saturation in range @f$ [0.0, 1.0] @f$.
         *
         * @see hue(), value(), toHSV(), fromHSV()
         */
        inline constexpr FloatingPointType saturation() const {
            return Implementation::saturation<T>(*this);
        }

        /**
         * @brief Value
         * @return Value in range @f$ [0.0, 1.0] @f$.
         *
         * @see hue(), saturation(), toHSV(), fromHSV()
         */
        inline constexpr FloatingPointType value() const {
            return Implementation::value<T>(*this);
        }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Color3, 3)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, 3, Color3<T>)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Color3, 3)

/**
@brief Four-component (RGBA) color

See Color3 for more information.
*/
/* Not using template specialization because some internal functions are
   impossible to explicitly instantiate */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class Color4: public Math::Vector4<T> {
    public:
        /** @copydoc Color3::FloatingPointType */
        typedef typename Color3<T>::FloatingPointType FloatingPointType;

        /** @copydoc Color3::HSV */
        typedef typename Color3<T>::HSV HSV;

        /** @copydoc Color3::fromNormalized() */
        template<class U> inline constexpr static typename std::enable_if<std::is_integral<T>::value && std::is_floating_point<U>::value, Color4<T>>::type fromNormalized(const Color4<U>& color) {
            return Color4<T>(Math::denormalize<T>(color.r()),
                             Math::denormalize<T>(color.g()),
                             Math::denormalize<T>(color.b()),
                             Math::denormalize<T>(color.a()));
        }

        /** @copydoc Color3::fromDenormalized() */
        template<class U> inline constexpr static typename std::enable_if<std::is_floating_point<T>::value && std::is_integral<U>::value, Color4<T>>::type fromDenormalized(const Color4<U>& color) {
            return Color4<T>(Math::normalize<T>(color.r()),
                             Math::normalize<T>(color.g()),
                             Math::normalize<T>(color.b()),
                             Math::normalize<T>(color.a()));
        }

        /**
         * @copydoc Color3::fromHSV()
         * @param a     Alpha value, defaults to 1.0 for floating-point types
         *      and maximum positive value for integral types.
         */
        inline constexpr static Color4<T> fromHSV(HSV hsv, T a = Implementation::defaultAlpha<T>()) {
            return Color4<T>(Implementation::fromHSV<T>(hsv), a);
        }
        /** @overload */
        inline constexpr static Color4<T> fromHSV(FloatingPointType hue, FloatingPointType saturation, FloatingPointType value, T alpha) {
            return fromHSV(std::make_tuple(hue, saturation, value), alpha);
        }

        /**
         * @brief Default constructor
         *
         * RGB components are set to zero, A component is set to 1.0 for
         * floating-point types and maximum positive value for integral types.
         */
        inline constexpr Color4(): Math::Vector4<T>(T(0), T(0), T(0), Implementation::defaultAlpha<T>()) {}

        /**
         * @copydoc Color3::Color3(T)
         * @param alpha Alpha value, defaults to 1.0 for floating-point types
         *      and maximum positive value for integral types.
         */
        inline constexpr explicit Color4(T rgb, T alpha = Implementation::defaultAlpha<T>()): Math::Vector4<T>(rgb, rgb, rgb, alpha) {}

        /** @brief Copy constructor */
        inline constexpr Color4(const Math::RectangularMatrix<1, 4, T>& other): Math::Vector4<T>(other) {}

        /**
         * @brief Constructor
         * @param r     R value
         * @param g     G value
         * @param b     B value
         * @param a     A value, defaults to 1.0 for floating-point types and
         *      maximum positive value for integral types.
         */
        inline constexpr Color4(T r, T g, T b, T a = Implementation::defaultAlpha<T>()): Math::Vector4<T>(r, g, b, a) {}

        /**
         * @brief Constructor
         * @param rgb   Three-component color
         * @param a     A value
         */
        /* Not marked as explicit, because conversion from Color3 to Color4
           is fairly common, nearly always with A set to 1 */
        inline constexpr Color4(const Math::Vector3<T>& rgb, T a = Implementation::defaultAlpha<T>()): Math::Vector4<T>(rgb[0], rgb[1], rgb[2], a) {}

        inline T& r() { return Math::Vector4<T>::x(); }                 /**< @brief R component */
        inline constexpr T r() const { return Math::Vector4<T>::x(); }  /**< @overload */
        inline T& g() { return Math::Vector4<T>::y(); }                 /**< @brief G component */
        inline constexpr T g() const { return Math::Vector4<T>::y(); }  /**< @overload */
        inline T& b() { return Math::Vector4<T>::z(); }                 /**< @brief B component */
        inline constexpr T b() const { return Math::Vector4<T>::z(); }  /**< @overload */
        inline T& a() { return Math::Vector4<T>::w(); }                 /**< @brief A component */
        inline constexpr T a() const { return Math::Vector4<T>::w(); }  /**< @overload */

        /**
         * @brief RGB part of the vector
         * @return First three components of the vector
         *
         * @see swizzle()
         */
        inline Color3<T>& rgb() { return Color3<T>::from(Math::Vector4<T>::data()); }
        inline constexpr Color3<T> rgb() const { return Color3<T>::from(Math::Vector4<T>::data()); } /**< @overload */

        /** @copydoc Color3::toHSV() */
        inline constexpr HSV toHSV() const {
            return Implementation::toHSV<T>(rgb());
        }

        /** @copydoc Color3::hue() */
        inline constexpr FloatingPointType hue() const {
            return Implementation::hue<T>(rgb());
        }

        /** @copydoc Color3::saturation() */
        inline constexpr FloatingPointType saturation() const {
            return Implementation::saturation<T>(rgb());
        }

        /** @copydoc Color3::value() */
        inline constexpr FloatingPointType value() const {
            return Implementation::value<T>(rgb());
        }

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Color4, 4)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, 4, Color4<T>)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Color4, 4)

/** @debugoperator{Magnum::Color3} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Color3<T>& value) {
    return debug << static_cast<const Math::Vector3<T>&>(value);
}

/** @debugoperator{Magnum::Color4} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Color4<T>& value) {
    return debug << static_cast<const Math::Vector4<T>&>(value);
}

}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Color3} */
    template<class T> struct ConfigurationValue<Magnum::Color3<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};

    /** @configurationvalue{Magnum::Color4} */
    template<class T> struct ConfigurationValue<Magnum::Color4<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
