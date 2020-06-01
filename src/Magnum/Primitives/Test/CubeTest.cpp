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
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CubeTest: TestSuite::Tester {
    explicit CubeTest();

    void solid();
    void solidStrip();
    void solidStripGlsl();
    void wireframe();
};

CubeTest::CubeTest() {
    addTests({&CubeTest::solid,
              &CubeTest::solidStrip,
              &CubeTest::solidStripGlsl,
              &CubeTest::wireframe});
}

void CubeTest::solid() {
    Trade::MeshData cube = Primitives::cubeSolid();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cube.isIndexed());
    CORRADE_COMPARE(cube.indexCount(), 36);
    CORRADE_COMPARE(cube.vertexCount(), 24);
    CORRADE_COMPARE(cube.attributeCount(), 2);
    CORRADE_COMPARE(cube.indices<UnsignedShort>()[17], 11);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[4],
        (Vector3{1.0f, -1.0f, 1.0f}));
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Normal)[6],
        (Vector3{1.0f, 0.0f, 0.0f}));
}

void CubeTest::solidStrip() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!cube.isIndexed());
    CORRADE_COMPARE(cube.vertexCount(), 14);
    CORRADE_COMPARE(cube.attributeCount(), 1);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[4],
        (Vector3{-1.0f, -1.0f, -1.0f}));
}

void CubeTest::solidStripGlsl() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();

    /* Yes, really. */
    auto solidStripVertex = [](UnsignedInt gl_VertexID) {
        typedef Vector3 vec3;
        #define mix Math::lerp
        #include "data.glsl"
        #undef mix
        return corner;
    };

    auto vertices = cube.attribute<Vector3>(Trade::MeshAttribute::Position);
    for(UnsignedInt i = 0; i != cube.vertexCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(solidStripVertex(i), vertices[i]);
    }
}

void CubeTest::wireframe() {
    Trade::MeshData cube = Primitives::cubeWireframe();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(cube.isIndexed());
    CORRADE_COMPARE(cube.indexCount(), 24);
    CORRADE_COMPARE(cube.vertexCount(), 8);
    CORRADE_COMPARE(cube.attributeCount(), 1);
    CORRADE_COMPARE(cube.indices<UnsignedShort>()[5], 3);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[5],
        (Vector3{1.0f, -1.0f, -1.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CubeTest)
