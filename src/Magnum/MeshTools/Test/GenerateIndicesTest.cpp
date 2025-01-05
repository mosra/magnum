/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateIndicesTest: TestSuite::Tester {
    explicit GenerateIndicesTest();

    void primitiveCount();
    void primitiveCountInvalidVertexCount();
    void primitiveCountInvalidPrimitive();

    void generateTrivialIndices();

    void generateLineStripIndices();
    template<class T> void generateLineStripIndicesIndexed();
    template<class T> void generateLineStripIndicesIndexed2D();
    void generateLineStripIndicesWrongVertexCount();
    void generateLineStripIndicesIntoWrongSize();
    void generateLineStripIndicesIndexed2DInvalid();

    void generateLineLoopIndices();
    template<class T> void generateLineLoopIndicesIndexed();
    template<class T> void generateLineLoopIndicesIndexed2D();
    void generateLineLoopIndicesWrongVertexCount();
    void generateLineLoopIndicesIntoWrongSize();
    void generateLineLoopIndicesIndexed2DInvalid();

    void generateTriangleStripIndices();
    template<class T> void generateTriangleStripIndicesIndexed();
    template<class T> void generateTriangleStripIndicesIndexed2D();
    void generateTriangleStripIndicesWrongVertexCount();
    void generateTriangleStripIndicesIntoWrongSize();
    void generateTriangleStripIndicesIndexed2DInvalid();

    void generateTriangleFanIndices();
    template<class T> void generateTriangleFanIndicesIndexed();
    template<class T> void generateTriangleFanIndicesIndexed2D();
    void generateTriangleFanIndicesWrongVertexCount();
    void generateTriangleFanIndicesIntoWrongSize();
    void generateTriangleFanIndicesIndexed2DInvalid();

    template<class T> void generateQuadIndices();
    template<class T> void generateQuadIndicesInto();
    void generateQuadIndicesWrongIndexCount();
    void generateQuadIndicesIndexOutOfRange();
    void generateQuadIndicesIntoWrongSize();

    void generateIndicesMeshData();
    template<class T> void generateIndicesMeshDataIndexed();
    void generateIndicesMeshDataEmpty();
    void generateIndicesMeshDataMove();
    void generateIndicesMeshDataNoAttributes();
    void generateIndicesMeshDataTrivial();
    template<class T> void generateIndicesMeshDataTrivialIndexed();
    void generateIndicesMeshDataTrivialIndexedMove();
    void generateIndicesMeshDataTrivialIndexedMoveDifferentIndexType();
    void generateIndicesMeshDataInvalidVertexCount();
    void generateIndicesMeshDataImplementationSpecificIndexType();
};

using namespace Math::Literals;

const struct {
    const char* name;
    UnsignedInt offset;
    Matrix4 transformation;
    UnsignedInt remap[4];
    UnsignedInt expected[6*5];
} QuadData[] {
    {"", 0, {}, {0, 1, 2, 3}, {
        0, 2, 3, 0, 3, 4,           // ABC ACD
        9, 5, 6, 9, 6, 7,           // DAB DBC
        10, 11, 14, 10, 14, 15,     // ABC ACD
        19, 16, 17, 19, 17, 18,     // DAB DBC
        20, 21, 22, 20, 22, 23      // ABC ACD
    }},
    {"rotated indices 1", 0, {}, {1, 2, 3, 0}, {
        2, 3, 4, 2, 4, 0,           // BCD BDA (both splits are fine)
        6, 7, 9, 6, 9, 5,           // BCD BDA
        10, 11, 14, 10, 14, 15,     // ABC ACD
        17, 18, 19, 17, 19, 16,     // BCD BDA
        20, 21, 22, 20, 22, 23      // ABC ACD
    }},
    {"rotated indices 2", 0, {}, {2, 3, 0, 1}, {
        3, 4, 0, 3, 0, 2,           // CDA CAB
        6, 7, 9, 6, 9, 5,           // BCD BDA
        14, 15, 10, 14, 10, 11,     // CDA CAB
        17, 18, 19, 17, 19, 16,     // BCD BDA
        22, 23, 20, 22, 20, 21      // CDA CAB
    }},
    {"rotated indices 3", 0, {}, {3, 0, 1, 2}, {
        4, 0, 2, 4, 2, 3,           // DAB DBC (both splits are fine)
        9, 5, 6, 9, 6, 7,           // DAB DBC
        14, 15, 10, 14, 10, 11,     // CDA CAB
        19, 16, 17, 19, 17, 18,     // DAB DBC
        22, 23, 20, 22, 20, 21      // CDA CAB
    }},
    {"reversed indices", 0, {}, {3, 2, 1, 0}, {
        4, 3, 2, 4, 2, 0,           // DCB DBA (both splits are fine)
        9, 7, 6, 9, 6, 5,           // DCB DBA
        10, 15, 14, 10, 14, 11,     // ADC ACB
        19, 18, 17, 19, 17, 16,     // DCB DBA
        20, 23, 22, 20, 22, 21      // ADC ACB
    }},
    {"rotated positions", 0, Matrix4::rotation(130.0_degf, Vector3{1.0f/Constants::sqrt3()}), {0, 1, 2, 3}, {
        0, 2, 3, 0, 3, 4,           // ABC ACD
        9, 5, 6, 9, 6, 7,           // DAB DBC
        10, 11, 14, 10, 14, 15,     // ABC ACD
        19, 16, 17, 19, 17, 18,     // DAB DBC
        20, 21, 22, 20, 22, 23      // ABC ACD
    }},
    {"mirrored positions", 0, Matrix4::scaling(Vector3::xScale(-1.0f)), {0, 1, 2, 3}, {
        0, 2, 3, 0, 3, 4,           // ABC ACD
        9, 5, 6, 9, 6, 7,           // DAB DBC
        10, 11, 14, 10, 14, 15,     // ABC ACD
        19, 16, 17, 19, 17, 18,     // DAB DBC
        20, 21, 22, 20, 22, 23      // ABC ACD
    }},
    {"additional offset", 100, {}, {0, 1, 2, 3}, {
        100, 102, 103, 100, 103, 104, // ABC ACD
        109, 105, 106, 109, 106, 107, // DAB DBC
        110, 111, 114, 110, 114, 115, // ABC ACD
        119, 116, 117, 119, 117, 118, // DAB DBC
        120, 121, 122, 120, 122, 123  // ABC ACD
    }},
};

