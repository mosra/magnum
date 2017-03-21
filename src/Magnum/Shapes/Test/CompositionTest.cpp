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

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shapes/Point.h"
#include "Magnum/Shapes/AxisAlignedBox.h"
#include "Magnum/Shapes/Composition.h"
#include "Magnum/Shapes/Sphere.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

struct CompositionTest: TestSuite::Tester {
    explicit CompositionTest();

    void negated();
    void anded();
    void ored();
    void multipleUnary();
    void hierarchy();
    void empty();

    void copy();
    void move();
    void transformed();
};

CompositionTest::CompositionTest() {
    addTests({&CompositionTest::negated,
              &CompositionTest::anded,
              &CompositionTest::ored,
              &CompositionTest::multipleUnary,
              &CompositionTest::hierarchy,
              &CompositionTest::empty,

              &CompositionTest::copy,
              &CompositionTest::move,
              &CompositionTest::transformed});
}

void CompositionTest::negated() {
    const Shapes::Composition2D a = !Shapes::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.type(0), Composition2D::Type::Point);
    CORRADE_COMPARE(a.get<Shapes::Point2D>(0).position(), Vector2::xAxis(0.5f));

    VERIFY_NOT_COLLIDES(a, Shapes::Sphere2D({}, 1.0f));
}

void CompositionTest::anded() {
    const Shapes::Composition2D a = Shapes::Sphere2D({}, 1.0f) && Shapes::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.type(0), Composition2D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), Composition2D::Type::Point);
    CORRADE_COMPARE(a.get<Shapes::Sphere2D>(0).position(), Vector2());
    CORRADE_COMPARE(a.get<Shapes::Sphere2D>(0).radius(), 1.0f);
    CORRADE_COMPARE(a.get<Shapes::Point2D>(1).position(), Vector2::xAxis(0.5f));

    VERIFY_NOT_COLLIDES(a, Shapes::Point2D());
    VERIFY_COLLIDES(a, Shapes::Sphere2D(Vector2::xAxis(0.5f), 0.25f));
}

void CompositionTest::ored() {
    const Shapes::Composition2D a = Shapes::Sphere2D({}, 1.0f) || Shapes::Point2D(Vector2::xAxis(1.5f));

    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.type(0), Composition2D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), Composition2D::Type::Point);
    CORRADE_COMPARE(a.get<Shapes::Sphere2D>(0).position(), Vector2());
    CORRADE_COMPARE(a.get<Shapes::Sphere2D>(0).radius(), 1.0f);
    CORRADE_COMPARE(a.get<Shapes::Point2D>(1).position(), Vector2::xAxis(1.5f));

    VERIFY_COLLIDES(a, Shapes::Point2D());
    VERIFY_COLLIDES(a, Shapes::Sphere2D(Vector2::xAxis(1.5f), 0.25f));
}

void CompositionTest::multipleUnary() {
    const Shapes::Composition2D a = !!!!Shapes::Point2D(Vector2::xAxis(0.5f));

    CORRADE_COMPARE(a.size(), 1);
    CORRADE_COMPARE(a.type(0), Composition2D::Type::Point);
    CORRADE_COMPARE(a.get<Shapes::Point2D>(0).position(), Vector2::xAxis(0.5f));

    VERIFY_COLLIDES(a, Shapes::Sphere2D({}, 1.0f));
}

void CompositionTest::hierarchy() {
    const Shapes::Composition3D a = Shapes::Sphere3D({}, 1.0f) &&
        (Shapes::Point3D(Vector3::xAxis(1.5f)) || !Shapes::AxisAlignedBox3D({}, Vector3(0.5f)));

    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.type(0), Composition3D::Type::Sphere);
    CORRADE_COMPARE(a.type(1), Composition3D::Type::Point);
    CORRADE_COMPARE(a.type(2), Composition3D::Type::AxisAlignedBox);
    CORRADE_COMPARE(a.get<Shapes::Point3D>(1).position(), Vector3::xAxis(1.5f));

    VERIFY_COLLIDES(a, Shapes::Sphere3D(Vector3::xAxis(1.5f), 0.6f));
    VERIFY_NOT_COLLIDES(a, Shapes::Point3D(Vector3(0.25f)));
}

void CompositionTest::empty() {
    const Shapes::Composition2D a;

    CORRADE_COMPARE(a.size(), 0);

    VERIFY_NOT_COLLIDES(a, Shapes::Sphere2D({}, 1.0f));
}

void CompositionTest::copy() {
    const Shapes::Composition3D a = Shapes::Sphere3D({}, 1.0f) &&
        (Shapes::Point3D(Vector3::xAxis(1.5f)) || !Shapes::AxisAlignedBox3D({}, Vector3(0.5f)));

    /* Copy constructor */
    const Shapes::Composition3D b(a);
    CORRADE_COMPARE(b.size(), 3);
    CORRADE_COMPARE(b.get<Shapes::AxisAlignedBox3D>(2).max(), Vector3(0.5f));

    /* Copy assignment */
    Shapes::Composition3D c;
    c = a;
    CORRADE_COMPARE(c.size(), 3);
    CORRADE_COMPARE(c.get<Shapes::Point3D>(1).position(), Vector3::xAxis(1.5f));
}

void CompositionTest::move() {
    {
        Shapes::Composition3D a = Shapes::Sphere3D({}, 1.0f) &&
            (Shapes::Point3D(Vector3::xAxis(1.5f)) || !Shapes::AxisAlignedBox3D({}, Vector3(0.5f)));

        /* Move constructor */
        const Shapes::Composition3D b(std::move(a));
        CORRADE_COMPARE(a.size(), 0);
        CORRADE_COMPARE(b.size(), 3);
        CORRADE_COMPARE(b.get<Shapes::Point3D>(1).position(), Vector3::xAxis(1.5f));
    } {
        Shapes::Composition3D a = Shapes::Sphere3D({}, 1.0f) &&
            (Shapes::Point3D(Vector3::xAxis(1.5f)) || !Shapes::AxisAlignedBox3D({}, Vector3(0.5f)));

        /* Move assignment */
        Shapes::Composition3D b;
        b = std::move(a);
        CORRADE_COMPARE(a.size(), 0);
        CORRADE_COMPARE(b.size(), 3);
        CORRADE_COMPARE(b.get<Shapes::AxisAlignedBox3D>(2).max(), Vector3(0.5f));
    }
}

void CompositionTest::transformed() {
    const Shapes::Composition2D a = Shapes::Sphere2D({}, 1.0f) &&
        (Shapes::Point2D(Vector2::xAxis(1.5f)) || !Shapes::AxisAlignedBox2D({}, Vector2(0.5f)));

    const Shapes::Composition2D b = a.transformed(Matrix3::translation({1.5f, -7.0f}));
    CORRADE_COMPARE(b.get<Shapes::Sphere2D>(0).position(), Vector2(1.5f, -7.0f));
    CORRADE_COMPARE(b.get<Shapes::Sphere2D>(0).radius(), 1.0f);
    CORRADE_COMPARE(b.get<Shapes::Point2D>(1).position(), Vector2(3.0f, -7.0f));
    CORRADE_COMPARE(b.get<Shapes::AxisAlignedBox2D>(2).min(), Vector2(1.5f, -7.0f));
    CORRADE_COMPARE(b.get<Shapes::AxisAlignedBox2D>(2).max(), Vector2(2.0f, -6.5f));
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::CompositionTest)
