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

#include "Square.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* Can't be just an array of Vector2 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexSolid {
    Vector2 position;
} VerticesSolid[] {
    {{ 1.0f, -1.0f}},
    {{ 1.0f,  1.0f}},
    {{-1.0f, -1.0f}},
    {{-1.0f,  1.0f}}
};
constexpr struct VertexSolidTextureCoords {
    Vector2 position;
    Vector2 textureCoords;
} VerticesSolidTextureCoords[] {
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}}
};
constexpr Trade::MeshAttributeData AttributesSolid[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].position,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))},
};
constexpr Trade::MeshAttributeData AttributesSolidTextureCoords[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolidTextureCoords,
            &VerticesSolidTextureCoords[0].position,
            Containers::arraySize(VerticesSolidTextureCoords), sizeof(VertexSolidTextureCoords))},
    Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
        Containers::stridedArrayView(VerticesSolidTextureCoords,
            &VerticesSolidTextureCoords[0].textureCoords,
            Containers::arraySize(VerticesSolidTextureCoords), sizeof(VertexSolidTextureCoords))}
};

}

Trade::MeshData squareSolid(const SquareFlags flags) {
    if(flags & SquareFlag::TextureCoordinates)
        return Trade::MeshData{MeshPrimitive::TriangleStrip,
            {}, VerticesSolidTextureCoords,
            Trade::meshAttributeDataNonOwningArray(AttributesSolidTextureCoords)};

    return Trade::MeshData{MeshPrimitive::TriangleStrip,
        {}, VerticesSolid,
        Trade::meshAttributeDataNonOwningArray(AttributesSolid)};
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Trade::MeshData squareSolid(const SquareTextureCoords textureCoords) {
    return squareSolid(textureCoords == SquareTextureCoords::Generate ?
        SquareFlag::TextureCoordinates : SquareFlags{});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace {

/* Can't be just an array of Vector2 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexWireframe {
    Vector2 position;
} VerticesWireframe[]{
    {{-1.0f, -1.0f}},
    {{ 1.0f, -1.0f}},
    {{ 1.0f,  1.0f}},
    {{-1.0f,  1.0f}}
};
constexpr Trade::MeshAttributeData AttributesWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesWireframe, &VerticesWireframe[0].position,
            Containers::arraySize(VerticesWireframe), sizeof(VertexWireframe))}
};

}

Trade::MeshData squareWireframe() {
    return Trade::MeshData{MeshPrimitive::LineLoop,
        {}, VerticesWireframe,
        Trade::meshAttributeDataNonOwningArray(AttributesWireframe)};
}

}}
