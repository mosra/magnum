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

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Combine.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CombineTest: TestSuite::Tester {
    explicit CombineTest();

    void indexedAttributes();
    void indexedAttributesIndicesOnly();
    void indexedAttributesSingleMesh();

    void indexedAttributesNoMeshes();
    void indexedAttributesNotIndexed();
    void indexedAttributesDifferentPrimitive();
    void indexedAttributesDifferentIndexCount();
    void indexedAttributesImplementationSpecificIndexType();
    void indexedAttributesImplementationSpecificVertexFormat();

    void faceAttributes();
    void faceAttributesMeshNotIndexed();
    void faceAttributesUnexpectedPrimitive();
    void faceAttributesUnexpectedFaceCount();
    void faceAttributesFacesNotInterleaved();
    void faceAttributesFaceAttributeOffsetOnly();
    void faceAttributesImplementationSpecificIndexType();
    void faceAttributesImplementationSpecificVertexFormat();
};

constexpr struct {
    const char* name;
    bool indexed;
} CombineFaceAttributesData[] {
    {"", false},
    {"indexed faces", true}
};

CombineTest::CombineTest() {
    addTests({&CombineTest::indexedAttributes,
              &CombineTest::indexedAttributesIndicesOnly,
              &CombineTest::indexedAttributesSingleMesh,

              &CombineTest::indexedAttributesNoMeshes,
              &CombineTest::indexedAttributesNotIndexed,
              &CombineTest::indexedAttributesDifferentPrimitive,
              &CombineTest::indexedAttributesDifferentIndexCount,
              &CombineTest::indexedAttributesImplementationSpecificIndexType,
              &CombineTest::indexedAttributesImplementationSpecificVertexFormat});

    addInstancedTests({&CombineTest::faceAttributes},
        Containers::arraySize(CombineFaceAttributesData));

    addTests({&CombineTest::faceAttributesMeshNotIndexed,
              &CombineTest::faceAttributesUnexpectedPrimitive,
              &CombineTest::faceAttributesUnexpectedFaceCount,
              &CombineTest::faceAttributesFacesNotInterleaved,
              &CombineTest::faceAttributesFaceAttributeOffsetOnly,
              &CombineTest::faceAttributesImplementationSpecificIndexType,
              &CombineTest::faceAttributesImplementationSpecificVertexFormat});
}

