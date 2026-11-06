/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateNormals.h"
#include "Magnum/Primitives/Cylinder.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateNormalsTest: TestSuite::Tester {
    explicit GenerateNormalsTest();

    void flatEmpty();
    void flat();
    void flatWrongCount();
    void flatIntoWrongSize();

    void smoothEmpty();
    template<class T> void smoothTwoTriangles();
    void smoothCube();
    void smoothBeveledCube();
    void smoothCylinder();
    void smoothZeroAreaTriangle();
    void smoothNanPosition();
    void smoothUnusedVertices();
    void smoothWrongCount();
    void smoothOutOfRange();
    void smoothIntoWrongSize();

    template<class T> void smoothErased();
    void smoothErasedNonContiguous();
    void smoothErasedWrongIndexSize();

    void benchmarkFlat();
    void benchmarkSmooth();

    template<class Position, class Normal> void meshDataFlat();
    template<class Index, class Position, class Normal> void meshDataSmooth();
    template<class Position> void meshDataAddNormalAttribute();
    void meshDataWrongPrimitiveIndexVertexCount();
    void meshDataNoPosition();
    void meshDataNot3D();
    void meshDataNotEnoughNormals();
    void meshDataImplementationSpecificAttributeFormat();
    void meshDataFlatIndexed();
    void meshDataSmoothNonIndexed();
    void meshDataSmoothImplementationSpecificIndexType();
    void meshDataRvaluePassthrough();
    void meshDataRvaluePassthroughIndexDataNotOwned();
    void meshDataRvaluePassthroughVertexDataNotOwned();
    void meshDataRvaluePassthroughWrongFormat();
};

const struct {
    const char* name;
    Containers::Array<UnsignedShort> indices;
    Vector3 expected[6];
} SmoothUnusedVerticesData[]{
    {"all vertices used", {InPlaceInit, {
        0, 1, 2, 3, 4, 5
    }}, {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
    }},
    {"one vertex unused", {InPlaceInit, {
        0, 3, 2, 3, 4, 5
    }}, {
        {0.0f, 0.0f, 1.0f},
        {1.0f, 3.0f, 2.0f}, /* unchanged */
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
    }},
    {"two vertices unused", {InPlaceInit, {
        0, 3, 2, 3, 0, 5
    }}, {
        {0.0f, 0.0f, 1.0f},
        {1.0f, 3.0f, 2.0f}, /* unchanged */
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {3.0f, 1.0f, 2.0f}, /* unchanged */
        {0.0f, 0.0f, 1.0f},
    }},
    {"sparse index buffer", {InPlaceInit, {
        4, 1, 2
    }}, {
        {1.0f, 2.0f, 3.0f}, /* unchanged */
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {2.0f, 3.0f, 1.0f}, /* unchanged */
        {0.0f, 0.0f, 1.0f},
        {3.0f, 2.0f, 1.0f}, /* unchanged */
    }},
    {"empty index buffer", {}, {
        {1.0f, 2.0f, 3.0f}, /* unchanged */
        {1.0f, 3.0f, 2.0f}, /* unchanged */
        {2.0f, 1.0f, 3.0f}, /* unchanged */
        {2.0f, 3.0f, 1.0f}, /* unchanged */
        {3.0f, 1.0f, 2.0f}, /* unchanged */
        {3.0f, 2.0f, 1.0f}, /* unchanged */
    }},
};

const struct {
    const char* name;
    UnsignedInt id;
    Int morphTargetId;
} MeshDataFlatData[]{
    {"", 0, -1},
    {"second set", 1, -1},
    {"morph target", 0, 37},
};

const struct {
    const char* name;
    UnsignedInt id;
    Int morphTargetId;
    InterleaveFlags interleaveFlags;
    std::ptrdiff_t expectedIndexStrideMultiplierWithExpandedVertexFormat;
} MeshDataSmoothData[]{
    {"",
        0, -1, {}, 1},
    {"PreserveStridedIndices",
        0, -1, InterleaveFlag::PreserveStridedIndices, 2},
    {"second set",
        1, -1, {}, 1},
    {"morph target",
        0, 37, {}, 1},
};

const struct {
    const char* name;
    bool smooth;
    UnsignedInt id;
    Int morphTargetId;
} MeshDataAddNormalAttributeData[]{
    {"flat", false, 0, -1},
    {"flat, second set", false, 1, -1},
    {"flat, morph target", false, 0, 37},
    {"smooth", true, 0, -1},
    {"smooth, second set", true, 1, -1},
    {"smooth, morph target", true, 0, 37},
};

