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
#include "Magnum/Shapes/Cylinder.h"
#include "Magnum/Shapes/Point.h"
#include "Magnum/Shapes/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

struct CylinderTest: TestSuite::Tester {
    explicit CylinderTest();

    void transformed();
    void transformedAverageScaling();
    void collisionPoint();
    void collisionSphere();
};

CylinderTest::CylinderTest() {
    addTests({&CylinderTest::transformed,
              &CylinderTest::collisionPoint,
              &CylinderTest::collisionSphere});
}

void CylinderTest::transformed() {
    const Shapes::Cylinder3D cylinder({1.0f, 2.0f, 3.0f}, {-1.0f, -2.0f, -3.0f}, 7.0f);

    const auto transformed = cylinder.transformed(Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(Deg(90.0f), Vector3::zAxis()));
    CORRADE_COMPARE(transformed.a(), Vector3(-4.0f, 2.0f, 6.0f));
    CORRADE_COMPARE(transformed.b(), Vector3(4.0f, -2.0f, -6.0f));
    CORRADE_COMPARE(transformed.radius(), 14.0f);
}

void CylinderTest::collisionPoint() {
    Shapes::Cylinder3D cylinder({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Shapes::Point3D point({2.0f, 0.0f, 0.0f});
    Shapes::Point3D point1({1.0f, 3.1f, 0.0f});
    Shapes::Point3D point2({2.9f, -1.0f, 0.0f});

    VERIFY_COLLIDES(cylinder, point);
    VERIFY_COLLIDES(cylinder, point1);
    VERIFY_NOT_COLLIDES(cylinder, point2);
}

void CylinderTest::collisionSphere() {
    Shapes::Cylinder3D cylinder({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Shapes::Sphere3D sphere({3.0f, 0.0f, 0.0f}, 0.9f);
    Shapes::Sphere3D sphere1({1.0f, 4.1f, 0.0f}, 1.0f);
    Shapes::Sphere3D sphere2({3.5f, -1.0f, 0.0f}, 0.6f);

    VERIFY_COLLIDES(cylinder, sphere);
    VERIFY_COLLIDES(cylinder, sphere1);
    VERIFY_NOT_COLLIDES(cylinder, sphere2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::CylinderTest)
