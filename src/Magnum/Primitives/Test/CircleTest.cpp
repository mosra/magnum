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
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CircleTest: TestSuite::Tester {
    explicit CircleTest();

    void solid2D();
    void solid3D();
    void wireframe2D();
    void wireframe3D();
};

CircleTest::CircleTest() {
    addTests({&CircleTest::solid2D,
              &CircleTest::solid3D,
              &CircleTest::wireframe2D,
              &CircleTest::wireframe3D});
}

void CircleTest::solid2D() {
    Trade::MeshData2D circle = Primitives::circle2DSolid(8);

    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_COMPARE_AS(circle.positions(0), (std::vector<Vector2>{
        { 0.0f,  0.0f},
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 1.0f,  0.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::solid3D() {
    Trade::MeshData3D circle = Primitives::circle3DSolid(8);

    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_COMPARE_AS(circle.positions(0), (std::vector<Vector3>{
        { 0.0f,  0.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(circle.normals(0), (std::vector<Vector3>{
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

void CircleTest::wireframe2D() {
    Trade::MeshData2D circle = Primitives::circle2DWireframe(8);

    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE_AS(circle.positions(0), (std::vector<Vector2>{
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::wireframe3D() {
    Trade::MeshData3D circle = Primitives::circle3DWireframe(8);

    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE_AS(circle.positions(0), (std::vector<Vector3>{
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CircleTest)
