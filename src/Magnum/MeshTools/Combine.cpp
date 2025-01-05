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

#include "Combine.h"

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace {

Trade::MeshData combineIndexedImplementation(
    #if !defined(CORRADE_NO_ASSERT) && !defined(CORRADE_STANDARD_ASSERT)
    const char* assertPrefix,
    #endif
    const MeshPrimitive primitive, const Containers::StridedArrayView2D<char>& combinedIndices, const Containers::Iterable<const Trade::MeshData>& meshes)
{
    /* Make the combined index array unique */
    Containers::Array<char> indexData{NoInit, combinedIndices.size()[0]*sizeof(UnsignedInt)};
    const auto indexDataI = Containers::arrayCast<UnsignedInt>(indexData);
    const UnsignedInt vertexCount = removeDuplicatesInPlaceInto(
        combinedIndices,
        indexDataI);

    /* Gather attributes of all input meshes together */
    Containers::Array<Trade::MeshAttributeData> attributes;
    {
        UnsignedInt attributeCount = 0;
        for(const Trade::MeshData& mesh: meshes)
            attributeCount += mesh.attributeCount();
        attributes = Containers::Array<Trade::MeshAttributeData>{ValueInit, attributeCount};
        UnsignedInt attributeOffset = 0;
        for(std::size_t i = 0; i != meshes.size(); ++i) {
            const Trade::MeshData& mesh = meshes[i];
            for(std::size_t j = 0; j != mesh.attributeCount(); ++j) {
                #ifndef CORRADE_NO_ASSERT
                const VertexFormat format = mesh.attributeFormat(j);
                #endif
                /* While interleavedLayout() has the same assert, we'd have no
                   way to detect if we should bail early if it triggers, so
                   this is easier; plus the user gets a less confusing function
                   name in the message */
                CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
                    assertPrefix << "attribute" << j << "of mesh" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(format),
                    (Trade::MeshData{MeshPrimitive::Points, 0}));
                attributes[attributeOffset++] = mesh.attributeData(j);
            }
        }
    }

    /* Allocate an interleaved layout */
    /** @todo support InterleaveFlag::PreserveInterleavedAttributes here, for
        example by putting all the attributes into the MeshData instance and
        not as extras? */
    Trade::MeshData out = interleavedLayout(Trade::MeshData{primitive, 0}, vertexCount, attributes, InterleaveFlags{});

    /* Duplicate the attributes there according to the combined index buffer */
    {
        UnsignedInt indexOffset = 0;
        UnsignedInt attributeOffset = 0;
        for(const Trade::MeshData& mesh: meshes) {
            const UnsignedInt indexSize = mesh.isIndexed() ?
                meshIndexTypeSize(mesh.indexType()) : 4;
            Containers::StridedArrayView2D<const char> indices = combinedIndices.sliceSize(
                {0, indexOffset},
                {vertexCount, indexSize});

            for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i)
                duplicateInto(indices,
                    mesh.attribute(i),
                    out.mutableAttribute(attributeOffset++));

            indexOffset += indexSize;
        }
    }

    /* Combine the index buffer in */
    return Trade::MeshData{primitive,
        Utility::move(indexData), Trade::MeshIndexData{indexDataI},
        out.releaseVertexData(), out.releaseAttributeData(), vertexCount};
}

}

