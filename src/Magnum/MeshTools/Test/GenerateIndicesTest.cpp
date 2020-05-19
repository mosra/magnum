/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector2.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateIndicesTest: TestSuite::Tester {
    explicit GenerateIndicesTest();

    void primitiveCount();
    void primitiveCountInvalidPrimitive();

    void generateLineStripIndices();
    void generateLineStripIndicesWrongVertexCount();
    void generateLineStripIndicesIntoWrongSize();

    void generateLineLoopIndices();
    void generateLineLoopIndicesWrongVertexCount();
    void generateLineLoopIndicesIntoWrongSize();

    void generateTriangleStripIndices();
    void generateTriangleStripIndicesWrongVertexCount();
    void generateTriangleStripIndicesIntoWrongSize();

    void generateTriangleFanIndices();
    void generateTriangleFanIndicesWrongVertexCount();
    void generateTriangleFanIndicesIntoWrongSize();

    void generateIndicesMeshData();
    void generateIndicesMeshDataMove();
    void generateIndicesMeshDataIndexed();
    void generateIndicesMeshDataInvalidPrimitive();
};

const struct {
    MeshPrimitive primitive;
    Containers::Array<UnsignedInt> indices;
} MeshDataData[] {
    {MeshPrimitive::LineStrip, Containers::array<UnsignedInt>({
        0, 1,
        1, 2,
        2, 3,
        3, 4
    })},
    {MeshPrimitive::LineLoop, Containers::array<UnsignedInt>({
        0, 1,
        1, 2,
        2, 3,
        3, 4,
        4, 0
    })},
    {MeshPrimitive::TriangleStrip, Containers::array<UnsignedInt>({
        0, 1, 2,
        2, 1, 3, /* Reversed */
        2, 3, 4
    })},
    {MeshPrimitive::TriangleFan, Containers::array<UnsignedInt>({
        0, 1, 2,
        0, 2, 3,
        0, 3, 4
    })}
};

GenerateIndicesTest::GenerateIndicesTest() {
    addTests({&GenerateIndicesTest::primitiveCount,
              &GenerateIndicesTest::primitiveCountInvalidPrimitive,

              &GenerateIndicesTest::generateLineStripIndices,
              &GenerateIndicesTest::generateLineStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineStripIndicesIntoWrongSize,

              &GenerateIndicesTest::generateLineLoopIndices,
              &GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount,
              &GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize,

              &GenerateIndicesTest::generateTriangleStripIndices,
              &GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize,

              &GenerateIndicesTest::generateTriangleFanIndices,
              &GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount,
              &GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize});

    addInstancedTests({&GenerateIndicesTest::generateIndicesMeshData},
        Containers::arraySize(MeshDataData));

    addTests({&GenerateIndicesTest::generateIndicesMeshDataMove,
              &GenerateIndicesTest::generateIndicesMeshDataIndexed,
              &GenerateIndicesTest::generateIndicesMeshDataInvalidPrimitive});
}

void GenerateIndicesTest::primitiveCount() {
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Points, 42), 42);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Instances, 13), 13);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Lines, 4), 2);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Lines, 5), 2);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 1), 0);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 2), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineStrip, 4), 3);

    /* This is a degenerate line, which technically still is a primitive */
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineLoop, 1), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::LineLoop, 2), 2);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 2), 0);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::Triangles, 6), 2);

    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 2), 0);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 2), 0);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 3), 1);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleStrip, 7), 5);
    CORRADE_COMPARE(MeshTools::primitiveCount(MeshPrimitive::TriangleFan, 7), 5);
}

void GenerateIndicesTest::primitiveCountInvalidPrimitive() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::primitiveCount(MeshPrimitive(0xdead), 2);
    CORRADE_COMPARE(out.str(),
        "MeshTools::primitiveCount(): invalid primitive MeshPrimitive(0xdead)\n");
}