const struct {
    MeshPrimitive primitive;
    MeshPrimitive expectedPrimitive;
    Containers::Array<UnsignedInt> expectedIndices;
    Containers::Array<UnsignedInt> expectedIndexedIndices;
} MeshDataData[] {
    {MeshPrimitive::LineStrip, MeshPrimitive::Lines, {InPlaceInit, {
            0, 1,
            1, 2,
            2, 3,
            3, 4
        }}, {InPlaceInit, {
            60, 21,
            21, 72,
            72, 93,
            93, 44
        }}},
    {MeshPrimitive::LineLoop, MeshPrimitive::Lines, {InPlaceInit, {
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 0
        }}, {InPlaceInit, {
            60, 21,
            21, 72,
            72, 93,
            93, 44,
            44, 60
        }}},
    {MeshPrimitive::TriangleStrip, MeshPrimitive::Triangles, {InPlaceInit, {
            0, 1, 2,
            2, 1, 3, /* Reversed */
            2, 3, 4
        }}, {InPlaceInit, {
            60, 21, 72,
            72, 21, 93, /* Reversed */
            72, 93, 44
        }}},
    {MeshPrimitive::TriangleFan, MeshPrimitive::Triangles, {InPlaceInit, {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4
        }}, {InPlaceInit, {
            60, 21, 72,
            60, 72, 93,
            60, 93, 44
        }}}
};

const struct {
    MeshPrimitive primitive;
    UnsignedInt invalidVertexCount, expectedVertexCount;
} MeshDataInvalidVertexCountData[] {
    {MeshPrimitive::LineStrip, 1, 2},
    {MeshPrimitive::LineLoop, 1, 2},
    {MeshPrimitive::TriangleStrip, 2, 3},
    {MeshPrimitive::TriangleFan, 2, 3}
};

GenerateIndicesTest::GenerateIndicesTest() {
    addTests({&GenerateIndicesTest::primitiveCount,
              &GenerateIndicesTest::primitiveCountInvalidVertexCount,
              &GenerateIndicesTest::primitiveCountInvalidPrimitive,

              &GenerateIndicesTest::generateTrivialIndices,

              &GenerateIndicesTest::generateLineStripIndices,
              &GenerateIndicesTest::generateLineStripIndicesIndexed<UnsignedInt>,
              &GenerateIndicesTest::generateLineStripIndicesIndexed<UnsignedShort>,
              &GenerateIndicesTest::generateLineStripIndicesIndexed<UnsignedByte>,
              &GenerateIndicesTest::generateLineStripIndicesIndexed2D<UnsignedInt>,
              &GenerateIndicesTest::generateLineStripIndicesIndexed2D<UnsignedShort>,
              &GenerateIndicesTest::generateLineStripIndicesIndexed2D<UnsignedByte>,
              &GenerateIndicesTest::generateLineStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineStripIndicesIntoWrongSize,
              &GenerateIndicesTest::generateLineStripIndicesIndexed2DInvalid,

              &GenerateIndicesTest::generateLineLoopIndices,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed<UnsignedInt>,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed<UnsignedShort>,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed<UnsignedByte>,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed2D<UnsignedInt>,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed2D<UnsignedShort>,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed2D<UnsignedByte>,
              &GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize,
              &GenerateIndicesTest::generateLineLoopIndicesIndexed2DInvalid,

              &GenerateIndicesTest::generateTriangleStripIndices,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed<UnsignedInt>,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed<UnsignedShort>,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed<UnsignedByte>,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed2D<UnsignedInt>,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed2D<UnsignedShort>,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed2D<UnsignedByte>,
              &GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize,
              &GenerateIndicesTest::generateTriangleStripIndicesIndexed2DInvalid,

              &GenerateIndicesTest::generateTriangleFanIndices,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed<UnsignedInt>,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed<UnsignedShort>,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed<UnsignedByte>,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed2D<UnsignedInt>,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed2D<UnsignedShort>,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed2D<UnsignedByte>,
              &GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize,
              &GenerateIndicesTest::generateTriangleFanIndicesIndexed2DInvalid});

    addInstancedTests<GenerateIndicesTest>({
        &GenerateIndicesTest::generateQuadIndices<UnsignedInt>,
        &GenerateIndicesTest::generateQuadIndices<UnsignedShort>,
        &GenerateIndicesTest::generateQuadIndices<UnsignedByte>},
        Containers::arraySize(QuadData));

    addTests({&GenerateIndicesTest::generateQuadIndicesInto<UnsignedInt>,
              &GenerateIndicesTest::generateQuadIndicesInto<UnsignedShort>,
              &GenerateIndicesTest::generateQuadIndicesInto<UnsignedByte>,
              &GenerateIndicesTest::generateQuadIndicesWrongIndexCount,
              &GenerateIndicesTest::generateQuadIndicesIndexOutOfRange,
              &GenerateIndicesTest::generateQuadIndicesIntoWrongSize});

    addInstancedTests({&GenerateIndicesTest::generateIndicesMeshData,
                       &GenerateIndicesTest::generateIndicesMeshDataIndexed<UnsignedInt>,
                       &GenerateIndicesTest::generateIndicesMeshDataIndexed<UnsignedShort>,
                       &GenerateIndicesTest::generateIndicesMeshDataIndexed<UnsignedByte>,
                       &GenerateIndicesTest::generateIndicesMeshDataEmpty},
        Containers::arraySize(MeshDataData));

    addTests({&GenerateIndicesTest::generateIndicesMeshDataMove,
              &GenerateIndicesTest::generateIndicesMeshDataNoAttributes,
              &GenerateIndicesTest::generateIndicesMeshDataTrivial,
              &GenerateIndicesTest::generateIndicesMeshDataTrivialIndexed<UnsignedInt>,
              &GenerateIndicesTest::generateIndicesMeshDataTrivialIndexed<UnsignedShort>,
              &GenerateIndicesTest::generateIndicesMeshDataTrivialIndexed<UnsignedByte>,
              &GenerateIndicesTest::generateIndicesMeshDataTrivialIndexedMove,
              &GenerateIndicesTest::generateIndicesMeshDataTrivialIndexedMoveDifferentIndexType});

    addInstancedTests({&GenerateIndicesTest::generateIndicesMeshDataInvalidVertexCount},
        Containers::arraySize(MeshDataInvalidVertexCountData));

    addTests({&GenerateIndicesTest::generateIndicesMeshDataImplementationSpecificIndexType});
}

