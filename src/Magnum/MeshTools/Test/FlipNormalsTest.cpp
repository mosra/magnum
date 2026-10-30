/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Half.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/MeshTools/FlipNormals.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FlipNormalsTest: TestSuite::Tester {
    explicit FlipNormalsTest();

    void flipFaceWindingWrongIndexCount();
    template<class T> void flipFaceWinding();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void flipFaceWindingDeprecated();
    #endif

    template<class T> void flipFaceWindingErased();
    void flipFaceWindingErasedNonContiguous();

    template<class T> void flipFaceWindingMeshDataIndexed();
    void flipFaceWindingMeshDataNonIndexed();
    /* Verifies also asserts propagated to flipNormals() */
    void flipFaceWindingMeshDataWrongPrimitiveIndexVertexCount();
    void flipFaceWindingMeshDataImplementationSpecificIndexType();
    void flipFaceWindingMeshDataImplementationSpecificVertexFormat();
    void flipFaceWindingMeshDataRvaluePassthroughIndexed();
    void flipFaceWindingMeshDataRvaluePassthroughNonIndexed();
    void flipFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned();
    void flipFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned();
    void flipFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned();
    /* In-place variant called from the others and as such tested sufficiently,
       except for the asserts below. Assertions tested for both
       flipFaceWindingInPlace() and flipNormalsInPlace(). */
    void flipFaceWindingMeshDataInPlaceIndexedIndexDataNotMutable();
    void flipFaceWindingMeshDataInPlaceNonIndexedVertexDataNotMutable();

    void flipNormals();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void flipNormalsDeprecated();
    #endif

    template<class T> void flipNormalsFaceWinding();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void flipNormalsFaceWindingDeprecated();
    #endif

    template<class T> void flipNormalsFaceWindingErased();
    /* Delegates into type-erased flipFaceWinding(), so testing asserts again
       isn't needed */

    template<class T> void flipNormalsFaceWindingMeshData();
    void flipNormalsFaceWindingMeshDataImplementationSpecificIndexType();
    void flipNormalsFaceWindingMeshDataImplementationSpecificVertexFormat();
    void flipNormalsFaceWindingMeshDataRvaluePassthrough();
    /* Assertions coming from flipFaceWinding() tested in
       flipFaceWindingMeshData*() above already */
    void flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned();
    void flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned();
    void flipNormalsFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned();
    void flipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormat();
    /* In-place variant called from the others and as such tested sufficiently,
       except for the cases and asserts below. Assertions coming from
       flipFaceWindingInPlace() tested in flipFaceWindingMeshDataInPlace*()
       above already. */
    void flipNormalsFaceWindingMeshDataInPlaceIndexedVertexDataNotMutable();
    void flipNormalsFaceWindingMeshDataInPlaceNormalDataNotMutable();
    void flipNormalsFaceWindingMeshDataInPlaceNormalDataWrongFormat();
};

using namespace Math::Literals;

const struct {
    const char* name;
    bool indexed;
    bool normals, secondaryNormals;
    Int secondaryNormalsMorphTarget;
    InterleaveFlags interleaveFlags;
    std::ptrdiff_t expectedIndexStrideWithExpandedVertexFormat;
} FlipNormalsFaceWindingMeshDataData[]{
    {"indexed, no normals",
        /* There are no normals so no vertex format expansion happens here and
           thus the indices preserve the original stride even with no
           InterleaveFlag */
        true, false, false, -1, {}, 4},
    {"indexed",
        true, true, false, -1, {}, 2},
    {"indexed, PreserveStridedIndices",
        true, true, false, -1, InterleaveFlag::PreserveStridedIndices, 4},
    {"indexed, secondary normals",
        true, true, true, -1, {}, 2},
    {"indexed, secondary normals in a different morph target",
        true, true, true, 37, {}, 2},
    {"non-indexed, no normals",
        /* There are no indices thus there is no stride, same below */
        false, false, false, -1, {}, 0},
    {"non-indexed",
        false, true, false, -1, {}, 0},
    {"non-indexed, secondary normals",
        false, true, true, -1, {}, 0},
    {"non-indexed, secondary normals in a different morph target",
        false, true, true, 37, {}, 0},
};

const struct {
    const char* name;
    Int morphTargetId;
    bool secondaryNormals;
} FlipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormatData[]{
    {"", -1, false},
    {"seconary normals", -1, true},
    {"custom morph target ID", 37, false},
    {"custom morph target ID, secondary normals", 37, true},
};

