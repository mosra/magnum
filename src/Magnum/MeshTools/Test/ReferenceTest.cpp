/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/MeshTools/Reference.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Trade/MeshData.h"
#include <Magnum/Primitives/Circle.h>

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct ReferenceTest: TestSuite::Tester {
    explicit ReferenceTest();

    void reference();
    void referenceNoIndexData();
    void referenceImplementationSpecificIndexType();
    void referenceNoIndexVertexAttributeData();

    void mutableReference();
    void mutableReferenceNoIndexData();
    void mutableReferenceImplementationSpecificIndexType();
    void mutableReferenceNoIndexVertexAttributeData();
    void mutableReferenceNotMutable();

    void owned();
    void ownedNoIndexData();
    void ownedNoAttributeVertexData();
    void ownedStridedIndices();
    void ownedArrayAttribute();
    void ownedImplementationSpecificVertexFormat();
    void ownedRvaluePassthrough();
    void ownedRvaluePartialPassthrough();
};

struct {
    const char* name;
    MeshIndexType type;
} StridedIndicesData[]{
    {"", MeshIndexType::UnsignedShort},
    {"implementation-specific index format", meshIndexTypeWrap(0xcaca)}
};

ReferenceTest::ReferenceTest() {
    addTests({&ReferenceTest::reference,
              &ReferenceTest::referenceNoIndexData,
              &ReferenceTest::referenceImplementationSpecificIndexType,
              &ReferenceTest::referenceNoIndexVertexAttributeData,

              &ReferenceTest::mutableReference,
              &ReferenceTest::mutableReferenceNoIndexData,
              &ReferenceTest::mutableReferenceImplementationSpecificIndexType,
              &ReferenceTest::mutableReferenceNoIndexVertexAttributeData,
              &ReferenceTest::mutableReferenceNotMutable,

              &ReferenceTest::owned,
              &ReferenceTest::ownedNoIndexData,
              &ReferenceTest::ownedNoAttributeVertexData});

    addInstancedTests({&ReferenceTest::ownedStridedIndices},
        Containers::arraySize(StridedIndicesData));

    addTests({&ReferenceTest::ownedArrayAttribute,
              &ReferenceTest::ownedImplementationSpecificVertexFormat,
              &ReferenceTest::ownedRvaluePassthrough,
              &ReferenceTest::ownedRvaluePartialPassthrough});
}

void ReferenceTest::reference() {
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

void ReferenceTest::referenceNoIndexData() {
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

void ReferenceTest::referenceImplementationSpecificIndexType() {
    const UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices)},
        16};

    /* The type should be preserved. not just dropped */
    Trade::MeshData reference = MeshTools::reference(stuff);
    CORRADE_COMPARE(reference.indexType(), meshIndexTypeWrap(0xcaca));
}

void ReferenceTest::referenceNoIndexVertexAttributeData() {
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

void ReferenceTest::mutableReference() {
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

void ReferenceTest::mutableReferenceNoIndexData() {
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

void ReferenceTest::mutableReferenceImplementationSpecificIndexType() {
    UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        Trade::DataFlag::Mutable, indices, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::stridedArrayView(indices)},
        16};

    /* The type should be preserved. not just dropped */
    Trade::MeshData reference = MeshTools::mutableReference(stuff);
    CORRADE_COMPARE(reference.indexType(), meshIndexTypeWrap(0xcaca));
}

void ReferenceTest::mutableReferenceNoIndexVertexAttributeData() {
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

void ReferenceTest::mutableReferenceNotMutable() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Trade::MeshData cube = Primitives::cubeSolid();
    CORRADE_COMPARE(cube.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlags{});

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::mutableReference(cube);
    CORRADE_COMPARE(out.str(), "MeshTools::mutableReference(): data not mutable\n");
}

void ReferenceTest::owned() {
    Trade::MeshData cube = Primitives::cubeSolid();
    CORRADE_COMPARE(cube.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlags{});

    Trade::MeshData owned = MeshTools::owned(cube);
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.primitive(), cube.primitive());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.indexCount(), cube.indexCount());
    CORRADE_COMPARE(owned.indexType(), cube.indexType());
    CORRADE_COMPARE(owned.indexOffset(), cube.indexOffset());
    CORRADE_COMPARE(owned.indexStride(), cube.indexStride());
    CORRADE_COMPARE(owned.vertexCount(), cube.vertexCount());
    CORRADE_COMPARE(owned.attributeCount(), cube.attributeCount());

    for(std::size_t i = 0; i != cube.attributeCount(); ++i) {
        CORRADE_ITERATION(i);

        CORRADE_COMPARE(owned.attributeName(i), cube.attributeName(i));
        CORRADE_COMPARE(owned.attributeFormat(i), cube.attributeFormat(i));
        CORRADE_COMPARE(owned.attributeOffset(i), cube.attributeOffset(i));
        CORRADE_COMPARE(owned.attributeStride(i), cube.attributeStride(i));
        CORRADE_COMPARE(owned.attributeArraySize(i), cube.attributeArraySize(i));
    }

    CORRADE_COMPARE_AS(owned.indexData(), cube.indexData(), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(owned.vertexData(), cube.vertexData(), TestSuite::Compare::Container);
}

void ReferenceTest::ownedNoIndexData() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();
    CORRADE_VERIFY(!cube.isIndexed());
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlags{});

    Trade::MeshData owned = MeshTools::owned(cube);
    CORRADE_VERIFY(!owned.isIndexed());
    CORRADE_COMPARE(owned.primitive(), cube.primitive());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexCount(), cube.vertexCount());
    CORRADE_COMPARE(owned.attributeCount(), cube.attributeCount());
}

