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

#include <tuple>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test {

struct FunctionsTest: Corrade::TestSuite::Tester {
    explicit FunctionsTest();

    void powIntegral();
    void pow();

    void min();
    void minList();
    void max();
    void maxList();
    void minmax();
    void minmaxList();
    void clamp();
    void nanPropagation();

    void sign();
    void abs();

    void floor();
    void round();
    void ceil();

    void sqrt();
    void sqrtInverted();
    void lerp();
    void lerpBool();
    void lerpInverted();
    void fma();

    void logIntegral();
    void log2();
    void log();
    void exp();
    void div();
    void trigonometric();
    void trigonometricWithBase();
};

typedef Math::Constants<Float> Constants;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<UnsignedByte> Vector3ub;
typedef Math::Vector3<Byte> Vector3b;
typedef Math::Vector3<Int> Vector3i;

FunctionsTest::FunctionsTest() {
    addTests({&FunctionsTest::powIntegral,
              &FunctionsTest::pow,

              &FunctionsTest::min,
              &FunctionsTest::minList,
              &FunctionsTest::max,
              &FunctionsTest::maxList,
              &FunctionsTest::minmax,
              &FunctionsTest::minmaxList,
              &FunctionsTest::clamp,
              &FunctionsTest::nanPropagation,

              &FunctionsTest::sign,
              &FunctionsTest::abs,

              &FunctionsTest::floor,
              &FunctionsTest::round,
              &FunctionsTest::ceil,

              &FunctionsTest::sqrt,
              &FunctionsTest::sqrtInverted,
              &FunctionsTest::lerp,
              &FunctionsTest::lerpBool,
              &FunctionsTest::lerpInverted,
              &FunctionsTest::fma,

              &FunctionsTest::logIntegral,
              &FunctionsTest::log2,
              &FunctionsTest::log,
              &FunctionsTest::exp,
              &FunctionsTest::div,
              &FunctionsTest::trigonometric,
              &FunctionsTest::trigonometricWithBase});
}

void FunctionsTest::powIntegral() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);

    /* Constant expression */
    constexpr Int a = Math::pow<3>(5);
    CORRADE_COMPARE(a, 125);

    CORRADE_COMPARE(Math::pow<2>(Vector3{2.0f, -3.0f, 1.5f}), (Vector3{4.0f, 9.0f, 2.25f}));
}

void FunctionsTest::pow() {
    CORRADE_COMPARE(Math::pow(2.0f, 0.5f), 1.414213562f);
    CORRADE_COMPARE(Math::pow(Vector3{2.0f, 9.0f, 25.0f}, 0.5f), (Vector3{1.414213562f, 3.0f, 5.0f}));
}

