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

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shapes/LineSegment.h"
#include "Magnum/Shapes/Point.h"
#include "Magnum/Shapes/Plane.h"

#include "ShapeTestBase.h"

namespace Magnum { namespace Shapes { namespace Test {

struct PlaneTest: TestSuite::Tester {
    explicit PlaneTest();

    void transformed();
    void collisionLine();
    void collisionLineSegment();
};

PlaneTest::PlaneTest() {
    addTests({&PlaneTest::transformed,
              &PlaneTest::collisionLine,
              &PlaneTest::collisionLineSegment});
}

void PlaneTest::transformed() {
    const Shapes::Plane plane({1.0f, 2.0f, 3.0f}, {Constants::sqrt2(), -Constants::sqrt2(), 0});

    /* The normal should stay normalized after scaling */
    const auto transformed = plane.transformed(Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(Deg(90.0f), Vector3::xAxis()));
    CORRADE_COMPARE(transformed.position(), Vector3(2.0f, -6.0f, 4.0f));
    CORRADE_COMPARE(transformed.normal(), Vector3(Constants::sqrt2(), 0, -Constants::sqrt2()));
}

void PlaneTest::collisionLine() {
    Shapes::Plane plane(Vector3(), Vector3::yAxis());
    Shapes::Line3D line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    Shapes::Line3D line2({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});
    Shapes::Line3D line3({0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f});

    VERIFY_COLLIDES(plane, line);
    VERIFY_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

void PlaneTest::collisionLineSegment() {
    Shapes::Plane plane(Vector3(), Vector3::yAxis());
    Shapes::LineSegment3D line({0.0f, -0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Shapes::LineSegment3D line2({0.0f, 0.1f, 0.0f}, {0.0f, 7.0f, 0.0f});
    Shapes::LineSegment3D line3({0.0f, -7.0f, 0.0f}, {0.0f, -0.1f, 0.0f});

    VERIFY_COLLIDES(plane, line);
    VERIFY_NOT_COLLIDES(plane, line2);
    VERIFY_NOT_COLLIDES(plane, line3);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::PlaneTest)
