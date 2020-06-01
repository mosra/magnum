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

#include "Image.h"

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/ImageProperties.h"

namespace Magnum {

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: Image{storage, format, {}, Magnum::pixelSize(format), size, std::move(data)} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: Image{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, std::move(data)} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{pixelSize}, _size{size}, _data{std::move(data)} {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= _data.size(), "Image: data too small, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format) noexcept: Image{storage, format, {}, Magnum::pixelSize(format)} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize) noexcept: Image{storage, pixelFormatWrap(format), formatExtra, pixelSize} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{pixelSize}, _data{} {}

template<UnsignedInt dimensions> Image<dimensions>::Image(Image<dimensions>&& other) noexcept: _storage{std::move(other._storage)}, _format{std::move(other._format)}, _formatExtra{std::move(other._formatExtra)}, _pixelSize{std::move(other._pixelSize)}, _size{std::move(other._size)}, _data{std::move(other._data)} {
    other._size = {};
}

template<UnsignedInt dimensions> Image<dimensions>& Image<dimensions>::operator=(Image<dimensions>&& other) noexcept {
    using std::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_formatExtra, other._formatExtra);
    swap(_pixelSize, other._pixelSize);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> Image<dimensions>::operator BasicMutableImageView<dimensions>() {
    return BasicMutableImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data};
}

template<UnsignedInt dimensions> Image<dimensions>::operator BasicImageView<dimensions>() const {
    return BasicImageView<dimensions>{_storage, _format, _formatExtra, _pixelSize, _size, _data};
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
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: _storage{storage}, _format{format}, _size{size}, _data{std::move(data)} {}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, Containers::Array<char>&& data) noexcept: CompressedImage{storage, compressedPixelFormatWrap(format), size, std::move(data)} {}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(const CompressedPixelStorage storage) noexcept: _storage{storage}, _format{} {}

template<UnsignedInt dimensions> CompressedImage<dimensions>::CompressedImage(CompressedImage<dimensions>&& other) noexcept: _storage{std::move(other._storage)}, _format{std::move(other._format)}, _size{std::move(other._size)}, _data{std::move(other._data)}
{
    other._size = {};
}

template<UnsignedInt dimensions> CompressedImage<dimensions>& CompressedImage<dimensions>::operator=(CompressedImage<dimensions>&& other) noexcept {
    using std::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_size, other._size);
    swap(_data, other._data);
    return *this;
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::operator BasicMutableCompressedImageView<dimensions>() {
    return BasicMutableCompressedImageView<dimensions>{_storage, _format, _size, _data};
}

template<UnsignedInt dimensions> CompressedImage<dimensions>::operator BasicCompressedImageView<dimensions>() const {
    return BasicCompressedImageView<dimensions>{_storage, _format, _size, _data};
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> CompressedImage<dimensions>::dataProperties() const {
    return Implementation::compressedImageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> Containers::Array<char> CompressedImage<dimensions>::release() {
    Containers::Array<char> data{std::move(_data)};
    _size = {};
    return data;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT Image<1>;
template class MAGNUM_EXPORT Image<2>;
template class MAGNUM_EXPORT Image<3>;

template class MAGNUM_EXPORT CompressedImage<1>;
template class MAGNUM_EXPORT CompressedImage<2>;
template class MAGNUM_EXPORT CompressedImage<3>;
#endif

}
