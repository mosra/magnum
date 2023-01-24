/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Reference.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

UnsignedInt primitiveCount(const MeshPrimitive primitive, const UnsignedInt elementCount) {
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt minElementCount;
    if(primitive == MeshPrimitive::Lines ||
       primitive == MeshPrimitive::LineStrip ||
       primitive == MeshPrimitive::LineLoop)
        minElementCount = 2;
    else if(primitive == MeshPrimitive::Triangles ||
            primitive == MeshPrimitive::TriangleStrip ||
            primitive == MeshPrimitive::TriangleFan)
        minElementCount = 3;
    else
        minElementCount = 1;
    CORRADE_ASSERT(elementCount == 0 || elementCount >= minElementCount,
        "MeshTools::primitiveCount(): expected either zero or at least" << minElementCount << "elements for" << primitive << Debug::nospace << ", got" << elementCount, {});

    UnsignedInt elementCountDivisor;
    if(primitive == MeshPrimitive::Lines)
        elementCountDivisor = 2;
    else if(primitive == MeshPrimitive::Triangles)
        elementCountDivisor = 3;
    else
        elementCountDivisor = 1;
    CORRADE_ASSERT(elementCount % elementCountDivisor == 0,
        "MeshTools::primitiveCount(): expected element count to be divisible by" << elementCountDivisor << "for" << primitive << Debug::nospace << ", got" << elementCount, {});
    #endif

    if(primitive == MeshPrimitive::Points ||
       primitive == MeshPrimitive::Edges ||
       primitive == MeshPrimitive::Faces ||
       primitive == MeshPrimitive::Instances)
        return elementCount;
    if(primitive == MeshPrimitive::Lines)
        return elementCount/2;
    if(primitive == MeshPrimitive::LineStrip)
        return elementCount < 1 ? 0 : elementCount - 1;
    if(primitive == MeshPrimitive::LineLoop)
        /* For a single element it'll define a degenerate line, which
           technically still *is* a primitive */
        return elementCount;
    if(primitive == MeshPrimitive::Triangles)
        return elementCount/3;
    if(primitive == MeshPrimitive::TriangleStrip ||
       primitive == MeshPrimitive::TriangleFan)
        return elementCount < 2 ? 0 : elementCount - 2;
    CORRADE_ASSERT_UNREACHABLE("MeshTools::primitiveCount(): invalid primitive" << primitive, {});
}

void generateLineStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= 2,
        "MeshTools::generateLineStripIndicesInto(): expected either zero or at least two vertices, got" << vertexCount, );

    const UnsignedInt iMax = Math::max(vertexCount, 1u) - 1;
    CORRADE_ASSERT(output.size() == 2*iMax,
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected" << 2*iMax << "but got" << output.size(), );

    /*
            1 --- 2             1 2 --- 3 4
           /       \           /           \
          /         \   =>    /             \
         /           \       /               \
        0             3     0                 5
    */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*2 + 0] = i;
        output[i*2 + 1] = i + 1;
    }
}

