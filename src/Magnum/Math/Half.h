#ifndef Magnum_Math_Half_h
#define Magnum_Math_Half_h
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
 * @brief Class @ref Magnum::Math::Half, literal @link Magnum::Math::Literals::HalfLiterals::operator""_h() @endlink
 */

#ifndef CORRADE_SINGLES_NO_DEBUG
#include <Corrade/Utility/Utility.h>
#endif

#include "Magnum/visibility.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Math/Tags.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* So we don't need to drag in the whole Packing.h */
MAGNUM_EXPORT UnsignedShort packHalf(Float value);
MAGNUM_EXPORT Float unpackHalf(UnsignedShort value);
#endif

/**
@brief Half-precision float literal

Represents a floating-point value in the [`binary16` format](https://en.wikipedia.org/wiki/Half-precision_floating-point_format).

The sole purpose of this type is to make creation, conversion and visualization
of half-float values easier. By design it doesn't support any arithmetic
operations as not all CPU architecture have native support for half-floats and
thus the operations would be done faster in a regular single-precision
@ref Magnum::Float "Float".

The class provides explicit conversion from and to @ref Magnum::Float "Float",
equality comparison with correct treatment of NaN values, promotion and
negation operator, a @link Literals::HalfLiterals::operator""_h() @endlink
literal and an @ref operator<<(Debug&, Half) debug operator. Internally the
class uses @ref packHalf() and @ref unpackHalf(). Example usage:

@snippet Math.cpp Half-usage

Note that it is also possible to use this type inside @ref Vector classes,
though, again, only for passing data around and converting them, without any
arithmetic operations:

@snippet Math.cpp Half-usage-vector

@see @ref Magnum::Half
*/
class Half {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Half(ZeroInitT).
         */
        constexpr /*implicit*/ Half() noexcept: _data{} {}

        /** @brief Construct a zero value */
        constexpr explicit Half(ZeroInitT) noexcept: _data{} {}

        /** @brief Construct a half value from underlying 16-bit representation */
        constexpr explicit Half(UnsignedShort data) noexcept: _data{data} {}

        /**
         * @brief Construct a half value from a 32-bit float representation
         *
         * @see @ref packHalf()
         */
        explicit Half(Float value) noexcept: _data{packHalf(value)} {}

        /**
         * @brief Construct a half value from a 64-bit float representation
         *
         * Present only to aid generic code so e.g. @cpp T(1.0) @ce works
         * without being ambigous.
         * @see @ref packHalf()
         */
        explicit Half(Double value) noexcept: _data{packHalf(Float(value))} {}

        /** @brief Construct without initializing the contents */
        explicit Half(Magnum::NoInitT) noexcept {}

        /**
         * @brief Equality comparison
         *
         * Returns `false` if one of the values is half-float representation of
         * NaN, otherwise does bitwise comparison. Note that, unlike with other
         * floating-point Magnum math types, due to the limited precision of
         * half floats it's *not* a fuzzy compare.
         */
        constexpr bool operator==(Half other) const {
            return (((      _data & 0x7c00) == 0x7c00 && (      _data & 0x03ff)) ||
                    ((other._data & 0x7c00) == 0x7c00 && (other._data & 0x03ff))) ?
                false : _data == other._data;
        }

        /**
         * @brief Non-equality comparison
         *
         * Simply negates the result of @ref operator==().
         */
        constexpr bool operator!=(Half other) const {
            return !operator==(other);
        }

        /**
         * @brief Promotion
         *
         * Returns the value as-is.
         */
        constexpr Half operator+() const { return *this; }

        /** @brief Negation */
        constexpr Half operator-() const {
            return Half{UnsignedShort(_data ^ (1 << 15))};
        }

        /**
         * @brief Conversion to underlying representation
         *
         * @see @ref data()
         */
        constexpr explicit operator UnsignedShort() const { return _data; }

        /**
         * @brief Conversion to 32-bit float representation
         *
         * @see @ref unpackHalf()
         */
        explicit operator Float() const { return unpackHalf(_data); }

        /**
         * @brief Underlying representation
         *
         * @see @ref operator UnsignedShort()
         */
        constexpr UnsignedShort data() const { return _data; }

    private:
        UnsignedShort _data;
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
    namespace HalfLiterals {

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
/** @relatesalso Magnum::Math::Half
@brief Half-float literal

See @ref Half for more information.
*/
inline Half operator"" _h(long double value) { return Half(Float(value)); }
#if defined(CORRADE_TARGET_CLANG) && __clang_major__ >= 17
#pragma clang diagnostic pop
#endif

}}

#ifndef CORRADE_SINGLES_NO_DEBUG
/**
@debugoperator{Half}

Prints the value with 4 significant digits.
@see @ref Debug::operator<<(float), @ref Debug::operator<<(double),
    @ref Debug::operator<<(long double)
*/
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Half value);
#endif

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
namespace Implementation {

template<> struct StrictWeakOrdering<Half> {
    bool operator()(Half a, Half b) const {
        /* Not mathematically equivalent to <, but does order */
        return a.data() < b.data();
    }
};

}
#endif

}}

#if !defined(CORRADE_NO_TWEAKABLE) && (defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN))
namespace Corrade { namespace Utility {

/**
@tweakableliteral{Magnum::Math::Half}

Parses the @link Magnum::Math::Literals::HalfLiterals::operator""_h @endlink literal.
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Half> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static Containers::Pair<TweakableState, Magnum::Math::Half> parse(Containers::StringView value);
};

}}
#endif

#endif
