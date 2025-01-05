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

#include "ColorBatch.h"

#include <cstring>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Math {

namespace {

inline void yFlipBc1BlockInPlace(char* const data) {
    /* The 64-bit block is laid out as follows:

        - 2 bytes for first endpoint color
        - 2 bytes for second endpoint color
        - 4 bytes for 4x4 2-bit color indices in this order:

            a b c d
            e f g h
            i j k l
            m n o p

       Which means each row is one byte, so the Y-flip reduces down to a simple
       byte swap. See the official specification for details:
       https://learn.microsoft.com/cs-cz/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc1 */
    {
        char tmp = data[4];
        data[4] = data[7];
        data[7] = tmp;
    } {
        char tmp = data[5];
        data[5] = data[6];
        data[6] = tmp;
    }
}

inline void yFlipBc2BlockInPlace(char* data) {
    /* The 128-bit block is laid out as follows:

       - 8 bytes for 4x4 4-bit alpha values, same order as BC1
       - 2 bytes for first endpoint color
       - 2 bytes for second endpoint color
       - 4 bytes for 4x4 2-bit color indices, same order as BC1

       Which means, swapping the alphas and then doing the same as with BC1
       for the color indices in the second half.
       https://learn.microsoft.com/cs-cz/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc2 */
    {
        char tmp1 = data[0];
        char tmp2 = data[1];
        data[0] = data[6];
        data[1] = data[7];
        data[6] = tmp1;
        data[7] = tmp2;
    } {
        char tmp1 = data[2];
        char tmp2 = data[3];
        data[2] = data[4];
        data[3] = data[5];
        data[4] = tmp1;
        data[5] = tmp2;
    }

    yFlipBc1BlockInPlace(data + 8);
}

inline void yFlipBc4BlockInPlace(char* data) {
    /* The 64-bit block is laid out as follows:

       - 1 byte for first endpoint color channel
       - 1 byte for second endpoint color channel
       - 6 bytes for 4x4 3-bit color indices + interpolation factors, same
         order as BC1

       Compared to BC1, this means swapping groups of 12 bits instead of 8.
       https://learn.microsoft.com/cs-cz/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc4 */

    /* Load & byte-swap last 6 bytes */
    union {
        char bytes[8];
        UnsignedLong value;
    } block;
    for(std::size_t i = 0; i != 6; ++i) {
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        block.bytes[i] = data[i + 2];
        #else
        block.bytes[i] = data[7 - i];
        #endif
    }

    /* Flip the 12-bit groups */
    block.value =
        (block.value & 0xfff000000000ull) >> 36 |
        (block.value & 0x000fff000000ull) >> 12 |
        (block.value & 0x000000fff000ull) << 12 |
        (block.value & 0x000000000fffull) << 36;

    /* Byte-swap & store the 6 bytes back */
    for(std::size_t i = 0; i != 6; ++i) {
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        data[i + 2] = block.bytes[i];
        #else
        data[7 - i] = block.bytes[i];
        #endif
    }
}

inline void yFlipBc3BlockInPlace(char* const data) {
    yFlipBc4BlockInPlace(data);
    yFlipBc1BlockInPlace(data + 8);
}

inline void yFlipBc5BlockInPlace(char* const data) {
    yFlipBc4BlockInPlace(data);
    yFlipBc4BlockInPlace(data + 8);
}

template<std::size_t blockSize, void(*flipBlock)(char*)> void yFlipBlocksInPlace(const Containers::StridedArrayView4D<char>& blocks
    #ifndef CORRADE_NO_ASSERT
    , const char* messagePrefix
    #endif
) {
    const std::size_t* const size = blocks.size().begin();
    CORRADE_ASSERT(size[3] == blockSize,
        messagePrefix << "expected last dimension to be" << blockSize << "bytes but got" << size[3], );
    CORRADE_ASSERT(blocks.isContiguous<3>(),
        messagePrefix << "last dimension is not contiguous", );

    /* The high-level logic is mostly a copy of Utility::flipInPlace() without
       the "leftovers" part. It's however not calling that function directly
       because it'd mean going through memory twice, once for copying whole
       blocks and once for recalculating each block. */

    CORRADE_INTERNAL_ASSERT(blocks.template isContiguous<3>());

    char* const ptr = static_cast<char*>(blocks.data());
    const std::ptrdiff_t* const stride = blocks.stride().begin();
    for(std::size_t z = 0; z != size[0]; ++z) {
        char* const ptrZ = ptr + z*stride[0];

        /* Go through half of the items in Y and swap them with the other
           half, flipping their contents along the way */
        for(std::size_t y = 0, yMax = size[1]/2; y != yMax; ++y) {
            char* const ptrYTop = ptrZ + y*stride[1];
            char* const ptrYBottom = ptrZ + (size[1] - y - 1)*stride[1];

            /* Copy a block at a time and flip its contents as well */
            alignas(blockSize) char tmp[blockSize];
            for(std::size_t x = 0; x != size[2]; ++x) {
                char* const ptrXTop = ptrYTop + x*stride[2];
                char* const ptrXBottom = ptrYBottom + x*stride[2];
                flipBlock(ptrXTop);
                flipBlock(ptrXBottom);
                std::memcpy(tmp, ptrXTop, blockSize);
                std::memcpy(ptrXTop, ptrXBottom, blockSize);
                std::memcpy(ptrXBottom, tmp, blockSize);
            }
        }

        /* If there was an odd number of rows, make sure to flip contents of
           the middle row as well */
        if(size[1] % 2) {
            char* const ptrYMid = ptrZ + (size[1]/2)*stride[1];
            for(std::size_t x = 0; x != size[2]; ++x)
                flipBlock(ptrYMid + x*stride[2]);
        }
    }
}

}

void yFlipBc1InPlace(const Containers::StridedArrayView4D<char>& blocks) {
    yFlipBlocksInPlace<8, yFlipBc1BlockInPlace>(blocks
        #ifndef CORRADE_NO_ASSERT
        , "Math::yFlipBc1InPlace():"
        #endif
    );
}

void yFlipBc2InPlace(const Containers::StridedArrayView4D<char>& blocks) {
    yFlipBlocksInPlace<16, yFlipBc2BlockInPlace>(blocks
        #ifndef CORRADE_NO_ASSERT
        , "Math::yFlipBc2InPlace():"
        #endif
    );
}

void yFlipBc3InPlace(const Containers::StridedArrayView4D<char>& blocks) {
    yFlipBlocksInPlace<16, yFlipBc3BlockInPlace>(blocks
        #ifndef CORRADE_NO_ASSERT
        , "Math::yFlipBc3InPlace():"
        #endif
    );
}

void yFlipBc4InPlace(const Containers::StridedArrayView4D<char>& blocks) {
    yFlipBlocksInPlace<8, yFlipBc4BlockInPlace>(blocks
        #ifndef CORRADE_NO_ASSERT
        , "Math::yFlipBc4InPlace():"
        #endif
    );
}

void yFlipBc5InPlace(const Containers::StridedArrayView4D<char>& blocks) {
    yFlipBlocksInPlace<16, yFlipBc5BlockInPlace>(blocks
        #ifndef CORRADE_NO_ASSERT
        , "Math::yFlipBc5InPlace():"
        #endif
    );
}

}}
