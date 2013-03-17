/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <TestSuite/Tester.h>

#include "Math/Matrix4.h"
#include "Physics/Line.h"

namespace Magnum { namespace Physics { namespace Test {

class LineTest: public Corrade::TestSuite::Tester {
    public:
        LineTest();

        void applyTransformation();
};

LineTest::LineTest() {
    addTests({&LineTest::applyTransformation});
}

void LineTest::applyTransformation() {
    Physics::Line3D line({1.0f, 2.0f, 3.0f}, {-1.0f, -2.0f, -3.0f});
    line.applyTransformationMatrix(Matrix4::rotation(Deg(90.0f), Vector3::zAxis()));
    CORRADE_COMPARE(line.transformedA(), Vector3(-2.0f, 1.0f, 3.0f));
    CORRADE_COMPARE(line.transformedB(), Vector3(2.0f, -1.0f, -3.0f));
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::LineTest)
