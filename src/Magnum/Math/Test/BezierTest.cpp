/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

struct QBezier2D {
    float x0, x1, x2, y0, y1, y2;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct BezierConverter<2, 2, Float, QBezier2D> {
    constexpr static QuadraticBezier2D<Float> from(const QBezier2D& other) {
        return {Vector2<Float>{other.x0, other.y0}, Vector2<Float>{other.x1, other.y1}, Vector2<Float>{other.x2, other.y2}};
    }

    constexpr static QBezier2D to(const QuadraticBezier2D<Float>& other) {
        return {other[0][0], other[1][0], other[2][0], other[0][1], other[1][1], other[2][1]};
    }
};

}

namespace Test {

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<Double> Vector2d;
typedef Math::Bezier<1, 2, Float> LinearBezier2D;
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
    void convert();

    void data();

    void compare();

    void valueLinear();
    void valueQuadratic();
    void valueCubic();
    void subdivideLinear();
    void subdivideQuadratic();
    void subdivideCubic();

    void debug();
    void configuration();
};

BezierTest::BezierTest() {
    addTests({&BezierTest::construct,
              &BezierTest::constructDefault,
              &BezierTest::constructNoInit,
              &BezierTest::constructConversion,
              &BezierTest::constructCopy,
              &BezierTest::convert,

              &BezierTest::data,

              &BezierTest::compare,

              &BezierTest::valueLinear,
              &BezierTest::valueQuadratic,
              &BezierTest::valueCubic,
              &BezierTest::subdivideLinear,
              &BezierTest::subdivideQuadratic,
              &BezierTest::subdivideCubic,

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
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<QuadraticBezier2D, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, QuadraticBezier2D>::value));
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

void BezierTest::convert() {
    constexpr QBezier2D a{0.5f, 1.1f, 0.1f, 1.0f, 0.3f, 1.2f};
    constexpr QuadraticBezier2D b{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};

    constexpr QuadraticBezier2D c{a};
    CORRADE_COMPARE(c, b);

    constexpr QBezier2D d(b);
    CORRADE_COMPARE(d.x0, a.x0);
    CORRADE_COMPARE(d.x1, a.x1);
    CORRADE_COMPARE(d.y0, a.y0);
    CORRADE_COMPARE(d.y1, a.y1);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<QBezier2D, QuadraticBezier2D>::value));
    CORRADE_VERIFY(!(std::is_convertible<QuadraticBezier2D, QBezier2D>::value));
}

void BezierTest::data() {
    QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    a[0] = {};
    a[2] = {0.7f, 20.3f};

    CORRADE_COMPARE(a[0], (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(a[2], (Vector2{0.7f, 20.3f}));
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.0f, 0.0f}, Vector2{1.1f, 0.3f}, Vector2{0.7f, 20.3f}}));

    constexpr QuadraticBezier2D b{Vector2{3.5f, 0.1f}, Vector2{1.3f, 10.3f}, Vector2{0.0f, -1.2f}};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why? */
    constexpr
    #endif
    Vector2 c = b[2];
    CORRADE_COMPARE(c, (Vector2{0.0f, -1.2f}));
}

void BezierTest::compare() {
    CORRADE_VERIFY((QuadraticBezier2D{Vector2{0.5f, 1.0f + TypeTraits<Float>::epsilon()/2}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}} == QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));
    CORRADE_VERIFY((QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f + TypeTraits<Float>::epsilon()*2}} != QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f}}));
}

void BezierTest::valueLinear() {
    LinearBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{20.0f, 4.0f}};

    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{4.0f, 0.8f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{10.0f, 2.0f}));
    CORRADE_COMPARE(bezier.value(0.2f), Math::lerp(bezier[0], bezier[1], 0.2f));
}

void BezierTest::valueQuadratic() {
    QuadraticBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}};

    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{4.0f, 4.96f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{10.0f, 8.5f}));
    CORRADE_VERIFY(bezier.value(0.2f) != Math::lerp(bezier[0], bezier[2], 0.2f));
}

void BezierTest::valueCubic() {
    Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f), p3(5.0f, -20.0f);
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};

    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{5.8f, 5.984f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{11.875f, 4.625f}));
    CORRADE_VERIFY(bezier.value(0.2f) != Math::lerp(bezier[0], bezier[3], 0.2f));
}

void BezierTest::subdivideLinear() {
    LinearBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{20.0f, 4.0f}};

    LinearBezier2D left, right;
    std::tie(left, right) = bezier.subdivide(0.25f);

    CORRADE_COMPARE(left[0], bezier[0]);
    CORRADE_COMPARE(left[1], right[0]);
    CORRADE_COMPARE(right[1], bezier[1]);
    CORRADE_COMPARE(left.value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(right.value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(left, (LinearBezier2D{Vector2{0.0f, 0.0f}, Vector2{5.0f, 1.0f}}));
    CORRADE_COMPARE(right, (LinearBezier2D{Vector2{5.0f, 1.0f}, Vector2{20.0f, 4.0f}}));
}

void BezierTest::subdivideQuadratic() {
    QuadraticBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}};

    QuadraticBezier2D left, right;
    std::tie(left, right) = bezier.subdivide(0.25f);

    CORRADE_COMPARE(left[0], bezier[0]);
    CORRADE_COMPARE(left[2], right[0]);
    CORRADE_COMPARE(right[2], bezier[2]);
    CORRADE_COMPARE(left.value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(right.value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(left, (QuadraticBezier2D{Vector2{0.0f, 0.0f}, Vector2{2.5f, 3.75f}, Vector2{5.0f, 5.875f}}));
    CORRADE_COMPARE(right, (QuadraticBezier2D{Vector2{5.0f, 5.875f}, Vector2{12.5f, 12.25f}, Vector2{20.0f, 4.0f}}));
}

void BezierTest::subdivideCubic() {
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};

    CubicBezier2D left, right;
    std::tie(left, right) = bezier.subdivide(0.25f);

    CORRADE_COMPARE(left[0], bezier[0]);
    CORRADE_COMPARE(left[3], right[0]);
    CORRADE_COMPARE(right[3], bezier[3]);
    CORRADE_COMPARE(left.value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(right.value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(left, (CubicBezier2D{Vector2{0.0f, 0.0f}, Vector2{2.5f, 3.75f}, Vector2{5.0f, 5.875f}, Vector2{7.10938f, 6.57812f}}));
    CORRADE_COMPARE(right, (CubicBezier2D{Vector2{7.10938f, 6.57812f}, Vector2{13.4375f, 8.6875f}, Vector2{16.25f, -2.0f}, Vector2{5.0f, -20.0f}}));
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
