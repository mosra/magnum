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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Concatenate.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct ConcatenateTest: TestSuite::Tester {
    explicit ConcatenateTest();

    void concatenate();
    void concatenateNotIndexed();
    void concatenateNoAttributes();
    void concatenateNoAttributesNotIndexed();
    void concatenateOne();
    void concatenateNone();
    void concatenateInto();
    void concatenateIntoNoIndexArray();
    void concatenateIntoNonOwnedAttributeArray();

    void concatenateUnsupportedPrimitive();
    void concatenateInconsistentPrimitive();
    void concatenateInconsistentAttributeType();
    void concatenateInconsistentAttributeArraySize();
    void concatenateIntoNoMeshes();
};

ConcatenateTest::ConcatenateTest() {
    addTests({&ConcatenateTest::concatenate,
              &ConcatenateTest::concatenateNotIndexed,
              &ConcatenateTest::concatenateNoAttributes,
              &ConcatenateTest::concatenateNoAttributesNotIndexed,
              &ConcatenateTest::concatenateOne,
              &ConcatenateTest::concatenateNone,
              &ConcatenateTest::concatenateInto,
              &ConcatenateTest::concatenateIntoNoIndexArray,
              &ConcatenateTest::concatenateIntoNonOwnedAttributeArray,

              &ConcatenateTest::concatenateUnsupportedPrimitive,
              &ConcatenateTest::concatenateInconsistentPrimitive,
              &ConcatenateTest::concatenateInconsistentAttributeType,
              &ConcatenateTest::concatenateInconsistentAttributeArraySize,
              &ConcatenateTest::concatenateIntoNoMeshes});
}

/* MSVC 2015 doesn't like unnamed bitfields in local structs, so thhis has to
   be outside */
struct VertexDataA {
    Vector2 texcoords1;
    Vector2 texcoords2;
    Int:32;
    Vector3 position;
    Short data[2];
};

