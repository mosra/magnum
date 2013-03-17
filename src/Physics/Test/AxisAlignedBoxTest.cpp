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
#include "Physics/AxisAlignedBox.h"
#include "Physics/Point.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Physics { namespace Test {

class AxisAlignedBoxTest: public Corrade::TestSuite::Tester, ShapeTestBase {
    public:
        AxisAlignedBoxTest();

        void applyTransformation();
        void collisionPoint();
};

AxisAlignedBoxTest::AxisAlignedBoxTest() {
    addTests({&AxisAlignedBoxTest::applyTransformation,
              &AxisAlignedBoxTest::collisionPoint});
}

void AxisAlignedBoxTest::applyTransformation() {
    Physics::AxisAlignedBox3D box({-1.0f, -2.0f, -3.0f}, {1.0f, 2.0f, 3.0f});

    box.applyTransformationMatrix(Matrix4::translation(Vector3(1.0f))*Matrix4::scaling({2.0f, -1.0f, 1.5f}));
    CORRADE_COMPARE(box.transformedMin(), Vector3(-1.0f, 3.0f, -3.5f));
    CORRADE_COMPARE(box.transformedMax(), Vector3(3.0f, -1.0f, 5.5f));
}

void AxisAlignedBoxTest::collisionPoint() {
    Physics::AxisAlignedBox3D box({-1.0f, -2.0f, -3.0f}, {1.0f, 2.0f, 3.0f});
    Physics::Point3D point1({-1.5f, -1.0f, 2.0f});
    Physics::Point3D point2({0.5f, 1.0f, -2.5f});

    randomTransformation(box);
    randomTransformation(point1);
    randomTransformation(point2);

    VERIFY_NOT_COLLIDES(box, point1);
    VERIFY_COLLIDES(box, point2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::AxisAlignedBoxTest)
