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
        void clamp();
        void normalizeUnsigned();
        void normalizeSigned();
        void denormalizeUnsigned();
        void denormalizeSigned();
        void renormalizeUnsinged();
        void renormalizeSinged();

        void pow();
        void log();
        void log2();
};

typedef Math::Vector3<float> Vector3;
typedef Math::Vector3<std::uint8_t> Vector3ub;
typedef Math::Vector3<std::int8_t> Vector3b;
typedef Math::Vector3<std::int32_t> Vector3i;

FunctionsTest::FunctionsTest() {
    addTests(&FunctionsTest::min,
             &FunctionsTest::max,
             &FunctionsTest::clamp,
             &FunctionsTest::normalizeUnsigned,
             &FunctionsTest::normalizeSigned,
             &FunctionsTest::denormalizeUnsigned,
             &FunctionsTest::denormalizeSigned,
             &FunctionsTest::renormalizeUnsinged,
             &FunctionsTest::renormalizeSinged,
             &FunctionsTest::pow,
             &FunctionsTest::log,
             &FunctionsTest::log2);
}

void FunctionsTest::min() {
    CORRADE_COMPARE(Math::min(5, 9), 5);
    CORRADE_COMPARE(Math::min(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(5, -5, 2));
}

void FunctionsTest::max() {
    CORRADE_COMPARE(Math::max(5, 9), 9);
    CORRADE_COMPARE(Math::max(Vector3i(5, -3, 2), Vector3i(9, -5, 18)), Vector3i(9, -3, 18));
}

void FunctionsTest::clamp() {
    CORRADE_COMPARE(Math::clamp(0.5f, -1.0f, 5.0f), 0.5f);
    CORRADE_COMPARE(Math::clamp(-1.6f, -1.0f, 5.0f), -1.0f);
    CORRADE_COMPARE(Math::clamp(9.5f, -1.0f, 5.0f), 5.0f);

    CORRADE_COMPARE(Math::clamp(Vector3(0.5f, -1.6f, 9.5f), -1.0f, 5.0f), Vector3(0.5f, -1.0f, 5.0f));
}

void FunctionsTest::normalizeUnsigned() {
    CORRADE_COMPARE((Math::normalize<float, std::uint8_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint8_t>(255)), 1.0f);

    CORRADE_COMPARE((Math::normalize<double, std::uint32_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<double, std::uint32_t>(std::numeric_limits<std::uint32_t>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, std::uint64_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, std::uint64_t>(std::numeric_limits<std::uint64_t>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<float, std::uint16_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::uint16_t>(std::numeric_limits<std::uint16_t>::max())), 1.0f);

    CORRADE_COMPARE((Math::normalize<float, std::uint16_t>(8192)), 0.125002f);
    CORRADE_COMPARE((Math::normalize<float, std::uint16_t>(49152)), 0.750011f);

    CORRADE_COMPARE(Math::normalize<Vector3>(Vector3ub(0, 127, 255)), Vector3(0.0f, 0.498039f, 1.0f));
}

void FunctionsTest::normalizeSigned() {
    CORRADE_COMPARE((Math::normalize<float, std::int8_t>(127)), 1.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int8_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int8_t>(-128)), -1.0f);

    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(std::numeric_limits<std::int16_t>::min())), -1.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(0)), 0.0f);
    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(std::numeric_limits<std::int16_t>::max())), 1.0f);

    CORRADE_COMPARE((Math::normalize<double, std::int32_t>(std::numeric_limits<std::int32_t>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<double, std::int32_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<double, std::int32_t>(std::numeric_limits<std::int32_t>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<long double, std::int64_t>(std::numeric_limits<std::int64_t>::min())), -1.0);
    CORRADE_COMPARE((Math::normalize<long double, std::int64_t>(0)), 0.0);
    CORRADE_COMPARE((Math::normalize<long double, std::int64_t>(std::numeric_limits<std::int64_t>::max())), 1.0);

    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(16384)), 0.500015f);
    CORRADE_COMPARE((Math::normalize<float, std::int16_t>(-16384)), -0.500015f);

    CORRADE_COMPARE(Math::normalize<Vector3>(Vector3b(0, -127, 64)), Vector3(0.0f, -1.0f, 0.503937f));
}

