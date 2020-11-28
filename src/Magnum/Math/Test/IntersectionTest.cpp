/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2020 janos <janos.meny@googlemail.com>

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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Intersection.h"

namespace Magnum { namespace Math { namespace Test { namespace {

using namespace Literals;

struct IntersectionTest: Corrade::TestSuite::Tester {
    explicit IntersectionTest();

    void planeLine();
    void lineLine();

    void pointFrustum();
    void rangeFrustum();
    void rayRange();
    void aabbFrustum();
    void sphereFrustum();

    void pointCone();
    void pointDoubleCone();
    void sphereCone();
    void sphereConeView();
    void sphereConeViewNotRigid();
    void rangeCone();
    void aabbCone();
};

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<Double> Vector3d;
typedef Math::Vector4<Float> Vector4;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Matrix4<Double> Matrix4d;
typedef Math::Frustum<Float> Frustum;
typedef Math::Constants<Float> Constants;
typedef Math::Range3D<Float> Range3D;
typedef Math::Rad<Float> Rad;
typedef Math::Rad<Double> Radd;

IntersectionTest::IntersectionTest() {
    addTests({&IntersectionTest::planeLine,
              &IntersectionTest::lineLine,

              &IntersectionTest::pointFrustum,
              &IntersectionTest::rangeFrustum,
              &IntersectionTest::rayRange,
              &IntersectionTest::aabbFrustum,
              &IntersectionTest::sphereFrustum,

              &IntersectionTest::pointCone,
              &IntersectionTest::pointDoubleCone,
              &IntersectionTest::sphereCone,
              &IntersectionTest::sphereConeView,
              &IntersectionTest::sphereConeViewNotRigid,
              &IntersectionTest::rangeCone,
              &IntersectionTest::aabbCone});
}

void IntersectionTest::planeLine() {
    const Vector3 planePosition(-1.0f, 1.0f, 0.5f);
    const Vector3 planeNormal(0.0f, 0.0f, 1.0f);
    const Vector4 planeEquation = Math::planeEquation(planeNormal, planePosition);
    CORRADE_COMPARE(planeEquation, (Vector4{0.0f, 0.0f, 1.0f, -0.5f}));

    /* Inside line segment */
    CORRADE_COMPARE(Intersection::planeLine(planeEquation,
        {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 2.0f}), 0.75f);

    /* Outside line segment */
    CORRADE_COMPARE(Intersection::planeLine(planeEquation,
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}), -0.5f);

    /* Line lies on the plane */
    CORRADE_COMPARE(Intersection::planeLine(planeEquation,
        {1.0f, 0.5f, 0.5f}, {-1.0f, 0.5f, 0.0f}), Constants::nan());

    /* Line is parallel to the plane */
    CORRADE_COMPARE(Intersection::planeLine(planeEquation,
        {1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}), -Constants::inf());
}

void IntersectionTest::lineLine() {
    const Vector2 p(-1.0f, -1.0f);
    const Vector2 r(1.0, 2.0f);

    /* Inside both line segments */
    CORRADE_COMPARE(Intersection::lineSegmentLineSegment(p, r,
        {0.0f, 0.0f}, {-1.0f, 0.0f}), std::make_pair(0.5f, 0.5f));
    CORRADE_COMPARE(Intersection::lineSegmentLine(p, r,
        {0.0f, 0.0f}, {-1.0f, 0.0f}), 0.5);

    /* Outside both line segments */
    CORRADE_COMPARE(Intersection::lineSegmentLineSegment(p, r,
        {0.0f, -2.0f}, {-1.0f, 0.0f}), std::make_pair(-0.5f, 1.5f));
    CORRADE_COMPARE(Intersection::lineSegmentLine(p, r,
        {0.0f, -2.0f}, {-1.0f, 0.0f}), -0.5f);

    /* Collinear lines */
    const auto tu = Intersection::lineSegmentLineSegment(p, r,
        {0.0f, 1.0f}, {-1.0f, -2.0f});
    CORRADE_COMPARE(tu.first, -Constants::nan());
    CORRADE_COMPARE(tu.second, -Constants::nan());
    CORRADE_COMPARE(Intersection::lineSegmentLine(p, r,
        {0.0f, 1.0f}, {-1.0f, -2.0f}), -Constants::nan());

    /* Parallel lines */
    CORRADE_COMPARE(Intersection::lineSegmentLineSegment(p, r,
        {0.0f, 0.0f}, {1.0f, 2.0f}), std::make_pair(Constants::inf(), Constants::inf()));
    CORRADE_COMPARE(Intersection::lineSegmentLine(p, r,
        {0.0f, 0.0f}, {1.0f, 2.0f}), Constants::inf());
}

void IntersectionTest::pointFrustum() {
    const Frustum frustum{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 10.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 10.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 10.0f}};