void GenerateIndicesTest::primitiveCount() {
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Points, 42), 42);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Instances, 13), 13);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Lines, 4), 2);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Lines, 14), 7);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 2), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 4), 3);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 10), 9);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineLoop, 2), 2);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineLoop, 9), 9);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 6), 2);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 21), 7);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 7), 5);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 7), 5);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 22), 20);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 22), 20);
}

void GenerateIndicesTest::primitiveCountInvalidVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::primitiveCount(MeshPrimitive::LineLoop, 1);
    MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 1);
    MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 2);
    MeshTools::primitiveCount(MeshPrimitive::Lines, 7);
    MeshTools::primitiveCount(MeshPrimitive::Triangles, 14);
    CORRADE_COMPARE(out,
        "MeshTools::primitiveCount(): expected either zero or at least 2 elements for MeshPrimitive::LineLoop, got 1\n"
        "MeshTools::primitiveCount(): expected either zero or at least 3 elements for MeshPrimitive::TriangleStrip, got 1\n"
        "MeshTools::primitiveCount(): expected either zero or at least 3 elements for MeshPrimitive::TriangleFan, got 2\n"
        "MeshTools::primitiveCount(): expected element count to be divisible by 2 for MeshPrimitive::Lines, got 7\n"
        "MeshTools::primitiveCount(): expected element count to be divisible by 3 for MeshPrimitive::Triangles, got 14\n");
}

void GenerateIndicesTest::primitiveCountInvalidPrimitive() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::primitiveCount(MeshPrimitive(0xdead), 2);
    CORRADE_COMPARE(out,
        "MeshTools::primitiveCount(): invalid primitive MeshPrimitive(0xdead)\n");
}

