/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "PixelFormat.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/PixelFormat.h"

namespace Magnum { namespace Vk {

namespace {

constexpr PixelFormat PixelFormatMapping[] {
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _c(input) PixelFormat::input,
    #define _s(input) PixelFormat{},
    #include "Magnum/Vk/Implementation/pixelFormatMapping.hpp"
    #undef _s
    #undef _c
};

constexpr PixelFormat CompressedPixelFormatMapping[] {
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _c(input, format) PixelFormat::Compressed ## format,
    #define _s(input) PixelFormat{},
    #include "Magnum/Vk/Implementation/compressedPixelFormatMapping.hpp"
    #undef _s
    #undef _c
};

}

Debug& operator<<(Debug& debug, const PixelFormat value) {
    debug << "Vk::PixelFormat" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::PixelFormat::value: return debug << "::" << Debug::nospace << #value;
        _c(R8Unorm)
        _c(RG8Unorm)
        _c(RGB8Unorm)
        _c(RGBA8Unorm)
        _c(R8Snorm)
        _c(RG8Snorm)
        _c(RGB8Snorm)
        _c(RGBA8Snorm)
        _c(R8Srgb)
        _c(RG8Srgb)
        _c(RGB8Srgb)
        _c(RGBA8Srgb)
        _c(R8UI)
        _c(RG8UI)
        _c(RGB8UI)
        _c(RGBA8UI)
        _c(R8I)
        _c(RG8I)
        _c(RGB8I)
        _c(RGBA8I)
        _c(R16Unorm)
        _c(RG16Unorm)
        _c(RGB16Unorm)
        _c(RGBA16Unorm)
        _c(R16Snorm)
        _c(RG16Snorm)
        _c(RGB16Snorm)
        _c(RGBA16Snorm)
        _c(R16UI)
        _c(RG16UI)
        _c(RGB16UI)
        _c(RGBA16UI)
        _c(R16I)
        _c(RG16I)
        _c(RGB16I)
        _c(RGBA16I)
        _c(R32UI)
        _c(RG32UI)
        _c(RGB32UI)
        _c(RGBA32UI)
        _c(R32I)
        _c(RG32I)
        _c(RGB32I)
        _c(RGBA32I)
        _c(R16F)
        _c(RG16F)
        _c(RGB16F)
        _c(RGBA16F)
        _c(R32F)
        _c(RG32F)
        _c(RGB32F)
        _c(RGBA32F)
        _c(Depth16Unorm)
        _c(Depth24Unorm)
        _c(Depth32F)
        _c(Stencil8UI)
        _c(Depth16UnormStencil8UI)
        _c(Depth24UnormStencil8UI)
        _c(Depth32FStencil8UI)
        _c(CompressedBc1RGBUnorm)
        _c(CompressedBc1RGBSrgb)
        _c(CompressedBc1RGBAUnorm)
        _c(CompressedBc1RGBASrgb)
        _c(CompressedBc2RGBAUnorm)
        _c(CompressedBc2RGBASrgb)
        _c(CompressedBc3RGBAUnorm)
        _c(CompressedBc3RGBASrgb)
        _c(CompressedBc4RUnorm)
        _c(CompressedBc4RSnorm)
        _c(CompressedBc5RGUnorm)
        _c(CompressedBc5RGSnorm)
        _c(CompressedBc6hRGBUfloat)
        _c(CompressedBc6hRGBSfloat)
        _c(CompressedBc7RGBAUnorm)
        _c(CompressedBc7RGBASrgb)
        _c(CompressedEacR11Unorm)
        _c(CompressedEacR11Snorm)
        _c(CompressedEacRG11Unorm)
        _c(CompressedEacRG11Snorm)
        _c(CompressedEtc2RGB8Unorm)
        _c(CompressedEtc2RGB8Srgb)
        _c(CompressedEtc2RGB8A1Unorm)
        _c(CompressedEtc2RGB8A1Srgb)
        _c(CompressedEtc2RGBA8Unorm)
        _c(CompressedEtc2RGBA8Srgb)
        _c(CompressedAstc4x4RGBAUnorm)
        _c(CompressedAstc4x4RGBASrgb)
        _c(CompressedAstc4x4RGBAF)
        _c(CompressedAstc5x4RGBAUnorm)
        _c(CompressedAstc5x4RGBASrgb)
        _c(CompressedAstc5x4RGBAF)
        _c(CompressedAstc5x5RGBAUnorm)
        _c(CompressedAstc5x5RGBASrgb)
        _c(CompressedAstc5x5RGBAF)
        _c(CompressedAstc6x5RGBAUnorm)
        _c(CompressedAstc6x5RGBASrgb)
        _c(CompressedAstc6x5RGBAF)
        _c(CompressedAstc6x6RGBAUnorm)
        _c(CompressedAstc6x6RGBASrgb)
        _c(CompressedAstc6x6RGBAF)
        _c(CompressedAstc8x5RGBAUnorm)
        _c(CompressedAstc8x5RGBASrgb)
        _c(CompressedAstc8x5RGBAF)
        _c(CompressedAstc8x6RGBAUnorm)
        _c(CompressedAstc8x6RGBASrgb)
        _c(CompressedAstc8x6RGBAF)
        _c(CompressedAstc8x8RGBAUnorm)
        _c(CompressedAstc8x8RGBASrgb)
        _c(CompressedAstc8x8RGBAF)
        _c(CompressedAstc10x5RGBAUnorm)
        _c(CompressedAstc10x5RGBASrgb)
        _c(CompressedAstc10x5RGBAF)
        _c(CompressedAstc10x6RGBAUnorm)
        _c(CompressedAstc10x6RGBASrgb)
        _c(CompressedAstc10x6RGBAF)
        _c(CompressedAstc10x8RGBAUnorm)
        _c(CompressedAstc10x8RGBASrgb)
        _c(CompressedAstc10x8RGBAF)
        _c(CompressedAstc10x10RGBAUnorm)
        _c(CompressedAstc10x10RGBASrgb)
        _c(CompressedAstc10x10RGBAF)
        _c(CompressedAstc12x10RGBAUnorm)
        _c(CompressedAstc12x10RGBASrgb)
        _c(CompressedAstc12x10RGBAF)
        _c(CompressedAstc12x12RGBAUnorm)
        _c(CompressedAstc12x12RGBASrgb)
        _c(CompressedAstc12x12RGBAF)
        _c(CompressedPvrtcRGBA2bppUnorm)
        _c(CompressedPvrtcRGBA2bppSrgb)
        _c(CompressedPvrtcRGBA4bppUnorm)
        _c(CompressedPvrtcRGBA4bppSrgb)
        _c(CompressedPvrtc2RGBA2bppUnorm)
        _c(CompressedPvrtc2RGBA2bppSrgb)
        _c(CompressedPvrtc2RGBA4bppUnorm)
        _c(CompressedPvrtc2RGBA4bppSrgb)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

bool hasPixelFormat(const Magnum::PixelFormat format) {
    if(isPixelFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(PixelFormatMapping),
        "Vk::hasPixelFormat(): invalid format" << format, {});
    return UnsignedInt(PixelFormatMapping[UnsignedInt(format) - 1]);
}

bool hasPixelFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedPixelFormatMapping),
        "Vk::hasPixelFormat(): invalid format" << format, {});
    return UnsignedInt(CompressedPixelFormatMapping[UnsignedInt(format) - 1]);
}

PixelFormat pixelFormat(const Magnum::PixelFormat format) {
    if(isPixelFormatImplementationSpecific(format))
        return pixelFormatUnwrap<PixelFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(PixelFormatMapping),
        "Vk::pixelFormat(): invalid format" << format, {});
    const PixelFormat out = PixelFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "Vk::pixelFormat(): unsupported format" << format, {});
    return out;
}

PixelFormat pixelFormat(const Magnum::CompressedPixelFormat format) {
    if(isCompressedPixelFormatImplementationSpecific(format))
        return compressedPixelFormatUnwrap<PixelFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(CompressedPixelFormatMapping),
        "Vk::pixelFormat(): invalid format" << format, {});
    const PixelFormat out = CompressedPixelFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "Vk::pixelFormat(): unsupported format" << format, {});
    return out;
}

}}
