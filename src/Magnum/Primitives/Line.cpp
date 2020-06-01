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

#include "Line.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

constexpr Trade::MeshAttributeData Attributes2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector2, 0, 2, sizeof(Vector2)},
};

}

Trade::MeshData line2D(const Vector2& a, const Vector2& b) {
    Containers::Array<char> vertexData{sizeof(Vector2)*2};
    auto positions = Containers::arrayCast<Vector2>(vertexData);
    positions[0] = a;
    positions[1] = b;

    return Trade::MeshData{MeshPrimitive::Lines, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(Attributes2D)};
}

namespace {

constexpr Trade::MeshAttributeData Attributes3D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector3, 0, 2, sizeof(Vector3)},
};

}

Trade::MeshData line3D(const Vector3& a, const Vector3& b) {
    Containers::Array<char> vertexData{sizeof(Vector3)*2};
    auto positions = Containers::arrayCast<Vector3>(vertexData);
    positions[0] = a;
    positions[1] = b;

    return Trade::MeshData{MeshPrimitive::Lines, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(Attributes3D)};
}

Trade::MeshData line2D() {
   return line2D({0.0f, 0.0f}, {1.0f, 0.0f});
}

Trade::MeshData line3D() {
    return line3D({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
}

}}
