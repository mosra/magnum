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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct PlaneTest: TestSuite::Tester {
    explicit PlaneTest();

    void solid();
    void wireframe();
};

constexpr struct {
    const char* name;
    PlaneFlags flags;
} SolidData[] {
    {"", PlaneFlags{}},
    {"texture coordinates", PlaneFlag::TextureCoordinates},
    {"tangents", PlaneFlag::Tangents},
    {"both", PlaneFlag::TextureCoordinates|PlaneFlag::Tangents}
};

PlaneTest::PlaneTest() {
    addInstancedTests({&PlaneTest::solid},
        Containers::arraySize(SolidData));

    addTests({&PlaneTest::wireframe});
}

void PlaneTest::solid() {
    auto&& data = SolidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData plane = Primitives::planeSolid(data.flags);

    CORRADE_COMPARE(plane.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!plane.isIndexed());
    CORRADE_COMPARE(plane.vertexCount(), 4);

    CORRADE_COMPARE(plane.attribute<Vector3>(Trade::MeshAttribute::Position)[3],
        (Vector3{-1.0f, 1.0f, 0.0f}));

    if(data.flags & PlaneFlag::Tangents)
        CORRADE_COMPARE(plane.attribute<Vector4>(Trade::MeshAttribute::Tangent)[1],
            (Vector4{1.0f, 0.0f, 0.0f, 1.0f}));
    else CORRADE_VERIFY(!plane.hasAttribute(Trade::MeshAttribute::Tangent));

    CORRADE_COMPARE(plane.attribute<Vector3>(Trade::MeshAttribute::Normal)[2],
        (Vector3{0.0f, 0.0f, 1.0f}));

    if(data.flags & PlaneFlag::TextureCoordinates)
        CORRADE_COMPARE(plane.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates)[1],
            (Vector2{1.0f, 1.0f}));
    else CORRADE_VERIFY(!plane.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & PlaneFlag::Tangents) {
        auto tangents = plane.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = plane.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }
}

void PlaneTest::wireframe() {
    Trade::MeshData plane = Primitives::planeWireframe();

    CORRADE_COMPARE(plane.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!plane.isIndexed());
    CORRADE_COMPARE(plane.vertexCount(), 4);
    CORRADE_COMPARE(plane.attributeCount(), 1);
    CORRADE_COMPARE(plane.attribute<Vector3>(Trade::MeshAttribute::Position)[3],
        (Vector3{-1.0f, 1.0f, 0.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::PlaneTest)
