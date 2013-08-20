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

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Magnum.h"
#include "Shapes/LineSegment.h"
#include "Shapes/Point.h"
#include "Shapes/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

class SphereTest: public TestSuite::Tester {
    public:
        SphereTest();

        void transformed();
        void collisionPoint();
        void collisionLine();
        void collisionLineSegment();
        void collisionSphere();
};

SphereTest::SphereTest() {
    addTests({&SphereTest::transformed,
              &SphereTest::collisionPoint,
              &SphereTest::collisionLine,
              &SphereTest::collisionLineSegment,
              &SphereTest::collisionSphere});
}

void SphereTest::transformed() {
    const Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 7.0f);

    const auto transformed = sphere.transformed(Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(Deg(90.0f), Vector3::yAxis()));
    CORRADE_COMPARE(transformed.position(), Vector3(6.0f, 4.0f, -2.0f));
    CORRADE_COMPARE(transformed.radius(), 14.0f);
}

void SphereTest::collisionPoint() {
    Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Shapes::Point3D point({1.0f, 3.0f, 3.0f});
    Shapes::Point3D point2({1.0f, 3.0f, 1.0f});

    VERIFY_COLLIDES(sphere, point);
    VERIFY_NOT_COLLIDES(sphere, point2);
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
    Shapes::Sphere3D sphere({1.0f, 2.0f, 3.0f}, 2.0f);
    Shapes::Sphere3D sphere1({1.0f, 3.0f, 5.0f}, 1.0f);
    Shapes::Sphere3D sphere2({1.0f, 3.0f, 0.0f}, 1.0f);

    VERIFY_COLLIDES(sphere, sphere1);
    VERIFY_NOT_COLLIDES(sphere, sphere2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::SphereTest)
