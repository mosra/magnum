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

#include "Interleave.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace {

inline std::size_t attributeSize(const Trade::MeshData& data, UnsignedInt i) {
    return vertexFormatSize(data.attributeFormat(i))*Math::max(data.attributeArraySize(i), UnsignedShort{1});
}

inline std::size_t attributeSize(const Trade::MeshAttributeData& data) {
    return vertexFormatSize(data.format())*Math::max(data.arraySize(), UnsignedShort{1});
}
Containers::Optional<Containers::StridedArrayView2D<const char>> interleavedDataInternal(const Trade::MeshData& data) {
    /* There is no attributes, return a zero-sized view to indicate a success */
    if(!data.attributeCount())
        return Containers::StridedArrayView2D<const char>{data.vertexData(), {data.vertexCount(), 0}};

    const UnsignedInt stride = data.attributeStride(0);
    std::size_t minOffset = ~std::size_t{};
    std::size_t maxOffset = 0;
    bool hasImplementationSpecificVertexFormat = false;
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(data.attributeStride(i) != stride) return Containers::NullOpt;

        const std::size_t offset = data.attributeOffset(i);
        minOffset = Math::min(minOffset, offset);

        /* If the attribute has implementation-specific format, remember that
           for later and optimistically use size of 1 byte for calculations */
        std::size_t size;
        if(isVertexFormatImplementationSpecific(data.attributeFormat(i))) {
            hasImplementationSpecificVertexFormat = true;
            size = 1;
        } else size = attributeSize(data, i);

        maxOffset = Math::max(maxOffset, offset + size);
    }

    /* If there's an attribute with implementation-specific format,
       conservatively use the whole stride for it. This should work for
       majority of cases except when the stride has a padding at the end and
       the padding isn't included in the vertexData array for the last vertex,
       but that'd probably blow up in many other cases (and drivers) as well. */
    if(hasImplementationSpecificVertexFormat)
        maxOffset = Math::max(maxOffset, minOffset + stride);

    /* The offsets can't fit into the stride, report failure */
    if(maxOffset - minOffset > stride) return Containers::NullOpt;

    return Containers::StridedArrayView2D<const char>{
        data.vertexData(), data.vertexData().data() + minOffset,
        {data.vertexCount(), maxOffset - minOffset},
        {std::ptrdiff_t(stride), 1}};
}

}

bool isInterleaved(const Trade::MeshData& data) {
    return !!interleavedDataInternal(data);
}

Containers::StridedArrayView2D<const char> interleavedData(const Trade::MeshData& data) {
    auto out = interleavedDataInternal(data);
    CORRADE_ASSERT(out, "MeshTools::interleavedData(): the mesh is not interleaved", {});
    return *out;
}

Containers::StridedArrayView2D<char> interleavedMutableData(Trade::MeshData& data) {
    Containers::StridedArrayView2D<const char> out = interleavedData(data);
    CORRADE_ASSERT(data.vertexDataFlags() & Trade::DataFlag::Mutable,
        "MeshTools::interleavedMutableData(): vertex data is not mutable", {});
    return Containers::StridedArrayView2D<char>{
        {nullptr, ~std::size_t{}}, /* to sidestep the range assertions */
        const_cast<char*>(reinterpret_cast<const char*>(out.data())),
        out.size(), out.stride()};
}

namespace Implementation {

Containers::Array<Trade::MeshAttributeData> interleavedLayout(Trade::MeshData&& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    /* Nothing to do here, bye! */
    if(!data.attributeCount() && extra.empty()) return {};

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
        /** @todo explitily assert on impl-specific vertex formats here --
            however it should work when the original is already interleaved and
            nothing in extras is impl-specific */
        for(UnsignedInt i = 0, max = data.attributeCount(); i != max; ++i)
            stride += attributeSize(data, i);
    }

