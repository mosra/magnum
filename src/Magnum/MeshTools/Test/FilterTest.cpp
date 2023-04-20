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
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/MeshTools/Filter.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHTOOLS_FILTERATTRIBUTES /* so it doesn't warn */
#include "Magnum/MeshTools/FilterAttributes.h"
#endif

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FilterTest: TestSuite::Tester {
    explicit FilterTest();

    void attributes();
    void attributesNoIndexData();
    void attributesWrongBitCount();

    void onlyAttributes();
    void onlyAttributesNoIndexData();
    void onlyAttributesNoAttributeData();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void onlyAttributeIds();
    void onlyAttributeIdsOutOfBounds();
    void onlyAttributeIdsNoIndexData();
    void onlyAttributeIdsNoAttributeData();
    #endif

    void exceptAttributes();
    void exceptAttributesNoIndexData();
    void exceptAttributesNoAttributeData();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void exceptAttributeIds();
    void exceptAttributeIdsOutOfBounds();
    void exceptAttributeIdsNoIndexData();
    void exceptAttributeIdsNoAttributeData();
    #endif
};

const struct {
    const char* name;
    MeshIndexType indexType;
} ImplementationSpecificIndexTypeData[]{
    {"", MeshIndexType::UnsignedShort},
    {"implementation-specific index type", meshIndexTypeWrap(0xcaca)}
};

FilterTest::FilterTest() {
    addInstancedTests({&FilterTest::attributes},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterTest::attributesNoIndexData,
              &FilterTest::attributesWrongBitCount});

    addInstancedTests({&FilterTest::onlyAttributes},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterTest::onlyAttributesNoIndexData,
              &FilterTest::onlyAttributesNoAttributeData});

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&FilterTest::onlyAttributeIds},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterTest::onlyAttributeIdsOutOfBounds,
              &FilterTest::onlyAttributeIdsNoIndexData,
              &FilterTest::onlyAttributeIdsNoAttributeData});
    #endif

    addInstancedTests({&FilterTest::exceptAttributes},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterTest::exceptAttributesNoIndexData,
              &FilterTest::exceptAttributesNoAttributeData});

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&FilterTest::exceptAttributeIds},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterTest::exceptAttributeIdsOutOfBounds,
              &FilterTest::exceptAttributeIdsNoIndexData,
              &FilterTest::exceptAttributeIdsNoAttributeData});
    #endif
}

struct Vertex {
    Vector3 position;
    Vector4 tangent;
    Vector2 textureCoordinates1, textureCoordinates2;
};

void FilterTest::attributes() {
    auto&& data = ImplementationSpecificIndexTypeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        std::move(indexData), Trade::MeshIndexData{data.indexType, indices},
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertices.slice(&Vertex::tangent)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates2)},
        }};

    Containers::BitArray attributesToKeep{ValueInit, mesh.attributeCount()};
    attributesToKeep.set(1);
    attributesToKeep.set(3);

    Trade::MeshData filtered = filterAttributes(mesh, attributesToKeep);
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), data.indexType);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 2);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Tangent);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, tangent));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates2));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::attributesNoIndexData() {
    /* A trivial subset of filterAttributes() testing it doesn't blow up if the
       mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    Trade::MeshData filtered = filterAttributes(mesh, Containers::BitArray{DirectInit, 1, true});
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleFan);

    CORRADE_VERIFY(!filtered.isIndexed());
    /* Consistent with behavior in reference() for index-less meshes */
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 1);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, textureCoordinates1));
}

void FilterTest::attributesWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* GCC 4.8 dies if I try to initialize this with a {}. I won't, then. */
    Vertex vertices[3];

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates1)}
        }};

    std::ostringstream out;
    Error redirectError{&out};
    filterAttributes(mesh, Containers::BitArray{ValueInit, 3});
    CORRADE_COMPARE(out.str(), "MeshTools::filterAttributes(): expected 2 bits but got 3\n");
}

void FilterTest::onlyAttributes() {
    auto&& data = ImplementationSpecificIndexTypeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        std::move(indexData), Trade::MeshIndexData{data.indexType, indices},
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertices.slice(&Vertex::tangent)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates2)},
        }};

    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        Trade::MeshAttribute::TextureCoordinates, /* present twice in the mesh */
        Trade::MeshAttribute::Position,
        Trade::MeshAttribute::Normal, /* not present in the mesh, ignored */
        Trade::MeshAttribute::Position, /* listed twice, ignored */
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), data.indexType);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 3);
    /* The original order stays even though Position was specified after
       TextureCoordinates in the list */
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, position));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates1));
    CORRADE_COMPARE(filtered.attributeName(2), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(2), offsetof(Vertex, textureCoordinates2));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::onlyAttributesNoIndexData() {
    /* A trivial subset of filterOnlyAttributeNames() testing it doesn't blow
       up if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        Trade::MeshAttribute::TextureCoordinates
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleFan);

    CORRADE_VERIFY(!filtered.isIndexed());
    /* Consistent with behavior in reference() for index-less meshes */
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 1);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, textureCoordinates1));
}

