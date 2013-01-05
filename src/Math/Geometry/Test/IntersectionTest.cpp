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
#include <limits>

#include "Math/Geometry/Intersection.h"

namespace Magnum { namespace Math { namespace Geometry { namespace Test {

class IntersectionTest: public Corrade::TestSuite::Tester {
    public:
        IntersectionTest();

        void planeLine();
};

typedef Magnum::Math::Vector3<float> Vector3;

IntersectionTest::IntersectionTest() {
    addTests(&IntersectionTest::planeLine);
}

void IntersectionTest::planeLine() {
    Vector3 planePosition(-1.0f, 1.0f, 0.5f);
    Vector3 planeNormal(0.0f, 0.0f, 1.0f);

    /* Inside line segment */
    CORRADE_COMPARE((Intersection::planeLine(planePosition, planeNormal,
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 1.0f))), 0.75f);

    /* Outside line segment */
    CORRADE_COMPARE((Intersection::planeLine(planePosition, planeNormal,
        Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 2.0f))), -0.5f);

    /* Line lies on the plane */
    CORRADE_COMPARE(Intersection::planeLine(planePosition, planeNormal,
        Vector3(1.0f, 0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.5f)), std::numeric_limits<float>::quiet_NaN());

    /* Line is parallell to the plane */
    CORRADE_COMPARE((Intersection::planeLine(planePosition, planeNormal,
        Vector3(1.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f))), -std::numeric_limits<float>::infinity());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Geometry::Test::IntersectionTest)
