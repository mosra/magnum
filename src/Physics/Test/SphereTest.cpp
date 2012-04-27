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

#include "SphereTest.h"

#include <QtTest/QTest>
#include "Utility/Debug.h"

#include "Physics/Sphere.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::SphereTest)

namespace Magnum { namespace Physics { namespace Test {

void SphereTest::applyTransformation() {
    Physics::Sphere sphere({1.0f, 2.0f, 3.0f}, 7.0f);

    sphere.applyTransformation(Matrix4::rotation(deg(90.0f), Vector3::yAxis()));
    QVERIFY((sphere.transformedPosition() == Vector3(3.0f, 2.0f, -1.0f)));
    QCOMPARE(sphere.transformedRadius(), 7.0f);

    /* Symmetric scaling */
    sphere.applyTransformation(Matrix4::scaling(Vector3(2.0f)));
    QVERIFY((sphere.transformedPosition() == Vector3(2.0f, 4.0f, 6.0f)));
    QCOMPARE(sphere.transformedRadius(), 14.0f);

    /* Apply average scaling to radius */
    sphere.applyTransformation(Matrix4::scaling({Math::Constants<GLfloat>::Sqrt3, -Math::Constants<GLfloat>::Sqrt2, 2.0f}));
    QCOMPARE(sphere.transformedRadius(), Math::Constants<GLfloat>::Sqrt3*7.0f);
}

void SphereTest::collisionPoint() {
    Physics::Sphere sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Physics::Point point({1.0f, 3.0f, 3.0f});
    Physics::Point point2({1.0f, 3.0f, 1.0f});

    randomTransformation(sphere);
    randomTransformation(point);
    randomTransformation(point2);

    VERIFY_COLLIDES(sphere, point);
    VERIFY_NOT_COLLIDES(sphere, point2);
}

}}}
