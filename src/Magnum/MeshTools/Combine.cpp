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

#include "Combine.h"

#include <numeric>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace {

Trade::MeshData combineIndexedImplementation(const MeshPrimitive primitive, Containers::Array<char>& combinedIndices, const UnsignedInt indexCount, const UnsignedInt indexStride, const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> data) {
    /* Calculate attribute count and vertex stride */
    UnsignedInt attributeCount = 0;
    UnsignedInt vertexStride = 0;
    for(std::size_t i = 0; i != data.size(); ++i) {
        attributeCount += data[i]->attributeCount();
        for(UnsignedInt j = 0; j != data[i]->attributeCount(); ++j)
            vertexStride += vertexFormatSize(data[i]->attributeFormat(j))*Math::max(data[i]->attributeArraySize(j), UnsignedShort{1});
    }

    /* Make the combined index array unique */
    Containers::Array<char> indexData{indexCount*sizeof(UnsignedInt)};
    const auto indexDataI = Containers::arrayCast<UnsignedInt>(indexData);
    const UnsignedInt vertexCount = removeDuplicatesInPlaceInto(
        Containers::StridedArrayView2D<char>{combinedIndices, {indexCount, indexStride}},
        indexDataI);

    /* Allocate resulting attribute and vertex data and duplicate the
       attributes there according to the combined index buffer */
    Containers::Array<char> vertexData{Containers::NoInit,
        vertexStride*vertexCount};
    Containers::Array<Trade::MeshAttributeData> attributeData{attributeCount};
    {
        std::size_t indexOffset = 0;
        std::size_t attributeOffset = 0;
        std::size_t vertexOffset = 0;
        for(const Trade::MeshData& mesh: data) {
            const UnsignedInt indexSize = mesh.isIndexed() ?
                meshIndexTypeSize(mesh.indexType()) : 4;
            Containers::StridedArrayView2D<const char> indices{combinedIndices,
                combinedIndices.data() + indexOffset,
                {vertexCount, indexSize},
                {std::ptrdiff_t(indexStride), 1}};

            for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
                Containers::StridedArrayView2D<const char> src = mesh.attribute(i);
                Containers::StridedArrayView2D<char> dst{vertexData,
                    vertexData.data() + vertexOffset,
                    {vertexCount, src.size()[1]},
                    {std::ptrdiff_t(vertexStride), 1}};
                duplicateInto(indices, src, dst);
                vertexOffset += src.size()[1];
                attributeData[attributeOffset++] = Trade::MeshAttributeData{
                    mesh.attributeName(i), mesh.attributeFormat(i), dst,
                    mesh.attributeArraySize(i)};
            }

            indexOffset += indexSize;
        }

        /* Check we pre-calculated correctly */
        CORRADE_INTERNAL_ASSERT(attributeOffset == attributeCount && vertexOffset == vertexStride);
    }

    return Trade::MeshData{primitive,
        std::move(indexData), Trade::MeshIndexData{indexDataI},
        std::move(vertexData), std::move(attributeData), vertexCount};
}

}

