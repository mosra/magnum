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

#include "BufferImage.h"

#include "Magnum/PixelFormat.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage): _storage{storage}, _format{format}, _type{type}, _size{size}, _buffer{Buffer::TargetHint::PixelPack}, _dataSize{data.size()} {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= data.size(), "BufferImage::BufferImage(): bad image data size, got" << data.size() << "but expected at least" << Implementation::imageDataSize(*this), );
    _buffer.setData(data, usage);
}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: _storage{storage}, _format{format}, _type{type}, _size{size}, _buffer{std::move(buffer)}, _dataSize{dataSize} {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= dataSize, "BufferImage::BufferImage(): bad image data size, got" << dataSize << "but expected at least" << Implementation::imageDataSize(*this), );
}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type): _storage{storage}, _format{format}, _type{type}, _buffer{Buffer::TargetHint::PixelPack}, _dataSize{0} {}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(NoCreateT) noexcept: _format{PixelFormat::RGBA}, _type{PixelType::UnsignedByte}, _buffer{NoCreate}, _dataSize{} {}

template<UnsignedInt dimensions> void BufferImage<dimensions>::setData(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage) {
    _storage = storage;
    _format = format;
    _type = type;
    _size = size;

    /* Keep the old storage if zero-sized nullptr buffer was passed */
    if(data.data() == nullptr && data.size() == 0)
        CORRADE_ASSERT(Implementation::imageDataSize(*this) <= _dataSize, "BufferImage::setData(): bad current storage size, got" << _dataSize << "but expected at least" << Implementation::imageDataSize(*this), );
    else {
        CORRADE_ASSERT(Implementation::imageDataSize(*this) <= data.size(), "BufferImage::setData(): bad image data size, got" << data.size() << "but expected at least" << Implementation::imageDataSize(*this), );
        _buffer.setData(data, usage);
        _dataSize = data.size();
    }
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage):
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{format}, _size{size}, _buffer{Buffer::TargetHint::PixelPack}, _dataSize{data.size()}
{
    _buffer.setData(data, usage);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept:
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{format}, _size{size}, _buffer{std::move(buffer)}, _dataSize{dataSize} {}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage
    #endif
    ):
    #ifndef MAGNUM_TARGET_GLES
    _storage{storage},
    #endif
    _format{}, _buffer{Buffer::TargetHint::PixelPack}, _dataSize{} {}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(NoCreateT) noexcept: _format{}, _buffer{NoCreate}, _dataSize{} {}

template<UnsignedInt dimensions> void CompressedBufferImage<dimensions>::setData(
    #ifndef MAGNUM_TARGET_GLES
    const CompressedPixelStorage storage,
    #endif
    const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage)
{
    #ifndef MAGNUM_TARGET_GLES
    _storage = storage;
    #endif
    _format = format;
    _size = size;
    _buffer.setData(data, usage);
    _dataSize = data.size();
}

template<UnsignedInt dimensions> Buffer BufferImage<dimensions>::release() {
    _size = {};
    _dataSize = {};
    return std::move(_buffer);
}

template<UnsignedInt dimensions> Buffer CompressedBufferImage<dimensions>::release() {
    _size = {};
    _dataSize = {};
    return std::move(_buffer);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT BufferImage<1>;
template class MAGNUM_EXPORT BufferImage<2>;
template class MAGNUM_EXPORT BufferImage<3>;

template class MAGNUM_EXPORT CompressedBufferImage<1>;
template class MAGNUM_EXPORT CompressedBufferImage<2>;
template class MAGNUM_EXPORT CompressedBufferImage<3>;
#endif
#endif

}
