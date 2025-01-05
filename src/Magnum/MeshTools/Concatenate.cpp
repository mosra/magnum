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

#include "Concatenate.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/MeshTools/Implementation/remapAttributeData.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

Containers::Pair<UnsignedInt, UnsignedInt> concatenateIndexVertexCount(const Containers::Iterable<const Trade::MeshData>& meshes) {
    UnsignedInt indexCount = 0;
    UnsignedInt vertexCount = 0;
    for(const Trade::MeshData& mesh: meshes) {
        /* If the mesh is indexed, add to index count. If this is the first
           indexed mesh, all previous meshes will have a trivial index buffer
           generated for all their vertices */
        if(mesh.isIndexed()) {
            if(!indexCount) indexCount = vertexCount;
            indexCount += mesh.indexCount();

        /* Otherwise, if some earlier mesh was indexed, this mesh will have a
           trivial index buffer generated for all its vertices */
        } else if(indexCount) indexCount += mesh.vertexCount();

        vertexCount += mesh.vertexCount();
    }

    return {indexCount, vertexCount};
}

Trade::MeshData concatenate(Containers::Array<char>&& indexData, const UnsignedInt vertexCount, Containers::Array<char>&& vertexData, Containers::Array<Trade::MeshAttributeData>&& attributeData, const Containers::Iterable<const Trade::MeshData>& meshes, const char* const assertPrefix) {
    #if defined(CORRADE_NO_ASSERT) || defined(CORRADE_STANDARD_ASSERT)
    static_cast<void>(assertPrefix);
    #endif

    /* Convert the attributes from offset-only and zero vertex count to
       absolute, referencing the vertex data array */
    for(Trade::MeshAttributeData& attribute: attributeData)
        attribute = Implementation::remapAttributeData(attribute, vertexCount, vertexData, vertexData);

    /* Only list primitives are supported currently */
    /** @todo delegate to generateIndices() for these */
    CORRADE_ASSERT(
        meshes.front().primitive() != MeshPrimitive::LineStrip &&
        meshes.front().primitive() != MeshPrimitive::LineLoop &&
        meshes.front().primitive() != MeshPrimitive::TriangleStrip &&
        meshes.front().primitive() != MeshPrimitive::TriangleFan,
        assertPrefix << meshes.front().primitive() << "is not supported, turn it into a plain indexed mesh first",
        (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Populate the resulting instance with what we have. It'll be used below
       for convenient access to vertex / index data */
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    Trade::MeshData out{meshes.front().primitive(),
        /* If the index array is empty, we're creating a non-indexed mesh (not
           an indexed mesh with zero indices) */
        Utility::move(indexData), indices.isEmpty() ?
            Trade::MeshIndexData{} : Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributeData), vertexCount};

    /* Go through all meshes and put all attributes and index arrays
       together */
    std::size_t indexOffset = 0;
    std::size_t vertexOffset = 0;
    for(std::size_t i = 0; i != meshes.size(); ++i) {
        const Trade::MeshData& mesh = meshes[i];

        /* This won't fire for i == ~std::size_t{}, as that's where
           out.primitive() comes from */
        CORRADE_ASSERT(mesh.primitive() == out.primitive(),
            assertPrefix << "expected" << out.primitive() << "but got" << mesh.primitive() << "in mesh" << i,
            (Trade::MeshData{MeshPrimitive{}, 0}));

        /* If the mesh is indexed, copy the indices over, expanded to 32bit */
        if(mesh.isIndexed()) {
            CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(mesh.indexType()),
                assertPrefix << "mesh" << i << "has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
                (Trade::MeshData{MeshPrimitive{}, 0}));

            Containers::ArrayView<UnsignedInt> dst = indices.slice(indexOffset, indexOffset + mesh.indexCount());
            mesh.indicesInto(dst);
            indexOffset += mesh.indexCount();

            /* Adjust indices for current vertex offset */
            for(UnsignedInt& index: dst) index += vertexOffset;

        /* Otherwise, if we need an index buffer (meaning at least one of the
           meshes is indexed), generate a trivial index buffer */
        } else if(!indices.isEmpty()) {
            MeshTools::generateTrivialIndicesInto(indices.sliceSize(indexOffset, mesh.vertexCount()), vertexOffset);
            indexOffset += mesh.vertexCount();
        }

        /* Copy attributes to their destination, skipping ones that don't have
           any equivalent in the destination mesh */
        for(UnsignedInt src = 0; src != mesh.attributeCount(); ++src) {
            /* Try to find a matching attribute in the destination mesh (same
               name, same set, same morph target ID). Skip if no such attribute
               is found. This is a O(m + n) complexity (linear lookup in both
               the source and the output mesh), but given the assumption that
               meshes rarely have more than 8-16 attributes it should still be
               faster than building a hashmap first and then doing a complex
               lookup in it (which is how it used to be before, using
               std::unordered_multimap). */
            const Containers::Optional<UnsignedInt> dst = out.findAttributeId(mesh.attributeName(src), mesh.attributeId(src), mesh.attributeMorphTargetId(src));
            if(!dst)
                continue;

            /* Check format compatibility. This won't fire for i == 0, as
               that's where out.primitive() comes from */
            CORRADE_ASSERT(out.attributeFormat(*dst) == mesh.attributeFormat(src),
                assertPrefix << "expected" << out.attributeFormat(*dst) << "for attribute" << dst << "(" << Debug::nospace << out.attributeName(*dst) << Debug::nospace << ") but got" << mesh.attributeFormat(src) << "in mesh" << i << "attribute" << src,
                (Trade::MeshData{MeshPrimitive{}, 0}));
            CORRADE_ASSERT(!out.attributeArraySize(*dst) == !mesh.attributeArraySize(src),
                assertPrefix << "attribute" << dst << "(" << Debug::nospace << out.attributeName(*dst) << Debug::nospace << ")" << (out.attributeArraySize(*dst) ? "is" : "isn't") << "an array but attribute" << src << "in mesh" << i << (mesh.attributeArraySize(src) ? "is" : "isn't"),
                (Trade::MeshData{MeshPrimitive{}, 0}));
            CORRADE_ASSERT(out.attributeArraySize(*dst) >= mesh.attributeArraySize(src),
                assertPrefix << "expected array size" << out.attributeArraySize(*dst) << "or less for attribute" << dst << "(" << Debug::nospace << out.attributeName(*dst) << Debug::nospace << ") but got" << mesh.attributeArraySize(src) << "in mesh" << i << "attribute" << src,
                (Trade::MeshData{MeshPrimitive{}, 0}));

            const Containers::StridedArrayView2D<const char> srcAttribute = mesh.attribute(src);
            const Containers::StridedArrayView2D<char> dstAttribute = out.mutableAttribute(*dst);

            /* Copy the data to a slice of the output. For non-array attributes
               the second dimension should be matching (because the format is
               matching), for array attributes we may be copying to just a
               prefix of the elements in dstAttribute. */
            CORRADE_INTERNAL_ASSERT(out.attributeArraySize(*dst) || srcAttribute.size()[1] == dstAttribute.size()[1]);
            Utility::copy(srcAttribute, dstAttribute.sliceSize(
                {vertexOffset, 0},
                {mesh.vertexCount(), srcAttribute.size()[1]}));
        }

        /* Update vertex offset for the next mesh */
        vertexOffset += mesh.vertexCount();
    }

    return out;
}

}