void ConcatenateTest::concatenate() {
    using namespace Math::Literals;

    /* First is non-indexed, this layout (including the gap) will be
       preserved */
    const VertexDataA vertexDataA[]{
        {{0.1f, 0.2f}, {0.5f, 0.6f}, {1.0f, 2.0f, 3.0f}, {15, 3}},
        {{0.3f, 0.4f}, {0.7f, 0.8f}, {4.0f, 5.0f, 6.0f}, {14, 2}}
    };
    Trade::MeshData a{MeshPrimitive::Points, {}, vertexDataA, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexDataA,
                &vertexDataA[0].texcoords1, 2, sizeof(VertexDataA))},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexDataA,
                &vertexDataA[0].texcoords2, 2, sizeof(VertexDataA))},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::stridedArrayView(vertexDataA,
                &vertexDataA[0].position, 2, sizeof(VertexDataA))},
        /* Array attribute to verify it's correctly propagated */
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Short,
            Containers::stridedArrayView(vertexDataA,
                &vertexDataA[0].data, 2, sizeof(VertexDataA)), 2}
    }};

    /* Second is indexed, has only one texture coordinate of the two, an extra
       color (which gets ignored) and misses the position (which will be
       zero-filled) */
    const struct VertexDataB {
        Color4 color;
        Short data[2];
        Vector2 texcoords1;
    } vertexDataB[]{
        {0x112233_rgbf, {28, -15}, {0.15f, 0.25f}},
        {0x445566_rgbf, {29, -16}, {0.35f, 0.45f}},
        {0x778899_rgbf, {30, -17}, {0.55f, 0.65f}},
        {0xaabbcc_rgbf, {40, -18}, {0.75f, 0.85f}}
    };
    const UnsignedShort indicesB[]{0, 2, 1, 0, 3, 2};
    Trade::MeshData b{MeshPrimitive::Points,
        {}, indicesB, Trade::MeshIndexData{indicesB}, {}, vertexDataB, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Color,
                Containers::stridedArrayView(vertexDataB,
                    &vertexDataB[0].color, 4, sizeof(VertexDataB))},
            /* Array attribute to verify it's correctly propagated */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                VertexFormat::Short,
                Containers::stridedArrayView(vertexDataB,
                    &vertexDataB[0].data, 4, sizeof(VertexDataB)), 2},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::stridedArrayView(vertexDataB,
                    &vertexDataB[0].texcoords1, 4, sizeof(VertexDataB))}
        }};

    /* Third is again non-indexed, has one texcoord attribute more (which will
       get ignored). Additionally, attribute memory order is inversed and mixed
       together to verify the attributes are picked based on declaration order,
       not memory order. */
    const struct VertexDataC {
        Vector2 texcoords2;
        Vector3 position;
        Vector2 texcoords3;
        Vector2 texcoords1;
    } vertexDataC[]{
        {{0.425f, 0.475f}, {1.5f, 2.5f, 3.5f}, {0.725f, 0.775f}, {0.125f, 0.175f}},
        {{0.525f, 0.575f}, {4.5f, 5.5f, 6.5f}, {0.825f, 0.875f}, {0.225f, 0.275f}},
        {{0.625f, 0.675f}, {7.5f, 8.5f, 9.5f}, {0.925f, 0.975f}, {0.325f, 0.375f}},
    };
    Trade::MeshData c{MeshPrimitive::Points, {}, vertexDataC, {
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexDataC,
                &vertexDataC[0].texcoords1, 3, sizeof(VertexDataC))},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::stridedArrayView(vertexDataC,
                &vertexDataC[0].position, 3, sizeof(VertexDataC))},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexDataC,
                &vertexDataC[0].texcoords2, 3, sizeof(VertexDataC))},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            Containers::stridedArrayView(vertexDataC,
                &vertexDataC[0].texcoords3, 3, sizeof(VertexDataC))},
    }};

    Trade::MeshData dst = MeshTools::concatenate({a, b, c});
    CORRADE_COMPARE(dst.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(dst.attributeCount(), 4);
    CORRADE_COMPARE_AS(dst.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {1.0f, 2.0f, 3.0f},
            {4.0f, 5.0f, 6.0f},
            {}, {}, {}, {}, /* Missing in the second mesh */
            {1.5f, 2.5f, 3.5f},
            {4.5f, 5.5f, 6.5f},
            {7.5f, 8.5f, 9.5f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.1f, 0.2f},
            {0.3f, 0.4f},
            {0.15f, 0.25f},
            {0.35f, 0.45f},
            {0.55f, 0.65f},
            {0.75f, 0.85f},
            {0.125f, 0.175f},
            {0.225f, 0.275f},
            {0.325f, 0.375f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates, 1),
        Containers::arrayView<Vector2>({
            {0.5f, 0.6f},
            {0.7f, 0.8f},
            {}, {}, {}, {}, /* Missing in the second mesh */
            {0.425f, 0.475f},
            {0.525f, 0.575f},
            {0.625f, 0.675f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE(dst.attributeName(3), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(dst.attributeFormat(3), VertexFormat::Short);
    CORRADE_COMPARE(dst.attributeArraySize(3), 2);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2s>(dst.attribute<Short[]>(3))),
        Containers::arrayView<Vector2s>({
            {15, 3}, {14, 2},
            {28, -15}, {29, -16}, {30, -17}, {40, -18},
            {}, {}, {}, /* Missing in the third mesh */
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(dst.isIndexed());
    CORRADE_COMPARE(dst.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(dst.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            0, 1,               /* implicit for the first nonindexed mesh */
            2, 4, 3, 2, 5, 4,   /* offset for the second indexed mesh */
            6, 7, 8             /* implicit + offset for the third mesh */
        }), TestSuite::Compare::Container);

    /* The original interleaved layout should be preserved */
    CORRADE_VERIFY(isInterleaved(dst));
    CORRADE_COMPARE(dst.attributeStride(0), sizeof(VertexDataA));
    CORRADE_COMPARE(dst.attributeOffset(0), 0);
    CORRADE_COMPARE(dst.attributeOffset(1), sizeof(Vector2));
    CORRADE_COMPARE(dst.attributeOffset(2), 2*sizeof(Vector2) + 4);
}

void ConcatenateTest::concatenateNotIndexed() {
    const Vector3 positionA[]{
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f}
    };
    Trade::MeshData a{MeshPrimitive::Points, {}, positionA, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positionA)}
    }};

    const Vector3 positionB[]{
        {1.5f, 2.5f, 3.5f},
        {4.5f, 5.5f, 6.5f},
        {7.5f, 8.5f, 9.5f},
    };
    Trade::MeshData b{MeshPrimitive::Points, {}, positionB, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positionB)}
    }};

    Trade::MeshData dst = MeshTools::concatenate({a, b, b});
    CORRADE_COMPARE(dst.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(dst.attributeCount(), 1);
    CORRADE_COMPARE_AS(dst.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {1.0f, 2.0f, 3.0f},
            {4.0f, 5.0f, 6.0f},
            {1.5f, 2.5f, 3.5f},
            {4.5f, 5.5f, 6.5f},
            {7.5f, 8.5f, 9.5f},
            {1.5f, 2.5f, 3.5f},
            {4.5f, 5.5f, 6.5f},
            {7.5f, 8.5f, 9.5f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(!dst.isIndexed());
}

void ConcatenateTest::concatenateNoAttributes() {
    /* Compared to concatenate(), now the first and last is indexed */
    const UnsignedShort indicesA[]{1, 0};
    Trade::MeshData a{MeshPrimitive::Points, {}, indicesA, Trade::MeshIndexData{indicesA}, 2};

    /* Second is not indexed, just a vertex count */
    Trade::MeshData b{MeshPrimitive::Points, 6};

    const UnsignedByte indicesC[]{1, 0, 1, 0};
    Trade::MeshData c{MeshPrimitive::Points, {}, indicesC, Trade::MeshIndexData{indicesC}, 2};

    Trade::MeshData dst = MeshTools::concatenate({a, b, c});
    CORRADE_COMPARE(dst.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(dst.attributeCount(), 0);
    CORRADE_COMPARE(dst.vertexCount(), 10);
    CORRADE_VERIFY(!dst.vertexData());
    CORRADE_VERIFY(dst.isIndexed());
    CORRADE_COMPARE(dst.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(dst.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            1, 0,
            2, 3, 4, 5, 6, 7,
            9, 8, 9, 8
        }), TestSuite::Compare::Container);
}

void ConcatenateTest::concatenateNoAttributesNotIndexed() {
    Trade::MeshData a{MeshPrimitive::Points, 3};
    Trade::MeshData b{MeshPrimitive::Points, 6};
    Trade::MeshData c{MeshPrimitive::Points, 2};

    Trade::MeshData dst = MeshTools::concatenate({a, b, c});
    CORRADE_COMPARE(dst.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(dst.attributeCount(), 0);
    CORRADE_COMPARE(dst.vertexCount(), 11);
    CORRADE_VERIFY(!dst.vertexData());
    CORRADE_VERIFY(!dst.isIndexed());
}

/* MSVC 2015 doesn't like unnamed bitfields in local structs, so thhis has to
   be outside */
struct VertexDataNonInterleaved {
    Vector2 texcoords1[2];
    Vector2 texcoords2[2];
    Int:32;
    Int:32;
    Vector3 position[2];
};

void ConcatenateTest::concatenateOne() {
    const VertexDataNonInterleaved vertexData[]{{
        {{0.1f, 0.2f},
         {0.3f, 0.4f}},
        {{0.5f, 0.6f},
         {0.7f, 0.8f}},
        {{1.0f, 2.0f, 3.0f},
         {4.0f, 5.0f, 6.0f}}
    }};
    const UnsignedByte indices[]{1, 0, 1};
    Trade::MeshData a{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{indices}, {}, vertexData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::arrayView(vertexData[0].texcoords1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                Containers::arrayView(vertexData[0].texcoords2)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(vertexData[0].position)},
        }};

    /* This is a rather pointless use case, but could happen in generic code
       that filters the input meshes and ends up with just one */
    Trade::MeshData dst = MeshTools::concatenate({a});
    CORRADE_COMPARE(dst.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(dst.attributeCount(), 3);
    CORRADE_COMPARE_AS(dst.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector3>({
            {1.0f, 2.0f, 3.0f},
            {4.0f, 5.0f, 6.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates),
        Containers::arrayView<Vector2>({
            {0.1f, 0.2f},
            {0.3f, 0.4f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates, 1),
        Containers::arrayView<Vector2>({
            {0.5f, 0.6f},
            {0.7f, 0.8f}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(dst.isIndexed());
    CORRADE_COMPARE(dst.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(dst.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            1, 0, 1
        }), TestSuite::Compare::Container);

    /* The mesh should get interleaved (w/o gaps) and owned */
    CORRADE_VERIFY(isInterleaved(dst));
    CORRADE_COMPARE(dst.attributeStride(0), 2*sizeof(Vector2) + sizeof(Vector3));
    CORRADE_COMPARE(dst.indexDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(dst.vertexDataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
}

void ConcatenateTest::concatenateNone() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenate({});
    CORRADE_COMPARE(out.str(), "MeshTools::concatenate(): expected at least one mesh\n");
}

void ConcatenateTest::concatenateInto() {
    Containers::Array<Trade::MeshAttributeData> attributeData{2};
    Containers::Array<char> vertexData;
    Containers::Array<char> indexData;
    arrayResize(vertexData, Containers::DirectInit, (sizeof(Vector2) + sizeof(Vector3))*7, '\xff');
    arrayResize(vertexData, 0);
    arrayResize(indexData, Containers::DirectInit, sizeof(UnsignedInt)*9, '\xff');
    arrayResize(indexData, 0);
    const void* attributeDataPointer = attributeData;
    const void* vertexDataPointer = vertexData;
    const void* indexDataPointer = indexData;

    attributeData[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector2, nullptr};
    attributeData[1] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        VertexFormat::Vector3, nullptr};
    Trade::MeshIndexData indices{MeshIndexType::UnsignedInt, indexData};
    Trade::MeshData dst{MeshPrimitive::Triangles,
        std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData)};

    const Vector2 positionsA[]{
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        {-1.0f,  1.0f},
        { 1.0f,  1.0f}
    };
    const UnsignedShort indicesA[]{
        0, 1, 2, 2, 1, 3
    };
    Trade::MeshData a{MeshPrimitive::Triangles,
        {}, indicesA, Trade::MeshIndexData{indicesA},
        {}, positionsA, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positionsA)}
        }};

    const Vector2 positionsB[]{
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 0.0f,  1.0f}
    };
    Trade::MeshData b{MeshPrimitive::Triangles,
        {}, positionsB, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positionsB)}
        }};

    MeshTools::concatenateInto(dst, {a, b});
    CORRADE_COMPARE(dst.attributeCount(), 2);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {-1.0f, -1.0f},
            { 1.0f, -1.0f},
            {-1.0f,  1.0f},
            { 1.0f,  1.0f},
            {-1.0f, -1.0f},
            { 1.0f, -1.0f},
            { 0.0f,  1.0f}
        }), TestSuite::Compare::Container);
    /* The normal isn't present in any attribute and thus should be zeroed out
       (*not* the whatever garbage present there from before) */
    CORRADE_COMPARE_AS(dst.attribute<Vector3>(Trade::MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            {}, {}, {}, {}, {}, {}, {}
        }), TestSuite::Compare::Container);
    CORRADE_VERIFY(dst.isIndexed());
    CORRADE_COMPARE_AS(dst.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2, 2, 1, 3,
            4, 5, 6
        }), TestSuite::Compare::Container);

    /* Verify that no reallocation happened */
    CORRADE_COMPARE(dst.attributeData().size(), 2);
    CORRADE_COMPARE(dst.attributeData().data(), attributeDataPointer);
    CORRADE_COMPARE(dst.vertexData().size(), 7*(sizeof(Vector2) + sizeof(Vector3)));
    CORRADE_COMPARE(dst.vertexData().data(), vertexDataPointer);
    CORRADE_COMPARE(dst.indexData().size(), 9*sizeof(UnsignedInt));
    CORRADE_COMPARE(dst.indexData().data(), indexDataPointer);
}