FlipNormalsTest::FlipNormalsTest() {
    addTests({&FlipNormalsTest::flipFaceWindingWrongIndexCount,
              &FlipNormalsTest::flipFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipFaceWinding<UnsignedInt>,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &FlipNormalsTest::flipFaceWindingDeprecated,
              #endif

              &FlipNormalsTest::flipFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingErased<Vector3us>,
              &FlipNormalsTest::flipFaceWindingErased<Vector4ui>,
              &FlipNormalsTest::flipFaceWindingErasedNonContiguous,

              &FlipNormalsTest::flipFaceWindingMeshDataIndexed<UnsignedByte>,
              &FlipNormalsTest::flipFaceWindingMeshDataIndexed<UnsignedShort>,
              &FlipNormalsTest::flipFaceWindingMeshDataIndexed<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingMeshDataNonIndexed,
              &FlipNormalsTest::flipFaceWindingMeshDataWrongPrimitiveIndexVertexCount,
              &FlipNormalsTest::flipFaceWindingMeshDataImplementationSpecificIndexType,
              &FlipNormalsTest::flipFaceWindingMeshDataImplementationSpecificVertexFormat,
              &FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexed,
              &FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughNonIndexed,
              &FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned,
              &FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned,
              &FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned,
              &FlipNormalsTest::flipFaceWindingMeshDataInPlaceIndexedIndexDataNotMutable,
              &FlipNormalsTest::flipFaceWindingMeshDataInPlaceNonIndexedVertexDataNotMutable,

              &FlipNormalsTest::flipNormals,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &FlipNormalsTest::flipNormalsDeprecated,
              #endif

              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedInt>,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &FlipNormalsTest::flipNormalsFaceWindingDeprecated,
              #endif

              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedInt>});

    addInstancedTests<FlipNormalsTest>({
        &FlipNormalsTest::flipNormalsFaceWindingMeshData<Float>,
        &FlipNormalsTest::flipNormalsFaceWindingMeshData<Half>
    }, Containers::arraySize(FlipNormalsFaceWindingMeshDataData));

    addTests({&FlipNormalsTest::flipNormalsFaceWindingMeshDataImplementationSpecificIndexType,
              &FlipNormalsTest::flipNormalsFaceWindingMeshDataImplementationSpecificVertexFormat});

    addInstancedTests({&FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthrough},
        Containers::arraySize(FlipNormalsFaceWindingMeshDataData));

    addTests({&FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned,
              &FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned,
              &FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned});

    addInstancedTests({&FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormat},
        Containers::arraySize(FlipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormatData));

    addTests({&FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceIndexedVertexDataNotMutable,
              &FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceNormalDataNotMutable,
              &FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceNormalDataWrongFormat});
}

void FlipNormalsTest::flipFaceWindingWrongIndexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Passing an empty view is okay */
    flipFaceWindingInPlace(Containers::ArrayView<UnsignedShort>{});

    UnsignedByte indices[7];
    Vector3b attribute[8];

    Containers::String out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::stridedArrayView(indices));
    flipNormalsInPlace(Containers::stridedArrayView(indices), {});
    flipFaceWindingInPlace(Containers::arrayCast<2, char>(Containers::stridedArrayView(attribute)));
    CORRADE_COMPARE_AS(out,
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 7\n"
        /* flipNormalsInPlace() delegates to flipFaceWindingInPlace() */
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 7\n"
        /* The fallback byte-by-byte flipping code has its own assert */
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 8\n",
        TestSuite::Compare::String);
}

template<class T> void FlipNormalsTest::flipFaceWinding() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    flipFaceWindingInPlace(indices);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void FlipNormalsTest::flipFaceWindingDeprecated() {
    std::vector<UnsignedInt> indices{0, 1, 2, 3, 4, 5};
    CORRADE_IGNORE_DEPRECATED_PUSH
    MeshTools::flipFaceWinding(indices);
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<UnsignedInt>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
}
#endif

template<class> struct IndexTraits;
template<> struct IndexTraits<UnsignedByte> {
    static const char* name() { return "UnsignedByte"; }
    static UnsignedByte multiplier() { return 47; };
};
template<> struct IndexTraits<UnsignedShort> {
    static const char* name() { return "UnsignedShort"; }
    static UnsignedShort multiplier() { return 13103; };
};
template<> struct IndexTraits<UnsignedInt> {
    static const char* name() { return "UnsignedInt"; }
    static UnsignedInt multiplier() { return 858993433; };
};
template<> struct IndexTraits<Vector3us> {
    static const char* name() { return "Vector3us"; }
    static Vector3us multiplier() { return {13103, 13099, 13001}; }
};
template<> struct IndexTraits<Vector4ui> {
    static const char* name() { return "Vector4ui"; }
    static Vector4ui multiplier() { return {858993433, 286331141, 1228889, 12413}; }
};

template<class T> void FlipNormalsTest::flipFaceWindingErased() {
    setTestCaseTemplateName(IndexTraits<T>::name());

    T indices[]{
        T(0*IndexTraits<T>::multiplier()),
        T(1*IndexTraits<T>::multiplier()),
        T(2*IndexTraits<T>::multiplier()),
        T(3*IndexTraits<T>::multiplier()),
        T(4*IndexTraits<T>::multiplier()),
        T(5*IndexTraits<T>::multiplier()),
    };
    flipFaceWindingInPlace(Containers::arrayCast<2, char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE_AS(Containers::arrayView(indices), Containers::arrayView({
        T(0*IndexTraits<T>::multiplier()),
        T(2*IndexTraits<T>::multiplier()),
        T(1*IndexTraits<T>::multiplier()),
        T(3*IndexTraits<T>::multiplier()),
        T(5*IndexTraits<T>::multiplier()),
        T(4*IndexTraits<T>::multiplier()),
    }), TestSuite::Compare::Container);
}

void FlipNormalsTest::flipFaceWindingErasedNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*4]{};

    Containers::String out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}});
    CORRADE_COMPARE(out,
        "MeshTools::flipFaceWindingInPlace(): second index view dimension is not contiguous\n");
}

