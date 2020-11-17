/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 janos <janos.meny@googlemail.com>

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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct FunctionsTest: Corrade::TestSuite::Tester {
    explicit FunctionsTest();

    template<class T> void popcount();

    void powIntegral();
    void pow();

    void min();
    void max();
    void minmax();
    void clamp();
    void nanPropagation();

    void sign();
    void abs();

    void floor();
    void round();
    void ceil();
    void fmod();

    void binomialCoefficient();
    void binomialCoefficientInvalidInput();
    void binomialCoefficientOverflow();

    void sqrt();
    void sqrtInverted();
    void lerp();
    void lerpBool();
    void lerpInverted();
    void select();
    void selectBool();
    void fma();

    void logIntegral();
    void log2();
    void log();
    void exp();
    void div();
    void isInf();
    void isInfVector();
    void isNan();
    void isNanfVector();

    void reflect();
    void reflectNotNormalized();
    void refract();
    void refractNotNormalized();

    void trigonometric();
    void trigonometricWithBase();
    template<class T> void sincos();
};

using namespace Literals;

typedef Math::Constants<Float> Constants;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;
typedef Math::Vector3<UnsignedByte> Vector3ub;
typedef Math::Vector3<Byte> Vector3b;
typedef Math::Vector3<Int> Vector3i;

FunctionsTest::FunctionsTest() {
    addRepeatedTests<FunctionsTest>({
        &FunctionsTest::popcount<UnsignedInt>,
        &FunctionsTest::popcount<UnsignedLong>}, 8);

    addTests({&FunctionsTest::powIntegral,
              &FunctionsTest::pow,

              &FunctionsTest::min,
              &FunctionsTest::max,
              &FunctionsTest::minmax,
              &FunctionsTest::clamp,
              &FunctionsTest::nanPropagation,

              &FunctionsTest::sign,
              &FunctionsTest::abs,

              &FunctionsTest::floor,
              &FunctionsTest::round,
              &FunctionsTest::ceil,
              &FunctionsTest::fmod,

              &FunctionsTest::binomialCoefficient,
              &FunctionsTest::binomialCoefficientInvalidInput,
              &FunctionsTest::binomialCoefficientOverflow,

              &FunctionsTest::sqrt,
              &FunctionsTest::sqrtInverted,
              &FunctionsTest::lerp,
              &FunctionsTest::lerpBool,
              &FunctionsTest::lerpInverted,
              &FunctionsTest::select,
              &FunctionsTest::selectBool,
              &FunctionsTest::fma,

              &FunctionsTest::logIntegral,
              &FunctionsTest::log2,
              &FunctionsTest::log,
              &FunctionsTest::exp,
              &FunctionsTest::div,
              &FunctionsTest::isInf,
              &FunctionsTest::isInfVector,
              &FunctionsTest::isNan,
              &FunctionsTest::isNanfVector,

              &FunctionsTest::reflect,
              &FunctionsTest::reflectNotNormalized,
              &FunctionsTest::refract,
              &FunctionsTest::refractNotNormalized,

              &FunctionsTest::trigonometric,
              &FunctionsTest::trigonometricWithBase,
              &FunctionsTest::sincos<Float>,
              &FunctionsTest::sincos<Double>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &FunctionsTest::sincos<long double>,
              #endif
              });
}

template<class T> void FunctionsTest::popcount() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    /* Trivial cases */
    CORRADE_COMPARE(Math::popcount(T(0)), 0);
    CORRADE_COMPARE(Math::popcount(~T{}), sizeof(T)*8);

    /* 0x101101011101000110010100 */
    CORRADE_COMPARE(Math::popcount(T(0xb5d194) << testCaseRepeatId()), 12);
}

void FunctionsTest::powIntegral() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);

    /* Constant expression */
    constexpr Int a = Math::pow<3>(5);
    CORRADE_COMPARE(a, 125);

    CORRADE_COMPARE(Math::pow<2>(Vector3{2.0f, -3.0f, 1.5f}), (Vector3{4.0f, 9.0f, 2.25f}));

    /* Not testing wrapped types -- what unit should have degrees squared? */
}

void FunctionsTest::pow() {
    CORRADE_COMPARE(Math::pow(2.0f, 0.5f), 1.414213562f);
    CORRADE_COMPARE(Math::pow(Vector3{2.0f, 9.0f, 25.0f}, 0.5f), (Vector3{1.414213562f, 3.0f, 5.0f}));

    /* Not testing wrapped types -- what unit should have degrees squared? */
}