void CombineTest::indexedAttributes() {
    const UnsignedInt indicesA[]{2, 1, 2, 0};
    const Int dataA[]{2, 1, 0};
    const UnsignedByte indicesB[]{3, 4, 3, 2};
    const Byte dataB[][2]{{4, 1}, {3, 2}, {2, 3}, {1, 4}, {0, 5}};
    const UnsignedShort indicesC[]{7, 6, 7, 5};
    const Float dataC[]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    Trade::MeshData a{MeshPrimitive::LineLoop,
        {}, indicesA, Trade::MeshIndexData{indicesA},
        {}, dataA, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(2), Containers::arrayView(dataA)}}};
    Trade::MeshData b{MeshPrimitive::LineLoop,
        {}, indicesB, Trade::MeshIndexData{indicesB},
        {}, dataB, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(17), VertexFormat::Byte,
            /* Array attribute to verify it's correctly propagated */
            Containers::arrayView(dataB), 2}}};
    Trade::MeshData c{MeshPrimitive::LineLoop,
        {}, indicesC, Trade::MeshIndexData{indicesC},
        {}, dataC, {Trade::MeshAttributeData{
            /* Offset-only to verify these aren't causing any issues
               internally */
            Trade::meshAttributeCustom(22), VertexFormat::Float, 0, 8, sizeof(Float)}}};

    Trade::MeshData result = combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(result.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(result.isIndexed());
    CORRADE_COMPARE(result.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(result.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 0, 2}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(result.attributeCount(), 3);
    CORRADE_COMPARE(result.attributeName(0), Trade::meshAttributeCustom(2));
    CORRADE_COMPARE(result.attributeFormat(0), VertexFormat::Int);
    CORRADE_COMPARE_AS(result.attribute<Int>(0),
        Containers::arrayView<Int>({0, 1, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeName(1), Trade::meshAttributeCustom(17));
    CORRADE_COMPARE(result.attributeFormat(1), VertexFormat::Byte);
    CORRADE_COMPARE(result.attributeArraySize(1), 2);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2b>(result.attribute<Byte[]>(1))),
        Containers::arrayView<Vector2b>({{1, 4}, {0, 5}, {2, 3}}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeName(2), Trade::meshAttributeCustom(22));
    CORRADE_COMPARE(result.attributeFormat(2), VertexFormat::Float);
    CORRADE_COMPARE_AS(result.attribute<Float>(2),
        Containers::arrayView<Float>({7.0f, 6.0f, 5.0f}),
        TestSuite::Compare::Container);
}

void CombineTest::indexedAttributesIndicesOnly() {
    const UnsignedInt indicesA[]{2, 1, 2};
    const UnsignedShort indicesB[]{3, 4, 3};
    const UnsignedByte indicesC[]{7, 6, 7};
    Trade::MeshData a{MeshPrimitive::LineLoop, {}, indicesA,
        Trade::MeshIndexData{indicesA}, 3};
    Trade::MeshData b{MeshPrimitive::LineLoop, {}, indicesB,
        Trade::MeshIndexData{indicesB}, 5};
    Trade::MeshData c{MeshPrimitive::LineLoop, {}, indicesC,
        Trade::MeshIndexData{indicesC}, 8};

    Trade::MeshData result = combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(result.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(result.isIndexed());
    CORRADE_COMPARE(result.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(result.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeCount(), 0);
    CORRADE_COMPARE(result.vertexCount(), 2);
}

void CombineTest::indexedAttributesSingleMesh() {
    const UnsignedInt indices[]{2, 1, 2, 0, 5, 7};
    const Float data[]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    Trade::MeshData a{MeshPrimitive::LineLoop,
        {}, indices, Trade::MeshIndexData{indices},
        {}, data, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(22), Containers::arrayView(data)}}};

    Trade::MeshData result = combineIndexedAttributes({a});
    CORRADE_COMPARE(result.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(result.isIndexed());
    CORRADE_COMPARE(result.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(result.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 0, 2, 3, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeCount(), 1);
    CORRADE_COMPARE(result.attributeFormat(0), VertexFormat::Float);
    CORRADE_COMPARE_AS(result.attribute<Float>(0),
        Containers::arrayView<Float>({2.0f, 1.0f, 0.0f, 5.0f, 7.0f}),
        TestSuite::Compare::Container);
}

void CombineTest::indexedAttributesNoMeshes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): no meshes passed\n");
}

void CombineTest::indexedAttributesNotIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    Trade::MeshData b{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    Trade::MeshData c{MeshPrimitive::Lines, 1};

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): data 2 is not indexed\n");
}

void CombineTest::indexedAttributesDifferentPrimitive() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    Trade::MeshData b{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{indices}, 1};

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({a, b});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): data 1 is MeshPrimitive::Points but expected MeshPrimitive::Lines\n");
}

void CombineTest::indexedAttributesDifferentIndexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    Trade::MeshData b{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    Trade::MeshData c{MeshPrimitive::Lines,
        {}, indices,
        Trade::MeshIndexData{Containers::arrayView(indices).prefix(4)}, 1};

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): data 2 has 4 indices but expected 5\n");
}

void CombineTest::indexedAttributesImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData a{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};
    Trade::MeshData b{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({a, b});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): data 1 has an implementation-specific index type 0xcaca\n");
}

void CombineTest::indexedAttributesImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData a{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};
    Trade::MeshData b{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(3), vertexFormatWrap(0xcaca), nullptr}
        }};

    Containers::String out;
    Error redirectError{&out};
    combineIndexedAttributes({a, b});
    CORRADE_COMPARE(out, "MeshTools::combineIndexedAttributes(): attribute 2 of mesh 1 has an implementation-specific format 0xcaca\n");
}

