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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CopyTest: TestSuite::Tester {
    explicit CopyTest();

    void copy();
    void copyNoIndexData();
    void copyNoAttributeVertexData();
    void copyStridedIndices();

    void copyRvalueNotOwned();
    void copyRvalueIndicesVerticesAttributesOwned();
    void copyRvalueAttributesOwned();

    void reference();
    void referenceNoIndexData();
    void referenceImplementationSpecificIndexType();
    void referenceNoIndexVertexAttributeData();

    void mutableReference();
    void mutableReferenceNoIndexData();
    void mutableReferenceImplementationSpecificIndexType();
    void mutableReferenceNoIndexVertexAttributeData();
    void mutableReferenceNotMutable();
};

struct {
    const char* name;
    MeshIndexType type;
} StridedIndicesData[]{
    {"", MeshIndexType::UnsignedShort},
    {"implementation-specific index format", meshIndexTypeWrap(0xcaca)}
};

CopyTest::CopyTest() {
    addTests({&CopyTest::copy,
              &CopyTest::copyNoIndexData,
              &CopyTest::copyNoAttributeVertexData});

    addInstancedTests({&CopyTest::copyStridedIndices},
        Containers::arraySize(StridedIndicesData));

    addTests({&CopyTest::copyRvalueNotOwned,
              &CopyTest::copyRvalueIndicesVerticesAttributesOwned,
              &CopyTest::copyRvalueAttributesOwned,

              &CopyTest::reference,
              &CopyTest::referenceNoIndexData,
              &CopyTest::referenceImplementationSpecificIndexType,
              &CopyTest::referenceNoIndexVertexAttributeData,

              &CopyTest::mutableReference,
              &CopyTest::mutableReferenceNoIndexData,
              &CopyTest::mutableReferenceImplementationSpecificIndexType,
              &CopyTest::mutableReferenceNoIndexVertexAttributeData,
              &CopyTest::mutableReferenceNotMutable});
}

void CopyTest::copy() {
    const struct Vertex {
        Vector3 position;
        Short array[3];
        Vector2ub textureCoordinates;
        bool bit;
    } vertices[]{
        {{1.0f, 2.0f, 3.0f}, {15, -36, 0}, {4, 5}, false},
        {{6.0f, 7.0f, 8.0f}, {0, -36, 12}, {9, 0}, true}
    };
    const UnsignedShort indices[]{
        /* First is not used */
        2, 1, 0, 1
    };

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{Containers::arrayView(indices).exceptPrefix(1)},
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            /* Array attribute */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(37),
                VertexFormat::Short,
                Containers::stridedArrayView(vertices).slice(&Vertex::array), 3},
            /* Offset-only attribute */
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
                VertexFormat::Vector2ub,
                offsetof(Vertex, textureCoordinates), 2, sizeof(Vertex)},
            /* Implementation-specific vertex format */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(56),
                vertexFormatWrap(0xb001),
                Containers::stridedArrayView(vertices).slice(&Vertex::bit)},
        }};

    Trade::MeshData copy = MeshTools::copy(mesh);
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), mesh.primitive());
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);

    CORRADE_COMPARE(copy.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(copy.indices<UnsignedShort>(),
        Containers::arrayView(indices).exceptPrefix(1),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(copy.vertexCount(), 2);
    CORRADE_COMPARE(copy.attributeCount(), 4);

    for(std::size_t i = 0; i != mesh.attributeCount(); ++i) {
        CORRADE_ITERATION(i);

        CORRADE_COMPARE(copy.attributeName(i), mesh.attributeName(i));
        CORRADE_COMPARE(copy.attributeFormat(i), mesh.attributeFormat(i));
        CORRADE_COMPARE(copy.attributeArraySize(i), mesh.attributeArraySize(i));
    }

    /* Offset-only attributes should be just passed through during the copy,
       not made absolute */
    CORRADE_VERIFY(copy.attributeData()[2].isOffsetOnly());

    CORRADE_COMPARE_AS(copy.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::stridedArrayView(vertices).slice(&Vertex::position),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.attribute<Short[]>(Trade::meshAttributeCustom(37))[0],
        Containers::arrayView<Short>({15, -36, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.attribute<Short[]>(Trade::meshAttributeCustom(37))[1],
        Containers::arrayView<Short>({0, -36, 12}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.attribute<Vector2ub>(Trade::MeshAttribute::TextureCoordinates),
        Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<const bool>(copy.attribute(Trade::meshAttributeCustom(56))).transposed<0, 1>()[0]),
        Containers::stridedArrayView(vertices).slice(&Vertex::bit),
        TestSuite::Compare::Container);

    /* The data layout should be the same and thus the raw data should match
       as well */
    CORRADE_COMPARE_AS(copy.indexData(),
        mesh.indexData(),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.vertexData(),
        mesh.vertexData(),
        TestSuite::Compare::Container);

    /* The data should have a default deleter to make this usable in plugins */
    Containers::Array<char> indexData = copy.releaseIndexData();
    Containers::Array<char> vertexData = copy.releaseVertexData();
    Containers::Array<Trade::MeshAttributeData> attributeData = copy.releaseAttributeData();
    CORRADE_VERIFY(!indexData.deleter());
    CORRADE_VERIFY(!vertexData.deleter());
    CORRADE_VERIFY(!attributeData.deleter());
}

void CopyTest::copyNoIndexData() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();
    CORRADE_VERIFY(!cube.isIndexed());
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlag::Global);

    Trade::MeshData copy = MeshTools::copy(cube);
    CORRADE_VERIFY(!copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), cube.primitive());
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexCount(), cube.vertexCount());
    CORRADE_COMPARE(copy.attributeCount(), cube.attributeCount());
}

