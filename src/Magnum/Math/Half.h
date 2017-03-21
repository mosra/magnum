#ifndef Magnum_Math_Half_h
#define Magnum_Math_Half_h
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
 * @brief Class @ref Magnum::Math::Half, literal @link Magnum::Math::Literals::operator""_h() @endlink
 */

#include "Magnum/Math/Packing.h"

namespace Magnum { namespace Math {

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
literal and an @ref operator<<(Debug&, Half) debug operator. Internally the class uses
@ref packHalf() and @ref unpackHalf(). Example usage:
@code
using namespace Math::Literals;

Half a = 3.14159_h;
Debug{} << a;                   // Prints 3.14159
Debug{} << Float(a);            // Prints 3.14159
Debug{} << UnsignedShort(a);    // Prints 25675
@endcode

Note that it is also possible to use this type inside @ref Vector classes,
though, again, only for passing data around and converting them, without any
arithmetic operations:
@code
Math::Vector3<Half> a{3.14159_h, -1.4142_h, 1.618_h};
Vector3 b{a};                                // converts to 32-bit floats
Debug{} << a;                                // prints {3.14159, -1.4142, 1.618}
Debug{} << Math::Vector3<UnsignedShort>{a};  // prints {16968, 48552, 15993}
@endcode

@see @ref Magnum::Half
*/
class Half {
    public:
        /**
         * @brief Default constructor
         *
         * Creates a zero value.
         */
        constexpr /*implicit*/ Half(ZeroInitT = ZeroInit) noexcept: _data{} {}

        /** @brief Construct a half value from underlying 16-bit representation */
        constexpr explicit Half(UnsignedShort data) noexcept: _data{data} {}

        /**
         * @brief Construct a half value from 32-bit float representation
         *
         * @see @ref packHalf()
         */
        explicit Half(Float value) noexcept: _data{packHalf(value)} {}

        /** @brief Construct without initializing the contents */
        explicit Half(NoInitT) noexcept {}

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

/** @debugoperator{Magnum::Math::Half} */
inline Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, Half value) {
    return debug << Float(value);
}

}}

#endif
