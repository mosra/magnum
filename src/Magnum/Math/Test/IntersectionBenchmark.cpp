/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>

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

#include <random>
#include <utility>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Angle.h"
#include "Magnum/Math/Intersection.h"

namespace Magnum { namespace Math { namespace Test { namespace {

template<class T> bool rangeFrustumNaive(const Math::Range3D<T>& box, const Math::Frustum<T>& frustum) {
    for(const Math::Vector4<T>& plane: frustum) {
        bool cornerHit = 0;

        for(UnsignedByte c = 0; c != 8; ++c) {
            const Math::Vector3<T> corner = Math::lerp(box.min(), box.max(), Math::BoolVector<3>{c});

            if(Distance::pointPlaneScaled<T>(corner, plane) >= T(0)) {
                cornerHit = true;
                break;
            }
        }

        /* All corners are outside this plane */
        if(!cornerHit) return false;
    }

    return true;
}

/*
    Ground truth, slow sphere cone intersection - calculating exact distances,
    no optimizations, no precomputations

    sphereCenter     Sphere center
    radius           Sphere radius
    origin           Origin of the cone
    normal           Cone normal
    angle            Cone opening angle (0 < angle < pi)

    Returns true if the sphere intersects with the cone.
*/
template<class T> bool sphereConeGT(
        const Math::Vector3<T>& sphereCenter, const T radius,
        const Math::Vector3<T>& origin, const Math::Vector3<T>& normal, const Math::Rad<T> angle) {
    const Math::Vector3<T> diff = sphereCenter - origin;
    const Math::Vector3<T> dir = diff.normalized();
    const Math::Rad<T> halfAngle = angle/T(2);

    /* Compute angle between normal and point */
    const Math::Rad<T> actual = Math::acos(dot(normal, dir));

    /* Distance from cone surface */
    const T distanceFromCone = Math::sin(actual - halfAngle)*diff.length();

    /* Either the sphere center lies in cone, or cone is max radius away from
       the cone */
    return actual <= halfAngle || distanceFromCone <= radius;
}

template<class T>
Math::Matrix4<T> coneViewFromCone(const Math::Vector3<T>& origin, const Math::Vector3<T>& normal) {
    return Math::Matrix4<T>::lookAt(origin, origin + normal, Math::Vector3<T>::yAxis()).inverted();
}

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Frustum<Float> Frustum;
typedef Math::Range3D<Float> Range3D;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;

struct IntersectionBenchmark: Corrade::TestSuite::Tester {
    explicit IntersectionBenchmark();

    void rangeFrustumNaive();
    void rangeFrustum();

    void rangeCone();

    void sphereFrustum();

    void sphereConeNaive();
    void sphereCone();
    void sphereConeView();

    Frustum _frustum;
    struct {
        Vector3 origin;
        Vector3 normal;
        Rad angle;
    } _cone;
    Matrix4 _coneView;

    std::vector<Range3D> _boxes;
    std::vector<Vector4> _spheres;
};

IntersectionBenchmark::IntersectionBenchmark() {
    addBenchmarks({&IntersectionBenchmark::rangeFrustumNaive,
                   &IntersectionBenchmark::rangeFrustum,

                   &IntersectionBenchmark::rangeCone,

                   &IntersectionBenchmark::sphereFrustum,

                   &IntersectionBenchmark::sphereConeNaive,
                   &IntersectionBenchmark::sphereCone,
                   &IntersectionBenchmark::sphereConeView}, 10);

    /* Generate random data for the benchmarks */
    std::random_device rnd;
    std::mt19937 g(rnd());
    /* Position distribution */
    std::uniform_real_distribution<float> pd(-10.0f, 10.0f);
    /* Cone angle distribution */
    std::uniform_real_distribution<float> ad(1.0f, 179.0f);

    _cone.origin = Vector3{pd(g), pd(g), pd(g)};
    _cone.normal = Vector3{pd(g), pd(g), pd(g)}.normalized();
    _cone.angle = Deg(ad(g));
    _coneView = coneViewFromCone(_cone.origin, _cone.normal);
    _frustum = Frustum::fromMatrix(_coneView*Matrix4::perspectiveProjection(_cone.angle, 1.0f, 0.001f, 100.0f));

    _boxes.reserve(512);
    _spheres.reserve(512);
    for(int i = 0; i < 512; ++i) {
        Vector3 center{pd(g), pd(g), pd(g)};
        Vector3 extents{pd(g), pd(g), pd(g)};
        _boxes.emplace_back(center - extents, center + extents);
        _spheres.emplace_back(center, extents.length());
    }
}

void IntersectionBenchmark::rangeFrustumNaive() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) for(auto& box: _boxes) {
        b = b ^ Test::rangeFrustumNaive<Float>(box, _frustum);
    }
}

void IntersectionBenchmark::rangeFrustum() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) for(auto& box: _boxes) {
        b = b ^ Intersection::rangeFrustum(box, _frustum);
    }
}

void IntersectionBenchmark::rangeCone() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) {
        const Float tanAngle = Math::tan(_cone.angle);
        const Float tanAngleSqPlusOne = tanAngle*tanAngle + 1.0f;
        for(auto& box: _boxes) {
            b = b ^ Intersection::rangeCone(box, _cone.origin, _cone.normal, tanAngleSqPlusOne);
        }
    }
}

void IntersectionBenchmark::sphereFrustum() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) for(auto& sphere: _spheres) {
        b = b ^ Intersection::sphereFrustum(sphere.xyz(), sphere.w(), _frustum);
    }
}

void IntersectionBenchmark::sphereConeNaive() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) for(auto& sphere: _spheres) {
        b = b ^ sphereConeGT<Float>(sphere.xyz(), sphere.w(), _cone.origin, _cone.normal, _cone.angle);
    }
}

void IntersectionBenchmark::sphereCone() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) {
        const Float sinAngle = Math::sin(_cone.angle);
        const Float tanAngle = Math::tan(_cone.angle);
        const Float tanAngleSqPlusOne = tanAngle*tanAngle + 1.0f;
        for(auto& sphere: _spheres) {
            b = b ^ Intersection::sphereCone(sphere.xyz(), sphere.w(), _cone.origin, _cone.normal, sinAngle, tanAngleSqPlusOne);
        }
    }
}

void IntersectionBenchmark::sphereConeView() {
    volatile bool b = false;
    CORRADE_BENCHMARK(50) {
        const Float sinAngle = Math::sin(_cone.angle);
        const Float tanAngle = Math::tan(_cone.angle);
        for(auto& sphere: _spheres) {
            b = b ^ Intersection::sphereConeView(sphere.xyz(), sphere.w(), _coneView, sinAngle, tanAngle);
        }
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::IntersectionBenchmark)
