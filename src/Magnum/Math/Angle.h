#ifndef Magnum_Math_Angle_h
#define Magnum_Math_Angle_h
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
 * @brief Class @ref Magnum::Math::Deg, @ref Magnum::Math::Rad, literal @link Magnum::Math::Literals::AngleLiterals::operator""_degf() @endlink, @link Magnum::Math::Literals::AngleLiterals::operator""_radf() @endlink, @link Magnum::Math::Literals::AngleLiterals::operator""_deg() @endlink, @link Magnum::Math::Literals::AngleLiterals::operator""_rad() @endlink
 */

#ifndef CORRADE_SINGLES_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif

#include "Magnum/visibility.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Math/Unit.h"

namespace Magnum { namespace Math {

/**
@brief Angle in degrees

Along with @ref Rad provides convenience classes to make angle specification
and conversion less error-prone.

@section Math-Deg-usage Usage

You can create the value either by using a literal:

@snippet Math.cpp Deg-usage

Or explicitly convert a unitless value (such as an output from some function)
to either degrees or radians:

@snippet Math.cpp Deg-usage-convert

The classes support all arithmetic operations, such as addition, subtraction
or multiplication/division by a unitless number:

@snippet Math.cpp Deg-usage-operations

It is also possible to compare angles with all comparison operators, but
comparison of degrees and radians is not possible without explicit conversion
to a common type:

@snippet Math.cpp Deg-usage-comparison

It is possible to seamlessly convert between degrees and radians and explicitly
convert the value back to the underlying type:

@snippet Math.cpp Deg-usage-conversion

@section Math-Angle-explicit-conversion Requirement of explicit conversion

The requirement of explicit conversions from and to unitless types helps to
reduce unit-based errors. Consider the following example that would compile
only if implicit conversions were allowed:

@code{.cpp}
namespace std { float sin(float angle); }
Float sine(Rad angle);

Float a = 60.0f;                // degrees
sine(a);                        // silent error, sine() expected radians

auto b = 60.0_degf;             // degrees
std::sin(b);                    // silent error, std::sin() expected radians
@endcode

These silent errors are easily avoided by requiring explicit conversions:

@snippet Math.cpp Deg-usage-explicit-conversion

@see @link Literals::AngleLiterals::operator""_degf() @endlink,
    @link Literals::AngleLiterals::operator""_deg() @endlink, @ref Magnum::Deg,
    @ref Magnum::Degh, @ref Magnum::Degd
*/
template<class T> class Deg: public Unit<Deg, T> {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Deg(ZeroInitT).
         */
        /* Needs to be Math::Deg here and in all other places because older
           Clang and both MSVC 2015 and 2017 treat it as a template instance
           Deg<T> instead of a Deg template */
        constexpr /*implicit*/ Deg() noexcept: Unit<Math::Deg, T>{ZeroInit} {}

        /** @brief Construct a zero angle */
        constexpr explicit Deg(ZeroInitT) noexcept: Unit<Math::Deg, T>{ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit Deg(Magnum::NoInitT) noexcept: Unit<Math::Deg, T>{Magnum::NoInit} {}

        /** @brief Explicit constructor from a unitless type */
        constexpr explicit Deg(T value) noexcept: Unit<Math::Deg, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Deg(Unit<Math::Deg, U> value) noexcept: Unit<Math::Deg, T>(value) {}

        /** @brief Copy constructor */
        /* Needed in order to make arithmetic operations (which have a Unit
           return type) convertible to Deg */
        constexpr /*implicit*/ Deg(Unit<Math::Deg, T> other) noexcept: Unit<Math::Deg, T>(other) {}

        /**
         * @brief Construct degrees from radians
         *
         * Performs conversion from radians to degrees, i.e.: @f[
         *      deg = rad \frac{180}{\pi}
         * @f]
         * @m_keyword{degrees(),GLSL degrees(),}
         */
        constexpr /*implicit*/ Deg(Unit<Rad, T> value) noexcept;
};

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
    namespace AngleLiterals {

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
/** @relatesalso Magnum::Math::Deg
@brief Double-precision degree value literal

Example usage:

@snippet Math.cpp _deg

@see @link operator""_degf() @endlink, @link operator""_rad() @endlink
@m_keywords{_deg deg}
*/
constexpr Deg<Double> operator"" _deg(long double value) { return Deg<Double>(Double(value)); }

/** @relatesalso Magnum::Math::Deg
@brief Single-precision degree value literal

Example usage:

@snippet Math.cpp _degf

@see @link operator""_deg() @endlink, @link operator""_radf() @endlink
@m_keywords{_degf degf}
*/
constexpr Deg<Float> operator"" _degf(long double value) { return Deg<Float>(Float(value)); }
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic pop
#endif

}}

/**
@brief Angle in radians

See @ref Deg for more information.
@see @link Literals::AngleLiterals::operator""_radf() @endlink,
    @link Literals::AngleLiterals::operator""_rad() @endlink, @ref Magnum::Rad,
    @ref Magnum::Radh, @ref Magnum::Radd
*/
template<class T> class Rad: public Unit<Rad, T> {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Rad(ZeroInitT).
         */
        constexpr /*implicit*/ Rad() noexcept: Unit<Math::Rad, T>{ZeroInit} {}

        /** @brief Constructor a zero angle */
        constexpr explicit Rad(ZeroInitT) noexcept: Unit<Math::Rad, T>{ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit Rad(Magnum::NoInitT) noexcept: Unit<Math::Rad, T>{Magnum::NoInit} {}

        /** @brief Construct from unitless type */
        constexpr explicit Rad(T value) noexcept: Unit<Math::Rad, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Rad(Unit<Math::Rad, U> value) noexcept: Unit<Math::Rad, T>(value) {}

        /** @brief Copy constructor */
        /* Needed in order to make arithmetic operations (which have a Unit
           return type) convertible to Rad */
        constexpr /*implicit*/ Rad(Unit<Math::Rad, T> value) noexcept: Unit<Math::Rad, T>(value) {}

        /**
         * @brief Construct radians from degrees
         *
         * Performs conversion from degrees to radians, i.e.: @f[
         *      rad = deg \frac{\pi}{180}
         * @f]
         * @m_keyword{radians(),GLSL radians(),}
         */
        constexpr /*implicit*/ Rad(Unit<Deg, T> value) noexcept;
};

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
    namespace AngleLiterals {

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
/** @relatesalso Magnum::Math::Rad
@brief Double-precision radian value literal

See @link operator""_deg() @endlink for more information.
@see @link operator""_radf() @endlink
@m_keywords{_rad rad}
*/
constexpr Rad<Double> operator"" _rad(long double value) { return Rad<Double>(Double(value)); }

/** @relatesalso Magnum::Math::Rad
@brief Single-precision radian value literal

See @link operator""_degf() @endlink for more information.
@see @link operator""_rad() @endlink
@m_keywords{_radf radf}
*/
constexpr Rad<Float> operator"" _radf(long double value) { return Rad<Float>(Float(value)); }
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic pop
#endif

}}

template<class T> constexpr Deg<T>::Deg(Unit<Rad, T> value) noexcept: Unit<Math::Deg, T>(T(180)*T(value)/Math::Constants<T>::pi()) {}
template<class T> constexpr Rad<T>::Rad(Unit<Deg, T> value) noexcept: Unit<Math::Rad, T>(T(value)*Math::Constants<T>::pi()/T(180)) {}

#ifndef CORRADE_SINGLES_NO_DEBUG
/** @debugoperator{Rad} */
template<class T> Utility::Debug& operator<<(Utility::Debug& debug, const Unit<Rad, T>& value) {
    if(debug.immediateFlags() >= Utility::Debug::Flag::Packed)
        return debug << T(value);
    return debug << "Rad(" << Utility::Debug::nospace << T(value) << Utility::Debug::nospace << ")";
}

/** @debugoperator{Deg} */
template<class T> Utility::Debug& operator<<(Utility::Debug& debug, const Unit<Deg, T>& value) {
    if(debug.immediateFlags() >= Utility::Debug::Flag::Packed)
        return debug << T(value);
    return debug << "Deg(" << Utility::Debug::nospace << T(value) << Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug&, const Unit<Rad, Float>&);
extern template MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug&, const Unit<Deg, Float>&);
extern template MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug&, const Unit<Rad, Double>&);
extern template MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug&, const Unit<Deg, Double>&);
#endif
#endif

}}