void ReferenceTest::ownedNoAttributeVertexData() {
    UnsignedShort indexData[]{0, 41, 2};
    Trade::MeshData indexedFourtytwo{MeshPrimitive::Edges,
        {}, indexData, Trade::MeshIndexData{indexData},
        42};

    Trade::MeshData owned = MeshTools::owned(indexedFourtytwo);
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.primitive(), MeshPrimitive::Edges);
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.indexCount(), 3);
    CORRADE_COMPARE(owned.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(owned.indexOffset(), 0);
    CORRADE_COMPARE(owned.vertexCount(), 42);
    CORRADE_COMPARE(owned.attributeCount(), 0);

    CORRADE_COMPARE_AS(owned.indexData(), indexedFourtytwo.indexData(), TestSuite::Compare::Container);
    CORRADE_VERIFY(!static_cast<const void*>(owned.vertexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(owned.attributeData().data()));
}

void ReferenceTest::ownedStridedIndices() {
    auto&& data = StridedIndicesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const UnsignedShort indices[7]{0, 3, 0, 7, 0, 15, 0};
    Trade::MeshData stuff{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{data.type, Containers::stridedArrayView(indices).exceptPrefix(1).every(2)},
        16};

    /* The full index data layout including whatever format should be
       preserved */
    Trade::MeshData owned = MeshTools::owned(stuff);
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.primitive(), MeshPrimitive::Points);
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.indexCount(), 3);
    CORRADE_COMPARE(owned.indexType(), data.type);
    CORRADE_COMPARE(owned.indexOffset(), 2);
    CORRADE_COMPARE(owned.indexStride(), 4);
    CORRADE_COMPARE(owned.vertexCount(), 16);
    CORRADE_COMPARE(owned.attributeCount(), 0);

    /* Has to do a prefix() because for an implementation-specific index type
       the returned size is equal to stride */
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(owned.indices().prefix({owned.indexCount(), 2}))),
        Containers::arrayView<UnsignedShort>({3, 7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(owned.indexData(), stuff.indexData(),
        TestSuite::Compare::Container);
}

void ReferenceTest::ownedArrayAttribute() {
    const Vector3us vertexData[13]{};
    /* Verify that array attributes are propagated correctly */
    Trade::MeshData weirdThing{MeshPrimitive::Faces,
        {}, vertexData,
        {Trade::MeshAttributeData{Trade::meshAttributeCustom(42), VertexFormat::Half, Containers::arrayView(vertexData), 3}}};

    Trade::MeshData owned = MeshTools::owned(weirdThing);
    CORRADE_COMPARE(owned.vertexCount(), 13);
    CORRADE_COMPARE(owned.attributeCount(), 1);
    CORRADE_COMPARE(owned.attributeArraySize(0), 3);
}

void ReferenceTest::ownedImplementationSpecificVertexFormat() {
    const Int vertexData[13]{};
    /* Verify that custom vertex formats are propagated without a problem */
    Trade::MeshData weirdThing{MeshPrimitive::Faces,
        {}, vertexData,
        {Trade::MeshAttributeData{Trade::meshAttributeCustom(42), vertexFormatWrap(0xcaca), Containers::arrayView(vertexData)}}};

    Trade::MeshData owned = MeshTools::owned(weirdThing);
    CORRADE_COMPARE(owned.vertexCount(), 13);
    CORRADE_COMPARE(owned.attributeCount(), 1);
    CORRADE_COMPARE(owned.attributeArraySize(0), 0);
    CORRADE_COMPARE(owned.attributeFormat(0), vertexFormatWrap(0xcaca));
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Int>(owned.attribute(0))),
        Containers::arrayView(vertexData),
        TestSuite::Compare::Container);
}

void ReferenceTest::ownedRvaluePassthrough() {
    Trade::MeshData grid = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);
    CORRADE_COMPARE(grid.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(grid.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    UnsignedInt indexCount = grid.indexCount();
    MeshIndexType indexType = grid.indexType();
    std::size_t indexOffset = grid.indexOffset();
    Int indexStride = grid.indexStride();
    UnsignedInt vertexCount = grid.vertexCount();
    const void* indexData = grid.indexData();
    const void* vertexData = grid.vertexData();
    const void* attributeData = grid.attributeData();

    Trade::MeshData owned = MeshTools::owned(std::move(grid));
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.indexCount(), indexCount);
    CORRADE_COMPARE(owned.indexType(), indexType);
    CORRADE_COMPARE(owned.indexOffset(), indexOffset);
    CORRADE_COMPARE(owned.indexStride(), indexStride);
    CORRADE_COMPARE(owned.vertexCount(), vertexCount);
    CORRADE_COMPARE(owned.indexData(), indexData);
    CORRADE_COMPARE(owned.vertexData(), vertexData);
    CORRADE_COMPARE(owned.attributeData(), attributeData);
}

void ReferenceTest::ownedRvaluePartialPassthrough() {
    Trade::MeshData gradient = Primitives::gradient3DHorizontal({}, {});
    CORRADE_COMPARE(gradient.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    UnsignedInt vertexCount = gradient.vertexCount();
    const void* vertexData = gradient.vertexData();
    const void* attributeData = gradient.attributeData();

    Trade::MeshData owned = MeshTools::owned(std::move(gradient));
    CORRADE_VERIFY(!owned.isIndexed());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexCount(), vertexCount);
    CORRADE_COMPARE(owned.vertexData(), vertexData);
    /* Attribute data is constant in the original, so this gets copied */
    CORRADE_VERIFY(owned.attributeData() != attributeData);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::ReferenceTest)