    /* Point on edge */
    CORRADE_VERIFY(Intersection::pointFrustum({}, frustum));
    /* Point inside */
    CORRADE_VERIFY(Intersection::pointFrustum({5.0f, 5.0f, 5.0f}, frustum));
    /* Point outside */
    CORRADE_VERIFY(!Intersection::pointFrustum({0.0f, 0.0f, 100.0f}, frustum));
}

void IntersectionTest::rangeFrustum() {
    const Frustum frustum{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 5.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 10.0f}};

    /* Fully inside */
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D{Vector3{1.0f}, Vector3{2.0f}}, frustum));
    /* Intersects with exactly one plane each */
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({2.4f, -0.1f, 4.9f}, Vector3{0.2f}), frustum));
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({2.4f, 0.9f, 4.9f}, Vector3{0.2f}), frustum));
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({-0.1f, 0.4f, 4.9f}, Vector3{0.2f}), frustum));
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({4.9f, 0.4f, 4.9f}, Vector3{0.2f}), frustum));
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({2.4f, 0.4f, -0.1f}, Vector3{0.2f}), frustum));
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D::fromSize({2.4f, 0.4f, 9.9f}, Vector3{0.2f}), frustum));
    /* Bigger than frustum, but still intersects */
    CORRADE_VERIFY(Intersection::rangeFrustum(Range3D{Vector3{-100.0f}, Vector3{100.0f}}, frustum));
    /* Outside of frustum */
    CORRADE_VERIFY(!Intersection::rangeFrustum(Range3D{Vector3{-10.0f}, Vector3{-5.0f}}, frustum));
}

void IntersectionTest::rayRange() {
    const Vector3 origin{2.0f, 2.0f, 2.0f};
    const Range3D range{{-1.0f, -1.0f, -1.0f},
                        { 1.0f,  1.0f,  1.0f}};

    const Vector3 center{0.0f, 0.0f, 1.0f};
    const Vector3 edge{0.0f, -1.0f, 1.0f};
    const Vector3 corner{-1.0f, -1.0f, 1.0f};
    const Float eps = 1e-6f;

    /* intersection at face center */
    const Vector3 direction1 = center - origin;
    const Vector3 invDir1 = 1.0f/direction1;
    CORRADE_VERIFY(Intersection::rayRange(origin, invDir1, range));

    /* intersection close to edge */
    const Vector3 direction2 = edge + Vector3{0.0f, eps, 0.0f} - origin;
    const Vector3 invDir2 = 1.0f/direction2;
    CORRADE_VERIFY(Intersection::rayRange(origin, invDir2, range));

    /* no intersection close to edge */
    const Vector3 direction3 = edge - Vector3{0.0f, eps, 0.0f} - origin;
    const Vector3 invDir3 = 1.0f/direction3;
    CORRADE_VERIFY(!Intersection::rayRange(origin, invDir3, range));

    /* intersection close to corner */
    const Vector3 direction4 = corner + Vector3{eps, eps, 0.0f} - origin;
    const Vector3 invDir4 = 1.0f/direction4;
    CORRADE_VERIFY(Intersection::rayRange(origin, invDir4, range));

    /* no intersection close to corner */
    const Vector3 direction5 = corner - Vector3{eps, eps, 0.0f} - origin;
    const Vector3 invDir5 = 1.0f/direction5;
    CORRADE_VERIFY(!Intersection::rayRange(origin, invDir5, range));

    /* divide by zero test with intersection */
    const Vector3 direction6{0.0f, 0.0f, -1.0f};
    const Vector3 invDir6 = 1.0f/direction6;
    CORRADE_VERIFY(Intersection::rayRange({0.0f, 0.0f, 2.0f}, invDir6, range));

    /* divide by zero test without intersection */
    const Vector3 direction7{0.0f, 0.0f, 1.0f};
    const Vector3 invDir7 = 1.0f/direction7;
    CORRADE_VERIFY(!Intersection::rayRange(origin, invDir7, range));
}

