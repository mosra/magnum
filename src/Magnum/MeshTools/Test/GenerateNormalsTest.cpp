/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateNormals.h"
#include "Magnum/Primitives/Cylinder.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateNormalsTest: TestSuite::Tester {
    explicit GenerateNormalsTest();

    void flat();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void flatDeprecated();
    #endif
    void flatWrongCount();
    void flatIntoWrongSize();

    template<class T> void smoothTwoTriangles();
    void smoothCube();
    void smoothBeveledCube();
    void smoothCylinder();
    void smoothZeroAreaTriangle();
    void smoothNanPosition();
    void smoothWrongCount();
    void smoothOutOfBounds();
    void smoothIntoWrongSize();

    template<class T> void smoothErased();
    void smoothErasedNonContiguous();
    void smoothErasedWrongIndexSize();

    void benchmarkFlat();
    void benchmarkSmooth();
};

GenerateNormalsTest::GenerateNormalsTest() {
    addTests({&GenerateNormalsTest::flat,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &GenerateNormalsTest::flatDeprecated,
              #endif
              &GenerateNormalsTest::flatWrongCount,
              &GenerateNormalsTest::flatIntoWrongSize,

              &GenerateNormalsTest::smoothTwoTriangles<UnsignedByte>,
              &GenerateNormalsTest::smoothTwoTriangles<UnsignedShort>,
              &GenerateNormalsTest::smoothTwoTriangles<UnsignedInt>,
              &GenerateNormalsTest::smoothCube,
              &GenerateNormalsTest::smoothBeveledCube,
              &GenerateNormalsTest::smoothCylinder,
              &GenerateNormalsTest::smoothZeroAreaTriangle,
              &GenerateNormalsTest::smoothNanPosition,
              &GenerateNormalsTest::smoothWrongCount,
              &GenerateNormalsTest::smoothOutOfBounds,
              &GenerateNormalsTest::smoothIntoWrongSize,

              &GenerateNormalsTest::smoothErased<UnsignedByte>,
              &GenerateNormalsTest::smoothErased<UnsignedShort>,
              &GenerateNormalsTest::smoothErased<UnsignedInt>,
              &GenerateNormalsTest::smoothErasedNonContiguous,
              &GenerateNormalsTest::smoothErasedWrongIndexSize});

    addBenchmarks({&GenerateNormalsTest::benchmarkFlat,
                   &GenerateNormalsTest::benchmarkSmooth}, 150);
}

/* Two vertices connected by one edge, each wound in another direction */
constexpr Vector3 TwoTriangles[]{
    {-1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},

    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}
};

void GenerateNormalsTest::flat() {
    CORRADE_COMPARE_AS(generateFlatNormals(TwoTriangles),
        Containers::arrayView<Vector3>({
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void GenerateNormalsTest::flatDeprecated() {
    /* Two vertices connected by one edge, each wound in another direction */
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1, 2,
        1, 2, 3
    }, {
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    });
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{
        0, 0, 0,
        1, 1, 1
    }));
    CORRADE_COMPARE(normals, (std::vector<Vector3>{
        Vector3::zAxis(),
        -Vector3::zAxis()
    }));
}
#endif

void GenerateNormalsTest::flatWrongCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    const Vector3 positions[7];
    generateFlatNormals(positions);
    CORRADE_COMPARE(out.str(), "MeshTools::generateFlatNormalsInto(): position count not divisible by 3\n");
}

void GenerateNormalsTest::flatIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    const Vector3 positions[6];
    Vector3 normals[7];
    generateFlatNormalsInto(positions, normals);
    CORRADE_COMPARE(out.str(), "MeshTools::generateFlatNormalsInto(): bad output size, expected 6 but got 7\n");
}

