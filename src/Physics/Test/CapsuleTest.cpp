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

#include "Physics/Capsule.h"
#include "Physics/Point.h"
#include "Physics/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Physics { namespace Test {

class CapsuleTest: public Corrade::TestSuite::Tester, ShapeTestBase {
    public:
        CapsuleTest();

        void applyTransformation();
        void collisionPoint();
        void collisionSphere();
};

CapsuleTest::CapsuleTest() {
    addTests({&CapsuleTest::applyTransformation,
              &CapsuleTest::collisionPoint,
              &CapsuleTest::collisionSphere});
}

void CapsuleTest::applyTransformation() {
    Physics::Capsule3D capsule({1.0f, 2.0f, 3.0f}, {-1.0f, -2.0f, -3.0f}, 7.0f);

    capsule.applyTransformationMatrix(Matrix4::rotation(Deg(90.0f), Vector3::zAxis()));
    CORRADE_COMPARE(capsule.transformedA(), Vector3(-2.0f, 1.0f, 3.0f));
    CORRADE_COMPARE(capsule.transformedB(), Vector3(2.0f, -1.0f, -3.0f));
    CORRADE_COMPARE(capsule.transformedRadius(), 7.0f);
}

    /* Apply average scaling to radius */
    capsule.applyTransformationMatrix(Matrix4::scaling({Constants::sqrt3(), -Constants::sqrt2(), 2.0f}));
    CORRADE_COMPARE(capsule.transformedRadius(), Constants::sqrt3()*7.0f);
}

void CapsuleTest::collisionPoint() {
    Physics::Capsule3D capsule({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Physics::Point3D point({2.0f, 0.0f, 0.0f});
    Physics::Point3D point1({2.9f, 1.0f, 0.0f});
    Physics::Point3D point2({1.0f, 3.1f, 0.0f});

    randomTransformation(capsule);
    randomTransformation(point);
    randomTransformation(point1);
    randomTransformation(point2);

    VERIFY_COLLIDES(capsule, point);
    VERIFY_COLLIDES(capsule, point1);
    VERIFY_NOT_COLLIDES(capsule, point2);
}

void CapsuleTest::collisionSphere() {
    Physics::Capsule3D capsule({-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f);
    Physics::Sphere3D sphere({3.0f, 0.0f, 0.0f}, 0.9f);
    Physics::Sphere3D sphere1({3.5f, 1.0f, 0.0f}, 0.6f);
    Physics::Sphere3D sphere2({1.0f, 4.1f, 0.0f}, 1.0f);

    randomTransformation(capsule);
    randomTransformation(sphere);
    randomTransformation(sphere1);
    randomTransformation(sphere2);

    VERIFY_COLLIDES(capsule, sphere);
    VERIFY_COLLIDES(capsule, sphere1);
    VERIFY_NOT_COLLIDES(capsule, sphere2);
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::CapsuleTest)