const struct {
    const char* name;
    bool smooth;
    const char* prefix;
} MeshDataData[]{
    {"flat", false, "generateFlatNormals"},
    {"smooth", true, "generateSmoothNormals"}
};

const struct {
    const char* name;
    bool smooth;
    UnsignedInt id;
    Int morphTargetId;
    const char* prefix;
    const char* suffix;
} MeshDataNoAttributeData[]{
    {"flat",
        false, 1, -1, "generateFlatNormals", ""},
    {"flat, morph target",
        false, 0, 37, "generateFlatNormals", " in morph target 37"},
    {"smooth",
        true, 1, -1, "generateSmoothNormals", ""},
    {"smooth, morph target",
        true, 0, 37, "generateSmoothNormals", " in morph target 37"},
};

const struct {
    const char* name;
    bool smooth;
    UnsignedInt id;
    Int morphTargetId;
} MeshDataRvaluePassthroughData[]{
    {"flat", false, 0, -1},
    {"flat, second set", false, 1, -1},
    {"flat, morph target", false, 0, 37},
    {"smooth", true, 0, -1},
    {"smooth, second set", true, 1, -1},
    {"smooth, morph target", true, 0, 37},
};

const struct {
    const char* name;
    bool smooth;
    VertexFormat positionFormat;
    VertexFormat normalFormat;
} MeshDataRvaluePassthroughWrongFormatData[]{
    {"flat, packed positions",
        false, VertexFormat::Vector3b, VertexFormat::Vector3},
    {"flat, packed normals",
        false, VertexFormat::Vector3, VertexFormat::Vector3s},
    {"flat, no normals",
        false, VertexFormat::Vector3, VertexFormat{}},
    {"smooth, packed positions",
        true, VertexFormat::Vector3b, VertexFormat::Vector3},
    {"smooth, packed normals",
        true, VertexFormat::Vector3, VertexFormat::Vector3s},
    {"smooth, no normals",
        true, VertexFormat::Vector3, VertexFormat{}},
};

GenerateNormalsTest::GenerateNormalsTest() {
    addTests({&GenerateNormalsTest::flatEmpty,
              &GenerateNormalsTest::flat,
              &GenerateNormalsTest::flatWrongCount,
              &GenerateNormalsTest::flatIntoWrongSize,

              &GenerateNormalsTest::smoothEmpty,
              &GenerateNormalsTest::smoothTwoTriangles<UnsignedByte>,
              &GenerateNormalsTest::smoothTwoTriangles<UnsignedShort>,
              &GenerateNormalsTest::smoothTwoTriangles<UnsignedInt>,
              &GenerateNormalsTest::smoothCube,
              &GenerateNormalsTest::smoothBeveledCube,
              &GenerateNormalsTest::smoothCylinder,
              &GenerateNormalsTest::smoothZeroAreaTriangle,
              &GenerateNormalsTest::smoothNanPosition});

    addInstancedTests({&GenerateNormalsTest::smoothUnusedVertices},
        Containers::arraySize(SmoothUnusedVerticesData));

    addTests({&GenerateNormalsTest::smoothWrongCount,
              &GenerateNormalsTest::smoothOutOfRange,
              &GenerateNormalsTest::smoothIntoWrongSize,

              &GenerateNormalsTest::smoothErased<UnsignedByte>,
              &GenerateNormalsTest::smoothErased<UnsignedShort>,
              &GenerateNormalsTest::smoothErased<UnsignedInt>,
              &GenerateNormalsTest::smoothErasedNonContiguous,
              &GenerateNormalsTest::smoothErasedWrongIndexSize});

    addBenchmarks({&GenerateNormalsTest::benchmarkFlat,
                   &GenerateNormalsTest::benchmarkSmooth}, 150);

    addInstancedTests<GenerateNormalsTest>({
        &GenerateNormalsTest::meshDataFlat<Float, Float>,
        &GenerateNormalsTest::meshDataFlat<Float, Half>,
        &GenerateNormalsTest::meshDataFlat<Half, Float>,
        &GenerateNormalsTest::meshDataFlat<Half, Half>
    }, Containers::arraySize(MeshDataFlatData));

    addInstancedTests<GenerateNormalsTest>({
        &GenerateNormalsTest::meshDataSmooth<UnsignedInt, Float, Float>,
        &GenerateNormalsTest::meshDataSmooth<UnsignedShort, Float, Half>,
        &GenerateNormalsTest::meshDataSmooth<UnsignedByte, Float, Half>,
        &GenerateNormalsTest::meshDataSmooth<UnsignedInt, Half, Half>,
        &GenerateNormalsTest::meshDataSmooth<UnsignedShort, Half, Half>
    }, Containers::arraySize(MeshDataSmoothData));

    addInstancedTests<GenerateNormalsTest>({
        &GenerateNormalsTest::meshDataAddNormalAttribute<Float>,
        &GenerateNormalsTest::meshDataAddNormalAttribute<Half>
    }, Containers::arraySize(MeshDataAddNormalAttributeData));

    addTests({&GenerateNormalsTest::meshDataWrongPrimitiveIndexVertexCount});

    addInstancedTests({&GenerateNormalsTest::meshDataNoPosition},
        Containers::arraySize(MeshDataNoAttributeData));

    addInstancedTests({&GenerateNormalsTest::meshDataNot3D},
        Containers::arraySize(MeshDataData));

    addInstancedTests({&GenerateNormalsTest::meshDataNotEnoughNormals},
        Containers::arraySize(MeshDataNoAttributeData));

    addInstancedTests({&GenerateNormalsTest::meshDataImplementationSpecificAttributeFormat},
        Containers::arraySize(MeshDataData));

    addTests({&GenerateNormalsTest::meshDataFlatIndexed,
              &GenerateNormalsTest::meshDataSmoothNonIndexed,
              &GenerateNormalsTest::meshDataSmoothImplementationSpecificIndexType});

    addInstancedTests({&GenerateNormalsTest::meshDataRvaluePassthrough},
        Containers::arraySize(MeshDataRvaluePassthroughData));

    addTests({&GenerateNormalsTest::meshDataRvaluePassthroughIndexDataNotOwned});

    addInstancedTests({&GenerateNormalsTest::meshDataRvaluePassthroughVertexDataNotOwned},
        Containers::arraySize(MeshDataData));

    addInstancedTests({&GenerateNormalsTest::meshDataRvaluePassthroughWrongFormat},
        Containers::arraySize(MeshDataRvaluePassthroughWrongFormatData));
}

