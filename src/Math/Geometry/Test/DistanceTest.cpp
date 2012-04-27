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

#include "DistanceTest.h"

#include <limits>
#include <QtTest/QTest>

#include "Math.h"
#include "Distance.h"

QTEST_APPLESS_MAIN(Magnum::Math::Geometry::Test::DistanceTest)

using namespace std;

namespace Magnum { namespace Math { namespace Geometry { namespace Test {

typedef Magnum::Math::Vector3<float> Vector3;

void DistanceTest::linePoint() {
    Vector3 a(0.0f);
    Vector3 b(1.0f);

    /* Point on the line */
    QCOMPARE((Distance::linePoint(a, b, Vector3(0.25f))), 0.0f);

    /* The distance should be the same for all equidistant points */
    QCOMPARE((Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f))),
        Constants<float>::Sqrt2/Constants<float>::Sqrt3);
    QCOMPARE((Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f)+Vector3(100.0f))),
        Constants<float>::Sqrt2/Constants<float>::Sqrt3);
}

}}}}
