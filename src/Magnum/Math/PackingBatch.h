#ifndef Magnum_Math_PackingBatch_h
#define Magnum_Math_PackingBatch_h
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
 * @brief Functions @ref Magnum::Math::packInto(), @ref Magnum::Math::unpackInto(), @ref Magnum::Math::packHalfInto(), @ref Magnum::Math::unpackHalfInto(), @ref Magnum::Math::castInto()
 * @m_since{2020,06}
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Types.h"
#include "Magnum/visibility.h"

namespace Magnum { namespace Math {

/**
@{ @name Batch packing functions

These functions process an ubounded range of values, as opposed to single
vectors or scalars.
*/

/**
@brief Unpack unsigned integral values into a floating-point representation
@param[in]  src     Source integral values
@param[out] dst     Destination floating-point values
@m_since{2020,06}

Converts integral values from full range of given *unsigned* integral type to
floating-point values in range @f$ [0, 1] @f$. Second dimension is meant to
contain vector/matrix components, or have a size of 1 for scalars. Expects that
@p src and @p dst have the same size and that the second dimension in both is
contiguous.
@see @ref packInto(), @ref castInto(),
    @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void unpackInto(const Corrade::Containers::StridedArrayView2D<const UnsignedByte>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void unpackInto(const Corrade::Containers::StridedArrayView2D<const UnsignedShort>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
@brief Unpack signed integral values into a floating-point representation
@param[in]  src     Source integral values
@param[out] dst     Destination floating-point values
@m_since{2020,06}

Converts integral values from full range of given *signed* integral type to
floating-point values in range @f$ [-1, 1] @f$. Second dimension is meant to
contain vector/matrix components, or have a size of 1 for scalars. Expects that
@p src and @p dst have the same size and that the second dimension in both is
contiguous.
@see @ref packInto(), @ref castInto(),
    @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void unpackInto(const Corrade::Containers::StridedArrayView2D<const Byte>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void unpackInto(const Corrade::Containers::StridedArrayView2D<const Short>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
@brief Pack floating-point values into an integer representation
@param[in]  src     Source floating-point values
@param[out] dst     Destination integral values
@m_since{2020,06}

Converts floating-point value in range @f$ [0, 1] @f$ to full range of
given *unsigned* integral type or range @f$ [-1, 1] @f$ to full range of
given *signed* integral type. Second dimension is meant to contain
vector/matrix components, or have a size of 1 for scalars. Expects that @p src
and @p dst have the same size and that the second dimension in both is
contiguous.

@attention Conversion result for floating-point numbers outside the normalized
    range is undefined.

@see @ref unpackInto(), @ref castInto(),
    @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void packInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedByte>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void packInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<Byte>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void packInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedShort>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void packInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<Short>& dst);

/**
@brief Pack 32-bit float values into 16-bit half-float representation
@param[in]  src     Source 32-bit float values
@param[out] dst     Destination 16-bit half-float values
@m_since{2020,06}

See [Wikipedia](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
for more information about half floats. Unlike @ref packHalf() this function is
a faster table-based implementation at the expense of using more memory, thus
more suitable for batch conversions of large data amounts. Expects that @p src
and @p dst have the same size and that the second dimension in both is
contiguous.

Algorithm used: *Jeroen van der Zijp -- Fast Half Float Conversions, 2008,
ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf*
@see @ref Half
*/
MAGNUM_EXPORT void packHalfInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedShort>& dst);

/**
@brief Unpack a range of 16-bit half-float values into 32-bit float representation
@param[in]  src     Source floating-point values
@param[out] dst     Destination integral values
@m_since{2020,06}

See [Wikipedia](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
for more information about half floats. Unlike @ref unpackHalf() this function
is a faster table-based implementation at the expense of using more memory,
thus more suitable for batch conversions of large data amounts. Expects that
@p src and @p dst have the same size and that the second dimension in both is
contiguous.

Algorithm used: *Jeroen van der Zijp -- Fast Half Float Conversions, 2008,
ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf*
@see @ref Half
*/
MAGNUM_EXPORT void unpackHalfInto(const Corrade::Containers::StridedArrayView2D<const UnsignedShort>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
@brief Cast integer values into a floating-point representation
@param[in]  src     Source integral values
@param[out] dst     Destination floating-point values
@m_since{2020,06}

Unlike @ref packInto(), this function performs only an equivalent of
@cpp Float(a) @ce over the range, so e.g. @cpp 135 @ce becomes @cpp 135.0f @ce.
Second dimension is meant to contain vector/matrix components, or have a size
of 1 for scalars. Expects that @p src and @p dst have the same size and that
the second dimension in both is contiguous.

@attention Numbers with more than 23 bits of precision will not be represented
    accurately when cast into a @ref Magnum::Float "Float".

@see @ref castInto(const Corrade::Containers::StridedArrayView2D<const Float>&, const Corrade::Containers::StridedArrayView2D<UnsignedByte>&),
    @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedByte>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Byte>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedShort>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Short>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedInt>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Int>& src, const Corrade::Containers::StridedArrayView2D<Float>& dst);

/**
@brief Cast floating-point values into an integer representation
@param[in]  src     Source floating-point values
@param[out] dst     Destination integral values
@m_since{2020,06}

Unlike @ref packInto(), this function performs only an equivalent of
@cpp Float(a) @ce over the range, so e.g. @cpp 135 @ce becomes @cpp 135.0f @ce.
Second dimension is meant to contain vector/matrix components, or have a size
of 1 for scalars. Expects that @p src and @p dst have the same size and that
the second dimension in both is contiguous.

@attention Numbers with more than 23 bits of precision will not be represented
    accurately when cast into a @ref Magnum::Float "Float".

@see @ref castInto(const Corrade::Containers::StridedArrayView2D<const Float>&, const Corrade::Containers::StridedArrayView2D<UnsignedByte>&),
    @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedByte>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<Byte>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedShort>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<Short>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<UnsignedInt>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Float>& src, const Corrade::Containers::StridedArrayView2D<Int>& dst);

/**
@brief Cast integer values into a differently sized type
@param[in]  src     Source values
@param[out] dst     Destination values
@m_since{2020,06}

Second dimension is meant to contain vector/matrix components, or have a size
of 1 for scalars. Expects that @p src and @p dst have the same size and that
the second dimension in both is contiguous.

@attention Values that don't fit into the resulting type will have undefined
    values.

@see @ref Corrade::Containers::StridedArrayView::isContiguous()
*/
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedByte>& src, const Corrade::Containers::StridedArrayView2D<UnsignedInt>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Byte>& src, const Corrade::Containers::StridedArrayView2D<Int>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedShort>& src, const Corrade::Containers::StridedArrayView2D<UnsignedInt>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Short>& src, const Corrade::Containers::StridedArrayView2D<Int>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedInt>& src, const Corrade::Containers::StridedArrayView2D<UnsignedShort>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Int>& src, const Corrade::Containers::StridedArrayView2D<Short>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const UnsignedInt>& src, const Corrade::Containers::StridedArrayView2D<UnsignedByte>& dst);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_EXPORT void castInto(const Corrade::Containers::StridedArrayView2D<const Int>& src, const Corrade::Containers::StridedArrayView2D<Byte>& dst);

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

}}

#endif
