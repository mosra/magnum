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
#include "Magnum/MeshTools/Reference.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Primitives/Grid.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct ReferenceTest: TestSuite::Tester {
    explicit ReferenceTest();

    void reference();
    void referenceNoIndexVertexAttributeData();

    void mutableReference();
    void mutableReferenceNoIndexVertexAttributeData();
    void mutableReferenceNotMutable();

    void owned();
    void ownedNoIndexData();
    void ownedNoAttributeVertexData();
    void ownedArrayAttribute();
    void ownedRvaluePassthrough();
    void ownedRvaluePartialPassthrough();
};

ReferenceTest::ReferenceTest() {
    addTests({&ReferenceTest::reference,
              &ReferenceTest::referenceNoIndexVertexAttributeData,

              &ReferenceTest::mutableReference,
              &ReferenceTest::mutableReferenceNoIndexVertexAttributeData,
              &ReferenceTest::mutableReferenceNotMutable,

              &ReferenceTest::owned,
              &ReferenceTest::ownedNoIndexData,
              &ReferenceTest::ownedNoAttributeVertexData,
              &ReferenceTest::ownedArrayAttribute,
              &ReferenceTest::ownedRvaluePassthrough,
              &ReferenceTest::ownedRvaluePartialPassthrough});
}

void ReferenceTest::reference() {
    const Trade::MeshData circle = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);

    Trade::MeshData reference = MeshTools::reference(circle);
    CORRADE_COMPARE(reference.primitive(), circle.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(reference.vertexCount(), circle.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), circle.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), circle.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), circle.attributeData().data());
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
    Trade::MeshData circle = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);
    CORRADE_VERIFY(circle.isIndexed());

    Trade::MeshData reference = MeshTools::mutableReference(circle);
    CORRADE_COMPARE(reference.primitive(), circle.primitive());
    CORRADE_COMPARE(reference.indexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexDataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(reference.vertexCount(), circle.vertexCount());
    CORRADE_COMPARE(static_cast<const void*>(reference.indexData().data()), circle.indexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.vertexData().data()), circle.vertexData().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.attributeData().data()), circle.attributeData().data());
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
    CORRADE_COMPARE(owned.primitive(), cube.primitive());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.indexCount(), cube.indexCount());
    CORRADE_COMPARE(owned.indexType(), cube.indexType());
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
    CORRADE_COMPARE(owned.primitive(), cube.primitive());
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(!owned.isIndexed());
    CORRADE_COMPARE(owned.vertexCount(), cube.vertexCount());
    CORRADE_COMPARE(owned.attributeCount(), cube.attributeCount());
}

void ReferenceTest::ownedNoAttributeVertexData() {
    UnsignedShort indexData[]{0, 41, 2};
    Trade::MeshData indexedFourtytwo{MeshPrimitive::Edges,
        {}, indexData, Trade::MeshIndexData{indexData},
        42};

    Trade::MeshData owned = MeshTools::owned(indexedFourtytwo);
    CORRADE_COMPARE(owned.primitive(), MeshPrimitive::Edges);
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(owned.isIndexed());
    CORRADE_COMPARE(owned.indexCount(), 3);
    CORRADE_COMPARE(owned.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(owned.vertexCount(), 42);
    CORRADE_COMPARE(owned.attributeCount(), 0);

    CORRADE_COMPARE_AS(owned.indexData(), indexedFourtytwo.indexData(), TestSuite::Compare::Container);
    CORRADE_VERIFY(!static_cast<const void*>(owned.vertexData().data()));
    CORRADE_VERIFY(!static_cast<const void*>(owned.attributeData().data()));
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

void ReferenceTest::ownedRvaluePassthrough() {
    Trade::MeshData circle = Primitives::grid3DSolid({15, 3}, Primitives::GridFlag::Tangents);
    CORRADE_COMPARE(circle.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(circle.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    const void* indexData = circle.indexData();
    const void* vertexData = circle.vertexData();
    const void* attributeData = circle.attributeData();

    Trade::MeshData owned = MeshTools::owned(std::move(circle));
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.indexData(), indexData);
    CORRADE_COMPARE(owned.vertexData(), vertexData);
    CORRADE_COMPARE(owned.attributeData(), attributeData);
}

void ReferenceTest::ownedRvaluePartialPassthrough() {
    Trade::MeshData gradient = Primitives::gradient3DHorizontal({}, {});
    CORRADE_COMPARE(gradient.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    const void* vertexData = gradient.vertexData();
    const void* attributeData = gradient.attributeData();

    Trade::MeshData owned = MeshTools::owned(std::move(gradient));
    CORRADE_COMPARE(owned.indexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_COMPARE(owned.vertexDataFlags(), Trade::DataFlag::Mutable|Trade::DataFlag::Owned);
    CORRADE_VERIFY(!owned.isIndexed());
    CORRADE_COMPARE(owned.vertexData(), vertexData);
    /* Attribute data is constant in the original, so this gets copied */
    CORRADE_VERIFY(owned.attributeData() != attributeData);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::ReferenceTest)
