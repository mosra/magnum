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
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#include "../Implementation/LineSegmentRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Test {

struct LineSegmentRendererTest: TestSuite::Tester {
    explicit LineSegmentRendererTest();

    void line2D();
    void line3D();
};

LineSegmentRendererTest::LineSegmentRendererTest() {
    addTests({&LineSegmentRendererTest::line2D,
              &LineSegmentRendererTest::line3D});
}

void LineSegmentRendererTest::line2D() {
    const Vector2 a(-2.0f, 3.0f);
    const Vector2 b(3.4f, -1.5f);
    const Matrix3 matrix = Implementation::lineSegmentRendererTransformation<2>(a, b);

    CORRADE_COMPARE(matrix.transformPoint({0.0f, 0.0f}), a);
    CORRADE_COMPARE(matrix.transformPoint({1.0f, 0.0f}), b);
}

void LineSegmentRendererTest::line3D() {
    const Vector3 a(-2.0f, 3.0f, 1.5f);
    const Vector3 b(3.4f, -1.5f, 0.5f);
    const Matrix4 matrix = Implementation::lineSegmentRendererTransformation<3>(a, b);

    CORRADE_COMPARE(matrix.transformPoint({0.0f, 0.0f, 0.0f}), a);
    CORRADE_COMPARE(matrix.transformPoint({1.0f, 0.0f, 0.0f}), b);
}

}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::LineSegmentRendererTest)