void IntersectionTest::aabbFrustum() {
    const Frustum frustum{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 5.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 10.0f}};

    /* Fully inside */
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{0.0f}, Vector3{1.0f}, frustum));
    /* Intersects with exactly one plane each */
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{2.5f, 0.0f, 5.0f}, Vector3{0.1f}, frustum));
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{2.5f, 1.0f, 5.0f}, Vector3{0.1f}, frustum));
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{0.0f, 0.5f, 5.0f}, Vector3{0.1f}, frustum));
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{5.0f, 0.5f, 5.0f}, Vector3{0.1f}, frustum));
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{2.5f, 0.5f, 0.0f}, Vector3{0.1f}, frustum));
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{2.5f, 0.5f, 10.0f}, Vector3{0.1f}, frustum));
    /* Bigger than frustum, but still intersects */
    CORRADE_VERIFY(Intersection::aabbFrustum(Vector3{0.0f}, Vector3{100.0f}, frustum));
    /* Outside of frustum */
    CORRADE_VERIFY(!Intersection::aabbFrustum(Vector3{-7.5f}, Vector3{2.5f}, frustum));
}

void IntersectionTest::sphereFrustum() {
    const Frustum frustum{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 10.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 10.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 10.0f}};

    /* Sphere on edge */
    CORRADE_VERIFY(Intersection::sphereFrustum({0.0f, 0.0f, -1.0f}, 1.5f, frustum));
    /* Sphere inside */
    CORRADE_VERIFY(Intersection::sphereFrustum({5.5f, 5.5f, 5.5f}, 1.5f,  frustum));
    /* Sphere outside */
    CORRADE_VERIFY(!Intersection::sphereFrustum({0.0f, 0.0f, 100.0f}, 0.5f, frustum));
}

void IntersectionTest::pointCone() {
    const Vector3 center{0.1f, 0.2f, 0.3f};
    const Vector3 normal{Vector3{0.5f, 1.0f, 2.0f}.normalized()};
    const Rad angle{72.0_degf};

    const Vector3d centerDouble{1.0, -2.0, 1.3};
    const Vector3d normalDouble{Vector3{0.5, 1.0, 2.0}.normalized()};
    const Radd angleDouble{72.0_deg};

    /* Some vector along the surface of the cone */
    auto axis = Math::cross(Vector3::yAxis(), normal).normalized();
    auto surface = Matrix4::rotation(0.5f*angle, axis).transformVector(normal);
    /* Normal on the curved surface */
    auto sNormal = Matrix4::rotation(90.0_degf, axis).transformVector(surface);
    /* Same for Double precision */
    auto axisDouble = Math::cross(Vector3d::yAxis(), normalDouble).normalized();
    auto surfaceDouble = Matrix4d::rotation(0.5*angleDouble, axisDouble).transformVector(normalDouble);

    /* Point on edge */
    CORRADE_VERIFY(Intersection::pointCone(center, center, normal, angle));
    /* Point inside */
    CORRADE_VERIFY(Intersection::pointCone(center + normal, center, normal, angle));
    /* Point outside */
    CORRADE_VERIFY(!Intersection::pointCone(Vector3{}, center, normal, angle));
    CORRADE_VERIFY(!Intersection::pointCone(center + 5.0f*surface + 0.01f*sNormal, center, normal, angle));
    /* Point behind the cone plane */
    CORRADE_VERIFY(!Intersection::pointCone(-normal, center, normal, angle));

    /* Point touching cone */
    {
        CORRADE_EXPECT_FAIL("Point touching cone fails, possibly because of precision.");
        CORRADE_VERIFY(Intersection::pointCone(centerDouble, centerDouble + surfaceDouble, normalDouble, angleDouble));
    }
}

