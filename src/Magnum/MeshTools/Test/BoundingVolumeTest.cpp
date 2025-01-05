/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/BoundingVolume.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Transform.h"
#include "Magnum/Primitives/Capsule.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct BoundingVolumeTest: TestSuite::Tester {
    explicit BoundingVolumeTest();

    void range();
    void rangeNaN();

    void sphereBouncingBubble();
    void sphereBouncingBubbleNaN();

    void benchmarkRange();
    void benchmarkSphereBouncingBubble();
};

BoundingVolumeTest::BoundingVolumeTest() {
    addTests({&BoundingVolumeTest::range,
              &BoundingVolumeTest::rangeNaN,
              &BoundingVolumeTest::sphereBouncingBubble,
              &BoundingVolumeTest::sphereBouncingBubbleNaN});

    addBenchmarks({&BoundingVolumeTest::benchmarkRange,
                   &BoundingVolumeTest::benchmarkSphereBouncingBubble}, 150);
}

void BoundingVolumeTest::range() {
    /* boundingboxAxisAligned() is just a wrapper around minmax() so only test
       that the input and output are forwarded correctly */
    constexpr Float cylinderLength = 7.0f;
    const Trade::MeshData cylinderMesh = Primitives::capsule3DSolid(3, 1, 12, cylinderLength*0.5f);
    const Range3D box = boundingRange(
        cylinderMesh.attribute<Vector3>(Trade::MeshAttribute::Position));

    CORRADE_COMPARE(box.center(), Vector3{});
    CORRADE_COMPARE(box.size(), (Vector3{2.0f, cylinderLength + 2.0f, 2.0f}));
}

void BoundingVolumeTest::rangeNaN() {
    /* NaNs are skipped (unless it's all NaNs), matching minmax() behaviour */
    {
        const Range3D box = boundingRange(Containers::stridedArrayView({
                Vector3{Constants::nan()},
                Vector3{1.0f, 1.0f, 1.0f},
                Vector3{Constants::nan()},
                Vector3{2.0f, 2.0f, 2.0f},
                Vector3{Constants::nan()}
            }));
        CORRADE_COMPARE(box.min(), (Vector3{1.0f, 1.0f, 1.0f}));
        CORRADE_COMPARE(box.max(), (Vector3{2.0f, 2.0f, 2.0f}));
    } {
        const Range3D box = boundingRange(Containers::stridedArrayView({
                Vector3{Constants::nan()},
                Vector3{Constants::nan()}
            }));
        CORRADE_VERIFY(Math::isNan(box.min()));
        CORRADE_VERIFY(Math::isNan(box.max()));
    }
}