void CombineTest::faceAttributes() {
    auto&& data = CombineFaceAttributesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    using namespace Math::Literals;

    /*
                                9 ------- 8
        5 ------- 4              \       / 6
         \       / \              \  C  / / \
          \  C  /   \              \   / /   \
           \   /  B  \              \ / /  B  \
            \ /       \              7 /       \
             1 ------- 3    ==>       3 ------- 5
            / \       /              2 \       /
           /   \  B  /              / \ \  B  /
          /  A  \   /              /   \ \   /
         /       \ /              /  A  \ \ /
        0 ------- 2              /       \ 4
                                0 ------- 1
    */
    const UnsignedShort indices[]{
        0, 2, 1,
        1, 2, 3,
        1, 3, 4,
        1, 4, 5
    };
    const Vector2 positions[] {
        {0.0f, 0.0f},
        {0.5f, 1.0f},
        {1.0f, 0.0f},
        {1.5f, 1.0f},
        {1.0f, 2.0f},
        {0.0f, 2.0f}
    };

    const struct FaceData {
        Color3 color;
        Byte id;
    } faceData[] {
        {0xaaaaaa_rgbf, 'A'},
        {0xbbbbbb_rgbf, 'B'},
        {0xbbbbbb_rgbf, 'B'},
        {0xcccccc_rgbf, 'C'}
    };

    const UnsignedByte faceIndices[] { 0, 1, 1, 2 };
    const FaceData faceDataIndexed[] {
        {0xaaaaaa_rgbf, 'A'},
        {0xbbbbbb_rgbf, 'B'},
        {0xcccccc_rgbf, 'C'}
    };

    const Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(positions)}
        }};
    const Trade::MeshData faceAttributesIndexed{MeshPrimitive::Faces,
        {}, faceIndices, Trade::MeshIndexData{faceIndices},
        {}, faceDataIndexed, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Color,
                Containers::StridedArrayView1D<const Color3>{faceDataIndexed,
                    &faceDataIndexed[0].color, 3, sizeof(FaceData)}},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(25),
                Containers::StridedArrayView1D<const Byte>{faceDataIndexed,
                    &faceDataIndexed[0].id, 3, sizeof(FaceData)}},
        }};

    /* For a non-indexed variant test the overload that takes the attributes
       directly, not wrapped in a MeshData */
    Containers::Optional<Trade::MeshData> combined;
    if(data.indexed)
        combined = combineFaceAttributes(mesh, faceAttributesIndexed);
    else
        combined = combineFaceAttributes(mesh, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Color,
                Containers::StridedArrayView1D<const Color3>{faceData,
                    &faceData[0].color, 4, sizeof(FaceData)}},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(25),
                Containers::StridedArrayView1D<const Byte>{faceData,
                    &faceData[0].id, 4, sizeof(FaceData)}}
        });

    CORRADE_COMPARE(combined->attributeCount(), 3);
    CORRADE_COMPARE(combined->indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(combined->indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({
            0, 1, 2,
            3, 4, 5,
            3, 5, 6,
            7, 8, 9
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(combined->attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.5f, 1.0f},
            {0.5f, 1.0f},
            {1.0f, 0.0f},
            {1.5f, 1.0f},
            {1.0f, 2.0f},
            {0.5f, 1.0f},
            {1.0f, 2.0f},
            {0.0f, 2.0f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(combined->attribute<Color3>(Trade::MeshAttribute::Color),
        Containers::arrayView<Color3>({
            0xaaaaaa_rgbf, 0xaaaaaa_rgbf, 0xaaaaaa_rgbf,
            0xbbbbbb_rgbf, 0xbbbbbb_rgbf, 0xbbbbbb_rgbf, 0xbbbbbb_rgbf,
            0xcccccc_rgbf, 0xcccccc_rgbf, 0xcccccc_rgbf
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(combined->attribute<Byte>(Trade::meshAttributeCustom(25)),
        Containers::arrayView<Byte>({
            'A', 'A', 'A',
            'B', 'B', 'B', 'B',
            'C', 'C', 'C'
        }), TestSuite::Compare::Container);
}

void CombineTest::faceAttributesMeshNotIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const Trade::MeshData mesh{MeshPrimitive::Triangles, 3};
    const Trade::MeshData faceAttributes{MeshPrimitive::Faces, 0};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(mesh, faceAttributes);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): vertex mesh is not indexed\n");
}

void CombineTest::faceAttributesUnexpectedPrimitive() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedInt indices[] { 0, 0, 0 };
    const Trade::MeshData a{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    const Trade::MeshData b{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    const Trade::MeshData faceA{MeshPrimitive::Instances, 0};
    const Trade::MeshData faceB{MeshPrimitive::Faces, 0};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(a, faceA);
    combineFaceAttributes(b, faceB);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): expected a MeshPrimitive::Triangles mesh and a MeshPrimitive::Faces mesh but got MeshPrimitive::Triangles and MeshPrimitive::Instances\n"
        "MeshTools::combineFaceAttributes(): expected a MeshPrimitive::Triangles mesh and a MeshPrimitive::Faces mesh but got MeshPrimitive::Lines and MeshPrimitive::Faces\n");
}

void CombineTest::faceAttributesUnexpectedFaceCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedInt indices[] { 0, 0, 0 };
    const Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    const Trade::MeshData faceAttributes{MeshPrimitive::Faces, 2};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(mesh, faceAttributes);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): expected 1 face entries for 3 indices but got 2\n");
}

void CombineTest::faceAttributesImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData triangles{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};
    Trade::MeshData trianglesImplementationSpecific{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};

    Trade::MeshData faces{MeshPrimitive::Faces,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        }};
    Trade::MeshData facesImplementationSpecific{MeshPrimitive::Faces,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(triangles, facesImplementationSpecific);
    combineFaceAttributes(trianglesImplementationSpecific, faces);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): face mesh has an implementation-specific index type 0xcaca\n"
        "MeshTools::combineFaceAttributes(): vertex mesh has an implementation-specific index type 0xcaca\n");
}