#if !defined(CORRADE_NO_TWEAKABLE) && (defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN))
namespace Corrade { namespace Utility {

/**
@tweakableliteral{Magnum::Math::Deg}

Parses the @link Magnum::Math::Literals::AngleLiterals::operator""_degf @endlink
literal.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Deg<Magnum::Float>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Deg<Magnum::Float>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Needed to parse e.g. -1.0_degf, which is the base class type */
template<> struct TweakableParser<Magnum::Math::Unit<Magnum::Math::Deg, Magnum::Float>>: TweakableParser<Magnum::Math::Deg<Magnum::Float>> {};
#endif

/**
@tweakableliteral{Magnum::Math::Deg}

Parses the @link Magnum::Math::Literals::AngleLiterals::operator""_deg @endlink
literal.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Deg<Magnum::Double>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Deg<Magnum::Double>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Needed to parse e.g. -1.0_deg, which is the base class type */
template<> struct TweakableParser<Magnum::Math::Unit<Magnum::Math::Deg, Magnum::Double>>: TweakableParser<Magnum::Math::Deg<Magnum::Double>> {};
#endif

/**
@tweakableliteral{Magnum::Math::Rad}

Parses the @link Magnum::Math::Literals::AngleLiterals::operator""_radf @endlink
literal.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Rad<Magnum::Float>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Rad<Magnum::Float>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Needed to parse e.g. -1.0_radf, which is the base class type */
template<> struct TweakableParser<Magnum::Math::Unit<Magnum::Math::Rad, Magnum::Float>>: TweakableParser<Magnum::Math::Rad<Magnum::Float>> {};
#endif

/**
@tweakableliteral{Magnum::Math::Rad}

Parses the @link Magnum::Math::Literals::AngleLiterals::operator""_rad @endlink
literal.
@experimental
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Rad<Magnum::Double>> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Rad<Magnum::Double>> parse(Containers::StringView value);
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Needed to parse e.g. -1.0_rad, which is the base class type */
template<> struct TweakableParser<Magnum::Math::Unit<Magnum::Math::Rad, Magnum::Double>>: TweakableParser<Magnum::Math::Rad<Magnum::Double>> {};
#endif

}}
#endif

#endif
