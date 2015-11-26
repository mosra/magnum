/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

    void min();
    void minList();
    void max();
    void maxList();
    void minmax();
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
    void normalizeUnsigned();
    void normalizeSigned();
    void denormalizeUnsigned();
    void denormalizeSigned();
    void renormalizeUnsinged();
    void renormalizeSinged();

    void normalizeTypeDeduction();

    void powIntegral();
    void logIntegral();
    void log2();
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
    addTests({&FunctionsTest::min,
              &FunctionsTest::minList,
              &FunctionsTest::max,
              &FunctionsTest::maxList,
              &FunctionsTest::minmax,
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
              &FunctionsTest::normalizeUnsigned,
              &FunctionsTest::normalizeSigned,
              &FunctionsTest::denormalizeUnsigned,
              &FunctionsTest::denormalizeSigned,
              &FunctionsTest::renormalizeUnsinged,
              &FunctionsTest::renormalizeSinged,

              &FunctionsTest::normalizeTypeDeduction,

              &FunctionsTest::powIntegral,
              &FunctionsTest::logIntegral,
              &FunctionsTest::log2,
              &FunctionsTest::div,
              &FunctionsTest::trigonometric,
              &FunctionsTest::trigonometricWithBase});
}

void FunctionsTest::min() {
    CORRADE_COMPARE(Math::min(5, 9), 5);
    CORRADE_COMPARE(Math::min(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(5, -5, 2));
}

void FunctionsTest::minList() {
    CORRADE_COMPARE(Math::min({5, -2, 9}), -2);
    CORRADE_COMPARE(Math::min({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(-2, -5, 2));
}

void FunctionsTest::max() {
    CORRADE_COMPARE(Math::max(5, 9), 9);
    CORRADE_COMPARE(Math::max(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(9, -3, 18));
}

void FunctionsTest::maxList() {
    CORRADE_COMPARE(Math::max({5, -2, 9}), 9);
    CORRADE_COMPARE(Math::max({Vector3i(5, -3, 2),
                               Vector3i(-2, 14, 7),
                               Vector3i(9, -5, 18)}), Vector3i(9, 14, 18));
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

void FunctionsTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);

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

    /* We are working around missing std::round() in NaCl newlib, thus we must
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
    CORRADE_COMPARE(Math::lerp(Vector3i{1, 2, 3}, Vector3i{5, 6, 7}, BoolVector<3>(5)), (Vector3i{1, 6, 3}));
    CORRADE_COMPARE(Math::lerp(BoolVector<3>{false}, BoolVector<3>{true}, BoolVector<3>(5)), BoolVector<3>{2});
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

void FunctionsTest::normalizeUnsigned() {
    CORRADE_COMPARE((Math::normalize<Float, UnsignedByte>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<Float, UnsignedByte>(255)), 1.0f);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE((Math::normalize<Double, UnsignedInt>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<Double, UnsignedInt>(std::numeric_limits<UnsignedInt>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, UnsignedLong>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, UnsignedLong>(std::numeric_limits<UnsignedLong>::max())), 1.0);
    #endif

    CORRADE_COMPARE((Math::normalize<Float, UnsignedShort>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<Float, UnsignedShort>(std::numeric_limits<UnsignedShort>::max())), 1.0f);

    CORRADE_COMPARE((Math::normalize<Float, UnsignedShort>(8192)), 0.125002f);
    CORRADE_COMPARE((Math::normalize<Float, UnsignedShort>(49152)), 0.750011f);

    CORRADE_COMPARE(Math::normalize<Vector3>(Vector3ub(0, 127, 255)), Vector3(0.0f, 0.498039f, 1.0f));
}

void FunctionsTest::normalizeSigned() {
    CORRADE_COMPARE((Math::normalize<Float, Byte>(127)), 1.0f);
    CORRADE_COMPARE((Math::normalize<Float, Byte>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<Float, Byte>(-128)), -1.0f);

    CORRADE_COMPARE((Math::normalize<Float, Short>(std::numeric_limits<Short>::min())), -1.0f);
    CORRADE_COMPARE((Math::normalize<Float, Short>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<Float, Short>(std::numeric_limits<Short>::max())), 1.0f);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE((Math::normalize<Double, Int>(std::numeric_limits<Int>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<Double, Int>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<Double, Int>(std::numeric_limits<Int>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, Long>(std::numeric_limits<Long>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<long double, Long>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, Long>(std::numeric_limits<Long>::max())), 1.0);
    #endif

    CORRADE_COMPARE((Math::normalize<Float, Short>(16384)), 0.500015f);
    CORRADE_COMPARE((Math::normalize<Float, Short>(-16384)), -0.500015f);

    CORRADE_COMPARE(Math::normalize<Vector3>(Vector3b(0, -127, 64)), Vector3(0.0f, -1.0f, 0.503937f));
}

void FunctionsTest::denormalizeUnsigned() {
    CORRADE_COMPARE(Math::denormalize<UnsignedByte>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedByte>(1.0f), 255);

    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(1.0f), std::numeric_limits<UnsignedShort>::max());

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Math::denormalize<UnsignedInt>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedInt>(1.0), std::numeric_limits<UnsignedInt>::max());

    CORRADE_COMPARE(Math::denormalize<UnsignedLong>(0.0l), 0);
    {
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC <= 2015.");
        #endif
        CORRADE_COMPARE(Math::denormalize<UnsignedLong>(1.0l), std::numeric_limits<UnsignedLong>::max());
    }
    #endif

    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(0.33f), 21626);
    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(0.66f), 43253);

    CORRADE_COMPARE(Math::denormalize<Vector3ub>(Vector3(0.0f, 0.5f, 1.0f)), Vector3ub(0, 127, 255));
}

void FunctionsTest::denormalizeSigned() {
    CORRADE_COMPARE(Math::denormalize<Byte>(-1.0f), -127);
    CORRADE_COMPARE(Math::denormalize<Byte>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<Byte>(1.0f), 127);

    CORRADE_COMPARE(Math::denormalize<Short>(-1.0f), std::numeric_limits<Short>::min()+1);
    CORRADE_COMPARE(Math::denormalize<Short>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<Short>(1.0f), std::numeric_limits<Short>::max());

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Math::denormalize<Int>(-1.0), std::numeric_limits<Int>::min()+1);
    CORRADE_COMPARE(Math::denormalize<Int>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<Int>(1.0), std::numeric_limits<Int>::max());

    {
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC <= 2015.");
        #endif
        CORRADE_COMPARE(Math::denormalize<Long>(-1.0l), std::numeric_limits<Long>::min()+1);
    }
    CORRADE_COMPARE(Math::denormalize<Long>(0.0l), 0);
    {
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC <= 2015.");
        #endif
        CORRADE_COMPARE(Math::denormalize<Long>(1.0l), std::numeric_limits<Long>::max());
    }
    #endif

    CORRADE_COMPARE(Math::denormalize<Short>(-0.33f), -10813);
    CORRADE_COMPARE(Math::denormalize<Short>(0.66f), 21626);

    CORRADE_COMPARE(Math::denormalize<Vector3b>(Vector3(0.0f, -1.0f, 0.5f)), Vector3b(0, -127, 63));
}

void FunctionsTest::renormalizeUnsinged() {
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedByte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedByte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedShort>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedShort>(1.0f)), 1.0f);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<UnsignedInt>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<UnsignedInt>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<UnsignedLong>(0.0l)), 0.0l);
    {
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC <= 2015.");
        #endif
        CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<UnsignedLong>(1.0l)), 1.0l);
    }
    #endif
}

void FunctionsTest::renormalizeSinged() {
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(1.0f)), 1.0f);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(-1.0)), -1.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(-1.0l)), -1.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(0.0l)), 0.0l);
    {
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC <= 2015.");
        #endif
        CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(1.0l)), 1.0l);
    }
    #endif
}

void FunctionsTest::normalizeTypeDeduction() {
    if(std::is_signed<char>::value)
        CORRADE_COMPARE(Math::normalize<Float>('\x7F'), 1.0f);
    else {
        /* Raspberry Pi `char` is unsigned (huh) */
        CORRADE_VERIFY(std::is_unsigned<char>::value);
        CORRADE_COMPARE(Math::normalize<Float>('\x7F'), 0.498039f);
    }
    CORRADE_COMPARE((Math::normalize<Float, Byte>('\x7F')), 1.0f);
}

void FunctionsTest::powIntegral() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);

    /* Constant expression */
    constexpr Int a = Math::pow<3>(5);
    CORRADE_COMPARE(a, 125);
}

void FunctionsTest::logIntegral() {
    CORRADE_COMPARE(Math::log(2, 256), 8ul);
    CORRADE_COMPARE(Math::log(256, 2), 0ul);
}

void FunctionsTest::log2() {
    CORRADE_COMPARE(Math::log2(2153), 11);
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
