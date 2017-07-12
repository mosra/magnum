#ifndef Magnum_Math_Packing_h
#define Magnum_Math_Packing_h
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
 * @brief Functions @ref Magnum::Math::pack(), @ref Magnum::Math::unpack(), @ref Magnum::Math::packHalf(), @ref Magnum::Math::unpackHalf()
 */

#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math {

namespace Implementation {

template<class T, UnsignedInt bits = sizeof(T)*8> inline constexpr T bitMax() {
    return T(typename std::make_unsigned<T>::type(~T{}) >> (sizeof(T)*8 - (std::is_signed<T>::value ? bits - 1 : bits)));
}

}

#ifdef DOXYGEN_GENERATING_OUTPUT
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
template<class FloatingPoint, class Integral> inline FloatingPoint unpack(const Integral& value);

/**
@brief Unpack integer bits into a floating-point representation

Alternative to the above with ability to specify how many bits of the integral
representation to use. Example usage:
@code
Float a = Math::unpack<Float, UnsignedShort>(8191);     // 0.124987f
Float b = Math::unpack<Float, UnsignedShort, 14>(8191); // 0.499969f
Float b = Math::unpack<Float, 14>(8191u);               // 0.499969f
Float b = Math::unpack<Float, 14>(8191);                // 1.0f
@endcode
*/
template<class FloatingPoint, class Integral, UnsignedInt bits> inline FloatingPoint unpack(const Integral& value);
#else
template<class FloatingPoint, class Integral, UnsignedInt bits = sizeof(Integral)*8> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_unsigned<Integral>::value, FloatingPoint>::type unpack(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
        "unpacking must be done from integral to floating-point type");
    static_assert(bits <= sizeof(Integral)*8,
        "bit count larger than size of the integral type");
    return value/FloatingPoint(Implementation::bitMax<Integral, bits>());
}
template<class FloatingPoint, class Integral, UnsignedInt bits = sizeof(Integral)*8> inline typename std::enable_if<std::is_arithmetic<Integral>::value && std::is_signed<Integral>::value, FloatingPoint>::type unpack(Integral value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
        "unpacking must be done from integral to floating-point type");
    static_assert(bits <= sizeof(Integral)*8,
        "bit count larger than size of the integral type");
    /* According to https://www.opengl.org/registry/specs/EXT/texture_snorm.txt */
    return Math::max(value/FloatingPoint(Implementation::bitMax<Integral, bits>()), FloatingPoint(-1.0));
}
template<class FloatingPoint, std::size_t size, class Integral, UnsignedInt bits = sizeof(Integral)*8> FloatingPoint unpack(const Vector<size, Integral>& value) {
    static_assert(FloatingPoint::Size == size,
        "return vector type should have the same size as input vector type");
    FloatingPoint out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = unpack<typename FloatingPoint::Type, Integral, bits>(value[i]);
    return out;
}
#endif

/** @overload */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class FloatingPoint, UnsignedInt bits, class Integral> inline FloatingPoint unpack(const Integral& value);
#else
template<class FloatingPoint, UnsignedInt bits, class Integral> inline typename std::enable_if<std::is_arithmetic<Integral>::value, FloatingPoint>::type unpack(const Integral& value) {
    return unpack<FloatingPoint, Integral, bits>(value);
}
template<class FloatingPoint, UnsignedInt bits, std::size_t size, class Integral> inline FloatingPoint unpack(const Vector<size, Integral>& value) {
    return unpack<FloatingPoint, size, Integral, bits>(value);
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
template<class Integral, class FloatingPoint, UnsignedInt bits = sizeof(Integral)*8> inline typename std::enable_if<std::is_arithmetic<FloatingPoint>::value, Integral>::type pack(FloatingPoint value) {
    static_assert(std::is_floating_point<FloatingPoint>::value && std::is_integral<Integral>::value,
        "packing must be done from floating-point to integral type");
    static_assert(bits <= sizeof(Integral)*8,
        "bit count larger than size of the integral type");
    return Integral(value*Implementation::bitMax<Integral, bits>());
}
template<class Integral, std::size_t size, class FloatingPoint, UnsignedInt bits = sizeof(typename Integral::Type)*8> Integral pack(const Vector<size, FloatingPoint>& value) {
    static_assert(Integral::Size == size,
        "return vector type should have the same size as input vector type");
    Integral out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = pack<typename Integral::Type, FloatingPoint, bits>(value[i]);
    return out;
}
#endif

/**
@brief Pack floating-point value into integer bits

Alternative to the above with ability to specify how many bits of the integral
representation to use. Example usage:
@code
auto a = Math::pack<UnsignedShort>(0.5f);     // 32767
auto b = Math::pack<UnsignedShort, 14>(0.5f); // 8191
@endcode
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class Integral, UnsignedInt bits, class FloatingPoint> inline Integral pack(FloatingPoint value);
#else
template<class Integral, UnsignedInt bits, class FloatingPoint> inline typename std::enable_if<std::is_arithmetic<FloatingPoint>::value, Integral>::type pack(FloatingPoint value) {
    return pack<Integral, FloatingPoint, bits>(value);
}
template<class Integral, UnsignedInt bits, std::size_t size, class FloatingPoint> inline Integral pack(const Vector<size, FloatingPoint>& value) {
    return pack<Integral, size, FloatingPoint, bits>(value);
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

/**
@brief Pack 32-bit float value into 16-bit half-float representation

See [Wikipedia](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
for more information about half floats. NaNs are converted to NaNs and
infinities to infinities, though their exact bit pattern is not preserved. Note
that rounding mode is unspecified in order to save some cycles.

Implementation based on CC0 / public domain code by *Fabian Giesen*,
https://fgiesen.wordpress.com/2012/03/28/half-to-float-done-quic/ .
@see @ref unpackHalf(), @ref Half
*/
MAGNUM_EXPORT UnsignedShort packHalf(Float value);

/** @overload */
template<std::size_t size> Vector<size, UnsignedShort> packHalf(const Vector<size, Float>& value) {
    Vector<size, UnsignedShort> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = packHalf(value[i]);
    return out;
}

/**
@brief Unpack 16-bit half-float value into 32-bit float representation

See [Wikipedia](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
for more information about half floats. NaNs are converted to NaNs and
infinities to infinities, though their exact bit pattern is not preserved.

Implementation based on CC0 / public domain code by *Fabian Giesen*,
https://fgiesen.wordpress.com/2012/03/28/half-to-float-done-quic/ .
@see @ref packHalf()
*/
MAGNUM_EXPORT Float unpackHalf(UnsignedShort value);

/** @overload */
template<std::size_t size> Vector<size, Float> unpackHalf(const Vector<size, UnsignedShort>& value) {
    Vector<size, Float> out{NoInit};
    for(std::size_t i = 0; i != size; ++i)
        out[i] = unpackHalf(value[i]);
    return out;
}

}}

#endif
