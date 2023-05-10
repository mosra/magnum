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
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Trade/MeshData.h"
#include <Magnum/Primitives/Circle.h>

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CopyTest: TestSuite::Tester {
    explicit CopyTest();

    void copy();
    void copyNoIndexData();
    void copyNoAttributeVertexData();
    void copyStridedIndices();
    void copyArrayAttribute();
    void copyImplementationSpecificVertexFormat();
    void copyRvaluePassthrough();
    void copyRvaluePartialPassthrough();

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

    addTests({&CopyTest::copyArrayAttribute,
              &CopyTest::copyImplementationSpecificVertexFormat,
              &CopyTest::copyRvaluePassthrough,
              &CopyTest::copyRvaluePartialPassthrough,

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
    Trade::MeshData cube = Primitives::cubeSolid();
    CORRADE_COMPARE(cube.indexDataFlags(), Trade::DataFlag::Global);
    CORRADE_COMPARE(cube.vertexDataFlags(), Trade::DataFlag::Global);

    Trade::MeshData copy = MeshTools::copy(cube);
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.primitive(), cube.primitive());
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.indexCount(), cube.indexCount());
    CORRADE_COMPARE(copy.indexType(), cube.indexType());
    CORRADE_COMPARE(copy.indexOffset(), cube.indexOffset());
    CORRADE_COMPARE(copy.indexStride(), cube.indexStride());
    CORRADE_COMPARE(copy.vertexCount(), cube.vertexCount());
    CORRADE_COMPARE(copy.attributeCount(), cube.attributeCount());

    for(std::size_t i = 0; i != cube.attributeCount(); ++i) {
        CORRADE_ITERATION(i);

        CORRADE_COMPARE(copy.attributeName(i), cube.attributeName(i));
        CORRADE_COMPARE(copy.attributeFormat(i), cube.attributeFormat(i));
        CORRADE_COMPARE(copy.attributeOffset(i), cube.attributeOffset(i));
        CORRADE_COMPARE(copy.attributeStride(i), cube.attributeStride(i));
        CORRADE_COMPARE(copy.attributeArraySize(i), cube.attributeArraySize(i));
    }

    CORRADE_COMPARE_AS(copy.indexData(), cube.indexData(), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.vertexData(), cube.vertexData(), TestSuite::Compare::Container);

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

void CopyTest::copyArrayAttribute() {
    const Vector3us vertexData[13]{};
    /* Verify that array attributes are propagated correctly */
    Trade::MeshData weirdThing{MeshPrimitive::Faces,
        {}, vertexData,
        {Trade::MeshAttributeData{Trade::meshAttributeCustom(42), VertexFormat::Half, Containers::arrayView(vertexData), 3}}};

    Trade::MeshData copy = MeshTools::copy(weirdThing);
    CORRADE_COMPARE(copy.vertexCount(), 13);
    CORRADE_COMPARE(copy.attributeCount(), 1);
    CORRADE_COMPARE(copy.attributeArraySize(0), 3);
}

void CopyTest::copyImplementationSpecificVertexFormat() {
    const Int vertexData[13]{};
    /* Verify that custom vertex formats are propagated without a problem */
    Trade::MeshData weirdThing{MeshPrimitive::Faces,
        {}, vertexData,
        {Trade::MeshAttributeData{Trade::meshAttributeCustom(42), vertexFormatWrap(0xcaca), Containers::arrayView(vertexData)}}};

    Trade::MeshData copy = MeshTools::copy(weirdThing);
    CORRADE_COMPARE(copy.vertexCount(), 13);
    CORRADE_COMPARE(copy.attributeCount(), 1);
    CORRADE_COMPARE(copy.attributeArraySize(0), 0);
    CORRADE_COMPARE(copy.attributeFormat(0), vertexFormatWrap(0xcaca));
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Int>(copy.attribute(0))),
        Containers::arrayView(vertexData),
        TestSuite::Compare::Container);
}

void CopyTest::copyRvaluePassthrough() {
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

    Trade::MeshData copy = MeshTools::copy(std::move(grid));
    CORRADE_VERIFY(copy.isIndexed());
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.indexCount(), indexCount);
    CORRADE_COMPARE(copy.indexType(), indexType);
    CORRADE_COMPARE(copy.indexOffset(), indexOffset);
    CORRADE_COMPARE(copy.indexStride(), indexStride);
    CORRADE_COMPARE(copy.vertexCount(), vertexCount);
    CORRADE_COMPARE(copy.indexData(), indexData);
    CORRADE_COMPARE(copy.vertexData(), vertexData);
    CORRADE_COMPARE(copy.attributeData(), attributeData);
}

void CopyTest::copyRvaluePartialPassthrough() {
    Trade::MeshData gradient = Primitives::gradient3DHorizontal({}, {});
    CORRADE_COMPARE(gradient.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    UnsignedInt vertexCount = gradient.vertexCount();
    const void* vertexData = gradient.vertexData();
    const void* attributeData = gradient.attributeData();

    Trade::MeshData copy = MeshTools::copy(std::move(gradient));
    CORRADE_VERIFY(!copy.isIndexed());
    CORRADE_COMPARE(copy.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(copy.vertexCount(), vertexCount);
    CORRADE_COMPARE(copy.vertexData(), vertexData);
    /* Attribute data is constant in the original, so this gets copied */
    CORRADE_VERIFY(copy.attributeData() != attributeData);
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

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::mutableReference(cube);
    CORRADE_COMPARE(out.str(), "MeshTools::mutableReference(): data not mutable\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CopyTest)
