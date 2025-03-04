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

#include "Interleave.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Implementation/remapAttributeData.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace {

inline std::size_t attributeSize(const Trade::MeshData& mesh, UnsignedInt i) {
    return vertexFormatSize(mesh.attributeFormat(i))*Math::max(mesh.attributeArraySize(i), UnsignedShort{1});
}

inline std::size_t attributeSize(const Trade::MeshAttributeData& mesh) {
    return vertexFormatSize(mesh.format())*Math::max(mesh.arraySize(), UnsignedShort{1});
}
Containers::Optional<Containers::StridedArrayView2D<const char>> interleavedDataInternal(const Trade::MeshData& mesh) {
    /* There is no attributes, return a zero-sized view to indicate a success */
    if(!mesh.attributeCount())
        return Containers::StridedArrayView2D<const char>{mesh.vertexData(), {mesh.vertexCount(), 0}};

    /* Technically zero and negative strides *may* also be categorized as
       interleaved if they are all the same, but it causes way too many
       problems especially when used within interleavedLayout() etc. May
       tackle properly later. */
    const Int stride = mesh.attributeStride(0);
    if(stride <= 0)
        return Containers::NullOpt;

    std::size_t minOffset = ~std::size_t{};
    std::size_t maxOffset = 0;
    bool hasImplementationSpecificVertexFormat = false;
    for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
        if(mesh.attributeStride(i) != stride) return Containers::NullOpt;

        const std::size_t offset = mesh.attributeOffset(i);
        minOffset = Math::min(minOffset, offset);

        /* If the attribute has implementation-specific format, remember that
           for later and optimistically use size of 1 byte for calculations */
        std::size_t size;
        if(isVertexFormatImplementationSpecific(mesh.attributeFormat(i))) {
            hasImplementationSpecificVertexFormat = true;
            size = 1;
        } else size = attributeSize(mesh, i);

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
    if(maxOffset - minOffset > UnsignedInt(stride))
        return Containers::NullOpt;

    return Containers::StridedArrayView2D<const char>{
        /* MeshData only requires the vertex data to be large enough to fit the
           actual data, not to have the size large enough to fit `count*stride`
           elements. The StridedArrayView expects the latter, so this extends
           the size to satisfy the assert. For simplicity it overextends by the
           whole stride instead of just max end offset, relying on the input
           MeshData having checked the bounds already. To be clear, the output
           is *never* out of bounds of the vertex data, the second dimension is
           always sized to fit within. This is verified in the
           interleavedDataGapsTrailingOmitted() test.

           Additionally, the max() is here because some algorithms such as
           combineFaceAttributes() pass `{nullptr, ~std::size_t{}}` as
           vertexData and without the max() it would overflow. That case is
           tested in interleavedDataVertexDataWholeMemory(). */
        /** @todo maybe just fix StridedArrayView to require only what's really
            needed? */
        {mesh.vertexData().data(), Math::max(mesh.vertexData().size(), mesh.vertexData().size() + stride)},
        mesh.vertexData().data() + minOffset,
        {mesh.vertexCount(), maxOffset - minOffset},
        {std::ptrdiff_t(stride), 1}};
}

}

bool isInterleaved(const Trade::MeshData& mesh) {
    return !!interleavedDataInternal(mesh);
}

Containers::StridedArrayView2D<const char> interleavedData(const Trade::MeshData& mesh) {
    auto out = interleavedDataInternal(mesh);
    CORRADE_ASSERT(out, "MeshTools::interleavedData(): the mesh is not interleaved", {});
    return *out;
}

Containers::StridedArrayView2D<char> interleavedMutableData(Trade::MeshData& mesh) {
    Containers::StridedArrayView2D<const char> out = interleavedData(mesh);
    CORRADE_ASSERT(mesh.vertexDataFlags() & Trade::DataFlag::Mutable,
        "MeshTools::interleavedMutableData(): vertex data is not mutable", {});
    return Containers::StridedArrayView2D<char>{
        {nullptr, ~std::size_t{}}, /* to sidestep the range assertions */
        const_cast<char*>(reinterpret_cast<const char*>(out.data())),
        out.size(), out.stride()};
}

