/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Constants.h"
#include "Magnum/Math/Distance.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct DistanceTest: TestSuite::Tester {
    explicit DistanceTest();

    void pointPoint2D();
    void pointPoint3D();

    void linePoint2D();
    void linePoint3D();
    void lineSegmentPoint2D();
    void lineSegmentPoint3D();

    void pointPlane();
    void pointPlaneScaled();
    void pointPlaneNormalized();
    void pointPlaneNormalizedNotNormalized();
};

using Magnum::Vector2;
using Magnum::Vector3;
using Magnum::Vector4;
using Magnum::Constants;

DistanceTest::DistanceTest() {
    addTests({&DistanceTest::pointPoint2D,
              &DistanceTest::pointPoint3D,

              &DistanceTest::linePoint2D,
              &DistanceTest::linePoint3D,
              &DistanceTest::lineSegmentPoint2D,
              &DistanceTest::lineSegmentPoint3D,

              &DistanceTest::pointPlane,
              &DistanceTest::pointPlaneScaled,
              &DistanceTest::pointPlaneNormalized,
              &DistanceTest::pointPlaneNormalizedNotNormalized});
}

void DistanceTest::pointPoint2D() {
    CORRADE_COMPARE(Distance::pointPoint(Vector2{5.0f, 1.0f},
                                         Vector2{6.0f, 1.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPointSquared(Vector2{5.0f, 1.0f},
                                                Vector2{6.0f, 1.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPoint(Vector2{5.0f, 1.0f},
                                         Vector2{5.0f, 2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPointSquared(Vector2{5.0f, 1.0f},
                                                Vector2{5.0f, 2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPoint(Vector2{5.0f, 1.0f},
                                         Vector2{6.0f, 2.0f}),
        Constants::sqrt2());
    CORRADE_COMPARE(Distance::pointPointSquared(Vector2{5.0f, 1.0f},
                                                Vector2{6.0f, 2.0f}), 2.0f);
}

void DistanceTest::pointPoint3D() {
    CORRADE_COMPARE(Distance::pointPoint(Vector3{5.0f, 1.0f, -2.0f},
                                         Vector3{6.0f, 1.0f, -2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPointSquared(Vector3{5.0f, 1.0f, -2.0f},
                                                Vector3{6.0f, 1.0f, -2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPoint(Vector3{5.0f, 1.0f, -2.0f},
                                         Vector3{5.0f, 2.0f, -2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPointSquared(Vector3{5.0f, 1.0f, -2.0f},
                                                Vector3{5.0f, 2.0f, -2.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPoint(Vector3{5.0f, 1.0f, -2.0f},
                                         Vector3{5.0f, 1.0f, -3.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPointSquared(Vector3{5.0f, 1.0f, -2.0f},
                                                Vector3{5.0f, 1.0f, -3.0f}), 1.0f);
    CORRADE_COMPARE(Distance::pointPoint(Vector3{5.0f, 1.0f, -2.0f},
                                         Vector3{6.0f, 2.0f, -3.0f}),
        Constants::sqrt3());
    CORRADE_COMPARE(Distance::pointPointSquared(Vector3{5.0f, 1.0f, -2.0f},
                                                Vector3{6.0f, 2.0f, -3.0f}), 3.0f);
}

void DistanceTest::linePoint2D() {
    Vector2 a(0.0f);
    Vector2 b(1.0f);

    /* Point on the line */
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector2(0.25f)),
                    0.0f);

    /* The distance should be the same for all equidistant points */
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector2(1.0f, 0.0f)),
                    1.0f/Constants::sqrt2());
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector2(1.0f, 0.0f) + Vector2(100.0f)),
                    1.0f/Constants::sqrt2());

    /* Be sure that *Squared() works the same, as it has slightly different implementation */
    CORRADE_COMPARE(Distance::linePointSquared(a, b, Vector2(1.0f, 0.0f)),
                    0.5f);
}

void DistanceTest::linePoint3D() {
    Vector3 a(0.0f);
    Vector3 b(1.0f);

    /* Point on the line */
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector3(0.25f)), 0.0f);

    /* The distance should be the same for all equidistant points */
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f)),
                    Constants::sqrt2()/Constants::sqrt3());
    CORRADE_COMPARE(Distance::linePoint(a, b, Vector3(1.0f, 0.0f, 1.0f) + Vector3(100.0f)),
                    Constants::sqrt2()/Constants::sqrt3());

    /* Check that 3D implementation gives the same result as 2D implementation */
    CORRADE_COMPARE(Distance::linePoint(a, {1.0f, 1.0f, 0.0f}, Vector3(1.0f, 0.0f, 0.0f)),
                    1.0f/Constants::sqrt2());
}

void DistanceTest::lineSegmentPoint2D() {
    Vector2 a(0.0f);
    Vector2 b(1.0f);

    /* Point on the line segment */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector2(0.25f)),
                    0.0f);

    /* Point on the line, outside the segment, closer to A */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector2(-1.0f)),
                    Constants::sqrt2());
    /* Be sure that *Squared() works the same, as it has slightly different implementation */
    CORRADE_COMPARE(Distance::lineSegmentPointSquared(a, b, Vector2(-1.0f)),
                    2.0f);

    /* Point on the line, outside the segment, closer to B */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector2(1.0f + 1.0f/Constants::sqrt2())),
                    1.0f);
    CORRADE_COMPARE(Distance::lineSegmentPointSquared(a, b, Vector2(1.0f + 1.0f/Constants::sqrt2())),
                    1.0f);

    /* Point next to the line segment */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, {1.0f, 0.0f}),
                    1.0f/Constants::sqrt2());
    CORRADE_COMPARE(Distance::lineSegmentPointSquared(a, b, {1.0f, 0.0f}),
                    0.5f);

    /* Point outside the line segment, closer to A */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector2(1.0f, 0.0f) - Vector2(1.0f, 0.5f)),
                    0.5f);
    CORRADE_COMPARE(Distance::lineSegmentPointSquared(a, b, Vector2(1.0f, 0.0f) - Vector2(1.0f, 0.5f)),
                    0.25f);

    /* Point outside the line segment, closer to B */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector2(1.0f, 0.0f) + Vector2(0.5f, 1.0f)),
                    0.5f);
    CORRADE_COMPARE(Distance::lineSegmentPointSquared(a, b, Vector2(1.0f, 0.0f) + Vector2(0.5f, 1.0f)),
                    0.25f);
}