template<class T> void GenerateNormalsTest::smoothTwoTriangles() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{0, 1, 2, 3, 4, 5};

    /* Should generate the same output as flat normals */
    CORRADE_COMPARE_AS(generateSmoothNormals(indices, TwoTriangles),
        Containers::arrayView<Vector3>({
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothCube() {
    const Vector3 positions[] {
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
    };

    const UnsignedByte indices[] {
        0, 1, 2, 0, 2, 3, /* +Z */
        1, 6, 5, 1, 5, 2, /* +X */
        3, 2, 5, 3, 5, 4, /* +Y */
        4, 5, 6, 4, 6, 7, /* -Z */
        3, 4, 7, 3, 7, 0, /* -X */
        7, 6, 1, 7, 1, 0  /* -Y */
    };

    /* Normals should be the same as positions, only normalized */
    CORRADE_COMPARE_AS(generateSmoothNormals(indices, positions),
        Containers::arrayView<Vector3>({
            positions[0]/Constants::sqrt3(),
            positions[1]/Constants::sqrt3(),
            positions[2]/Constants::sqrt3(),
            positions[3]/Constants::sqrt3(),
            positions[4]/Constants::sqrt3(),
            positions[5]/Constants::sqrt3(),
            positions[6]/Constants::sqrt3(),
            positions[7]/Constants::sqrt3()
        }), TestSuite::Compare::Container);
}

constexpr Vector3 BeveledCubePositions[] {
    {-1.0f, -0.6f,  1.1f},
    { 1.0f, -0.6f,  1.1f},
    { 1.0f,  0.6f,  1.1f}, /* +Z */
    {-1.0f,  0.6f,  1.1f},

    { 1.1f, -0.6f,  1.0f},
    { 1.1f, -0.6f, -1.0f},
    { 1.1f,  0.6f, -1.0f}, /* +X */
    { 1.1f,  0.6f,  1.0f},

    {-1.0f,  0.7f,  1.0f},
    { 1.0f,  0.7f,  1.0f},
    { 1.0f,  0.7f, -1.0f}, /* +Y */
    {-1.0f,  0.7f, -1.0f},

    { 1.0f, -0.6f, -1.1f},
    {-1.0f, -0.6f, -1.1f},
    {-1.0f,  0.6f, -1.1f}, /* -Z */
    { 1.0f,  0.6f, -1.1f},

    {-1.0f, -0.7f, -1.0f},
    { 1.0f, -0.7f, -1.0f},
    { 1.0f, -0.7f,  1.0f}, /* -Y */
    {-1.0f, -0.7f,  1.0f},

    {-1.1f, -0.6f, -1.0f},
    {-1.1f, -0.6f,  1.0f},
    {-1.1f,  0.6f,  1.0f}, /* -X */
    {-1.1f,  0.6f, -1.0f}
};

constexpr UnsignedByte BeveledCubeIndices[] {
     0,  1,  2,  0,  2,  3, /* +Z */
     4,  5,  6,  4,  6,  7, /* +X */
     8,  9, 10,  8, 10, 11, /* +Y */
    12, 13, 14, 12, 14, 15, /* -Z */
    16, 17, 18, 16, 18, 19, /* -Y */
    20, 21, 22, 20, 22, 23,  /* -X */

     3,  2,  9,  3,  9,  8, /* +Z / +Y bevel */
     7,  6, 10,  7, 10,  9, /* +X / +Y bevel */
    15, 14, 11, 15, 11, 10, /* -Z / +Y bevel */
    23, 22,  8, 23,  8, 11, /* -X / +Y bevel */

    19, 18,  1, 19,  1,  0, /* -Y / +Z bevel */
    16, 19, 21, 16, 21, 20, /* -Y / -X bevel */
    17, 16, 13, 17, 13, 12, /* -Y / -Z bevel */
    18, 17,  5, 18,  5,  4, /* -Z / +X bevel */

     2,  1,  4,  2,  4,  7, /* +Z / +X bevel */
     6,  5, 12,  6, 12, 15, /* +X / -Z bevel */
    14, 13, 20, 14, 20, 23, /* -Z / -X bevel */
    22, 21,  0, 22,  0,  3, /* -X / +X bevel */

    22,  3,  8, /* -X / +Z / +Y corner */
     2,  7,  9, /* +Z / +X / +Y corner */
     6, 15, 10, /* +X / -Z / +Y corner */
    14, 23, 11, /* -Z / -X / +Y corner */

     0, 21, 19, /* +Z / -X / -Y corner */
    20, 13, 16, /* -X / -Z / -Y corner */
    12,  5, 17, /* -Z / +X / -Y corner */
     4,  1, 18  /* +X / +Z / -Y corner */
};

void GenerateNormalsTest::smoothBeveledCube() {
    /* Data taken from Primitives::cubeSolid() and expanded a bit, with bevel
       faces added */

    /* Normals should be mirrored on the X/Y/Z plane and with a circular
       symmetry around the Y axis, signs corresponding to position signs. */
    Vector3 z{0.0462723f, 0.0754969f, 0.996072f};
    Vector3 x{0.996072f, 0.0754969f, 0.0462723f};
    Vector3 y{0.0467958f, 0.997808f, 0.0467958f};
    CORRADE_COMPARE_AS(generateSmoothNormals(BeveledCubeIndices, BeveledCubePositions),
        Containers::arrayView<Vector3>({
            z*Math::sign(BeveledCubePositions[ 0]),
            z*Math::sign(BeveledCubePositions[ 1]),
            z*Math::sign(BeveledCubePositions[ 2]), /* +Z */
            z*Math::sign(BeveledCubePositions[ 3]),

            x*Math::sign(BeveledCubePositions[ 4]),
            x*Math::sign(BeveledCubePositions[ 5]),
            x*Math::sign(BeveledCubePositions[ 6]), /* +X */
            x*Math::sign(BeveledCubePositions[ 7]),

            y*Math::sign(BeveledCubePositions[ 8]),
            y*Math::sign(BeveledCubePositions[ 9]),
            y*Math::sign(BeveledCubePositions[10]), /* +Y */
            y*Math::sign(BeveledCubePositions[11]),

            z*Math::sign(BeveledCubePositions[12]),
            z*Math::sign(BeveledCubePositions[13]),
            z*Math::sign(BeveledCubePositions[14]), /* -Z */
            z*Math::sign(BeveledCubePositions[15]),

            y*Math::sign(BeveledCubePositions[16]),
            y*Math::sign(BeveledCubePositions[17]),
            y*Math::sign(BeveledCubePositions[18]), /* -Y */
            y*Math::sign(BeveledCubePositions[19]),

            x*Math::sign(BeveledCubePositions[20]),
            x*Math::sign(BeveledCubePositions[21]),
            x*Math::sign(BeveledCubePositions[22]), /* -X */
            x*Math::sign(BeveledCubePositions[23])
        }), TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothCylinder() {
    const Trade::MeshData data = Primitives::cylinderSolid(1, 5, 1.0f);

    /* Output should be exactly the same as the cylinder normals */
    CORRADE_COMPARE_AS(Containers::arrayView(generateSmoothNormals(
        data.indices(),
        data.attribute<Vector3>(Trade::MeshAttribute::Position))),
        data.attribute<Vector3>(Trade::MeshAttribute::Normal),
        TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothZeroAreaTriangle() {
    constexpr Vector3 positions[] {
        {-1.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
    };

    /* Second triangle is just an edge, so it shouldn't contribute to the first
       triangle normal */
    constexpr UnsignedInt indices[] {
        0, 1, 2, 1, 2, 1
    };

    CORRADE_COMPARE_AS(generateSmoothNormals(indices, positions),
        (Containers::Array<Vector3>{Containers::InPlaceInit, {
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis()
        }}), TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothNanPosition() {
    constexpr Vector3 positions[] {
        {-1.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 0.0f, Constants::nan(), 0.0f},
    };

    /* Second triangle will poison a part of the first with NaNs, but it won't
       crash */
    constexpr UnsignedInt indices[] {
        0, 1, 2, 1, 2, 3
    };

    Containers::Array<Vector3> generated = generateSmoothNormals(indices, positions);
    CORRADE_COMPARE(generated[0], Vector3::zAxis());
    CORRADE_VERIFY(Math::isNan(generated[1]).all());
    CORRADE_VERIFY(Math::isNan(generated[2]).all());
    CORRADE_VERIFY(Math::isNan(generated[3]).all());
}

void GenerateNormalsTest::smoothWrongCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    const UnsignedByte indices[7]{};
    const Vector3 positions[1];
    generateSmoothNormals(indices, positions);
    CORRADE_COMPARE(out.str(), "MeshTools::generateSmoothNormalsInto(): index count not divisible by 3\n");
}

void GenerateNormalsTest::smoothOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    const Vector3 positions[2];
    const UnsignedInt indices[] { 0, 1, 2 };
    generateSmoothNormals(indices, positions);
    CORRADE_COMPARE(out.str(), "MeshTools::generateSmoothNormalsInto(): index 2 out of bounds for 2 elements\n");
}

void GenerateNormalsTest::smoothIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    const UnsignedByte indices[6]{};
    const Vector3 positions[3];
    Vector3 normals[4];
    generateSmoothNormalsInto(indices, positions, normals);
    CORRADE_COMPARE(out.str(), "MeshTools::generateSmoothNormalsInto(): bad output size, expected 3 but got 4\n");
}

void GenerateNormalsTest::benchmarkFlat() {
    Containers::Array<Vector3> positions = duplicate(
        Containers::stridedArrayView(BeveledCubeIndices),
        Containers::stridedArrayView(BeveledCubePositions));

    Containers::Array<Vector3> normals{Containers::NoInit, positions.size()};
    CORRADE_BENCHMARK(10) {
        generateFlatNormalsInto(positions, normals);
    }

    CORRADE_COMPARE(Math::min(normals), (Vector3{-1.0f, -1.0f, -1.0f}));
}

void GenerateNormalsTest::benchmarkSmooth() {
    Containers::Array<Vector3> normals{Containers::NoInit, Containers::arraySize(BeveledCubePositions)};
    CORRADE_BENCHMARK(10) {
        generateSmoothNormalsInto(BeveledCubeIndices, BeveledCubePositions, normals);
    }

    CORRADE_COMPARE(Math::min(normals), (Vector3{-0.996072f, -0.997808f, -0.996072f}));
}

template<class T> void GenerateNormalsTest::smoothErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{0, 1, 2, 3, 4, 5};

    /* Should generate the same output as flat normals */
    CORRADE_COMPARE_AS(generateSmoothNormals(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)), TwoTriangles),
        Containers::arrayView<Vector3>({
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis(),
            -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothErasedNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char indices[6*4]{};
    const Vector3 positions[3];

    std::stringstream out;
    Error redirectError{&out};
    generateSmoothNormals(Containers::StridedArrayView2D<const char>{indices, {6, 2}, {4, 2}}, positions);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateSmoothNormalsInto(): second index view dimension is not contiguous\n");
}

void GenerateNormalsTest::smoothErasedWrongIndexSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char indices[6*3]{};
    const Vector3 positions[3];

    std::stringstream out;
    Error redirectError{&out};
    generateSmoothNormals(Containers::StridedArrayView2D<const char>{indices, {6, 3}}.every(2), positions);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateSmoothNormalsInto(): expected index type size 1, 2 or 4 but got 3\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateNormalsTest)
