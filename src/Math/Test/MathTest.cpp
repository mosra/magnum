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

#include "MathTest.h"

#include <QtTest/QTest>

#include "Math.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::MathTest)

namespace Magnum { namespace Math { namespace Test {

void MathTest::degrad() {
    QCOMPARE(deg(90.0), Constants<double>::Pi/2);
    QCOMPARE(deg(90.0f), Constants<float>::Pi/2);
    QCOMPARE(rad(Constants<double>::Pi/2), Constants<double>::Pi/2);
}

void MathTest::pow() {
    QCOMPARE(Math::pow<10>(2ul), 1024ul);
    QCOMPARE(Math::pow<0>(3ul), 1ul);
    QCOMPARE(Math::pow<2>(2.0f), 4.0f);
}

void MathTest::log() {
    QCOMPARE(Math::log(2, 256), 8ul);
    QCOMPARE(Math::log(256, 2), 0ul);
}

}}}