void FunctionsTest::min() {
    CORRADE_COMPARE(Math::min(5, 9), 5);
    CORRADE_COMPARE(Math::min(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(5, -5, 2));
    CORRADE_COMPARE(Math::min(Vector3i{5, -3, 2}, 1), (Vector3i{1, -3, 1}));

    /* Wrapped types */
    CORRADE_COMPARE(Math::min(5.0_degf, 9.0_degf), 5.0_degf);
}

void FunctionsTest::max() {
    CORRADE_COMPARE(Math::max(5, 9), 9);
    CORRADE_COMPARE(Math::max(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(9, -3, 18));
    CORRADE_COMPARE(Math::max(Vector3i{5, -3, 2}, 3), (Vector3i{5, 3, 3}));

    /* Wrapped types */
    CORRADE_COMPARE(Math::max(5.0_degf, 9.0_degf), 9.0_degf);
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

    /* Wrapped types */
    CORRADE_COMPARE(Math::minmax(4.0_degf, 5.0_degf), std::make_pair(4.0_degf, 5.0_degf));
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

    /* Wrapped types */
    CORRADE_COMPARE(Math::clamp(0.5_degf, 0.75_degf, 1.0_degf), 0.75_degf);
}

void FunctionsTest::nanPropagation() {
    CORRADE_COMPARE(Math::min(Constants::nan(), 5.0f), Constants::nan());
    CORRADE_COMPARE(Math::min(5.0f, Constants::nan()), 5.0f);
    CORRADE_COMPARE(Math::min(Vector2{Constants::nan(), 6.0f}, Vector2{5.0f})[0], Constants::nan());
    CORRADE_COMPARE(Math::min(Vector2{Constants::nan(), 6.0f}, Vector2{5.0f})[1], 5.0f);

    CORRADE_COMPARE(Math::max(Constants::nan(), 5.0f), Constants::nan());
    CORRADE_COMPARE(Math::max(5.0f, Constants::nan()), 5.0f);
    CORRADE_COMPARE(Math::max(Vector2{Constants::nan(), 4.0f}, Vector2{5.0f})[0], Constants::nan());
    CORRADE_COMPARE(Math::max(Vector2{Constants::nan(), 4.0f}, Vector2{5.0f})[1], 5.0f);

    CORRADE_COMPARE(Math::clamp(Constants::nan(), 2.0f, 6.0f), Constants::nan());
    /* NaN clamp bounds are not propagating the NaN, but those cases are
       usually user error anyway */
    CORRADE_COMPARE(Math::clamp(Vector2{Constants::nan(), 1.0f}, 2.0f, 6.0f)[0], Constants::nan());
    CORRADE_COMPARE(Math::clamp(Vector2{Constants::nan(), 1.0f}, 2.0f, 6.0f)[1], 2.0f);
}

void FunctionsTest::sign() {
    CORRADE_COMPARE(Math::sign(3516), 1);
    CORRADE_COMPARE(Math::sign(0.0f), 0.0f);
    CORRADE_COMPARE(Math::sign(-3.7), -1.0);
    CORRADE_COMPARE(Math::sign(Vector3i(0, -3, 2)), Vector3i(0, -1, 1));

    /* Wrapped types */
    CORRADE_COMPARE(Math::sign(-3.7_degf), -1.0_degf);
}

void FunctionsTest::abs() {
    CORRADE_COMPARE(Math::abs(-5), 5);
    CORRADE_COMPARE(Math::abs(5), 5);
    CORRADE_COMPARE(Math::abs(Vector3i(5, -3, 2)), Vector3i(5, 3, 2));

    /* Wrapped types */
    CORRADE_COMPARE(Math::abs(-5.0_degf), 5.0_degf);
}

void FunctionsTest::floor() {
    CORRADE_COMPARE(Math::floor(0.7f), 0.0f);
    CORRADE_COMPARE(Math::floor(Vector3(2.3f, 0.7f, 1.5f)), Vector3(2.0f, 0.0f, 1.0f));

    /* Wrapped types */
    CORRADE_COMPARE(Math::floor(2.7_degf), 2.0_degf);
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

    /* Wrapped types */
    CORRADE_COMPARE(Math::round(2.7_degf), 3.0_degf);
}

void FunctionsTest::ceil() {
    CORRADE_COMPARE(Math::ceil(2.3f), 3.0f);
    CORRADE_COMPARE(Math::ceil(Vector3(2.3f, 0.7f, 1.5f)), Vector3(3.0f, 1.0f, 2.0f));

    /* Wrapped types */
    CORRADE_COMPARE(Math::ceil(2.7_degf), 3.0_degf);
}

void FunctionsTest::binomialCoefficient() {
    CORRADE_COMPARE(Math::binomialCoefficient(1, 1), 1ull);
    CORRADE_COMPARE(Math::binomialCoefficient(1, 0), 1ull);
    CORRADE_COMPARE(Math::binomialCoefficient(19, 11), 75582ull);
    CORRADE_COMPARE(Math::binomialCoefficient(1000, 999), 1000ull);
    CORRADE_COMPARE(Math::binomialCoefficient(0, 0), 1ull);
    CORRADE_COMPARE(Math::binomialCoefficient(32, 11), 129024480ull);
    CORRADE_COMPARE(Math::binomialCoefficient(62, 31), 465428353255261088ull);
}

void FunctionsTest::binomialCoefficientInvalidInput() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Math::binomialCoefficient(15, 16);
    CORRADE_COMPARE(out.str(), "Math::binomialCoefficient(): k can't be greater than n in (15 choose 16)\n");
}

void FunctionsTest::binomialCoefficientOverflow() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Math::binomialCoefficient(63, 31);
    CORRADE_COMPARE(out.str(), "Math::binomialCoefficient(): overflow for (63 choose 31)\n");
}