void GenerateIndicesTest::generateLineStripIndices() {
    /* Minimal input */
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
}

void GenerateIndicesTest::generateLineStripIndicesWrongVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndicesInto(1, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineStripIndicesInto(): expected at least two vertices, got 1\n");
}

void GenerateIndicesTest::generateLineStripIndicesIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt indices[7];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineStripIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineStripIndicesInto(): bad output size, expected 8 but got 7\n");
}

void GenerateIndicesTest::generateLineLoopIndices() {
    /* Minimal input */
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
}

void GenerateIndicesTest::generateLineLoopIndicesWrongVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndicesInto(1, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineLoopIndicesInto(): expected at least two vertices, got 1\n");
}

void GenerateIndicesTest::generateLineLoopIndicesIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt indices[9];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateLineLoopIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateLineLoopIndicesInto(): bad output size, expected 10 but got 9\n");
}

void GenerateIndicesTest::generateTriangleStripIndices() {
    /* Minimal input */
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
}

void GenerateIndicesTest::generateTriangleStripIndicesWrongVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndicesInto(2, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleStripIndicesInto(): expected at least three vertices, got 2\n");
}

void GenerateIndicesTest::generateTriangleStripIndicesIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt indices[8];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleStripIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleStripIndicesInto(): bad output size, expected 9 but got 8\n");
}

void GenerateIndicesTest::generateTriangleFanIndices() {
    /* Minimal input */
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
}

void GenerateIndicesTest::generateTriangleFanIndicesWrongVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndicesInto(2, nullptr);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleFanIndicesInto(): expected at least three vertices, got 2\n");
}

void GenerateIndicesTest::generateTriangleFanIndicesIntoWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt indices[8];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::generateTriangleFanIndicesInto(5, indices);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateTriangleFanIndicesInto(): bad output size, expected 9 but got 8\n");
}

void GenerateIndicesTest::generateIndicesMeshData() {
    auto&& data = MeshDataData[testCaseInstanceId()];
    {
        std::ostringstream out;
        Debug{&out, Debug::Flag::NoNewlineAtTheEnd} << data.primitive;
        setTestCaseDescription(out.str());
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

    Trade::MeshData mesh{data.primitive,
        {}, vertexData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertexData,
                    &vertexData[0].position, 5, sizeof(Vertex))},
            /* Array attribute to verify it's correctly propagated */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                VertexFormat::Short, 2,
                Containers::stridedArrayView(vertexData,
                    &vertexData[0].data, 5, sizeof(Vertex))},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertexData,
                    &vertexData[0].textureCoordinates, 5, sizeof(Vertex))}
        }};

    Trade::MeshData out = generateIndices(mesh);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(out.indices<UnsignedInt>(), data.indices,
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
        MeshPrimitive::TriangleFan, std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertices,
                    &vertices[0].position, 5, sizeof(Vertex))},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertices,
                    &vertices[0].textureCoordinates, 5, sizeof(Vertex))}
        }});
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

void GenerateIndicesTest::generateIndicesMeshDataIndexed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedByte indices[]{0};
    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, indices, Trade::MeshIndexData{indices}, 0};

    /* Test both r-value and l-value overload */
    std::ostringstream out;
    Error redirectError{&out};
    generateIndices(mesh);
    generateIndices(Trade::MeshData{MeshPrimitive::TriangleFan,
        {}, indices, Trade::MeshIndexData{indices}, 0});
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateIndices(): mesh data already indexed\n"
        "MeshTools::generateIndices(): mesh data already indexed\n");
}

void GenerateIndicesTest::generateIndicesMeshDataInvalidPrimitive() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData mesh{MeshPrimitive::Triangles, 2};

    std::ostringstream out;
    Error redirectError{&out};
    generateIndices(mesh);
    CORRADE_COMPARE(out.str(),
        "MeshTools::generateIndices(): invalid primitive MeshPrimitive::Triangles\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateIndicesTest)
