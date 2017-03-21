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

#include "Magnum/Magnum.h"
#include "Magnum/Shapes/Collision.h"

namespace Magnum { namespace Shapes { namespace Test {

struct CollisionTest: TestSuite::Tester {
    explicit CollisionTest();

    void boolConversion();
    void flipped();
};

CollisionTest::CollisionTest() {
    addTests({&CollisionTest::boolConversion,
              &CollisionTest::flipped});
}

void CollisionTest::boolConversion() {
    CORRADE_VERIFY(!Collision3D());
    CORRADE_VERIFY(!Collision3D({}, {2.0f, 0.0f, 0.0f}, 0.0f));
    CORRADE_VERIFY(!Collision3D({}, {0.0f, 0.0f, 2.0f}, -0.1f));
    CORRADE_VERIFY(Collision3D({}, {0.0f, 1.0f, 0.0f}, 0.1f));
}

void CollisionTest::flipped() {
    const auto flipped = Collision3D({-1.0f, 0.5f, 3.0f}, {1.0f, 0.0f, 0.0f}, 0.5f).flipped();
    CORRADE_COMPARE(flipped.position(), Vector3(-1.5f, 0.5f, 3.0f));
    CORRADE_COMPARE(flipped.separationNormal(), Vector3(-1.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(flipped.separationDistance(), 0.5f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::CollisionTest)