void GenerateIndicesTest::generateTrivialIndices() {
    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTrivialIndices(0),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Common case */
    CORRADE_COMPARE_AS(MeshTools::generateTrivialIndices(7),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2, 3, 4, 5, 6
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTrivialIndices(7, 100),
        Containers::arrayView<UnsignedInt>({
            100, 101, 102, 103, 104, 105, 106
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateLineStripIndices() {
    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(0),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(2),
        Containers::arrayView<UnsignedInt>({
            0, 1
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(5),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(6),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 5
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(5, 100),
        Containers::arrayView<UnsignedInt>({
            100, 101,
            101, 102,
            102, 103,
            103, 104
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateLineStripIndicesIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* The second digit is 0, 1, 2, 3, 4, 5 for easier ordering. The output in
       the second digit should then be the same as in
       generateLineStripIndices() above. */
    T indexData[]{60, 21, 72, 93, 44, 85};
    auto indices = Containers::arrayView(indexData);

    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices.prefix(2)),
        Containers::arrayView<UnsignedInt>({
            60, 21
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices.prefix(5)),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44,
            44, 85
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021,
            10021, 10072,
            10072, 10093,
            10093, 10044,
            10044, 10085
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateLineStripIndicesIndexed2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Subset of the above, testing the empty and non-empty case with a 2D
       index array */
    T indexData[]{60, 21, 72, 93, 44, 85};
    auto indices = Containers::arrayCast<2, char>(Containers::stridedArrayView(indexData));

    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices.prefix(5)),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineStripIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021,
            10021, 10072,
            10072, 10093,
            10093, 10044,
            10044, 10085
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateLineStripIndicesWrongVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort indices[1];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndices(1);
    MeshTools::generateLineStripIndices(indices);
    CORRADE_COMPARE(out,
        "MeshTools::generateLineStripIndicesInto(): expected either zero or at least two vertices, got 1\n"
        "MeshTools::generateLineStripIndicesInto(): expected either zero or at least two indices, got 1\n");
}

void GenerateIndicesTest::generateLineStripIndicesIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedByte indices[5];
    UnsignedInt output[7];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndicesInto(0, output);
    MeshTools::generateLineStripIndicesInto(Containers::arrayView(indices).prefix(0), output);
    MeshTools::generateLineStripIndicesInto(5, output);
    MeshTools::generateLineStripIndicesInto(indices, output);
    CORRADE_COMPARE(out,
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 0 but got 7\n"
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 0 but got 7\n"
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 8 but got 7\n"
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 8 but got 7\n");
}

void GenerateIndicesTest::generateLineStripIndicesIndexed2DInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[3*4];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndices(Containers::StridedArrayView2D<char>{indices, {3, 4}}.every({1, 2}));
    MeshTools::generateLineStripIndices(Containers::StridedArrayView2D<char>{indices, {4, 3}});
    CORRADE_COMPARE(out,
        "MeshTools::generateLineStripIndicesInto(): second index view dimension is not contiguous\n"
        "MeshTools::generateLineStripIndicesInto(): expected index type size 1, 2 or 4 but got 3\n");
}

void GenerateIndicesTest::generateLineLoopIndices() {
    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(0),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(2),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 0
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(5),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 0
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(6),
        Containers::arrayView<UnsignedInt>({
            0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 5,
            5, 0
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(5, 100),
        Containers::arrayView<UnsignedInt>({
            100, 101,
            101, 102,
            102, 103,
            103, 104,
            104, 100
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateLineLoopIndicesIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* The second digit is 0, 1, 2, 3, 4, 5 for easier ordering. The output in
       the second digit should then be the same as in generateLineLoopIndices()
       above. */
    T indexData[]{60, 21, 72, 93, 44, 85};
    auto indices = Containers::arrayView(indexData);

    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices.prefix(2)),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 60
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices.prefix(5)),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44,
            44, 60
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44,
            44, 85,
            85, 60
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021,
            10021, 10072,
            10072, 10093,
            10093, 10044,
            10044, 10085,
            10085, 10060
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateLineLoopIndicesIndexed2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Subset of the above, testing the empty and non-empty case with a 2D
       index array */
    T indexData[]{60, 21, 72, 93, 44, 85};
    auto indices = Containers::arrayCast<2, char>(Containers::arrayView(indexData));

    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices.prefix(5)),
        Containers::arrayView<UnsignedInt>({
            60, 21,
            21, 72,
            72, 93,
            93, 44,
            44, 60
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateLineLoopIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021,
            10021, 10072,
            10072, 10093,
            10093, 10044,
            10044, 10085,
            10085, 10060
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[1];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndices(1);
    MeshTools::generateLineLoopIndices(indices);
    CORRADE_COMPARE(out,
        "MeshTools::generateLineLoopIndicesInto(): expected either zero or at least two vertices, got 1\n"
        "MeshTools::generateLineLoopIndicesInto(): expected either zero or at least two indices, got 1\n");
}

void GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort indices[5];
    UnsignedInt output[9];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndicesInto(0, output);
    MeshTools::generateLineLoopIndicesInto(Containers::arrayView(indices).prefix(0), output);
    MeshTools::generateLineLoopIndicesInto(5, output);
    MeshTools::generateLineLoopIndicesInto(indices, output);
    CORRADE_COMPARE(out,
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 0 but got 9\n"
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 0 but got 9\n"
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 10 but got 9\n"
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 10 but got 9\n");
}

void GenerateIndicesTest::generateLineLoopIndicesIndexed2DInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[3*4];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndices(Containers::StridedArrayView2D<char>{indices, {3, 4}}.every({1, 2}));
    MeshTools::generateLineLoopIndices(Containers::StridedArrayView2D<char>{indices, {4, 3}});
    CORRADE_COMPARE(out,
        "MeshTools::generateLineLoopIndicesInto(): second index view dimension is not contiguous\n"
        "MeshTools::generateLineLoopIndicesInto(): expected index type size 1, 2 or 4 but got 3\n");
}

void GenerateIndicesTest::generateTriangleStripIndices() {
    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(0),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(3),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(7),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            2, 1, 3, /* Reversed */
            2, 3, 4,
            4, 3, 5, /* Reversed */
            4, 5, 6
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(8),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            2, 1, 3, /* Reversed */
            2, 3, 4,
            4, 3, 5, /* Reversed */
            4, 5, 6,
            6, 5, 7  /* Reversed */
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(7, 100),
        Containers::arrayView<UnsignedInt>({
            100, 101, 102,
            102, 101, 103, /* Reversed */
            102, 103, 104,
            104, 103, 105, /* Reversed */
            104, 105, 106
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateTriangleStripIndicesIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* The second digit is 0, 1, 2, 3, 4, 5, 6, 7 for easier ordering. The
       output in the second digit should then be the same as in
       generateTriangleStripIndices() above. */
    T indexData[]{60, 21, 72, 93, 44, 85, 36, 17};
    auto indices = Containers::arrayView(indexData);

    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(3)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(7)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            72, 21, 93, /* Reversed */
            72, 93, 44,
            44, 93, 85, /* Reversed */
            44, 85, 36
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            72, 21, 93, /* Reversed */
            72, 93, 44,
            44, 93, 85, /* Reversed */
            44, 85, 36,
            36, 85, 17  /* Reversed */
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(7), 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021, 10072,
            10072, 10021, 10093, /* Reversed */
            10072, 10093, 10044,
            10044, 10093, 10085, /* Reversed */
            10044, 10085, 10036
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateTriangleStripIndicesIndexed2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Subset of the above, testing the empty and non-empty case with a 2D
       index array */
    T indexData[]{60, 21, 72, 93, 44, 85, 36, 17};
    auto indices = Containers::arrayCast<2, char>(Containers::arrayView(indexData));

    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices.prefix(7)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            72, 21, 93, /* Reversed */
            72, 93, 44,
            44, 93, 85, /* Reversed */
            44, 85, 36
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleStripIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021, 10072,
            10072, 10021, 10093, /* Reversed */
            10072, 10093, 10044,
            10044, 10093, 10085, /* Reversed */
            10044, 10085, 10036,
            10036, 10085, 10017  /* Reversed */
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedByte indices[2];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndices(2);
    MeshTools::generateTriangleStripIndices(indices);
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleStripIndicesInto(): expected either zero or at least three vertices, got 2\n"
        "MeshTools::generateTriangleStripIndicesInto(): expected either zero or at least three indices, got 2\n");
}

void GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[5];
    UnsignedInt output[8];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndicesInto(0, output);
    MeshTools::generateTriangleStripIndicesInto(Containers::arrayView(indices).prefix(0), output);
    MeshTools::generateTriangleStripIndicesInto(5, output);
    MeshTools::generateTriangleStripIndicesInto(indices, output);
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 0 but got 8\n"
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 0 but got 8\n"
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 9 but got 8\n"
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 9 but got 8\n");
}

void GenerateIndicesTest::generateTriangleStripIndicesIndexed2DInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[3*4];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndices(Containers::StridedArrayView2D<char>{indices, {3, 4}}.every({1, 2}));
    MeshTools::generateTriangleStripIndices(Containers::StridedArrayView2D<char>{indices, {4, 3}});
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleStripIndicesInto(): second index view dimension is not contiguous\n"
        "MeshTools::generateTriangleStripIndicesInto(): expected index type size 1, 2 or 4 but got 3\n");
}

void GenerateIndicesTest::generateTriangleFanIndices() {
    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(0),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(3),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(7),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,
            0, 5, 6
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(8),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 5,
            0, 5, 6,
            0, 6, 7
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(8, 100),
        Containers::arrayView<UnsignedInt>({
            100, 101, 102,
            100, 102, 103,
            100, 103, 104,
            100, 104, 105,
            100, 105, 106,
            100, 106, 107
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateTriangleFanIndicesIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* The second digit is 0, 1, 2, 3, 4, 5, 6, 7 for easier ordering. The
       output in the second digit should then be the same as in
       generateTriangleFanIndices() above. */
    T indexData[]{60, 21, 72, 93, 44, 85, 36, 17};
    auto indices = Containers::arrayView(indexData);

    /* Empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    /* Minimal non-empty input */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(3)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72
        }), TestSuite::Compare::Container);

    /* Odd */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(7)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            60, 72, 93,
            60, 93, 44,
            60, 44, 85,
            60, 85, 36
        }), TestSuite::Compare::Container);

    /* Even */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            60, 72, 93,
            60, 93, 44,
            60, 44, 85,
            60, 85, 36,
            60, 36, 17
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(7), 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021, 10072,
            10060, 10072, 10093,
            10060, 10093, 10044,
            10060, 10044, 10085,
            10060, 10085, 10036
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateTriangleFanIndicesIndexed2D() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Subset of the above, testing the empty and non-empty case with a 2D
       index array */
    T indexData[]{60, 21, 72, 93, 44, 85, 36, 17};
    auto indices = Containers::arrayCast<2, char>(Containers::arrayView(indexData));

    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(0)),
        Containers::ArrayView<const UnsignedInt>{},
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices.prefix(7)),
        Containers::arrayView<UnsignedInt>({
            60, 21, 72,
            60, 72, 93,
            60, 93, 44,
            60, 44, 85,
            60, 85, 36
        }), TestSuite::Compare::Container);

    /* Additional offset */
    CORRADE_COMPARE_AS(MeshTools::generateTriangleFanIndices(indices, 10000),
        Containers::arrayView<UnsignedInt>({
            10060, 10021, 10072,
            10060, 10072, 10093,
            10060, 10093, 10044,
            10060, 10044, 10085,
            10060, 10085, 10036,
            10060, 10036, 10017
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[2];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndices(2);
    MeshTools::generateTriangleFanIndices(indices);
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleFanIndicesInto(): expected either zero or at least three vertices, got 2\n"
        "MeshTools::generateTriangleFanIndicesInto(): expected either zero or at least three indices, got 2\n");
}

void GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[5];
    UnsignedInt output[8];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndicesInto(0, output);
    MeshTools::generateTriangleFanIndicesInto(Containers::arrayView(indices).prefix(0), output);
    MeshTools::generateTriangleFanIndicesInto(5, output);
    MeshTools::generateTriangleFanIndicesInto(indices, output);
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 0 but got 8\n"
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 0 but got 8\n"
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 9 but got 8\n"
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 9 but got 8\n");
}

