/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "ImageView.h"

#include "Magnum/PixelFormat.h"

namespace Magnum {

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: ImageView{storage, format, {},  Magnum::pixelSize(format), size, data} {}

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: ImageView{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, data} {}

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{pixelSize}, _size{size}, _data{reinterpret_cast<const char*>(data.data()), data.size()} {
    CORRADE_ASSERT(!_data || Implementation::imageDataSize(*this) <= _data.size(), "ImageView::ImageView(): data too small, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
}

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{storage, format, {}, Magnum::pixelSize(format), size} {}

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size) noexcept: ImageView{storage, pixelFormatWrap(format), formatExtra, pixelSize, size} {}

template<UnsignedInt dimensions> ImageView<dimensions>::ImageView(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{pixelSize}, _size{size}, _data{nullptr} {}

template<UnsignedInt dimensions> void ImageView<dimensions>::setData(const Containers::ArrayView<const void> data) {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= data.size(), "ImageView::setData(): data too small, got" << data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
    _data = {reinterpret_cast<const char*>(data.data()), data.size()};
}

template<UnsignedInt dimensions> CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: _storage{storage}, _format{format}, _size{size}, _data{reinterpret_cast<const char*>(data.data()), data.size()} {}

template<UnsignedInt dimensions> CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size) noexcept: _storage{storage}, _format{format}, _size{size} {}

template<UnsignedInt dimensions> CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data) noexcept: CompressedImageView{storage, compressedPixelFormatWrap(format), size, data} {}

template<UnsignedInt dimensions> CompressedImageView<dimensions>::CompressedImageView(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size) noexcept: CompressedImageView{storage, compressedPixelFormatWrap(format), size} {}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT ImageView<1>;
template class MAGNUM_EXPORT ImageView<2>;
template class MAGNUM_EXPORT ImageView<3>;

template class MAGNUM_EXPORT CompressedImageView<1>;
template class MAGNUM_EXPORT CompressedImageView<2>;
template class MAGNUM_EXPORT CompressedImageView<3>;
#endif

}
