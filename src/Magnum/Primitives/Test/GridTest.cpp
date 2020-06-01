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
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct GridTest: TestSuite::Tester {
    explicit GridTest();

    void solid3D();
    void wireframe3D();
};

constexpr struct {
    const char* name;
    GridFlags flags;
} Solid3DData[] {
    {"", GridFlags{}},
    {"normals", GridFlag::Normals},
    {"texture coordinates", GridFlag::TextureCoordinates},
    {"tangents", GridFlag::Tangents},
    {"normals + tangents", GridFlag::Normals|GridFlag::Tangents},
    {"all", GridFlag::TextureCoordinates|GridFlag::Normals|GridFlag::Tangents}
};

GridTest::GridTest() {
    addInstancedTests({&GridTest::solid3D},
        Containers::arraySize(Solid3DData));

    addTests({&GridTest::wireframe3D});
}

void GridTest::solid3D() {
    auto&& data = Solid3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData grid = grid3DSolid({5, 3}, data.flags);

    CORRADE_COMPARE(grid.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(grid.isIndexed());

    CORRADE_COMPARE_AS(grid.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {-1.0f, -1.0f, 0.0f},
        {-0.666667f, -1.0f, 0.0f},
        {-0.333333f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.333333f, -1.0f, 0.0f},
        {0.666667f, -1.0f, 0.0f},
        {1.0f, -1.0f, 0.0f},

        {-1.0f, -0.5f, 0.0f},
        {-0.666667f, -0.5f, 0.0f},
        {-0.333333f, -0.5f, 0.0f},
        {0.0f, -0.5f, 0.0f},
        {0.333333f, -0.5f, 0.0f},
        {0.666667f, -0.5f, 0.0f},
        {1.0f, -0.5f, 0.0f},

        {-1.0f, 0.0f, 0.0f},
        {-0.666667f, 0.0f, 0.0f},
        {-0.333333f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.333333f, 0.0f, 0.0f},
        {0.666667f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},

        {-1.0f, 0.5f, 0.0f},
        {-0.666667f, 0.5f, 0.0f},
        {-0.333333f, 0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f},
        {0.333333f, 0.5f, 0.0f},
        {0.666667f, 0.5f, 0.0f},
        {1.0f, 0.5f, 0.0f},

        {-1.0f, 1.0f, 0.0f},
        {-0.666667f, 1.0f, 0.0f},
        {-0.333333f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.333333f, 1.0f, 0.0f},
        {0.666667f, 1.0f, 0.0f},
        {1.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & GridFlag::Tangents) {
        CORRADE_COMPARE_AS(grid.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},

            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!grid.hasAttribute(Trade::MeshAttribute::Tangent));

    if(data.flags & GridFlag::Normals) {
        CORRADE_COMPARE_AS(grid.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!grid.hasAttribute(Trade::MeshAttribute::Normal));

    if(data.flags & GridFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(grid.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.0f, 0.0f},
            {0.166667f, 0.0f},
            {0.333333f, 0.0f},
            {0.5f, 0.0f},
            {0.666667f, 0.0f},
            {0.833333f, 0.0f},
            {1.0f, 0.0f},

            {0.0f, 0.25f},
            {0.166667f, 0.25f},
            {0.333333f, 0.25f},
            {0.5f, 0.25f},
            {0.666667f, 0.25f},
            {0.833333f, 0.25f},
            {1.0f, 0.25f},

            {0.0f, 0.5f},
            {0.166667f, 0.5f},
            {0.333333f, 0.5f},
            {0.5f, 0.5f},
            {0.666667f, 0.5f},
            {0.833333f, 0.5f},
            {1.0f, 0.5f},

            {0.0f, 0.75f},
            {0.166667f, 0.75f},
            {0.333333f, 0.75f},
            {0.5f, 0.75f},
            {0.666667f, 0.75f},
            {0.833333f, 0.75f},
            {1.0f, 0.75f},

            {0.0f, 1.0f},
            {0.166667f, 1.0f},
            {0.333333f, 1.0f},
            {0.5f, 1.0f},
            {0.666667f, 1.0f},
            {0.833333f, 1.0f},
            {1.0f, 1.0f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!grid.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags >= (GridFlag::Tangents|GridFlag::Normals)) {
        auto tangents = grid.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = grid.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }

    CORRADE_COMPARE_AS(grid.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 8, 7, 0, 1, 8,
        1, 9, 8, 1, 2, 9,
        2, 10, 9, 2, 3, 10,
        3, 11, 10, 3, 4, 11,
        4, 12, 11, 4, 5, 12,
        5, 13, 12, 5, 6, 13,

        7, 15, 14, 7, 8, 15,
        8, 16, 15, 8, 9, 16,
        9, 17, 16, 9, 10, 17,
        10, 18, 17, 10, 11, 18,
        11, 19, 18, 11, 12, 19,
        12, 20, 19, 12, 13, 20,

        14, 22, 21, 14, 15, 22,
        15, 23, 22, 15, 16, 23,
        16, 24, 23, 16, 17, 24,
        17, 25, 24, 17, 18, 25,
        18, 26, 25, 18, 19, 26,
        19, 27, 26, 19, 20, 27,

        21, 29, 28, 21, 22, 29,
        22, 30, 29, 22, 23, 30,
        23, 31, 30, 23, 24, 31,
        24, 32, 31, 24, 25, 32,
        25, 33, 32, 25, 26, 33,
        26, 34, 33, 26, 27, 34
    }), TestSuite::Compare::Container);
}

void GridTest::wireframe3D() {
    Trade::MeshData grid = grid3DWireframe({5, 3});

    CORRADE_COMPARE(grid.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(grid.isIndexed());
    CORRADE_COMPARE(grid.attributeCount(), 1);

    CORRADE_COMPARE_AS(grid.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {-1.0f, -1.0f, 0.0f},
        {-0.666667f, -1.0f, 0.0f},
        {-0.333333f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.333333f, -1.0f, 0.0f},
        {0.666667f, -1.0f, 0.0f},
        {1.0f, -1.0f, 0.0f},

        {-1.0f, -0.5f, 0.0f},
        {-0.666667f, -0.5f, 0.0f},
        {-0.333333f, -0.5f, 0.0f},
        {0.0f, -0.5f, 0.0f},
        {0.333333f, -0.5f, 0.0f},
        {0.666667f, -0.5f, 0.0f},
        {1.0f, -0.5f, 0.0f},

        {-1.0f, 0.0f, 0.0f},
        {-0.666667f, 0.0f, 0.0f},
        {-0.333333f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.333333f, 0.0f, 0.0f},
        {0.666667f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},

        {-1.0f, 0.5f, 0.0f},
        {-0.666667f, 0.5f, 0.0f},
        {-0.333333f, 0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f},
        {0.333333f, 0.5f, 0.0f},
        {0.666667f, 0.5f, 0.0f},
        {1.0f, 0.5f, 0.0f},

        {-1.0f, 1.0f, 0.0f},
        {-0.666667f, 1.0f, 0.0f},
        {-0.333333f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.333333f, 1.0f, 0.0f},
        {0.666667f, 1.0f, 0.0f},
        {1.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(grid.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 0, 7,
        1, 2, 1, 8,
        2, 3, 2, 9,
        3, 4, 3, 10,
        4, 5, 4, 11,
        5, 6, 5, 12,
        6, 13,

        7, 8, 7, 14,
        8, 9, 8, 15,
        9, 10, 9, 16,
        10, 11, 10, 17,
        11, 12, 11, 18,
        12, 13, 12, 19,
        13, 20,

        14, 15, 14, 21,
        15, 16, 15, 22,
        16, 17, 16, 23,
        17, 18, 17, 24,
        18, 19, 18, 25,
        19, 20, 19, 26,
        20, 27,

        21, 22, 21, 28,
        22, 23, 22, 29,
        23, 24, 23, 30,
        24, 25, 24, 31,
        25, 26, 25, 32,
        26, 27, 26, 33,
        27, 34,

        28, 29,
        29, 30,
        30, 31,
        31, 32,
        32, 33,
        33, 34
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::GridTest)
