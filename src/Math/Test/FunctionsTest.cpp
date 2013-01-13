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

namespace Magnum { namespace Math { namespace Test {

class FunctionsTest: public Corrade::TestSuite::Tester {
    public:
        FunctionsTest();

        void normalize();
        void denormalize();
        void clamp();
        void pow();
        void log();
        void log2();
};

FunctionsTest::FunctionsTest() {
    addTests(&FunctionsTest::normalize,
             &FunctionsTest::denormalize,
             &FunctionsTest::clamp,
             &FunctionsTest::pow,
             &FunctionsTest::log,
             &FunctionsTest::log2);
}

void FunctionsTest::normalize() {
    /* Range for signed and unsigned */
    CORRADE_COMPARE((Math::normalize<float, std::int8_t>(-128)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int8_t>(127)), 1.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint8_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint8_t>(255)), 1.0f);

    /* Between */
    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(16384)), 0.750011f);
    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(-16384)), 0.250004f);

    /* Test overflow for large types */
    CORRADE_COMPARE((Math::normalize<float, std::int32_t>(std::numeric_limits<std::int32_t>::min())), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int32_t>(std::numeric_limits<std::int32_t>::max())), 1.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint32_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint32_t>(std::numeric_limits<std::uint32_t>::max())), 1.0f);

    CORRADE_COMPARE((Math::normalize<double, std::int64_t>(std::numeric_limits<std::int64_t>::min())), 0.0);
    CORRADE_COMPARE((Math::normalize<double, std::int64_t>(std::numeric_limits<std::int64_t>::max())), 1.0);
    CORRADE_COMPARE((Math::normalize<double, std::uint64_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<double, std::uint64_t>(std::numeric_limits<std::uint64_t>::max())), 1.0);
}

void FunctionsTest::denormalize() {
    /* Range for signed and unsigned */
    CORRADE_COMPARE(Math::denormalize<std::int8_t>(0.0f), -128);
    CORRADE_COMPARE(Math::denormalize<std::int8_t>(1.0f), 127);
    CORRADE_COMPARE(Math::denormalize<std::uint8_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::uint8_t>(1.0f), 255);

    /* Between */
    CORRADE_COMPARE(Math::denormalize<std::int16_t>(0.33f), -11141);
    CORRADE_COMPARE(Math::denormalize<std::int16_t>(0.66f), 10485);

    /* Test overflow for large types */
    CORRADE_COMPARE(Math::denormalize<std::int32_t>(0.0f), std::numeric_limits<std::int32_t>::min());
    CORRADE_COMPARE(Math::denormalize<std::uint32_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::int64_t>(0.0), std::numeric_limits<std::int64_t>::min());
    CORRADE_COMPARE(Math::denormalize<std::uint64_t>(0.0), 0);

    CORRADE_COMPARE(Math::denormalize<std::int32_t>(1.0), std::numeric_limits<std::int32_t>::max());
    CORRADE_COMPARE(Math::denormalize<std::uint32_t>(1.0), std::numeric_limits<std::uint32_t>::max());

//     {
//         CORRADE_EXPECT_FAIL("Denormalize doesn't work for large types well");
//         CORRADE_COMPARE((Math::denormalize<long long, long double>(1.0)), numeric_limits<long long>::max());
//         CORRADE_COMPARE((Math::denormalize<unsigned long long, long double>(1.0)), numeric_limits<unsigned long long>::max());
//     }
}

void FunctionsTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);
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

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsTest)