namespace Implementation {

Containers::Array<Trade::MeshAttributeData> interleavedLayout(Trade::MeshData&& mesh, const Containers::ArrayView<const Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    /* Nothing to do here, bye! */
    if(!mesh.attributeCount() && extra.isEmpty()) return {};

    /* If we're not told to preserve the layout, treat the mesh as
       noninterleaved always, forcing a repack. Otherwise check if it's already
       interleaved. */
    const bool interleaved = flags >= InterleaveFlag::PreserveInterleavedAttributes && isInterleaved(mesh);

    /* If the mesh is already interleaved, use the original stride to
       preserve all padding, but remove the initial offset. Otherwise calculate
       a tightly-packed stride. */
    std::size_t stride;
    std::size_t minOffset;
    if(interleaved && mesh.attributeCount()) {
        stride = mesh.attributeStride(0);
        minOffset = ~std::size_t{};
        for(UnsignedInt i = 0, max = mesh.attributeCount(); i != max; ++i)
            minOffset = Math::min(minOffset, mesh.attributeOffset(i));
    } else {
        stride = 0;
        minOffset = 0;
        for(UnsignedInt i = 0, max = mesh.attributeCount(); i != max; ++i) {
            CORRADE_ASSERT(!isVertexFormatImplementationSpecific(mesh.attributeFormat(i)),
                "MeshTools::interleavedLayout(): attribute" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(mesh.attributeFormat(i)), {});
            stride += attributeSize(mesh, i);
        }
    }

    /* Add the extra attributes and explicit padding */
    std::size_t extraAttributeCount = 0;
    for(std::size_t i = 0; i != extra.size(); ++i) {
        const VertexFormat format = extra[i].format();
        if(format == VertexFormat{}) {
            CORRADE_ASSERT(extra[i].stride() > 0 || stride >= std::size_t(-extra[i].stride()),
                "MeshTools::interleavedLayout(): negative padding" << extra[i].stride() << "in extra attribute" << i << "too large for stride" << stride, {});
            stride += extra[i].stride();
        } else {
            CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
                "MeshTools::interleavedLayout(): extra attribute" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(format), {});
            stride += attributeSize(extra[i]);
            ++extraAttributeCount;
        }
    }

    /* Transfer the attribute data array. If there are no extra attributes and
       the attribute data array is owned (the array has a default deleter), we
       can take over the ownership and avoid an allocation. Otherwise we
       allocate a new array and copy the prefix over so we can just patch the
       data array later. */
    const UnsignedInt originalAttributeCount = mesh.attributeCount();
    const UnsignedInt originalAttributeStride = originalAttributeCount ?
        mesh.attributeStride(0) : 0;
    Containers::Array<Trade::MeshAttributeData> originalAttributeData =
        mesh.releaseAttributeData();
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(!extraAttributeCount && !originalAttributeData.deleter())
        attributeData = Utility::move(originalAttributeData);
    else {
        attributeData = Containers::Array<Trade::MeshAttributeData>{originalAttributeCount + extraAttributeCount};
        Utility::copy(originalAttributeData, attributeData.prefix(originalAttributeCount));
    }

    /* Copy existing attribute layout. If the original is already interleaved,
       preserve relative attribute offsets, otherwise pack tightly. */
    std::size_t offset = 0;
    for(UnsignedInt i = 0; i != originalAttributeCount; ++i) {
        if(interleaved) offset = attributeData[i].offset(mesh.vertexData()) - minOffset;

        attributeData[i] = Trade::MeshAttributeData{
            attributeData[i].name(), attributeData[i].format(),
            offset, 0, std::ptrdiff_t(stride),
            attributeData[i].arraySize(), attributeData[i].morphTargetId()};

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
            offset, 0, std::ptrdiff_t(stride),
            extra[i].arraySize(), extra[i].morphTargetId()};

        offset += attributeSize(extra[i]);
    }

    return attributeData;
}

}

Trade::MeshData interleavedLayout(Trade::MeshData&& mesh, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    Containers::Array<Trade::MeshAttributeData> attributeData = Implementation::interleavedLayout(Utility::move(mesh), extra, flags);

    /* If there are no attributes, bail -- return an empty mesh with desired
       vertex count but nothing else */
    if(!attributeData)
        return Trade::MeshData{mesh.primitive(), vertexCount};

    /* Allocate new data array */
    Containers::Array<char> vertexData{NoInit, attributeData[0].stride()*vertexCount};

    /* Convert the attributes from all being offset-only and zero vertex count
       to absolute, referencing the above-allocated data array */
    for(Trade::MeshAttributeData& attribute: attributeData) {
        CORRADE_INTERNAL_ASSERT(attribute.isOffsetOnly());
        attribute = Implementation::remapAttributeData(attribute, vertexCount, vertexData, vertexData);
    }

    return Trade::MeshData{mesh.primitive(), Utility::move(vertexData), Utility::move(attributeData)};
}

