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

#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct IcosphereTest: TestSuite::Tester {
    explicit IcosphereTest();

    void count0();
    void data1();
    void count2();

    void wireframe();
};

IcosphereTest::IcosphereTest() {
    addTests({&IcosphereTest::count0,
              &IcosphereTest::data1,
              &IcosphereTest::count2,

              &IcosphereTest::wireframe});
}

void IcosphereTest::count0() {
    Trade::MeshData icosphere = Primitives::icosphereSolid(0);

    CORRADE_COMPARE(icosphere.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(icosphere.isIndexed());
    CORRADE_COMPARE(icosphere.indexCount(), 60);
    CORRADE_COMPARE(icosphere.vertexCount(), 12);
    CORRADE_COMPARE(icosphere.attributeCount(), 2);
    CORRADE_COMPARE(icosphere.indices<UnsignedInt>()[18], 9);
    CORRADE_COMPARE(icosphere.attribute<Vector3>(Trade::MeshAttribute::Position)[8],
        (Vector3{-0.525731f, -0.850651f, 0.0f}));
    CORRADE_COMPARE(icosphere.attribute<Vector3>(Trade::MeshAttribute::Normal)[8],
        (Vector3{-0.525731f, -0.850651f, 0.0f}));
}

void IcosphereTest::data1() {
    /* This also tests the subdivide() and removeDuplicates() mesh tools */

    Trade::MeshData icosphere = Primitives::icosphereSolid(1);

    CORRADE_COMPARE(icosphere.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(icosphere.isIndexed());
    CORRADE_COMPARE(icosphere.attributeCount(), 2);

    CORRADE_COMPARE_AS(icosphere.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        12, 13, 14, 15, 16, 12, 17, 18, 19, 17, 20, 21, 22, 23, 24, 22, 25, 26,
        27, 28, 29, 27, 30, 31, 32, 33, 34, 32, 35, 36, 37, 38, 39, 37, 40, 41,
        13, 28, 25, 14, 24, 39, 19, 26, 31, 18, 40, 23, 16, 34, 29, 15, 38, 35,
        30, 33, 20, 21, 36, 41, 1, 12, 14, 12, 2, 13, 14, 13, 6, 1, 15, 12, 15,
        7, 16, 12, 16, 2, 3, 17, 19, 17, 4, 18, 19, 18, 5, 4, 17, 21, 17, 3,
        20, 21, 20, 8, 6, 22, 24, 22, 5, 23, 24, 23, 11, 5, 22, 26, 22, 6, 25,
        26, 25, 10, 9, 27, 29, 27, 10, 28, 29, 28, 2, 10, 27, 31, 27, 9, 30,
        31, 30, 3, 7, 32, 34, 32, 8, 33, 34, 33, 9, 8, 32, 36, 32, 7, 35, 36,
        35, 0, 11, 37, 39, 37, 0, 38, 39, 38, 1, 0, 37, 41, 37, 11, 40, 41, 40,
        4, 6, 13, 25, 13, 2, 28, 25, 28, 10, 1, 14, 39, 14, 6, 24, 39, 24, 11,
        3, 19, 31, 19, 5, 26, 31, 26, 10, 5, 18, 23, 18, 4, 40, 23, 40, 11, 2,
        16, 29, 16, 7, 34, 29, 34, 9, 7, 15, 35, 15, 1, 38, 35, 38, 0, 3, 30,
        20, 30, 9, 33, 20, 33, 8, 4, 21, 41, 21, 8, 36, 41, 36, 0}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(icosphere.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -0.525731f, 0.850651f},
        {0.850651f, 0.0f, 0.525731f},
        {0.850651f, 0.0f, -0.525731f},
        {-0.850651f, 0.0f, -0.525731f},
        {-0.850651f, 0.0f, 0.525731f},
        {-0.525731f, 0.850651f, 0.0f},
        {0.525731f, 0.850651f, 0.0f},
        {0.525731f, -0.850651f, 0.0f},
        {-0.525731f, -0.850651f, 0.0f},
        {0.0f, -0.525731f, -0.850651f},
        {0.0f, 0.525731f, -0.850651f},
        {0.0f, 0.525731f, 0.850651f},
        {1.0f, 0.0f, 0.0f},
        {0.809017f, 0.5f, -0.309017f},
        {0.809017f, 0.5f, 0.309017f},
        {0.809017f, -0.5f, 0.309017f},
        {0.809017f, -0.5f, -0.309017f},
        {-1.0f, 0.0f, 0.0f},
        {-0.809017f, 0.5f, 0.309017f},
        {-0.809017f, 0.5f, -0.309017f},
        {-0.809017f, -0.5f, -0.309017f},
        {-0.809017f, -0.5f, 0.309017f},
        {0.0f, 1.0f, 0.0f},
        {-0.309017f, 0.809017f, 0.5f},
        {0.309017f, 0.809017f, 0.5f},
        {0.309017f, 0.809017f, -0.5f},
        {-0.309017f, 0.809017f, -0.5f},
        {0.0f, 0.0f, -1.0f},
        {0.5f, 0.309017f, -0.809017f},
        {0.5f, -0.309017f, -0.809017f},
        {-0.5f, -0.309017f, -0.809017f},
        {-0.5f, 0.309017f, -0.809017f},
        {0.0f, -1.0f, 0.0f},
        {-0.309017f, -0.809017f, -0.5f},
        {0.309017f, -0.809017f, -0.5f},
        {0.309017f, -0.809017f, 0.5f},
        {-0.309017f, -0.809017f, 0.5f},
        {0.0f, 0.0f, 1.0f},
        {0.5f, -0.309017f, 0.809017f},
        {0.5f, 0.309017f, 0.809017f},
        {-0.5f, 0.309017f, 0.809017f},
        {-0.5f, -0.309017f, 0.809017f}}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(
        icosphere.attribute<Vector3>(Trade::MeshAttribute::Position),
        icosphere.attribute<Vector3>(Trade::MeshAttribute::Normal),
        TestSuite::Compare::Container);
}

void IcosphereTest::count2() {
    Trade::MeshData icosphere = Primitives::icosphereSolid(2);

    CORRADE_COMPARE(icosphere.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(icosphere.isIndexed());
    CORRADE_COMPARE(icosphere.indexCount(), 960);
    CORRADE_COMPARE(icosphere.vertexCount(), 162);
    CORRADE_COMPARE(icosphere.attributeCount(), 2);
}

void IcosphereTest::wireframe() {
    Trade::MeshData icosphere = Primitives::icosphereWireframe();

    CORRADE_COMPARE(icosphere.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(icosphere.isIndexed());
    CORRADE_COMPARE(icosphere.indexCount(), 60);
    CORRADE_COMPARE(icosphere.vertexCount(), 12);
    CORRADE_COMPARE(icosphere.attributeCount(), 1);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::IcosphereTest)
