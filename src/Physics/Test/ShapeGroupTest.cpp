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

#include <TestSuite/Tester.h>

#include "Math/Matrix4.h"
#include "Physics/Point.h"
#include "Physics/AxisAlignedBox.h"
#include "Physics/ShapeGroup.h"
#include "Physics/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Physics { namespace Test {

class ShapeGroupTest: public Corrade::TestSuite::Tester {
    public:
        ShapeGroupTest();

        void negated();
        void anded();
        void ored();
        void multipleUnary();
        void hierarchy();
        void empty();
};

ShapeGroupTest::ShapeGroupTest() {
    addTests({&ShapeGroupTest::negated,
              &ShapeGroupTest::anded,
              &ShapeGroupTest::ored,
              &ShapeGroupTest::multipleUnary,
              &ShapeGroupTest::hierarchy,
              &ShapeGroupTest::empty});
}

void ShapeGroupTest::negated() {
    const Physics::ShapeGroup2D a = !Physics::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.type(0), ShapeGroup2D::Type::Point);
    CORRADE_COMPARE(a.get<Physics::Point2D>(0).position(), Vector2::xAxis(0.5f));

    VERIFY_NOT_COLLIDES(a, Physics::Sphere2D({}, 1.0f));
}

void ShapeGroupTest::anded() {
    const Physics::ShapeGroup2D a = Physics::Sphere2D({}, 1.0f) && Physics::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.type(0), ShapeGroup2D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), ShapeGroup2D::Type::Point);
    CORRADE_COMPARE(a.get<Physics::Sphere2D>(0).position(), Vector2());
    CORRADE_COMPARE(a.get<Physics::Sphere2D>(0).radius(), 1.0f);
    CORRADE_COMPARE(a.get<Physics::Point2D>(1).position(), Vector2::xAxis(0.5f));

    VERIFY_NOT_COLLIDES(a, Physics::Point2D());
    VERIFY_COLLIDES(a, Physics::Sphere2D(Vector2::xAxis(0.5f), 0.25f));
}

void ShapeGroupTest::ored() {
    const Physics::ShapeGroup2D a = Physics::Sphere2D({}, 1.0f) || Physics::Point2D(Vector2::xAxis(1.5f));

    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.type(0), ShapeGroup2D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), ShapeGroup2D::Type::Point);
    CORRADE_COMPARE(a.get<Physics::Sphere2D>(0).position(), Vector2());
    CORRADE_COMPARE(a.get<Physics::Sphere2D>(0).radius(), 1.0f);
    CORRADE_COMPARE(a.get<Physics::Point2D>(1).position(), Vector2::xAxis(1.5f));

    VERIFY_COLLIDES(a, Physics::Point2D());
    VERIFY_COLLIDES(a, Physics::Sphere2D(Vector2::xAxis(1.5f), 0.25f));
}

void ShapeGroupTest::multipleUnary() {
    const Physics::ShapeGroup2D a = !!!!Physics::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.type(0), ShapeGroup2D::Type::Point);
    CORRADE_COMPARE(a.get<Physics::Point2D>(0).position(), Vector2::xAxis(0.5f));

    VERIFY_COLLIDES(a, Physics::Sphere2D({}, 1.0f));
}

void ShapeGroupTest::hierarchy() {
    const Physics::ShapeGroup3D a = Physics::Sphere3D({}, 1.0f) &&
        (Physics::Point3D(Vector3::xAxis(1.5f)) || !Physics::AxisAlignedBox3D({}, Vector3(0.5f)));

    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.type(0), ShapeGroup3D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), ShapeGroup3D::Type::Point);
    CORRADE_COMPARE(a.type(2), ShapeGroup3D::Type::AxisAlignedBox);
    CORRADE_COMPARE(a.get<Physics::Point3D>(1).position(), Vector3::xAxis(1.5f));

    VERIFY_COLLIDES(a, Physics::Sphere3D(Vector3::xAxis(1.5f), 0.6f));
    VERIFY_NOT_COLLIDES(a, Physics::Point3D(Vector3(0.25f)));
}

void ShapeGroupTest::empty() {
    const Physics::ShapeGroup2D a;

    CORRADE_COMPARE(a.size(), 0);

    VERIFY_NOT_COLLIDES(a, Physics::Sphere2D({}, 1.0f));
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::ShapeGroupTest)