void IntersectionTest::pointDoubleCone() {
    const Vector3 center{0.1f, 0.2f, 0.3f};
    const Vector3 normal{Vector3{0.5f, 1.0f, 2.0f}.normalized()};
    const Rad angle{72.0_degf};

    /* Some vector along the surface of the cone */
    auto axis = Math::cross(Vector3::yAxis(), normal).normalized();
    auto surface = Matrix4::rotation(0.5f*angle, axis).transformVector(normal);
    /* Normal on the curved surface */
    auto sNormal = Matrix4::rotation(90.0_degf, axis).transformVector(surface);

    /* Point on edge */
    CORRADE_VERIFY(Intersection::pointDoubleCone(center, center, normal, angle));
    /* Point inside */
    CORRADE_VERIFY(Intersection::pointDoubleCone(center + normal, center, normal, angle));
    CORRADE_VERIFY(Intersection::pointDoubleCone(center - normal, center, normal, angle));
    /* Point outside */
    CORRADE_VERIFY(!Intersection::pointDoubleCone(center + sNormal, center, normal, angle));
}

void IntersectionTest::sphereCone() {
    const Vector3 center{1.0f, -2.0f, 1.3f};
    const Vector3 normal{Vector3{0.5f, 1.0f, 2.0f}.normalized()};
    const Rad angle(72.0_degf);

    /* Same for Double precision */
    const Vector3d centerDouble{1.0, -2.0, 1.3};
    const Vector3d normalDouble{Vector3{0.5, 1.0, 2.0}.normalized()};
    const Radd angleDouble(72.0_deg);

    /* Some vector along the surface of the cone */
    auto axis = Math::cross(Vector3::yAxis(), normal).normalized();
    auto surface = Matrix4::rotation(0.5f*angle, axis).transformVector(normal);
    /* Normal on the curved surface */
    auto sNormal = Matrix4::rotation(90.0_degf, axis).transformVector(surface);

    /* Same for Double precision */
    auto axisDouble = Math::cross(Vector3d::yAxis(), normalDouble).normalized();
    auto surfaceDouble = Matrix4d::rotation(0.5*angleDouble, axisDouble).transformVector(normalDouble);
    auto sNormalDouble = Matrix4d::rotation(90.0_deg, axisDouble).transformVector(surfaceDouble);

    /* Sphere fully contained in cone */
    CORRADE_VERIFY(Intersection::sphereCone(center + normal*5.0f, 0.8f, center, normal, angle));
    /* Sphere fully contained in double side of cone */
    CORRADE_VERIFY(!Intersection::sphereCone(center + normal*-5.0f, 0.75f, center, normal, angle));
    /* Sphere fully outside of the cone */
    CORRADE_VERIFY(!Intersection::sphereCone(center + surface + sNormal*5.0f, 0.75f, center, normal, angle));

    /* Sphere intersecting apex with sphere center behind the cone plane */
    CORRADE_VERIFY(Intersection::sphereCone(center - normal*0.1f, 0.55f, center, normal, angle));
    /* Sphere intersecting apex with sphere center in front of the cone plane */
    CORRADE_VERIFY(Intersection::sphereCone(center + normal*0.1f, 0.55f, center, normal, angle));

    /* Sphere barely touching the surface of the cone, from inside and outside the cone */
    {
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL("Cone touching from the outside fails, possibly because of precision.");
        #else
        CORRADE_EXPECT_FAIL_IF(!Intersection::sphereCone(centerDouble + 4.0*surfaceDouble + sNormalDouble*0.5, 0.5, centerDouble, normalDouble, angleDouble), "Cone touching from the outside fails on optimized Emscripten builds, possibly because of precision.");
        #endif
        CORRADE_VERIFY(Intersection::sphereCone(centerDouble + 4.0*surfaceDouble + sNormalDouble*0.5, 0.5, centerDouble, normalDouble, angleDouble));
    }
    CORRADE_VERIFY(Intersection::sphereCone(centerDouble + 4.0*surfaceDouble - sNormalDouble*0.5, 0.5, centerDouble, normalDouble, angleDouble));

    /* Same on double side of the cone */
    CORRADE_VERIFY(!Intersection::sphereCone(center - 4.0f*surface + sNormal*0.5f, 0.5f, center, normal, angle));
    CORRADE_VERIFY(!Intersection::sphereCone(center - 4.0f*surface - sNormal*0.5f, 0.5f, center, normal, angle));

    /* Sphere clearly, but not fully intersecting the cone */
    CORRADE_VERIFY(Intersection::sphereCone(center + surface + sNormal*0.25f, 0.5f, center, normal, angle));
    /* Sphere with center on the cone's surface */
    CORRADE_VERIFY(Intersection::sphereCone(center + 4.0f*surface, 0.5f, center, normal, angle));

    /* Same as above on double side of the cone */
    CORRADE_VERIFY(!Intersection::sphereCone(center - surface + sNormal*0.25f, 0.5f, center, normal, angle));
    CORRADE_VERIFY(!Intersection::sphereCone(center - 4.0f*surface, 0.5f, center, normal, angle));
}

