/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "Arrow.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort Indices[]{
    0, 1,
    1, 2,
    1, 3
};
constexpr Trade::MeshAttributeData Attributes2D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector2, 0, 4, sizeof(Vector2)},
};

}

Trade::MeshData arrow2D(const Vector2& a, const Vector2& b) {
    Containers::Array<char> vertexData{NoInit, sizeof(Vector2)*4};
    auto positions = Containers::arrayCast<Vector2>(vertexData);

    /* Line */
    positions[0] = a;
    positions[1] = b;

    /* Arrowhead */
    const Vector2 direction = (b - a).resized(0.1f);
    const Vector2 perpendicular = direction.perpendicular();
    positions[2] = b - direction - perpendicular;
    positions[3] = b - direction + perpendicular;

    return Trade::MeshData{MeshPrimitive::Lines,
        Trade::DataFlag::Global, Indices, Trade::MeshIndexData{Indices},
        Utility::move(vertexData), Trade::meshAttributeDataNonOwningArray(Attributes2D)};
}

namespace {

constexpr Trade::MeshAttributeData Attributes3D[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector3, 0, 4, sizeof(Vector3)},
};

}

Trade::MeshData arrow3D(const Vector3& a, const Vector3& b, const Vector3& tangent) {
    Containers::Array<char> vertexData{NoInit, sizeof(Vector3)*4};
    auto positions = Containers::arrayCast<Vector3>(vertexData);

    /* Line */
    positions[0] = a;
    positions[1] = b;

    /* Arrowhead. The perpendicular sign is flipped compared to the 2D version
       to have vertex positions match the 2D version in the default
       orientation. */
    const Vector3 direction = (b - a).resized(0.1f);
    const Vector3 perpendicular = Math::cross(direction, tangent).resized(0.1f);
    positions[2] = b - direction + perpendicular;
    positions[3] = b - direction - perpendicular;

    return Trade::MeshData{MeshPrimitive::Lines,
        Trade::DataFlag::Global, Indices, Trade::MeshIndexData{Indices},
        Utility::move(vertexData), Trade::meshAttributeDataNonOwningArray(Attributes3D)};
}

Trade::MeshData arrow2D() {
   return arrow2D({0.0f, 0.0f}, {1.0f, 0.0f});
}

Trade::MeshData arrow3D() {
    return arrow3D({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
}

}}
