/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <TestSuite/Tester.h>

#include "Math/Functions.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test {

class FunctionsTest: public Corrade::TestSuite::Tester {
    public:
        FunctionsTest();

        void min();
        void max();
        void abs();
        void sqrt();
        void clamp();
        void lerp();
        void normalizeUnsigned();
        void normalizeSigned();
        void denormalizeUnsigned();
        void denormalizeSigned();
        void renormalizeUnsinged();
        void renormalizeSinged();

        void normalizeTypeDeduction();

        void pow();
        void log();
        void log2();
        void trigonometric();
};

typedef Math::Constants<Float> Constants;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<UnsignedByte> Vector3ub;
typedef Math::Vector3<Byte> Vector3b;
typedef Math::Vector3<Int> Vector3i;

FunctionsTest::FunctionsTest() {
    addTests({&FunctionsTest::min,
              &FunctionsTest::max,
              &FunctionsTest::abs,
              &FunctionsTest::sqrt,
              &FunctionsTest::clamp,
              &FunctionsTest::lerp,
              &FunctionsTest::normalizeUnsigned,
              &FunctionsTest::normalizeSigned,
              &FunctionsTest::denormalizeUnsigned,
              &FunctionsTest::denormalizeSigned,
              &FunctionsTest::renormalizeUnsinged,
              &FunctionsTest::renormalizeSinged,

              &FunctionsTest::normalizeTypeDeduction,

              &FunctionsTest::pow,
              &FunctionsTest::log,
              &FunctionsTest::log2,
              &FunctionsTest::trigonometric});
}

void FunctionsTest::min() {
    CORRADE_COMPARE(Math::min(5, 9), 5);
    CORRADE_COMPARE(Math::min(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(5, -5, 2));
}

void FunctionsTest::max() {
    CORRADE_COMPARE(Math::max(5, 9), 9);
    CORRADE_COMPARE(Math::max(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(9, -3, 18));
}

void FunctionsTest::abs() {
    CORRADE_COMPARE(Math::abs(-5), 5);
    CORRADE_COMPARE(Math::abs(5), 5);
    CORRADE_COMPARE(Math::abs(Vector3i(5, -3, 2)), Vector3i(5, 3, 2));
}

void FunctionsTest::sqrt() {
    CORRADE_COMPARE(Math::sqrt(16), 4);
    CORRADE_COMPARE(Math::sqrt(Vector3i(256, 1, 0)), Vector3i(16, 1, 0));
}

void FunctionsTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);

    CORRADE_COMPARE(Math::clamp(Vector3(0.5f, -1.6f, 9.5f), -1.0f, 5.0f), Vector3(0.5f, -1.0f, 5.0f));
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
    typedef Math::Vector<3, Int> Vector3ub;
    Vector3ub c(0, 128, 64);
    Vector3ub d(16, 0, 32);
    CORRADE_COMPARE(Math::lerp(c, d, 0.25f), Vector3ub(4, 96, 56));
}

