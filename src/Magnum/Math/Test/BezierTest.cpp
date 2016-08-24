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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<Double> Vector2d;
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2D;
typedef Math::QuadraticBezier2D<Double> QuadraticBezier2Dd;
typedef Math::CubicBezier2D<Float> CubicBezier2D;

struct BezierTest : Corrade::TestSuite::Tester {
    explicit BezierTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void constructCopy();

    void data();

    void compare();

    void lerpQuadratic();
    void lerpCubic();

    void debug();
    void configuration();
};

BezierTest::BezierTest() {
    addTests({&BezierTest::construct,
              &BezierTest::constructDefault,
              &BezierTest::constructNoInit,
              &BezierTest::constructConversion,
              &BezierTest::constructCopy,

              &BezierTest::data,

              &BezierTest::compare,

              &BezierTest::lerpQuadratic,
              &BezierTest::lerpCubic,

              &BezierTest::debug,
              &BezierTest::configuration});
}

void BezierTest::construct() {
    /* The constructor should be implicit */
    constexpr QuadraticBezier2D a = {Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<QuadraticBezier2D, Vector2, Vector2, Vector2>::value));
}

void BezierTest::constructDefault() {
    constexpr QuadraticBezier2D a;
    constexpr QuadraticBezier2D b{ZeroInit};
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{}, Vector2{}, Vector2{}}));
    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{}, Vector2{}, Vector2{}}));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<QuadraticBezier2D>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<QuadraticBezier2D, ZeroInitT>::value));
}

void BezierTest::constructNoInit() {
    QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    new(&a) QuadraticBezier2D{NoInit};
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<QuadraticBezier2D, NoInitT>::value));
}

void BezierTest::constructConversion() {
    constexpr QuadraticBezier2Dd a{Vector2d{0.5, 1.0}, Vector2d{1.1, 0.3}, Vector2d{0.1, 1.2}};
    constexpr QuadraticBezier2D b{a};

    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<QuadraticBezier2Dd, QuadraticBezier2D>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<QuadraticBezier2D, QuadraticBezier2Dd>::value));
}

void BezierTest::constructCopy() {
    constexpr QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    constexpr QuadraticBezier2D b{a};
    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<QuadraticBezier2D>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<QuadraticBezier2D>::value);
}

void BezierTest::data() {
    QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    a[0] = {};
    a[2] = {0.7f, 20.3f};

    CORRADE_COMPARE(a[0], (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(a[2], (Vector2{0.7f, 20.3f}));
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.0f, 0.0f}, Vector2{1.1f, 0.3f}, Vector2{0.7f, 20.3f}}));

    constexpr QuadraticBezier2D b{Vector2{3.5f, 0.1f}, Vector2{1.3f, 10.3f}, Vector2{0.0f, -1.2f}};
    constexpr Vector2 c = b[2];
    CORRADE_COMPARE(c, (Vector2{0.0f, -1.2f}));
}

void BezierTest::compare() {
    CORRADE_VERIFY((QuadraticBezier2D{Vector2{0.5f, 1.0f + TypeTraits<Float>::epsilon()/2}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}} == QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));
    CORRADE_VERIFY((QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f + TypeTraits<Float>::epsilon()*2}} != QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f}}));
}

void BezierTest::lerpQuadratic() {
    Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f);
    QuadraticBezier2D bezier(p0, p1, p2);
    for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
        Vector2 expected = Math::pow<2>(1 - t)*p0 + 2*(1 - t)*t*p1 + Math::pow<2>(t)*p2;
        CORRADE_COMPARE(bezier.lerp(t), expected);
    }
}

void BezierTest::lerpCubic() {
    Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f), p3(5.0f, -20.0f);
    CubicBezier2D bezier(p0, p1, p2, p3);
    for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
        Vector2 expected = Math::pow<3>(1 - t)*p0 +
                           3*Math::pow<2>(1 - t)*t*p1 +
                           3*(1 - t)*Math::pow<2>(t)*p2 +
                           Math::pow<3>(t)*p3;
        CORRADE_COMPARE(bezier.lerp(t), expected);
    }
}

void BezierTest::debug() {
    std::ostringstream out;
    Debug(&out) << CubicBezier2D{Vector2{0.0f, 1.0f}, Vector2{1.5f, -0.3f}, Vector2{2.1f, 0.5f}, Vector2{0.0f, 2.0f}};
    CORRADE_COMPARE(out.str(), "Bezier({0, 1}, {1.5, -0.3}, {2.1, 0.5}, {0, 2})\n");
}

void BezierTest::configuration() {
    Corrade::Utility::Configuration c;

    CubicBezier2D bezier{Vector2{0.0f, 1.0f}, Vector2{1.5f, -0.3f}, Vector2{2.1f, 0.5f}, Vector2{0.0f, 2.0f}};
    std::string value("0 1 1.5 -0.3 2.1 0.5 0 2");

    c.setValue("bezier", bezier);
    CORRADE_COMPARE(c.value("bezier"), value);
    CORRADE_COMPARE(c.value<CubicBezier2D>("bezier"), bezier);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BezierTest)
