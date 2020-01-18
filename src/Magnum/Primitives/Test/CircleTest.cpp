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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CircleTest: TestSuite::Tester {
    explicit CircleTest();

    void solid2D();
    void solid2DTextureCoords();

    void solid3D();
    void solid3DTextureCoords();

    void wireframe2D();
    void wireframe3D();
};

CircleTest::CircleTest() {
    addTests({&CircleTest::solid2D,
              &CircleTest::solid2DTextureCoords,

              &CircleTest::solid3D,
              &CircleTest::solid3DTextureCoords,

              &CircleTest::wireframe2D,
              &CircleTest::wireframe3D});
}

void CircleTest::solid2D() {
    Trade::MeshData circle = Primitives::circle2DSolid(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 1);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 0.0f,  0.0f},
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 1.0f,  0.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::solid2DTextureCoords() {
    Trade::MeshData circle = Primitives::circle2DSolid(8, Primitives::CircleTextureCoords::Generate);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 2);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 0.0f,  0.0f},
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 1.0f,  0.0f}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
        {0.5f, 0.5f},
        {1.0f, 0.5f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
        {0.5f, 1.0f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
        {0.0f, 0.5f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
        {0.5f, 0.0f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
        {1.0f, 0.5f}
    }), TestSuite::Compare::Container);
}

void CircleTest::solid3D() {
    Trade::MeshData circle = Primitives::circle3DSolid(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 2);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::solid3DTextureCoords() {
    Trade::MeshData circle = Primitives::circle3DSolid(8, Primitives::CircleTextureCoords::Generate);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 3);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
        {0.5f, 0.5f},
        {1.0f, 0.5f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
        {0.5f, 1.0f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
        {0.0f, 0.5f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
        {0.5f, 0.0f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
        {1.0f, 0.5f}
    }), TestSuite::Compare::Container);
}

void CircleTest::wireframe2D() {
    Trade::MeshData circle = Primitives::circle2DWireframe(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 1);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::wireframe3D() {
    Trade::MeshData circle = Primitives::circle3DWireframe(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 1);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CircleTest)
