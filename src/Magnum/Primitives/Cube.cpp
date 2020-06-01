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

#include "Cube.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort IndicesSolid[]{
     0,  1,  2,  0,  2,  3, /* +Z */
     4,  5,  6,  4,  6,  7, /* +X */
     8,  9, 10,  8, 10, 11, /* +Y */
    12, 13, 14, 12, 14, 15, /* -Z */
    16, 17, 18, 16, 18, 19, /* -Y */
    20, 21, 22, 20, 22, 23  /* -X */
};
constexpr struct VertexSolid {
    Vector3 position;
    Vector3 normal;
} VerticesSolid[]{
    {{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    {{ 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}}, /* +Z */
    {{-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},

    {{ 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}}, /* +X */
    {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},

    {{-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}}, /* +Y */
    {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},

    {{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    {{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}}, /* -Z */
    {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},

    {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}}, /* -Y */
    {{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},

    {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}}, /* -X */
    {{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}}
};
constexpr Trade::MeshAttributeData AttributesSolid[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].position,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].normal,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))}
};

}

Trade::MeshData cubeSolid() {
    return Trade::MeshData{MeshPrimitive::Triangles,
        {}, IndicesSolid, Trade::MeshIndexData{IndicesSolid},
        {}, VerticesSolid, Trade::meshAttributeDataNonOwningArray(AttributesSolid)};
}

namespace {

/* Can't be just an array of Vector3 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexSolidStrip {
    Vector3 position;
} VerticesSolidStrip[]{
    /* Sources:
        https://twitter.com/Donzanoid/status/436843034966507520
        http://www.asmcommunity.net/forums/topic/?id=6284#post-45209
        https://gist.github.com/cdwfs/2cab675b333632d940cf

        0---2---3---1
        |E /|\ A|H /|
        | / | \ | / |
        |/ D|B \|/ I|
        4---7---6---5
            |C /|
            | / |
            |/ J|
            4---5
            |\ K|
            | \ |
            |L \|
            0---1
            |\ G|
            | \ |
            |F \|
            2---3
    */
    {{ 1.0f,  1.0f,  1.0f}}, /* 3 */
    {{-1.0f,  1.0f,  1.0f}}, /* 2 */
    {{ 1.0f, -1.0f,  1.0f}}, /* 6 */
    {{-1.0f, -1.0f,  1.0f}}, /* 7 */
    {{-1.0f, -1.0f, -1.0f}}, /* 4 */
    {{-1.0f,  1.0f,  1.0f}}, /* 2 */
    {{-1.0f,  1.0f, -1.0f}}, /* 0 */
    {{ 1.0f,  1.0f,  1.0f}}, /* 3 */
    {{ 1.0f,  1.0f, -1.0f}}, /* 1 */
    {{ 1.0f, -1.0f,  1.0f}}, /* 6 */
    {{ 1.0f, -1.0f, -1.0f}}, /* 5 */
    {{-1.0f, -1.0f, -1.0f}}, /* 4 */
    {{ 1.0f,  1.0f, -1.0f}}, /* 1 */
    {{-1.0f,  1.0f, -1.0f}}  /* 0 */
};
constexpr Trade::MeshAttributeData AttributesSolidStrip[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolidStrip, &VerticesSolidStrip[0].position,
            Containers::arraySize(VerticesSolidStrip), sizeof(VertexSolidStrip))}
};

}

Trade::MeshData cubeSolidStrip() {
    return Trade::MeshData{MeshPrimitive::TriangleStrip,
        {}, VerticesSolidStrip, Trade::meshAttributeDataNonOwningArray(AttributesSolidStrip)};
}

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort IndicesWireframe[]{
    0, 1, 1, 2, 2, 3, 3, 0, /* +Z */
    4, 5, 5, 6, 6, 7, 7, 4, /* -Z */
    1, 5, 2, 6,             /* +X */
    0, 4, 3, 7              /* -X */
};
/* Can't be just an array of Vector3 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexWireframe {
    Vector3 position;
} VerticesWireframe[]{
    {{-1.0f, -1.0f,  1.0f}},
    {{ 1.0f, -1.0f,  1.0f}},
    {{ 1.0f,  1.0f,  1.0f}},
    {{-1.0f,  1.0f,  1.0f}},

    {{-1.0f, -1.0f, -1.0f}},
    {{ 1.0f, -1.0f, -1.0f}},
    {{ 1.0f,  1.0f, -1.0f}},
    {{-1.0f,  1.0f, -1.0f}}
};
constexpr Trade::MeshAttributeData AttributesWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesWireframe, &VerticesWireframe[0].position,
            Containers::arraySize(VerticesWireframe), sizeof(VertexWireframe))}
};

}

Trade::MeshData cubeWireframe() {
    return Trade::MeshData{MeshPrimitive::Lines,
        {}, IndicesWireframe, Trade::MeshIndexData{IndicesWireframe},
        {}, VerticesWireframe, Trade::meshAttributeDataNonOwningArray(AttributesWireframe)};
}

}}