void ConcatenateTest::concatenateIntoNoIndexArray() {
    Containers::Array<Trade::MeshAttributeData> attributeData{1};
    Containers::Array<char> vertexData;
    Containers::Array<char> indexData;
    arrayReserve(vertexData, sizeof(Vector2)*3);
    arrayReserve(indexData, sizeof(UnsignedInt));
    const void* attributeDataPointer = attributeData;
    const void* vertexDataPointer = vertexData;

    attributeData[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        VertexFormat::Vector2, nullptr};
    Trade::MeshIndexData indices{MeshIndexType::UnsignedInt, indexData};
    Trade::MeshData dst{MeshPrimitive::Triangles,
        std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData)};
    CORRADE_VERIFY(dst.isIndexed());

    const Vector2 positions[]{
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 0.0f,  1.0f}
    };
    Trade::MeshData a{MeshPrimitive::Triangles,
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positions)}
        }};

    MeshTools::concatenateInto(dst, {a});
    CORRADE_COMPARE(dst.attributeCount(), 1);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 0.0f,  1.0f}
        }), TestSuite::Compare::Container);

    /* The index array gets removed, but no reallocation happens for the other
       two */
    CORRADE_VERIFY(!dst.isIndexed());
    CORRADE_COMPARE(dst.attributeData().size(), 1);
    CORRADE_COMPARE(dst.attributeData().data(), attributeDataPointer);
    CORRADE_COMPARE(dst.vertexData().size(), 3*sizeof(Vector2));
    CORRADE_COMPARE(dst.vertexData().data(), vertexDataPointer);
}

