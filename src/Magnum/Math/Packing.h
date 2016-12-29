#ifndef Magnum_Math_Packing_h
#define Magnum_Math_Packing_h
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

#include <limits>

#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math {

/**
@brief Unpack integral value into a floating-point representation

Converts integral value from full range of given *unsigned* integral type to
value in range @f$ [0, 1] @f$ or from *signed* integral to range @f$ [-1, 1] @f$.

@note For best precision, resulting `FloatingPoint` type should be always
    larger that `Integral` type (e.g. @ref Magnum::Float "Float" from
    @ref Magnum::Short "Short", @ref Magnum::Double "Double" from
    @ref Magnum::Int "Int" and similarly for vector types).

@attention To ensure the integral type is correctly detected when using
    literals, this function should be called with both template parameters
    explicit, e.g.:
@code
// Literal type is (signed) char, but we assumed unsigned char, a != 1.0f
Float a = Math::unpack<Float>('\xFF');

// b = 1.0f
Float b = Math::unpack<Float, UnsignedByte>('\xFF');
@endcode

@see @ref pack()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class FloatingPoint, class Integral> inline FloatingPoint unpack(const Integral& value);
#else
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_unsigned<Integral>::value, FloatingPoint>::type unpack(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "unpacking must be done from integral to floating-point type");
    return value/FloatingPoint(std::numeric_limits<Integral>::max());
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_signed<Integral>::value, FloatingPoint>::type unpack(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "unpacking must be done from integral to floating-point type");
    return Math::max(value/FloatingPoint(std::numeric_limits<Integral>::max()), FloatingPoint(-1.0));
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_unsigned<typename Integral::Type>::value, FloatingPoint>::type unpack(const Integral& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "unpacking must be done from integral to floating-point type");
    return FloatingPoint(value)/typename FloatingPoint::Type(std::numeric_limits<typename Integral::Type>::max());
}
template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_signed<typename Integral::Type>::value, FloatingPoint>::type unpack(const Integral& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "unpacking must be done from integral to floating-point type");
    return Math::max(FloatingPoint(value)/typename FloatingPoint::Type(std::numeric_limits<typename Integral::Type>::max()), FloatingPoint(-1.0));
}
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/** @copybrief unpack()
 * @deprecated Use @ref unpack() instead.
 */
template<class FloatingPoint, class Integral> CORRADE_DEPRECATED("use unpack() instead") inline FloatingPoint normalize(const Integral& value) {
    return unpack<FloatingPoint, Integral>(value);
}
#endif

/**
@brief Pack floating-point value into an integer representation

Converts floating-point value in range @f$ [0, 1] @f$ to full range of given
*unsigned* integral type or range @f$ [-1, 1] @f$ to full range of given *signed*
integral type.

@note For best precision, `FloatingPoint` type should be always larger that
    resulting `Integral` type (e.g. @ref Magnum::Float "Float" to
    @ref Magnum::Short "Short", @ref Magnum::Double "Double" to @ref Magnum::Int "Int"
    and similarly for vector types).

@attention Return value for floating point numbers outside the normalized
    range is undefined.

@see @ref unpack()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Integral, class FloatingPoint> inline Integral pack(const FloatingPoint& value);
#else
template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_arithmetic<FloatingPoint>::value, Integral>::type pack(FloatingPoint value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
                  "packing must be done from floating-point to integral type");
    return Integral(value*std::numeric_limits<Integral>::max());
}
template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_arithmetic<typename Integral::Type>::value, Integral>::type pack(const FloatingPoint& value) {
    static_assert(std::is_floating_point<typename FloatingPoint::Type>::value && std::is_integral<typename Integral::Type>::value,
                  "packing must be done from floating-point to integral type");
    return Integral(value*std::numeric_limits<typename Integral::Type>::max());
}
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/** @copybrief pack()
 * @deprecated Use @ref pack() instead.
 */
template<class Integral, class FloatingPoint> CORRADE_DEPRECATED("use pack() instead") inline Integral denormalize(const FloatingPoint& value) {
    return pack<Integral, FloatingPoint>(value);
}
#endif

}}

#endif