void CopyTest::copyNoAttributeVertexData() {
    UnsignedShort indexData[]{0, 41, 2};
    Trade::MeshData indexedFourtytwo{MeshPrimitive::Edges,
        {}, indexData, Trade::MeshIndexData{indexData},
        42};

    Trade::MeshData copy = MeshTools::copy(indexedFourtytwo);
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), MeshPrimitive::Edges);
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.indexCount(), 3);
    CORRADE_COMPARE(copy.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(copy.indexOffset(), 0);
    CORRADE_COMPARE(copy.vertexCount(), 42);
    CORRADE_COMPARE(copy.attributeCount(), 0);

    CORRADE_COMPARE_AS(copy.indexData(), indexedFourtytwo.indexData(), TestSuite::Compare::Container);
    CORRADE_VERIFY(!static_cast<const void*>(copy.vertexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(copy.attributeData().data()));
}

void CopyTest::copyStridedIndices() {
    auto&& data = StridedIndicesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{data.type, Containers::stridedArrayView(indices).exceptPrefix(1).every(2)},
        16};

    /* The full index data layout including whatever format should be
       preserved */
    Trade::MeshData copy = MeshTools::copy(stuff);
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.indexCount(), 3);
    CORRADE_COMPARE(copy.indexType(), data.type);
    CORRADE_COMPARE(copy.indexOffset(), 2);
    CORRADE_COMPARE(copy.indexStride(), 4);
    CORRADE_COMPARE(copy.vertexCount(), 16);
    CORRADE_COMPARE(copy.attributeCount(), 0);

    /* Has to do a prefix() because for an implementation-specific index type
       the returned size is equal to stride */
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(copy.indices().prefix({copy.indexCount(), 2}))),
        Containers::arrayView<UnsignedShort>({3, 7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.indexData(), stuff.indexData(),
        TestSuite::Compare::Container);
}

