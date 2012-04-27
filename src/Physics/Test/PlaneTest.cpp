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

#include "PlaneTest.h"

#include <QtTest/QTest>

#include "Physics/Plane.h"

QTEST_APPLESS_MAIN(Magnum::Physics::Test::PlaneTest)

namespace Magnum { namespace Physics { namespace Test {

void PlaneTest::applyTransformation() {
    Physics::Plane plane({1.0f, 2.0f, 3.0f}, {Math::Constants<float>::Sqrt2, -Math::Constants<float>::Sqrt2, 0});

    plane.applyTransformation(Matrix4::rotation(deg(90.0f), Vector3::xAxis()));
    QVERIFY(plane.transformedPosition() == Vector3(1.0f, -3.0f, 2.0f));
    QVERIFY(plane.transformedNormal() == Vector3(Math::Constants<float>::Sqrt2, 0, -Math::Constants<float>::Sqrt2));

    /* The normal should stay normalized */
    plane.applyTransformation(Matrix4::scaling({1.5f, 2.0f, 3.0f}));
    QVERIFY(plane.transformedPosition() == Vector3(1.5f, 4.0f, 9.0f));
    QVERIFY(plane.transformedNormal() == Vector3(Math::Constants<float>::Sqrt2, -Math::Constants<float>::Sqrt2, 0));
}

void PlaneTest::collisionLine() {
    Physics::Plane plane(Vector3(), Vector3::yAxis());
    Physics::Line line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    Physics::Line line2({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});
    Physics::Line line3({0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});

    randomTransformation(plane);
    randomTransformation(line);
    randomTransformation(line2);
    randomTransformation(line3);

    VERIFY_COLLIDES(plane, line);
    VERIFY_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

}}}