namespace {

template<class T> void generateLineStripIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 2,
        "MeshTools::generateLineStripIndicesInto(): expected either zero or at least two indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{1}) - 1;
    CORRADE_ASSERT(output.size() == 2*iMax,
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected" << 2*iMax << "but got" << output.size(), );

    /* Same as generateLineStripIndicesInto() above, just with the index array
       indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*2 + 0] = indices[i];
        output[i*2 + 1] = indices[i + 1];
    }
}

}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineStripIndicesIntoImplementation(indices, output);
}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineStripIndicesIntoImplementation(indices, output);
}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineStripIndicesIntoImplementation(indices, output);
}

Containers::Array<UnsignedInt> generateLineStripIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(vertexCount, 1u) - 1)};
    generateLineStripIndicesInto(vertexCount, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output);
    return output;
}

void generateLineLoopIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= 2,
        "MeshTools::generateLineLoopIndicesInto(): expected either zero or at least two vertices, got" << vertexCount, );
    CORRADE_ASSERT(output.size() == 2*vertexCount,
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected" << 2*vertexCount << "but got" << output.size(), );

    /*
        Same as with line strip, with one additional line segment at the end.

            1 --- 2             1 2 --- 3 4
           /       \           /           \
          /         \   =>    /             \
         /           \       /               \
        0 ----------- 3     0 7 ----------- 6 5
    */
    for(std::size_t i = 0, iMax = Math::max(vertexCount, 1u) - 1; i != iMax; ++i) {
        output[i*2 + 0] = i;
        output[i*2 + 1] = i + 1;
    }
    if(vertexCount >= 2) {
        output[2*vertexCount - 2] = vertexCount - 1;
        output[2*vertexCount - 1] = 0;
    }
}

namespace {

template<class T> void generateLineLoopIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 2,
        "MeshTools::generateLineLoopIndicesInto(): expected either zero or at least two indices, got" << indices.size(), );
    CORRADE_ASSERT(output.size() == 2*indices.size(),
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected" << 2*indices.size() << "but got" << output.size(), );

    /* Same as generateLineLoopIndicesInto() above, just with the index array
       indirection on top */
    for(std::size_t i = 0, iMax = Math::max(indices.size(), std::size_t{1}) - 1; i != iMax; ++i) {
        output[i*2 + 0] = indices[i];
        output[i*2 + 1] = indices[i + 1];
    }
    if(indices.size() >= 2) {
        output[2*indices.size() - 2] = indices[indices.size() - 1];
        output[2*indices.size() - 1] = indices[0];
    }
}

}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineLoopIndicesIntoImplementation(indices, output);
}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineLoopIndicesIntoImplementation(indices, output);
}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateLineLoopIndicesIntoImplementation(indices, output);
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> output{NoInit, 2*vertexCount};
    generateLineLoopIndicesInto(vertexCount, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output);
    return output;
}

void generateTriangleStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= 3,
        "MeshTools::generateTriangleStripIndicesInto(): expected either zero or at least three vertices, got" << vertexCount, );

    const UnsignedInt iMax = Math::max(vertexCount, 2u) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

    /*
        Triangles starting with odd vertices (marked with !) have the first two
        indices swapped to preserve winding.

        0 ----- 2 ----- 4           0 ----- 2 3 6 ----- 8 9
         \     / \     / \           \     / / \ \     / / \
          \   /   \   /   \     =>    \   / /   \ \   / /   \
           \ /     \ /     \           \ / /  !  \ \ / /  !  \
            1 ----- 3 ----- 5           1 4 ----- 5 7 10 ---- 11
    */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = i % 2 ? i + 1 : i;
        output[i*3 + 1] = i % 2 ? i : i + 1;
        output[i*3 + 2] = i + 2;
    }
}

namespace {

template<class T> void generateTriangleStripIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 3,
        "MeshTools::generateTriangleStripIndicesInto(): expected either zero or at least three indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{2}) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

