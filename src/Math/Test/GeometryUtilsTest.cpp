/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "GeometryUtilsTest.h"

#include <limits>
#include <QtTest/QTest>

#include "Matrix3.h"
#include "GeometryUtils.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::GeometryUtilsTest)

typedef Magnum::Math::Matrix3<float> Matrix3;
typedef Magnum::Math::Vector3<float> Vector3;

Q_DECLARE_METATYPE(Matrix3)
Q_DECLARE_METATYPE(Vector3)

using namespace std;

namespace Magnum { namespace Math { namespace Test {

using ::Matrix3;
using ::Vector3;

void GeometryUtilsTest::intersection_data() {
    QTest::addColumn<Matrix3>("plane");
    QTest::addColumn<Vector3>("a");
    QTest::addColumn<Vector3>("b");
    QTest::addColumn<float>("expected");

    float plane[] = {
        0, 0, 0,
        1, 0, 0,
        0, 1, 0
    };
    QTest::newRow("inside") << Matrix3(plane) << Vector3(0, 0, -1) << Vector3(0, 0, 1) << 0.5f;
    QTest::newRow("outside") << Matrix3(plane) << Vector3(0, 0, 1) << Vector3(0, 0, 2) << -1.0f;
    QTest::newRow("NaN") << Matrix3(plane) << Vector3(1, 0, 0) << Vector3(0, 1, 0) << numeric_limits<float>::quiet_NaN();
    QTest::newRow("inf") << Matrix3(plane) << Vector3(1, 0, 1) << Vector3(0, 0, 1) << numeric_limits<float>::infinity();
}

void GeometryUtilsTest::intersection() {
    QFETCH(Matrix3, plane);
    QFETCH(Vector3, a);
    QFETCH(Vector3, b);
    QFETCH(float, expected);

    /* Handling also NaN, which cannot be fuzzy compared */
    float actual = GeometryUtils<float>::intersection(plane, a, b);

    /* All possible workarounds for comparing to inf and NaN */
    if(expected > numeric_limits<float>::max()) QCOMPARE(actual, expected);
    else if(expected != expected) QVERIFY(actual != actual);
    else QVERIFY(actual == expected);
}

}}}
