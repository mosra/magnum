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
#include "Magnum/Math/Vector2.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct SquareTest: TestSuite::Tester {
    explicit SquareTest();

    void solid();
    void solidTextured();
    void wireframe();
};

SquareTest::SquareTest() {
    addTests({&SquareTest::solid,
              &SquareTest::solidTextured,
              &SquareTest::wireframe});
}

void SquareTest::solid() {
    Trade::MeshData square = Primitives::squareSolid();

    CORRADE_COMPARE(square.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!square.isIndexed());
    CORRADE_COMPARE(square.vertexCount(), 4);
    CORRADE_COMPARE(square.attributeCount(), 1);
    CORRADE_COMPARE(square.attribute<Vector2>(Trade::MeshAttribute::Position)[3],
        (Vector2{-1.0f, 1.0f}));
}

void SquareTest::solidTextured() {
    Trade::MeshData square = Primitives::squareSolid(Primitives::SquareFlag::TextureCoordinates);

    CORRADE_COMPARE(square.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!square.isIndexed());
    CORRADE_COMPARE(square.vertexCount(), 4);
    CORRADE_COMPARE(square.attributeCount(), 2);
    CORRADE_COMPARE(square.attribute<Vector2>(Trade::MeshAttribute::Position)[3],
        (Vector2{-1.0f, 1.0f}));
    CORRADE_COMPARE(square.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates)[1],
        (Vector2{1.0f, 1.0f}));
}

void SquareTest::wireframe() {
    Trade::MeshData square = Primitives::squareWireframe();

    CORRADE_COMPARE(square.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!square.isIndexed());
    CORRADE_COMPARE(square.vertexCount(), 4);
    CORRADE_COMPARE(square.attributeCount(), 1);
    CORRADE_COMPARE(square.attribute<Vector2>(Trade::MeshAttribute::Position)[3],
        (Vector2{-1.0f, 1.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::SquareTest)