void GenerateIndicesTest::generateTriangleFanIndicesIndexed2DInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[3*4];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndices(Containers::StridedArrayView2D<char>{indices, {3, 4}}.every({1, 2}));
    MeshTools::generateTriangleFanIndices(Containers::StridedArrayView2D<char>{indices, {4, 3}});
    CORRADE_COMPARE(out,
        "MeshTools::generateTriangleFanIndicesInto(): second index view dimension is not contiguous\n"
        "MeshTools::generateTriangleFanIndicesInto(): expected index type size 1, 2 or 4 but got 3\n");
}

constexpr Vector3 QuadPositions[] {
    /*
        D    C
                        -> ABC ACD (trivial case)
        A    B
    */
    {0.0f, 0.0f, 0.0f}, {},     // A 0
    {1.0f, 0.0f, 0.0f},         // B 2
    {1.0f, 1.0f, 0.0f},         // C 3
    {0.0f, 1.0f, 0.0f},         // D 4

    /*
             D
        A         C     -> DAB DBC (shorter diagonal)
             B
    */
    { 0.0f, 0.0f, 1.0f},        // A 5
    { 5.0f, 0.0f, 0.0f},        // B 6
    {10.0f, 0.0f, 1.0f}, {},    // C 7
    { 5.0f, 0.0f, 2.0f},        // D 9

    /*
                D
        A     C         -> ABC ACD (concave)
                B
    */
    {0.0f, 0.5f, 0.0f},         // A 10
    {5.0f, 0.0f, 0.0f}, {}, {}, // B 11
    {4.0f, 0.5f, 0.0f},         // C 14
    {5.0f, 1.0f, 0.0f},         // D 15

    /*
                C
        D     B         -> DAB DBC (concave, non-planar)
                A
    */
    {5.0f, 0.0f, 0.5f},         // A 16
    /* This used to be 1.0 in Z, which resulted in the dabDbcOppositeDirection
       dot product being very close to zero due to the cross products being
       perpendicular, which caused the test to fail depending on a target
       architecture. */
    {4.0f, 0.5f, 0.9f},         // B 17
    {5.0f, 1.0f, 0.5f},         // C 18
    {0.0f, 0.5f, 1.0f},         // D 19

    /*
                C
        D     B         -> ABC ACD (concave, non-planar, ambiguous -> picking
                A                   shorter diagonal)
    */
    {5.0f, 0.0f, 0.5f},         // A 20
    {4.0f, 0.5f, 2.0f},         // B 21
    {5.0f, 1.0f, 0.5f},         // C 22
    {0.0f, 0.5f, 1.0f},         // D 23
};