void IntersectionTest::sphereConeView() {
    const Vector3 center{1.0f, -2.0f, 1.3f};
    const Vector3 normal{Vector3{0.5f, 1.0f, 2.0f}.normalized()};
    const Matrix4 coneView = Matrix4::lookAt(center, center + normal, Vector3::yAxis()).invertedRigid();

    const Vector3d centerDouble{1.0, -2.0, 1.3};
    const Vector3d normalDouble{Vector3{0.5, 1.0, 2.0}.normalized()};
    const Matrix4d coneViewDouble = Matrix4d::lookAt(centerDouble, centerDouble + normalDouble, Vector3d::yAxis()).invertedRigid();

    const Rad angle(72.0_degf);
    const Radd angleDouble(72.0_deg);

    /* Some vector along the surface of the cone */
    auto axis = Math::cross(Vector3::yAxis(), normal).normalized();
    auto surface = Matrix4::rotation(0.5f*angle, axis).transformVector(normal);
    /* Normal on the curved surface */
    auto sNormal = Matrix4::rotation(90.0_degf, axis).transformVector(surface);

    /* Sphere fully contained in cone */
    CORRADE_VERIFY(Intersection::sphereConeView(center + normal*5.0f, 0.8f, coneView, angle));
    /* Sphere fully contained in double side of cone */
    CORRADE_VERIFY(!Intersection::sphereConeView(center + normal*-5.0f, 0.75f, coneView, angle));
    /* Sphere fully outside of the cone */
    CORRADE_VERIFY(!Intersection::sphereConeView(center + surface + sNormal*5.0f, 0.75f, coneView, angle));

    /* Sphere intersecting apex with sphere center behind the cone plane */
    CORRADE_VERIFY(Intersection::sphereConeView(center - normal*0.1f, 0.55f, coneView, angle));
    /* Sphere intersecting apex with sphere center in front of the cone plane */
    CORRADE_VERIFY(Intersection::sphereConeView(center + normal*0.1f, 0.55f, coneView, angle));

    /* Sphere barely touching the surface of the cone, from inside and outside the cone
       Note: behaviour differs from sphereCone! */
    CORRADE_VERIFY(Intersection::sphereConeView(centerDouble + 4.0*Vector3d(surface) + Vector3d(sNormal)*0.5, 0.5, coneViewDouble, angleDouble));
    CORRADE_VERIFY(Intersection::sphereConeView(centerDouble + 4.0*Vector3d(surface) - Vector3d(sNormal)*0.5, 0.5, coneViewDouble, angleDouble));
    /* Same on double side of the cone */
    CORRADE_VERIFY(!Intersection::sphereConeView(center - 4.0f*surface + sNormal*0.5f, 0.5f, coneView, angle));
    CORRADE_VERIFY(!Intersection::sphereConeView(center - 4.0f*surface - sNormal*0.5f, 0.5f, coneView, angle));

    /* Sphere clearly, but not fully intersecting the cone */
    CORRADE_VERIFY(Intersection::sphereConeView(center + surface + sNormal*0.25f, 0.5f, coneView, angle));
    /* Sphere with center on the cone's surface */
    CORRADE_VERIFY(Intersection::sphereConeView(center + 4.0f*surface, 0.5f, coneView, angle));

    /* Same as above on double side of the cone */
    CORRADE_VERIFY(!Intersection::sphereConeView(center - surface + sNormal*0.25f, 0.5f, coneView, angle));
    CORRADE_VERIFY(!Intersection::sphereConeView(center - 4.0f*surface, 0.5f, coneView, angle));

}

