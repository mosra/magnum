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

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shapes/LineSegment.h"
#include "Magnum/Shapes/Point.h"
#include "Magnum/Shapes/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

struct SphereTest: TestSuite::Tester {
    explicit SphereTest();

    void transformed();
    void collisionPoint();
    void collisionPointInverted();
    void collisionLine();
    void collisionLineSegment();
    void collisionSphere();
    void collisionSphereInverted();
};

SphereTest::SphereTest() {
    addTests({&SphereTest::transformed,
              &SphereTest::collisionPoint,
              &SphereTest::collisionPointInverted,
              &SphereTest::collisionLine,
              &SphereTest::collisionLineSegment,
              &SphereTest::collisionSphere,
              &SphereTest::collisionSphereInverted});
}

void SphereTest::transformed() {
    const Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 7.0f);

    const auto transformed = sphere.transformed(Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(Deg(90.0f), Vector3::yAxis()));
    CORRADE_COMPARE(transformed.position(), Vector3(6.0f, 4.0f, -2.0f));
    CORRADE_COMPARE(transformed.radius(), 14.0f);
}

void SphereTest::collisionPoint() {
    const Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);

    /* Collision */
    const Shapes::Point3D point({2.5f, 2.0f, 3.0f});
    const Shapes::Collision3D collision = sphere/point;
    CORRADE_VERIFY(sphere%point && point%sphere);
    CORRADE_COMPARE(collision.position(), point.position());
    CORRADE_COMPARE(collision.separationNormal(), -Vector3::xAxis());
    CORRADE_COMPARE(collision.separationDistance(), 0.5f);

    /* Collision, flipped */
    CORRADE_COMPARE(collision.separationNormal(), -(point/sphere).separationNormal());

    /* Collision with ambiguous separation vector */
    const Shapes::Point3D point2(sphere.position());
    const Shapes::Collision3D collision2 = sphere/point2;
    CORRADE_COMPARE(collision2.position(), point2.position());
    CORRADE_COMPARE(collision2.separationNormal(), Vector3::yAxis());
    CORRADE_COMPARE(collision2.separationDistance(), 2.0f);

    /* No collision */
    const Shapes::Point3D point3({-1.5f, 2.0f, 3.0f});
    CORRADE_VERIFY(!(sphere%point3) && !(sphere/point3));
}

void SphereTest::collisionPointInverted() {
    const Shapes::InvertedSphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);

    /* Collision */
    const Shapes::Point3D point({-1.5f, 2.0f, 3.0f});
    const Shapes::Collision3D collision = sphere/point;
    CORRADE_VERIFY(sphere%point && point%sphere);
    CORRADE_COMPARE(collision.position(), point.position());
    CORRADE_COMPARE(collision.separationNormal(), -Vector3::xAxis());
    CORRADE_COMPARE(collision.separationDistance(), 0.5f);

    /* Collision, flipped */
    CORRADE_COMPARE(collision.separationNormal(), -(point/sphere).separationNormal());

    /* No collision */
    const Shapes::Point3D point3({2.0f, 2.0f, 3.0f});
    CORRADE_VERIFY(!(sphere%point3) && !(sphere/point3));
}

void SphereTest::collisionLine() {
    Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Shapes::Line3D line({1.0f, 1.5f, 3.5f}, {1.0f, 2.5f, 2.5f});
    Shapes::Line3D line2({1.0f, 2.0f, 5.1f}, {1.0f, 3.0f, 5.1f});

    VERIFY_COLLIDES(sphere, line);
    VERIFY_NOT_COLLIDES(sphere, line2);
}

void SphereTest::collisionLineSegment() {
    Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Shapes::LineSegment3D line({1.0f, 2.0f, 4.9f}, {1.0f, 2.0f, 7.0f});
    Shapes::LineSegment3D line2({1.0f, 2.0f, 5.1f}, {1.0f, 2.0f, 7.0f});

    VERIFY_COLLIDES(sphere, line);
    VERIFY_NOT_COLLIDES(sphere, line2);
}

void SphereTest::collisionSphere() {
    const Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);

    /* Collision */
    const Shapes::Sphere3D sphere1({3.5f, 2.0f, 3.0f}, 1.0f);
    const Shapes::Collision3D collision = sphere/sphere1;
    CORRADE_VERIFY(sphere%sphere1 && sphere1%sphere);
    CORRADE_COMPARE(collision.position(), sphere1.position() - Vector3::xAxis(sphere1.radius()));
    CORRADE_COMPARE(collision.separationNormal(), -Vector3::xAxis());
    CORRADE_COMPARE(collision.separationDistance(), 0.5f);

    /* Collision, flipped */
    CORRADE_COMPARE(collision.separationNormal(), -(sphere1/sphere).separationNormal());

    /* Collision with ambiguous separation vector */
    const Shapes::Sphere3D sphere2(sphere.position(), 0.5f);
    const Shapes::Collision3D collision2 = sphere/sphere2;
    CORRADE_COMPARE(collision2.position(), sphere2.position() + Vector3::yAxis(sphere2.radius()));
    CORRADE_COMPARE(collision2.separationNormal(), Vector3::yAxis());
    CORRADE_COMPARE(collision2.separationDistance(), 2.5f);

    /* No collision */
    const Shapes::Sphere3D sphere3({-2.5f, 2.0f, 3.0f}, 1.0f);
    CORRADE_VERIFY(!(sphere%sphere3) && !(sphere/sphere3));
}

void SphereTest::collisionSphereInverted() {
    const Shapes::InvertedSphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);

    /* Collision */
    const Shapes::Sphere3D sphere1({-0.5f, 2.0f, 3.0f}, 1.0f);
    const Shapes::Collision3D collision = sphere/sphere1;
    CORRADE_VERIFY(sphere%sphere1 && sphere1%sphere);
    CORRADE_COMPARE(collision.position(), sphere1.position() - Vector3::xAxis(sphere1.radius()));
    CORRADE_COMPARE(collision.separationNormal(), -Vector3::xAxis());
    CORRADE_COMPARE(collision.separationDistance(), 0.5f);

    /* Collision, flipped */
    CORRADE_COMPARE(collision.separationNormal(), -(sphere1/sphere).separationNormal());

    /* No collision */
    const Shapes::Sphere3D sphere3({1.5f, 2.0f, 3.0f}, 1.0f);
    CORRADE_VERIFY(!(sphere%sphere3) && !(sphere/sphere3));
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::SphereTest)