Trade::MeshData combineIndexedAttributes(const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> data) {
    CORRADE_ASSERT(!data.empty(),
        "MeshTools::combineIndexedAttributes(): no meshes passed",
        (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Decide on the output primitive and index count, calculated total
       combined index type size. Initialize primitive and indexCount even
       though not needed, otherwise GCC complains about "potentially
       uninitialized" in the assert below */
    MeshPrimitive primitive{};
    UnsignedInt indexCount{};
    UnsignedInt indexStride = 0;
    for(std::size_t i = 0; i != data.size(); ++i) {
        CORRADE_ASSERT(data[i]->isIndexed(),
            "MeshTools::combineIndexedAttributes(): data" << i << "is not indexed",
            (Trade::MeshData{MeshPrimitive{}, 0}));
        if(i == 0) {
            primitive = data[i]->primitive();
            indexCount = data[i]->indexCount();
        } else {
            CORRADE_ASSERT(data[i]->primitive() == primitive,
                "MeshTools::combineIndexedAttributes(): data" << i << "is" << data[i]->primitive() << "but expected" << primitive, (Trade::MeshData{MeshPrimitive{}, 0}));
            CORRADE_ASSERT(data[i]->indexCount() == indexCount,
                "MeshTools::combineIndexedAttributes(): data" << i << "has" << data[i]->indexCount() << "indices but expected" << indexCount, (Trade::MeshData{MeshPrimitive{}, 0}));
        }
        indexStride += meshIndexTypeSize(data[i]->indexType());
    }

    /** @todo handle alignment in the above somehow (duplicate() will fail when
        reading 32-bit values from odd addresses on some platforms) */

    /* Create a combined index array */
    Containers::Array<char> combinedIndices{Containers::NoInit,
        indexCount*indexStride};
    {
        std::size_t indexOffset = 0;
        for(const Trade::MeshData& mesh: data) {
            const UnsignedInt indexSize = meshIndexTypeSize(mesh.indexType());
            Containers::StridedArrayView2D<char> dst{combinedIndices,
                combinedIndices.data() + indexOffset,
                {indexCount, indexSize},
                {std::ptrdiff_t(indexStride), 1}};
            Utility::copy(mesh.indices(), dst);
            indexOffset += indexSize;
        }

        /* Check we pre-calculated correctly */
        CORRADE_INTERNAL_ASSERT(indexOffset == indexStride);
    }

    return combineIndexedImplementation(primitive, combinedIndices, indexCount, indexStride, data);
}

Trade::MeshData combineIndexedAttributes(std::initializer_list<Containers::Reference<const Trade::MeshData>> data) {
    return combineIndexedAttributes(Containers::arrayView(data));
}

Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, const Trade::MeshData& faceAttributes) {
    CORRADE_ASSERT(mesh.isIndexed(),
        "MeshTools::combineFaceAttributes(): vertex mesh is not indexed",
        (Trade::MeshData{MeshPrimitive{}, 0}));
    CORRADE_ASSERT(mesh.primitive() == MeshPrimitive::Triangles && faceAttributes.primitive() == MeshPrimitive::Faces,
        "MeshTools::combineFaceAttributes(): expected a MeshPrimitive::Triangles mesh and a MeshPrimitive::Faces mesh but got" << mesh.primitive() << "and" << faceAttributes.primitive(),
        (Trade::MeshData{MeshPrimitive{}, 0}));
    const UnsignedInt meshIndexCount = mesh.indexCount();
    const UnsignedInt faceIndexCount = faceAttributes.isIndexed() ?
        faceAttributes.indexCount() : faceAttributes.vertexCount();
    CORRADE_ASSERT(faceIndexCount*3 == meshIndexCount,
        "MeshTools::combineFaceAttributes(): expected" << meshIndexCount/3 << "face entries for" << meshIndexCount << "indices but got" << faceIndexCount,
        (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Make a combined index array. First copy the mesh indices as-is. */
    const UnsignedInt meshIndexSize = meshIndexTypeSize(mesh.indexType());
    const UnsignedInt faceIndexSize = faceAttributes.isIndexed() ?
        meshIndexTypeSize(faceAttributes.indexType()) : 4;
    const UnsignedInt indexStride = meshIndexSize + faceIndexSize;
    Containers::Array<char> combinedIndices{Containers::NoInit,
        meshIndexCount*indexStride};
    Utility::copy(mesh.indices(),
        Containers::StridedArrayView2D<char>{combinedIndices, {meshIndexCount, meshIndexSize}, {std::ptrdiff_t(indexStride), 1}});

    /* Then, if the face attributes are not indexed, remove duplicates and put
       the resulting indices into the combined array above. For simplicity
       assume face data are interleaved. */
    Containers::StridedArrayView3D<char> combinedFaceIndices{combinedIndices,
        combinedIndices.data() + meshIndexSize,
        {3, faceIndexCount, faceIndexSize},
        {std::ptrdiff_t(indexStride), 3*std::ptrdiff_t(indexStride), 1}};
    if(!faceAttributes.isIndexed()) {
        /** @todo this could go into a dedicated removeDuplicates(MeshData)
            feature at some point, which would handle everything including
            in-place / non-in-place, indexed / non-indexed etc. */
        CORRADE_ASSERT(isInterleaved(faceAttributes),
            "MeshTools::combineFaceAttributes(): face attributes are not interleaved",
            (Trade::MeshData{MeshPrimitive{}, 0}));
        removeDuplicatesInto(interleavedData(faceAttributes), Containers::arrayCast<1, UnsignedInt>(combinedFaceIndices[0]));

    /* Otherwise, simply copy the indices directly */
    } else Utility::copy(faceAttributes.indices(), combinedFaceIndices[0]);

    /* Duplicate the vertex index to the other two vertices of each triangle */
    Utility::copy(combinedFaceIndices[0], combinedFaceIndices[1]);
    Utility::copy(combinedFaceIndices[0], combinedFaceIndices[2]);

    /* Then combine the two into a single buffer */
    return combineIndexedImplementation(mesh.primitive(), combinedIndices,
        meshIndexCount, indexStride,
        Containers::arrayView<const Containers::Reference<const Trade::MeshData>>({
            mesh, faceAttributes
        }));
}

Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttributeData> faceAttributes) {
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != faceAttributes.size(); ++i)
        CORRADE_ASSERT(!faceAttributes[i].isOffsetOnly(),
            "MeshTools::combineFaceAttributes(): face attribute" << i << "is offset-only, which is not supported",
                (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    #endif

    return combineFaceAttributes(mesh, Trade::MeshData{MeshPrimitive::Faces,
        /* Supply a vertex data view spanning the whole memory. It's not used
           directly and this shuts off asserts for attribute bounds */
        {}, {nullptr, ~std::size_t{}},
        Trade::meshAttributeDataNonOwningArray(faceAttributes)});
}

Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttributeData> faceAttributes) {
    return combineFaceAttributes(mesh, Containers::arrayView(faceAttributes));
}

}}