void FunctionsTest::normalizeUnsigned() {
    CORRADE_COMPARE((Math::normalize<Float, UnsignedByte>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<Float, UnsignedByte>(255)), 1.0f);

    CORRADE_COMPARE((Math::normalize<Double, UnsignedInt>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<Double, UnsignedInt>(std::numeric_limits<UnsignedInt>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, UnsignedLong>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, UnsignedLong>(std::numeric_limits<UnsignedLong>::max())), 1.0);

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

    CORRADE_COMPARE((Math::normalize<Double, Int>(std::numeric_limits<Int>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<Double, Int>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<Double, Int>(std::numeric_limits<Int>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, Long>(std::numeric_limits<Long>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<long double, Long>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, Long>(std::numeric_limits<Long>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<Float, Short>(16384)), 0.500015f);
    CORRADE_COMPARE((Math::normalize<Float, Short>(-16384)), -0.500015f);

    CORRADE_COMPARE(Math::normalize<Vector3>(Vector3b(0, -127, 64)), Vector3(0.0f, -1.0f, 0.503937f));
}

void FunctionsTest::denormalizeUnsigned() {
    CORRADE_COMPARE(Math::denormalize<UnsignedByte>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedByte>(1.0f), 255);

    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedShort>(1.0f), std::numeric_limits<UnsignedShort>::max());

    CORRADE_COMPARE(Math::denormalize<UnsignedInt>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<UnsignedInt>(1.0), std::numeric_limits<UnsignedInt>::max());

    CORRADE_COMPARE(Math::denormalize<UnsignedLong>(0.0), 0);
    {
        CORRADE_EXPECT_FAIL("Wrong result with GCC and non-optimized code.");
        CORRADE_VERIFY(false);
        //CORRADE_COMPARE(Math::denormalize<UnsignedLong>(1.0), std::numeric_limits<UnsignedLong>::max());
    }

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

    CORRADE_COMPARE(Math::denormalize<Int>(-1.0), std::numeric_limits<Int>::min()+1);
    CORRADE_COMPARE(Math::denormalize<Int>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<Int>(1.0), std::numeric_limits<Int>::max());

    CORRADE_COMPARE(Math::denormalize<Long>(-1.0l), std::numeric_limits<Long>::min()+1);
    CORRADE_COMPARE(Math::denormalize<Long>(0.0l), 0);
    CORRADE_COMPARE(Math::denormalize<Long>(1.0l), std::numeric_limits<Long>::max());

    CORRADE_COMPARE(Math::denormalize<Short>(-0.33f), -10813);
    CORRADE_COMPARE(Math::denormalize<Short>(0.66f), 21626);

    CORRADE_COMPARE(Math::denormalize<Vector3b>(Vector3(0.0f, -1.0f, 0.5f)), Vector3b(0, -127, 63));
}

void FunctionsTest::renormalizeUnsinged() {
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedByte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedByte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedShort>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<UnsignedShort>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<UnsignedInt>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<UnsignedInt>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<UnsignedLong>(0.0l)), 0.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<UnsignedLong>(1.0l)), 1.0l);
}

void FunctionsTest::renormalizeSinged() {
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Byte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<Float>(Math::denormalize<Short>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(-1.0)), -1.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<Double>(Math::denormalize<Int>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(-1.0l)), -1.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(0.0l)), 0.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<Long>(1.0l)), 1.0l);
}

void FunctionsTest::normalizeTypeDeduction() {
    CORRADE_COMPARE(Math::normalize<Float>('\x7F'), 1.0f);
    CORRADE_COMPARE((Math::normalize<Float, Byte>('\x7F')), 1.0f);
}

void FunctionsTest::pow() {
    CORRADE_COMPARE(Math::pow<10>(2ul), 1024ul);
    CORRADE_COMPARE(Math::pow<0>(3ul), 1ul);
    CORRADE_COMPARE(Math::pow<2>(2.0f), 4.0f);
}

void FunctionsTest::log() {
    CORRADE_COMPARE(Math::log(2, 256), 8ul);
    CORRADE_COMPARE(Math::log(256, 2), 0ul);
}

void FunctionsTest::log2() {
    CORRADE_COMPARE(Math::log2(2153), 11);
}

void FunctionsTest::trigonometric() {
    CORRADE_COMPARE(Math::sin(Deg(30.0f)), 0.5f);
    CORRADE_COMPARE(Math::sin(Rad(Constants::pi()/6)), 0.5f);
    CORRADE_COMPARE_AS(Math::asin(0.5f), Deg(30.0f), Deg);

    CORRADE_COMPARE(Math::cos(Deg(60.0f)), 0.5f);
    CORRADE_COMPARE(Math::cos(Rad(Constants::pi()/3)), 0.5f);
    CORRADE_COMPARE_AS(Math::acos(0.5f), Deg(60.0f), Deg);

    CORRADE_COMPARE(Math::tan(Deg(45.0f)), 1.0f);
    CORRADE_COMPARE(Math::tan(Rad(Constants::pi()/4)), 1.0f);
    CORRADE_COMPARE_AS(Math::atan(1.0f), Deg(45.0f), Deg);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsTest)
