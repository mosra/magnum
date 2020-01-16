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

#include "Interleave.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

bool isInterleaved(const Trade::MeshData& data) {
    /* There is nothing, so yes it is (because there is nothing we could do
       to make it interleaved anyway) */
    if(!data.attributeCount()) return true;

    const UnsignedInt stride = data.attributeStride(0);
    std::size_t minOffset = data.attributeOffset(0);
    std::size_t maxOffset = minOffset;
    for(UnsignedInt i = 1; i != data.attributeCount(); ++i) {
        if(data.attributeStride(i) != stride) return false;

        const std::size_t offset = data.attributeOffset(i);
        minOffset = Math::min(minOffset, offset);
        maxOffset = Math::max(maxOffset, offset + vertexFormatSize(data.attributeFormat(i)));
    }

    return maxOffset - minOffset <= stride;
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    /* If there are no attributes, bail -- return an empty mesh with desired
       vertex count but nothing else */
    if(!data.attributeCount() && extra.empty())
        return Trade::MeshData{data.primitive(), vertexCount};

    const bool interleaved = isInterleaved(data);

    /* If the mesh is already interleaved, use the original stride to
       preserve all padding, but remove the initial offset. Otherwise calculate
       a tightly-packed stride. */
    std::size_t stride;
    std::size_t minOffset;
    if(interleaved && data.attributeCount()) {
        stride = data.attributeStride(0);
        minOffset = ~std::size_t{};
        for(UnsignedInt i = 0, max = data.attributeCount(); i != max; ++i)
            minOffset = Math::min(minOffset, data.attributeOffset(i));
    } else {
        stride = 0;
        minOffset = 0;
        for(UnsignedInt i = 0, max = data.attributeCount(); i != max; ++i)
            stride += vertexFormatSize(data.attributeFormat(i));
    }

    /* Add the extra attributes and explicit padding */
    std::size_t extraAttributeCount = 0;
    for(std::size_t i = 0; i != extra.size(); ++i) {
        if(extra[i].format() == VertexFormat{}) {
            CORRADE_ASSERT(extra[i].data().stride() > 0 || stride >= std::size_t(-extra[i].data().stride()),
                "MeshTools::interleavedLayout(): negative padding" << extra[i].data().stride() << "in extra attribute" << i << "too large for stride" << stride, (Trade::MeshData{MeshPrimitive::Points, 0}));
            stride += extra[i].data().stride();
        } else {
            stride += vertexFormatSize(extra[i].format());
            ++extraAttributeCount;
        }
    }

    /* Allocate new data and attribute array */
    Containers::Array<char> vertexData{Containers::NoInit, stride*vertexCount};
    Containers::Array<Trade::MeshAttributeData> attributeData{data.attributeCount() + extraAttributeCount};

    /* Copy existing attribute layout. If the original is already interleaved,
       preserve relative attribute offsets, otherwise pack tightly. */
    std::size_t offset = 0;
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(interleaved) offset = data.attributeOffset(i) - minOffset;

        attributeData[i] = Trade::MeshAttributeData{
            data.attributeName(i), data.attributeFormat(i),
            Containers::StridedArrayView1D<void>{vertexData, vertexData + offset,
            vertexCount, std::ptrdiff_t(stride)}};

        if(!interleaved) offset += vertexFormatSize(data.attributeFormat(i));
    }

    /* In case the original is already interleaved, set the offset for extra
       attribs to the original stride to preserve also potential padding at the
       end. */
    if(interleaved && data.attributeCount())
        offset = data.attributeStride(0);

    /* Mix in the extra attributes */
    UnsignedInt attributeIndex = data.attributeCount();
    for(UnsignedInt i = 0; i != extra.size(); ++i) {
        /* Padding, only adjust the offset for next attribute */
        if(extra[i].format() == VertexFormat{}) {
            offset += extra[i].data().stride();
            continue;
        }

        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            extra[i].name(), extra[i].format(), Containers::StridedArrayView1D<void>{vertexData, vertexData + offset,
            vertexCount, std::ptrdiff_t(stride)}};

        offset += vertexFormatSize(extra[i].format());
    }

    return Trade::MeshData{data.primitive(), std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleavedLayout(data, vertexCount, Containers::arrayView(extra));
}

}}