void FunctionsTest::fmod() {
    CORRADE_COMPARE(Math::fmod(5.1f, 3.0f), 2.1f);
    CORRADE_COMPARE(Math::fmod(Vector3(5.1f, -5.1f, 6.8f), Vector3(3.0f, 3.0f, 1.1f)), Vector3(2.1f, -2.1f, 0.2f));

    /* Wrapped types */
    CORRADE_COMPARE(Math::fmod(2.7_degf, 1.3_degf), 0.1_degf);
}

void FunctionsTest::sqrt() {
    CORRADE_COMPARE(Math::sqrt(16), 4);
    CORRADE_COMPARE(Math::sqrt(Vector3i(256, 1, 0)), Vector3i(16, 1, 0));

    /* Not testing wrapped types -- what unit should have degrees squared? */
}

void FunctionsTest::sqrtInverted() {
    CORRADE_COMPARE(Math::sqrtInverted(16.0f), 0.25f);
    CORRADE_COMPARE(Math::sqrtInverted(Vector3(1.0f, 4.0f, 16.0f)), Vector3(1.0f, 0.5f, 0.25f));

    /* Not testing wrapped types -- what unit should have degrees squared? */
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

    /* Wrapped types */
    CORRADE_COMPARE(Math::lerp(2.0_degf, 5.0_degf, 0.5f), 3.5_degf);
}

void FunctionsTest::lerpBool() {
    /* Scalar interpolation phase */
    CORRADE_COMPARE(Math::lerp(Vector3i{1, 2, 3}, Vector3i{5, 6, 7}, true), (Vector3i{5, 6, 7}));
    CORRADE_COMPARE(Math::lerp(BoolVector<3>{5}, BoolVector<3>{true}, false), BoolVector<3>{5});

    /* Vector interpolation phase */
    CORRADE_COMPARE(Math::lerp(Vector3i{1, 2, 3}, Vector3i{5, 6, 7}, BoolVector<3>(5)), (Vector3i{5, 2, 7}));
    CORRADE_COMPARE(Math::lerp(BoolVector<3>{false}, BoolVector<3>{true}, BoolVector<3>(5)), BoolVector<3>{5});

    /* Wrapped types */
    CORRADE_COMPARE(Math::lerp(2.0_degf, 5.0_degf, true), 5.0_degf);
}

void FunctionsTest::lerpInverted() {
    /* Floating-point scalar */
    CORRADE_COMPARE(Math::lerpInverted(2.0f, 5.0f, 3.5f), 0.5f);

    /* Floating-point vector */
    Vector3 a(-1.0f, 2.0f, 3.0f);
    Vector3 b(3.0f, -2.0f, 11.0f);
    CORRADE_COMPARE(Math::lerpInverted(a, b, Vector3(0.0f, 0.0f, 9.0f)), Vector3(0.25f, 0.5f, 0.75f));

    /* Wrapped types */
    CORRADE_COMPARE(Math::lerpInverted(2.0_degf, 5.0_degf, 3.5_degf), 0.5f);
}

