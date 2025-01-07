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

#include "GenerateIndices.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
#include <Corrade/Utility/Algorithms.h>
#endif

#include "Magnum/Math/Vector3.h"
#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Implementation/remapAttributeData.h"
#include "Magnum/Trade/MeshData.h"
#endif

namespace Magnum { namespace MeshTools {

#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
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
#endif

void generateTrivialIndicesInto(const Containers::StridedArrayView1D<UnsignedInt>& output, /*mutable*/ UnsignedInt offset) {
    for(std::size_t i = 0; i != output.size(); ++i)
        output[i] = offset++;
}

Containers::Array<UnsignedInt> generateTrivialIndices(const UnsignedInt vertexCount, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, vertexCount};
    generateTrivialIndicesInto(output, offset);
    return output;
}

void generateLineStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
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
        output[i*2 + 0] = offset + i;
        output[i*2 + 1] = offset + i + 1;
    }
}

namespace {

template<class T> void generateLineStripIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 2,
        "MeshTools::generateLineStripIndicesInto(): expected either zero or at least two indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{1}) - 1;
    CORRADE_ASSERT(output.size() == 2*iMax,
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected" << 2*iMax << "but got" << output.size(), );

    /* Same as generateLineStripIndicesInto() above, just with the index array
       indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*2 + 0] = offset + indices[i];
        output[i*2 + 1] = offset + indices[i + 1];
    }
}

}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineStripIndicesIntoImplementation(indices, output, offset);
}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineStripIndicesIntoImplementation(indices, output, offset);
}

void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineStripIndicesIntoImplementation(indices, output, offset);
}

void generateLineStripIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::generateLineStripIndicesInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return generateLineStripIndicesInto(Containers::arrayCast<1, const UnsignedInt>(indices), output, offset);
    else if(indices.size()[1] == 2)
        return generateLineStripIndicesInto(Containers::arrayCast<1, const UnsignedShort>(indices), output, offset);
    else if(indices.size()[1] == 1)
        return generateLineStripIndicesInto(Containers::arrayCast<1, const UnsignedByte>(indices), output, offset);
    else CORRADE_ASSERT_UNREACHABLE("MeshTools::generateLineStripIndicesInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
}

Containers::Array<UnsignedInt> generateLineStripIndices(const UnsignedInt vertexCount, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(vertexCount, 1u) - 1)};
    generateLineStripIndicesInto(vertexCount, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size(), std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView2D<const char>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*(Math::max(indices.size()[0], std::size_t{1}) - 1)};
    generateLineStripIndicesInto(indices, output, offset);
    return output;
}

void generateLineLoopIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
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
        output[i*2 + 0] = offset + i;
        output[i*2 + 1] = offset + i + 1;
    }
    if(vertexCount >= 2) {
        output[2*vertexCount - 2] = offset + vertexCount - 1;
        output[2*vertexCount - 1] = offset + 0;
    }
}

namespace {

template<class T> void generateLineLoopIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 2,
        "MeshTools::generateLineLoopIndicesInto(): expected either zero or at least two indices, got" << indices.size(), );
    CORRADE_ASSERT(output.size() == 2*indices.size(),
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected" << 2*indices.size() << "but got" << output.size(), );

    /* Same as generateLineLoopIndicesInto() above, just with the index array
       indirection on top */
    for(std::size_t i = 0, iMax = Math::max(indices.size(), std::size_t{1}) - 1; i != iMax; ++i) {
        output[i*2 + 0] = offset + indices[i];
        output[i*2 + 1] = offset + indices[i + 1];
    }
    if(indices.size() >= 2) {
        output[2*indices.size() - 2] = offset + indices[indices.size() - 1];
        output[2*indices.size() - 1] = offset + indices[0];
    }
}

}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineLoopIndicesIntoImplementation(indices, output, offset);
}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineLoopIndicesIntoImplementation(indices, output, offset);
}

void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateLineLoopIndicesIntoImplementation(indices, output, offset);
}

void generateLineLoopIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::generateLineLoopIndicesInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return generateLineLoopIndicesInto(Containers::arrayCast<1, const UnsignedInt>(indices), output, offset);
    else if(indices.size()[1] == 2)
        return generateLineLoopIndicesInto(Containers::arrayCast<1, const UnsignedShort>(indices), output, offset);
    else if(indices.size()[1] == 1)
        return generateLineLoopIndicesInto(Containers::arrayCast<1, const UnsignedByte>(indices), output, offset);
    else CORRADE_ASSERT_UNREACHABLE("MeshTools::generateLineLoopIndicesInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const UnsignedInt vertexCount, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*vertexCount};
    generateLineLoopIndicesInto(vertexCount, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()};
    generateLineLoopIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView2D<const char>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 2*indices.size()[0]};
    generateLineLoopIndicesInto(indices, output, offset);
    return output;
}

void generateTriangleStripIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
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
        output[i*3 + 0] = offset + (i % 2 ? i + 1 : i);
        output[i*3 + 1] = offset + (i % 2 ? i : i + 1);
        output[i*3 + 2] = offset + i + 2;
    }
}

namespace {

template<class T> void generateTriangleStripIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 3,
        "MeshTools::generateTriangleStripIndicesInto(): expected either zero or at least three indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{2}) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

    /* Same as generateTriangleStripIndicesInto() above, just with the index
       array indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = offset + indices[i % 2 ? i + 1 : i];
        output[i*3 + 1] = offset + indices[i % 2 ? i : i + 1];
        output[i*3 + 2] = offset + indices[i + 2];
    }
}

}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleStripIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleStripIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleStripIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleStripIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::generateTriangleStripIndicesInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return generateTriangleStripIndicesInto(Containers::arrayCast<1, const UnsignedInt>(indices), output, offset);
    else if(indices.size()[1] == 2)
        return generateTriangleStripIndicesInto(Containers::arrayCast<1, const UnsignedShort>(indices), output, offset);
    else if(indices.size()[1] == 1)
        return generateTriangleStripIndicesInto(Containers::arrayCast<1, const UnsignedByte>(indices), output, offset);
    else CORRADE_ASSERT_UNREACHABLE("MeshTools::generateTriangleStripIndicesInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const UnsignedInt vertexCount, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(vertexCount, 2u) - 2u)};
    generateTriangleStripIndicesInto(vertexCount, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView2D<const char>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size()[0], std::size_t{2}) - 2)};
    generateTriangleStripIndicesInto(indices, output, offset);
    return output;
}

void generateTriangleFanIndicesInto(const UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
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
        output[i*3 + 0] = offset + 0;
        output[i*3 + 1] = offset + i + 1;
        output[i*3 + 2] = offset + i + 2;
    }
}

namespace {

template<class T> void generateTriangleFanIndicesIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.size() == 0 || indices.size() >= 3,
        "MeshTools::generateTriangleFanIndicesInto(): expected either zero or at least three indices, got" << indices.size(), );

    const UnsignedInt iMax = Math::max(indices.size(), std::size_t{2}) - 2;
    CORRADE_ASSERT(output.size() == 3*iMax,
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected" << 3*iMax << "but got" << output.size(), );

    /* Same as generateTriangleStripIndicesInto() above, just with the index
       array indirection on top */
    for(std::size_t i = 0; i != iMax; ++i) {
        output[i*3 + 0] = offset + indices[0];
        output[i*3 + 1] = offset + indices[i + 1];
        output[i*3 + 2] = offset + indices[i + 2];
    }
}

}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleFanIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleFanIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    generateTriangleFanIndicesIntoImplementation(indices, output, offset);
}

void generateTriangleFanIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::generateTriangleFanIndicesInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return generateTriangleFanIndicesInto(Containers::arrayCast<1, const UnsignedInt>(indices), output, offset);
    else if(indices.size()[1] == 2)
        return generateTriangleFanIndicesInto(Containers::arrayCast<1, const UnsignedShort>(indices), output, offset);
    else if(indices.size()[1] == 1)
        return generateTriangleFanIndicesInto(Containers::arrayCast<1, const UnsignedByte>(indices), output, offset);
    else CORRADE_ASSERT_UNREACHABLE("MeshTools::generateTriangleFanIndicesInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const UnsignedInt vertexCount, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(vertexCount, 2u) - 2)};
    generateTriangleFanIndicesInto(vertexCount, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size(), std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output, offset);
    return output;
}

Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView2D<const char>& indices, const UnsignedInt offset) {
    Containers::Array<UnsignedInt> output{NoInit, 3*(Math::max(indices.size()[0], std::size_t{2}) - 2)};
    generateTriangleFanIndicesInto(indices, output, offset);
    return output;
}

