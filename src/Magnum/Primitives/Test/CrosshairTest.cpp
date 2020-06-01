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
#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Crosshair.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CrosshairTest: TestSuite::Tester {
    explicit CrosshairTest();

    void twoDimensions();
    void threeDimensions();
};

CrosshairTest::CrosshairTest() {
    addTests({&CrosshairTest::twoDimensions,
              &CrosshairTest::threeDimensions});
}

void CrosshairTest::twoDimensions() {
    Trade::MeshData crosshair = Primitives::crosshair2D();

    CORRADE_COMPARE(crosshair.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(!crosshair.isIndexed());
    CORRADE_COMPARE(crosshair.vertexCount(), 4);
    CORRADE_COMPARE(crosshair.attributeCount(), 1);
    CORRADE_COMPARE(crosshair.attribute<Vector2>(Trade::MeshAttribute::Position)[3],
        (Vector2{0.0f, 1.0f}));
}

void CrosshairTest::threeDimensions() {
    Trade::MeshData crosshair = Primitives::crosshair3D();

    CORRADE_COMPARE(crosshair.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(!crosshair.isIndexed());
    CORRADE_COMPARE(crosshair.vertexCount(), 6);
    CORRADE_COMPARE(crosshair.attributeCount(), 1);
    CORRADE_COMPARE(crosshair.attribute<Vector3>(Trade::MeshAttribute::Position)[4],
        (Vector3{ 0.0f,  0.0f, -1.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CrosshairTest)