void FunctionsTest::select() {
    /* Floating-point / integral scalar */
    CORRADE_COMPARE(Math::select(2.0f, 5.0f, 0.5f), 2.0f);
    CORRADE_COMPARE(Math::select(2.0f, 5.0f, 1.0f), 5.0f);
    CORRADE_COMPARE(Math::select(2, 5, -0.5f), 2);
    CORRADE_COMPARE(Math::select(2, 5, 1.1f), 5);

    /* Floating-point vector */
    Vector3 a(-1.0f, 2.0f, 3.0f);
    Vector3 b(3.0f, -2.0f, 11.0f);
    CORRADE_COMPARE(Math::select(a, b, -0.25f), Vector3(-1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(Math::select(a, b, 5.5f), Vector3(3.0f, -2.0f, 11.0f));

    /* Vector as interpolation phase */
    CORRADE_COMPARE(Math::select(a, b, Vector3(0.25f, 1.5f, 1.0f)), Vector3(-1.0f, -2.0f, 11.0f));

    /* Wrapped types */
    CORRADE_COMPARE(Math::select(2.0_degf, 5.0_degf, 0.5_degf), 2.0_degf);
}

void FunctionsTest::selectBool() {
    CORRADE_COMPARE(Math::select(true, false, 0.5f), true);
    CORRADE_COMPARE(Math::select(Math::BoolVector<4>{0xa}, Math::BoolVector<4>{0x5}, 1.1f), Math::BoolVector<4>{0x5});
    CORRADE_COMPARE(Math::select(Math::BoolVector<4>{0xa}, Math::BoolVector<4>{0x5}, Vector4{1.1f, -1.0f, 1.3f, 0.5f}), Math::BoolVector<4>{0xf});

    /* Wrapped types */
    CORRADE_COMPARE(Math::select(true, false, 0.5_degf), true);
}

void FunctionsTest::fma() {
    CORRADE_COMPARE(Math::fma(2.0f, 3.0f, 0.75f), 6.75f);
    CORRADE_COMPARE(Math::fma(Vector3( 2.0f,  1.5f,  0.5f),
                              Vector3( 3.0f,  2.0f, -1.0f),
                              Vector3(0.75f, 0.25f,  0.1f)),
                              Vector3(6.75f, 3.25f, -0.4f));

    /* Not testing wrapped types as the resulting unit is less clear -- convert
       to an unitless type first */
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

    /* Not testing wrapped types -- what unit should have degrees squared? */
}

void FunctionsTest::exp() {
    CORRADE_COMPARE(Math::exp(0.693147f), 2.0f);

    /* Not testing wrapped types -- what unit should have degrees squared? */
}

void FunctionsTest::div() {
    std::pair<Int, Int> div = Math::div(57, 6);
    CORRADE_COMPARE(div.first, 9);
    CORRADE_COMPARE(div.second, 3);
}

void FunctionsTest::isInf() {
    CORRADE_VERIFY(Math::isInf(Constants::inf()));
    CORRADE_VERIFY(Math::isInf(-Constants::inf()));
    CORRADE_VERIFY(!Math::isInf(Constants::nan()));
    CORRADE_VERIFY(!Math::isInf(5.3f));

    /* Wrapped types */
    CORRADE_VERIFY(Math::isInf(-Rad(Constants::inf())));
    CORRADE_VERIFY(!Math::isInf(5.3_degf));
}

void FunctionsTest::isInfVector() {
    CORRADE_COMPARE(Math::isInf(Vector3{0.3f, -Constants::inf(), 1.0f}), Math::BoolVector<3>{0x02});
    CORRADE_COMPARE(Math::isInf(Vector3{0.3f, 1.0f, -Constants::nan()}), Math::BoolVector<3>{0x00});
}

void FunctionsTest::isNan() {
    CORRADE_VERIFY(!Math::isNan(Constants::inf()));
    CORRADE_VERIFY(!Math::isNan(-Constants::inf()));
    CORRADE_VERIFY(Math::isNan(Constants::nan()));
    CORRADE_VERIFY(!Math::isNan(5.3f));

    /* Wrapped types */
    CORRADE_VERIFY(Math::isNan(-Rad(Constants::nan())));
    CORRADE_VERIFY(!Math::isNan(5.3_degf));
}

void FunctionsTest::isNanfVector() {
    CORRADE_COMPARE(Math::isNan(Vector3{0.3f, 1.0f, -Constants::nan()}), Math::BoolVector<3>{0x04});
    CORRADE_COMPARE(Math::isNan(Vector3{0.3f, -Constants::inf(), 1.0f}), Math::BoolVector<3>{0x00});
}

void FunctionsTest::reflect() {
    /* Reflection along Y will simply flip the Y component */
    CORRADE_COMPARE(Math::reflect(
        Vector3{1.0f, 2.0f, 3.0f},
        Vector3::yAxis()),
        (Vector3{1.0f, -2.0f, 3.0f}));

    CORRADE_COMPARE(Math::reflect(
        Vector3{2.0f, 1.0f, 1.0f},
        Vector3{1.0f, -1.0f, 1.0f}.normalized()),
        (Vector3{0.666667f, 2.33333f, -0.333333f}));
}

void FunctionsTest::reflectNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Math::reflect(Vector3{}, Vector3{1.0f});
    CORRADE_COMPARE(out.str(),
        "Math::reflect(): normal Vector(1, 1, 1) is not normalized\n");
}

void FunctionsTest::refract() {
    CORRADE_COMPARE(Math::refract(
        Vector3{1.0f, 0.0f, 1.0f}.normalized(),
        Vector3{0.0f, 0.0f, -1.0f}, 1.0f/1.5f),
        (Vector3{0.471405f, 0.0f, 0.881917f}));
    CORRADE_COMPARE(Math::refract(
        Vector3{4.0f, 1.0f, 1.0f}.normalized(),
        Vector3{0.0f, -2.0f, -1.0f}.normalized(), 1.0f/1.5f),
        (Vector3{0.628539f, 0.661393f, 0.409264f}));

    /* Total absorption */
    CORRADE_COMPARE(Math::refract(
        Vector3{1.0f, 0.1f, 0.0f}.normalized(),
        Vector3::yAxis(), 1.5f),
        Vector3{0.0f});
}

void FunctionsTest::refractNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Math::refract(Vector3{}, Vector3{1.0f}, 0.0f);
    CORRADE_COMPARE(out.str(),
        "Math::refract(): vectors Vector(0, 0, 0) and Vector(1, 1, 1) are not normalized\n");
}