namespace {

template<class T> inline void generateQuadIndicesIntoImplementation(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const T>& quads, const Containers::StridedArrayView1D<T>& output, const UnsignedInt offset) {
    CORRADE_ASSERT(quads.size() % 4 == 0,
        "MeshTools::generateQuadIndicesInto(): quad index count" << quads.size() << "not divisible by 4", );
    CORRADE_ASSERT(quads.size()*6/4 == output.size(),
        "MeshTools::generateQuadIndicesInto(): bad output size, expected" << quads.size()*6/4 << "but got" << output.size(), );

    for(std::size_t i = 0, max = quads.size()/4; i != max; ++i) {
        auto get = [&](UnsignedInt j) -> const Vector3& {
            UnsignedInt index = quads[4*i + j];
            CORRADE_ASSERT(index < positions.size(),
                "MeshTools::generateQuadIndicesInto(): index" << index << "out of range for" << positions.size() << "elements", positions[0]);
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
            output[6*i + j] = offset + quads[4*i + split[j]];
    }
}

}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, const UnsignedInt offset) {
    /* We can skip zero-initialization here */
    Containers::Array<UnsignedInt> out{NoInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads, Containers::stridedArrayView(out), offset);
    return out;
}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, const UnsignedInt offset) {
    /* Explicitly ensure we have the unused bytes zeroed out */
    Containers::Array<UnsignedInt> out{ValueInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads,
        /* Could be just arrayCast<UnsignedShort>(stridedArrayView(out) on LE,
           but I want to be sure as much as possible that this compiles on BE
           as well. Hmm, now I get why LE won. */
        Containers::StridedArrayView1D<UnsignedShort>{
            Containers::arrayCast<UnsignedShort>(out),
            reinterpret_cast<UnsignedShort*>(out.data())
                #ifdef CORRADE_TARGET_BIG_ENDIAN
                + 1
                #endif
            , out.size(), 4},
        offset);
    return out;
}

Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, const UnsignedInt offset) {
    /* Explicitly ensure we have the unused bytes zeroed out */
    Containers::Array<UnsignedInt> out{ValueInit, quads.size()*6/4};
    generateQuadIndicesIntoImplementation(positions, quads,
        /* Could be just arrayCast<UnsignedShort>(stridedArrayView(out) on LE,
           but I want to be sure as much as possible that this compiles on BE
           as well. Hmm, now I get why LE won. */
        Containers::StridedArrayView1D<UnsignedByte>{
            Containers::arrayCast<UnsignedByte>(out),
            reinterpret_cast<UnsignedByte*>(out.data())
                #ifdef CORRADE_TARGET_BIG_ENDIAN
                + 3
                #endif
            , out.size(), 4},
        offset);
    return out;
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, const Containers::StridedArrayView1D<UnsignedInt>& output, const UnsignedInt offset) {
    return generateQuadIndicesIntoImplementation(positions, quads, output, offset);
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, const Containers::StridedArrayView1D<UnsignedShort>& output, const UnsignedInt offset) {
    return generateQuadIndicesIntoImplementation(positions, quads, output, offset);
}

void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, const Containers::StridedArrayView1D<UnsignedByte>& output, const UnsignedInt offset) {
    return generateQuadIndicesIntoImplementation(positions, quads, output, offset);
}

