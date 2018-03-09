/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include "Magnum/Math/Vector2.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Trade/MeshData2D.h"

namespace Magnum { namespace Primitives { namespace Test {

struct SquareTest: TestSuite::Tester {
    explicit SquareTest();

    void solid();
    void wireframe();
};

SquareTest::SquareTest() {
    addTests({&SquareTest::solid,
              &SquareTest::wireframe});
}

void SquareTest::solid() {
    Trade::MeshData2D square = Primitives::Square::solid();

    CORRADE_VERIFY(!square.isIndexed());
    CORRADE_COMPARE(square.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_COMPARE(square.positions(0).size(), 4);
}

void SquareTest::wireframe() {
    Trade::MeshData2D square = Primitives::Square::wireframe();

    CORRADE_VERIFY(!square.isIndexed());
    CORRADE_COMPARE(square.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(square.positions(0).size(), 4);
}

}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::SquareTest)

