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

#include "Crosshair.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* Can't be just an array of Vector2 but has to be a struct, because then MSVC
   2015 fails with an assertion like

    Trade::MeshData: attribute 0 [0x7ffd6c4fb290:0x7ffd6c4fb338] is not contained in passed vertexData array [0x7ffd6c4fb170:0x7ffd6c4fb218]

   which leads me to believe that it will make two copies of the data, one for
   the MeshAttributeData constructor and one for the MeshData constructor. Same
   for the Cube, Square and Plane primitives. Interestingly enough, this isn't
   a problem for index arrays (maybe because those are integers and not
   constexpr classes?). Also not a problem for MSVC 2017 and up. */
constexpr struct Vertex2D {
    Vector2 position;
} Vertices2D[]{
    {{-1.0f,  0.0f}}, {{1.0f, 0.0f}},
    {{ 0.0f, -1.0f}}, {{0.0f, 1.0f}}
};
constexpr struct Vertex3D {
    Vector3 position;
} Vertices3D[]{
    {{-1.0f,  0.0f,  0.0f}}, {{1.0f, 0.0f, 0.0f}},
    {{ 0.0f, -1.0f,  0.0f}}, {{0.0f, 1.0f, 0.0f}},
    {{ 0.0f,  0.0f, -1.0f}}, {{0.0f, 0.0f, 1.0f}}
};

constexpr Trade::MeshAttributeData Attributes2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(Vertices2D, &Vertices2D[0].position,
            Containers::arraySize(Vertices2D), sizeof(Vertex2D))}
};
constexpr Trade::MeshAttributeData Attributes3D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(Vertices3D, &Vertices3D[0].position,
            Containers::arraySize(Vertices3D), sizeof(Vertex3D))}
};

}

Trade::MeshData crosshair2D() {
    return Trade::MeshData{MeshPrimitive::Lines, {}, Vertices2D,
        Trade::meshAttributeDataNonOwningArray(Attributes2D)};
}

Trade::MeshData crosshair3D() {
    return Trade::MeshData{MeshPrimitive::Lines, {}, Vertices3D,
        Trade::meshAttributeDataNonOwningArray(Attributes3D)};
}

}}
