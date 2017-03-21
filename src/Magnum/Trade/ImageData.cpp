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

#include "ImageData.h"

namespace Magnum { namespace Trade {

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: _compressed{false}, _storage{storage}, _format{format}, _type{type}, _size{size}, _data{std::move(data)}, _importerState{importerState} {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= _data.size(), "Trade::ImageData::ImageData(): bad image data size, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this), );
}

template<UnsignedInt dimensions> PixelStorage ImageData<dimensions>::storage() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::storage(): the image is compressed", {});
    return _storage;
}

template<UnsignedInt dimensions> PixelFormat ImageData<dimensions>::format() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::format(): the image is compressed", {});
    return _format;
}

template<UnsignedInt dimensions> PixelType ImageData<dimensions>::type() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::type(): the image is compressed", {});
    return _type;
}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> CompressedPixelStorage ImageData<dimensions>::compressedStorage() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData::compressedStorage(): the image is not compressed", {});
    return _compressedStorage;
}
#endif

template<UnsignedInt dimensions> CompressedPixelFormat ImageData<dimensions>::compressedFormat() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData::compressedFormat(): the image is not compressed", {});
    return _compressedFormat;
}

template<UnsignedInt dimensions> std::size_t ImageData<dimensions>::pixelSize() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::pixelSize(): the image is compressed", {});
    return PixelStorage::pixelSize(_format, _type);
}

template<UnsignedInt dimensions> std::tuple<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>, std::size_t> ImageData<dimensions>::dataProperties() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::dataProperties(): the image is compressed", {});
    return Implementation::imageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator ImageView<dimensions>() const
{
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::type(): the image is compressed", (ImageView<dimensions>{_storage, _format, _type, _size}));
    return ImageView<dimensions>{_storage, _format, _type, _size, _data};
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator CompressedImageView<dimensions>() const
{
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_compressed, "Trade::ImageData::type(): the image is not compressed", (CompressedImageView<dimensions>{_compressedStorage, _compressedFormat, _size}));
    #else
    CORRADE_ASSERT(_compressed, "Trade::ImageData::type(): the image is not compressed", (CompressedImageView<dimensions>{_compressedFormat, _size}));
    #endif
    return CompressedImageView<dimensions>{
        #ifndef MAGNUM_TARGET_GLES
        _compressedStorage,
        #endif
        _compressedFormat, _size, _data};
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT ImageData<1>;
template class MAGNUM_EXPORT ImageData<2>;
template class MAGNUM_EXPORT ImageData<3>;
#endif

}}
