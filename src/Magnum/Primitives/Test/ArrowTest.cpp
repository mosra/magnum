/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Primitives/Arrow.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct ArrowTest: TestSuite::Tester {
    explicit ArrowTest();

    void twoDimensions();
    void twoDimensionsTransformed();
    void threeDimensions();
    void threeDimensionsTransformed();
    void threeDimensionsTangent();
};

ArrowTest::ArrowTest() {
    addTests({&ArrowTest::twoDimensions,
              &ArrowTest::twoDimensionsTransformed,
              &ArrowTest::threeDimensions,
              &ArrowTest::threeDimensionsTransformed,
              &ArrowTest::threeDimensionsTangent});
}

using namespace Math::Literals;

void ArrowTest::twoDimensions() {
    Trade::MeshData mesh = Primitives::arrow2D();
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE_AS(mesh.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 1,
        1, 2,
        1, 3
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(mesh.attributeCount(), 1);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.9f, -0.1f},
        {0.9f, 0.1f}
    }), TestSuite::Compare::Container);
}

void ArrowTest::twoDimensionsTransformed() {
    /* The arrowhead should follow line direction, length shouldn't affect the
       arrowhead size in any way */

    Trade::MeshData mesh = Primitives::arrow2D({5.0f, 20.0f}, {5.0f, 10.0f});
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        {5.0f, 20.0f},
        {5.0f, 10.0f},
        {4.9f, 10.1f},
        {5.1f, 10.1f}
    }), TestSuite::Compare::Container);
}

void ArrowTest::threeDimensions() {
    Trade::MeshData mesh = Primitives::arrow3D();
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE_AS(mesh.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 1,
        1, 2,
        1, 3
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(mesh.attributeCount(), 1);
    CORRADE_COMPARE_AS(mesh.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.9f, -0.1f, 0.0f},
        {0.9f, 0.1f, 0.0f}
    }), TestSuite::Compare::Container);
}

void ArrowTest::threeDimensionsTransformed() {
    /* The arrowhead should follow line direction, length shouldn't affect the
       arrowhead size in any way */

    Trade::MeshData mesh = Primitives::arrow3D({5.0f, 20.0f, 3.0f}, {5.0f, 10.0f, 3.0f});
    CORRADE_COMPARE_AS(mesh.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {5.0f, 20.0f, 3.0f},
        {5.0f, 10.0f, 3.0f},
        {4.9f, 10.1f, 3.0f},
        {5.1f, 10.1f, 3.0f}
    }), TestSuite::Compare::Container);
}

void ArrowTest::threeDimensionsTangent() {
    /* Arrow on a -Z plane with a X tangent should be the same as the default
       one rotated by 90° around Y */

    Trade::MeshData mesh = Primitives::arrow3D({}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f});
    Trade::MeshData rotated = MeshTools::transform3D(Primitives::arrow3D(), Matrix4::rotationY(90.0_degf));
    CORRADE_COMPARE_AS(
        mesh.attribute<Vector3>(Trade::MeshAttribute::Position),
        rotated.attribute<Vector3>(Trade::MeshAttribute::Position),
        TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::ArrowTest)
