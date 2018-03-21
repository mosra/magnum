/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Math/Vector4.h"

#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum {

std::size_t PixelStorage::pixelSize(GL::PixelFormat format, GL::PixelType type) {
    std::size_t size = 0;
    switch(type) {
        case GL::PixelType::UnsignedByte:
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelType::Byte:
        #endif
            size = 1; break;
        case GL::PixelType::UnsignedShort:
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelType::Short:
        #endif
        case GL::PixelType::HalfFloat:
            size = 2; break;
        case GL::PixelType::UnsignedInt:
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelType::Int:
        #endif
        case GL::PixelType::Float:
            size = 4; break;

        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelType::UnsignedByte332:
        case GL::PixelType::UnsignedByte233Rev:
            return 1;
        #endif
        case GL::PixelType::UnsignedShort565:
        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelType::UnsignedShort565Rev:
        #endif
        case GL::PixelType::UnsignedShort4444:
        #ifndef MAGNUM_TARGET_WEBGL
        case GL::PixelType::UnsignedShort4444Rev:
        #endif
        case GL::PixelType::UnsignedShort5551:
        #ifndef MAGNUM_TARGET_WEBGL
        case GL::PixelType::UnsignedShort1555Rev:
        #endif
            return 2;
        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelType::UnsignedInt8888:
        case GL::PixelType::UnsignedInt8888Rev:
        case GL::PixelType::UnsignedInt1010102:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case GL::PixelType::UnsignedInt2101010Rev:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelType::UnsignedInt10F11F11FRev:
        case GL::PixelType::UnsignedInt5999Rev:
        #endif
        case GL::PixelType::UnsignedInt248:
            return 4;
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelType::Float32UnsignedInt248Rev:
            return 8;
        #endif
    }

    switch(format) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case GL::PixelFormat::Red:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::RedInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelFormat::Green:
        case GL::PixelFormat::Blue:
        case GL::PixelFormat::GreenInteger:
        case GL::PixelFormat::BlueInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::Luminance:
        #endif
        case GL::PixelFormat::DepthComponent:
        #ifndef MAGNUM_TARGET_WEBGL
        case GL::PixelFormat::StencilIndex:
        #endif
            return 1*size;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case GL::PixelFormat::RG:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::RGInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::LuminanceAlpha:
        #endif
            return 2*size;
        case GL::PixelFormat::RGB:
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::RGBInteger:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelFormat::BGR:
        case GL::PixelFormat::BGRInteger:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::SRGB:
        #endif
            return 3*size;
        case GL::PixelFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::RGBAInteger:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case GL::PixelFormat::BGRA:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case GL::PixelFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case GL::PixelFormat::BGRAInteger:
        #endif
            return 4*size;

        /* Handled above */
        case GL::PixelFormat::DepthStencil:
            CORRADE_ASSERT(false, "PixelStorage::pixelSize(): invalid PixelType specified for depth/stencil PixelFormat", 0);
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

bool PixelStorage::operator==(const PixelStorage& other) const {
    return
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        _rowLength == other._rowLength &&
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _imageHeight == other._imageHeight &&
        #endif
        _skip == other._skip &&
        _alignment == other._alignment;
}

std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> PixelStorage::dataProperties(const GL::PixelFormat format, const GL::PixelType type, const Vector3i& size) const {
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

    return std::make_tuple(Math::Vector3<std::size_t>{pixelSize, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{_skip},
        size.product() ? dataSize : Math::Vector3<std::size_t>{}, pixelSize);
}

#ifndef MAGNUM_TARGET_GLES
std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> CompressedPixelStorage::dataProperties(const Vector3i& size) const {
    CORRADE_ASSERT(_blockDataSize && _blockSize.product(), "CompressedPixelStorage::dataProperties(): expected non-zero storage parameters", {});

    const Vector3i blockCount = (size + _blockSize - Vector3i{1})/_blockSize;
    const Math::Vector3<std::size_t> dataSize{
        std::size_t(_rowLength ? (_rowLength + _blockSize.x() - 1)/_blockSize.x() : blockCount.x()),
        std::size_t(_imageHeight ? (_imageHeight + _blockSize.y() - 1)/_blockSize.y() : blockCount.y()),
        std::size_t(blockCount.z())};

    const Vector3i skipBlockCount = (_skip + _blockSize - Vector3i{1})/_blockSize;
    const Math::Vector3<std::size_t> offset = (Math::Vector3<std::size_t>{1, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{skipBlockCount})*_blockDataSize;

    return std::make_tuple(offset, size.product() ? dataSize : Math::Vector3<std::size_t>{}, _blockDataSize);
}
#endif

void PixelStorage::applyInternal(const bool isUnpack) {
    GL::Implementation::RendererState::PixelStorage& state = isUnpack ?
        Context::current().state().renderer->unpackPixelStorage :
        Context::current().state().renderer->packPixelStorage;

    /* Alignment */
    if(state.alignment == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.alignment != _alignment)
        glPixelStorei(isUnpack ? GL_UNPACK_ALIGNMENT : GL_PACK_ALIGNMENT,
            state.alignment = _alignment);

    /* Row length */
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    if(state.rowLength == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.rowLength != _rowLength)
    {
        /** @todo Use real value for GL_PACK_ROW_LENGTH_NV when it is in headers */
        #ifndef MAGNUM_TARGET_GLES2
        glPixelStorei(isUnpack ? GL_UNPACK_ROW_LENGTH : GL_PACK_ROW_LENGTH,
            state.rowLength = _rowLength);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        glPixelStorei(isUnpack ? GL_UNPACK_ROW_LENGTH_EXT : 0xD02 /*GL_PACK_ROW_LENGTH_NV*/,
            state.rowLength = _rowLength);
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* Image height (on ES for unpack only, taken care of below) */
    if(state.imageHeight == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.imageHeight != _imageHeight)
        glPixelStorei(isUnpack ? GL_UNPACK_IMAGE_HEIGHT : GL_PACK_IMAGE_HEIGHT,
            state.imageHeight = _imageHeight);
    #endif

    /* On ES2 done by modifying data pointer */
    #ifndef MAGNUM_TARGET_GLES2
    /* Skip pixels */
    if(state.skip.x() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.x() != _skip.x())
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_PIXELS : GL_PACK_SKIP_PIXELS,
            state.skip.x() = _skip.x());

    /* Skip rows */
    if(state.skip.y() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.y() != _skip.y())
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_ROWS : GL_PACK_SKIP_ROWS,
            state.skip.y() = _skip.y());

    #ifndef MAGNUM_TARGET_GLES
    /* Skip images (on ES for unpack only, taken care of below) */
    if(state.skip.z() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.z() != _skip.z())
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_IMAGES : GL_PACK_SKIP_IMAGES,
            state.skip.z() = _skip.z());
    #endif
    #endif
}

void PixelStorage::applyUnpack() {
    applyInternal(true);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    Implementation::RendererState::PixelStorage& state = Context::current().state().renderer->unpackPixelStorage;

    /* Image height (on ES for unpack only) */
    if(state.imageHeight == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.imageHeight != _imageHeight)
        glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, state.imageHeight = _imageHeight);

    /* Skip images (on ES for unpack only) */
    if(state.skip.z() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.z() != _skip.z())
        glPixelStorei(GL_UNPACK_SKIP_IMAGES, state.skip.z() = _skip.z());
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool CompressedPixelStorage::operator==(const CompressedPixelStorage& other) const {
    return PixelStorage::operator==(other) &&
        _blockSize == other._blockSize &&
        _blockDataSize == other._blockDataSize;
}

void CompressedPixelStorage::applyInternal(const bool isUnpack) {
    PixelStorage::applyInternal(isUnpack);

    GL::Implementation::RendererState::PixelStorage& state = isUnpack ?
        Context::current().state().renderer->unpackPixelStorage :
        Context::current().state().renderer->packPixelStorage;

    /* Compressed block width */
    if(state.compressedBlockSize.x() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.compressedBlockSize.x() != _blockSize.x())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_WIDTH : GL_PACK_COMPRESSED_BLOCK_WIDTH,
            state.compressedBlockSize.x() = _blockSize.x());

    /* Compressed block height */
    if(state.compressedBlockSize.y() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.compressedBlockSize.y() != _blockSize.y())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_HEIGHT : GL_PACK_COMPRESSED_BLOCK_HEIGHT,
            state.compressedBlockSize.y() = _blockSize.y());

    /* Compressed block depth */
    if(state.compressedBlockSize.z() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.compressedBlockSize.z() != _blockSize.z())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_DEPTH : GL_PACK_COMPRESSED_BLOCK_DEPTH,
            state.compressedBlockSize.z() = _blockSize.z());

    /* Compressed block size */
    if(state.compressedBlockDataSize == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.compressedBlockDataSize != _blockDataSize)
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_SIZE : GL_PACK_COMPRESSED_BLOCK_SIZE,
            state.compressedBlockDataSize = _blockDataSize);
}
#endif

}
