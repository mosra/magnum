#ifndef Magnum_Math_Half_h
#define Magnum_Math_Half_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::Half, literal @link Magnum::Math::Literals::operator""_h() @endlink
 */

#include <utility>
#ifndef CORRADE_NO_DEBUG
#include <Corrade/Utility/Utility.h>
#endif

#include "Magnum/visibility.h"
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

The purpose of this class is just to make specifying and printing of half-float
literals easier. By design no arithmetic operations are supported, as majority
of CPUs has no dedicated instructions for half-precision floats and thus it is
faster to just use regular single-precision @ref Magnum::Float "Float". See
[Wikipedia](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
for more information about half floats.

The class provides explicit conversion from and to @ref Magnum::Float "Float",
equality comparison with correct treatment of NaN values, promotion and
negation operator, an @link Literals::operator""_h() operator""_h() @endlink
literal and an @ref operator<<(Debug&, Half) debug operator. Internally the
class uses @ref packHalf() and @ref unpackHalf(). Example usage:

@snippet MagnumMath.cpp Half-usage

Note that it is also possible to use this type inside @ref Vector classes,
though, again, only for passing data around and converting them, without any
arithmetic operations:

@snippet MagnumMath.cpp Half-usage-vector

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
         * NaN, otherwise does bitwise comparison.
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

namespace Literals {

/** @relatesalso Magnum::Math::Half
@brief Half-float literal

See @ref Half for more information.
*/
inline Half operator "" _h(long double value) { return Half(Float(value)); }

}

#ifndef CORRADE_NO_DEBUG
/**
@debugoperator{Half}

Prints the value with 4 significant digits.
@see @ref Corrade::Utility::Debug::operator<<(float),
    @ref Corrade::Utility::Debug::operator<<(double),
    @ref Corrade::Utility::Debug::operator<<(long double value)
@todoc remove `long double value` once doxygen can link to long double overloads properly
*/
MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, Half value);
#endif

namespace Implementation {

template<> struct StrictWeakOrdering<Half> {
    bool operator()(Half a, Half b) const {
        /* Not mathematically equivalent to <, but does order */
        return a.data() < b.data();
    }
};

}

}}

#if !defined(CORRADE_NO_TWEAKABLE) && (defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN))
namespace Corrade { namespace Utility {

/**
@tweakableliteral{Magnum::Math::Half}

Parses the @link Magnum::Math::Literals::operator""_h @endlink literal.
*/
template<> struct MAGNUM_EXPORT TweakableParser<Magnum::Math::Half> {
    TweakableParser() = delete;

    /** @brief Parse the value */
    static std::pair<TweakableState, Magnum::Math::Half> parse(Containers::StringView value);
};

}}
#endif

#endif
