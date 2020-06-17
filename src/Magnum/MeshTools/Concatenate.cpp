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

#include "Concatenate.h"

#include <numeric>
#include <unordered_map>
#include <Corrade/Utility/Algorithms.h>

namespace Magnum { namespace MeshTools {

namespace Implementation {

std::pair<UnsignedInt, UnsignedInt> concatenateIndexVertexCount(Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes) {
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

/* std::hash for enumeration types is only since C++14, so we need to make our
   own. It's amazing how extremely verbose this can get, ugh. */
struct MeshAttributeHash: std::hash<typename std::underlying_type<Trade::MeshAttribute>::type> {
    std::size_t operator()(Trade::MeshAttribute value) const {
        return std::hash<typename std::underlying_type<Trade::MeshAttribute>::type>::operator()(static_cast<typename std::underlying_type<Trade::MeshAttribute>::type>(value));
    }
};

Trade::MeshData concatenate(Containers::Array<char>&& indexData, const UnsignedInt vertexCount, Containers::Array<char>&& vertexData, Containers::Array<Trade::MeshAttributeData>&& attributeData, const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes, const char* const assertPrefix) {
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(assertPrefix);
    #endif

    /* Convert the attributes from offset-only and zero vertex count to
       absolute, referencing the vertex data array */
    for(Trade::MeshAttributeData& attribute: attributeData) {
        attribute = Trade::MeshAttributeData{
            attribute.name(), attribute.format(),
            Containers::StridedArrayView1D<void>{vertexData,
                vertexData + attribute.offset(vertexData),
                vertexCount, attribute.stride()},
            attribute.arraySize()};
    }

    /* Only list primitives are supported currently */
    /** @todo delegate to `indexTriangleStrip()` (`duplicate*()`?) etc when
        those are done */
    CORRADE_ASSERT(
        meshes.front()->primitive() != MeshPrimitive::LineStrip &&
        meshes.front()->primitive() != MeshPrimitive::LineLoop &&
        meshes.front()->primitive() != MeshPrimitive::TriangleStrip &&
        meshes.front()->primitive() != MeshPrimitive::TriangleFan,
        assertPrefix << meshes.front()->primitive() << "is not supported, turn it into a plain indexed mesh first",
        (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Populate the resulting instance with what we have. It'll be used below
       for convenient access to vertex / index data */
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    Trade::MeshData out{meshes.front()->primitive(),
        /* If the index array is empty, we're creating a non-indexed mesh (not
           an indexed mesh with zero indices) */
        std::move(indexData), indices.empty() ?
            Trade::MeshIndexData{} : Trade::MeshIndexData{indices},
        std::move(vertexData), std::move(attributeData), vertexCount};
    /* Create an attribute map. Yes, this is an inevitable fugly thing that
       allocates like mad, while everything else is zero-alloc.
       Containers::HashMap can't be here soon enough. */
    std::unordered_multimap<Trade::MeshAttribute, std::pair<UnsignedInt, bool>, MeshAttributeHash> attributeMap;
    attributeMap.reserve(out.attributeCount());
    for(UnsignedInt i = 0; i != out.attributeCount(); ++i)
        attributeMap.emplace(out.attributeName(i), std::make_pair(i, false));

    /* Go through all meshes and put all attributes and index arrays together. */
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
            Containers::ArrayView<UnsignedInt> dst = indices.slice(indexOffset, indexOffset + mesh.indexCount());
            mesh.indicesInto(dst);
            indexOffset += mesh.indexCount();

            /* Adjust indices for current vertex offset */
            for(UnsignedInt& index: dst) index += vertexOffset;

        /* Otherwise, if we need an index buffer (meaning at least one of the
           meshes is indexed), generate a trivial index buffer */
        } else if(!indices.empty()) {
            std::iota(indices + indexOffset, indices + indexOffset + mesh.vertexCount(), UnsignedInt(vertexOffset));
            indexOffset += mesh.vertexCount();
        }

        /* Reset markers saying which attribute has already been copied */
        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
            it->second.second = false;

        /* Copy attributes to their destination, skipping ones that don't have
           any equivalent in the destination mesh */
        for(UnsignedInt src = 0; src != mesh.attributeCount(); ++src) {
            /* Go through destination attributes of the same name and find the
               earliest one that hasn't been copied yet */
            auto range = attributeMap.equal_range(mesh.attributeName(src));
            UnsignedInt dst = ~UnsignedInt{};
            auto found = attributeMap.end();
            for(auto it = range.first; it != range.second; ++it) {
                if(it->second.second) continue;

                /* The range is unordered so we need to go through everything
                   and pick one with smallest ID */
                if(it->second.first < dst) {
                    dst = it->second.first;
                    found = it;
                }
            }

            /* No corresponding attribute found, continue */
            if(dst == ~UnsignedInt{}) continue;

            /* Check format compatibility. This won't fire for i ==
               ~std::size_t{}, as that's where out.primitive() comes from */
            CORRADE_ASSERT(out.attributeFormat(dst) == mesh.attributeFormat(src),
                assertPrefix << "expected" << out.attributeFormat(dst) << "for attribute" << dst << "(" << Debug::nospace << out.attributeName(dst) << Debug::nospace << ") but got" << mesh.attributeFormat(src) << "in mesh" << i << "attribute" << src,
                (Trade::MeshData{MeshPrimitive{}, 0}));
            CORRADE_ASSERT(out.attributeArraySize(dst) == mesh.attributeArraySize(src),
                assertPrefix << "expected array size" << out.attributeArraySize(dst) << "for attribute" << dst << "(" << Debug::nospace << out.attributeName(dst) << Debug::nospace << ") but got" << mesh.attributeArraySize(src) << "in mesh" << i << "attribute" << src,
                (Trade::MeshData{MeshPrimitive{}, 0}));

            /* Copy the data to a slice of the output, mark the attribute as
               copied */
            Utility::copy(mesh.attribute(src), out.mutableAttribute(dst)
                .slice(vertexOffset, vertexOffset + mesh.vertexCount()));
            found->second.second = true;
        }

        /* Update vertex offset for the next mesh */
        vertexOffset += mesh.vertexCount();
    }

    return out;
}

}

Trade::MeshData concatenate(const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes) {
    CORRADE_ASSERT(!meshes.empty(),
        "MeshTools::concatenate(): expected at least one mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    /* Calculate final attribute stride and offsets. Make a non-owning copy of
       the attribute data to avoid interleavedLayout() stealing the original
       (we still need it to be able to reference the original data). If there's
       no attributes in the original array, pass just vertex count ---
       otherwise MeshData will assert on that to avoid it getting lost. */
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(meshes.front()->attributeCount())
        attributeData = Implementation::interleavedLayout(Trade::MeshData{meshes.front()->primitive(),
            {}, meshes.front()->vertexData(),
            Trade::meshAttributeDataNonOwningArray(meshes.front()->attributeData())}, {});
    else attributeData =
        Implementation::interleavedLayout(Trade::MeshData{meshes.front()->primitive(),
            meshes.front()->vertexCount()}, {});

    /* Calculate total index/vertex count and allocate the target memory.
       Index data are allocated with NoInit as the whole array will be written,
       however vertex data might have holes and thus it's zero-initialized. */
    const std::pair<UnsignedInt, UnsignedInt> indexVertexCount = Implementation::concatenateIndexVertexCount(meshes);
    Containers::Array<char> indexData{Containers::NoInit,
        indexVertexCount.first*sizeof(UnsignedInt)};
    Containers::Array<char> vertexData{Containers::ValueInit,
        attributeData.empty() ? 0 : (attributeData[0].stride()*indexVertexCount.second)};
    return Implementation::concatenate(std::move(indexData), indexVertexCount.second, std::move(vertexData), std::move(attributeData), meshes, "MeshTools::concatenate():");
}

Trade::MeshData concatenate(std::initializer_list<Containers::Reference<const Trade::MeshData>> meshes) {
    return concatenate(Containers::arrayView(meshes));
}

}}