constexpr UnsignedInt QuadIndices[] {
    0, 2, 3, 4,
    5, 6, 7, 9,
    10, 11, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23
};

template<class T> void GenerateIndicesTest::generateQuadIndices() {
    auto&& data = QuadData[testCaseInstanceId()];
    setTestCaseTemplateName(Math::TypeTraits<T>::name());
    setTestCaseDescription(data.name);

    Vector3 transformedPositions[Containers::arraySize(QuadPositions)];
    for(std::size_t i = 0; i != Containers::arraySize(QuadPositions); ++i)
        transformedPositions[i] = data.transformation.transformPoint(QuadPositions[i]);

    T remappedIndices[Containers::arraySize(QuadIndices)];
    for(std::size_t i = 0; i != Containers::arraySize(QuadIndices)/4; ++i)
        for(std::size_t j = 0; j != 4; ++j)
            remappedIndices[i*4 + j] = QuadIndices[i*4 + data.remap[j]];

    Containers::Array<UnsignedInt> triangleIndices;
    if(data.offset)
        triangleIndices = MeshTools::generateQuadIndices(transformedPositions, remappedIndices, data.offset);
    else
        triangleIndices = MeshTools::generateQuadIndices(transformedPositions, remappedIndices);

    CORRADE_COMPARE_AS(Containers::arrayView(triangleIndices), Containers::arrayView(data.expected), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateQuadIndicesInto() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Simpler variant of the above w/o data transformations just to verify
       everything is passed through as expected */

    T indices[Containers::arraySize(QuadIndices)];
    for(std::size_t i = 0; i != Containers::arraySize(QuadIndices); ++i)
        indices[i] = QuadIndices[i];

    T triangleIndices[Containers::arraySize(QuadIndices)*6/4];

    MeshTools::generateQuadIndicesInto(QuadPositions, indices, triangleIndices);
    CORRADE_COMPARE_AS(Containers::arrayView(triangleIndices), Containers::arrayView<T>({
        0, 2, 3, 0, 3, 4,           // ABC ACD
        9, 5, 6, 9, 6, 7,           // DAB DBC
        10, 11, 14, 10, 14, 15,     // ABC ACD
        19, 16, 17, 19, 17, 18,     // DAB DBC
        20, 21, 22, 20, 22, 23      // ABC ACD
    }), TestSuite::Compare::Container);

    /* With additional offset */
    MeshTools::generateQuadIndicesInto(QuadPositions, indices, triangleIndices, 100);
    CORRADE_COMPARE_AS(Containers::arrayView(triangleIndices), Containers::arrayView<T>({
        100, 102, 103, 100, 103, 104,     // ABC ACD
        109, 105, 106, 109, 106, 107,     // DAB DBC
        110, 111, 114, 110, 114, 115,     // ABC ACD
        119, 116, 117, 119, 117, 118,     // DAB DBC
        120, 121, 122, 120, 122, 123      // ABC ACD
    }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateQuadIndicesWrongIndexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt quads[13];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateQuadIndices({}, quads);
    CORRADE_COMPARE(out,
        "MeshTools::generateQuadIndicesInto(): quad index count 13 not divisible by 4\n");
}

void GenerateIndicesTest::generateQuadIndicesIndexOutOfRange() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt quads[]{5, 4, 6, 7};
    Vector3 positions[7];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateQuadIndices(positions, quads);
    CORRADE_COMPARE(out,
        "MeshTools::generateQuadIndicesInto(): index 7 out of range for 7 elements\n");
}

void GenerateIndicesTest::generateQuadIndicesIntoWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt quads[12];
    UnsignedInt output[19];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::generateQuadIndicesInto({}, quads, output);
    CORRADE_COMPARE(out,
        "MeshTools::generateQuadIndicesInto(): bad output size, expected 18 but got 19\n");
}

void GenerateIndicesTest::generateIndicesMeshData() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    {
        Containers::String out;
        Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << data.primitive;
        setTestCaseDescription(out);
    }

    const struct Vertex {
        Vector2 position;
        Short data[2];
        Vector2 textureCoordinates;
    } vertexData[] {
        {{1.5f, 0.3f}, {28, -15}, {0.2f, 0.8f}},
        {{2.5f, 1.3f}, {29, -16}, {0.3f, 0.7f}},
        {{3.5f, 2.3f}, {30, -17}, {0.4f, 0.6f}},
        {{4.5f, 3.3f}, {40, -18}, {0.5f, 0.5f}},
        {{5.5f, 4.3f}, {41, -19}, {0.6f, 0.4f}}
    };
    Containers::StridedArrayView1D<const Vertex> vertices = vertexData;

    Trade::MeshData mesh{data.primitive, {}, vertexData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            vertices.slice(&Vertex::position)},
        /* Array attribute to verify it's correctly propagated */
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Short, vertices.slice(&Vertex::data), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            vertices.slice(&Vertex::textureCoordinates)}
    }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_COMPARE(out.primitive(), data.expectedPrimitive);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), data.expectedIndices,
        TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 3);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeName(1), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Short);
    CORRADE_COMPARE(out.attributeArraySize(1), 2);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2s>(out.attribute<Short[]>(1))),
        Containers::arrayView<Vector2s>({
            {28, -15}, {29, -16}, {30, -17}, {40, -18}, {41, -19}
        }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.2f, 0.8f}, {0.3f, 0.7f}, {0.4f, 0.6f}, {0.5f, 0.5f}, {0.6f, 0.4f}
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateIndicesMeshDataIndexed() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    setTestCaseTemplateName(Math::TypeTraits<T>::name());
    {
        Containers::String out;
        Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << data.primitive;
        setTestCaseDescription(out);
    }

    const struct Vertex {
        Vector2 position;
        Short data[2];
        Vector2 textureCoordinates;
    } vertexData[] {
        {{1.5f, 0.3f}, {28, -15}, {0.2f, 0.8f}},
        {{2.5f, 1.3f}, {29, -16}, {0.3f, 0.7f}},
        {{3.5f, 2.3f}, {30, -17}, {0.4f, 0.6f}},
        {{4.5f, 3.3f}, {40, -18}, {0.5f, 0.5f}},
        {{5.5f, 4.3f}, {41, -19}, {0.6f, 0.4f}}
    };
    Containers::StridedArrayView1D<const Vertex> vertices = vertexData;

    const T indexData[]{60, 21, 72, 93, 44};

    Trade::MeshData mesh{data.primitive,
        {}, indexData, Trade::MeshIndexData{indexData},
        {}, vertexData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                vertices.slice(&Vertex::position)},
            /* Array attribute to verify it's correctly propagated */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                VertexFormat::Short, vertices.slice(&Vertex::data), 2},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                vertices.slice(&Vertex::textureCoordinates)}
        }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_COMPARE(out.primitive(), data.expectedPrimitive);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), data.expectedIndexedIndices,
        TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 3);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeName(1), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Short);
    CORRADE_COMPARE(out.attributeArraySize(1), 2);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2s>(out.attribute<Short[]>(1))),
        Containers::arrayView<Vector2s>({
            {28, -15}, {29, -16}, {30, -17}, {40, -18}, {41, -19}
        }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.2f, 0.8f}, {0.3f, 0.7f}, {0.4f, 0.6f}, {0.5f, 0.5f}, {0.6f, 0.4f}
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateIndicesMeshDataEmpty() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    {
        Containers::String out;
        Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << data.primitive;
        setTestCaseDescription(out);
    }

    /* Similar to generateIndicesMeshData(), just with 0 vertices. Verifying it
       doesn't crash anywhere and produces an empty mesh as well. */

    struct Vertex {
        Vector2 position;
        Short data[2];
        Vector2 textureCoordinates;
    };
    Containers::StridedArrayView1D<const Vertex> vertices;

    Trade::MeshData mesh{data.primitive, {}, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            vertices.slice(&Vertex::position)},
        /* Array attribute to verify it's correctly propagated */
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Short,
            vertices.slice(&Vertex::data), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            vertices.slice(&Vertex::textureCoordinates)}
    }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_COMPARE(out.primitive(), data.expectedPrimitive);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexCount(), 0);
    CORRADE_COMPARE(out.attributeCount(), 3);
    CORRADE_COMPARE(out.vertexCount(), 0);
}