void FunctionsTest::denormalizeUnsigned() {
    CORRADE_COMPARE(Math::denormalize<std::uint8_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::uint8_t>(1.0f), 255);

    CORRADE_COMPARE(Math::denormalize<std::uint16_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::uint16_t>(1.0f), std::numeric_limits<std::uint16_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::uint32_t>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<std::uint32_t>(1.0), std::numeric_limits<std::uint32_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::uint64_t>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<std::uint64_t>(1.0), std::numeric_limits<std::uint64_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::uint16_t>(0.33f), 21626);
    CORRADE_COMPARE(Math::denormalize<std::uint16_t>(0.66f), 43253);

    CORRADE_COMPARE(Math::denormalize<Vector3ub>(Vector3(0.0f, 0.5f, 1.0f)), Vector3ub(0, 127, 255));
}

void FunctionsTest::denormalizeSigned() {
    CORRADE_COMPARE(Math::denormalize<std::int8_t>(-1.0f), -127);
    CORRADE_COMPARE(Math::denormalize<std::int8_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::int8_t>(1.0f), 127);

    CORRADE_COMPARE(Math::denormalize<std::int16_t>(-1.0f), std::numeric_limits<std::int16_t>::min()+1);
    CORRADE_COMPARE(Math::denormalize<std::int16_t>(0.0f), 0);
    CORRADE_COMPARE(Math::denormalize<std::int16_t>(1.0f), std::numeric_limits<std::int16_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::int32_t>(-1.0), std::numeric_limits<std::int32_t>::min()+1);
    CORRADE_COMPARE(Math::denormalize<std::int32_t>(0.0), 0);
    CORRADE_COMPARE(Math::denormalize<std::int32_t>(1.0), std::numeric_limits<std::int32_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::int64_t>(-1.0l), std::numeric_limits<std::int64_t>::min()+1);
    CORRADE_COMPARE(Math::denormalize<std::int64_t>(0.0l), 0);
    CORRADE_COMPARE(Math::denormalize<std::int64_t>(1.0l), std::numeric_limits<std::int64_t>::max());

    CORRADE_COMPARE(Math::denormalize<std::int16_t>(-0.33f), -10813);
    CORRADE_COMPARE(Math::denormalize<std::int16_t>(0.66f), 21626);

    CORRADE_COMPARE(Math::denormalize<Vector3b>(Vector3(0.0f, -1.0f, 0.5f)), Vector3b(0, -127, 63));
}

void FunctionsTest::renormalizeUnsinged() {
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::uint8_t>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::uint8_t>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::uint16_t>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::uint16_t>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<double>(Math::denormalize<std::uint32_t>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<double>(Math::denormalize<std::uint32_t>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<std::uint64_t>(0.0l)), 0.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<std::uint64_t>(1.0l)), 1.0l);
}

void FunctionsTest::renormalizeSinged() {
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int8_t>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int8_t>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int8_t>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int16_t>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int16_t>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::normalize<float>(Math::denormalize<std::int16_t>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::normalize<double>(Math::denormalize<std::int32_t>(-1.0)), -1.0);
    CORRADE_COMPARE(Math::normalize<double>(Math::denormalize<std::int32_t>(0.0)), 0.0);
    CORRADE_COMPARE(Math::normalize<double>(Math::denormalize<std::int32_t>(1.0)), 1.0);

    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<std::int64_t>(-1.0l)), -1.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<std::int64_t>(0.0l)), 0.0l);
    CORRADE_COMPARE(Math::normalize<long double>(Math::denormalize<std::int64_t>(1.0l)), 1.0l);
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