void FunctionsTest::trigonometric() {
    CORRADE_COMPARE(Math::sin(30.0_degf), 0.5f);
    CORRADE_COMPARE(Math::sin(Rad(Constants::pi()/6)), 0.5f);
    CORRADE_COMPARE_AS(Math::asin(0.5f), 30.0_degf, Deg);

    CORRADE_COMPARE(Math::cos(60.0_degf), 0.5f);
    CORRADE_COMPARE(Math::cos(Rad(Constants::pi()/3)), 0.5f);
    CORRADE_COMPARE_AS(Math::acos(0.5f), 60.0_degf, Deg);

    CORRADE_COMPARE(Math::sincos(30.0_degf).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(30.0_degf).second, 0.8660254037844386f);
    CORRADE_COMPARE(Math::sincos(Rad(Constants::pi()/6)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(Rad(Constants::pi()/6)).second, 0.8660254037844386f);

    CORRADE_COMPARE(Math::tan(45.0_degf), 1.0f);
    CORRADE_COMPARE(Math::tan(Rad(Constants::pi()/4)), 1.0f);
    CORRADE_COMPARE_AS(Math::atan(1.0f), 45.0_degf, Deg);
}

void FunctionsTest::trigonometricWithBase() {
    /* Verify that the functions can be called with Unit<Deg, T> and Unit<Rad, T> */
    CORRADE_VERIFY((std::is_same<decltype(2*15.0_degf), Unit<Math::Deg, Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(2*Rad(Constants::pi()/12)), Unit<Math::Rad, Float>>::value));

    CORRADE_COMPARE(Math::sin(2*15.0_degf), 0.5f);
    CORRADE_COMPARE(Math::sin(2*Rad(Constants::pi()/12)), 0.5f);

    CORRADE_COMPARE(Math::cos(2*30.0_degf), 0.5f);
    CORRADE_COMPARE(Math::cos(2*Rad(Constants::pi()/6)), 0.5f);

    CORRADE_COMPARE(Math::sincos(2*15.0_degf).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(2*15.0_degf).second, 0.8660254037844386f);
    CORRADE_COMPARE(Math::sincos(2*Rad(Constants::pi()/12)).first, 0.5f);
    CORRADE_COMPARE(Math::sincos(2*Rad(Constants::pi()/12)).second, 0.8660254037844386f);

    CORRADE_COMPARE(Math::tan(2*22.5_degf), 1.0f);
    CORRADE_COMPARE(Math::tan(2*Rad(Constants::pi()/8)), 1.0f);
}

template<class T> void FunctionsTest::sincos() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    /* For GCC's __builtin_sincos this verifies that all specializations are
       correct */
    CORRADE_COMPARE(Math::sincos(Math::Deg<T>(T(30.0))).first, T(0.5));
    CORRADE_COMPARE(Math::sincos(Math::Deg<T>(T(30.0))).second, T(0.866025403784438647l));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsTest)
