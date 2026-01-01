/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "Image.h"

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/ImageProperties.h"

namespace Magnum {

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: Image{storage, format, {}, (CORRADE_CONSTEXPR_ASSERT(!isPixelFormatImplementationSpecific(format), "Image: can't determine size of an implementation-specific pixel format" << Debug::hex << pixelFormatUnwrap(format) << Debug::nospace << ", pass it explicitly"), pixelFormatSize(format)), size, Utility::move(data), flags} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: Image{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, Utility::move(data), flags} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{UnsignedByte(pixelSize)}, _flags{flags}, _size{size}, _data{Utility::move(data)} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkPixelSize("Image:", pixelSize);
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= _data.size(), "Image: data too small, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
    Implementation::checkImageFlagsForSize("Image:", flags, size);
    #endif
}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format) noexcept: Image{storage, format, {}, (CORRADE_CONSTEXPR_ASSERT(!isPixelFormatImplementationSpecific(format), "Image: can't determine size of an implementation-specific pixel format" << Debug::hex << pixelFormatUnwrap(format) << Debug::nospace << ", pass it explicitly"), pixelFormatSize(format))} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize) noexcept: Image{storage, pixelFormatWrap(format), formatExtra, pixelSize} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{UnsignedByte(pixelSize)}, _data{} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkPixelSize("Image:", pixelSize);
    #endif
}

template<UnsignedInt dimensions> Image<dimensions>::Image(Image<dimensions>&& other) noexcept: _storage{Utility::move(other._storage)}, _format{Utility::move(other._format)}, _formatExtra{Utility::move(other._formatExtra)}, _pixelSize{Utility::move(other._pixelSize)}, _flags{Utility::move(other._flags)}, _size{Utility::move(other._size)}, _data{Utility::move(other._data)} {
    other._size = {};
}

template<UnsignedInt dimensions> Image<dimensions>& Image<dimensions>::operator=(Image<dimensions>&& other) noexcept {
    using Utility::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_formatExtra, other._formatExtra);
    swap(_pixelSize, other._pixelSize);
    swap(_flags, other._flags);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> Image<dimensions>::operator BasicMutableImageView<dimensions>() {
    return BasicMutableImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data, _flags};
}

template<UnsignedInt dimensions> Image<dimensions>::operator BasicImageView<dimensions>() const {
    return BasicImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data, _flags};
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> Image<dimensions>::dataProperties() const {
    return Implementation::imageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> Containers::StridedArrayView<dimensions + 1, char> Image<dimensions>::pixels() {
    return Implementation::imagePixelView<dimensions, char>(*this, data());
}

template<UnsignedInt dimensions> Containers::StridedArrayView<dimensions + 1, const char> Image<dimensions>::pixels() const {
    return Implementation::imagePixelView<dimensions, const char>(*this, data());
}

template<UnsignedInt dimensions> Containers::Array<char> Image<dimensions>::release() {
    Containers::Array<char> data{Utility::move(_data)};
    _size = {};
    return data;
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: CompressedImage{storage, format, (CORRADE_CONSTEXPR_ASSERT(!isCompressedPixelFormatImplementationSpecific(format), "CompressedImage: can't determine block size of an implementation-specific pixel format" << Debug::hex << compressedPixelFormatUnwrap(format) << Debug::nospace << ", pass it explicitly"), compressedPixelFormatBlockSize(format)), (
    /* Have to do it like this to ensure the above assertion is printed before
       the subsequent one from compressedPixelFormatBlockDataSize(), as
       compilers are free to reorder these as they want */
    #ifndef CORRADE_NO_ASSERT
    isCompressedPixelFormatImplementationSpecific(format) ? 0 :
    #endif
        compressedPixelFormatBlockDataSize(format)
), size, Utility::move(data), flags} {}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage, const CompressedPixelFormat format, const Vector3i& blockSize, const UnsignedInt blockDataSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _flags{flags}, _blockSize{Vector3ub{blockSize}}, _blockDataSize{UnsignedByte(blockDataSize)}, _size{size}, _data{Utility::move(data)} {
    #ifndef CORRADE_NO_ASSERT
    const bool passed =
    Implementation::checkBlockProperties("CompressedImage:", blockSize, blockDataSize);
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* If the above check fails on a build with graceful assertions, the data
       size check below could then die on division by zero. Exit early in that
       case. */
    /** @todo any better idea to handle this? ugh */
    if(!passed) return;
    #else
    static_cast<void>(passed);
    #endif
    Implementation::checkBlockPropertiesForStorage("CompressedImage:", blockSize, blockDataSize, storage);
    CORRADE_ASSERT(Implementation::compressedImageDataSize(*this) <= _data.size(), "CompressedImage: data too small, got" << _data.size() << "but expected at least" << Implementation::compressedImageDataSize(*this) << "bytes", );
    Implementation::checkImageFlagsForSize("CompressedImage:", flags, size);
    #endif
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage, const UnsignedInt format, const Vector3i& blockSize, const UnsignedInt blockDataSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const ImageFlags<dimensions> flags) noexcept: CompressedImage{storage, compressedPixelFormatWrap(format), blockSize, blockDataSize, size, Utility::move(data), flags} {}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage) noexcept: _storage{storage}, _format{}, _blockDataSize{} {
    CORRADE_ASSERT(storage.compressedBlockSize() == Vector3i{},
        "CompressedImage: expected pixel storage block size to not be set at all but got" << Debug::packed << storage.compressedBlockSize(), );
    CORRADE_ASSERT(!storage.compressedBlockDataSize(),
        "CompressedImage: expected pixel storage block data size to not be set at all but got" << storage.compressedBlockDataSize(), );
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(CompressedImage<dimensions>&& other) noexcept: _storage{other._storage}, _format{other._format}, _flags{other._flags}, _blockSize{other._blockSize}, _blockDataSize{other._blockDataSize}, _size{other._size}, _data{Utility::move(other._data)} {
    other._size = {};
}

template<UnsignedInt dimensions> CompressedImage<dimensions>& CompressedImage<dimensions>::operator=(CompressedImage<dimensions>&& other) noexcept {
    using Utility::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_flags, other._flags);
    swap(_blockSize, other._blockSize);
    swap(_blockDataSize, other._blockDataSize);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::operator BasicMutableCompressedImageView<dimensions>() {
    return BasicMutableCompressedImageView<dimensions>{_storage, _format, Vector3i{_blockSize}, _blockDataSize, _size, _data, _flags};
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::operator BasicCompressedImageView<dimensions>() const {
    return BasicCompressedImageView<dimensions>{_storage, _format, Vector3i{_blockSize}, _blockDataSize, _size, _data, _flags};
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> CompressedImage<dimensions>::dataProperties() const {
    return Implementation::compressedImageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> Containers::Array<char> CompressedImage<dimensions>::release() {
    Containers::Array<char> data{Utility::move(_data)};
    _size = {};
    return data;
}

template class MAGNUM_EXPORT Image<1>;
template class MAGNUM_EXPORT Image<2>;
template class MAGNUM_EXPORT Image<3>;

template class MAGNUM_EXPORT CompressedImage<1>;
template class MAGNUM_EXPORT CompressedImage<2>;
template class MAGNUM_EXPORT CompressedImage<3>;

}
