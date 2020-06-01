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
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Primitives/Capsule.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CapsuleTest: TestSuite::Tester {
    explicit CapsuleTest();

    void wireframe2D();

    void solid3DWithoutTextureCoordinates();
    void solid3DWithTextureCoordinatesOrTangents();
    void wireframe3D();
};

constexpr struct {
    const char* name;
    CapsuleFlags flags;
} TextureCoordinatesOrTangentsData[] {
    {"texture coordinates", CapsuleFlag::TextureCoordinates},
    {"tangents", CapsuleFlag::Tangents},
    {"both", CapsuleFlag::TextureCoordinates|CapsuleFlag::Tangents}
};

CapsuleTest::CapsuleTest() {
    addTests({&CapsuleTest::wireframe2D,
              &CapsuleTest::solid3DWithoutTextureCoordinates});

    addInstancedTests({&CapsuleTest::solid3DWithTextureCoordinatesOrTangents},
        Containers::arraySize(TextureCoordinatesOrTangentsData));

    addTests({&CapsuleTest::wireframe3D});
}

void CapsuleTest::wireframe2D() {
    Trade::MeshData capsule = capsule2DWireframe(2, 4, 0.5f);

    CORRADE_COMPARE(capsule.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(capsule.isIndexed());
    CORRADE_COMPARE(capsule.attributeCount(), 1);

    CORRADE_COMPARE_AS(capsule.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        {0.0f, -1.5f},

        {-0.707107f, -1.20711f},
        {0.707107f, -1.20711f},

        {-1.0f, -0.5f},
        {1.0f, -0.5f},

        {-1.0f, -0.25f},
        {1.0f, -0.25f},

        {-1.0f, 0.0f},
        {1.0f, 0.0f},

        {-1.0f, 0.25f},
        {1.0f, 0.25f},

        {-1.0f, 0.5f},
        {1.0f, 0.5f},

        {-0.707107f, 1.20711f},
        {0.707107f, 1.20711f},

        {0.0f, 1.5f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(capsule.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 0, 2,

        1, 3, 2, 4,
        3, 5, 4, 6,
        5, 7, 6, 8,
        7, 9, 8, 10,
        9, 11, 10, 12,
        11, 13, 12, 14,

        13, 15, 14, 15
    }), TestSuite::Compare::Container);
}

void CapsuleTest::solid3DWithoutTextureCoordinates() {
    Trade::MeshData capsule = capsule3DSolid(2, 4, 3, 0.5f);

    CORRADE_COMPARE(capsule.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(capsule.isIndexed());
    CORRADE_COMPARE(capsule.attributeCount(), 2);

    CORRADE_COMPARE_AS(capsule.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.5f, 0.0f},

        {0.0f, -1.20711f, 0.707107f},
        {0.612372f, -1.20711f, -0.353553f},
        {-0.612373f, -1.20711f, -0.353553f},

        {0.0f, -0.5f, 1.0f},
        {0.866025f, -0.5f, -0.5f},
        {-0.866025f, -0.5f, -0.5f},

        {0.0f, -0.25f, 1.0f},
        {0.866025f, -0.25f, -0.5f},
        {-0.866025f, -0.25f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.25f, 1.0f},
        {0.866025f, 0.25f, -0.5f},
        {-0.866025f, 0.25f, -0.5f},

        {0.0f, 0.5f, 1.0f},
        {0.866025f, 0.5f, -0.5f},
        {-0.866025f, 0.5f, -0.5f},

        {0.0f, 1.20711f, 0.707107f},
        {0.612372f, 1.20711f, -0.353553f},
        {-0.612372f, 1.20711f, -0.353553f},

        {0.0f, 1.5f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(capsule.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.707107f, 0.707107f},
        {0.612372f, -0.707107f, -0.353553f},
        {-0.612373f, -0.707107f, -0.353553f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.707107f, 0.707107f},
        {0.612372f, 0.707107f, -0.353553f},
        {-0.612372f, 0.707107f, -0.353553f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(capsule.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 2, 1, 0, 3, 2, 0, 1, 3,
        1, 2, 5, 1, 5, 4, 2, 3, 6, 2, 6, 5, 3, 1, 4, 3, 4, 6,
        4, 5, 8, 4, 8, 7, 5, 6, 9, 5, 9, 8, 6, 4, 7, 6, 7, 9,
        7, 8, 11, 7, 11, 10, 8, 9, 12, 8, 12, 11, 9, 7, 10, 9, 10, 12,
        10, 11, 14, 10, 14, 13, 11, 12, 15, 11, 15, 14, 12, 10, 13, 12, 13, 15,
        13, 14, 17, 13, 17, 16, 14, 15, 18, 14, 18, 17, 15, 13, 16, 15, 16, 18,
        16, 17, 20, 16, 20, 19, 17, 18, 21, 17, 21, 20, 18, 16, 19, 18, 19, 21,
        19, 20, 22, 20, 21, 22, 21, 19, 22
    }), TestSuite::Compare::Container);
}

void CapsuleTest::solid3DWithTextureCoordinatesOrTangents() {
    auto&& data = TextureCoordinatesOrTangentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData capsule = capsule3DSolid(2, 2, 3, 0.5f, data.flags);

    CORRADE_COMPARE(capsule.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(capsule.isIndexed());

    CORRADE_COMPARE_AS(capsule.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.5f, 0.0f},

        {0.0f, -1.20711f, 0.707107f},
        {0.612372f, -1.20711f, -0.353553f},
        {-0.612373f, -1.20711f, -0.353553f},
        {0.0f, -1.20711f, 0.707107f},

        {0.0f, -0.5f, 1.0f},
        {0.866025f, -0.5f, -0.5f},
        {-0.866025f, -0.5f, -0.5f},
        {0.0f, -0.5f, 1.0f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},
        {0.0f, 0.0f, 1.0f},

        {0.0f, 0.5f, 1.0f},
        {0.866025f, 0.5f, -0.5f},
        {-0.866025f, 0.5f, -0.5f},
        {0.0f, 0.5f, 1.0f},

        {0.0f, 1.20711f, 0.707107f},
        {0.612372f, 1.20711f, -0.353553f},
        {-0.612372f, 1.20711f, -0.353553f},
        {0.0f, 1.20711f, 0.707107f},

        {0.0f, 1.5f, 0.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & CapsuleFlag::Tangents) {
        CORRADE_COMPARE_AS(capsule.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {-1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {-0.5f, 0.0f, -0.866025f, 1.0f},
            {-0.5f, 0.0f, 0.866025f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {-0.5f, 0.0f, -0.866025f, 1.0f},
            {-0.5f, 0.0f, 0.866025f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {-0.5f, 0.0f, -0.866025f, 1.0f},
            {-0.5f, 0.0f, 0.866025f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

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
    } else CORRADE_VERIFY(!capsule.hasAttribute(Trade::MeshAttribute::Tangent));

    if(data.flags & CapsuleFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(capsule.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.5f, 0.0f},

            {0.0f, 0.166667f},
            {0.333333f, 0.166667f},
            {0.666667f, 0.166667f},
            {1.0f, 0.166667f},

            {0.0f, 0.333333f},
            {0.333333f, 0.333333f},
            {0.666667f, 0.333333f},
            {1.0f, 0.333333f},

            {0.0f, 0.5f},
            {0.333333f, 0.5f},
            {0.666667f, 0.5f},
            {1.0f, 0.5f},

            {0.0f, 0.666667f},
            {0.333333f, 0.666667f},
            {0.666667f, 0.666667f},
            {1.0f, 0.666667f},

            {0.0f, 0.833333f},
            {0.333333f, 0.833333f},
            {0.666667f, 0.833333f},
            {1.0f, 0.833333f},

            {0.5f, 1.0f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!capsule.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & CapsuleFlag::Tangents) {
        auto tangents = capsule.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = capsule.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }

    CORRADE_COMPARE_AS(capsule.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 10, 5, 10, 9, 6, 7, 11, 6, 11, 10, 7, 8, 12, 7, 12, 11,
        9, 10, 14, 9, 14, 13, 10, 11, 15, 10, 15, 14, 11, 12, 16, 11, 16, 15,
        13, 14, 18, 13, 18, 17, 14, 15, 19, 14, 19, 18, 15, 16, 20, 15, 20, 19,
        17, 18, 21, 18, 19, 21, 19, 20, 21
    }), TestSuite::Compare::Container);
}

void CapsuleTest::wireframe3D() {
    Trade::MeshData capsule = capsule3DWireframe(2, 2, 8, 0.5f);

    CORRADE_COMPARE(capsule.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(capsule.isIndexed());
    CORRADE_COMPARE(capsule.attributeCount(), 1);

    CORRADE_COMPARE_AS(capsule.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.5f, 0.0f},

        {0.0f, -1.20711f, 0.707107f},
        {0.707107f, -1.20711f, 0.0f},
        {0.0f, -1.20711f, -0.707107f},
        {-0.707107f, -1.20711f, 0.0f},

        {0.0f, -0.5f, 1.0f},
        {1.0f, -0.5f, 0.0f},
        {0.0f, -0.5f, -1.0f},
        {-1.0f, -0.5f, 0.0f},
        {0.707107f, -0.5f, 0.707107f},
        {0.707107f, -0.5f, -0.707107f},
        {-0.707107f, -0.5f, -0.707107f},
        {-0.707107f, -0.5f, 0.707107f},

        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.707107f, 0.0f, 0.707107f},
        {0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, 0.707107f},

        {0.0f, 0.5f, 1.0f},
        {1.0f, 0.5f, 0.0f},
        {0.0f, 0.5f, -1.0f},
        {-1.0f, 0.5f, 0.0f},
        {0.707107f, 0.5f, 0.707107f},
        {0.707107f, 0.5f, -0.707107f},
        {-0.707107f, 0.5f, -0.707107f},
        {-0.707107f, 0.5f, 0.707107f},

        {0.0f, 1.20711f, 0.707107f},
        {0.707107f, 1.20711f, 0.0f},
        {0.0f, 1.20711f, -0.707107f},
        {-0.707107f, 1.20711f, 0.0f},

        {0.0f, 1.5f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(capsule.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 0, 2, 0, 3, 0, 4,
        1, 5, 2, 6, 3, 7, 4, 8,
        5, 9, 6, 10, 7, 11, 8, 12,
        9, 6, 10, 7, 11, 8, 12, 5,

        5, 13, 6, 14, 7, 15, 8, 16,

        13, 17, 14, 18, 15, 19, 16, 20,
        17, 14, 18, 15, 19, 16, 20, 13,

        13, 21, 14, 22, 15, 23, 16, 24,

        21, 25, 22, 26, 23, 27, 24, 28,
        25, 22, 26, 23, 27, 24, 28, 21,
        21, 29, 22, 30, 23, 31, 24, 32,
        29, 33, 30, 33, 31, 33, 32, 33
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CapsuleTest)