Trade::MeshData concatenate(const Containers::Iterable<const Trade::MeshData>& meshes, const InterleaveFlags flags) {
    CORRADE_ASSERT(!meshes.isEmpty(),
        "MeshTools::concatenate(): expected at least one mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != meshes.front().attributeCount(); ++i) {
        const VertexFormat format = meshes.front().attributeFormat(i);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
            "MeshTools::concatenate(): attribute" << i << "of the first mesh has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(format),
            (Trade::MeshData{MeshPrimitive::Points, 0}));
    }
    #endif

    /* Calculate final attribute stride and offsets. Make a non-owning copy of
       the attribute data to avoid interleavedLayout() stealing the original
       (we still need it to be able to reference the original data). If there's
       no attributes in the original array, pass just vertex count ---
       otherwise MeshData will assert on that to avoid it getting lost. */
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(meshes.front().attributeCount())
        attributeData = Implementation::interleavedLayout(Trade::MeshData{meshes.front().primitive(),
            {}, meshes.front().vertexData(),
            Trade::meshAttributeDataNonOwningArray(meshes.front().attributeData())}, {}, flags);
    else attributeData =
        Implementation::interleavedLayout(Trade::MeshData{meshes.front().primitive(),
            meshes.front().vertexCount()}, {}, flags);

    /* Calculate total index/vertex count and allocate the target memory.
       Index data are allocated with NoInit as the whole array will be written,
       however vertex data might have holes and thus it's zero-initialized. */
    const Containers::Pair<UnsignedInt, UnsignedInt> indexVertexCount = Implementation::concatenateIndexVertexCount(meshes);
    Containers::Array<char> indexData{NoInit,
        indexVertexCount.first()*sizeof(UnsignedInt)};
    Containers::Array<char> vertexData{ValueInit,
        attributeData.isEmpty() ? 0 : (attributeData[0].stride()*indexVertexCount.second())};
    return Implementation::concatenate(Utility::move(indexData), indexVertexCount.second(), Utility::move(vertexData), Utility::move(attributeData), meshes, "MeshTools::concatenate():");
}

}}