void BoundingVolumeTest::sphereBouncingBubble() {
    /* Empty positions -- produces radius epsilon for consistency with all
       all identical positions */
    {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::StridedArrayView1D<const Vector3>{});
        CORRADE_COMPARE(sphere.first(), (Vector3{}));
        CORRADE_COMPARE(sphere.second(), Math::TypeTraits<Float>::epsilon());

    /* Identical positions -- radius is always >= epsilon due to specifics of
       the algorithm */
    } {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{1.0f, 2.0f, 3.0f}
            }));
        CORRADE_COMPARE(sphere.first(), (Vector3{1.0f, 2.0f, 3.0f}));
        CORRADE_COMPARE(sphere.second(), Math::TypeTraits<Float>::epsilon());
    } {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{3.0f, 1.0f, 2.0f},
                Vector3{3.0f, 1.0f, 2.0f},
                Vector3{3.0f, 1.0f, 2.0f}
            }));
        CORRADE_COMPARE(sphere.first(), (Vector3{3.0f, 1.0f, 2.0f}));
        CORRADE_COMPARE(sphere.second(), Math::TypeTraits<Float>::epsilon());

    /* Simple cases */
    } {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{1.0f, 1.0f, 1.0f},
                Vector3{2.0f, 2.0f, 2.0f}
            }));
        CORRADE_COMPARE(sphere.first(), (Vector3{1.5f}));
        CORRADE_COMPARE(sphere.second(), Vector3{0.5f}.length());
    } {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{2.0f, 0.0f, 0.0f},
                Vector3{-2.0f, 0.0f, 0.0f},
                Vector3{0.0f, 2.0f, 0.0f},
                Vector3{0.0f, -2.0f, 0.0f}
            }));
        CORRADE_COMPARE(sphere.first(), (Vector3{0.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(sphere.second(), 2.0f);

    /* Icosphere -- original/translated and scaled */
    } {
        const Trade::MeshData sphereMesh = Primitives::icosphereSolid(1);
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(sphereMesh.attribute<Vector3>(Trade::MeshAttribute::Position));
        /* No error */
        CORRADE_COMPARE(sphere.first(), (Vector3{0.0f, 0.0f, 0.0f}));
        CORRADE_COMPARE(sphere.second(), 1.0f);
    } {
        Trade::MeshData sphereMesh = Primitives::icosphereSolid(1);
        constexpr Vector3 translation{1.0f, 2.0f, 3.0f};
        constexpr Vector3 scale{0.5f, 1.2f, 2.8f};
        transform3DInPlace(sphereMesh, Matrix4::translation(translation)*Matrix4::scaling(scale));
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(sphereMesh.attribute<Vector3>(Trade::MeshAttribute::Position));
        /* Noticeable error */
        constexpr Float Delta = 0.04f;
        CORRADE_COMPARE_WITH(sphere.first(), translation, TestSuite::Compare::around(Vector3{Delta}));
        /* Radius should never be smaller than the ground truth */
        CORRADE_COMPARE_WITH(sphere.second(), scale.max() + Delta, TestSuite::Compare::around(Delta));

    /* Cube -- translated and scaled */
    } {
        Trade::MeshData cubeMesh = copy(Primitives::cubeSolid());
        constexpr Vector3 translation{1.0f, 2.0f, 3.0f};
        constexpr Float scale = 13.2f;
        transform3DInPlace(cubeMesh, Matrix4::translation(translation)*Matrix4::scaling(Vector3{scale}));
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(cubeMesh.attribute<Vector3>(Trade::MeshAttribute::Position));
        /* Noticeable error */
        constexpr Float Delta = 0.04f;
        CORRADE_COMPARE_WITH(sphere.first(), translation, TestSuite::Compare::around(Vector3{Delta}));
        CORRADE_COMPARE_WITH(sphere.second(), Constants::sqrt3()*scale + Delta,
            TestSuite::Compare::around(Delta));
    }

    /* Radius is rotationally invariant */
    using namespace Math::Literals;

    for(Deg degrees = 0.0_degf; degrees < 360.0_degf; degrees += 60.0_degf) {
        CORRADE_ITERATION(degrees);
        Trade::MeshData cubeMesh = copy(Primitives::cubeSolid());
        constexpr Vector3 translation{1.0f, 2.0f, 3.0f};
        transform3DInPlace(cubeMesh, Matrix4::rotationY(degrees)*Matrix4::translation(translation));
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(cubeMesh.attribute<Vector3>(Trade::MeshAttribute::Position));
        CORRADE_COMPARE(sphere.second(), Constants::sqrt3());
    }
}

void BoundingVolumeTest::sphereBouncingBubbleNaN() {
    /* NaN is ignored except for the first position element */
    {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{1.0f, 1.0f, 1.0f},
                Vector3{Constants::nan()},
                Vector3{2.0f, 2.0f, 2.0f},
                Vector3{Constants::nan()}
            }));
        CORRADE_COMPARE(sphere.first(), (Vector3{1.5f}));
        CORRADE_COMPARE(sphere.second(), Vector3{0.5f}.length());

    } {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(Containers::stridedArrayView({
                Vector3{Constants::nan()},
                Vector3{1.0f, 1.0f, 1.0f},
                Vector3{2.0f, 2.0f, 2.0f}
            }));
        {
            CORRADE_EXPECT_FAIL("NaN in the first position is not ignored.");
            CORRADE_COMPARE(sphere.first(), (Vector3{1.5f}));
            CORRADE_COMPARE(sphere.second(), Vector3{0.5f}.length());
        }
        CORRADE_VERIFY(Math::isNan(sphere.first()));
        CORRADE_COMPARE(sphere.second(), Math::TypeTraits<Float>::epsilon());
    }
}

void BoundingVolumeTest::benchmarkRange() {
    Containers::Array<Vector3> points{NoInit, 500};
    for(size_t i = 0; i < points.size(); ++i) {
        points[i] = Vector3{Float(i)*0.01f};
    }

    Float r = 0.0f;
    CORRADE_BENCHMARK(50) {
        const Range3D box = boundingRange(points);
        r += box.size().x();
    }

    CORRADE_COMPARE_AS(r, 1.0f, TestSuite::Compare::Greater);
}

void BoundingVolumeTest::benchmarkSphereBouncingBubble() {
    Containers::Array<Vector3> points{NoInit, 500};
    for(size_t i = 0; i < points.size(); ++i) {
        points[i] = Vector3{Float(i)*0.01f};
    }

    Float r = 0.0f;
    CORRADE_BENCHMARK(50) {
        const Containers::Pair<Vector3, Float> sphere =
            boundingSphereBouncingBubble(points);
        r += sphere.second();
    }

    CORRADE_COMPARE_AS(r, 1.0f, TestSuite::Compare::Greater);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::BoundingVolumeTest)
