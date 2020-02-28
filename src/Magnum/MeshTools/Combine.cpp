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

#include "Combine.h"

#include <numeric>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

Trade::MeshData combineIndexedAttributes(const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> data) {
    CORRADE_ASSERT(!data.empty(), "MeshTools::combineIndexedAttributes(): no meshes passed", (Trade::MeshData{MeshPrimitive{}, 0}));

    /* Decide on the output primitive and index count, calculated total
       combined index type size and also the count and stride of all
       attributes */
    MeshPrimitive primitive;
    UnsignedInt indexCount;
    std::size_t indexStride = 0;
    std::size_t attributeCount = 0;
    UnsignedInt vertexStride = 0;
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
        attributeCount += data[i]->attributeCount();
        for(std::size_t j = 0; j != data[i]->attributeCount(); ++j)
            vertexStride += vertexFormatSize(data[i]->attributeFormat(j));
    }

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

    /** @todo handle alignment in the above somehow (duplicate() will fail when
        reading 32-bit values from odd addresses on some platforms) */

    /* Make the combined index array unique */
    Containers::Array<char> indexData{indexCount*sizeof(UnsignedInt)};
    const auto indexDataI = Containers::arrayCast<UnsignedInt>(indexData);
    const std::size_t vertexCount = removeDuplicatesInPlaceInto(
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
                const UnsignedInt attributeSize = vertexFormatSize(mesh.attributeFormat(i));
                Containers::StridedArrayView2D<char> dst{vertexData,
                    vertexData.data() + vertexOffset,
                    {vertexCount, attributeSize},
                    {std::ptrdiff_t(vertexStride), 1}};
                duplicateInto(indices, mesh.attribute(i), dst);
                vertexOffset += attributeSize;
                attributeData[attributeOffset++] = Trade::MeshAttributeData{
                    mesh.attributeName(i), mesh.attributeFormat(i), dst};
            }

            indexOffset += indexSize;
        }

        /* Check we pre-calculated correctly */
        CORRADE_INTERNAL_ASSERT(attributeOffset == attributeCount && vertexOffset == vertexStride);
    }

    return Trade::MeshData{primitive,
        std::move(indexData), Trade::MeshIndexData{indexDataI},
        std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData combineIndexedAttributes(std::initializer_list<Containers::Reference<const Trade::MeshData>> data) {
    return combineIndexedAttributes(Containers::arrayView(data));
}

}}