    /* Same as generateTriangleStripIndicesInto() above, just with the index
       array indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = indices[i % 2 ? i + 1 : i];
        output[i*3 + 1] = indices[i % 2 ? i : i + 1];
        output[i*3 + 2] = indices[i + 2];
    }
}

}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleStripIndicesIntoImplementation(indices, output);
}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleStripIndicesIntoImplementation(indices, output);
}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleStripIndicesIntoImplementation(indices, output);
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(vertexCount, 2u) - 2u)};
    generateTriangleStripIndicesInto(vertexCount, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output);
    return output;
}

void generateTriangleFanIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= 3,
        "MeshTools::generateTriangleFanIndicesInto(): expected either zero or at least three vertices, got" << vertexCount, );

    const UnsignedInt iMax = Math::max(vertexCount, 2u) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

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
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = 0;
        output[i*3 + 1] = i + 1;
        output[i*3 + 2] = i + 2;
    }
}

namespace {

template<class T> void generateTriangleFanIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 3,
        "MeshTools::generateTriangleFanIndicesInto(): expected either zero or at least three indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{2}) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

    /* Same as generateTriangleStripIndicesInto() above, just with the index
       array indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = indices[0];
        output[i*3 + 1] = indices[i + 1];
        output[i*3 + 2] = indices[i + 2];
    }
}

}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleFanIndicesIntoImplementation(indices, output);
}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleFanIndicesIntoImplementation(indices, output);
}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    generateTriangleFanIndicesIntoImplementation(indices, output);
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const UnsignedInt vertexCount) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(vertexCount, 2u) - 2)};
    generateTriangleFanIndicesInto(vertexCount, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output);
    return output;
}

namespace {

template<class T> inline void generateQuadIndicesIntoImplementation(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const T>& quads, const Containers::StridedArrayView1D<T>& output) {
    CORRADE_ASSERT(quads.size() % 4 == 0,
        "MeshTools::generateQuadIndicesInto(): quad index count" << quads.size() << "not divisible by 4", );
    CORRADE_ASSERT(quads.size()*6/4 == output.size(),
        "MeshTools::generateQuadIndicesInto(): bad output size, expected" << quads.size()*6/4 << "but got" << output.size(), );

    for(std::size_t i = 0, max = quads.size()/4; i != max; ++i) {
        auto get = [&](UnsignedInt j) -> const Vector3& {
            UnsignedInt index = quads[4*i + j];
            CORRADE_ASSERT(index < positions.size(),
                "MeshTools::generateQuadIndicesInto(): index" << index << "out of bounds for" << positions.size() << "elements", positions[0]);
            return positions[index];
        };
        const Vector3& a = get(0);
        const Vector3& b = get(1);
        const Vector3& c = get(2);
        const Vector3& d = get(3);

        constexpr UnsignedInt SplitAbcAcd[] { 0, 1, 2, 0, 2, 3 };
        constexpr UnsignedInt SplitDabDbc[] { 3, 0, 1, 3, 1, 2 };
        const UnsignedInt* split;
        const bool abcAcdOppositeDirection = Math::dot(Math::cross(c - b, a - b), Math::cross(d - c, a - c)) < 0.0f;
        const bool dabDbcOppositeDirection = Math::dot(Math::cross(d - b, a - b), Math::cross(c - b, d - b)) < 0.0f;

        /* If normals of ABC and ACD point in opposite direction and DAB DBC
           point in the same direction, split as DAB DBC; and vice versa. */
        if(abcAcdOppositeDirection != dabDbcOppositeDirection)
            split = abcAcdOppositeDirection ? SplitDabDbc : SplitAbcAcd;

        /* Otherwise the normals of both cases point in the same direction or
           it's a pathological case where both cases point in the opposite.
           Pick the shorter diagonal. If both are the same, pick the "obvious"
           ABC ACD. */
        else split = (b - d).dot() < (c - a).dot() ? SplitDabDbc : SplitAbcAcd;

        /* Assign the two triangles */
        for(std::size_t j = 0; j != 6; ++j)
            output[6*i + j] = quads[4*i + split[j]];
    }
}

}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads) {
    /* We can skip zero-initialization here */
    Containers::Array<UnsignedInt> out{NoInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads, Containers::stridedArrayView(out));
    return out;
}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads) {
    /* Explicitly ensure we have the unused bytes zeroed out */
    Containers::Array<UnsignedInt> out{ValueInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads,
        /* Could be just arrayCast<UnsignedShort>(stridedArrayView(out) on LE,
           but I want to be sure as much as possible that this compiles on BE
           as well. Hmm, now I get why LE won. */
        Containers::StridedArrayView1D<UnsignedShort>{
            Containers::arrayCast<UnsignedShort>(out),
            reinterpret_cast<UnsignedShort*>(out.data())
                #ifdef CORRADE_BIG_ENDIAN
                + 1
                #endif
            , out.size(), 4}
    );
    return out;
}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads) {
    /* Explicitly ensure we have the unused bytes zeroed out */
    Containers::Array<UnsignedInt> out{ValueInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads,
        /* Could be just arrayCast<UnsignedShort>(stridedArrayView(out) on LE,
           but I want to be sure as much as possible that this compiles on BE
           as well. Hmm, now I get why LE won. */
        Containers::StridedArrayView1D<UnsignedByte>{
            Containers::arrayCast<UnsignedByte>(out),
            reinterpret_cast<UnsignedByte*>(out.data())
                #ifdef CORRADE_BIG_ENDIAN
                + 3
                #endif
            , out.size(), 4}
    );
    return out;
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, const Containers::StridedArrayView1D<UnsignedInt>& output) {
    return generateQuadIndicesIntoImplementation(positions, quads, output);
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, const Containers::StridedArrayView1D<UnsignedShort>& output) {
    return generateQuadIndicesIntoImplementation(positions, quads, output);
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, const Containers::StridedArrayView1D<UnsignedByte>& output) {
    return generateQuadIndicesIntoImplementation(positions, quads, output);
}

