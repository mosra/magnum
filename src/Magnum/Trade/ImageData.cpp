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

#include "ImageData.h"

#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/ImageProperties.h"

namespace Magnum { namespace Trade {

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, format, {}, Magnum::pixelSize(format), size, std::move(data), importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{storage, format, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{{}, format, size, std::move(data), importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{format, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, std::move(data), importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _compressed{false}, _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{pixelSize}, _size{size}, _data{std::move(data)}, _importerState{importerState} {
    CORRADE_ASSERT(Magnum::Implementation::imageDataSize(*this) <= _data.size(), "Trade::ImageData: data too small, got" << _data.size() << "but expected at least" << Magnum::Implementation::imageDataSize(*this) << "bytes", );
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, dataFlags, data, importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{storage, format, formatExtra, pixelSize, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _compressed{true}, _compressedStorage{storage}, _compressedFormat{format}, _size{size}, _data{std::move(data)}, _importerState{importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{storage, format, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{{}, format, size, std::move(data), importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{format, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data, const void* const importerState) noexcept: ImageData{storage, compressedPixelFormatWrap(format), size, std::move(data), importerState} {}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, const DataFlags dataFlags, const Containers::ArrayView<const void> data, const void* const importerState) noexcept: ImageData{storage, format, size, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::ImageData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(ImageData<dimensions>&& other) noexcept: _dataFlags{other._dataFlags}, _compressed{std::move(other._compressed)}, _size{std::move(other._size)}, _data{std::move(other._data)}, _importerState{std::move(other._importerState)} {
    if(_compressed) {
        new(&_compressedStorage) CompressedPixelStorage{std::move(other._compressedStorage)};
        _compressedFormat = std::move(other._compressedFormat);
    }
    else {
        new(&_storage) PixelStorage{std::move(other._storage)};
        _format = std::move(other._format);
        _formatExtra = std::move(other._formatExtra);
        _pixelSize = std::move(other._pixelSize);
    }

    other._size = {};
}

template<UnsignedInt dimensions> ImageData<dimensions>::ImageData(ImageData<dimensions>&& other, const void* const importerState) noexcept: ImageData{std::move(other)} {
    _importerState = importerState;
}

template<UnsignedInt dimensions> ImageData<dimensions>& ImageData<dimensions>::operator=(ImageData<dimensions>&& other) noexcept {
    using std::swap;
    swap(_dataFlags, other._dataFlags);
    swap(_compressed, other._compressed);
    if(_compressed) {
        swap(_compressedStorage, other._compressedStorage);
        swap(_compressedFormat, other._compressedFormat);
    }
    else {
        swap(_storage, other._storage);
        swap(_format, other._format);
    }
    swap(_formatExtra, other._formatExtra);
    swap(_pixelSize, other._pixelSize);
    swap(_size, other._size);
    swap(_data, other._data);
    swap(_importerState, other._importerState);
    return *this;
}

template<UnsignedInt dimensions> PixelStorage ImageData<dimensions>::storage() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::storage(): the image is compressed", {});
    return _storage;
}

template<UnsignedInt dimensions> PixelFormat ImageData<dimensions>::format() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::format(): the image is compressed", {});
    return _format;
}

template<UnsignedInt dimensions> UnsignedInt ImageData<dimensions>::formatExtra() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::formatExtra(): the image is compressed", {});
    return _formatExtra;
}

template<UnsignedInt dimensions> CompressedPixelStorage ImageData<dimensions>::compressedStorage() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData::compressedStorage(): the image is not compressed", {});
    return _compressedStorage;
}

template<UnsignedInt dimensions> CompressedPixelFormat ImageData<dimensions>::compressedFormat() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData::compressedFormat(): the image is not compressed", {});
    return _compressedFormat;
}

template<UnsignedInt dimensions> UnsignedInt ImageData<dimensions>::pixelSize() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::pixelSize(): the image is compressed", {});
    return _pixelSize;
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> ImageData<dimensions>::dataProperties() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::dataProperties(): the image is compressed", {});
    return Magnum::Implementation::imageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> Containers::ArrayView<char> ImageData<dimensions>::mutableData() & {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::ImageData::mutableData(): the image is not mutable", {});
    return _data;
}

template<UnsignedInt dimensions> Containers::StridedArrayView<dimensions + 1, const char> ImageData<dimensions>::pixels() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::pixels(): the image is compressed", {});
    return Magnum::Implementation::imagePixelView<dimensions, const char>(*this, data());
}

template<UnsignedInt dimensions> Containers::StridedArrayView<dimensions + 1, char> ImageData<dimensions>::mutablePixels() {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::ImageData::mutablePixels(): the image is not mutable", {});
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::mutablePixels(): the image is compressed", {});
    return Magnum::Implementation::imagePixelView<dimensions, char>(*this, mutableData());
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator BasicImageView<dimensions>() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData: the image is compressed", (BasicImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size}));
    return BasicImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data};
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator BasicMutableImageView<dimensions>() {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::ImageData: the image is not mutable", (BasicMutableImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size}));
    CORRADE_ASSERT(!_compressed, "Trade::ImageData: the image is compressed", (BasicMutableImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size}));
    return BasicMutableImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data};
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator BasicCompressedImageView<dimensions>() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData: the image is not compressed", (BasicCompressedImageView<dimensions>{_compressedStorage, _compressedFormat, _size}));
    return BasicCompressedImageView<dimensions>{
        _compressedStorage,
        _compressedFormat, _size, _data};
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator BasicMutableCompressedImageView<dimensions>() {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::ImageData: the image is not mutable",
        (BasicMutableCompressedImageView<dimensions>{_compressedStorage, _compressedFormat, _size}));
    CORRADE_ASSERT(_compressed, "Trade::ImageData: the image is not compressed", (BasicMutableCompressedImageView<dimensions>{_compressedStorage, _compressedFormat, _size}));
    return BasicMutableCompressedImageView<dimensions>{
        _compressedStorage,
        _compressedFormat, _size, _data};
}

template<UnsignedInt dimensions> Containers::Array<char> ImageData<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_TRADE_EXPORT ImageData<1>;
template class MAGNUM_TRADE_EXPORT ImageData<2>;
template class MAGNUM_TRADE_EXPORT ImageData<3>;
#endif

}}