void CopyTest::copyRvalueNotOwned() {
    Vector3 positions[]{
        {1.0f, 2.0f, 3.0f},
        {6.0f, 7.0f, 8.0f},
    };
    const UnsignedShort indices[]{
        1, 0, 1
    };
    const Trade::MeshAttributeData attributes[]{
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            Containers::arrayView(positions)},
    };

    Trade::MeshData copy = MeshTools::copy(Trade::MeshData{MeshPrimitive::Triangles,
        Trade::DataFlag::ExternallyOwned, indices, Trade::MeshIndexData{indices},
        Trade::DataFlag::Mutable, positions, Trade::meshAttributeDataNonOwningArray(attributes)});
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);

    CORRADE_COMPARE(copy.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(copy.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(copy.vertexCount(), 2);
    CORRADE_COMPARE(copy.attributeCount(), 1);
    CORRADE_COMPARE_AS(copy.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView(positions),
        TestSuite::Compare::Container);

    /* Nothing should be copied in this case */
    CORRADE_VERIFY(copy.indexData().data() != static_cast<const void*>(indices));
    CORRADE_VERIFY(copy.vertexData().data() != static_cast<void*>(positions));
    CORRADE_VERIFY(copy.attributeData().data() != attributes);
}

void CopyTest::copyRvalueIndicesVerticesAttributesOwned() {
    Containers::Array<char> vertexData{NoInit, sizeof(Vector3)*2};
    auto positions = Containers::arrayCast<Vector3>(vertexData);
    Utility::copy({
        {1.0f, 2.0f, 3.0f},
        {6.0f, 7.0f, 8.0f},
    }, positions);

    Containers::Array<char> indexData{NoInit, sizeof(UnsignedShort)*3};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        1, 0, 1
    }, indices);

    /* InPlaceInit causes a non-default deleter to be used, which would cause
       a copy to be made internally */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 1};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::arrayView(positions)};
    const Trade::MeshAttributeData* originalAttributes = attributes;

    Trade::MeshData copy = MeshTools::copy(Trade::MeshData{MeshPrimitive::Triangles,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)});
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);

    CORRADE_COMPARE(copy.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(copy.indices<UnsignedShort>(),
        Containers::arrayView(indices),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(copy.vertexCount(), 2);
    CORRADE_COMPARE(copy.attributeCount(), 1);
    CORRADE_COMPARE_AS(copy.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView(positions),
        TestSuite::Compare::Container);

    /* All data should be transferred without any copy */
    CORRADE_COMPARE(copy.indexData().data(), static_cast<const void*>(indices));
    CORRADE_COMPARE(copy.vertexData().data(), static_cast<void*>(positions));
    CORRADE_COMPARE(copy.attributeData().data(), originalAttributes);
}

void CopyTest::copyRvalueAttributesOwned() {
    Vector3 positions[]{
        {1.0f, 2.0f, 3.0f},
        {6.0f, 7.0f, 8.0f},
    };

    /* InPlaceInit causes a non-default deleter to be used, which would cause
       a copy to be made internally */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 1};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::arrayView(positions)};
    const Trade::MeshAttributeData* originalAttributes = attributes;

    Trade::MeshData copy = MeshTools::copy(Trade::MeshData{MeshPrimitive::Triangles, {}, positions, Utility::move(attributes)});
    CORRADE_COMPARE(copy.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);

    CORRADE_COMPARE(copy.vertexCount(), 2);
    CORRADE_COMPARE(copy.attributeCount(), 1);
    CORRADE_COMPARE_AS(copy.attribute<Vector3>(Trade::MeshAttribute::Position),
        Containers::arrayView(positions),
        TestSuite::Compare::Container);

    /* Data should be copied */
    CORRADE_VERIFY(copy.vertexData().data() != static_cast<void*>(positions));
    {
        CORRADE_EXPECT_FAIL("Attribute data currently get copied always when they need to be modified.");
        CORRADE_COMPARE(copy.attributeData().data(), originalAttributes);
    }
}

