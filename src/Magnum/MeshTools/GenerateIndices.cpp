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

#include "GenerateIndices.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

void generateLineStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    CORRADE_ASSERT(vertexCount >= 2,
        "MeshTools::generateLineStripIndicesInto(): expected at least two vertices, got" << vertexCount, );
    CORRADE_ASSERT(indices.size() == 2*(vertexCount - 1),
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected" << 2*(vertexCount - 1) << "but got" << indices.size(), );

    /*
            1 --- 2             1 2 --- 3 4
           /       \           /           \
          /         \   =>    /             \
         /           \       /               \
        0             3     0                 5
    */
    for(std::size_t i = 0; i != vertexCount - 1; ++i) {
        indices[i*2 + 0] = i;
        indices[i*2 + 1] = i + 1;
    }
}

Containers::Array<UnsignedInt> generateLineStripIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, 2*(vertexCount - 1)};
    generateLineStripIndicesInto(vertexCount, indices);
    return indices;
}

void generateLineLoopIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    CORRADE_ASSERT(vertexCount >= 2,
        "MeshTools::generateLineLoopIndicesInto(): expected at least two vertices, got" << vertexCount, );
    CORRADE_ASSERT(indices.size() == 2*vertexCount,
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected" << 2*vertexCount << "but got" << indices.size(), );

    /*
        Same as with line strip, with one additional line segment at the end.

            1 --- 2             1 2 --- 3 4
           /       \           /           \
          /         \   =>    /             \
         /           \       /               \
        0 ----------- 3     0 7 ----------- 6 5
    */
    for(std::size_t i = 0; i != vertexCount - 1; ++i) {
        indices[i*2 + 0] = i;
        indices[i*2 + 1] = i + 1;
    }
    indices[2*vertexCount - 2] = vertexCount - 1;
    indices[2*vertexCount - 1] = 0;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, 2*vertexCount};
    generateLineLoopIndicesInto(vertexCount, indices);
    return indices;
}

void generateTriangleStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    CORRADE_ASSERT(vertexCount >= 3,
        "MeshTools::generateTriangleStripIndicesInto(): expected at least three vertices, got" << vertexCount, );
    CORRADE_ASSERT(indices.size() == 3*(vertexCount - 2),
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected" << 3*(vertexCount - 2) << "but got" << indices.size(), );

    /*
        Triangles starting with odd vertices (marked with !) have the first two
        indices swapped to preserve winding.

        0 ----- 2 ----- 4           0 ----- 2 3 6 ----- 8 9
         \     / \     / \           \     / / \ \     / / \
          \   /   \   /   \     =>    \   / /   \ \   / /   \
           \ /     \ /     \           \ / /  !  \ \ / /  !  \
            1 ----- 3 ----- 5           1 4 ----- 5 7 10 ---- 11
    */
    for(std::size_t i = 0; i != vertexCount - 2; ++i) {
        indices[i*3 + 0] = i % 2 ? i + 1 : i;
        indices[i*3 + 1] = i % 2 ? i : i + 1;
        indices[i*3 + 2] = i + 2;
    }
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, 3*(vertexCount - 2)};
    generateTriangleStripIndicesInto(vertexCount, indices);
    return indices;
}

void generateTriangleFanIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    CORRADE_ASSERT(vertexCount >= 3,
        "MeshTools::generateTriangleFanIndicesInto(): expected at least three vertices, got" << vertexCount, );
    CORRADE_ASSERT(indices.size() == 3*(vertexCount - 2),
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected" << 3*(vertexCount - 2) << "but got" << indices.size(), );

    /*                              10 8 ----- 7 5
            4 ----- 3               / \ \     / / \
           / \     / \             /   \ \   / /   \
          /   \   /   \           /     \ \ / /     \
         /     \ /     \        11 ----- 9 6 3 ----- 4
        5 ----- 0 ----- 2   =>               0 ----- 2
                 \     /                      \     /
                  \   /                        \   /
                   \ /                          \ /
                    1                            1
    */
    for(std::size_t i = 0; i != vertexCount - 2; ++i) {
        indices[i*3 + 0] = 0;
        indices[i*3 + 1] = i + 1;
        indices[i*3 + 2] = i + 2;
    }
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> indices{Containers::NoInit, 3*(vertexCount - 2)};
    generateTriangleFanIndicesInto(vertexCount, indices);
    return indices;
}

Trade::MeshData generateIndices(Trade::MeshData&& data) {
    CORRADE_ASSERT(!data.isIndexed(),
        "MeshTools::generateIndices(): mesh data already indexed",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Transfer vertex / attribute data as-is, as those don't need any changes.
       Release if possible. */
    Containers::Array<char> vertexData;
    const UnsignedInt vertexCount = data.vertexCount();
    if(data.vertexDataFlags() & Trade::DataFlag::Owned)
        vertexData = data.releaseVertexData();
    else {
        vertexData = Containers::Array<char>{Containers::NoInit, data.vertexData().size()};
        Utility::copy(data.vertexData(), vertexData);
    }

    /* Recreate the attribute array with views on the new vertexData */
    /** @todo if the vertex data were moved and this array is owned, it
        wouldn't need to be recreated, but the logic is a bit complex */
    Containers::Array<Trade::MeshAttributeData> attributeData{data.attributeCount()};
    for(UnsignedInt i = 0, max = attributeData.size(); i != max; ++i) {
        attributeData[i] = Trade::MeshAttributeData{data.attributeName(i),
            data.attributeFormat(i),
            Containers::StridedArrayView1D<const void>{vertexData, vertexData.data() + data.attributeOffset(i), vertexCount, data.attributeStride(i)}};
    }

    /* Generate the index array */
    MeshPrimitive primitive;
    Containers::Array<char> indexData;
    if(data.primitive() == MeshPrimitive::LineStrip) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{Containers::NoInit, 2*(vertexCount - 1)*sizeof(UnsignedInt)};
        generateLineStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::LineLoop) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{Containers::NoInit, 2*vertexCount*sizeof(UnsignedInt)};
        generateLineLoopIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::TriangleStrip) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{Containers::NoInit, 3*(vertexCount - 2)*sizeof(UnsignedInt)};
        generateTriangleStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::TriangleFan) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{Containers::NoInit, 3*(vertexCount - 2)*sizeof(UnsignedInt)};
        generateTriangleFanIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else CORRADE_ASSERT(false,
        "MeshTools::generateIndices(): invalid primitive" << data.primitive(),
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    Trade::MeshIndexData indices{MeshIndexType::UnsignedInt, indexData};
    return Trade::MeshData{primitive, std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData generateIndices(const Trade::MeshData& data) {
    CORRADE_ASSERT(!data.isIndexed(),
        "MeshTools::generateIndices(): mesh data already indexed",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    return generateIndices(Trade::MeshData{data.primitive(),
        {}, data.vertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData()),
        data.vertexCount()});
}

}}
