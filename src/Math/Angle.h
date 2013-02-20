#ifndef Magnum_Math_Angle_h
#define Magnum_Math_Angle_h
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
 * @brief Class Magnum::Math::Deg, Magnum::Math::Rad and related operators.
 */

#include <Utility/Debug.h>

#include "Math/Constants.h"
#include "Math/Math.h"
#include "Math/Unit.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Math {

/**
@brief Angle in degrees

Along with Rad provides convenience classes to make angle specification and
conversion less error-prone.

@section Rad-usage Usage

You can enter the value either by using literal:
@code
auto degrees = 60.0_degf;       // type is Deg<float>
auto radians = 1.047_rad;       // type is Rad<double>
@endcode

Or explicitly convert unitless value (such as output from some function) to
either degrees or radians:
@code
double foo();

Deg<float> degrees(35.0f);
Rad<double> radians(foo());
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
Rad<float> angle();

Deg<float> x = angle();         // convert to degrees for easier comparison
if(x < 30.0_degf) foo();
//if(x > 1.57_radf) bar();      // error, both need to be of the same type
@endcode

It is possible to seamlessly convert between degrees and radians and explicitly
convert the value back to underlying type:
@code
float sine(Rad<float> angle);
float a = sine(60.0_degf);      // the same as sine(1.047_radf)
Deg<double> b = 1.047_rad;      // the same as 60.0_deg
float d = double(b);            // 60.0
//float e = b;                  // error, no implicit conversion
@endcode

@section Rad-conversions Requirement of explicit conversion

The requirement of explicit conversions from and to unitless types helps to
reduce unit-based errors. Consider following example with implicit conversions
allowed:
@code
float std::sin(float angle);
float sine(Rad<float> angle);

float a = 60.0f;                // degrees
sine(a);                        // silent error, sine() expected radians

auto b = 60.0_degf;             // degrees
std::sin(b);                    // silent error, std::sin() expected radians
@endcode

These silent errors are easily avoided by requiring explicit conversions:
@code
//sine(angleInDegrees);                     // compilation error
sine(Deg<float>(angleInDegrees));           // explicitly specifying unit

//std::sin(angleInDegrees);                 // compilation error
std::sin(float(Rad<float>(angleInDegrees)); // required explicit conversion hints
                                            // to user that this case needs special
                                            // attention (i.e., conversion to radians)
@endcode
*/
template<class T> class Deg: public Unit<Deg, T> {
    public:
        /** @brief Default constructor */
        inline constexpr /*implicit*/ Deg() {}

        /** @brief Explicit constructor from unitless type */
        inline constexpr explicit Deg(T value): Unit<Deg, T>(value) {}

        /** @brief Copy constructor */
        inline constexpr /*implicit*/ Deg(Unit<Deg, T> value): Unit<Deg, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> inline constexpr explicit Deg(Unit<Deg, U> value): Unit<Deg, T>(value) {}

        /**
         * @brief Construct degrees from radians
         *
         * Performs conversion from radians to degrees, i.e.:
         * @f[
         *      deg = 180 \frac {rad} \pi
         * @f]
         */
        inline constexpr /*implicit*/ Deg(Unit<Rad, T> value);
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/** @relates Deg
@brief Double-precision degree value literal

Example usage:
@code
double cosine = Math::cos(60.0_deg);  // cosine = 0.5
double cosine = Math::cos(1.047_rad); // cosine = 0.5
@endcode
@see operator""_degf(), operator""_rad()
@note Not available on GCC < 4.7. Use Deg::Deg(T) instead.
*/
inline constexpr Deg<double> operator "" _deg(long double value) { return Deg<double>(value); }

/** @relates Deg
@brief Single-precision degree value literal

Example usage:
@code
float tangent = Math::tan(60.0_degf);  // tangent = 1.732f
float tangent = Math::tan(1.047_radf); // tangent = 1.732f
@endcode
@see operator""_deg(), operator""_radf()
@note Not available on GCC < 4.7. Use Deg::Deg(T) instead.
*/
inline constexpr Deg<float> operator "" _degf(long double value) { return Deg<float>(value); }
#endif

/**
@brief Angle in radians

See Deg for more information.
*/
template<class T> class Rad: public Unit<Rad, T> {
    public:
        /** @brief Default constructor */
        inline constexpr /*implicit*/ Rad() {}

        /** @brief Construct from unitless type */
        inline constexpr explicit Rad(T value): Unit<Rad, T>(value) {}

        /** @brief Copy constructor */
        inline constexpr /*implicit*/ Rad(Unit<Rad, T> value): Unit<Rad, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> inline constexpr explicit Rad(Unit<Rad, U> value): Unit<Rad, T>(value) {}

        /**
         * @brief Construct radians from degrees
         *
         * Performs conversion from degrees to radians, i.e.:
         * @f[
         *      rad = deg \frac \pi 180
         * @f]
         */
        inline constexpr /*implicit*/ Rad(Unit<Deg, T> value);
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/** @relates Rad
@brief Double-precision radian value literal

See operator""_rad() for more information.
@see operator""_radf(), operator""_deg()
@note Not available on GCC < 4.7. Use Rad::Rad(T) instead.
*/
inline constexpr Rad<double> operator "" _rad(long double value) { return Rad<double>(value); }

/** @relates Rad
@brief Single-precision radian value literal

See operator""_degf() for more information.
@see operator""_rad(), operator""_degf()
@note Not available on GCC < 4.7. Use Rad::Rad(T) instead.
*/
inline constexpr Rad<float> operator "" _radf(long double value) { return Rad<float>(value); }
#endif

template<class T> inline constexpr Deg<T>::Deg(Unit<Rad, T> value): Unit<Deg, T>(T(180)*T(value)/Math::Constants<T>::pi()) {}
template<class T> inline constexpr Rad<T>::Rad(Unit<Deg, T> value): Unit<Rad, T>(T(value)*Math::Constants<T>::pi()/T(180)) {}

/** @debugoperator{Magnum::Math::Rad} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Rad<T>& value) {
    debug << "Rad(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << T(value) << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/** @debugoperator{Magnum::Math::Deg} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Deg<T>& value) {
    debug << "Deg(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << T(value) << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Rad<float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Deg<float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Rad<double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Deg<double>&);
#endif
#endif

}}

#endif