/* Two triangles connected by one edge, each wound in another direction */
constexpr Vector3 TwoTriangles[]{
    {-1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},

    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}
};

void GenerateNormalsTest::flatEmpty() {
    /* Verifies that there isn't any logic that would assume at least one
       triangle and crash */
    CORRADE_COMPARE_AS(generateFlatNormals({}),
        Containers::ArrayView<const Vector3>{},
        TestSuite::Compare::Container);
}

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

void GenerateNormalsTest::flatWrongCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    const Vector3 positions[7];
    generateFlatNormals(positions);
    CORRADE_COMPARE(out, "MeshTools::generateFlatNormalsInto(): position count not divisible by 3\n");
}

void GenerateNormalsTest::flatIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    const Vector3 positions[6];
    Vector3 normals[7];
    generateFlatNormalsInto(positions, normals);
    CORRADE_COMPARE(out, "MeshTools::generateFlatNormalsInto(): bad output size, expected 6 but got 7\n");
}

void GenerateNormalsTest::smoothEmpty() {
    /* Verifies that there isn't any logic that would assume at least one
       triangle and crash. See smoothUnusedVertices() for a case where the
       positions are non-empty but indices are. */
    CORRADE_COMPARE_AS(generateSmoothNormals(Containers::ArrayView<const UnsignedShort>{}, {}),
        Containers::ArrayView<const Vector3>{},
        TestSuite::Compare::Container);
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
        Containers::arrayView<Vector3>({
            Vector3::zAxis(),
            Vector3::zAxis(),
            Vector3::zAxis()
        }), TestSuite::Compare::Container);
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

void GenerateNormalsTest::smoothUnusedVertices() {
    auto&& data = SmoothUnusedVerticesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* See smoothEmpty() for a case with both positions and indices empty */

    /* Two triangles connected by an edge, with the vertices either shared or
       duplicated */
    const Vector3 positions[]{
        {-1.0f,  0.0f, 0.0f}, /* 0, duplicated with 4 */
        {+1.0f,  0.0f, 0.0f}, /* 1, duplicated with 3 */
        { 0.0f, +1.0f, 0.0f}, /* 2 */

        {+1.0f,  0.0f, 0.0f}, /* 3, duplicated with 1 */
        {-1.0f,  0.0f, 0.0f}, /* 4, duplicated with 0 */
        { 0.0f, -1.0f, 0.0f}, /* 5 */
    };

    /* If no indices reference given point, the output normal won't change. In
       the worst case with no indices, the normals won't be changed at all. */
    Vector3 normals[]{
        {1.0f, 2.0f, 3.0f},
        {1.0f, 3.0f, 2.0f},
        {2.0f, 1.0f, 3.0f},
        {2.0f, 3.0f, 1.0f},
        {3.0f, 1.0f, 2.0f},
        {3.0f, 2.0f, 1.0f},
    };
    generateSmoothNormalsInto(data.indices, positions, normals);
    CORRADE_COMPARE_AS(Containers::arrayView(normals),
        Containers::arrayView(data.expected),
        TestSuite::Compare::Container);
}

