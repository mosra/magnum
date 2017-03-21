#ifndef Magnum_Math_Angle_h
#define Magnum_Math_Angle_h
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
 * @brief Class @ref Magnum::Math::Deg, @ref Magnum::Math::Rad, literal @link Magnum::Math::Literals::operator""_degf() @endlink, @link Magnum::Math::Literals::operator""_radf() @endlink, @link Magnum::Math::Literals::operator""_deg() @endlink, @link Magnum::Math::Literals::operator""_rad() @endlink
 */

#include <Corrade/configure.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/visibility.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Math/Unit.h"

namespace Magnum { namespace Math {

/**
@brief Angle in degrees

Along with Rad provides convenience classes to make angle specification and
conversion less error-prone.

## Usage

You can enter the value either by using literal:
@code
using namespace Literals;

auto degrees = 60.0_degf;       // type is Deg<Float>
auto radians = 1.047_rad;       // type is Rad<Double>
@endcode

Or explicitly convert unitless value (such as output from some function) to
either degrees or radians:
@code
Double foo();

Deg<Float> degrees(35.0f);
Rad<Double> radians(foo());
//degrees = 60.0f;              // error, no implicit conversion
@endcode

The classes support all arithmetic operations, such as addition, subtraction
or multiplication/division by unitless number:
@code
auto a = 60.0_degf + 17.35_degf;
auto b = -a + 23.0_degf*4;
//auto c = 60.0_degf*45.0_degf; // error, undefined resulting unit
@endcode

It is also possible to compare angles with all comparison operators, but
comparison of degrees and radians is not possible without explicit conversion
to common type:
@code
Rad<Float> angle();

Deg<Float> x = angle();         // convert to degrees for easier comparison
if(x < 30.0_degf) foo();
//if(x > 1.57_radf) bar();      // error, both need to be of the same type
@endcode

It is possible to seamlessly convert between degrees and radians and explicitly
convert the value back to underlying type:
@code
Float sine(Rad<Float> angle);
Float a = sine(60.0_degf);      // the same as sine(1.047_radf)
Deg<Double> b = 1.047_rad;      // the same as 60.0_deg
Float d = Double(b);            // 60.0
//Float e = b;                  // error, no implicit conversion
@endcode

## Requirement of explicit conversion

The requirement of explicit conversions from and to unitless types helps to
reduce unit-based errors. Consider following example with implicit conversions
allowed:
@code
namespace std { float sin(float angle); }
Float sine(Rad<Float> angle);

Float a = 60.0f;                // degrees
sine(a);                        // silent error, sine() expected radians

auto b = 60.0_degf;             // degrees
std::sin(b);                    // silent error, std::sin() expected radians
@endcode

These silent errors are easily avoided by requiring explicit conversions:
@code
//sine(a);                      // compilation error
sine(Deg<Float>{a});            // explicitly specifying unit

//std::sin(b);                  // compilation error
std::sin(Float(Rad<Float>(b));  // required explicit conversion hints to user
                                // that this case needs special attention
                                // (i.e., conversion to radians)
@endcode

@see @ref Magnum::Deg, @ref Magnum::Degd
*/
template<class T> class Deg: public Unit<Deg, T> {
    public:
        /** @brief Construct zero angle */
        constexpr /*implicit*/ Deg(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Unit<Math::Deg, T>{ZeroInit}
            #endif
            {}

        /** @brief Construct without initializing the contents */
        explicit Deg(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Unit<Math::Deg, T>{NoInit}
            #endif
            {}

        /** @brief Explicit constructor from unitless type */
        constexpr explicit Deg(T value) noexcept: Unit<Math::Deg, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Deg(Unit<Math::Deg, U> value) noexcept: Unit<Math::Deg, T>(value) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Deg(Unit<Math::Deg, T> other) noexcept: Unit<Math::Deg, T>(other) {}

        /**
         * @brief Construct degrees from radians
         *
         * Performs conversion from radians to degrees, i.e.:
         * @f[
         *      deg = 180 \frac {rad} \pi
         * @f]
         */
        constexpr /*implicit*/ Deg(Unit<Rad, T> value);
};

namespace Literals {

/** @relatesalso Magnum::Math::Deg
@brief Double-precision degree value literal

Example usage:
@code
Double cosine = Math::cos(60.0_deg);  // cosine = 0.5
Double cosine = Math::cos(1.047_rad); // cosine = 0.5
@endcode
@see @link operator""_degf() @endlink, @link operator""_rad() @endlink
*/
constexpr Deg<Double> operator "" _deg(long double value) { return Deg<Double>(Double(value)); }

/** @relatesalso Magnum::Math::Deg
@brief Single-precision degree value literal

Example usage:
@code
Float tangent = Math::tan(60.0_degf);  // tangent = 1.732f
Float tangent = Math::tan(1.047_radf); // tangent = 1.732f
@endcode
@see @link operator""_deg() @endlink, @link operator""_radf() @endlink
*/
constexpr Deg<Float> operator "" _degf(long double value) { return Deg<Float>(Float(value)); }

}

/**
@brief Angle in radians

See @ref Deg for more information.
@see @ref Magnum::Rad, @ref Magnum::Radd
*/
template<class T> class Rad: public Unit<Rad, T> {
    public:
        /** @brief Default constructor */
        constexpr /*implicit*/ Rad(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Unit<Math::Rad, T>{ZeroInit}
            #endif
            {}

        /** @brief Construct without initializing the contents */
        explicit Rad(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Unit<Math::Rad, T>{NoInit}
            #endif
            {}

        /** @brief Construct from unitless type */
        constexpr explicit Rad(T value) noexcept: Unit<Math::Rad, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Rad(Unit<Math::Rad, U> value) noexcept: Unit<Math::Rad, T>(value) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Rad(Unit<Math::Rad, T> value) noexcept: Unit<Math::Rad, T>(value) {}

        /**
         * @brief Construct radians from degrees
         *
         * Performs conversion from degrees to radians, i.e.:
         * @f[
         *      rad = deg \frac \pi 180
         * @f]
         */
        constexpr /*implicit*/ Rad(Unit<Deg, T> value);
};

namespace Literals {

/** @relatesalso Magnum::Math::Rad
@brief Double-precision radian value literal

See @link operator""_deg() @endlink for more information.
@see @link operator""_radf() @endlink
*/
constexpr Rad<Double> operator "" _rad(long double value) { return Rad<Double>(Double(value)); }

/** @relatesalso Magnum::Math::Rad
@brief Single-precision radian value literal

See @link operator""_degf() @endlink for more information.
@see @link operator""_rad() @endlink
*/
constexpr Rad<Float> operator "" _radf(long double value) { return Rad<Float>(Float(value)); }

}

template<class T> constexpr Deg<T>::Deg(Unit<Rad, T> value): Unit<Math::Deg, T>(T(180)*T(value)/Math::Constants<T>::pi()) {}
template<class T> constexpr Rad<T>::Rad(Unit<Deg, T> value): Unit<Math::Rad, T>(T(value)*Math::Constants<T>::pi()/T(180)) {}

/** @debugoperator{Magnum::Math::Rad} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Unit<Rad, T>& value) {
    return debug << "Rad(" << Corrade::Utility::Debug::nospace << T(value) << Corrade::Utility::Debug::nospace << ")";
}

/** @debugoperator{Magnum::Math::Deg} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Unit<Deg, T>& value) {
    return debug << "Deg(" << Corrade::Utility::Debug::nospace << T(value) << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Unit<Rad, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Unit<Deg, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Unit<Rad, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Unit<Deg, Double>&);
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Deg} */
template<class T> struct ConfigurationValue<Magnum::Math::Deg<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes degrees as a number */
    static std::string toString(const Magnum::Math::Deg<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<T>::toString(T(value), flags);
    }

    /** @brief Reads degrees as a number */
    static Magnum::Math::Deg<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        return Magnum::Math::Deg<T>(ConfigurationValue<T>::fromString(stringValue, flags));
    }
};

/** @configurationvalue{Magnum::Math::Rad} */
template<class T> struct ConfigurationValue<Magnum::Math::Rad<T>> {
    ConfigurationValue() = delete;

    /** @brief Writes degrees as a number */
    static std::string toString(const Magnum::Math::Rad<T>& value, ConfigurationValueFlags flags) {
        return ConfigurationValue<T>::toString(T(value), flags);
    }

    /** @brief Reads degrees as a number */
    static Magnum::Math::Rad<T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        return Magnum::Math::Rad<T>(ConfigurationValue<T>::fromString(stringValue, flags));
    }
};

}}

#endif