void GenerateIndicesTest::generateIndicesMeshDataMove() {
    struct Vertex {
        Vector2 position;
        Vector2 textureCoordinates;
    };

    Containers::Array<char> vertexData{5*sizeof(Vertex)};
    auto vertices = Containers::arrayCast<Vertex>(vertexData);
    vertices[0].position = {1.5f, 0.3f};
    vertices[1].position = {2.5f, 1.3f};
    vertices[2].position = {3.5f, 2.3f};
    vertices[3].position = {4.5f, 3.3f};
    vertices[4].position = {5.5f, 4.3f};
    vertices[0].textureCoordinates = {0.2f, 0.8f};
    vertices[1].textureCoordinates = {0.3f, 0.7f};
    vertices[2].textureCoordinates = {0.4f, 0.6f};
    vertices[3].textureCoordinates = {0.5f, 0.5f};
    vertices[4].textureCoordinates = {0.6f, 0.4f};

    Trade::MeshData out = generateIndices(Trade::MeshData{
        MeshPrimitive::TriangleFan, Utility::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertices,
                    &vertices[0].position, 5, sizeof(Vertex))},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertices,
                    &vertices[0].textureCoordinates, 5, sizeof(Vertex))}
        }});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            0, 2, 3,
            0, 3, 4
        }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 2);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.2f, 0.8f}, {0.3f, 0.7f}, {0.4f, 0.6f}, {0.5f, 0.5f}, {0.6f, 0.4f}
        }), TestSuite::Compare::Container);

    /* The vertex data should be moved, not copied */
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
}

