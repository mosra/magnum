#ifndef Magnum_Math_ColorBatch_h
#define Magnum_Math_ColorBatch_h
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
 * @brief Function @ref Magnum::Math::yFlipBc1InPlace(), @ref Magnum::Math::yFlipBc3InPlace(), @ref Magnum::Math::yFlipBc4InPlace(), @ref Magnum::Math::yFlipBc5InPlace()
 * @m_since_latest
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum { namespace Math {

/**
@brief Y-flip BC1 texture blocks in-place
@m_since_latest

Performs a Y flip of given 3D image by flipping block order and modifying
internal block representation to encode the same information, just upside down.
No decoding or re-encoding of the block data is performed, thus the operation
is lossless. However note that this operation flips full blocks --- if size of
the actual image isn't whole blocks, the flipped image will be shifted compared
to the original, possibly with garbage data appearing in the first few rows.

First dimension is expected to be image slices, second block rows, third
2D blocks, fourth the 64-bit 4x4 block data, i.e. the last dimension is
expected to be contiguous with size of 8.
@see @ref CompressedPixelFormat::Bc1RGBUnorm,
    @ref CompressedPixelFormat::Bc1RGBSrgb,
    @ref CompressedPixelFormat::Bc1RGBAUnorm,
    @ref CompressedPixelFormat::Bc1RGBASrgb
*/
MAGNUM_EXPORT void yFlipBc1InPlace(const Containers::StridedArrayView4D<char>& blocks);

/**
@brief Y-flip BC2 texture blocks in-place
@m_since_latest

Performs a Y flip of given 3D image by flipping block order and modifying
internal block representation to encode the same information, just upside down.
No decoding or re-encoding of the block data is performed, thus the operation
is lossless. However note that this operation flips full blocks --- if size of
the actual image isn't whole blocks, the flipped image will be shifted compared
to the original, possibly with garbage data appearing in the first few rows.

First dimension is expected to be image slices, second block rows, third
2D blocks, fourth the 128-bit 4x4 block data, i.e. the last dimension is
expected to be contiguous with size of 16.
@see @ref CompressedPixelFormat::Bc2RGBAUnorm,
    @ref CompressedPixelFormat::Bc2RGBASrgb
*/
MAGNUM_EXPORT void yFlipBc2InPlace(const Containers::StridedArrayView4D<char>& blocks);

/**
@brief Y-flip BC3 texture blocks in-place
@m_since_latest

Performs a Y flip of given 3D image by flipping block order and modifying
internal block representation to encode the same information, just upside down.
No decoding or re-encoding of the block data is performed, thus the operation
is lossless. However note that this operation flips full blocks --- if size of
the actual image isn't whole blocks, the flipped image will be shifted compared
to the original, possibly with garbage data appearing in the first few rows.

First dimension is expected to be image slices, second block rows, third
2D blocks, fourth the 128-bit 4x4 block data, i.e. the last dimension is
expected to be contiguous with size of 16. As BC3 is internally a 64-bit BC4
block for alpha followed by a 64-bit BC1 block for RGB, the operation is the
same as performing @ref yFlipBc4InPlace() on the first half and
@ref yFlipBc1InPlace() on second half of each block.
@see @ref CompressedPixelFormat::Bc3RGBAUnorm,
    @ref CompressedPixelFormat::Bc3RGBASrgb
*/
MAGNUM_EXPORT void yFlipBc3InPlace(const Containers::StridedArrayView4D<char>& blocks);

/**
@brief Y-flip BC4 texture blocks in-place
@m_since_latest

Performs a Y flip of given 3D image by flipping block order and modifying
internal block representation to encode the same information, just upside down.
No decoding or re-encoding of the block data is performed, thus the operation
is lossless. However note that this operation flips full blocks --- if size of
the actual image isn't whole blocks, the flipped image will be shifted compared
to the original, possibly with garbage data appearing in the first few rows.

First dimension is expected to be image slices, second block rows, third
2D blocks, fourth the 64-bit 4x4 block data, i.e. the last dimension is
expected to be contiguous with size of 8.
@see @ref CompressedPixelFormat::Bc4RUnorm,
    @ref CompressedPixelFormat::Bc4RSnorm
*/
MAGNUM_EXPORT void yFlipBc4InPlace(const Containers::StridedArrayView4D<char>& blocks);

/**
@brief Y-flip BC5 texture blocks in-place
@m_since_latest

Performs a Y flip of given 3D image by flipping block order and modifying
internal block representation to encode the same information, just upside down.
No decoding or re-encoding of the block data is performed, thus the operation
is lossless. However note that this operation flips full blocks --- if size of
the actual image isn't whole blocks, the flipped image will be shifted compared
to the original, possibly with garbage data appearing in the first few rows.

First dimension is expected to be image slices, second block rows, third
2D blocks, fourth the 128-bit 4x4 block data, i.e. the last dimension is
expected to be contiguous with size of 16. As BC5 is internally two 64-bit BC4
blocks, the operation is the same as performing @ref yFlipBc4InPlace() on both
halves of each block.
@see @ref CompressedPixelFormat::Bc5RGUnorm,
    @ref CompressedPixelFormat::Bc5RGSnorm
*/
MAGNUM_EXPORT void yFlipBc5InPlace(const Containers::StridedArrayView4D<char>& blocks);

}}

#endif
