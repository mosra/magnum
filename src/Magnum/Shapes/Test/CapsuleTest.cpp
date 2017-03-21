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
#include "Magnum/Shapes/Capsule.h"
#include "Magnum/Shapes/Point.h"
#include "Magnum/Shapes/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

struct CapsuleTest: TestSuite::Tester {
    explicit CapsuleTest();

    void transformed();
    void transformedAverageScaling();
    void collisionPoint();
    void collisionSphere();
};

CapsuleTest::CapsuleTest() {
    addTests({&CapsuleTest::transformed,
              &CapsuleTest::collisionPoint,
              &CapsuleTest::collisionSphere});
}

void CapsuleTest::transformed() {
    const Shapes::Capsule3D capsule({1.0f, 2.0f, 3.0f}, {-1.0f, -2.0f, -3.0f}, 7.0f);

    const auto transformed = capsule.transformed(Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(Deg(90.0f), Vector3::zAxis()));
    CORRADE_COMPARE(transformed.a(), Vector3(-4.0f, 2.0f, 6.0f));
    CORRADE_COMPARE(transformed.b(), Vector3(4.0f, -2.0f, -6.0f));
    CORRADE_COMPARE(transformed.radius(), 14.0f);
}

void CapsuleTest::collisionPoint() {
    Shapes::Capsule3D capsule({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Shapes::Point3D point({2.0f, 0.0f, 0.0f});
    Shapes::Point3D point1({2.9f, 1.0f, 0.0f});
    Shapes::Point3D point2({1.0f, 3.1f, 0.0f});

    VERIFY_COLLIDES(capsule, point);
    VERIFY_COLLIDES(capsule, point1);
    VERIFY_NOT_COLLIDES(capsule, point2);
}

void CapsuleTest::collisionSphere() {
    Shapes::Capsule3D capsule({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Shapes::Sphere3D sphere({3.0f, 0.0f, 0.0f}, 0.9f);
    Shapes::Sphere3D sphere1({3.5f, 1.0f, 0.0f}, 0.6f);
    Shapes::Sphere3D sphere2({1.0f, 4.1f, 0.0f}, 1.0f);

    VERIFY_COLLIDES(capsule, sphere);
    VERIFY_COLLIDES(capsule, sphere1);
    VERIFY_NOT_COLLIDES(capsule, sphere2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::CapsuleTest)
