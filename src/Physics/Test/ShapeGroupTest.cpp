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
#include "Physics/Point.h"
#include "Physics/LineSegment.h"
#include "Physics/ShapeGroup.h"

namespace Magnum { namespace Physics { namespace Test {

class ShapeGroupTest: public Corrade::TestSuite::Tester {
    public:
        ShapeGroupTest();

        void copy();
        void reference();
};

ShapeGroupTest::ShapeGroupTest() {
    addTests({&ShapeGroupTest::copy,
              &ShapeGroupTest::reference});
}

void ShapeGroupTest::copy() {
    ShapeGroup3D group;
    {
        Physics::Point3D point({1.0f, 2.0f, 3.0f});
        Physics::LineSegment3D segment({2.0f, 1.0f, 30.0f}, {1.0f, -20.0f, 3.0f});

        group = !(point || segment);
    }

    /* Just to test that it doesn't crash */
    group.applyTransformationMatrix(Matrix4::translation(Vector3::xAxis(1.0f)));

    CORRADE_VERIFY(true);
}

void ShapeGroupTest::reference() {
    Physics::Point3D point({1.0f, 2.0f, 3.0f});
    Physics::LineSegment3D segment({2.0f, 1.0f, 30.0f}, {1.0f, -20.0f, 3.0f});

    ShapeGroup3D group = !(std::ref(point) || std::ref(segment));

    group.applyTransformationMatrix(Matrix4::translation(Vector3(1.0f)));

    CORRADE_VERIFY((point.transformedPosition() == Vector3(2.0f, 3.0f, 4.0f)));
    CORRADE_VERIFY((segment.transformedA() == Vector3(3.0f, 2.0f, 31.0f)));
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::ShapeGroupTest)
