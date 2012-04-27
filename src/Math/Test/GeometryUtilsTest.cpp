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

#include "GeometryUtilsTest.h"

#include <limits>
#include <QtTest/QTest>

#include "GeometryUtils.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::GeometryUtilsTest)

using namespace std;

namespace Magnum { namespace Math { namespace Test {

typedef Magnum::Math::Vector3<float> Vector3;

void GeometryUtilsTest::intersection() {
    Vector3 planePosition;
    Vector3 planeNormal(0.0f, 0.0f, 1.0f);

    /* Inside line segment */
    QCOMPARE((GeometryUtils::intersection(planePosition, planeNormal,
        Vector3(0, 0, -1), Vector3(0, 0, 1))), 0.5f);

    /* Outside line segment */
    QCOMPARE((GeometryUtils::intersection(planePosition, planeNormal,
        Vector3(0, 0, 1), Vector3(0, 0, 2))), -1.0f);

    /* Line lies on the plane */
    float nan = GeometryUtils::intersection(planePosition, planeNormal,
        Vector3(1, 0, 0), Vector3(0, 1, 0));
    QVERIFY(nan != nan);

    /* Line is parallell to the plane */
    QCOMPARE((GeometryUtils::intersection(planePosition, planeNormal,
        Vector3(1, 0, 1), Vector3(0, 0, 1))), numeric_limits<float>::infinity());
}

}}}