Trade::MeshData generateIndices(Trade::MeshData&& data) {
    CORRADE_ASSERT(!data.isIndexed(),
        "MeshTools::generateIndices(): mesh data already indexed",
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    const UnsignedInt vertexCount = data.vertexCount();
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt minVertexCount;
    if(data.primitive() == MeshPrimitive::LineStrip ||
       data.primitive() == MeshPrimitive::LineLoop) {
        minVertexCount = 2;
    } else if(data.primitive() == MeshPrimitive::TriangleStrip ||
              data.primitive() == MeshPrimitive::TriangleFan) {
        minVertexCount = 3;
    } else CORRADE_ASSERT_UNREACHABLE("MeshTools::generateIndices(): invalid primitive" << data.primitive(),
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= minVertexCount,
        "MeshTools::generateIndices(): expected either zero or at least" << minVertexCount << "vertices for" << data.primitive() << Debug::nospace << ", got" << vertexCount,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    #endif

    /* Transfer vertex / attribute data as-is, as those don't need any changes.
       Release if possible. */
    Containers::Array<char> vertexData;
    if(data.vertexDataFlags() & Trade::DataFlag::Owned)
        vertexData = data.releaseVertexData();
    else {
        vertexData = Containers::Array<char>{NoInit, data.vertexData().size()};
        Utility::copy(data.vertexData(), vertexData);
    }

    /* Recreate the attribute array with views on the new vertexData */
    /** @todo if the vertex data were moved and this array is owned, it
        wouldn't need to be recreated, but the logic is a bit complex */
    Containers::Array<Trade::MeshAttributeData> attributeData{data.attributeCount()};
    for(UnsignedInt i = 0, max = attributeData.size(); i != max; ++i) {
        attributeData[i] = Trade::MeshAttributeData{data.attributeName(i),
            data.attributeFormat(i),
            Containers::StridedArrayView1D<const void>{vertexData, vertexData.data() + data.attributeOffset(i), vertexCount, data.attributeStride(i)},
            data.attributeArraySize(i)};
    }

    /* Generate the index array */
    MeshPrimitive primitive;
    Containers::Array<char> indexData;
    if(data.primitive() == MeshPrimitive::LineStrip) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{NoInit, 2*(Math::max(vertexCount, 1u) - 1)*sizeof(UnsignedInt)};
        generateLineStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::LineLoop) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{NoInit, 2*vertexCount*sizeof(UnsignedInt)};
        generateLineLoopIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::TriangleStrip) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{NoInit, 3*(Math::max(vertexCount, 2u) - 2)*sizeof(UnsignedInt)};
        generateTriangleStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(data.primitive() == MeshPrimitive::TriangleFan) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{NoInit, 3*(Math::max(vertexCount, 2u) - 2)*sizeof(UnsignedInt)};
        generateTriangleFanIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    Trade::MeshIndexData indices{MeshIndexType::UnsignedInt, indexData};
    return Trade::MeshData{primitive, std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData), vertexCount};
}

Trade::MeshData generateIndices(const Trade::MeshData& data) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return generateIndices(reference(data));
}

}}