void FilterTest::onlyAttributesNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        Trade::MeshAttribute::Position
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::Points);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    /* The vertex count should get preserved even if there are no attributes */
    CORRADE_COMPARE(filtered.vertexCount(), 15);
    /* Consistent with behavior in reference() for vertex-less meshes */
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.attributeCount(), 0);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void FilterTest::onlyAttributeIds() {
    auto&& data = ImplementationSpecificIndexTypeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        std::move(indexData), Trade::MeshIndexData{data.indexType, indices},
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertices.slice(&Vertex::tangent)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates2)},
        }};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        /* The attribute 1 is specified twice, but that won't result in the
           same attribute being added twice */
        1, 1, 3
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), data.indexType);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 2);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Tangent);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, tangent));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates2));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::onlyAttributeIdsOutOfBounds() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* GCC 4.8 dies if I try to initialize this with a {}. I won't, then. */
    Vertex vertices[3];

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates1)}
        }};

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    filterOnlyAttributes(mesh, {0, 0, 2});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "MeshTools::filterOnlyAttributes(): index 2 out of range for 2 attributes\n");
}

void FilterTest::onlyAttributeIdsNoIndexData() {
    /* A trivial subset of filterOnlyAttributeIds() testing it doesn't blow up
       if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        0
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleFan);

    CORRADE_VERIFY(!filtered.isIndexed());
    /* Consistent with behavior in reference() for index-less meshes */
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 1);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, textureCoordinates1));
}

void FilterTest::onlyAttributeIdsNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterOnlyAttributes(mesh, std::initializer_list<UnsignedInt>{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::Points);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    /* The vertex count should get preserved even if there are no attributes */
    CORRADE_COMPARE(filtered.vertexCount(), 15);
    /* Consistent with behavior in reference() for vertex-less meshes */
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.attributeCount(), 0);
}
#endif

void FilterTest::exceptAttributes() {
    auto&& data = ImplementationSpecificIndexTypeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        std::move(indexData), Trade::MeshIndexData{data.indexType, indices},
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertices.slice(&Vertex::tangent)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates2)},
            /* Positions again, just under a different name. Should be kept. */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(0xbaf), vertices.slice(&Vertex::position)},
        }};

    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        Trade::MeshAttribute::Position,
        Trade::MeshAttribute::Normal, /* not present in the mesh, ignored */
        Trade::MeshAttribute::TextureCoordinates, /* present twice in the mesh */
        Trade::MeshAttribute::Position, /* listed twice, ignored */
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), data.indexType);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 2);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Tangent);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, tangent));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::meshAttributeCustom(0xbaf));
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, position));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::exceptAttributesNoIndexData() {
    /* A trivial subset of filterExceptAttributeNames() testing it doesn't blow
       up if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        Trade::MeshAttribute::Position
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleFan);

    CORRADE_VERIFY(!filtered.isIndexed());
    /* Consistent with behavior in reference() for index-less meshes */
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 1);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, textureCoordinates1));
}

void FilterTest::exceptAttributesNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        Trade::MeshAttribute::Position
    });
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::Points);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    /* The vertex count should get preserved even if there are no attributes */
    CORRADE_COMPARE(filtered.vertexCount(), 15);
    /* Consistent with behavior in reference() for vertex-less meshes */
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.attributeCount(), 0);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void FilterTest::exceptAttributeIds() {
    auto&& data = ImplementationSpecificIndexTypeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleStrip,
        std::move(indexData), Trade::MeshIndexData{data.indexType, indices},
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, vertices.slice(&Vertex::tangent)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates2)},
        }};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        /* The attribute 1 is specified twice, but that won't result in
           attribute 1 being removed and then again */
        1, 1, 3
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleStrip);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), data.indexType);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 2);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, position));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates1));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterTest::exceptAttributeIdsOutOfBounds() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* GCC 4.8 dies if I try to initialize this with a {}. I won't, then. */
    Vertex vertices[3];

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates1)}
        }};

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    filterExceptAttributes(mesh, {0, 0, 2});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "MeshTools::filterExceptAttributes(): index 2 out of range for 2 attributes\n");
}

void FilterTest::exceptAttributeIdsNoIndexData() {
    /* A trivial subset of filterExceptAttributeIds() testing it doesn't blow up
       if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        0
    });
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::TriangleFan);

    CORRADE_VERIFY(!filtered.isIndexed());
    /* Consistent with behavior in reference() for index-less meshes */
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.vertexCount(), 3);
    CORRADE_COMPARE(filtered.vertexData().data(), vertices.data());
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    /* Testing just the offset if it matches expectations, the
       MeshAttributeData is copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.attributeCount(), 1);
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, textureCoordinates1));
}

void FilterTest::exceptAttributeIdsNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    CORRADE_IGNORE_DEPRECATED_PUSH
    Trade::MeshData filtered = filterExceptAttributes(mesh, std::initializer_list<UnsignedInt>{});
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(filtered.primitive(), MeshPrimitive::Points);

    CORRADE_VERIFY(filtered.isIndexed());
    CORRADE_COMPARE(filtered.indexCount(), 5);
    CORRADE_COMPARE(filtered.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(filtered.indexData().data(), indices.data());
    CORRADE_COMPARE(filtered.indexDataFlags(), Trade::DataFlags{});

    /* The vertex count should get preserved even if there are no attributes */
    CORRADE_COMPARE(filtered.vertexCount(), 15);
    /* Consistent with behavior in reference() for vertex-less meshes */
    CORRADE_COMPARE(filtered.vertexDataFlags(), Trade::DataFlags{});

    CORRADE_COMPARE(filtered.attributeCount(), 0);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FilterTest)
