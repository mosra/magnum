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

#include "Math/Constants.h"
#include "Physics/LineSegment.h"
#include "Physics/Point.h"
#include "Physics/Sphere.h"

CORRADE_TEST_MAIN(Magnum::Physics::Test::SphereTest)

namespace Magnum { namespace Physics { namespace Test {

SphereTest::SphereTest() {
    addTests(&SphereTest::applyTransformation,
             &SphereTest::collisionPoint,
             &SphereTest::collisionLine,
             &SphereTest::collisionLineSegment,
             &SphereTest::collisionSphere);
}

void SphereTest::applyTransformation() {
    Physics::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 7.0f);

    sphere.applyTransformation(Matrix4::rotation(deg(90.0f), Vector3::yAxis()));
    CORRADE_COMPARE(sphere.transformedPosition(), Vector3(3.0f, 2.0f, -1.0f));
    CORRADE_COMPARE(sphere.transformedRadius(), 7.0f);

    /* Symmetric scaling */
    sphere.applyTransformation(Matrix4::scaling(Vector3(2.0f)));
    CORRADE_COMPARE(sphere.transformedPosition(), Vector3(2.0f, 4.0f, 6.0f));
    CORRADE_COMPARE(sphere.transformedRadius(), 14.0f);

    /* Apply average scaling to radius */
    sphere.applyTransformation(Matrix4::scaling({Math::Constants<GLfloat>::sqrt3(), -Math::Constants<GLfloat>::sqrt2(), 2.0f}));
    CORRADE_COMPARE(sphere.transformedRadius(), Math::Constants<GLfloat>::sqrt3()*7.0f);
}

void SphereTest::collisionPoint() {
    Physics::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Physics::Point3D point({1.0f, 3.0f, 3.0f});
    Physics::Point3D point2({1.0f, 3.0f, 1.0f});

    randomTransformation(sphere);
    randomTransformation(point);
    randomTransformation(point2);

    VERIFY_COLLIDES(sphere, point);
    VERIFY_NOT_COLLIDES(sphere, point2);
}

void SphereTest::collisionLine() {
    Physics::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Physics::Line3D line({1.0f, 1.5f, 3.5f}, {1.0f, 2.5f, 2.5f});
    Physics::Line3D line2({1.0f, 2.0f, 5.1f}, {1.0f, 3.0f, 5.1f});

    randomTransformation(sphere);
    randomTransformation(line);
    randomTransformation(line2);

    VERIFY_COLLIDES(sphere, line);
    VERIFY_NOT_COLLIDES(sphere, line2);
}

void SphereTest::collisionLineSegment() {
    Physics::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Physics::LineSegment3D line({1.0f, 2.0f, 4.9f}, {1.0f, 2.0f, 7.0f});
    Physics::LineSegment3D line2({1.0f, 2.0f, 5.1f}, {1.0f, 2.0f, 7.0f});

    randomTransformation(sphere);
    randomTransformation(line);
    randomTransformation(line2);

    VERIFY_COLLIDES(sphere, line);
    VERIFY_NOT_COLLIDES(sphere, line2);
}

void SphereTest::collisionSphere() {
    Physics::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Physics::Sphere3D sphere1({1.0f, 3.0f, 5.0f}, 1.0f);
    Physics::Sphere3D sphere2({1.0f, 3.0f, 0.0f}, 1.0f);

    randomTransformation(sphere);
    randomTransformation(sphere1);
    randomTransformation(sphere2);

    VERIFY_COLLIDES(sphere, sphere1);
    VERIFY_NOT_COLLIDES(sphere, sphere2);
}

}}}