void FunctionsTest::min() {
    CORRADE_COMPARE(Math::min(5, 9), 5);
    CORRADE_COMPARE(Math::min(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(5, -5, 2));
    CORRADE_COMPARE(Math::min(Vector3i{5, -3, 2}, 1), (Vector3i{1, -3, 1}));
}

void FunctionsTest::minList() {
    CORRADE_COMPARE(Math::min({5, -2, 9}), -2);
    CORRADE_COMPARE(Math::min({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(-2, -5, 2));

    CORRADE_COMPARE(Math::min(std::initializer_list<Vector3i>{}), Vector3i{});
}

void FunctionsTest::max() {
    CORRADE_COMPARE(Math::max(5, 9), 9);
    CORRADE_COMPARE(Math::max(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(9, -3, 18));
    CORRADE_COMPARE(Math::max(Vector3i{5, -3, 2}, 3), (Vector3i{5, 3, 3}));
}

void FunctionsTest::maxList() {
    CORRADE_COMPARE(Math::max({5, -2, 9}), 9);
    CORRADE_COMPARE(Math::max({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(9, 14, 18));

    CORRADE_COMPARE(Math::max(std::initializer_list<Vector3i>{}), Vector3i{});
}

void FunctionsTest::minmax() {
    const auto expectedScalar = std::make_pair(-5.0f, 4.0f);
    CORRADE_COMPARE(Math::minmax(-5.0f, 4.0f), expectedScalar);
    CORRADE_COMPARE(Math::minmax(4.0f, -5.0f), expectedScalar);

    const Vector3 a(5.0f, -4.0f, 1.0f);
    const Vector3 b(7.0f, -3.0f, 1.0f);
    const std::pair<Vector3, Vector3> expectedVector{{5.0f, -4.0f, 1.0f}, {7.0f, -3.0f, 1.0f}};
    CORRADE_COMPARE_AS(Math::minmax(a, b), expectedVector, std::pair<Vector3, Vector3>);
    CORRADE_COMPARE_AS(Math::minmax(b, a), expectedVector, std::pair<Vector3, Vector3>);
}

void FunctionsTest::minmaxList() {
    const auto expected = std::make_pair(-3.0f, 2.0f);
    CORRADE_COMPARE(Math::minmax({-1.0f, 2.0f, -3.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-1.0f, -3.0f, 2.0f}), expected);
    CORRADE_COMPARE(Math::minmax({2.0f, -1.0f, -3.0f}), expected);
    CORRADE_COMPARE(Math::minmax({2.0f, -3.0f, -1.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-3.0f, 2.0f, -1.0f}), expected);
    CORRADE_COMPARE(Math::minmax({-3.0f, -1.0f, 2.0f}), expected);

    const std::pair<Vector2, Vector2> expectedVec{Vector2{-3.0f, -2.0f}, Vector2{2.0f, 3.0f}};
    CORRADE_COMPARE(Math::minmax({Vector2{-1.0f, 3.0f}, Vector2{2.0f, 1.0f}, Vector2{-3.0f, -2.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-1.0f, 1.0f}, Vector2{-3.0f, 3.0f}, Vector2{2.0f, -2.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{2.0f, -2.0f}, Vector2{-1.0f, 1.0f}, Vector2{-3.0f, 3.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{2.0f, 1.0f}, Vector2{-3.0f, -2.0f}, Vector2{-1.0f, 3.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-3.0f, 3.0f}, Vector2{2.0f, -2.0f}, Vector2{-1.0f, 1.0f}}), expectedVec);
    CORRADE_COMPARE(Math::minmax({Vector2{-3.0f, -2.0f}, Vector2{-1.0f, 3.0f}, Vector2{2.0f, 1.0f}}), expectedVec);
}

void FunctionsTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);

    CORRADE_COMPARE(Math::clamp(
        Vector3{0.5f, -1.6f, 9.5f},
        Vector3{-1.0f, 2.0f, 0.3f},
        Vector3{0.7f, 3.0f, 5.0f}),
        Vector3(0.5f, 2.0f, 5.0f));

    CORRADE_COMPARE(Math::clamp(Vector3(0.5f, -1.6f, 9.5f), -1.0f, 5.0f), Vector3(0.5f, -1.0f, 5.0f));
}

void FunctionsTest::nanPropagation() {
    CORRADE_COMPARE(Math::min(Constants::nan(), 5.0f), Constants::nan());
    CORRADE_COMPARE(Math::min(Vector2{Constants::nan(), 6.0f}, Vector2{5.0f})[0], Constants::nan());
    CORRADE_COMPARE(Math::min(Vector2{Constants::nan(), 6.0f}, Vector2{5.0f})[1], 5.0f);

    CORRADE_COMPARE(Math::max(Constants::nan(), 5.0f), Constants::nan());
    CORRADE_COMPARE(Math::max(Vector2{Constants::nan(), 4.0f}, Vector2{5.0f})[0], Constants::nan());
    CORRADE_COMPARE(Math::max(Vector2{Constants::nan(), 4.0f}, Vector2{5.0f})[1], 5.0f);

    CORRADE_COMPARE(Math::clamp(Constants::nan(), 2.0f, 6.0f), Constants::nan());
    CORRADE_COMPARE(Math::clamp(Vector2{Constants::nan(), 1.0f}, 2.0f, 6.0f)[0], Constants::nan());
    CORRADE_COMPARE(Math::clamp(Vector2{Constants::nan(), 1.0f}, 2.0f, 6.0f)[1], 2.0f);
}

void FunctionsTest::sign() {
    CORRADE_COMPARE(Math::sign(3516), 1);
    CORRADE_COMPARE(Math::sign(0.0f), 0.0f);
    CORRADE_COMPARE(Math::sign(-3.7), -1.0);
    CORRADE_COMPARE(Math::sign(Vector3i(0, -3, 2)), Vector3i(0, -1, 1));
}

void FunctionsTest::abs() {
    CORRADE_COMPARE(Math::abs(-5), 5);
    CORRADE_COMPARE(Math::abs(5), 5);
    CORRADE_COMPARE(Math::abs(Vector3i(5, -3, 2)), Vector3i(5, 3, 2));
}

void FunctionsTest::floor() {
    CORRADE_COMPARE(Math::floor(0.7f), 0.0f);
    CORRADE_COMPARE(Math::floor(Vector3(2.3f, 0.7f, 1.5f)), Vector3(2.0f, 0.0f, 1.0f));
}

void FunctionsTest::round() {
    CORRADE_COMPARE(Math::round(2.3f), 2.0f);
    CORRADE_COMPARE(Math::round(Vector3(2.3f, 0.7f, 1.5f)), Vector3(2.0f, 1.0f, 2.0f));

    /* We are working around missing std::round() in Android, thus we must
       test that the behavior is the same on both implementations */
    CORRADE_COMPARE(Math::round(-2.0f), -2.0f);
    CORRADE_COMPARE(Math::round(-1.5f), -2.0f);
    CORRADE_COMPARE(Math::round(-1.3f), -1.0f);
    CORRADE_COMPARE(Math::round(1.3f), 1.0f);
    CORRADE_COMPARE(Math::round(1.5f), 2.0f);
    CORRADE_COMPARE(Math::round(2.0f), 2.0f);
}

void FunctionsTest::ceil() {
    CORRADE_COMPARE(Math::ceil(2.3f), 3.0f);
    CORRADE_COMPARE(Math::ceil(Vector3(2.3f, 0.7f, 1.5f)), Vector3(3.0f, 1.0f, 2.0f));
}

void FunctionsTest::sqrt() {
    CORRADE_COMPARE(Math::sqrt(16), 4);
    CORRADE_COMPARE(Math::sqrt(Vector3i(256, 1, 0)), Vector3i(16, 1, 0));
}

void FunctionsTest::sqrtInverted() {
    CORRADE_COMPARE(Math::sqrtInverted(16.0f), 0.25f);
    CORRADE_COMPARE(Math::sqrtInverted(Vector3(1.0f, 4.0f, 16.0f)), Vector3(1.0f, 0.5f, 0.25f));
}

void FunctionsTest::lerp() {
    /* Floating-point / integral scalar */
    CORRADE_COMPARE(Math::lerp(2.0f, 5.0f, 0.5f), 3.5f);
    CORRADE_COMPARE(Math::lerp(2, 5, 0.5f), 3);

    /* Floating-point vector */
    Vector3 a(-1.0f, 2.0f, 3.0f);
    Vector3 b(3.0f, -2.0f, 11.0f);
    CORRADE_COMPARE(Math::lerp(a, b, 0.25f), Vector3(0.0f, 1.0f, 5.0f));

    /* Integer vector */
    Vector3i c(0, 128, 64);
    Vector3i d(16, 0, 32);
    CORRADE_COMPARE(Math::lerp(c, d, 0.25f), Vector3i(4, 96, 56));

    /* Vector as interpolation phase */
    CORRADE_COMPARE(Math::lerp(a, b, Vector3(0.25f, 0.5f, 0.75f)), Vector3(0.0f, 0.0f, 9.0f));
}

void FunctionsTest::lerpBool() {
    CORRADE_COMPARE(Math::lerp(Vector3i{1, 2, 3}, Vector3i{5, 6, 7}, BoolVector<3>(5)), (Vector3i{5, 2, 7}));
    CORRADE_COMPARE(Math::lerp(BoolVector<3>{false}, BoolVector<3>{true}, BoolVector<3>(5)), BoolVector<3>{5});
}

void FunctionsTest::lerpInverted() {
    /* Floating-point scalar */
    CORRADE_COMPARE(Math::lerpInverted(2.0f, 5.0f, 3.5f), 0.5f);

    /* Floating-point vector */
    Vector3 a(-1.0f, 2.0f, 3.0f);
    Vector3 b(3.0f, -2.0f, 11.0f);
    CORRADE_COMPARE(Math::lerpInverted(a, b, Vector3(0.0f, 0.0f, 9.0f)), Vector3(0.25f, 0.5f, 0.75f));
}

void FunctionsTest::fma() {
    CORRADE_COMPARE(Math::fma(2.0f, 3.0f, 0.75f), 6.75f);
    CORRADE_COMPARE(Math::fma(Vector3( 2.0f,  1.5f,  0.5f),
                              Vector3( 3.0f,  2.0f, -1.0f),
                              Vector3(0.75f, 0.25f,  0.1f)),
                              Vector3(6.75f, 3.25f, -0.4f));
}

void FunctionsTest::logIntegral() {
    CORRADE_COMPARE(Math::log(2, 256), 8ul);
    CORRADE_COMPARE(Math::log(256, 2), 0ul);
}

void FunctionsTest::log2() {
    CORRADE_COMPARE(Math::log2(2153), 11);
}

void FunctionsTest::log() {
    CORRADE_COMPARE(Math::log(2.0f), 0.693147f);
}

void FunctionsTest::exp() {
    CORRADE_COMPARE(Math::exp(0.693147f), 2.0f);
}

void FunctionsTest::div() {
    Int quotient, remainder;
    std::tie(quotient, remainder) = Math::div(57, 6);
    CORRADE_COMPARE(quotient, 9);
    CORRADE_COMPARE(remainder, 3);
}

void FunctionsTest::trigonometric() {
    CORRADE_COMPARE(Math::sin(Deg(30.0f)), 0.5f);
    CORRADE_COMPARE(Math::sin(Rad(Constants::pi()/6)), 0.5f);
    CORRADE_COMPARE_AS(Math::asin(0.5f), Deg(30.0f), Deg);

    CORRADE_COMPARE(Math::cos(Deg(60.0f)), 0.5f);
    CORRADE_COMPARE(Math::cos(Rad(Constants::pi()/3)), 0.5f);
    CORRADE_COMPARE_AS(Math::acos(0.5f), Deg(60.0f), Deg);

    CORRADE_COMPARE(Math::sincos(Deg(30.0f)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(Deg(30.0f)).second, 0.8660254037844386f);
    CORRADE_COMPARE(Math::sincos(Rad(Constants::pi()/6)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(Rad(Constants::pi()/6)).second, 0.8660254037844386f);

    CORRADE_COMPARE(Math::tan(Deg(45.0f)), 1.0f);
    CORRADE_COMPARE(Math::tan(Rad(Constants::pi()/4)), 1.0f);
    CORRADE_COMPARE_AS(Math::atan(1.0f), Deg(45.0f), Deg);
}

void FunctionsTest::trigonometricWithBase() {
    /* Verify that the functions can be called with Unit<Deg, T> and Unit<Rad, T> */
    CORRADE_VERIFY((std::is_same<decltype(2*Deg(15.0f)), Unit<Math::Deg, Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(2*Rad(Constants::pi()/12)), Unit<Math::Rad, Float>>::value));

    CORRADE_COMPARE(Math::sin(2*Deg(15.0f)), 0.5f);
    CORRADE_COMPARE(Math::sin(2*Rad(Constants::pi()/12)), 0.5f);

    CORRADE_COMPARE(Math::cos(2*Deg(30.0f)), 0.5f);
    CORRADE_COMPARE(Math::cos(2*Rad(Constants::pi()/6)), 0.5f);

    CORRADE_COMPARE(Math::sincos(2*Deg(15.0f)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(2*Deg(15.0f)).second, 0.8660254037844386f);
    CORRADE_COMPARE(Math::sincos(2*Rad(Constants::pi()/12)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(2*Rad(Constants::pi()/12)).second, 0.8660254037844386f);

    CORRADE_COMPARE(Math::tan(2*Deg(22.5f)), 1.0f);
    CORRADE_COMPARE(Math::tan(2*Rad(Constants::pi()/8)), 1.0f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsTest)