void GenerateNormalsTest::smoothWrongCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    const UnsignedByte indices[7]{};
    const Vector3 positions[1];
    generateSmoothNormals(indices, positions);
    CORRADE_COMPARE(out, "MeshTools::generateSmoothNormalsInto(): index count not divisible by 3\n");
}

void GenerateNormalsTest::smoothOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    const Vector3 positions[2];
    const UnsignedInt indices[] { 0, 1, 2 };
    generateSmoothNormals(indices, positions);
    CORRADE_COMPARE(out, "MeshTools::generateSmoothNormalsInto(): index 2 out of range for 2 elements\n");
}

void GenerateNormalsTest::smoothIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    const UnsignedByte indices[6]{};
    const Vector3 positions[3];
    Vector3 normals[4];
    generateSmoothNormalsInto(indices, positions, normals);
    CORRADE_COMPARE(out, "MeshTools::generateSmoothNormalsInto(): bad output size, expected 3 but got 4\n");
}

void GenerateNormalsTest::benchmarkFlat() {
    Containers::Array<Vector3> positions = duplicate(
        Containers::stridedArrayView(BeveledCubeIndices),
        Containers::stridedArrayView(BeveledCubePositions));

    Containers::Array<Vector3> normals{NoInit, positions.size()};
    CORRADE_BENCHMARK(10) {
        generateFlatNormalsInto(positions, normals);
    }

    CORRADE_COMPARE(Math::min(normals), (Vector3{-1.0f, -1.0f, -1.0f}));
}

void GenerateNormalsTest::benchmarkSmooth() {
    Containers::Array<Vector3> normals{NoInit, Containers::arraySize(BeveledCubePositions)};
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
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indices[6*4]{};
    const Vector3 positions[3];

    Containers::String out;
    Error redirectError{&out};
    generateSmoothNormals(Containers::StridedArrayView2D<const char>{indices, {6, 2}, {4, 2}}, positions);
    CORRADE_COMPARE(out,
        "MeshTools::generateSmoothNormalsInto(): second index view dimension is not contiguous\n");
}

void GenerateNormalsTest::smoothErasedWrongIndexSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indices[6*3]{};
    const Vector3 positions[3];

    Containers::String out;
    Error redirectError{&out};
    generateSmoothNormals(Containers::StridedArrayView2D<const char>{indices, {6, 3}}.every(2), positions);
    CORRADE_COMPARE(out,
        "MeshTools::generateSmoothNormalsInto(): expected index type size 1, 2 or 4 but got 3\n");
}

template<class Position, class Normal> void GenerateNormalsTest::meshDataFlat() {
    auto&& data = MeshDataFlatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName({Math::TypeTraits<Position>::name(),
                             Math::TypeTraits<Normal>::name()});

    /* Like the flat() test case, just with a bunch of templates on top */

    struct Vertex {
        Vector3b secondaryPositionOrNormal;
        Math::Vector3<Position> position;
        Math::Vector3<Normal> normal;
        Float somethingElse;
    } vertices[]{
        {{15, -34, 3}, {Position(-1.0), Position(0.0), Position(0.0)}, {}, 3.0f},
        {{11, -25, 0}, {Position(0.0), Position(-1.0), Position(0.0)}, {}, 4.0f},
        {{16, -27, 2}, {Position(0.0), Position(1.0), Position(0.0)}, {}, 1.0f},

        {{-22, 19, 1}, {Position(0.0), Position(-1.0), Position(0.0)}, {}, 7.0f},
        {{-45, 99, 7}, {Position(0.0), Position(1.0), Position(0.0)}, {}, 8.0f},
        {{-89, 98, 6}, {Position(1.0), Position(0.0), Position(0.0)}, {}, 2.0f}
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::secondaryPositionOrNormal)});
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionOrNormal)});
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::position), data.morphTargetId},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, view.slice(&Vertex::normal), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });

    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, vertices, Utility::move(attributes)};

    Trade::MeshData out = generateFlatNormals(mesh, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!out.isIndexed());

    /* The extra attributes should be preserved without format expansion */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount());
    if(data.id == 1) {
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Position),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Normal),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The positions should be expanded to floats if not already and the
       normals generated, again the same as in the flat() test case above */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId),
        Containers::arrayView(TwoTriangles),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
        -Vector3::zAxis(),
        -Vector3::zAxis(),
        -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}