void CombineTest::faceAttributesImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData triangles{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};
    Trade::MeshData trianglesImplementationSpecific{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Color, VertexFormat::Vector4, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertexFormatWrap(0xcaca), nullptr}
        }};

    Trade::MeshData faces{MeshPrimitive::Faces,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr},
        }};
    Trade::MeshData facesImplementationSpecific{MeshPrimitive::Faces,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedShort, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Color, VertexFormat::Vector4, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertexFormatWrap(0xcaca), nullptr}
        }};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(triangles, facesImplementationSpecific);
    combineFaceAttributes(trianglesImplementationSpecific, faces);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): attribute 2 of mesh 1 has an implementation-specific format 0xcaca\n"
        "MeshTools::combineFaceAttributes(): attribute 2 of mesh 0 has an implementation-specific format 0xcaca\n");
}

void CombineTest::faceAttributesFacesNotInterleaved() {
    CORRADE_SKIP_IF_NO_ASSERT();

    using namespace Math::Literals;

    const UnsignedInt indices[] { 0, 0, 0, 0, 0, 0 };
    const Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices}, 1};
    const struct {
        Color3 color[2];
        Byte id[2];
    } faceData[]{{
        {0xaaaaaa_rgbf, 0xbbbbbb_rgbf},
        {'A', 'B'}
    }};
    const Trade::MeshData faceAttributes{MeshPrimitive::Faces,
        {}, faceData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Color,
                Containers::arrayView(faceData[0].color)},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(25),
                Containers::arrayView(faceData[0].id)}
        }};

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(mesh, faceAttributes);
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): face attributes are not interleaved\n");
}

void CombineTest::faceAttributesFaceAttributeOffsetOnly() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    combineFaceAttributes(Trade::MeshData{MeshPrimitive::Triangles, 0}, {
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId,
            Containers::ArrayView<UnsignedInt>{}},
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector4, 0, 5, 16}
    });
    CORRADE_COMPARE(out,
        "MeshTools::combineFaceAttributes(): face attribute 1 is offset-only\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CombineTest)