void DistanceTest::lineSegmentPoint3D() {
    Vector3 a(0.0f);
    Vector3 b(1.0f);

    /* Point on the line segment */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector3(0.25f)),
                    0.0f);

    /* Point on the line, outside the segment, closer to A */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector3(-1.0f)),
                    Constants::sqrt3());

    /* Point on the line, outside the segment, closer to B */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector3(1.0f + 1.0f/Constants::sqrt3())),
                    1.0f);

    /* Point next to the line segment */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, {1.0f, 0.0f, 1.0f}),
                    Constants::sqrt2()/Constants::sqrt3());

    /* Point outside the line segment, closer to A */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector3(1.0f, 0.0f, 1.0f) - Vector3(1.0f)),
                    1.0f);

    /* Point outside the line segment, closer to B */
    CORRADE_COMPARE(Distance::lineSegmentPoint(a, b, Vector3(1.0f, 0.0f, 1.0f) + Vector3(1.0f)),
                    Constants::sqrt2());
}

void DistanceTest::pointPlane() {
    Vector3 point{0.0f, 0.0f, 0.0f};
    Vector4 plane{3.0f, 0.0f, 4.0f, 5.0f};

    CORRADE_COMPARE(Distance::pointPlane(point, plane), 1.0f);
}

void DistanceTest::pointPlaneScaled() {
    Vector3 point{1.0f, 1.0f, 1.0f};
    Vector4 plane{2.0f, 2.0f, 2.0f, 0.0f};

    CORRADE_COMPARE(Distance::pointPlaneScaled(point, plane), 6.0f);
}

void DistanceTest::pointPlaneNormalized() {
    Vector3 point{1.0f, 2.0f, 3.0f};

    const Vector4 plane{0.0f, 1.0f, 0.0f, 1.0f};
    CORRADE_COMPARE(Distance::pointPlaneNormalized(point, plane), 3.0f);
}

void DistanceTest::pointPlaneNormalizedNotNormalized() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    Vector4 invalidPlane{2.0f, 2.0f, 2.0f, 0.0f};
    Distance::pointPlaneNormalized({}, invalidPlane);
    CORRADE_COMPARE(out, "Math::Distance::pointPlaneNormalized(): plane normal Vector(2, 2, 2) is not normalized\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DistanceTest)