template<class Index, class Position, class Normal> void GenerateNormalsTest::meshDataSmooth() {
    auto&& data = MeshDataSmoothData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName({Math::TypeTraits<Index>::name(),
                             Math::TypeTraits<Position>::name(),
                             Math::TypeTraits<Normal>::name()});

    /* First triangle with a normal facing backward, second downward. The
       shared edge then being average of the two. Every second index is used to
       test propagation of InterleaveFlags. */
    Index indices[]{
        2, 0, 3, 0, 0, 0,
        3, 0, 2, 0, 1, 0
    };
    struct Vertex {
        Vector3b secondaryPositionOrNormal;
        Math::Vector3<Position> position;
        Math::Vector3<Normal> normal;
        Float somethingElse;
    } vertices[]{
        {{16, -27, 2}, {Position(0.0), Position(1.0), Position(0.0)}, {}, 1.0f},
        {{-89, 98, 6}, {Position(0.0), Position(0.0), Position(-1.0)}, {}, 2.0f},
        {{15, -34, 3}, {Position(-1.0), Position(0.0), Position(0.0)}, {}, 3.0f},
        {{11, -25, 0}, {Position(+1.0), Position(0.0), Position(0.0)}, {}, 4.0f},
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::secondaryPositionOrNormal)});
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionOrNormal)});
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::position), data.morphTargetId},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, view.slice(&Vertex::normal), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{Containers::stridedArrayView(indices).every(2)},
        {}, vertices, Utility::move(attributes)};

    Trade::MeshData out = generateSmoothNormals(mesh, data.id, data.morphTargetId, data.interleaveFlags);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);

    /* The indices should be preserved in the same type */
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), Trade::Implementation::meshIndexTypeFor<Index>());
    /* If no format expansion happens, the index data is passed through
       unchanged, thus keeping the original stride */
    CORRADE_COMPARE(out.indexStride(), (std::is_same<Position, Float>::value && std::is_same<Normal, Float>::value ? 2 : data.expectedIndexStrideMultiplierWithExpandedVertexFormat)*sizeof(Index));
    CORRADE_COMPARE_AS(out.indices<Index>(),
        Containers::stridedArrayView(indices).every(2),
        TestSuite::Compare::Container);

    /* The extra attributes should be preserved without format expansion */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount());
    if(data.id == 1) {
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Position),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Normal),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The positions should be expanded to floats if not already and the
       normals generated */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {+1.0f, 0.0f, 0.0f},
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        Vector3::zAxis(), /* backward */
        -Vector3::yAxis(), /* downward */
        Vector3{0.0f, -1.0f, 1.0f}.normalized(), /* average of the above two */
        Vector3{0.0f, -1.0f, 1.0f}.normalized(),
    }), TestSuite::Compare::Container);
}

template<class Position> void GenerateNormalsTest::meshDataAddNormalAttribute() {
    auto&& data = MeshDataAddNormalAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName({Math::TypeTraits<Position>::name()});

    UnsignedShort indices[]{
        0, 1, 2
    };
    struct Vertex {
        Vector3b secondaryPositionOrNormal;
        Math::Vector3<Position> position;
        Float somethingElse;
    } vertices[]{
        {{15, -34, 3}, {Position(-1.0), Position(0.0), Position(0.0)}, 3.0f},
        {{11, -25, 0}, {Position(+1.0), Position(0.0), Position(0.0)}, 4.0f},
        {{16, -27, 2}, {Position(0.0), Position(1.0), Position(0.0)}, 1.0f},
    };
    Containers::StridedArrayView1D<const Vertex> view = vertices;

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::secondaryPositionOrNormal)});
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, view.slice(&Vertex::secondaryPositionOrNormal)});
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, view.slice(&Vertex::position), data.morphTargetId},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(0), view.slice(&Vertex::somethingElse)}
    });

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, data.smooth ? Containers::arrayView(indices) : nullptr, data.smooth ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        {}, vertices, Utility::move(attributes)};

    Trade::MeshData out = data.smooth ?
        generateSmoothNormals(mesh, data.id, data.morphTargetId) :
        generateFlatNormals(mesh, data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);

    /* The indices should be preserved in the same type */
    CORRADE_COMPARE(out.isIndexed(), data.smooth);
    if(data.smooth) {
        CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }

    /* The extra attributes should be preserved without format expansion, and
       the missing normal attribute added */
    CORRADE_COMPARE(out.attributeCount(), mesh.attributeCount() + 1);
    if(data.id == 1) {
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Position),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Normal),
            view.slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(out.attribute<Float>(Trade::meshAttributeCustom(0)),
        view.slice(&Vertex::somethingElse),
        TestSuite::Compare::Container);

    /* The positions should be expanded to floats if not already and the
       normals generated into a fresh attribute */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        {-1.0f, 0.0f, 0.0f},
        {+1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
    }), TestSuite::Compare::Container);
}

