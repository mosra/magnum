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

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Axis.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Test {

struct AxisTest: TestSuite::Tester {
    explicit AxisTest();

    void twoDimensions();
    void threeDimensions();
};

AxisTest::AxisTest() {
    addTests({&AxisTest::twoDimensions,
              &AxisTest::threeDimensions});
}

void AxisTest::twoDimensions() {
    Trade::MeshData2D axis = Primitives::axis2D();

    CORRADE_COMPARE(axis.primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(axis.indices().size(), 12);
    CORRADE_COMPARE(axis.positions(0).size(), 8);
    CORRADE_COMPARE(axis.colors(0).size(), 8);
}

void AxisTest::threeDimensions() {
    Trade::MeshData3D axis = Primitives::axis3D();

    CORRADE_COMPARE(axis.primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(axis.indices().size(), 18);
    CORRADE_COMPARE(axis.positions(0).size(), 12);
    CORRADE_COMPARE(axis.colors(0).size(), 12);
}

}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::AxisTest)