template<class T> void FlipNormalsTest::flipFaceWindingMeshDataIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{
        0, 1, 2,
        3, 4, 5
    };
    /* Normals added just to verify they don't get expanded, flipped or
       reordered */
    Vector3h normals[]{
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}
    };
    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices},
        {}, normals, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)},
            /* Just to verify that implementation-specific formats don't affect
               this case at all */
            Trade::MeshAttributeData{Trade::meshAttributeCustom(35), vertexFormatWrap(0xcaca), Containers::arrayView(normals)},
        }};

    Trade::MeshData out = MeshTools::flipFaceWinding(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    /* Second and third in each group swapped, no format conversion */
    CORRADE_COMPARE(out.indexType(), Trade::Implementation::meshIndexTypeFor<T>());
    CORRADE_COMPARE_AS(out.indices<T>(), Containers::arrayView<T>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 2);
    /* No swapping or format conversion in any attributes */
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(0),
        Containers::arrayView(normals),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeFormat(1), vertexFormatWrap(0xcaca));
}

void FlipNormalsTest::flipFaceWindingMeshDataNonIndexed() {
    /* Deliberately not interleaved to verify it doesn't work with just
       interleaved meshes */
    struct {
        UnsignedInt id[6];
        /* Normals added just to verify they don't get expanded or flipped,
           just reordered */
        Vector3h normal[6];
    } vertices[]{{
        {100, 101, 102,
         103, 104, 105},
        {{0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h},
         {3.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}}
    }};
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, vertices, {
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertices->id)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices->normal)},
    }};

    Trade::MeshData out = MeshTools::flipFaceWinding(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!out.isIndexed());
    CORRADE_COMPARE(out.attributeCount(), 2);
    /* Second and third in each group swapped */
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS(out.attribute<UnsignedInt>(0), Containers::arrayView<UnsignedInt>({
        100, 102, 101,
        103, 105, 104
    }), TestSuite::Compare::Container);
    /* Second and third in each group swapped, no flipping, no format
       conversion */
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(1), Containers::arrayView<Vector3h>({
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h},
        {3.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}
    }), TestSuite::Compare::Container);
}

void FlipNormalsTest::flipFaceWindingMeshDataWrongPrimitiveIndexVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort indices[8];
    Trade::MeshData wrongPrimitive{MeshPrimitive::TriangleStrip, 6};
    Trade::MeshData wrongIndexCount{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices}, 6};
    Trade::MeshData wrongVertexCount{MeshPrimitive::Triangles, 7};

    Containers::String out;
    Error redirectError{&out};
    /* flipNormals() delegates to flipFaceWinding() for the assert,
       flipFaceWinding() then to flipFaceWindingInPlace() */
    MeshTools::flipFaceWinding(wrongPrimitive);
    MeshTools::flipNormals(wrongPrimitive);
    MeshTools::flipFaceWinding(wrongIndexCount);
    MeshTools::flipNormals(wrongIndexCount);
    MeshTools::flipFaceWinding(wrongVertexCount);
    MeshTools::flipNormals(wrongVertexCount);
    CORRADE_COMPARE_AS(out,
        "MeshTools::flipFaceWindingInPlace(): expected a MeshPrimitive::Triangles mesh but got MeshPrimitive::TriangleStrip\n"
        "MeshTools::flipFaceWindingInPlace(): expected a MeshPrimitive::Triangles mesh but got MeshPrimitive::TriangleStrip\n"
        "MeshTools::flipFaceWindingInPlace(): expected index count divisible by 3 but got 8\n"
        "MeshTools::flipFaceWindingInPlace(): expected index count divisible by 3 but got 8\n"
        "MeshTools::flipFaceWindingInPlace(): expected vertex count divisible by 3 but got 7\n"
        "MeshTools::flipFaceWindingInPlace(): expected vertex count divisible by 3 but got 7\n",
        TestSuite::Compare::String);
}

void FlipNormalsTest::flipFaceWindingMeshDataImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}}, 0};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipFaceWinding(mesh);
    CORRADE_COMPARE(out, "MeshTools::flipFaceWindingInPlace(): mesh has an implementation-specific index type 0xcaca\n");
}

void FlipNormalsTest::flipFaceWindingMeshDataImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* It should check all attributes, not just the first */
    Trade::MeshData mesh{MeshPrimitive::Triangles, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xcaca), nullptr},
    }};

    /* Indexed meshes are allowed to have implementation-specific attributes */
    MeshTools::flipFaceWinding(Trade::MeshData{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedInt, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xbebe), nullptr},
        }});

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipFaceWinding(mesh);
    CORRADE_COMPARE(out, "MeshTools::flipFaceWindingInPlace(): attribute 1 has an implementation-specific format 0xcaca\n");
}

void FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexed() {
    /* Like flipFaceWindingMeshDataIndexed() but using owned index and vertex
       arrays, to verify everything gets passed through */

    Containers::Array<char> indexData{NoInit, 6*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        0, 1, 2,
        3, 4, 5
    }, indices);

    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector3h)};
    auto vertices = Containers::arrayCast<Vector3h>(vertexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const Vector3h normalsExpected[]{
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}
    };
    Utility::copy(normalsExpected, vertices);

    /* Have to use ValueInit + copy to have an array with a default deleter
       because that's what copy() internally uses to distinguish whether the
       attributes are owned */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 1};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices};
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData out = MeshTools::flipFaceWinding(Trade::MeshData{MeshPrimitive::Triangles,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(0),
        Containers::arrayView(normalsExpected),
        TestSuite::Compare::Container);

    /* The memory should point to the original data */
    CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughNonIndexed() {
    /* Like flipFaceWindingMeshDataIndexed() but using an owned vertex array,
       to verify everything gets passed through */

    struct VertexData {
        UnsignedInt id[6];
        Vector3h normal[6];
    };
    Containers::Array<char> vertexData{NoInit, sizeof(VertexData)};
    auto vertices = Containers::arrayCast<VertexData>(vertexData);
    Utility::copy({{
        {100, 101, 102,
         103, 104, 105},
        {{0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h},
         {3.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}}
    }}, vertices);

    /* Have to use ValueInit + copy to have an array with a default deleter
       because that's what copy() internally uses to distinguish whether the
       attributes are owned */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 2};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertices[0].id)};
    attributes[1] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices[0].normal)};
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData out = MeshTools::flipFaceWinding(Trade::MeshData{MeshPrimitive::Triangles,
        Utility::move(vertexData), Utility::move(attributes)});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!out.isIndexed());
    CORRADE_COMPARE(out.attributeCount(), 2);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS(out.attribute<UnsignedInt>(0), Containers::arrayView<UnsignedInt>({
        100, 102, 101,
        103, 105, 104
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(1), Containers::arrayView<Vector3h>({
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h},
        {3.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}
    }), TestSuite::Compare::Container);

    /* The memory should point to the original data */
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned() {
    UnsignedShort indices[]{
        0, 1, 2,
        3, 4, 5
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector3h)};
    auto vertices = Containers::arrayCast<Vector3h>(vertexData);
    /* Not using copy({...}) as we'd have no way to compare against the
       original unchanged data */
    const Vector3h normalsExpected[]{
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}
    };
    Utility::copy(normalsExpected, vertices);

    /* Have to use ValueInit + copy to have an array with a default deleter
       because that's what copy() internally uses to distinguish whether the
       attributes are owned */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 1};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices};
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData out = MeshTools::flipFaceWinding(Trade::MeshData{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(0),
        Containers::arrayView(normalsExpected),
        TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the indices */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned() {
    Containers::Array<char> indexData{NoInit, 6*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        0, 1, 2,
        3, 4, 5
    }, indices);

    Vector3h vertices[]{
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}
    };

    Trade::MeshData out = MeshTools::flipFaceWinding(Trade::MeshData{MeshPrimitive::Triangles,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, vertices, {Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices)}}});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(0),
        Containers::arrayView(vertices),
        TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as that's an implementation
       detail in copy(). */
    CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
}

void FlipNormalsTest::flipFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned() {
    struct {
        UnsignedInt id[6];
        Vector3h normal[6];
    } vertices[]{{
        {100, 101, 102,
         103, 104, 105},
        {{0.0_h, 0.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h},
         {3.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}}
    }};
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, vertices, {
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertices->id)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices->normal)},
    }};

    Trade::MeshData out = MeshTools::flipFaceWinding(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!out.isIndexed());
    CORRADE_COMPARE(out.attributeCount(), 2);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS(out.attribute<UnsignedInt>(0), Containers::arrayView<UnsignedInt>({
        100, 102, 101,
        103, 105, 104
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3h);
    CORRADE_COMPARE_AS(out.attribute<Vector3h>(1), Containers::arrayView<Vector3h>({
        {0.0_h, 0.0_h, 0.0_h}, {0.0_h, 2.0_h, 0.0_h}, {0.0_h, 1.0_h, 0.0_h},
        {3.0_h, 0.0_h, 0.0_h}, {5.0_h, 0.0_h, 0.0_h}, {4.0_h, 0.0_h, 0.0_h}
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as that's an implementation
       detail in copy(). */
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
}

void FlipNormalsTest::flipFaceWindingMeshDataInPlaceIndexedIndexDataNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[3];
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, indices, Trade::MeshIndexData{indices}, 3};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipFaceWindingInPlace(mesh);
    /* flipNormalsInPlace() delegates to flipFaceWindingInPlace() so it should
       print the same message */
    MeshTools::flipNormalsInPlace(mesh);
    CORRADE_COMPARE(out,
        "MeshTools::flipFaceWindingInPlace(): index data not mutable\n"
        "MeshTools::flipFaceWindingInPlace(): index data not mutable\n");
}

void FlipNormalsTest::flipFaceWindingMeshDataInPlaceNonIndexedVertexDataNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 positions[3];
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)},
    }};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipFaceWindingInPlace(mesh);
    /* flipNormalsInPlace() delegates to flipFaceWindingInPlace() so it should
       print the same message */
    MeshTools::flipNormalsInPlace(mesh);
    CORRADE_COMPARE(out,
        "MeshTools::flipFaceWindingInPlace(): vertex data not mutable\n"
        "MeshTools::flipFaceWindingInPlace(): vertex data not mutable\n");
}

