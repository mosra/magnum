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

#include "ImageData.h"

namespace Magnum { namespace Trade {

template<UnsignedInt dimensions> ColorFormat ImageData<dimensions>::format() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::format(): the image is compressed", {});
    return _format;
}

template<UnsignedInt dimensions> ColorType ImageData<dimensions>::type() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::type(): the image is compressed", {});
    return _type;
}

template<UnsignedInt dimensions> CompressedColorFormat ImageData<dimensions>::compressedFormat() const {
    CORRADE_ASSERT(_compressed, "Trade::ImageData::format(): the image is not compressed", {});
    return _compressedFormat;
}

template<UnsignedInt dimensions> std::size_t ImageData<dimensions>::pixelSize() const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::pixelSize(): the image is compressed", {});
    return Implementation::imagePixelSize(_format, _type);
}

template<UnsignedInt dimensions> std::size_t ImageData<dimensions>::dataSize(const VectorTypeFor< dimensions, Int >& size) const {
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::dataSize(): the image is compressed", {});
    return Implementation::imageDataSize<dimensions>(_format, _type, size);
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator ImageView<dimensions>()
#ifndef CORRADE_GCC47_COMPATIBILITY
const &
#else
const
#endif
{
    CORRADE_ASSERT(!_compressed, "Trade::ImageData::type(): the image is compressed", (ImageView<dimensions>{_format, _type, _size}));
    return ImageView<dimensions>{_format, _type, _size, _data};
}

template<UnsignedInt dimensions> ImageData<dimensions>::operator CompressedImageView<dimensions>()
#ifndef CORRADE_GCC47_COMPATIBILITY
const &
#else
const
#endif
{
    CORRADE_ASSERT(_compressed, "Trade::ImageData::type(): the image is not compressed", (CompressedImageView<dimensions>{_compressedFormat, _size}));
    return CompressedImageView<dimensions>{_compressedFormat, _size, _data};
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_EXPORT ImageData<1>;
template class MAGNUM_EXPORT ImageData<2>;
template class MAGNUM_EXPORT ImageData<3>;
#endif

}}