void GenerateNormalsTest::meshDataWrongPrimitiveIndexVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort indices[8];
    Trade::MeshData wrongPrimitive{MeshPrimitive::TriangleStrip, 6};
    Trade::MeshData wrongIndexedPrimitive{MeshPrimitive::TriangleStrip,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr}, 6};
    Trade::MeshData wrongVertexCount{MeshPrimitive::Triangles, 7};
    Trade::MeshData wrongIndexCount{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices}, 6};

    Containers::String out;
    Error redirectError{&out};
    generateFlatNormals(wrongPrimitive);
    generateSmoothNormals(wrongIndexedPrimitive);
    generateFlatNormals(wrongVertexCount);
    generateSmoothNormals(wrongIndexCount);
    CORRADE_COMPARE_AS(out,
        "MeshTools::generateFlatNormals(): expected a MeshPrimitive::Triangles mesh but got MeshPrimitive::TriangleStrip\n"
        "MeshTools::generateSmoothNormals(): expected a MeshPrimitive::Triangles mesh but got MeshPrimitive::TriangleStrip\n"
        "MeshTools::generateFlatNormals(): expected vertex count divisible by 3 but got 7\n"
        "MeshTools::generateSmoothNormals(): expected index count divisible by 3 but got 8\n",
        TestSuite::Compare::String);
}

void GenerateNormalsTest::meshDataNoPosition() {
    auto&& data = MeshDataNoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr}}};

    Containers::String out;
    Error redirectError{&out};
    if(data.smooth)
        generateSmoothNormals(mesh, data.id, data.morphTargetId);
    else
        generateFlatNormals(mesh, data.id, data.morphTargetId);
    CORRADE_COMPARE(out, Utility::format("MeshTools::{}(): the mesh has no positions with index {}{}\n", data.prefix, data.id, data.suffix));
}

void GenerateNormalsTest::meshDataNot3D() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr}}};

    Containers::String out;
    Error redirectError{&out};
    if(data.smooth)
        generateSmoothNormals(mesh);
    else
        generateFlatNormals(mesh);
    CORRADE_COMPARE(out, Utility::format("MeshTools::{}(): expected 3D positions but got VertexFormat::Vector2\n", data.prefix));
}

void GenerateNormalsTest::meshDataNotEnoughNormals() {
    auto&& data = MeshDataNoAttributeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* The trivial case of asking to generate normals for mesh with just a
       position attribute should always work */
    Trade::MeshData singlePositionAttribute{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr}}};
    if(data.smooth)
        generateSmoothNormals(singlePositionAttribute);
    else
        generateFlatNormals(singlePositionAttribute);

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr, 0, data.morphTargetId},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr, 0, 12},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr, 0, data.morphTargetId},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr, 0, data.morphTargetId},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr, 0, data.morphTargetId},
        }};

    Containers::String out;
    Error redirectError{&out};
    if(data.smooth)
        generateSmoothNormals(mesh, 2, data.morphTargetId);
    else
        generateFlatNormals(mesh, 2, data.morphTargetId);
    CORRADE_COMPARE(out, Utility::format("MeshTools::{}(): expected at least 2 normal attributes{} but got 1\n", data.prefix, data.suffix));
}

