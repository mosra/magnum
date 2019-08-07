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

#include "Circle.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData2D circle2DSolid(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle2DSolid(): segments must be >= 3",
        (Trade::MeshData2D{MeshPrimitive::TriangleFan, {}, {}, {}, {}, nullptr}));

    std::vector<Vector2> positions;
    positions.reserve(segments + 2);

    /* Central point */
    positions.emplace_back();

    /* Points on circle. The first/last point is here twice to close the circle
       properly. */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments + 1; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions.emplace_back(sincos.second, sincos.first);
    }

    return Trade::MeshData2D{MeshPrimitive::TriangleFan, {}, {std::move(positions)}, {}, {}, nullptr};
}

Trade::MeshData2D circle2DWireframe(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle2DWireframe(): segments must be >= 3",
        (Trade::MeshData2D{MeshPrimitive::LineLoop, {}, {}, {}, {}, nullptr}));

    std::vector<Vector2> positions;
    positions.reserve(segments);

    /* Points on circle */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions.emplace_back(sincos.second, sincos.first);
    }

    return Trade::MeshData2D{MeshPrimitive::LineLoop, {}, {std::move(positions)}, {}, {}, nullptr};
}

Trade::MeshData3D circle3DSolid(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle3DSolid(): segments must be >= 3",
        (Trade::MeshData3D{MeshPrimitive::TriangleFan, {}, {}, {}, {}, {}, nullptr}));

    std::vector<Vector3> positions;
    positions.reserve(segments + 2);

    /* Central point */
    positions.emplace_back();

    /* Points on circle. The first/last point is here twice to close the circle
       properly. */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments + 1; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions.emplace_back(sincos.second, sincos.first, 0.0f);
    }

    /* Normals. All pointing in the same direction. */
    std::vector<Vector3> normals{segments + 2, Vector3::zAxis(1.0f)};

    return Trade::MeshData3D{MeshPrimitive::TriangleFan, {}, {std::move(positions)}, {std::move(normals)}, {}, {}, nullptr};
}

Trade::MeshData3D circle3DWireframe(const UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::circle3DWireframe(): segments must be >= 3",
        (Trade::MeshData3D{MeshPrimitive::LineLoop, {}, {}, {}, {}, {}, nullptr}));

    std::vector<Vector3> positions;
    positions.reserve(segments);

    /* Points on circle */
    const Rad angleIncrement(Constants::tau()/segments);
    for(UnsignedInt i = 0; i != segments; ++i) {
        const Rad angle(Float(i)*angleIncrement);
        const std::pair<Float, Float> sincos = Math::sincos(angle);
        positions.emplace_back(sincos.second, sincos.first, 0.0f);
    }

    return Trade::MeshData3D{MeshPrimitive::LineLoop, {}, {std::move(positions)}, {}, {}, {}, nullptr};
}

}}
