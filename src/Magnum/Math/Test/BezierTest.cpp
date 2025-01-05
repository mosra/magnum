/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <new>
#include <Corrade/Containers/ArrayView.h> /* arraySize() */
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/StrictWeakOrdering.h"

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

namespace Test { namespace {

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Vector2;
typedef Math::Bezier<1, 2, Float> LinearBezier2D;
using Magnum::QuadraticBezier2D;
using Magnum::CubicBezier2D;
using Magnum::CubicHermite2D;

struct BezierTest: TestSuite::Tester {
    explicit BezierTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void constructFromCubicHermite();
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

    void strictWeakOrdering();

    void debug();
};

BezierTest::BezierTest() {
    addTests({&BezierTest::construct,
              &BezierTest::constructDefault,
              &BezierTest::constructNoInit,
              &BezierTest::constructConversion,
              &BezierTest::constructFromCubicHermite,
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

              &BezierTest::strictWeakOrdering,

              &BezierTest::debug});
}

void BezierTest::construct() {
    /* The constructor should be implicit */
    constexpr QuadraticBezier2D a = {Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    CORRADE_VERIFY(std::is_nothrow_constructible<QuadraticBezier2D, Vector2, Vector2, Vector2>::value);
}

void BezierTest::constructDefault() {
    constexpr QuadraticBezier2D a;
    constexpr QuadraticBezier2D b{ZeroInit};
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{}, Vector2{}, Vector2{}}));
    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{}, Vector2{}, Vector2{}}));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<QuadraticBezier2D>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<QuadraticBezier2D, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, QuadraticBezier2D>::value);
}

void BezierTest::constructNoInit() {
    QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    new(&a) QuadraticBezier2D{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<QuadraticBezier2D, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, QuadraticBezier2D>::value);
}

void BezierTest::constructConversion() {
    constexpr QuadraticBezier2Dd a{Vector2d{0.5, 1.0}, Vector2d{1.1, 0.3}, Vector2d{0.1, 1.2}};
    constexpr QuadraticBezier2D b{a};

    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<QuadraticBezier2Dd, QuadraticBezier2D>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<QuadraticBezier2D, QuadraticBezier2Dd>::value);
}

void BezierTest::constructFromCubicHermite() {
    /* See CubicHermiterTest::constructFromBezier() for the inverse. Expected
       value the same as in valueCubic() to test also interpolation with it. */
    CubicHermite2D a{{}, Vector2{0.0f, 0.0f}, Vector2{30.0f, 45.0f}};
    CubicHermite2D b{Vector2{-45, -72}, Vector2{5.0f, -20.0f}, {}};
    auto bezier = CubicBezier2D::fromCubicHermite(a, b);

    CORRADE_COMPARE(bezier, (CubicBezier2D{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}}));
}

void BezierTest::constructCopy() {
    constexpr QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    constexpr QuadraticBezier2D b{a};
    CORRADE_COMPARE(b, (QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}}));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<QuadraticBezier2D>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<QuadraticBezier2D>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<QuadraticBezier2D>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<QuadraticBezier2D>::value);
}

void BezierTest::convert() {
    constexpr QBezier2D a{0.5f, 1.1f, 0.1f, 1.0f, 0.3f, 1.2f};
    constexpr QuadraticBezier2D b{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};

    constexpr QuadraticBezier2D c{a};
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = QBezier2D(b);
    #else
    constexpr QBezier2D d(b);
    #endif
    CORRADE_COMPARE(d.x0, a.x0);
    CORRADE_COMPARE(d.x1, a.x1);
    CORRADE_COMPARE(d.y0, a.y0);
    CORRADE_COMPARE(d.y1, a.y1);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<QBezier2D, QuadraticBezier2D>::value);
    CORRADE_VERIFY(!std::is_convertible<QuadraticBezier2D, QBezier2D>::value);
}

void BezierTest::data() {
    QuadraticBezier2D a{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}};
    a[0] = {};
    a[2] = {0.7f, 20.3f};

    CORRADE_COMPARE(a[0], (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(a[2], (Vector2{0.7f, 20.3f}));
    CORRADE_COMPARE(a, (QuadraticBezier2D{Vector2{0.0f, 0.0f}, Vector2{1.1f, 0.3f}, Vector2{0.7f, 20.3f}}));

    constexpr QuadraticBezier2D ca{Vector2{3.5f, 0.1f}, Vector2{1.3f, 10.3f}, Vector2{0.0f, -1.2f}};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why? */
    constexpr
    #endif
    Vector2 c = ca[2];
    CORRADE_COMPARE(c, (Vector2{0.0f, -1.2f}));

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why? */
    constexpr
    #endif
    Vector2 d = *ca.data();
    CORRADE_COMPARE(a.data()[2], (Vector2{0.7f, 20.3f}));
    CORRADE_COMPARE(d, (Vector2{3.5f, 0.1f}));

    /* It actually returns an array */
    CORRADE_COMPARE(Containers::arraySize(a.data()), 3);
    CORRADE_COMPARE(Containers::arraySize(ca.data()), 3);
}

void BezierTest::compare() {
    CORRADE_VERIFY(QuadraticBezier2D{Vector2{0.5f, 1.0f + TypeTraits<Float>::epsilon()/2}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}} == QuadraticBezier2D{Vector2{0.5f, 1.0f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.2f}});
    CORRADE_VERIFY(QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f + TypeTraits<Float>::epsilon()*2}} != QuadraticBezier2D{Vector2{0.5f, 1.1f}, Vector2{1.1f, 0.3f}, Vector2{0.1f, 1.0f}});
}