void GenerateNormalsTest::meshDataImplementationSpecificAttributeFormat() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    /* It should check a coordinate attribute with given ID */
    Trade::MeshData implementationSpecificPosition{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xcaca), nullptr},
        }};
    Trade::MeshData implementationSpecificNormal{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertexFormatWrap(0xbebe), nullptr},
        }};
    /* In case the coordinate vertex format needs expansion it has to assert
       for all other attributes early enough to not cause a confusing assertion
       in interleave() */
    Trade::MeshData needsPositionExpansion{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, vertexFormatWrap(0xdede), nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3s, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
        }};
    Trade::MeshData needsNormalExpansion{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, vertexFormatWrap(0xafaf), nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3sNormalized, nullptr},
        }};
    Trade::MeshData needsNormalAddition{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, vertexFormatWrap(0xebeb), nullptr},
        }};

    /* However if a format doesn't nneed expansion and we also have a normal
       attribute of expected format to write to then implementation-specific
       formats are alright and this will pass with no issues */
    Trade::MeshData implementationSpecificOther{MeshPrimitive::Triangles,
        nullptr, data.smooth ? Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr} : Trade::MeshIndexData{nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3s, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3sNormalized, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, vertexFormatWrap(0xfefe), nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
        }};
    if(data.smooth)
        generateSmoothNormals(implementationSpecificOther, 1);
    else
        generateFlatNormals(implementationSpecificOther, 1);

    Containers::String out;
    Error redirectError{&out};
    if(data.smooth) {
        generateSmoothNormals(implementationSpecificPosition, 1);
        generateSmoothNormals(implementationSpecificNormal, 1);
        generateSmoothNormals(needsPositionExpansion, 1);
        generateSmoothNormals(needsNormalExpansion, 1);
        generateSmoothNormals(needsNormalAddition, 1);
    } else {
        generateFlatNormals(implementationSpecificPosition, 1);
        generateFlatNormals(implementationSpecificNormal, 1);
        generateFlatNormals(needsPositionExpansion, 1);
        generateFlatNormals(needsNormalExpansion, 1);
        generateFlatNormals(needsNormalAddition, 1);
    }
    CORRADE_COMPARE_AS(out, Utility::format(
        "MeshTools::{0}(): positions have an implementation-specific format 0xcaca\n"
        "MeshTools::{0}(): normals have an implementation-specific format 0xbebe\n"
        "MeshTools::{0}(): attribute 2 has an implementation-specific format 0xdede\n"
        "MeshTools::{0}(): attribute 1 has an implementation-specific format 0xafaf\n"
        "MeshTools::{0}(): attribute 3 has an implementation-specific format 0xebeb\n",
        data.prefix), TestSuite::Compare::String);
}

void GenerateNormalsTest::meshDataFlatIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    generateFlatNormals(mesh);
    CORRADE_COMPARE(out, "MeshTools::generateFlatNormals(): expected a non-indexed mesh\n");
}

void GenerateNormalsTest::meshDataSmoothNonIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    generateSmoothNormals(mesh);
    CORRADE_COMPARE(out, "MeshTools::generateSmoothNormals(): expected an indexed mesh\n");
}

void GenerateNormalsTest::meshDataSmoothImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcece), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    generateSmoothNormals(mesh);
    CORRADE_COMPARE(out, "MeshTools::generateSmoothNormals(): mesh has an implementation-specific index type 0xcece\n");
}