Trade::MeshData combineIndexedAttributes(const Containers::Iterable<const Trade::MeshData>& meshes) {
    CORRADE_ASSERT(!meshes.isEmpty(),
        "MeshTools::combineIndexedAttributes(): no meshes passed",
        (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Decide on the output primitive and index count, calculated total
       combined index type size. Initialize primitive and indexCount even
       though not needed, otherwise GCC complains about "potentially
       uninitialized" in the assert below */
    MeshPrimitive primitive{};
    UnsignedInt indexCount{};
    UnsignedInt indexStride = 0;
    for(std::size_t i = 0; i != meshes.size(); ++i) {
        CORRADE_ASSERT(meshes[i].isIndexed(),
            "MeshTools::combineIndexedAttributes(): data" << i << "is not indexed",
            (Trade::MeshData{MeshPrimitive{}, 0}));
        const MeshIndexType indexType = meshes[i].indexType();
        CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(indexType),
            "MeshTools::combineIndexedAttributes(): data" << i << "has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(indexType),
            (Trade::MeshData{MeshPrimitive{}, 0}));
        if(i == 0) {
            primitive = meshes[i].primitive();
            indexCount = meshes[i].indexCount();
        } else {
            CORRADE_ASSERT(meshes[i].primitive() == primitive,
                "MeshTools::combineIndexedAttributes(): data" << i << "is" << meshes[i].primitive() << "but expected" << primitive, (Trade::MeshData{MeshPrimitive{}, 0}));
            CORRADE_ASSERT(meshes[i].indexCount() == indexCount,
                "MeshTools::combineIndexedAttributes(): data" << i << "has" << meshes[i].indexCount() << "indices but expected" << indexCount, (Trade::MeshData{MeshPrimitive{}, 0}));
        }
        indexStride += meshIndexTypeSize(indexType);
    }

    /** @todo handle alignment in the above somehow (duplicate() will fail when
        reading 32-bit values from odd addresses on some platforms) */

    /* Create a combined index array */
    Containers::Array<char> combinedIndicesStorage{NoInit, indexCount*indexStride};
    const Containers::StridedArrayView2D<char> combinedIndices{combinedIndicesStorage, {indexCount, indexStride}};
    {
        std::size_t indexOffset = 0;
        for(const Trade::MeshData& mesh: meshes) {
            const UnsignedInt indexSize = meshIndexTypeSize(mesh.indexType());
            Containers::StridedArrayView2D<char> dst = combinedIndices
                .sliceSize({0, indexOffset}, {indexCount, indexSize});
            Utility::copy(mesh.indices(), dst);
            indexOffset += indexSize;
        }

        /* Check we pre-calculated correctly */
        CORRADE_INTERNAL_ASSERT(indexOffset == indexStride);
    }

    return combineIndexedImplementation(
        #if !defined(CORRADE_NO_ASSERT) && !defined(CORRADE_STANDARD_ASSERT)
        "MeshTools::combineIndexedAttributes():",
        #endif
        primitive, combinedIndices, meshes);
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
    const MeshIndexType meshIndexType = mesh.indexType();
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(meshIndexType),
        "MeshTools::combineFaceAttributes(): vertex mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(meshIndexType),
        (Trade::MeshData{MeshPrimitive{}, 0}));
    const UnsignedInt meshIndexSize = meshIndexTypeSize(meshIndexType);
    UnsignedInt faceIndexSize;
    if(faceAttributes.isIndexed()) {
        CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(faceAttributes.indexType()),
            "MeshTools::combineFaceAttributes(): face mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(faceAttributes.indexType()),
            (Trade::MeshData{MeshPrimitive{}, 0}));
        faceIndexSize = meshIndexTypeSize(faceAttributes.indexType());
    } else faceIndexSize = 4;
    const UnsignedInt indexStride = meshIndexSize + faceIndexSize;
    Containers::Array<char> combinedIndicesStorage{NoInit, meshIndexCount*indexStride};
    const Containers::StridedArrayView2D<char> combinedIndices{combinedIndicesStorage, {meshIndexCount, indexStride}};
    Utility::copy(mesh.indices(),
        combinedIndices.prefix({meshIndexCount, meshIndexSize}));

    /* Then, if the face attributes are not indexed, remove duplicates and put
       the resulting indices into the combined array above. For simplicity
       assume face data are interleaved. */
    Containers::StridedArrayView3D<char> combinedFaceIndices = combinedIndices
        .sliceSize({0, meshIndexSize}, {meshIndexCount, faceIndexSize})
        .expanded<0>(Containers::Size2D{faceIndexCount, 3})
        .transposed<0, 1>();
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
    return combineIndexedImplementation(
        #if !defined(CORRADE_NO_ASSERT) && !defined(CORRADE_STANDARD_ASSERT)
        "MeshTools::combineFaceAttributes():",
        #endif
        mesh.primitive(), combinedIndices, {
            mesh, faceAttributes
        });
}

Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttributeData> faceAttributes) {
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != faceAttributes.size(); ++i)
        CORRADE_ASSERT(!faceAttributes[i].isOffsetOnly(),
            "MeshTools::combineFaceAttributes(): face attribute" << i << "is offset-only",
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
