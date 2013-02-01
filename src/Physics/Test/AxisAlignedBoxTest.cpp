/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <TestSuite/Tester.h>

#include "Math/Constants.h"
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
    addTests(&AxisAlignedBoxTest::applyTransformation,
             &AxisAlignedBoxTest::collisionPoint);
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
