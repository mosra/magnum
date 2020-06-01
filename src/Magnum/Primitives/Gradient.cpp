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

#include "Gradient.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Intersection.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

struct Vertex2D {
    Vector2 position;
    Color4 color;
};

constexpr Trade::MeshAttributeData Attributes2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector2, offsetof(Vertex2D, position),
        4, sizeof(Vertex2D)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Color,
        VertexFormat::Vector4, offsetof(Vertex2D, color),
        4, sizeof(Vertex2D)},
};

}

Trade::MeshData gradient2D(const Vector2& a, const Color4& colorA, const Vector2& b, const Color4& colorB) {
    Containers::Array<char> vertexData{sizeof(Vertex2D)*4};
    auto vertices = Containers::arrayCast<Vertex2D>(vertexData);
    vertices[0].position = { 1.0f, -1.0f};
    vertices[1].position = { 1.0f,  1.0f};
    vertices[2].position = {-1.0f, -1.0f};
    vertices[3].position = {-1.0f,  1.0f};

    /* For every corner, take a line perpendicular to the gradient direction
       and passing through the corner. The calculated intersection position
       with the gradient line segment is the blend factor for the two colors
       for given corner. */
    const Vector2 direction = b - a;
    const Vector2 perpendicular = direction.perpendicular();
    for(std::size_t i = 0; i != 4; ++i) {
        const Float t = Math::Intersection::lineSegmentLine(a, direction, vertices[i].position, perpendicular);
        vertices[i].color = Math::lerp(colorA, colorB, t);
    }

    return Trade::MeshData{MeshPrimitive::TriangleStrip, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(Attributes2D)};
}

Trade::MeshData gradient2DHorizontal(const Color4& colorLeft, const Color4& colorRight) {
    return Primitives::gradient2D({-1.0f, 0.0f}, colorLeft, {1.0f, 0.0f}, colorRight);
}

Trade::MeshData gradient2DVertical(const Color4& colorBottom, const Color4& colorTop) {
    return Primitives::gradient2D({0.0f, -1.0f}, colorBottom, {0.0f, 1.0f}, colorTop);
}

namespace {

struct Vertex3D {
    Vector3 position;
    Vector3 normal;
    Color4 color;
};

constexpr Trade::MeshAttributeData Attributes3D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector3, offsetof(Vertex3D, position),
        4, sizeof(Vertex3D)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        VertexFormat::Vector3, offsetof(Vertex3D, normal),
        4, sizeof(Vertex3D)},
    Trade::MeshAttributeData{Trade::MeshAttribute::Color,
        VertexFormat::Vector4, offsetof(Vertex3D, color),
        4, sizeof(Vertex3D)},
};

}

Trade::MeshData gradient3D(const Vector3& a, const Color4& colorA, const Vector3& b, const Color4& colorB) {
    Containers::Array<char> vertexData{sizeof(Vertex3D)*4};
    auto vertices = Containers::arrayCast<Vertex3D>(vertexData);
    vertices[0].position = { 1.0f, -1.0f, 0};
    vertices[1].position = { 1.0f,  1.0f, 0};
    vertices[2].position = {-1.0f, -1.0f, 0};
    vertices[3].position = {-1.0f,  1.0f, 0};
    vertices[0].normal = {0.0f, 0.0f, 1.0f};
    vertices[1].normal = {0.0f, 0.0f, 1.0f};
    vertices[2].normal = {0.0f, 0.0f, 1.0f};
    vertices[3].normal = {0.0f, 0.0f, 1.0f};

    /* For every corner, take a plane perpendicular to the gradient direction
       and passing through the corner. The calculated intersection position
       with the gradient line segment is the blend factor for the two colors
       for given corner. */
    const Vector3 direction = b - a;
    for(std::size_t i = 0; i != 4; ++i) {
        const Vector4 plane = Math::planeEquation(direction, vertices[i].position);
        const Float t = Math::Intersection::planeLine(plane, a, direction);
        vertices[i].color = Math::lerp(colorA, colorB, t);
    }

    return Trade::MeshData{MeshPrimitive::TriangleStrip, std::move(vertexData),
        Trade::meshAttributeDataNonOwningArray(Attributes3D)};
}

Trade::MeshData gradient3DHorizontal(const Color4& colorLeft, const Color4& colorRight) {
    return Primitives::gradient3D({-1.0f, 0.0f, 0.0f}, colorLeft, {1.0f, 0.0f, 0.0f}, colorRight);
}

Trade::MeshData gradient3DVertical(const Color4& colorBottom, const Color4& colorTop) {
    return Primitives::gradient3D({0.0f, -1.0f, 0.0f}, colorBottom, {0.0f, 1.0f, 0.0f}, colorTop);
}

}}
