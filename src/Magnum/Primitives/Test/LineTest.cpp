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
#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Line.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct LineTest: TestSuite::Tester {
    explicit LineTest();

    void twoDimensions();
    void threeDimensions();
};

LineTest::LineTest() {
    addTests({&LineTest::twoDimensions,
              &LineTest::threeDimensions});
}

void LineTest::twoDimensions() {
    Trade::MeshData2D line = Primitives::line2D();

    CORRADE_VERIFY(!line.isIndexed());
    CORRADE_COMPARE(line.primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(line.positions(0).size(), 2);
}

void LineTest::threeDimensions() {
    Trade::MeshData3D line = Primitives::line3D();

    CORRADE_VERIFY(!line.isIndexed());
    CORRADE_COMPARE(line.primitive(), MeshPrimitive::Lines);
    CORRADE_COMPARE(line.positions(0).size(), 2);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::LineTest)
