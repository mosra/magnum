#ifndef Magnum_Implementation_ImageProperties_h
#define Magnum_Implementation_ImageProperties_h
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

#include <utility>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/DimensionTraits.h"

namespace Magnum { namespace Implementation {

/* Used in *Image::dataProperties() */
template<std::size_t dimensions, class T> std::pair<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>> imageDataProperties(const T& image) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(image.pixelSize(), Vector3i::pad(image.size(), 1));
    return std::make_pair(Math::Vector<dimensions, std::size_t>::pad(dataProperties.first), Math::Vector<dimensions, std::size_t>::pad(dataProperties.second));
}

/* Used in Compressed*Image::dataProperties() */
template<std::size_t dimensions, class T> std::pair<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>> compressedImageDataProperties(const T& image) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(Vector3i::pad(image.size(), 1));
    return std::make_pair(Math::Vector<dimensions, std::size_t>::pad(dataProperties.first), Math::Vector<dimensions, std::size_t>::pad(dataProperties.second));
}

template<UnsignedInt dimensions, class T, class Image, class Data> Containers::StridedArrayView<dimensions + 1, T> imagePixelView(Image& image, const Data data) {
    const std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> properties = image.dataProperties();

    /* Size in the last dimension is byte size of the pixel, the remaining
       dimensions are reverted (first images, then rows, then pixels, last
       pixel bytes) */
    Containers::StridedDimensions<dimensions + 1, std::size_t> size{Containers::NoInit};
    size[dimensions] = image.pixelSize();
    for(UnsignedInt i = dimensions; i != 0; --i)
        size[i - 1] = image.size()[dimensions - i];

    /* Stride in the last dimension is 1, stride in the second-to-last
       dimension ix pixel byte size. The remaining imensions are reverted
       (first image size, then row size, then pixel size, last 1). The
       data properties include pixel size in row size, so we have to take it
       out from the cumulative product. */
    Containers::StridedDimensions<dimensions + 1, std::ptrdiff_t> stride{Containers::NoInit};
    stride[dimensions] = 1;
    stride[dimensions - 1] = 1;
    for(UnsignedInt i = dimensions - 1; i != 0; --i)
        stride[i - 1] = stride[i]*properties.second[dimensions - i - 1];
    stride[dimensions - 1] = image.pixelSize();

    static_assert(sizeof(decltype(image.data().front())) == 1,
        "pointer arithmetic expects image data type to have 1 byte");
    return {data.suffix(properties.first[dimensions - 1]), data + properties.first.sum(), size, stride};
}

}}

#endif