    /* Add the extra attributes and explicit padding */
    std::size_t extraAttributeCount = 0;
    for(std::size_t i = 0; i != extra.size(); ++i) {
        if(extra[i].format() == VertexFormat{}) {
            CORRADE_ASSERT(extra[i].stride() > 0 || stride >= std::size_t(-extra[i].stride()),
                "MeshTools::interleavedLayout(): negative padding" << extra[i].stride() << "in extra attribute" << i << "too large for stride" << stride, {});
            stride += extra[i].stride();
        } else {
            /** @todo explitily assert on impl-specific vertex formats here */
            stride += attributeSize(extra[i]);
            ++extraAttributeCount;
        }
    }

    /* Transfer the attribute data array. If there are no extra attributes and
       the attribute data array is owned (the array has a default deleter), we
       can take over the ownership and avoid an allocation. Otherwise we
       allocate a new array and copy the prefix over so we can just patch the
       data array later. */
    const UnsignedInt originalAttributeCount = data.attributeCount();
    const UnsignedInt originalAttributeStride = originalAttributeCount ?
        data.attributeStride(0) : 0;
    Containers::Array<Trade::MeshAttributeData> originalAttributeData =
        data.releaseAttributeData();
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(!extraAttributeCount && !originalAttributeData.deleter())
        attributeData = std::move(originalAttributeData);
    else {
        attributeData = Containers::Array<Trade::MeshAttributeData>{originalAttributeCount + extraAttributeCount};
        Utility::copy(originalAttributeData, attributeData.prefix(originalAttributeCount));
    }

    /* Copy existing attribute layout. If the original is already interleaved,
       preserve relative attribute offsets, otherwise pack tightly. */
    std::size_t offset = 0;
    for(UnsignedInt i = 0; i != originalAttributeCount; ++i) {
        if(interleaved) offset = attributeData[i].offset(data.vertexData()) - minOffset;

        attributeData[i] = Trade::MeshAttributeData{
            attributeData[i].name(), attributeData[i].format(),
            offset, 0, std::ptrdiff_t(stride), attributeData[i].arraySize()};

        if(!interleaved) offset += attributeSize(attributeData[i]);
    }

    /* In case the original is already interleaved, set the offset for extra
       attribs to the original stride to preserve also potential padding at the
       end. */
    if(interleaved && originalAttributeCount)
        offset = originalAttributeStride;

    /* Mix in the extra attributes */
    UnsignedInt attributeIndex = originalAttributeCount;
    for(UnsignedInt i = 0; i != extra.size(); ++i) {
        /* Padding, only adjust the offset for next attribute */
        if(extra[i].format() == VertexFormat{}) {
            offset += extra[i].stride();
            continue;
        }

        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            extra[i].name(), extra[i].format(),
            offset, 0, std::ptrdiff_t(stride), extra[i].arraySize()};

        offset += attributeSize(extra[i]);
    }

    return attributeData;
}

}