void FlipNormalsTest::flipNormals() {
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(normals);

    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void FlipNormalsTest::flipNormalsDeprecated() {
    std::vector<UnsignedInt> indices{0, 1, 2, 3, 4, 5};
    std::vector<Vector3> normals{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    CORRADE_IGNORE_DEPRECATED_PUSH
    MeshTools::flipNormals(indices, normals);
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<UnsignedInt>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}
#endif

template<class T> void FlipNormalsTest::flipNormalsFaceWinding() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(indices, normals);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void FlipNormalsTest::flipNormalsFaceWindingDeprecated() {
    std::vector<Vector3> normals{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    CORRADE_IGNORE_DEPRECATED_PUSH
    MeshTools::flipNormals(normals);
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}
#endif

template<class T> void FlipNormalsTest::flipNormalsFaceWindingErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(
        Containers::arrayCast<2, char>(Containers::stridedArrayView(indices)),
        normals);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}

template<class T> void FlipNormalsTest::flipNormalsFaceWindingMeshData() {
    auto&& data = FlipNormalsFaceWindingMeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Every second index is used to test propagation of InterleaveFlags */
    const UnsignedShort indices[]{
        0, 666, 1, 666, 2, 666,
        3, 666, 4, 666, 5, 666
    };
    /* Deliberately not interleaved to verify it doesn't work with just
       interleaved meshes */
    struct Vertex {
        UnsignedShort id;
        Vector3 secondaryNormal;
    };
    const struct {
        Vertex vertices[6];
        Math::Vector3<T> normals[6];
    } vertexData[]{{
        {{100, {0.0f, 0.0f, 6.0f}},
         {101, {0.0f, 0.0f, 7.0f}},
         {102, {0.0f, 0.0f, 8.0f}},
         {103, {1.0f, 1.0f, 1.0f}},
         {104, {2.0f, 2.0f, 2.0f}},
         {105, {3.0f, 3.0f, 3.0f}}},
        {{T(0.0), T(0.0), T(0.0)}, {T(0.0), T(1.0), T(0.0)}, {T(0.0), T(2.0), T(0.0)},
         {T(3.0), T(0.0), T(0.0)}, {T(4.0), T(0.0), T(0.0)}, {T(5.0), T(0.0), T(0.0)}}
    }};

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.normals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertexData->normals)});
    if(data.secondaryNormals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::stridedArrayView(vertexData->vertices).slice(&Vertex::secondaryNormal), data.secondaryNormalsMorphTarget});
    arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::stridedArrayView(vertexData->vertices).slice(&Vertex::id)});

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        {}, data.indexed ? Containers::arrayView(indices) : nullptr,
        data.indexed ? Trade::MeshIndexData{Containers::stridedArrayView(indices).every(2)} : Trade::MeshIndexData{nullptr},
        {}, vertexData, Utility::move(attributes)};
    CORRADE_COMPARE(mesh.isIndexed(), data.indexed);

    Trade::MeshData out = MeshTools::flipNormals(mesh, data.interleaveFlags);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(out.isIndexed(), data.indexed);
    CORRADE_COMPARE(out.attributeCount(), 1 + data.normals + data.secondaryNormals);

    /* If indexed, only the index buffer has second and third elements
       swapped */
    if(data.indexed) {
        CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
        /* If no format expansion happens, the index data is passed through
           unchanged, thus keeping the original stride */
        CORRADE_COMPARE(out.indexStride(), (std::is_same<T, Float>::value ? 4 : data.expectedIndexStrideWithExpandedVertexFormat));
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
            0, 2, 1,
            3, 5, 4
        }), TestSuite::Compare::Container);

        /* Non-normal attributes stay unchanged */
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::ObjectId), VertexFormat::UnsignedShort);
        CORRADE_COMPARE_AS(out.attribute<UnsignedShort>(Trade::MeshAttribute::ObjectId),
            Containers::stridedArrayView(vertexData->vertices).slice(&Vertex::id),
            TestSuite::Compare::Container);

        /* Normals get expanded to floats and flipped but not swapped */
        if(data.normals) {
            CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(0), -1);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -0.0f}, {-0.0f, -1.0f, -0.0f}, {-0.0f, -2.0f, -0.0f},
                {-3.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}
            }), TestSuite::Compare::Container);
        }
        if(data.secondaryNormals) {
            CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(1), data.secondaryNormalsMorphTarget);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(1), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -6.0f}, {-0.0f, -0.0f, -7.0f}, {-0.0f, -0.0f, -8.0f},
                {-1.0f, -1.0f, -1.0f}, {-2.0f, -2.0f, -2.0f}, {-3.0f, -3.0f, -3.0f}
            }), TestSuite::Compare::Container);
        }

    /* If non-indexed, the attributes get swapped as well */
    } else {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::ObjectId), VertexFormat::UnsignedShort);
        CORRADE_COMPARE_AS(out.attribute<UnsignedShort>(Trade::MeshAttribute::ObjectId), Containers::arrayView<UnsignedShort>({
            100, 102, 101,
            103, 105, 104
        }), TestSuite::Compare::Container);

        /* Normals get expanded to floats and flipped but not swapped */
        if(data.normals) {
            CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(0), -1);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -0.0f}, {-0.0f, -2.0f, -0.0f}, {-0.0f, -1.0f, -0.0f},
                {-3.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}
            }), TestSuite::Compare::Container);
        }
        if(data.secondaryNormals) {
            CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(1), data.secondaryNormalsMorphTarget);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(1), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -6.0f}, {-0.0f, -0.0f, -8.0f}, {-0.0f, -0.0f, -7.0f},
                {-1.0f, -1.0f, -1.0f}, {-3.0f, -3.0f, -3.0f}, {-2.0f, -2.0f, -2.0f}
            }), TestSuite::Compare::Container);
        }
    }
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}}, 0};
    /* In case normal vertex format needs expansion it has to assert early
       enough to not cause assertion in interleave() */
    Trade::MeshData needsFormatExpansion{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xbebe), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, nullptr},
        }};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipNormals(mesh);
    MeshTools::flipNormals(needsFormatExpansion);
    CORRADE_COMPARE_AS(out,
        "MeshTools::flipNormals(): mesh has an implementation-specific index type 0xcaca\n"
        "MeshTools::flipNormals(): mesh has an implementation-specific index type 0xbebe\n",
        TestSuite::Compare::String);
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* For non-indexed meshes it should check all attributes, not just the
       first */
    Trade::MeshData mesh{MeshPrimitive::Triangles, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xcaca), nullptr},
    }};
    /* In case normal vertex format needs expansion it has to assert early
       enough to not cause assertion in interleave(), for both indexed and
       non-indexed meshes. */
    Trade::MeshData needsFormatExpansion{MeshPrimitive::Triangles,
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xbebe), nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, nullptr},
        }};
    Trade::MeshData needsFormatExpansionIndexed{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedInt, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xfafa), nullptr},
        }};

    /* Indexed meshes with no normals or Vector3 normals are allowed to have
       implementation-specific attributes */
    MeshTools::flipNormals(Trade::MeshData{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedInt, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xbebe), nullptr},
        }});
    MeshTools::flipNormals(Trade::MeshData{MeshPrimitive::Triangles,
        nullptr, Trade::MeshIndexData{MeshIndexType::UnsignedInt, nullptr},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertexFormatWrap(0xbebe), nullptr},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr},
        }});

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipNormals(mesh);
    MeshTools::flipNormals(needsFormatExpansion);
    MeshTools::flipNormals(needsFormatExpansionIndexed);
    CORRADE_COMPARE_AS(out,
        "MeshTools::flipNormals(): attribute 1 has an implementation-specific format 0xcaca\n"
        "MeshTools::flipNormals(): attribute 0 has an implementation-specific format 0xbebe\n"
        "MeshTools::flipNormals(): attribute 1 has an implementation-specific format 0xfafa\n",
        TestSuite::Compare::String);
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthrough() {
    auto&& data = FlipNormalsFaceWindingMeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 6*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        0, 1, 2,
        3, 4, 5
    }, indices);

    struct Vertex {
        UnsignedShort id;
        Vector3 secondaryNormal;
    };
    struct VertexData {
        Vertex vertices[6];
        Vector3 normals[6];
    };
    Containers::Array<char> vertexData{NoInit, sizeof(VertexData)};
    auto vertices = Containers::arrayCast<VertexData>(vertexData);
    Utility::copy({{
        {{100, {0.0f, 0.0f, 6.0f}},
         {101, {0.0f, 0.0f, 7.0f}},
         {102, {0.0f, 0.0f, 8.0f}},
         {103, {1.0f, 1.0f, 1.0f}},
         {104, {2.0f, 2.0f, 2.0f}},
         {105, {3.0f, 3.0f, 3.0f}}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 2.0f, 0.0f},
         {3.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 0.0f}, {5.0f, 0.0f, 0.0f}}
    }}, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    if(data.normals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices[0].normals)});
    if(data.secondaryNormals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::stridedArrayView(vertices[0].vertices).slice(&Vertex::secondaryNormal), data.secondaryNormalsMorphTarget});
    arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::stridedArrayView(vertices[0].vertices).slice(&Vertex::id)});
    /* Just to verify that implementation-specific formats don't affect the
       indexed case as neither any format conversion happens nor the vertices
       need to get reordered */
    if(data.indexed)
       arrayAppend(attributes, Trade::MeshAttributeData{Trade::meshAttributeCustom(35), vertexFormatWrap(0xcaca), Containers::arrayView(vertices[0].normals)});
    /* Have to convert the attributes back to a non-growable array with a
       default deleter because that's what copy() internally uses to
       distinguish whether the attributes are owned */
    arrayShrink(attributes, ValueInit);
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        data.indexed ? Utility::move(indexData) : nullptr,
        data.indexed ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{nullptr},
        Utility::move(vertexData), Utility::move(attributes)};
    CORRADE_COMPARE(mesh.isIndexed(), data.indexed);

    Trade::MeshData out = MeshTools::flipNormals(Utility::move(mesh));
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(out.isIndexed(), data.indexed);
    CORRADE_COMPARE(out.attributeCount(), 1 + data.normals + data.secondaryNormals + data.indexed);

    /* If indexed, only the index buffer has second and third elements
       swapped */
    if(data.indexed) {
        CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
            0, 2, 1,
            3, 5, 4
        }), TestSuite::Compare::Container);

        /* Non-normal attributes stay unchanged */
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::ObjectId), VertexFormat::UnsignedShort);
        CORRADE_COMPARE_AS(out.attribute<UnsignedShort>(Trade::MeshAttribute::ObjectId),
            Containers::stridedArrayView(vertices[0].vertices).slice(&Vertex::id),
            TestSuite::Compare::Container);

        /* Normals get expanded to floats and flipped but not swapped */
        if(data.normals) {
            CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(0), -1);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -0.0f}, {-0.0f, -1.0f, -0.0f}, {-0.0f, -2.0f, -0.0f},
                {-3.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}
            }), TestSuite::Compare::Container);
        }
        if(data.secondaryNormals) {
            CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(1), data.secondaryNormalsMorphTarget);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(1), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -6.0f}, {-0.0f, -0.0f, -7.0f}, {-0.0f, -0.0f, -8.0f},
                {-1.0f, -1.0f, -1.0f}, {-2.0f, -2.0f, -2.0f}, {-3.0f, -3.0f, -3.0f}
            }), TestSuite::Compare::Container);
        }

        /* Just to verify that the implementation-specific attribute didn't get
           lost */
        CORRADE_COMPARE(out.attributeFormat(1 + data.normals + data.secondaryNormals), vertexFormatWrap(0xcaca));

    /* If non-indexed, the attributes get swapped as well */
    } else {
        CORRADE_COMPARE(out.attributeFormat(Trade::MeshAttribute::ObjectId), VertexFormat::UnsignedShort);
        CORRADE_COMPARE_AS(out.attribute<UnsignedShort>(Trade::MeshAttribute::ObjectId), Containers::arrayView<UnsignedShort>({
            100, 102, 101,
            103, 105, 104
        }), TestSuite::Compare::Container);

        /* Normals get expanded to floats and flipped but not swapped */
        if(data.normals) {
            CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(0), -1);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -0.0f}, {-0.0f, -2.0f, -0.0f}, {-0.0f, -1.0f, -0.0f},
                {-3.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}
            }), TestSuite::Compare::Container);
        }
        if(data.secondaryNormals) {
            CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3);
            CORRADE_COMPARE(out.attributeMorphTargetId(1), data.secondaryNormalsMorphTarget);
            CORRADE_COMPARE_AS(out.attribute<Vector3>(1), Containers::arrayView<Vector3>({
                {-0.0f, -0.0f, -6.0f}, {-0.0f, -0.0f, -8.0f}, {-0.0f, -0.0f, -7.0f},
                {-1.0f, -1.0f, -1.0f}, {-3.0f, -3.0f, -3.0f}, {-2.0f, -2.0f, -2.0f}
            }), TestSuite::Compare::Container);
        }
    }

    /* The memory should point to the original data */
    if(data.indexed)
        CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned() {
    /* Like flipFaceWindingMeshDataRvaluePassthroughIndexedIndexDataNotOwned()
       but this time verifying that the normals get flipped as well */

    UnsignedShort indices[]{
        0, 1, 2,
        3, 4, 5
    };
    Containers::Array<char> vertexData{NoInit, 3*sizeof(Vector3)};
    auto vertices = Containers::arrayCast<Vector3>(vertexData);
    Utility::copy({
        {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 2.0f, 0.0f},
    }, vertices);

    /* Have to use ValueInit + copy to have an array with a default deleter
       because that's what copy() internally uses to distinguish whether the
       attributes are owned */
    Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, 1};
    attributes[0] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices};
    CORRADE_VERIFY(!attributes.deleter());
    const void* originalAttributeData = attributes.data();

    Trade::MeshData out = MeshTools::flipNormals(Trade::MeshData{MeshPrimitive::Triangles,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
        {-0.0f, -0.0f, -0.0f}, {-0.0f, -1.0f, -0.0f}, {-0.0f, -2.0f, -0.0f},
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the indices */
    CORRADE_VERIFY(out.indexData().data() != static_cast<const void*>(indices));
    CORRADE_COMPARE(out.vertexData().data(), static_cast<void*>(vertices.data()));
    CORRADE_COMPARE(out.attributeData().data(), originalAttributeData);
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned() {
    /* Like flipFaceWindingMeshDataRvaluePassthroughIndexedVertexDataNotOwned()
       but this time verifying that the normals get flipped as well */

    Containers::Array<char> indexData{NoInit, 6*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        0, 1, 2,
        3, 4, 5
    }, indices);

    Vector3 vertices[]{
        {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 2.0f, 0.0f}
    };

    Trade::MeshData out = MeshTools::flipNormals(Trade::MeshData{MeshPrimitive::Triangles,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        {}, vertices, {Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices)}}});
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeCount(), 1);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
        {-0.0f, -0.0f, -0.0f}, {-0.0f, -1.0f, -0.0f}, {-0.0f, -2.0f, -0.0f},
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as that's an implementation
       detail in copy(). */
    CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned() {
    /* Like flipFaceWindingMeshDataRvaluePassthroughNonIndexedVertexDataNotOwned()
       but this time verifying that the normals get flipped as well */

    struct {
        UnsignedInt id[6];
        Vector3 normal[6];
    } vertices[]{{
        {100, 101, 102,
         103, 104, 105},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 2.0f, 0.0f},
         {3.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 0.0f}, {5.0f, 0.0f, 0.0f}}
    }};
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, vertices, {
        Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(vertices->id)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertices->normal)},
    }};

    Trade::MeshData out = MeshTools::flipNormals(mesh);
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!out.isIndexed());
    CORRADE_COMPARE(out.attributeCount(), 2);
    CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS(out.attribute<UnsignedInt>(0), Containers::arrayView<UnsignedInt>({
        100, 102, 101,
        103, 105, 104
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(1), Containers::arrayView<Vector3>({
        {-0.0f, -0.0f, -0.0f}, {-0.0f, -2.0f, -0.0f}, {-0.0f, -1.0f, -0.0f},
        {-3.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as that's an implementation
       detail in copy(). */
    CORRADE_VERIFY(out.vertexData().data() != static_cast<const void*>(vertices));
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormat() {
    auto&& data = FlipNormalsFaceWindingMeshDataRvaluePassthroughNormalDataWrongFormatData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Array<char> indexData{NoInit, 6*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({
        0, 1, 2,
        3, 4, 5
    }, indices);

    struct Vertex {
        Vector3 secondaryNormal;
        Vector3h normal;
    };
    Containers::Array<char> vertexData{NoInit, 6*sizeof(Vertex)};
    Containers::StridedArrayView1D<Vertex> vertices = Containers::arrayCast<Vertex>(vertexData);
    Utility::copy({
        {{0.0f, 0.0f, 6.0f}, {0.0_h, 0.0_h, 0.0_h}},
        {{0.0f, 0.0f, 7.0f}, {0.0_h, 1.0_h, 0.0_h}},
        {{0.0f, 0.0f, 8.0f}, {0.0_h, 2.0_h, 0.0_h}},
        {{1.0f, 1.0f, 1.0f}, {3.0_h, 0.0_h, 0.0_h}},
        {{2.0f, 2.0f, 2.0f}, {4.0_h, 0.0_h, 0.0_h}},
        {{3.0f, 3.0f, 3.0f}, {5.0_h, 0.0_h, 0.0_h}},
    }, vertices);

    Containers::Array<Trade::MeshAttributeData> attributes;
    /* Secondary normals, if present, are first, to verify that it doesn't
       check just the very first normal attribute */
    if(data.secondaryNormals)
        arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices.slice(&Vertex::secondaryNormal)});
    arrayAppend(attributes, Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertices.slice(&Vertex::normal), data.morphTargetId});

    Trade::MeshData mesh{MeshPrimitive::Triangles,
        Utility::move(indexData), Trade::MeshIndexData{indices},
        Utility::move(vertexData), Utility::move(attributes)};

    Trade::MeshData out = MeshTools::flipNormals(Utility::move(mesh));
    CORRADE_COMPARE(out.primitive(), MeshPrimitive::Triangles);

    CORRADE_VERIFY(out.isIndexed());
    CORRADE_COMPARE(out.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(out.indices<UnsignedShort>(), Containers::arrayView<UnsignedShort>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(out.attributeCount(), 1 + data.secondaryNormals);
    if(data.secondaryNormals) {
        CORRADE_COMPARE(out.attributeFormat(0), VertexFormat::Vector3);
        CORRADE_COMPARE_AS(out.attribute<Vector3>(0), Containers::arrayView<Vector3>({
            {-0.0f, -0.0f, -6.0f}, {-0.0f, -0.0f, -7.0f}, {-0.0f, -0.0f, -8.0f},
            {-1.0f, -1.0f, -1.0f}, {-2.0f, -2.0f, -2.0f}, {-3.0f, -3.0f, -3.0f}
        }), TestSuite::Compare::Container);
    }
    CORRADE_COMPARE(out.attributeFormat(data.secondaryNormals ? 1 : 0), VertexFormat::Vector3);
    CORRADE_COMPARE(out.attributeMorphTargetId(data.secondaryNormals ? 1 : 0), data.morphTargetId);
    CORRADE_COMPARE_AS(out.attribute<Vector3>(data.secondaryNormals ? 1 : 0), Containers::arrayView<Vector3>({
        {-0.0f, -0.0f, -0.0f}, {-0.0f, -1.0f, -0.0f}, {-0.0f, -2.0f, -0.0f},
        {-3.0f, -0.0f, -0.0f}, {-4.0f, -0.0f, -0.0f}, {-5.0f, -0.0f, -0.0f}
    }), TestSuite::Compare::Container);

    /* The memory should *not* point to the original data for the vertices. Not
       verifying the behavior for attribute data as those are copied in this
       case always. */
    CORRADE_COMPARE(out.indexData().data(), static_cast<void*>(indices.data()));
    CORRADE_VERIFY(out.vertexData().data() != static_cast<void*>(vertices.data()));
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceIndexedVertexDataNotMutable() {
    /* Should work even though the vertex data are not mutable as there are no
       normals to change */
    UnsignedByte indices[]{
        0, 1, 2,
        3, 4, 5
    };
    UnsignedShort ids[4];
    Trade::MeshData mesh{MeshPrimitive::Triangles,
        Trade::DataFlag::Mutable, indices, Trade::MeshIndexData{indices},
        {}, ids, {
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, Containers::arrayView(ids)},
        }};
    MeshTools::flipNormalsInPlace(mesh);
    CORRADE_COMPARE_AS(mesh.indices<UnsignedByte>(), Containers::arrayView<UnsignedByte>({
        0, 2, 1,
        3, 5, 4
    }), TestSuite::Compare::Container);
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceNormalDataNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 normals[3];
    Trade::MeshData mesh{MeshPrimitive::Triangles, {}, normals, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)},
    }};

    Containers::String out;
    Error redirectError{&out};
    /* Index / non-indexed vertex data not mutable tested along with
       flipFaceWindingInPlace() */
    MeshTools::flipNormalsInPlace(mesh);
    CORRADE_COMPARE(out, "MeshTools::flipNormalsInPlace(): vertex data not mutable\n");
}

void FlipNormalsTest::flipNormalsFaceWindingMeshDataInPlaceNormalDataWrongFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* It should check all normal attributes, not just the first */
    Vector3 normals[3];
    Trade::MeshData mesh{MeshPrimitive::Triangles, Trade::DataFlag::Mutable, normals, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3bNormalized, Containers::arrayView(normals)},
    }};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::flipNormalsInPlace(mesh);
    CORRADE_COMPARE(out, "MeshTools::flipNormalsInPlace(): expected VertexFormat::Vector3 normals but got VertexFormat::Vector3bNormalized for attribute 1\n");
}
}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FlipNormalsTest)
