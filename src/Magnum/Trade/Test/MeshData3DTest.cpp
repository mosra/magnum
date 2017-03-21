/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Trade { namespace Test {

struct MeshData3DTest: TestSuite::Tester {
    explicit MeshData3DTest();

    void construct();
    void constructNonIndexed();
    void constructNoNormals();
    void constructNoTexCoords();
    void constructNoColors();
    void constructCopy();
    void constructMove();
};

MeshData3DTest::MeshData3DTest() {
    addTests({&MeshData3DTest::construct,
              &MeshData3DTest::constructNonIndexed,
              &MeshData3DTest::constructNoNormals,
              &MeshData3DTest::constructNoTexCoords,
              &MeshData3DTest::constructNoColors,
              &MeshData3DTest::constructCopy,
              &MeshData3DTest::constructMove});
}

using namespace Math::Literals;

void MeshData3DTest::construct() {
    const int a{};
    const MeshData3D data{MeshPrimitive::Lines, {12, 1, 0},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}},
         {{1.4f, 0.2f, 0.5f}, {1.1f, 0.13f, -0.3f}}},
        {{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}},
        {{{0.0f, 0.0f}, {0.3f, 0.7f}},
         {{0.1f, 0.2f}, {0.7f, 1.0f}},
         {{0.0f, 0.0f}, {1.0f, 1.0f}}},
        {{0xff98ab_rgbf, 0xff3366_rgbf}},
        &a};

    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Lines);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indices(), (std::vector<UnsignedInt>{12, 1, 0}));

    CORRADE_COMPARE(data.positionArrayCount(), 2);
    CORRADE_COMPARE(data.positions(0), (std::vector<Vector3>{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}));
    CORRADE_COMPARE(data.positions(1), (std::vector<Vector3>{{1.4f, 0.2f, 0.5f}, {1.1f, 0.13f, -0.3f}}));

    CORRADE_VERIFY(data.hasNormals());
    CORRADE_COMPARE(data.normalArrayCount(), 1);
    CORRADE_COMPARE(data.normals(0), (std::vector<Vector3>{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}));

    CORRADE_VERIFY(data.hasTextureCoords2D());
    CORRADE_COMPARE(data.textureCoords2DArrayCount(), 3);
    CORRADE_COMPARE(data.textureCoords2D(0), (std::vector<Vector2>{{0.0f, 0.0f}, {0.3f, 0.7f}}));
    CORRADE_COMPARE(data.textureCoords2D(1), (std::vector<Vector2>{{0.1f, 0.2f}, {0.7f, 1.0f}}));
    CORRADE_COMPARE(data.textureCoords2D(2), (std::vector<Vector2>{{0.0f, 0.0f}, {1.0f, 1.0f}}));

    CORRADE_VERIFY(data.hasColors());
    CORRADE_COMPARE(data.colorArrayCount(), 1);
    CORRADE_COMPARE(data.colors(0), (std::vector<Color4>{0xff98ab_rgbf, 0xff3366_rgbf}));

    CORRADE_COMPARE(data.importerState(), &a);
}

void MeshData3DTest::constructNonIndexed() {
    const int a{};
    const MeshData3D data{MeshPrimitive::Lines, {},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}},
        {{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}},
        {{{0.0f, 0.0f}, {0.3f, 0.7f}}},
        {{0xff98ab_rgbf, 0xff3366_rgbf}},
        &a};

    CORRADE_VERIFY(!data.isIndexed());
}

void MeshData3DTest::constructNoNormals() {
    const int a{};
    const MeshData3D data{MeshPrimitive::Lines, {12, 1, 0},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}},
        {},
        {{{0.0f, 0.0f}, {0.3f, 0.7f}}},
        {{0xff98ab_rgbf, 0xff3366_rgbf}},
        &a};

    CORRADE_VERIFY(!data.hasNormals());
    CORRADE_COMPARE(data.normalArrayCount(), 0);
}

void MeshData3DTest::constructNoTexCoords() {
    const int a{};
    const MeshData3D data{MeshPrimitive::Lines, {12, 1, 0},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}},
        {{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}},
        {},
        {{0xff98ab_rgbf, 0xff3366_rgbf}},
        &a};

    CORRADE_VERIFY(!data.hasTextureCoords2D());
    CORRADE_COMPARE(data.textureCoords2DArrayCount(), 0);
}

void MeshData3DTest::constructNoColors() {
    const int a{};
    const MeshData3D data{MeshPrimitive::Lines, {12, 1, 0},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}},
        {{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}},
        {{{0.0f, 0.0f}, {0.3f, 0.7f}}},
        {},
        &a};

    CORRADE_VERIFY(!data.hasColors());
    CORRADE_COMPARE(data.colorArrayCount(), 0);
}

void MeshData3DTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MeshData3D, const MeshData3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshData3D, const MeshData3D&>{}));
}

void MeshData3DTest::constructMove() {
    const int a{};
    MeshData3D data{MeshPrimitive::LineStrip, {12, 1, 0},
        {{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}},
        {{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}},
        {{{0.0f, 0.0f}, {0.3f, 0.7f}}},
        {{0xff98ab_rgbf, 0xff3366_rgbf}},
        &a};

    MeshData3D b{std::move(data)};

    CORRADE_COMPARE(b.primitive(), MeshPrimitive::LineStrip);
    CORRADE_VERIFY(b.isIndexed());
    CORRADE_COMPARE(b.indices(), (std::vector<UnsignedInt>{12, 1, 0}));
    CORRADE_COMPARE(b.positionArrayCount(), 1);
    CORRADE_COMPARE(b.positions(0), (std::vector<Vector3>{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}));
    CORRADE_COMPARE(b.normalArrayCount(), 1);
    CORRADE_COMPARE(b.normals(0), (std::vector<Vector3>{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}));
    CORRADE_COMPARE(b.textureCoords2DArrayCount(), 1);
    CORRADE_COMPARE(b.textureCoords2D(0), (std::vector<Vector2>{{0.0f, 0.0f}, {0.3f, 0.7f}}));
    CORRADE_COMPARE(b.colorArrayCount(), 1);
    CORRADE_COMPARE(b.colors(0), (std::vector<Color4>{0xff98ab_rgbf, 0xff3366_rgbf}));
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    MeshData3D d{MeshPrimitive::TriangleFan, {},
        {{}},
        {},
        {{}},
        {},
        &c};
    d = std::move(b);
    CORRADE_COMPARE(d.primitive(), MeshPrimitive::LineStrip);
    CORRADE_VERIFY(d.isIndexed());
    CORRADE_COMPARE(d.indices(), (std::vector<UnsignedInt>{12, 1, 0}));
    CORRADE_COMPARE(d.positionArrayCount(), 1);
    CORRADE_COMPARE(d.positions(0), (std::vector<Vector3>{{0.5f, 1.0f, 0.1f}, {-1.0f, 0.3f, -1.0f}}));
    CORRADE_COMPARE(d.normalArrayCount(), 1);
    CORRADE_COMPARE(d.normals(0), (std::vector<Vector3>{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}));
    CORRADE_COMPARE(d.textureCoords2DArrayCount(), 1);
    CORRADE_COMPARE(d.textureCoords2D(0), (std::vector<Vector2>{{0.0f, 0.0f}, {0.3f, 0.7f}}));
    CORRADE_COMPARE(d.colorArrayCount(), 1);
    CORRADE_COMPARE(d.colors(0), (std::vector<Color4>{0xff98ab_rgbf, 0xff3366_rgbf}));
    CORRADE_COMPARE(d.importerState(), &a);
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MeshData3DTest)
