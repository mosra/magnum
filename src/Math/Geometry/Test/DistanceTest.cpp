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

#include <limits>
#include <TestSuite/Tester.h>

#include "Math/Constants.h"
#include "Math/Geometry/Distance.h"

namespace Magnum { namespace Math { namespace Geometry { namespace Test {

class DistanceTest: public Corrade::TestSuite::Tester {
    public:
        DistanceTest();

        void linePoint2D();
        void linePoint3D();
        void lineSegmentPoint2D();
        void lineSegmentPoint3D();
};

typedef Magnum::Math::Vector2<float> Vector2;
typedef Magnum::Math::Vector3<float> Vector3;

DistanceTest::DistanceTest() {
    addTests(&DistanceTest::linePoint2D,
             &DistanceTest::linePoint3D,
             &DistanceTest::lineSegmentPoint2D,
             &DistanceTest::lineSegmentPoint3D);
}

void DistanceTest::linePoint2D() {
    Vector2 a(0.0f);
    Vector2 b(1.0f);

    /* Point on the line */
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector2(0.25f))), 0.0f);

    /* The distance should be the same for all equidistant points */
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector2(1.0f, 0.0f))),
        1.0f/Constants<float>::sqrt2());
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector2(1.0f, 0.0f)+Vector2(100.0f))),
        1.0f/Constants<float>::sqrt2());

    /* Be sure that *Squared() works the same, as it has slightly different implementation */
    CORRADE_COMPARE((Distance::linePointSquared(a, b, Vector2(1.0f, 0.0f))),
        0.5f);
}

void DistanceTest::linePoint3D() {
    Vector3 a(0.0f);
    Vector3 b(1.0f);

    /* Point on the line */
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector3(0.25f))), 0.0f);

    /* The distance should be the same for all equidistant points */
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f))),
        Constants<float>::sqrt2()/Constants<float>::sqrt3());
    CORRADE_COMPARE((Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f)+Vector3(100.0f))),
        Constants<float>::sqrt2()/Constants<float>::sqrt3());
}

void DistanceTest::lineSegmentPoint2D() {
    Vector2 a(0.0f);
    Vector2 b(1.0f);

    /* Point on the line segment */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(0.25f))), 0.0f);

    /* Point on the line, outside the segment, closer to A */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(-1.0f))), Constants<float>::sqrt2());
    /* Be sure that *Squared() works the same, as it has slightly different implementation */
    CORRADE_COMPARE((Distance::lineSegmentPointSquared(a, b, Vector2(-1.0f))), 2.0f);

    /* Point on the line, outside the segment, closer to B */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(1.0f+1.0f/Constants<float>::sqrt2()))), 1.0f);
    CORRADE_COMPARE((Distance::lineSegmentPointSquared(a, b, Vector2(1.0f+1.0f/Constants<float>::sqrt2()))), 1.0f);

    /* Point next to the line segment */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(1.0f, 0.0f))),
        1.0f/Constants<float>::sqrt2());
    CORRADE_COMPARE((Distance::lineSegmentPointSquared(a, b, Vector2(1.0f, 0.0f))),
        0.5f);

    /* Point outside the line segment, closer to A */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(1.0f, 0.0f)-Vector2(1.0f, 0.5f))), 0.5f);
    CORRADE_COMPARE((Distance::lineSegmentPointSquared(a, b, Vector2(1.0f, 0.0f)-Vector2(1.0f, 0.5f))), 0.25f);

    /* Point outside the line segment, closer to B */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector2(1.0f, 0.0f)+Vector2(0.5f, 1.0f))), 0.5f);
    CORRADE_COMPARE((Distance::lineSegmentPointSquared(a, b, Vector2(1.0f, 0.0f)+Vector2(0.5f, 1.0f))), 0.25f);
}

void DistanceTest::lineSegmentPoint3D() {
    Vector3 a(0.0f);
    Vector3 b(1.0f);

    /* Point on the line segment */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(0.25f))), 0.0f);

    /* Point on the line, outside the segment, closer to A */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(-1.0f))), +Constants<float>::sqrt3());

    /* Point on the line, outside the segment, closer to B */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(1.0f+1.0f/Constants<float>::sqrt3()))), 1.0f);

    /* Point next to the line segment */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(1.0f, 0.0f, 1.0f))),
        Constants<float>::sqrt2()/Constants<float>::sqrt3());

    /* Point outside the line segment, closer to A */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(1.0f, 0.0f, 1.0f)-Vector3(1.0f))), 1.0f);

    /* Point outside the line segment, closer to B */
    CORRADE_COMPARE((Distance::lineSegmentPoint(a, b, Vector3(1.0f, 0.0f, 1.0f)+Vector3(1.0f))), +Constants<float>::sqrt2());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Geometry::Test::DistanceTest)