void GenerateIndicesTest::generateIndicesMeshDataNoAttributes() {
    Trade::MeshData out = generateIndices(Trade::MeshData{MeshPrimitive::TriangleStrip, 4});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            2, 1, 3, /* Reversed */
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.vertexCount(), 4);
    CORRADE_COMPARE(out.attributeCount(), 0);
}

void GenerateIndicesTest::generateIndicesMeshDataTrivial() {
    const Vector2 positions[]{
        {1.5f, 0.3f},
        {2.5f, 1.3f},
        {3.5f, 2.3f},
        {4.5f, 3.3f},
        {5.5f, 4.3f},
    };

    /* Should work with just any primitive, not just Lines etc */
    Trade::MeshData mesh{MeshPrimitive::Meshlets, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positions)},
    }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Meshlets);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 2, 3, 4
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);
}

template<class T> void GenerateIndicesTest::generateIndicesMeshDataTrivialIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const Vector2 positions[]{
        {1.5f, 0.3f},
        {2.5f, 1.3f},
        {3.5f, 2.3f},
        {4.5f, 3.3f},
        {5.5f, 4.3f},
    };

    const T indices[]{60, 21, 72, 93};

    /* Should work with just any primitive, not just Lines etc */
    Trade::MeshData mesh{MeshPrimitive::Meshlets,
        {}, indices, Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(positions)},
        }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Meshlets);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        60, 21, 72, 93
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateIndicesMeshDataTrivialIndexedMove() {
    const Vector2 positions[]{
        {1.5f, 0.3f},
        {2.5f, 1.3f},
        {3.5f, 2.3f},
        {4.5f, 3.3f},
        {5.5f, 4.3f},
    };

    Containers::Array<char> indexData{4*sizeof(UnsignedInt)};
    Containers::ArrayView<UnsignedInt> indices = Containers::arrayCast<UnsignedInt>(indexData);
    Utility::copy({60, 21, 72, 93}, indices);

    Trade::MeshData out = generateIndices(Trade::MeshData{meshPrimitiveWrap(0xcaca),
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(positions)},
        }});
    CORRADE_COMPARE(out.primitive(), meshPrimitiveWrap(0xcaca));
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        60, 21, 72, 93
    }), TestSuite::Compare::Container);

    /* The index data should be moved, not copied */
    CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));

    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateIndicesMeshDataTrivialIndexedMoveDifferentIndexType() {
    const Vector2 positions[]{
        {1.5f, 0.3f},
        {2.5f, 1.3f},
        {3.5f, 2.3f},
        {4.5f, 3.3f},
        {5.5f, 4.3f},
    };

    Containers::Array<char> indexData{4*sizeof(UnsignedShort)};
    Containers::ArrayView<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({60, 21, 72, 93}, indices);

    Trade::MeshData out = generateIndices(Trade::MeshData{MeshPrimitive::Points,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(positions)},
        }});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Points);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        60, 21, 72, 93
    }), TestSuite::Compare::Container);

    /* In this case it has to be copied because the type is different */
    CORRADE_VERIFY(out.indexData().data() != static_cast<void*>(indices.data()));

    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE_AS(out.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.5f, 0.3f}, {2.5f, 1.3f}, {3.5f, 2.3f}, {4.5f, 3.3f}, {5.5f, 4.3f}
        }), TestSuite::Compare::Container);
}

void GenerateIndicesTest::generateIndicesMeshDataInvalidVertexCount() {
    auto&& data = MeshDataInvalidVertexCountData[testCaseInstanceId()];
    Containers::String primitiveName;
    Debug{&primitiveName, Debug::Flag::NoNewlineAtTheEnd} << data.primitive;
    setTestCaseDescription(primitiveName);

    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{data.primitive, data.invalidVertexCount};

    Containers::String out;
    Error redirectError{&out};
    generateIndices(mesh);
    CORRADE_COMPARE(out, Utility::format(
        "MeshTools::generateIndices(): expected either zero or at least {} vertices for {}, got {}\n", data.expectedVertexCount, primitiveName, data.invalidVertexCount));
}

void GenerateIndicesTest::generateIndicesMeshDataImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData a{MeshPrimitive::LineLoop,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {}, 3};

    Containers::String out;
    Error redirectError{&out};
    generateIndices(a);
    CORRADE_COMPARE(out, "MeshTools::generateIndices(): mesh has an implementation-specific index type 0xcaca\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateIndicesTest)
