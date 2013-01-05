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

#include "Math/Constants.h"
#include "Physics/LineSegment.h"
#include "Physics/Point.h"
#include "Physics/Plane.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Physics { namespace Test {

class PlaneTest: public Corrade::TestSuite::Tester, ShapeTestBase {
    public:
        PlaneTest();

        void applyTransformation();
        void collisionLine();
        void collisionLineSegment();
};

PlaneTest::PlaneTest() {
    addTests(&PlaneTest::applyTransformation,
             &PlaneTest::collisionLine,
             &PlaneTest::collisionLineSegment);
}

void PlaneTest::applyTransformation() {
    Physics::Plane plane({1.0f, 2.0f, 3.0f}, {Constants::sqrt2(), -Constants::sqrt2(), 0});

    plane.applyTransformationMatrix(Matrix4::rotation(deg(90.0f), Vector3::xAxis()));
    CORRADE_COMPARE(plane.transformedPosition(), Vector3(1.0f, -3.0f, 2.0f));
    CORRADE_COMPARE(plane.transformedNormal(), Vector3(Constants::sqrt2(), 0, -Constants::sqrt2()));

    /* The normal should stay normalized */
    plane.applyTransformationMatrix(Matrix4::scaling({1.5f, 2.0f, 3.0f}));
    CORRADE_COMPARE(plane.transformedPosition(), Vector3(1.5f, 4.0f, 9.0f));
    CORRADE_COMPARE(plane.transformedNormal(), Vector3(Constants::sqrt2(), -Constants::sqrt2(), 0));
}

void PlaneTest::collisionLine() {
    Physics::Plane plane(Vector3(), Vector3::yAxis());
    Physics::Line3D line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    Physics::Line3D line2({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});
    Physics::Line3D line3({0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});

    randomTransformation(plane);
    randomTransformation(line);
    randomTransformation(line2);
    randomTransformation(line3);

    VERIFY_COLLIDES(plane, line);
    VERIFY_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

void PlaneTest::collisionLineSegment() {
    Physics::Plane plane(Vector3(), Vector3::yAxis());
    Physics::LineSegment3D line({0.0f, -0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Physics::LineSegment3D line2({0.0f, 0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Physics::LineSegment3D line3({0.0f, -7.0f, 0.0f}, {0.0f, -0.1f, 0.0f});

    randomTransformation(plane);
    randomTransformation(line);
    randomTransformation(line2);
    randomTransformation(line3);

    VERIFY_COLLIDES(plane, line);
    VERIFY_NOT_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::PlaneTest)
