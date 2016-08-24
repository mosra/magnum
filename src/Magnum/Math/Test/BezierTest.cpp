/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Ashwin Ravichandran <ashwinravichandran24@gmail.com>

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

#include <Corrade/TestSuite/Tester.h>
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Functions.h"


namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector<2, Float> Vector2;

struct BezierTest : Corrade::TestSuite::Tester {
    explicit BezierTest();

    void implicitConstructor();

    void quadratic();

    void cubic();
};

BezierTest::BezierTest() {
    addTests({&BezierTest::implicitConstructor,
              &BezierTest::quadratic,
              &BezierTest::cubic});
}

void BezierTest::implicitConstructor() {
    QuadraticBezier2D<Float> bezier;
    Vector2 zero;
    for(int i = 0; i < 3; ++i) {
        CORRADE_COMPARE(bezier[i], zero);
    }
}

void BezierTest::quadratic() {
    Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f);
    QuadraticBezier2D<Float> bezier(p0, p1, p2);
    for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
        Vector2 expected = Math::pow<2>(1 - t)*p0 + 2*(1 - t)*t*p1 + Math::pow<2>(t)*p2;
        CORRADE_COMPARE(bezier.lerp(t), expected);
    }
}

void BezierTest::cubic() {
    Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f), p3(5.0f, -20.0f);
    CubicBezier2D<Float> bezier(p0, p1, p2, p3);
    for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
        Vector2 expected = Math::pow<3>(1 - t)*p0
                           + 3*Math::pow<2>(1 - t)*t*p1
                           + 3*(1 - t)*Math::pow<2>(t)*p2
                           + Math::pow<3>(t)*p3;
        CORRADE_COMPARE(bezier.lerp(t), expected);
    }
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BezierTest)