void CopyTest::reference() {
    const Trade::MeshData grid = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);
    CORRADE_VERIFY(grid.isIndexed());

    Trade::MeshData reference = MeshTools::reference(grid);
    CORRADE_VERIFY(reference.isIndexed());
    CORRADE_COMPARE(reference.primitive(), grid.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.indexCount(), grid.indexCount());
    CORRADE_COMPARE(reference.indexType(), grid.indexType());
    CORRADE_COMPARE(reference.indexOffset(), grid.indexOffset());
    CORRADE_COMPARE(reference.indexStride(), grid.indexStride());
    CORRADE_COMPARE(reference.vertexCount(), grid.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), grid.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), grid.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), grid.attributeData().data());
}

void CopyTest::referenceNoIndexData() {
    const Trade::MeshData circle = Primitives::circle3DSolid(5);
    CORRADE_VERIFY(!circle.isIndexed());

    Trade::MeshData reference = MeshTools::reference(circle);
    CORRADE_VERIFY(!reference.isIndexed());
    CORRADE_COMPARE(reference.primitive(), circle.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexCount(), circle.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), circle.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), circle.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), circle.attributeData().data());
}

void CopyTest::referenceImplementationSpecificIndexType() {
    const UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices)},
        16};

    /* The type should be preserved. not just dropped */
    Trade::MeshData reference = MeshTools::reference(stuff);
    CORRADE_COMPARE(reference.indexType(), meshIndexTypeWrap(0xcaca));
}

void CopyTest::referenceNoIndexVertexAttributeData() {
    Trade::MeshData fourtytwo{MeshPrimitive::Edges, 42};

    Trade::MeshData reference = MeshTools::reference(fourtytwo);
    CORRADE_COMPARE(reference.primitive(), fourtytwo.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexCount(), 42);
    CORRADE_VERIFY(!static_cast<const void*>(reference.indexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.vertexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.attributeData().data()));
}

void CopyTest::mutableReference() {
    Trade::MeshData grid = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);
    CORRADE_VERIFY(grid.isIndexed());

    Trade::MeshData reference = MeshTools::mutableReference(grid);
    CORRADE_VERIFY(reference.isIndexed());
    CORRADE_COMPARE(reference.primitive(), grid.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.indexCount(), grid.indexCount());
    CORRADE_COMPARE(reference.indexType(), grid.indexType());
    CORRADE_COMPARE(reference.indexOffset(), grid.indexOffset());
    CORRADE_COMPARE(reference.indexStride(), grid.indexStride());
    CORRADE_COMPARE(reference.vertexCount(), grid.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), grid.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), grid.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), grid.attributeData().data());
}

void CopyTest::mutableReferenceNoIndexData() {
    Trade::MeshData circle = Primitives::circle3DSolid(5);
    CORRADE_VERIFY(!circle.isIndexed());

    Trade::MeshData reference = MeshTools::mutableReference(circle);
    CORRADE_VERIFY(!reference.isIndexed());
    CORRADE_COMPARE(reference.primitive(), circle.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexCount(), circle.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), circle.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), circle.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), circle.attributeData().data());
}

void CopyTest::mutableReferenceImplementationSpecificIndexType() {
    UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        Trade::DataFlag::Mutable, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices)},
        16};

    /* The type should be preserved. not just dropped */
    Trade::MeshData reference = MeshTools::mutableReference(stuff);
    CORRADE_COMPARE(reference.indexType(), meshIndexTypeWrap(0xcaca));
}

void CopyTest::mutableReferenceNoIndexVertexAttributeData() {
    Trade::MeshData fourtytwo{MeshPrimitive::Edges, 42};

    Trade::MeshData reference = MeshTools::mutableReference(fourtytwo);
    CORRADE_COMPARE(reference.primitive(), fourtytwo.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexCount(), 42);
    CORRADE_VERIFY(!static_cast<const void*>(reference.indexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.vertexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.attributeData().data()));
}

void CopyTest::mutableReferenceNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData cube = Primitives::cubeSolid();
    CORRADE_COMPARE(cube.indexDataFlags(), Trade::DataFlag::Global);
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlag::Global);

    Containers::String out;
    Error redirectError{&out};
    MeshTools::mutableReference(cube);
    CORRADE_COMPARE(out, "MeshTools::mutableReference(): data not mutable\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CopyTest)
