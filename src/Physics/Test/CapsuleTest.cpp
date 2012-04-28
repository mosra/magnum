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

#include "CapsuleTest.h"

#include <QtTest/QTest>

#include "Physics/Capsule.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::CapsuleTest)

namespace Magnum { namespace Physics { namespace Test {

void CapsuleTest::applyTransformation() {
    Physics::Capsule capsule({1.0f, 2.0f, 3.0f}, {-1.0f, -2.0f, -3.0f}, 7.0f);

    capsule.applyTransformation(Matrix4::rotation(deg(90.0f), Vector3::zAxis()));
    QVERIFY((capsule.transformedA() == Vector3(-2.0f, 1.0f, 3.0f)));
    QVERIFY((capsule.transformedB() == Vector3(2.0f, -1.0f, -3.0f)));
    QCOMPARE(capsule.radius(), 7.0f);

    /* Apply average scaling to radius */
    capsule.applyTransformation(Matrix4::scaling({Math::Constants<GLfloat>::Sqrt3, -Math::Constants<GLfloat>::Sqrt2, 2.0f}));
    QCOMPARE(capsule.transformedRadius(), Math::Constants<GLfloat>::Sqrt3*7.0f);
}

void CapsuleTest::collisionPoint() {
    Physics::Capsule capsule({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Physics::Point point({2.0f, 0.0f, 0.0f});
    Physics::Point point1({2.9f, 1.0f, 0.0f});
    Physics::Point point2({1.0f, 3.1f, 0.0f});

    randomTransformation(capsule);
    randomTransformation(point);
    randomTransformation(point1);
    randomTransformation(point2);

    VERIFY_COLLIDES(capsule, point);
    VERIFY_COLLIDES(capsule, point1);
    VERIFY_NOT_COLLIDES(capsule, point2);
}

}}}
