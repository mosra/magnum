#ifndef Magnum_Math_Time_h
#define Magnum_Math_Time_h
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
 * @brief Class @ref Magnum::Math::Nanoseconds, @ref Magnum::Math::Seconds, literal @link Magnum::Math::Literals::TimeLiterals::operator""_nsec() @endlink, @link Magnum::Math::Literals::TimeLiterals::operator""_usec() @endlink, @link Magnum::Math::Literals::TimeLiterals::operator""_msec() @endlink, @link Magnum::Math::Literals::TimeLiterals::operator""_sec() @endlink
 * @m_since_latest
 */

#ifndef CORRADE_SINGLES_NO_DEBUG
#include <Corrade/Utility/Utility.h>
#endif

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"
#include "Magnum/Math/Unit.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<class, class> struct NanosecondsConverter;
    template<class, class> struct SecondsConverter;
}

/**
@brief Nanoseconds
@m_since_latest

Along with @ref Seconds provides convenience classes to make time specification
and conversion less error-prone. As there's little need to represent fractions
of nanoseconds, the @ref Magnum::Nanoseconds typedef uses a 64-bit signed
integer, which covers a span of ±292 years. In scenarios where nanosecond
precision or a large range isn't needed, the @ref Magnum::Seconds typedef,
which is a 32-bit floating-point type, may be sufficient.

@section Math-Nanoseconds-usage Usage

You can create the value by using one of the time literals. For all of them the
result type is @ref Nanoseconds for preserving maximum precision, but you can
directly convert the literal value @link Seconds @endlink:

@snippet Math.cpp Nanoseconds-usage

Or by explicitly converting a unitless value (such as an output from some
function) to either nanoseconds or seconds. And same can be done in the other direction:

@snippet Math-stl.cpp Nanoseconds-usage-convert

The classes support all arithmetic operations, such as addition, subtraction
or multiplication/division by a unitless number:

@snippet Math.cpp Nanoseconds-usage-operations

It is also possible to compare time values with all comparison operators. As
the literals are all producing @ref Nanoseconds, it's most convenient to
compare to nanosecond values. Comparison of @ref Nanoseconds and @ref Seconds
is not possible without conversion to a common type first.

@snippet Math.cpp Nanoseconds-usage-comparison

@section Math-Nanoseconds-stl STL compatibility

Instances of @ref Nanoseconds are explicitly convertible from and to
@ref std::chrono::duration and @ref std::chrono::time_point types if you
include @ref Magnum/Math/TimeStl.h. The conversion is provided in a separate
header to avoid unconditional @cpp #include <chrono> @ce, which can
significantly affect compile times. The following table lists allowed
conversions, conversions in certain directions aren't allowed as they cause a
precision loss:

Magnum type      | ↭ | STL type
-----------------| - | ---------------------
@ref Nanoseconds | ⇆ | @ref std::chrono::nanoseconds
@ref Nanoseconds | ← | @ref std::chrono::microseconds
@ref Nanoseconds | ← | @ref std::chrono::milliseconds
@ref Nanoseconds | ← | @ref std::chrono::seconds
@ref Nanoseconds | ← | @ref std::chrono::minutes
@ref Nanoseconds | ← | @ref std::chrono::hours
@ref Nanoseconds | ← | @ref std::chrono::duration
@ref Nanoseconds | ⇆ | @ref std::chrono::duration "std::chrono::duration<Rep, std::nano>"
@ref Nanoseconds | ← | @ref std::chrono::time_point
@ref Nanoseconds | ⇆ | @ref std::chrono::time_point "std::chrono::time_point<Clock, std::chrono::duration<Rep, std::nano>>"

Example:

@snippet Math-stl.cpp Nanoseconds-usage

<b></b>

@m_class{m-block m-warning}

@par Conversion from and to std::time_t
    Even though @ref std::time_t may look like an implementation-defined strong
    type, it's actually just an alias to an integer type, which in turn means
    it's not possible to provide safe conversion for it. Thus a simple
    conversion, while it may compile, won't do the right thing:
@par
    @snippet Math-stl.cpp Nanoseconds-usage-time

@see @link Literals::TimeLiterals::operator""_nsec() @endlink,
    @link Literals::TimeLiterals::operator""_usec() @endlink,
    @link Literals::TimeLiterals::operator""_msec() @endlink,
    @link Literals::TimeLiterals::operator""_sec() @endlink
*/
template<class T> class Nanoseconds: public Unit<Nanoseconds, T> {
    public:
        /**
         * @brief Minimal representable value
         *
         * Returns @cpp -0x8000000000000000_nsec @ce.
         */
        constexpr static Nanoseconds<T> min();

        /**
         * @brief Maximal representable value
         *
         * Returns @cpp 0x7fffffffffffffff_nsec @ce.
         */
        constexpr static Nanoseconds<T> max();

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Nanoseconds(ZeroInitT).
         */
        /* Needs to be Math::Nanoseconds here and in all other places because
           older Clang and both MSVC 2015 and 2017 treat it as a template
           instance Nanoseconds<T> instead of a Nanoseconds template */
        constexpr /*implicit*/ Nanoseconds() noexcept: Unit<Math::Nanoseconds, T>{ZeroInit} {}

        /** @brief Construct a zero time */
        constexpr explicit Nanoseconds(ZeroInitT) noexcept: Unit<Math::Nanoseconds, T>{ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit Nanoseconds(Magnum::NoInitT) noexcept: Unit<Math::Nanoseconds, T>{Magnum::NoInit} {}

        /** @brief Explicit constructor from a unitless type */
        constexpr explicit Nanoseconds(T value) noexcept: Unit<Math::Nanoseconds, T>{value} {}

        /** @brief Copy constructor */
        /* Needed in order to make arithmetic operations (which have a Unit
           return type) convertible to Nanoseconds */
        constexpr /*implicit*/ Nanoseconds(Unit<Math::Nanoseconds, T> other) noexcept: Unit<Math::Nanoseconds, T>(other) {}

        /**
         * @brief Construct nanoseconds from seconds
         *
         * The floating-point value is multiplied by a billion and rounded.
         */
        template<class U> constexpr /*implicit*/ Nanoseconds(Unit<Seconds, U> value) noexcept;

        /** @brief Construct nanoseconds from external representation */
        template<class U, class V = decltype(Implementation::NanosecondsConverter<T, U>::from(std::declval<U>()))> constexpr explicit Nanoseconds(const U& other) noexcept: Nanoseconds{Implementation::NanosecondsConverter<T, U>::from(other)} {}

        /** @brief Convert nanoseconds to external representation */
        template<class U, class V = decltype(Implementation::NanosecondsConverter<T, U>::to(std::declval<Nanoseconds<T>>()))> constexpr explicit operator U() const {
            return Implementation::NanosecondsConverter<T, U>::to(*this);
        }
};

/* Doxygen can't match these to the class, meh */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<> constexpr Nanoseconds<Long> Nanoseconds<Long>::min() {
    /* There's apparently no way to say -0x8000000000000000ll. C++, LOL. */
    return Nanoseconds<Long>{Long(0x8000000000000000ull)};
}
template<> constexpr Nanoseconds<Long> Nanoseconds<Long>::max() {
    return Nanoseconds<Long>{0x7fffffffffffffffll};
}
#endif

/**
@brief Seconds
@m_since_latest

Represents a floating-point second value. Compared to @ref Nanoseconds, the
@ref Magnum::Seconds typedef uses a 32-bit float which offers a
microsecond-level precision and a reasonable range for scenarios where storing
a full 64-bit nanosecond value isn't needed. See @ref Nanoseconds for more
information and usage examples.
*/
template<class T> class Seconds: public Unit<Seconds, T> {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Seconds(ZeroInitT).
         */
        /* Needs to be Math::Seconds here and in all other places because
           older Clang and both MSVC 2015 and 2017 treat it as a template
           instance Seconds<T> instead of a Seconds template */
        constexpr /*implicit*/ Seconds() noexcept: Unit<Math::Seconds, T>{ZeroInit} {}

        /** @brief Construct a zero time */
        constexpr explicit Seconds(ZeroInitT) noexcept: Unit<Math::Seconds, T>{ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit Seconds(Magnum::NoInitT) noexcept: Unit<Math::Seconds, T>{Magnum::NoInit} {}

        /** @brief Explicit constructor from a unitless type */
        constexpr explicit Seconds(T value) noexcept: Unit<Math::Seconds, T>{value} {}

        /** @brief Copy constructor */
        /* Needed in order to make arithmetic operations (which have a Unit
           return type) convertible to Seconds */
        constexpr /*implicit*/ Seconds(Unit<Math::Seconds, T> other) noexcept: Unit<Math::Seconds, T>(other) {}

        /**
         * @brief Construct seconds from nanoseconds
         *
         * A floating-point value can accurately only represent microseconds
         * and only in a limited range, so the conversion may result in some
         * precision loss.
         */
        template<class U> constexpr /*implicit*/ Seconds(Unit<Nanoseconds, U> value) noexcept;

        /** @brief Construct seconds from external representation */
        template<class U, class V = decltype(Implementation::SecondsConverter<T, U>::from(std::declval<U>()))> constexpr explicit Seconds(const U& other) noexcept: Seconds{Implementation::SecondsConverter<T, U>::from(other)} {}

        /** @brief Convert seconds to external representation */
        template<class U, class V = decltype(Implementation::SecondsConverter<T, U>::to(std::declval<Seconds<T>>()))> constexpr explicit operator U() const {
            return Implementation::SecondsConverter<T, U>::to(*this);
        }
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
    namespace TimeLiterals {

/* Note on literal naming: while the STL in C++14 uses `ns`, `us`, `s` and
   https://en.cppreference.com/w/cpp/chrono/operator%22%22s claims that having
   `s` for both a string literal, taking const char*, and a second literal,
   taking long double, "just works", at least on GCC and Clang it's only true
   if both are defined next to each other IN THE SAME NAMESPACE. Which
   completely breaks any library composability or encapsulation, as it means I
   can not have a StringView _s literal defined in Corrade::Containers and a
   Nanosecond _s literal defined in Magnum::Math. The only workaround I found
   was to do something along the lines of the following, which was NOT NICE at
   all (and no, `using namespace Containers::Literals` wasn't enough).

    namespace Magnum { namespace Math { namespace Literals {
        using Containers::Literals::operator""_s;
    }}}

   Thus I'm choosing a different name to prevent the conflict from even
   happening. On the other hand, that's probably a better solution even without
   the above design issue in the C++ spec, because it prevents potential
   conflicts with _s / _us being eventually used for Short and UnsignedShort
   literals, or _h conflicting between half-float and hour literals (now it'd
   be _hr). Even C++14 picked `min` for minutes instead of `m` because it
   apparently seemed to become problematic once/if distance literals for meters
   and such get introduced. So why not go with the more clear name for
   everything already. Seeing 15.0_sec in unfamiliar code doesn't feel
   ambiguous, seeing 127_s or 0.5_h definitely does. */

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
/** @relatesalso Magnum::Math::Nanoseconds
@brief Nanosecond value literal
@m_since_latest

Compared to the microsecond, millisecond and second literals, this literal is
an integer value and not a floating-point, as it's not possible to represent
fractions of nanoseconds. Usage example:

@snippet Math.cpp _nsec

@see @link operator""_usec() @endlink, @link operator""_msec() @endlink,
    @link operator""_sec() @endlink
@m_keywords{_nsec nsec}
*/
constexpr Nanoseconds<Long> operator"" _nsec(unsigned long long value) {
    return Nanoseconds<Long>{Long(value)};
}

/** @relatesalso Magnum::Math::Nanoseconds
@brief Microsecond value literal
@m_since_latest

As the value is converted to whole nanoseconds, everything after thousandths is
truncated. Additionally, up to thousandths the conversion is without precision
loss only on systems with a 80-bit @cpp long double @ce (which has a 63-bit
mantissa). If you need to ensure nanosecond-level precision on systems that
have a 64-bit @cpp long double @ce, use @link operator""_nsec() @endlink
instead. On the other hand, if nanosecond-level precision isn't needed, it's
possible to convert directly to @ref Seconds that offer a microsecond-level
precision on a range of roughly ±8 seconds. For example:

@snippet Math.cpp _usec

@see @link operator""_msec() @endlink, @link operator""_sec() @endlink,
    @ref CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
@m_keywords{_usec usec}
*/
constexpr Nanoseconds<Long> operator"" _usec(long double value) {
    return Nanoseconds<Long>{Long(value*1000.0l)};
}

/** @relatesalso Magnum::Math::Nanoseconds
@brief Millisecond value literal
@m_since_latest

As the value is converted to whole nanoseconds, everything after millionths is
truncated. Additionally, up to millionths the conversion is without precision
loss only on systems with a 80-bit @cpp long double @ce (which has a 63-bit
mantissa). If you need to ensure nanosecond-level precision on systems that
have a 64-bit @cpp long double @ce, use @link operator""_nsec() @endlink
instead. On the other hand, if nanosecond-level precision isn't needed, it's
possible to convert directly to @ref Seconds that offer a millisecond-level
precision on a range of roughly ±2 hours. For example:

@snippet Math.cpp _msec

@see @link operator""_usec() @endlink, @link operator""_sec() @endlink,
    @ref CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
@m_keywords{_msec msec}
*/
constexpr Nanoseconds<Long> operator"" _msec(long double value) {
    return Nanoseconds<Long>{Long(value*1000000.0l)};
}

/** @relatesalso Magnum::Math::Nanoseconds
@brief Second value literal
@m_since_latest

As the value is converted to whole nanoseconds, everything after billionths is
truncated. Additionally, up to billionths the conversion is without precision
loss only on systems with a 80-bit @cpp long double @ce (which has a 63-bit
mantissa). If you need to ensure nanosecond-level precision on systems that
have a 64-bit @cpp long double @ce, use @link operator""_nsec() @endlink
instead. On the other hand, if nanosecond-level precision isn't needed, it's
possible to convert directly to @ref Seconds that offer a millisecond-level
precision on a range of roughly ±2 hours. For example:

@snippet Math.cpp _sec

@see @link operator""_usec() @endlink, @link operator""_msec() @endlink,
    @ref CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
@m_keywords{_sec sec}
*/
constexpr Nanoseconds<Long> operator"" _sec(long double value) {
    return Nanoseconds<Long>{Long(value*1000000000.0l)};
}
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic pop
#endif

}}

template<class T> template<class U> constexpr Nanoseconds<T>::Nanoseconds(Unit<Seconds, U> value) noexcept: Unit<Math::Nanoseconds, T>{T(static_cast<long double>(U(value))*1000000000.0l)} {}

template<class T> template<class U> constexpr Seconds<T>::Seconds(Unit<Nanoseconds, U> value) noexcept: Unit<Math::Seconds, T>{T(static_cast<long double>(U(value))/1000000000.0l)} {}

#ifndef CORRADE_SINGLES_NO_DEBUG
/**
 * @debugoperator{Nanoseconds}
 * @m_since_latest
 */
MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug& debug, const Unit<Nanoseconds, Long>& value);

/**
 * @debugoperator{Seconds}
 * @m_since_latest
 */
MAGNUM_EXPORT Utility::Debug& operator<<(Utility::Debug& debug, const Unit<Seconds, Float>& value);
#endif

}}

#endif