void IntersectionTest::sphereConeViewNotRigid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!Intersection::sphereConeView({}, 1.0f, Matrix4{ZeroInit}, {}));
    CORRADE_COMPARE(out.str(),
        "Math::Intersection::sphereConeView(): coneView does not represent a rigid transformation:\n"
        "Matrix(0, 0, 0, 0,\n"
        "       0, 0, 0, 0,\n"
        "       0, 0, 0, 0,\n"
        "       0, 0, 0, 0)\n");
}

void IntersectionTest::rangeCone() {
    const Vector3 center{1.0f, -2.0f, 1.3f};
    const Vector3 normal{0.453154f, 0.422618f, 0.784886f};
    const Rad angle{72.0_degf};

    /* Box fully inside cone */
    CORRADE_VERIFY(Intersection::rangeCone(Range3D::fromSize(
        15.0f*normal - Vector3{1.0f}, Vector3{2.0f}), center, normal, angle));
    /* Box intersecting cone */
    CORRADE_VERIFY(Intersection::rangeCone(Range3D::fromSize(
        5.0f*normal - Vector3{10.0f, 10.0f, 0.5f}, Vector3{20.0f, 20.0f, 1.0f}),
        center, normal, angle));
    CORRADE_VERIFY(Intersection::rangeCone(
        Range3D{{-1.0f, -2.0f, -3.0f}, {1.0f, 2.0f, 3.0f}}, center, normal, angle));
    /* Cone inside large box */
    CORRADE_VERIFY(Intersection::rangeCone(Range3D::fromSize(
        12.0f*normal - Vector3{20.0f}, Vector3{40.0f}), center, normal, angle));
    /* Same corner chosen on all intersecting faces */
    CORRADE_VERIFY(Intersection::rangeCone(
        Range3D{{2.0f, -0.1f, -1.5f}, {3.0f, 0.1f, 1.5f}},
        center, {0.353553f, 0.707107f, 0.612372f}, angle));

    /* Boxes outside cone */
    CORRADE_VERIFY(!Intersection::rangeCone(
        Range3D{Vector3{2.0f, 2.0f, -2.0f}, Vector3{8.0f, 7.0f, 2.0f}},
        center, normal, angle));
    CORRADE_VERIFY(!Intersection::rangeCone(
        Range3D{Vector3{6.0f, 5.0f, -7.0f}, Vector3{5.0f, 9.0f, -3.0f}},
        center, normal, angle));
    /* Box fully contained in double cone */
    CORRADE_VERIFY(!Intersection::rangeCone(
        Range3D::fromSize(-15.0f*normal - Vector3{1.0f}, Vector3{2.0f}),
        center, normal, angle));
}

void IntersectionTest::aabbCone() {
    const Vector3 center{1.0f, -2.0f, 1.3f};
    const Vector3 normal{0.453154f, 0.422618f, 0.784886f};
    const Rad angle{72.0_degf};

    /* Box fully inside cone */
    CORRADE_VERIFY(Intersection::aabbCone(15.0f*normal, Vector3{1.0f}, center, normal, angle));
    /* Box intersecting cone */
    CORRADE_VERIFY(Intersection::aabbCone(5.0f*normal, {10.0f, 10.0f, 0.5f}, center, normal, angle));
    CORRADE_VERIFY(Intersection::aabbCone({}, { 1.0f, 2.0f, 3.0f }, center, normal, angle));
    /* Cone inside large box */
    CORRADE_VERIFY(Intersection::aabbCone(12.0f*normal, {20.0f, 20.0f, 20.0f}, center, normal, angle));
    /* Same corner chosen on all intersecting faces */
    CORRADE_VERIFY(Intersection::aabbCone({2.5f, 0.0f, 0.0f}, {0.5f, 0.1f, 1.5f}, center, {0.353553f, 0.707107f, 0.612372f}, angle));

    /* Boxes outside cone */
    CORRADE_VERIFY(!Intersection::aabbCone({5.0f, 5.0f, 0.0f}, {3.0f, 2.0f, 2.0f}, center, normal, angle));
    CORRADE_VERIFY(!Intersection::aabbCone({8.0f, 7.0f, -5.0f}, {2.0f, 2.0f, 2.0f}, center, normal, angle));
    /* Box fully contained in double cone */
    CORRADE_VERIFY(!Intersection::aabbCone(-15.0f*normal, Vector3{1.0f}, center, normal, angle));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::IntersectionTest)