void BezierTest::valueLinear() {
    LinearBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{20.0f, 4.0f}};

    CORRADE_COMPARE(bezier.value(0.0f), (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{4.0f, 0.8f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{10.0f, 2.0f}));
    CORRADE_COMPARE(bezier.value(1.0f), (Vector2{20.0f, 4.0f}));
    CORRADE_COMPARE(bezier.value(0.2f), Math::lerp(bezier[0], bezier[1], 0.2f));
}

void BezierTest::valueQuadratic() {
    QuadraticBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}};

    CORRADE_COMPARE(bezier.value(0.0f), (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{4.0f, 4.96f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{10.0f, 8.5f}));
    CORRADE_COMPARE(bezier.value(1.0f), (Vector2{20.0f, 4.0f}));
    CORRADE_VERIFY(bezier.value(0.2f) != Math::lerp(bezier[0], bezier[2], 0.2f));
}

void BezierTest::valueCubic() {
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};

    /* Values should be exactly the same as in CubicHermiterTest::splerpVectorFromBezier() */
    CORRADE_COMPARE(bezier.value(0.0f), (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{5.8f, 5.984f}));
    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{11.875f, 4.625f}));
    CORRADE_COMPARE(bezier.value(1.0f), (Vector2{5.0f, -20.0f}));
    CORRADE_VERIFY(bezier.value(0.2f) != Math::lerp(bezier[0], bezier[3], 0.2f));
}

void BezierTest::subdivideLinear() {
    LinearBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{20.0f, 4.0f}};

    Containers::Pair<LinearBezier2D, LinearBezier2D> subdivided = bezier.subdivide(0.25f);

    CORRADE_COMPARE(subdivided.first()[0], bezier[0]);
    CORRADE_COMPARE(subdivided.first()[1], subdivided.second()[0]);
    CORRADE_COMPARE(subdivided.second()[1], bezier[1]);
    CORRADE_COMPARE(subdivided.first().value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(subdivided.second().value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(subdivided.first(), (LinearBezier2D{Vector2{0.0f, 0.0f}, Vector2{5.0f, 1.0f}}));
    CORRADE_COMPARE(subdivided.second(), (LinearBezier2D{Vector2{5.0f, 1.0f}, Vector2{20.0f, 4.0f}}));
}

void BezierTest::subdivideQuadratic() {
    QuadraticBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}};

    Containers::Pair<QuadraticBezier2D, QuadraticBezier2D> subdivided = bezier.subdivide(0.25f);

    CORRADE_COMPARE(subdivided.first()[0], bezier[0]);
    CORRADE_COMPARE(subdivided.first()[2], subdivided.second()[0]);
    CORRADE_COMPARE(subdivided.second()[2], bezier[2]);
    CORRADE_COMPARE(subdivided.first().value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(subdivided.second().value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(subdivided.first(), (QuadraticBezier2D{Vector2{0.0f, 0.0f}, Vector2{2.5f, 3.75f}, Vector2{5.0f, 5.875f}}));
    CORRADE_COMPARE(subdivided.second(), (QuadraticBezier2D{Vector2{5.0f, 5.875f}, Vector2{12.5f, 12.25f}, Vector2{20.0f, 4.0f}}));
}

void BezierTest::subdivideCubic() {
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};

    Containers::Pair<CubicBezier2D, CubicBezier2D> subdivided = bezier.subdivide(0.25f);

    CORRADE_COMPARE(subdivided.first()[0], bezier[0]);
    CORRADE_COMPARE(subdivided.first()[3], subdivided.second()[0]);
    CORRADE_COMPARE(subdivided.second()[3], bezier[3]);
    CORRADE_COMPARE(subdivided.first().value(0.8f), bezier.value(0.2f));
    CORRADE_COMPARE(subdivided.second().value(0.33333f), bezier.value(0.5f));
    CORRADE_COMPARE(subdivided.first(), (CubicBezier2D{Vector2{0.0f, 0.0f}, Vector2{2.5f, 3.75f}, Vector2{5.0f, 5.875f}, Vector2{7.10938f, 6.57812f}}));
    CORRADE_COMPARE(subdivided.second(), (CubicBezier2D{Vector2{7.10938f, 6.57812f}, Vector2{13.4375f, 8.6875f}, Vector2{16.25f, -2.0f}, Vector2{5.0f, -20.0f}}));
}

void BezierTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    CubicBezier2D a{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};
    CubicBezier2D b{Vector2{1.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};
    CubicBezier2D c{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, 20.0f}};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));

    CORRADE_VERIFY(!o(a, a));
}

void BezierTest::debug() {
    Containers::String out;
    Debug(&out) << CubicBezier2D{Vector2{0.0f, 1.0f}, Vector2{1.5f, -0.3f}, Vector2{2.1f, 0.5f}, Vector2{0.0f, 2.0f}};
    CORRADE_COMPARE(out, "Bezier({0, 1}, {1.5, -0.3}, {2.1, 0.5}, {0, 2})\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BezierTest)