void GenerateNormalsTest::meshDataRvaluePassthrough() {
    auto&& data = MeshDataRvaluePassthroughData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Trivial index buffer, the result is the same for both flat and smooth
       normal generation */
    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const UnsignedShort indicesExpected[]{0, 1, 2};
    Utility::copy(indicesExpected, indices);

    /* Same here, we want to have a way to compare against the original */
    struct Vertex {
        Vector3b secondaryPositionOrNormal;
        Vector3 position, normal;
    } verticesExpected[]{
        {{15, -34, 3}, {-1.0f, 0.0f, 0.0f}, {}},
        {{11, -25, 0}, {+1.0f, 0.0f, 0.0f}, {}},
        {{16, -27, 2}, {0.0f, 1.0f, 0.0f}, {}}
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy(verticesExpected, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.id == 1 || data.morphTargetId != -1) {
        arrayAppend(attributes, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::secondaryPositionOrNormal)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, vertices.slice(&Vertex::secondaryPositionOrNormal)},
        });
    } else {
        CORRADE_COMPARE(data.id, 0);
        CORRADE_COMPARE(data.morphTargetId, -1);
    }
    arrayAppend(attributes, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices.slice(&Vertex::normal), data.morphTargetId},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position), data.morphTargetId},
    });
    /* Have to convert the attributes back to a non-growable array with a
       default deleter because that's what copy() internally uses to
       distinguish whether the attributes are owned */
    arrayShrink(attributes, ValueInit);
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        data.smooth ? Utility::move(indexData) : nullptr,
        data.smooth ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};

    Trade::MeshData out = data.smooth ?
        generateSmoothNormals(Utility::move(mesh), data.id, data.morphTargetId) :
        generateFlatNormals(Utility::move(mesh), data.id, data.morphTargetId);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(out.isIndexed(), data.smooth);
    CORRADE_COMPARE(out.attributeCount(), data.id == 1 || data.morphTargetId != -1 ? 4 : 2);

    /* Indices should be passed through unchanged */
    if(data.smooth) {
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indicesExpected),
            TestSuite::Compare::Container);
    }

    /* Positions and secondary attribute as well */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position, data.id, data.morphTargetId),
        Containers::stridedArrayView(verticesExpected).slice(&Vertex::position),
        TestSuite::Compare::Container);
    if(data.id == 1 || data.morphTargetId != -1) {
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Position),
            Containers::stridedArrayView(verticesExpected).slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(out.attribute<Vector3b>(Trade::MeshAttribute::Normal),
            Containers::stridedArrayView(verticesExpected).slice(&Vertex::secondaryPositionOrNormal),
            TestSuite::Compare::Container);
    }

    /* Normals should be generated, same for smooth and flat */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal, data.id, data.morphTargetId), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
    }), TestSuite::Compare::Container);

    /* The memory should point to the original data */
    if(data.smooth)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void GenerateNormalsTest::meshDataRvaluePassthroughIndexDataNotOwned() {
    /* Done only for the smooth normal generation, as flat normals require a
       non-indexed mesh, which is tested above already */

    const UnsignedShort indices[]{
        0, 1, 2
    };

    struct Vertex {
        Vector3 position, normal;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{-1.0f, 0.0f, 0.0f}, {}},
        {{+1.0f, 0.0f, 0.0f}, {}},
        {{0.0f, 1.0f, 0.0f}, {}}
    }, vertices);

    /* Have to use ValueInit + copy to have an array with a default deleter
       because that's what copy() internally uses to distinguish whether the
       attributes are owned */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 2};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position, stridedArrayView(vertices).slice(&Vertex::position)};
    attributes[1] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, stridedArrayView(vertices).slice(&Vertex::normal)};
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)};

    Trade::MeshData out = generateSmoothNormals(Utility::move(mesh));
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.attributeCount(), 2);

    /* Indices and positions should be passed through unchanged */
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {-1.0f, 0.0f, 0.0f},
        {+1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
    }), TestSuite::Compare::Container);

    /* Normals should be generated */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the indices */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void GenerateNormalsTest::meshDataRvaluePassthroughVertexDataNotOwned() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({0, 1, 2}, indices);

    struct Vertex {
        Vector3 position, normal;
    } vertices[]{
        {{-1.0f, 0.0f, 0.0f}, {}},
        {{+1.0f, 0.0f, 0.0f}, {}},
        {{0.0f, 1.0f, 0.0f}, {}}
    };

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        data.smooth ? Utility::move(indexData) : nullptr,
        data.smooth ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::stridedArrayView(vertices).slice(&Vertex::normal)},
        }};

    Trade::MeshData out = data.smooth ?
        generateSmoothNormals(Utility::move(mesh)) :
        generateFlatNormals(Utility::move(mesh));
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(out.isIndexed(), data.smooth);
    CORRADE_COMPARE(out.attributeCount(), 2);

    /* Indices and positions should be passed through unchanged */
    if(data.smooth) {
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {-1.0f, 0.0f, 0.0f},
        {+1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
    }), TestSuite::Compare::Container);

    /* Normals should be generated */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as that's an implementation
       detail in copy(). */
    if(data.smooth)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
}

void GenerateNormalsTest::meshDataRvaluePassthroughWrongFormat() {
    auto&& data = MeshDataRvaluePassthroughWrongFormatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({0, 1, 2}, indices);

    struct Vertex {
        Vector3 position;
        Vector3b positionPacked;
        Vector3 normal;
        Vector3s normalPacked;
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData));
    Utility::copy({
        {{-1.0f, 0.0f, 0.0f},
         {-1, 0, 0},
         {}, {}},
        {{+1.0f, 0.0f, 0.0f},
         {+1, 0, 0},
         {}, {}},
        {{0.0f, 1.0f, 0.0f},
         {0, 1, 0},
         {}, {}},
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes{InPlaceInit, {
        data.positionFormat == VertexFormat::Vector3 ?
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                vertices.slice(&Vertex::position)} :
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                vertices.slice(&Vertex::positionPacked)}
    }};
    if(data.normalFormat != VertexFormat{}) {
        arrayAppend(attributes, data.normalFormat == VertexFormat::Vector3 ?
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices.slice(&Vertex::normal)} :
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3sNormalized, vertices.slice(&Vertex::normalPacked)});
    }
    Trade::MeshData mesh{MeshPrimitive::Triangles,
        data.smooth ? Utility::move(indexData) : nullptr,
        data.smooth ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};

    Trade::MeshData out = data.smooth ?
        generateSmoothNormals(Utility::move(mesh)) :
        generateFlatNormals(Utility::move(mesh));

    /* Indices and positions should be passed through unchanged, positions
       potentially expanded */
    if(data.smooth) {
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(),
            Containers::arrayView(indices),
            TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {-1.0f, 0.0f, 0.0f},
        {+1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
    }), TestSuite::Compare::Container);

    /* Normals should be generated, potentially into an expanded buffer */
    CORRADE_COMPARE_AS(out.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        Vector3::zAxis(),
        Vector3::zAxis(),
        Vector3::zAxis(),
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as those are copied in this
       case always. */
    if(data.smooth)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateNormalsTest)