Trade::MeshData interleavedLayout(Trade::MeshData&& mesh, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    return interleavedLayout(Utility::move(mesh), vertexCount, Containers::arrayView(extra), flags);
}

Trade::MeshData interleavedLayout(const Trade::MeshData& mesh, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return interleavedLayout(reference(mesh), vertexCount, extra, flags);
}

Trade::MeshData interleavedLayout(const Trade::MeshData& mesh, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    return interleavedLayout(mesh, vertexCount, Containers::arrayView(extra), flags);
}

Trade::MeshData interleave(Trade::MeshData&& mesh, const Containers::ArrayView<const Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    /* Transfer the indices unchanged, in case the mesh is indexed */
    Containers::Array<char> indexData;
    Trade::MeshIndexData indices;
    if(mesh.isIndexed()) {
        const MeshIndexType indexType = mesh.indexType();

        /* If we can steal the data and we're allowed to preserve a strided
           layout or it's tightly packed, do the steal */
        if((mesh.indexDataFlags() & Trade::DataFlag::Owned) && ((flags & InterleaveFlag::PreserveStridedIndices) || (!isMeshIndexTypeImplementationSpecific(indexType) && mesh.indexStride() == Int(meshIndexTypeSize(indexType))))) {
            indices = Trade::MeshIndexData{indexType,
                Containers::StridedArrayView1D<const void>{
                    mesh.indexData(),
                    mesh.indexData().data() + mesh.indexOffset(),
                    mesh.indexCount(),
                    mesh.indexStride()}};
            indexData = mesh.releaseIndexData();

        /* Otherwise, if we can't steal the data but we're told to preserve
           strided indices, make a full copy including any extra offsets and
           paddings */
        } else if(flags & InterleaveFlag::PreserveStridedIndices) {
            indexData = Containers::Array<char>{NoInit, mesh.indexData().size()};
            indices = Trade::MeshIndexData{indexType,
                Containers::StridedArrayView1D<const void>{
                    indexData,
                    indexData.data() + mesh.indexOffset(),
                    mesh.indexCount(),
                    mesh.indexStride()}};
            Utility::copy(mesh.indexData(), indexData);

        /* Otherwise, make a tightly packed copy, in which case we can't have
           an implementation-specific index type */
        } else {
            CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(indexType),
                "MeshTools::interleave(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(indexType) << Debug::nospace << ", enable MeshTools::InterleaveFlag::PreserveStridedIndices to pass the array through unchanged",
                (Trade::MeshData{MeshPrimitive{}, 0}));

            const std::size_t indexTypeSize = meshIndexTypeSize(indexType);
            indexData = Containers::Array<char>{NoInit, mesh.indexCount()*indexTypeSize};
            Containers::StridedArrayView2D<char> out{indexData,
                {mesh.indexCount(), indexTypeSize},
                {std::ptrdiff_t(indexTypeSize), 1}};
            indices = Trade::MeshIndexData{out};
            Utility::copy(mesh.indices(), out);
        }
    }

    /* If we're not told to preserve the layout, treat the mesh as
       noninterleaved always, forcing a repack. Otherwise check if it's already
       interleaved. */
    const bool interleaved = flags >= InterleaveFlag::PreserveInterleavedAttributes && isInterleaved(mesh);
    const UnsignedInt vertexCount = mesh.vertexCount();

    /* If the mesh is already interleaved and we don't have anything extra,
       steal that data as well */
    Containers::Array<char> vertexData;
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(interleaved && extra.isEmpty() && (mesh.vertexDataFlags() & Trade::DataFlag::Owned)) {
        attributeData = mesh.releaseAttributeData();
        vertexData = mesh.releaseVertexData();

    /* Otherwise do it the hard way */
    } else {
        /* Calculate the layout. Can't Utility::move() the data in to avoid
           copying the attribute array as we need the original attributes
           below. */
        Trade::MeshData layout = interleavedLayout(mesh, vertexCount, extra, flags);
        #ifdef CORRADE_GRACEFUL_ASSERT
        /* If interleavedLayout() gracefully asserted and returned no
           attributes (but the original had some), exit right away to not blow
           up on something else later. Sorry, yes, this is shitty. */
        if(!layout.attributeCount() && (mesh.attributeCount() || extra.size()))
            return Trade::MeshData{MeshPrimitive::Points, 0};
        #endif

        /* Copy existing attributes to new locations */
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i)
            Utility::copy(mesh.attribute(i), layout.mutableAttribute(i));

        /* Mix in the extra attributes */
        UnsignedInt attributeIndex = mesh.attributeCount();
        for(UnsignedInt i = 0; i != extra.size(); ++i) {
            /* Padding, ignore */
            if(extra[i].format() == VertexFormat{}) continue;

            /* Asserting here even though data() has another assert since that
               one would be too confusing in this context */
            CORRADE_ASSERT(!extra[i].isOffsetOnly(),
                "MeshTools::interleave(): extra attribute" << i << "is offset-only",
                (Trade::MeshData{MeshPrimitive::Triangles, 0}));

            /* Copy the attribute in, if it is non-null, otherwise keep the
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

    return Trade::MeshData{mesh.primitive(), Utility::move(indexData), indices,
        Utility::move(vertexData), Utility::move(attributeData), vertexCount};
}

Trade::MeshData interleave(Trade::MeshData&& mesh, const std::initializer_list<Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    return interleave(Utility::move(mesh), Containers::arrayView(extra), flags);
}

Trade::MeshData interleave(const Trade::MeshData& mesh, const Containers::ArrayView<const Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return interleave(reference(mesh), extra, flags);
}

Trade::MeshData interleave(const Trade::MeshData& mesh, const std::initializer_list<Trade::MeshAttributeData> extra, const InterleaveFlags flags) {
    return interleave(Utility::move(mesh), Containers::arrayView(extra), flags);
}

Trade::MeshData interleave(const MeshPrimitive primitive, const Trade::MeshIndexData& indices, const Containers::ArrayView<const Trade::MeshAttributeData> attributes) {
    /* Get vertex count from the first non-padding attribute. Checking that all
       arrays have the same size etc is done in the delegated-to function. */
    UnsignedInt vertexCount = ~UnsignedInt{};
    for(const Trade::MeshAttributeData& attribute: attributes) {
        if(attribute.format() != VertexFormat{}) {
            vertexCount = attribute.data().size();
            break;
        }
    }
    CORRADE_ASSERT(vertexCount != ~UnsignedInt{},
        "MeshTools::interleave(): only padding found among" << attributes.size() << "attributes, can't infer vertex count",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Check that indices aren't implementation-specific. The assert inside the
       delegated-to interleave() suggests PreserveStridedIndices, which would
       be confusing as here it's no such argument */
    CORRADE_ASSERT(indices.type() == MeshIndexType{} || !isMeshIndexTypeImplementationSpecific(indices.type()),
        "MeshTools::interleave(): implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(indices.type()),
        (Trade::MeshData{MeshPrimitive{}, 0}));

    return interleave(Trade::MeshData{primitive,
        /* Pass indices as non-owned so they get copied. We can say the index
           data is the whole memory as it's not going to get used because the
           indices get tightly packed. */
        {},
        indices.type() == MeshIndexType{} ?
            nullptr : Containers::ArrayView<char>{nullptr, ~std::size_t{}},
        indices,
        vertexCount},
        attributes,
        /* Explicitly *not* PreserveStridedIndices to ensure the indices get
           tightly packed */
        InterleaveFlags{});
}

Trade::MeshData interleave(const MeshPrimitive primitive, const Trade::MeshIndexData& indices, const std::initializer_list<Trade::MeshAttributeData> attributes) {
    return interleave(primitive, indices, Containers::arrayView(attributes));
}

Trade::MeshData interleave(const MeshPrimitive primitive, const Containers::ArrayView<const Trade::MeshAttributeData> attributes) {
    return interleave(primitive, Trade::MeshIndexData{}, attributes);
}

Trade::MeshData interleave(const MeshPrimitive primitive, const std::initializer_list<Trade::MeshAttributeData> attributes) {
    return interleave(primitive, Containers::arrayView(attributes));
}

}}
