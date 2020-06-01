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
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct UVSphereTest: TestSuite::Tester {
    explicit UVSphereTest();

    void solidWithoutTextureCoordinates();
    void solidWithTextureCoordinatesOrTangents();
    void wireframe();
};

constexpr struct {
    const char* name;
    UVSphereFlags flags;
} TextureCoordinatesOrTangentsData[] {
    {"texture coordinates", UVSphereFlag::TextureCoordinates},
    {"tangents", UVSphereFlag::Tangents},
    {"both", UVSphereFlag::TextureCoordinates|UVSphereFlag::Tangents}
};

UVSphereTest::UVSphereTest() {
    addTests({&UVSphereTest::solidWithoutTextureCoordinates});

    addInstancedTests({&UVSphereTest::solidWithTextureCoordinatesOrTangents},
        Containers::arraySize(TextureCoordinatesOrTangentsData));

    addTests({&UVSphereTest::wireframe});
}

void UVSphereTest::solidWithoutTextureCoordinates() {
    Trade::MeshData sphere = uvSphereSolid(3, 3);

    CORRADE_COMPARE(sphere.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(sphere.isIndexed());
    CORRADE_COMPARE(sphere.attributeCount(), 2);

    CORRADE_COMPARE_AS(sphere.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},

        {0, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},

        {0, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 2, 1, 0, 3, 2, 0, 1, 3,
        1, 2, 5, 1, 5, 4, 2, 3, 6, 2, 6, 5, 3, 1, 4, 3, 4, 6,
        4, 5, 7, 5, 6, 7, 6, 4, 7
    }), TestSuite::Compare::Container);
}

void UVSphereTest::solidWithTextureCoordinatesOrTangents() {
    auto&& data = TextureCoordinatesOrTangentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData sphere = uvSphereSolid(3, 3, data.flags);

    CORRADE_COMPARE(sphere.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(sphere.isIndexed());

    CORRADE_COMPARE_AS(sphere.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},
        {0.0f, -0.5f, 0.866025f},

        {0.0f, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},
        {0.0f, 0.5f, 0.866025f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & UVSphereFlag::Tangents) {
        CORRADE_COMPARE_AS(sphere.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {-1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {-0.5f, 0.0f, -0.866025f, 1.0f},
            {-0.5f, 0.0f, 0.866025f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {-0.5f, 0.0f, -0.866025f, 1.0f},
            {-0.5f, 0.0f, 0.866025f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!sphere.hasAttribute(Trade::MeshAttribute::Tangent));

    if(data.flags & UVSphereFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(sphere.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.5f, 0.0f},

            {0.0f, 0.333333f},
            {0.333333f, 0.333333f},
            {0.666667f, 0.333333f},
            {1.0f, 0.333333f},

            {0.0f, 0.666667f},
            {0.333333f, 0.666667f},
            {0.666667f, 0.666667f},
            {1.0f, 0.666667f},

            {0.5f, 1.0f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!sphere.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & UVSphereFlag::Tangents) {
        auto tangents = sphere.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = sphere.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }

    CORRADE_COMPARE_AS(sphere.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 9, 6, 7, 9, 7, 8, 9
    }), TestSuite::Compare::Container);
}

void UVSphereTest::wireframe() {
    Trade::MeshData sphere = uvSphereWireframe(6, 8);

    CORRADE_COMPARE(sphere.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(sphere.isIndexed());
    CORRADE_COMPARE(sphere.attributeCount(), 1);

    CORRADE_COMPARE_AS(sphere.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.866025f, 0.5f},
        {0.5f, -0.866025f, 0.0f},
        {0.0f, -0.866025f, -0.5f},
        {-0.5f, -0.866025f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.866025f, -0.5f, 0.0f},
        {0.0f, -0.5f, -0.866025f},
        {-0.866025f, -0.5f, 0.0f},

        /* Four "corners" of the center ring */
        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},

        /* In between the four corners of the center ring */
        {0.707107f, 0.0f, 0.707107f},
        {0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, 0.707107f},

        {0.0f, 0.5f, 0.866025f},
        {0.866025f, 0.5f, 0.0f},
        {0.0f, 0.5f, -0.866025f},
        {-0.866025f, 0.5f, 0.0f},

        {0.0f, 0.866025f, 0.5f},
        {0.5f, 0.866025f, 0.0f},
        {0.0f, 0.866025f, -0.5f},
        {-0.5f, 0.866025f, 0.0f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 0, 2, 0, 3, 0, 4,
        1, 5, 2, 6, 3, 7, 4, 8,

        5, 9, 6, 10, 7, 11, 8, 12,
        9, 13, 10, 14, 11, 15, 12, 16,

        13, 10, 14, 11, 15, 12, 16, 9,
        9, 17, 10, 18, 11, 19, 12, 20,

        17, 21, 18, 22, 19, 23, 20, 24,
        21, 25, 22, 25, 23, 25, 24, 25
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::UVSphereTest)
