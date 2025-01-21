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

#include "ImageView.h"

#include "Magnum/PixelFormat.h"
#include "Magnum/Implementation/ImageProperties.h"

namespace Magnum {

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, format, {}, (CORRADE_CONSTEXPR_ASSERT(!isPixelFormatImplementationSpecific(format), "ImageView: can't determine size of an implementation-specific pixel format" << Debug::hex << pixelFormatUnwrap(format) << Debug::nospace << ", pass it explicitly"), pixelFormatSize(format)), size, data, flags} {}

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, data, flags} {}

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{UnsignedByte(pixelSize)}, _flags{flags}, _size{size}, _data{reinterpret_cast<Type*>(data.data()), data.size()} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkPixelSize("ImageView:", pixelSize);
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= _data.size(), "ImageView: data too small, got" << _data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
    Implementation::checkImageFlagsForSize("ImageView:", flags, size);
    #endif
}

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const PixelFormat format, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, format, {}, (CORRADE_CONSTEXPR_ASSERT(!isPixelFormatImplementationSpecific(format), "ImageView: can't determine size of an implementation-specific pixel format" << Debug::hex << pixelFormatUnwrap(format) << Debug::nospace << ", pass it explicitly"), pixelFormatSize(format)), size, flags} {}

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const UnsignedInt format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: ImageView{storage, pixelFormatWrap(format), formatExtra, pixelSize, size, flags} {}

template<UnsignedInt dimensions, class T> ImageView<dimensions, T>::ImageView(const PixelStorage storage, const PixelFormat format, const UnsignedInt formatExtra, const UnsignedInt pixelSize, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _formatExtra{formatExtra}, _pixelSize{UnsignedByte(pixelSize)}, _flags{flags}, _size{size}, _data{nullptr} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkPixelSize("ImageView:", pixelSize);
    Implementation::checkImageFlagsForSize("ImageView:", flags, size);
    #endif
}

template<UnsignedInt dimensions, class T> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> ImageView<dimensions, T>::dataProperties() const {
    return Implementation::imageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions, class T> void ImageView<dimensions, T>::setData(const Containers::ArrayView<ErasedType> data) {
    CORRADE_ASSERT(Implementation::imageDataSize(*this) <= data.size(), "ImageView::setData(): data too small, got" << data.size() << "but expected at least" << Implementation::imageDataSize(*this) << "bytes", );
    _data = {reinterpret_cast<Type*>(data.data()), data.size()};
}

template<UnsignedInt dimensions, class T> auto ImageView<dimensions, T>::pixels() const -> Containers::StridedArrayView<dimensions + 1, Type> {
    if(!_data && !_data.size()) return {};
    return Implementation::imagePixelView<dimensions, Type>(*this, data());
}

template<UnsignedInt dimensions, class T> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _flags{flags}, _size{size}, _data{reinterpret_cast<Type*>(data.data()), data.size()} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkImageFlagsForSize("CompressedImageView:", flags, size);
    #endif
}

template<UnsignedInt dimensions, class T> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: _storage{storage}, _format{format}, _flags{flags}, _size{size} {
    #ifndef CORRADE_NO_ASSERT
    Implementation::checkImageFlagsForSize("CompressedImageView:", flags, size);
    #endif
}

template<UnsignedInt dimensions, class T> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<ErasedType> data, const ImageFlags<dimensions> flags) noexcept: CompressedImageView{storage, compressedPixelFormatWrap(format), size, data, flags} {}

template<UnsignedInt dimensions, class T> CompressedImageView<dimensions, T>::CompressedImageView(const CompressedPixelStorage storage, const UnsignedInt format, const VectorTypeFor<dimensions, Int>& size, const ImageFlags<dimensions> flags) noexcept: CompressedImageView{storage, compressedPixelFormatWrap(format), size, flags} {}

template<UnsignedInt dimensions, class T> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> CompressedImageView<dimensions, T>::dataProperties() const {
    return Implementation::compressedImageDataProperties<dimensions>(*this);
}

template class MAGNUM_EXPORT ImageView<1, const char>;
template class MAGNUM_EXPORT ImageView<2, const char>;
template class MAGNUM_EXPORT ImageView<3, const char>;
template class MAGNUM_EXPORT ImageView<1, char>;
template class MAGNUM_EXPORT ImageView<2, char>;
template class MAGNUM_EXPORT ImageView<3, char>;

template class MAGNUM_EXPORT CompressedImageView<1, const char>;
template class MAGNUM_EXPORT CompressedImageView<2, const char>;
template class MAGNUM_EXPORT CompressedImageView<3, const char>;
template class MAGNUM_EXPORT CompressedImageView<1, char>;
template class MAGNUM_EXPORT CompressedImageView<2, char>;
template class MAGNUM_EXPORT CompressedImageView<3, char>;

}