#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
Trade::MeshData generateIndices(Trade::MeshData&& mesh) {
    CORRADE_ASSERT(!mesh.isIndexed() || !isMeshIndexTypeImplementationSpecific(mesh.indexType()),
        "MeshTools::generateIndices(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
        (Trade::MeshData{MeshPrimitive{}, 0}));

    const UnsignedInt vertexCount = mesh.vertexCount();
    #ifndef CORRADE_NO_ASSERT
    UnsignedInt minVertexCount;
    if(mesh.primitive() == MeshPrimitive::LineStrip ||
       mesh.primitive() == MeshPrimitive::LineLoop) {
        minVertexCount = 2;
    } else if(mesh.primitive() == MeshPrimitive::TriangleStrip ||
              mesh.primitive() == MeshPrimitive::TriangleFan) {
        minVertexCount = 3;
    } else {
        minVertexCount = 1;
    }
    CORRADE_ASSERT(vertexCount == 0 || vertexCount >= minVertexCount,
        "MeshTools::generateIndices(): expected either zero or at least" << minVertexCount << "vertices for" << mesh.primitive() << Debug::nospace << ", got" << vertexCount,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    #endif

    /* Transfer vertex data as-is, as those don't need any changes. Release if
       possible. */
    const Containers::ArrayView<const char> originalVertexData = mesh.vertexData();
    Containers::Array<char> vertexData;
    if(mesh.vertexDataFlags() & Trade::DataFlag::Owned)
        vertexData = mesh.releaseVertexData();
    else {
        vertexData = Containers::Array<char>{NoInit, mesh.vertexData().size()};
        Utility::copy(mesh.vertexData(), vertexData);
    }

    /* Recreate the attribute array with views on the new vertexData */
    /** @todo if the vertex data were moved and this array is owned, it
        wouldn't need to be recreated, but the logic is a bit complex */
    Containers::Array<Trade::MeshAttributeData> attributeData{mesh.attributeCount()};
    for(UnsignedInt i = 0, max = attributeData.size(); i != max; ++i)
        attributeData[i] = Implementation::remapAttributeData(mesh.attributeData(i), vertexCount, originalVertexData, vertexData);

    /* Generate the index array */
    MeshPrimitive primitive;
    Containers::Array<char> indexData;
    if(mesh.primitive() == MeshPrimitive::LineStrip) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{NoInit, 2*(Math::max(vertexCount, 1u) - 1)*sizeof(UnsignedInt)};
        if(mesh.isIndexed())
            generateLineStripIndicesInto(mesh.indices(), Containers::arrayCast<UnsignedInt>(indexData));
        else
            generateLineStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(mesh.primitive() == MeshPrimitive::LineLoop) {
        primitive = MeshPrimitive::Lines;
        indexData = Containers::Array<char>{NoInit, 2*vertexCount*sizeof(UnsignedInt)};
        if(mesh.isIndexed())
            generateLineLoopIndicesInto(mesh.indices(), Containers::arrayCast<UnsignedInt>(indexData));
        else
            generateLineLoopIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(mesh.primitive() == MeshPrimitive::TriangleStrip) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{NoInit, 3*(Math::max(vertexCount, 2u) - 2)*sizeof(UnsignedInt)};
        if(mesh.isIndexed())
            generateTriangleStripIndicesInto(mesh.indices(), Containers::arrayCast<UnsignedInt>(indexData));
        else
            generateTriangleStripIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else if(mesh.primitive() == MeshPrimitive::TriangleFan) {
        primitive = MeshPrimitive::Triangles;
        indexData = Containers::Array<char>{NoInit, 3*(Math::max(vertexCount, 2u) - 2)*sizeof(UnsignedInt)};
        if(mesh.isIndexed())
            generateTriangleFanIndicesInto(mesh.indices(), Containers::arrayCast<UnsignedInt>(indexData));
        else
            generateTriangleFanIndicesInto(vertexCount, Containers::arrayCast<UnsignedInt>(indexData));
    } else {
        primitive = mesh.primitive();
        /* If the mesh is indexed, transfer the index data as-is if possible
           and it's the right type already */
        if(mesh.isIndexed()) {
            if(mesh.indexDataFlags() >= Trade::DataFlag::Owned && mesh.indexType() == MeshIndexType::UnsignedInt) {
                indexData = mesh.releaseIndexData();
            } else {
                indexData = Containers::Array<char>{NoInit, mesh.indexCount()*sizeof(UnsignedInt)};
                mesh.indicesInto(Containers::arrayCast<UnsignedInt>(indexData));
            }
        } else {
            indexData = Containers::Array<char>{NoInit, vertexCount*sizeof(UnsignedInt)};
            generateTrivialIndicesInto(Containers::arrayCast<UnsignedInt>(indexData));
        }
    }

    Trade::MeshIndexData indices{MeshIndexType::UnsignedInt, indexData};
    return Trade::MeshData{primitive, Utility::move(indexData), indices,
        Utility::move(vertexData), Utility::move(attributeData), vertexCount};
}

Trade::MeshData generateIndices(const Trade::MeshData& mesh) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return generateIndices(reference(mesh));
}
#endif

}}
