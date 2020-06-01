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

#include "PixelStorage.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/Math/Vector3.h"

namespace Magnum {

bool PixelStorage::operator==(const PixelStorage& other) const {
    return
        _rowLength == other._rowLength &&
        _imageHeight == other._imageHeight &&
        _skip == other._skip &&
        _alignment == other._alignment;
}

std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> PixelStorage::dataProperties(const std::size_t pixelSize, const Vector3i& size) const {
    const Math::Vector3<std::size_t> dataSize{
        std::size_t((((_rowLength ? _rowLength*pixelSize : size[0]*pixelSize) +
            _alignment - 1)/_alignment)*_alignment),
        std::size_t(_imageHeight ? _imageHeight : size.y()),
        std::size_t(size.z())};

    return {Math::Vector3<std::size_t>{pixelSize, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{_skip},
        size.product() ? dataSize : Math::Vector3<std::size_t>{}};
}

std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> CompressedPixelStorage::dataProperties(const Vector3i& size) const {
    CORRADE_ASSERT(_blockDataSize && _blockSize.product(), "CompressedPixelStorage::dataProperties(): expected non-zero storage parameters", {});

    const Vector3i blockCount = (size + _blockSize - Vector3i{1})/_blockSize;
    const Math::Vector3<std::size_t> dataSize{
        std::size_t(_rowLength ? (_rowLength + _blockSize.x() - 1)/_blockSize.x() : blockCount.x()),
        std::size_t(_imageHeight ? (_imageHeight + _blockSize.y() - 1)/_blockSize.y() : blockCount.y()),
        std::size_t(blockCount.z())};

    const Vector3i skipBlockCount = (_skip + _blockSize - Vector3i{1})/_blockSize;
    const Math::Vector3<std::size_t> offset = (Math::Vector3<std::size_t>{1, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{skipBlockCount})*_blockDataSize;

    return std::make_pair(offset, size.product() ? dataSize : Math::Vector3<std::size_t>{});
}

bool CompressedPixelStorage::operator==(const CompressedPixelStorage& other) const {
    return PixelStorage::operator==(other) &&
        _blockSize == other._blockSize &&
        _blockDataSize == other._blockDataSize;
}

}