void ConcatenateTest::concatenateIntoNonOwnedAttributeArray() {
    Containers::Array<char> vertexData;
    arrayReserve(vertexData, sizeof(Vector2)*3);
    const void* vertexDataPointer = vertexData;

    const Trade::MeshAttributeData attributeData[]{
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector2, nullptr}
    };
    Trade::MeshData dst{MeshPrimitive::Triangles,
        std::move(vertexData), Trade::meshAttributeDataNonOwningArray(attributeData)};

    const Vector2 positions[]{
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 0.0f,  1.0f}
    };
    Trade::MeshData a{MeshPrimitive::Triangles,
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positions)}
        }};

    MeshTools::concatenateInto(dst, {a});
    CORRADE_COMPARE(dst.attributeCount(), 1);
    CORRADE_COMPARE_AS(dst.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 0.0f,  1.0f}
        }), TestSuite::Compare::Container);

    /* Reallocation happens only for the attribute data as it's not owned */
    CORRADE_VERIFY(!dst.isIndexed());
    CORRADE_COMPARE(dst.attributeData().size(), 1);
    CORRADE_VERIFY(dst.attributeData().data() != attributeData);
    CORRADE_COMPARE(dst.vertexData().size(), 3*sizeof(Vector2));
    CORRADE_COMPARE(dst.vertexData().data(), vertexDataPointer);
}

