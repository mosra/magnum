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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/MeshTools/FilterAttributes.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FilterAttributesTest: TestSuite::Tester {
    explicit FilterAttributesTest();

    void filterOnlyAttributeNames();
    void filterOnlyAttributeNamesNoIndexData();
    void filterOnlyAttributeNamesNoAttributeData();

    void filterOnlyAttributeIds();
    void filterOnlyAttributeIdsOutOfBounds();
    void filterOnlyAttributeIdsNoIndexData();
    void filterOnlyAttributeIdsNoAttributeData();

    void filterExceptAttributeNames();
    void filterExceptAttributeNamesNoIndexData();
    void filterExceptAttributeNamesNoAttributeData();

    void filterExceptAttributeIds();
    void filterExceptAttributeIdsOutOfBounds();
    void filterExceptAttributeIdsNoIndexData();
    void filterExceptAttributeIdsNoAttributeData();
};

const struct {
    const char* name;
    MeshIndexType indexType;
} ImplementationSpecificIndexTypeData[]{
    {"", MeshIndexType::UnsignedShort},
    {"implementation-specific index type", meshIndexTypeWrap(0xcaca)}
};

FilterAttributesTest::FilterAttributesTest() {
    addInstancedTests({&FilterAttributesTest::filterOnlyAttributeNames},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterAttributesTest::filterOnlyAttributeNamesNoIndexData,
              &FilterAttributesTest::filterOnlyAttributeNamesNoAttributeData});

    addInstancedTests({&FilterAttributesTest::filterOnlyAttributeIds},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterAttributesTest::filterOnlyAttributeIdsOutOfBounds,
              &FilterAttributesTest::filterOnlyAttributeIdsNoIndexData,
              &FilterAttributesTest::filterOnlyAttributeIdsNoAttributeData});

    addInstancedTests({&FilterAttributesTest::filterExceptAttributeNames},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterAttributesTest::filterExceptAttributeNamesNoIndexData,
              &FilterAttributesTest::filterExceptAttributeNamesNoAttributeData});

    addInstancedTests({&FilterAttributesTest::filterExceptAttributeIds},
        Containers::arraySize(ImplementationSpecificIndexTypeData));

    addTests({&FilterAttributesTest::filterExceptAttributeIdsOutOfBounds,
              &FilterAttributesTest::filterExceptAttributeIdsNoIndexData,
              &FilterAttributesTest::filterExceptAttributeIdsNoAttributeData});
}

struct Vertex {
    Vector3 position;
    Vector4 tangent;
    Vector2 textureCoordinates1, textureCoordinates2;
};

void FilterAttributesTest::filterOnlyAttributeNames() {
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
        Trade::MeshAttribute::Position,
        Trade::MeshAttribute::Normal, /* not present, ignored */
        Trade::MeshAttribute::TextureCoordinates, /* present twice */
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

void FilterAttributesTest::filterOnlyAttributeNamesNoIndexData() {
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

void FilterAttributesTest::filterOnlyAttributeNamesNoAttributeData() {
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

void FilterAttributesTest::filterOnlyAttributeIds() {
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
        /* The attribute 1 is specified twice, but that won't result in the
           same attribute being added twice */
        1, 1, 3
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
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates2));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterAttributesTest::filterOnlyAttributeIdsOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* GCC 4.8 dies if I try to initialize this with a {}. I won't, then. */
    Vertex vertices[3];

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates1)}
        }};

    std::ostringstream out;
    Error redirectError{&out};
    filterOnlyAttributes(mesh, {0, 0, 2});
    CORRADE_COMPARE(out.str(), "MeshTools::filterOnlyAttributes(): index 2 out of range for 2 attributes\n");
}

void FilterAttributesTest::filterOnlyAttributeIdsNoIndexData() {
    /* A trivial subset of filterOnlyAttributeIds() testing it doesn't blow up
       if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    Trade::MeshData filtered = filterOnlyAttributes(mesh, {
        0
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

void FilterAttributesTest::filterOnlyAttributeIdsNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    Trade::MeshData filtered = filterOnlyAttributes(mesh, std::initializer_list<UnsignedInt>{});
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

void FilterAttributesTest::filterExceptAttributeNames() {
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
        Trade::MeshAttribute::Normal, /* not present, ignored */
        Trade::MeshAttribute::TextureCoordinates, /* present twice */
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

void FilterAttributesTest::filterExceptAttributeNamesNoIndexData() {
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

void FilterAttributesTest::filterExceptAttributeNamesNoAttributeData() {
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

void FilterAttributesTest::filterExceptAttributeIds() {
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

    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        /* The attribute 1 is specified twice, but that won't result in
           attribute 1 being removed and then again */
        1, 1, 3
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
    CORRADE_COMPARE(filtered.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(filtered.attributeOffset(0), offsetof(Vertex, position));
    CORRADE_COMPARE(filtered.attributeName(1), Trade::MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(filtered.attributeOffset(1), offsetof(Vertex, textureCoordinates1));

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::MeshAttributeData> attributeData = filtered.releaseAttributeData();
    CORRADE_VERIFY(!attributeData.deleter());
}

void FilterAttributesTest::filterExceptAttributeIdsOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* GCC 4.8 dies if I try to initialize this with a {}. I won't, then. */
    Vertex vertices[3];

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(vertices).slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, Containers::stridedArrayView(vertices).slice(&Vertex::textureCoordinates1)}
        }};

    std::ostringstream out;
    Error redirectError{&out};
    filterExceptAttributes(mesh, {0, 0, 2});
    CORRADE_COMPARE(out.str(), "MeshTools::filterExceptAttributes(): index 2 out of range for 2 attributes\n");
}

void FilterAttributesTest::filterExceptAttributeIdsNoIndexData() {
    /* A trivial subset of filterExceptAttributeIds() testing it doesn't blow up
       if the mesh is not indexed */

    Containers::Array<char> vertexData{3*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);

    Trade::MeshData mesh{MeshPrimitive::TriangleFan,
        std::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, vertices.slice(&Vertex::textureCoordinates1)}
        }};

    Trade::MeshData filtered = filterExceptAttributes(mesh, {
        0
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

void FilterAttributesTest::filterExceptAttributeIdsNoAttributeData() {
    Containers::Array<char> indexData{5*sizeof(UnsignedShort)};
    Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);

    Trade::MeshData mesh{MeshPrimitive::Points,
        std::move(indexData), Trade::MeshIndexData{indices}, 15};

    Trade::MeshData filtered = filterExceptAttributes(mesh, std::initializer_list<UnsignedInt>{});
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

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FilterAttributesTest)
