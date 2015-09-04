/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "PixelStorage.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum {

std::size_t PixelStorage::pixelSize(PixelFormat format, PixelType type) {
    std::size_t size = 0;
    switch(type) {
        case PixelType::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Byte:
        #endif
            size = 1; break;
        case PixelType::UnsignedShort:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Short:
        #endif
        case PixelType::HalfFloat:
            size = 2; break;
        case PixelType::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Int:
        #endif
        case PixelType::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedByte332:
        case PixelType::UnsignedByte233Rev:
            return 1;
        #endif
        case PixelType::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedShort565Rev:
        #endif
        case PixelType::UnsignedShort4444:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelType::UnsignedShort4444Rev:
        #endif
        case PixelType::UnsignedShort5551:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelType::UnsignedShort1555Rev:
        #endif
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case PixelType::UnsignedInt8888:
        case PixelType::UnsignedInt8888Rev:
        case PixelType::UnsignedInt1010102:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelType::UnsignedInt2101010Rev:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::UnsignedInt10F11F11FRev:
        case PixelType::UnsignedInt5999Rev:
        #endif
        case PixelType::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case PixelType::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }

    switch(format) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::Red:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::Green:
        case PixelFormat::Blue:
        case PixelFormat::GreenInteger:
        case PixelFormat::BlueInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::Luminance:
        #endif
        case PixelFormat::DepthComponent:
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::StencilIndex:
        #endif
            return 1*size;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case PixelFormat::RG:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case PixelFormat::LuminanceAlpha:
        #endif
            return 2*size;
        case PixelFormat::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGR:
        case PixelFormat::BGRInteger:
        #endif
            return 3*size;
        case PixelFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case PixelFormat::RGBAInteger:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case PixelFormat::BGRA:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case PixelFormat::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case PixelFormat::DepthStencil:
            CORRADE_ASSERT(false, "AbstractImage::pixelSize(): invalid PixelType specified for depth/stencil PixelFormat", 0);
    }

    CORRADE_ASSERT_UNREACHABLE();
}

std::tuple<std::size_t, Math::Vector3<std::size_t>, std::size_t> PixelStorage::dataProperties(const PixelFormat format, const PixelType type, const Vector3i& size) const {
    const std::size_t pixelSize = PixelStorage::pixelSize(format, type);
    const Math::Vector3<std::size_t> dataSize{
        std::size_t((((
            #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
            _rowLength ? _rowLength*pixelSize :
            #endif
            size[0]*pixelSize) + _alignment - 1)/_alignment)*_alignment),
        #ifndef MAGNUM_TARGET_GLES2
        std::size_t(_imageHeight ? _imageHeight : size.y()),
        #else
        std::size_t(size.y()),
        #endif
        std::size_t(size.z())};
    const std::size_t offset = (Math::Vector3<std::size_t>{pixelSize, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{_skip}).sum();

    return std::make_tuple(offset, size.product() ? dataSize : Math::Vector3<std::size_t>{}, pixelSize);
}

#ifndef MAGNUM_TARGET_GLES
std::tuple<std::size_t, Math::Vector3<std::size_t>, std::size_t> CompressedPixelStorage::dataProperties(const Vector3i& size) const {
    CORRADE_ASSERT(_blockDataSize && _blockSize.product(), "CompressedPixelStorage::dataProperties(): expected non-zero storage parameters", {});

    const Vector3i blockCount = (size + _blockSize - Vector3i{1})/_blockSize;
    const Math::Vector3<std::size_t> dataSize{
        std::size_t(_rowLength ? (_rowLength + _blockSize.x() - 1)/_blockSize.x() : blockCount.x()),
        std::size_t(_imageHeight ? (_imageHeight + _blockSize.y() - 1)/_blockSize.y() : blockCount.y()),
        std::size_t(blockCount.z())};

    const Vector3i skipBlockCount = (_skip + _blockSize - Vector3i{1})/_blockSize;
    const std::size_t offset = (Math::Vector3<std::size_t>{1, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{skipBlockCount}).sum()*_blockDataSize;

    return std::make_tuple(offset, size.product() ? dataSize : Math::Vector3<std::size_t>{}, _blockDataSize);
}
#endif

}