void ConcatenateTest::concatenateUnsupportedPrimitive() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData a{MeshPrimitive::TriangleStrip, 0};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenate({a});
    MeshTools::concatenateInto(a, {a});
    CORRADE_COMPARE(out.str(),
        "MeshTools::concatenate(): MeshPrimitive::TriangleStrip is not supported, turn it into a plain indexed mesh first\n"
        "MeshTools::concatenateInto(): MeshPrimitive::TriangleStrip is not supported, turn it into a plain indexed mesh first\n");
}

void ConcatenateTest::concatenateInconsistentPrimitive() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Things are a bit duplicated to test correct numbering */
    Trade::MeshData a{MeshPrimitive::Triangles, 0};
    Trade::MeshData b{MeshPrimitive::Lines, 0};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenate({a, a, b});
    MeshTools::concatenateInto(a, {a, b});
    CORRADE_COMPARE(out.str(),
        "MeshTools::concatenate(): expected MeshPrimitive::Triangles but got MeshPrimitive::Lines in mesh 2\n"
        "MeshTools::concatenateInto(): expected MeshPrimitive::Triangles but got MeshPrimitive::Lines in mesh 1\n");
}

void ConcatenateTest::concatenateInconsistentAttributeType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Things are a bit duplicated to test correct numbering */
    Trade::MeshData a{MeshPrimitive::Lines, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector3ubNormalized, nullptr}
    }};
    Trade::MeshData b{MeshPrimitive::Lines, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector3usNormalized, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenate({a, a, a, a, b});
    MeshTools::concatenateInto(a, {a, a, a, b});
    CORRADE_COMPARE(out.str(),
        "MeshTools::concatenate(): expected VertexFormat::Vector3ubNormalized for attribute 2 (Trade::MeshAttribute::Color) but got VertexFormat::Vector3usNormalized in mesh 4 attribute 1\n"
        "MeshTools::concatenateInto(): expected VertexFormat::Vector3ubNormalized for attribute 2 (Trade::MeshAttribute::Color) but got VertexFormat::Vector3usNormalized in mesh 3 attribute 1\n");
}

void ConcatenateTest::concatenateInconsistentAttributeArraySize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Things are a bit duplicated to test correct numbering */
    Trade::MeshData a{MeshPrimitive::Lines, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::ByteNormalized, nullptr, 5}
    }};
    Trade::MeshData b{MeshPrimitive::Lines, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::ByteNormalized, nullptr, 4}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenate({a, a, a, a, b});
    MeshTools::concatenateInto(a, {a, a, a, b});
    CORRADE_COMPARE(out.str(),
        "MeshTools::concatenate(): expected array size 5 for attribute 2 (Trade::MeshAttribute::Custom(42)) but got 4 in mesh 4 attribute 1\n"
        "MeshTools::concatenateInto(): expected array size 5 for attribute 2 (Trade::MeshAttribute::Custom(42)) but got 4 in mesh 3 attribute 1\n");
}

void ConcatenateTest::concatenateIntoNoMeshes() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData destination{MeshPrimitive::Triangles, 0};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::concatenateInto(destination, {});
    CORRADE_COMPARE(out.str(), "MeshTools::concatenateInto(): no meshes passed\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::ConcatenateTest)