Trade::MeshData interleavedLayout(Trade::MeshData&& data, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    Containers::Array<Trade::MeshAttributeData> attributeData = Implementation::interleavedLayout(std::move(data), extra);

    /* If there are no attributes, bail -- return an empty mesh with desired
       vertex count but nothing else */
    if(!attributeData)
        return Trade::MeshData{data.primitive(), vertexCount};

    /* Allocate new data array */
    Containers::Array<char> vertexData{Containers::NoInit, attributeData[0].stride()*vertexCount};

    /* Convert the attributes from offset-only and zero vertex count to
       absolute, referencing the above-allocated data array */
    for(Trade::MeshAttributeData& attribute: attributeData) {
        attribute = Trade::MeshAttributeData{
            attribute.name(), attribute.format(),
            Containers::StridedArrayView1D<void>{vertexData,
                vertexData + attribute.offset(vertexData),
                vertexCount, attribute.stride()},
            attribute.arraySize()};
    }

    return Trade::MeshData{data.primitive(), std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData interleavedLayout(Trade::MeshData&& data, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleavedLayout(std::move(data), vertexCount, Containers::arrayView(extra));
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    return interleavedLayout(
        Trade::MeshData{data.primitive(), {}, data.vertexData(),
            Trade::meshAttributeDataNonOwningArray(data.attributeData()),
            data.vertexCount()},
        vertexCount, extra);
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleavedLayout(data, vertexCount, Containers::arrayView(extra));
}

Trade::MeshData interleave(Trade::MeshData&& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    /* Transfer the indices unchanged, in case the mesh is indexed */
    Containers::Array<char> indexData;
    Trade::MeshIndexData indices;
    if(data.isIndexed()) {
        /* If we can steal the data, do it */
        if(data.indexDataFlags() & Trade::DataFlag::Owned) {
            indices = Trade::MeshIndexData{data.indices()};
            indexData = data.releaseIndexData();
        } else {
            indexData = Containers::Array<char>{data.indexData().size()};
            Utility::copy(data.indexData(), indexData);
            indices = Trade::MeshIndexData{data.indexType(),
                Containers::ArrayView<const void>{indexData + data.indexOffset(), data.indices().size()[0]*data.indices().size()[1]}};
        }
    }

    const bool interleaved = isInterleaved(data);
    const UnsignedInt vertexCount = data.vertexCount();

    /* If the mesh is already interleaved and we don't have anything extra,
       steal that data as well */
    Containers::Array<char> vertexData;
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(interleaved && extra.empty() && (data.vertexDataFlags() & Trade::DataFlag::Owned)) {
        attributeData = data.releaseAttributeData();
        vertexData = data.releaseVertexData();

    /* Otherwise do it the hard way */
    } else {
        /* Calculate the layout */
        Trade::MeshData layout = interleavedLayout(data, vertexCount, extra);

        /* Copy existing attributes to new locations */
        for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
            Utility::copy(data.attribute(i), layout.mutableAttribute(i));

        /* Mix in the extra attributes */
        UnsignedInt attributeIndex = data.attributeCount();
        for(UnsignedInt i = 0; i != extra.size(); ++i) {
            /* Padding, ignore */
            if(extra[i].format() == VertexFormat{}) continue;

            /* Asserting here even though data() has another assert since that
               one would be too confusing in this context */
            CORRADE_ASSERT(!extra[i].isOffsetOnly(),
                "MeshTools::interleave(): extra attribute" << i << "is offset-only, which is not supported",
                (Trade::MeshData{MeshPrimitive::Triangles, 0}));

            /* Copy the attribute in, if it is non-empty, otherwise keep the
               memory uninitialized */
            if(extra[i].data()) {
                CORRADE_ASSERT(extra[i].data().size() == vertexCount,
                    "MeshTools::interleave(): extra attribute" << i << "expected to have" << vertexCount << "items but got" << extra[i].data().size(),
                    (Trade::MeshData{MeshPrimitive::Triangles, 0}));
                const Containers::StridedArrayView2D<const char> attribute =
                    Containers::arrayCast<2, const char>(extra[i].data(), vertexFormatSize(extra[i].format()));
                Utility::copy(attribute, layout.mutableAttribute(attributeIndex));
            }

            ++attributeIndex;
        }

        /* Release the data from the layout to pack them into the output */
        vertexData = layout.releaseVertexData();
        attributeData = layout.releaseAttributeData();
    }

    return Trade::MeshData{data.primitive(), std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData), vertexCount};
}

Trade::MeshData interleave(Trade::MeshData&& data, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleave(std::move(data), Containers::arrayView(extra));
}

Trade::MeshData interleave(const Trade::MeshData& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    return interleave(Trade::MeshData{data.primitive(),
        /* If data is not indexed, the reference will be also non-indexed */
        {}, data.indexData(), Trade::MeshIndexData{data.indices()},
        {}, data.vertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData()),
        data.vertexCount()
    }, extra);
}

Trade::MeshData interleave(const Trade::MeshData& data, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleave(std::move(data), Containers::arrayView(extra));
}

}}
