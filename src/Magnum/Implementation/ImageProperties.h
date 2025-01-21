#ifndef Magnum_Implementation_ImageProperties_h
#define Magnum_Implementation_ImageProperties_h
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

#include <utility> /* std::pair */
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#ifndef CORRADE_NO_ASSERT
#include "Magnum/ImageFlags.h"
#endif
#include "Magnum/DimensionTraits.h"

namespace Magnum { namespace Implementation {

/* Used in *Image and Compressed*Image constructors */
#ifndef CORRADE_NO_ASSERT
inline void checkPixelSize(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const UnsignedInt pixelSize)
{
    CORRADE_ASSERT(pixelSize && pixelSize < 256,
        prefix << "expected pixel size to be non-zero and less than 256 but got" << pixelSize, );
}

inline void checkImageFlagsForSize(const char*, const ImageFlags1D, const Math::Vector<1, Int>&) {}
inline void checkImageFlagsForSize(const char*, const ImageFlags2D, const Vector2i&) {}
inline void checkImageFlagsForSize(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const ImageFlags3D flags, const Vector3i& size)
{
    CORRADE_ASSERT(!(flags & ImageFlag3D::CubeMap) || size.x() == size.y(),
        prefix << "expected square faces for a cube map, got" << Debug::packed << size.xy(), );
    CORRADE_ASSERT(!(flags & ImageFlag3D::CubeMap) || (flags & ImageFlag3D::Array) || size.z() == 6,
        prefix << "expected exactly 6 faces for a cube map, got" << size.z(), );
    CORRADE_ASSERT(!(flags >= (ImageFlag3D::CubeMap|ImageFlag3D::Array)) || size.z() % 6 == 0,
        prefix << "expected a multiple of 6 faces for a cube map array, got" << size.z(), );
}
#endif

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

/* Used in image query functions */
template<std::size_t dimensions, class T> std::size_t imageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1));

    /* Smallest line/rectangle/cube that covers the area */
    std::size_t dataOffset = 0;
    if(dataProperties.first.z())
        dataOffset += dataProperties.first.z();
    else if(dataProperties.first.y()) {
        if(!image.storage().imageHeight())
            dataOffset += dataProperties.first.y();
    } else if(dataProperties.first.x()) {
        if(!image.storage().rowLength())
            dataOffset += dataProperties.first.x();
    }
    return dataOffset + dataProperties.second.product();
}

/* Used in data size assertions */
template<class T> inline std::size_t imageDataSize(const T& image) {
    return imageDataSizeFor(image, image.size());
}

template<std::size_t dimensions, class T> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    CORRADE_INTERNAL_ASSERT(image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize());

    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(Vector3i::pad(size, 1));

    const auto realBlockCount = Math::Vector3<std::size_t>{(Vector3i::pad(size, 1) + image.storage().compressedBlockSize() - Vector3i{1})/image.storage().compressedBlockSize()};

    return {dataProperties.first.sum(), (dataProperties.second.product() - (dataProperties.second.x() - realBlockCount.x()) - (dataProperties.second.y() - realBlockCount.y())*dataProperties.second.x())*image.storage().compressedBlockDataSize()};
}

/* Used in image query functions */
template<std::size_t dimensions, class T> std::size_t compressedImageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    auto r = compressedImageDataOffsetSizeFor(image, size);
    return r.first + r.second;
}

/* Use in compressed image upload functions */
template<class T> std::size_t occupiedCompressedImageDataSize(const T& image, std::size_t dataSize) {
    return image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize()
        ? compressedImageDataOffsetSizeFor(image, image.size()).second : dataSize;
}

template<std::size_t dimensions, class T> std::ptrdiff_t pixelStorageSkipOffsetFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    return image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1)).first.sum();
}
template<class T> std::ptrdiff_t pixelStorageSkipOffset(const T& image) {
    return pixelStorageSkipOffsetFor(image, image.size());
}

template<UnsignedInt dimensions, class T, class Image, class Data> Containers::StridedArrayView<dimensions + 1, T> imagePixelView(Image& image, const Data data) {
    const std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> properties = image.dataProperties();

    /* Size in the last dimension is byte size of the pixel, the remaining
       dimensions are reverted (first images, then rows, then pixels, last
       pixel bytes) */
    Containers::Size<dimensions + 1> size{NoInit};
    size[dimensions] = image.pixelSize();
    for(UnsignedInt i = dimensions; i != 0; --i)
        size[i - 1] = image.size()[dimensions - i];

    /* Stride in the last dimension is 1, stride in the second-to-last
       dimension ix pixel byte size. The remaining imensions are reverted
       (first image size, then row size, then pixel size, last 1). The
       data properties include pixel size in row size, so we have to take it
       out from the cumulative product. */
    Containers::Stride<dimensions + 1> stride{NoInit};
    stride[dimensions] = 1;
    stride[dimensions - 1] = 1;
    for(UnsignedInt i = dimensions - 1; i != 0; --i)
        stride[i - 1] = stride[i]*properties.second[dimensions - i - 1];
    stride[dimensions - 1] = image.pixelSize();

    static_assert(sizeof(decltype(image.data().front())) == 1,
        "pointer arithmetic expects image data type to have 1 byte");
    return {data.exceptPrefix(properties.first[dimensions - 1]), data + properties.first.sum(), size, stride};
}

}}

#endif
