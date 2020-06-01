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

#include "Axis.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort Indices2D[]{
    0, 1,
    1, 2, /* X axis */
    1, 3,

    4, 5,
    5, 6, /* Y axis */
    5, 7
};
constexpr UnsignedShort Indices3D[]{
    0, 1,
    1, 2,  /* X axis */
    1, 3,

    4, 5,
    5, 6,  /* Y axis */
    5, 7,

    8, 9,
    9, 10, /* Z axis */
    9, 11
};

constexpr struct Vertex2D {
    Vector2 position;
    Color3 color;
} Vertices2D[]{
    {{ 0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}}, /* X axis */
    {{ 0.9f,  0.1f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.9f, -0.1f}, {1.0f, 0.0f, 0.0f}},

    {{ 0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}}, /* Y axis */
    {{ 0.1f,  0.9f}, {0.0f, 1.0f, 0.0f}},
    {{-0.1f,  0.9f}, {0.0f, 1.0f, 0.0f}}
};
constexpr struct Vertex3D {
    Vector3 position;
    Color3 color;
} Vertices3D[]{
    {{ 0.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}}, /* X axis */
    {{ 0.9f,  0.1f,  0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.9f, -0.1f,  0.0f}, {1.0f, 0.0f, 0.0f}},

    {{ 0.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f, 0.0f}}, /* Y axis */
    {{ 0.1f,  0.9f,  0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.1f,  0.9f,  0.0f}, {0.0f, 1.0f, 0.0f}},

    {{ 0.0f,  0.0f,  0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f, 1.0f}}, /* Z axis */
    {{ 0.1f,  0.0f,  0.9f}, {0.0f, 0.0f, 1.0f}},
    {{-0.1f,  0.0f,  0.9f}, {0.0f, 0.0f, 1.0f}}
};

constexpr Trade::MeshAttributeData Attributes2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(Vertices2D, &Vertices2D[0].position,
            Containers::arraySize(Vertices2D), sizeof(Vertex2D))},
    Trade::MeshAttributeData{Trade::MeshAttribute::Color,
        Containers::stridedArrayView(Vertices2D, &Vertices2D[0].color,
            Containers::arraySize(Vertices2D), sizeof(Vertex2D))}
};
constexpr Trade::MeshAttributeData Attributes3D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(Vertices3D, &Vertices3D[0].position,
            Containers::arraySize(Vertices3D), sizeof(Vertex3D))},
    Trade::MeshAttributeData{Trade::MeshAttribute::Color,
        Containers::stridedArrayView(Vertices3D, &Vertices3D[0].color,
            Containers::arraySize(Vertices3D), sizeof(Vertex3D))}
};

}

Trade::MeshData axis2D() {
    return Trade::MeshData{MeshPrimitive::Lines,
        {}, Indices2D, Trade::MeshIndexData{Indices2D},
        {}, Vertices2D, Trade::meshAttributeDataNonOwningArray(Attributes2D)};
}

Trade::MeshData axis3D() {
    return Trade::MeshData{MeshPrimitive::Lines,
        {}, Indices3D, Trade::MeshIndexData{Indices3D},
        {}, Vertices3D, Trade::meshAttributeDataNonOwningArray(Attributes3D)};
}

}}
