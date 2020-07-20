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
#include "Magnum/Math/Half.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct MeshDataTest: TestSuite::Tester {
    explicit MeshDataTest();

    void customAttributeName();
    void customAttributeNameTooLarge();
    void customAttributeNameNotCustom();
    void debugAttributeName();

    void constructIndex();
    void constructIndexTypeErased();
    void constructIndexTypeErasedWrongSize();
    void constructIndex2D();
    void constructIndex2DNotIndexed();
    void constructIndex2DWrongSize();
    void constructIndex2DNonContiguous();
    void constructIndexNullptr();

    void constructAttribute();
    void constructAttributeDefault();
    void constructAttributeCustom();
    void constructAttribute2D();
    void constructAttribute2DWrongSize();
    void constructAttribute2DNonContiguous();
    void constructAttributeTypeErased();
    void constructAttributeNullptr();
    void constructAttributePadding();
    void constructAttributeNonOwningArray();
    void constructAttributeOffsetOnly();
    void constructAttributeImplementationSpecificFormat();
    void constructAttributeWrongFormat();
    void constructAttributeWrongStride();
    void constructAttributeWrongDataAccess();

    void constructArrayAttribute();
    void constructArrayAttributeNonContiguous();
    void constructArrayAttribute2D();
    void constructArrayAttribute2DWrongSize();
    void constructArrayAttribute2DNonContiguous();
    void constructArrayAttributeTypeErased();
    void constructArrayAttributeNullptr();
    void constructArrayAttributeOffsetOnly();
    void constructArrayAttributeNotAllowed();

    void construct();

    void constructZeroIndices();
    void constructZeroAttributes();
    void constructZeroVertices();
    void constructIndexless();
    void constructIndexlessZeroVertices();
    void constructAttributeless();
    void constructIndexlessAttributeless();
    void constructIndexlessAttributelessZeroVertices();

    void constructNotOwned();
    void constructIndicesNotOwned();
    void constructVerticesNotOwned();
    void constructIndexlessNotOwned();
    void constructAttributelessNotOwned();

    void constructIndexDataButNotIndexed();
    void constructAttributelessImplicitVertexCount();
    void constructIndicesNotContained();
    void constructAttributeNotContained();
    void constructInconsitentVertexCount();
    void constructNotOwnedIndexFlagOwned();
    void constructNotOwnedVertexFlagOwned();
    void constructIndicesNotOwnedFlagOwned();
    void constructVerticesNotOwnedFlagOwned();
    void constructIndexlessNotOwnedFlagOwned();
    void constructAttributelessNotOwnedFlagOwned();
    void constructInvalidAttributeData();

    void constructCopy();
    void constructMove();

    template<class T> void indicesAsArray();
    void indicesIntoArrayInvalidSize();
    template<class T> void positions2DAsArray();
    template<class T> void positions2DAsArrayPackedUnsigned();
    template<class T> void positions2DAsArrayPackedSigned();
    template<class T> void positions2DAsArrayPackedUnsignedNormalized();
    template<class T> void positions2DAsArrayPackedSignedNormalized();
    void positions2DIntoArrayInvalidSize();
    template<class T> void positions3DAsArray();
    template<class T> void positions3DAsArrayPackedUnsigned();
    template<class T> void positions3DAsArrayPackedSigned();
    template<class T> void positions3DAsArrayPackedUnsignedNormalized();
    template<class T> void positions3DAsArrayPackedSignedNormalized();
    void positions3DIntoArrayInvalidSize();
    template<class T> void tangentsAsArray();
    template<class T> void tangentsAsArrayPackedSignedNormalized();
    void tangentsIntoArrayInvalidSize();
    template<class T> void bitangentSignsAsArray();
    template<class T> void bitangentSignsAsArrayPackedSignedNormalized();
    void bitangentSignsAsArrayNotFourComponent();
    void bitangentSignsIntoArrayInvalidSize();
    template<class T> void bitangentsAsArray();
    template<class T> void bitangentsAsArrayPackedSignedNormalized();
    void bitangentsIntoArrayInvalidSize();
    template<class T> void normalsAsArray();
    template<class T> void normalsAsArrayPackedSignedNormalized();
    void normalsIntoArrayInvalidSize();
    template<class T> void textureCoordinates2DAsArray();
    template<class T> void textureCoordinates2DAsArrayPackedUnsigned();
    template<class T> void textureCoordinates2DAsArrayPackedSigned();
    template<class T> void textureCoordinates2DAsArrayPackedUnsignedNormalized();
    template<class T> void textureCoordinates2DAsArrayPackedSignedNormalized();
    void textureCoordinates2DIntoArrayInvalidSize();
    template<class T> void colorsAsArray();
    template<class T> void colorsAsArrayPackedUnsignedNormalized();
    void colorsIntoArrayInvalidSize();
    template<class T> void objectIdsAsArray();
    void objectIdsIntoArrayInvalidSize();

    void implementationSpecificVertexFormat();
    void implementationSpecificVertexFormatWrongAccess();
    void implementationSpecificVertexFormatNotContained();

    void arrayAttribute();
    void arrayAttributeWrongAccess();

    void mutableAccessNotAllowed();

    void indicesNotIndexed();
    void indicesWrongType();

    void attributeNotFound();
    void attributeWrongType();

    void releaseIndexData();
    void releaseAttributeData();
    void releaseVertexData();
};

const struct {
    const char* name;
    UnsignedInt vertexCount, expectedVertexCount;
} ConstructData[] {
    {"implicit vertex count", MeshData::ImplicitVertexCount, 3},
    {"explicit vertex count", 3, 3},
    {"explicit large vertex count", 17, 17},
    {"explicit zero vertex count", 0, 0}
};

const struct {
    const char* name;
    DataFlags indexDataFlags, vertexDataFlags;
} NotOwnedData[] {
    {"", {}, {}},
    {"indices mutable", DataFlag::Mutable, {}},
    {"vertices mutable", {}, DataFlag::Mutable},
    {"both mutable", DataFlag::Mutable, DataFlag::Mutable}
};

const struct {
    const char* name;
    DataFlags dataFlags;
} SingleNotOwnedData[] {
    {"", {}},
    {"mutable", DataFlag::Mutable}
};

MeshDataTest::MeshDataTest() {
    addTests({&MeshDataTest::customAttributeName,
              &MeshDataTest::customAttributeNameTooLarge,
              &MeshDataTest::customAttributeNameNotCustom,
              &MeshDataTest::debugAttributeName,

              &MeshDataTest::constructIndex,
              &MeshDataTest::constructIndexTypeErased,
              &MeshDataTest::constructIndexTypeErasedWrongSize,
              &MeshDataTest::constructIndex2D,
              &MeshDataTest::constructIndex2DNotIndexed,
              &MeshDataTest::constructIndex2DWrongSize,
              &MeshDataTest::constructIndex2DNonContiguous,
              &MeshDataTest::constructIndexNullptr,

              &MeshDataTest::constructAttribute,
              &MeshDataTest::constructAttributeDefault,
              &MeshDataTest::constructAttributeCustom,
              &MeshDataTest::constructAttribute2D,
              &MeshDataTest::constructAttribute2DWrongSize,
              &MeshDataTest::constructAttribute2DNonContiguous,
              &MeshDataTest::constructAttributeTypeErased,
              &MeshDataTest::constructAttributeNullptr,
              &MeshDataTest::constructAttributePadding,
              &MeshDataTest::constructAttributeNonOwningArray,
              &MeshDataTest::constructAttributeOffsetOnly,
              &MeshDataTest::constructAttributeImplementationSpecificFormat,
              &MeshDataTest::constructAttributeWrongFormat,
              &MeshDataTest::constructAttributeWrongStride,
              &MeshDataTest::constructAttributeWrongDataAccess,

              &MeshDataTest::constructArrayAttribute,
              &MeshDataTest::constructArrayAttributeNonContiguous,
              &MeshDataTest::constructArrayAttribute2D,
              &MeshDataTest::constructArrayAttribute2DWrongSize,
              &MeshDataTest::constructArrayAttribute2DNonContiguous,
              &MeshDataTest::constructArrayAttributeTypeErased,
              &MeshDataTest::constructArrayAttributeNullptr,
              &MeshDataTest::constructArrayAttributeOffsetOnly,
              &MeshDataTest::constructArrayAttributeNotAllowed});

    addInstancedTests({&MeshDataTest::construct},
        Containers::arraySize(ConstructData));

    addTests({&MeshDataTest::constructZeroIndices,
              &MeshDataTest::constructZeroAttributes,
              &MeshDataTest::constructZeroVertices,
              &MeshDataTest::constructIndexless,
              &MeshDataTest::constructIndexlessZeroVertices,
              &MeshDataTest::constructAttributeless,
              &MeshDataTest::constructIndexlessAttributeless,
              &MeshDataTest::constructIndexlessAttributelessZeroVertices});

    addInstancedTests({&MeshDataTest::constructNotOwned},
        Containers::arraySize(NotOwnedData));
    addInstancedTests({&MeshDataTest::constructIndicesNotOwned,
                       &MeshDataTest::constructVerticesNotOwned,
                       &MeshDataTest::constructIndexlessNotOwned,
                       &MeshDataTest::constructAttributelessNotOwned},
        Containers::arraySize(SingleNotOwnedData));

    addTests({&MeshDataTest::constructIndexDataButNotIndexed,
              &MeshDataTest::constructAttributelessImplicitVertexCount,
              &MeshDataTest::constructIndicesNotContained,
              &MeshDataTest::constructAttributeNotContained,
              &MeshDataTest::constructInconsitentVertexCount,
              &MeshDataTest::constructNotOwnedIndexFlagOwned,
              &MeshDataTest::constructNotOwnedVertexFlagOwned,
              &MeshDataTest::constructIndicesNotOwnedFlagOwned,
              &MeshDataTest::constructVerticesNotOwnedFlagOwned,
              &MeshDataTest::constructIndexlessNotOwnedFlagOwned,
              &MeshDataTest::constructAttributelessNotOwnedFlagOwned,
              &MeshDataTest::constructInvalidAttributeData,

              &MeshDataTest::constructCopy,
              &MeshDataTest::constructMove,

              &MeshDataTest::indicesAsArray<UnsignedByte>,
              &MeshDataTest::indicesAsArray<UnsignedShort>,
              &MeshDataTest::indicesAsArray<UnsignedInt>,
              &MeshDataTest::indicesIntoArrayInvalidSize,
              &MeshDataTest::positions2DAsArray<Vector2>,
              &MeshDataTest::positions2DAsArray<Vector2h>,
              &MeshDataTest::positions2DAsArray<Vector3>,
              &MeshDataTest::positions2DAsArray<Vector3h>,
              &MeshDataTest::positions2DAsArrayPackedUnsigned<Vector2ub>,
              &MeshDataTest::positions2DAsArrayPackedUnsigned<Vector2us>,
              &MeshDataTest::positions2DAsArrayPackedUnsigned<Vector3ub>,
              &MeshDataTest::positions2DAsArrayPackedUnsigned<Vector3us>,
              &MeshDataTest::positions2DAsArrayPackedSigned<Vector2b>,
              &MeshDataTest::positions2DAsArrayPackedSigned<Vector2s>,
              &MeshDataTest::positions2DAsArrayPackedSigned<Vector3b>,
              &MeshDataTest::positions2DAsArrayPackedSigned<Vector3s>,
              &MeshDataTest::positions2DAsArrayPackedUnsignedNormalized<Vector2ub>,
              &MeshDataTest::positions2DAsArrayPackedUnsignedNormalized<Vector2us>,
              &MeshDataTest::positions2DAsArrayPackedUnsignedNormalized<Vector3ub>,
              &MeshDataTest::positions2DAsArrayPackedUnsignedNormalized<Vector3us>,
              &MeshDataTest::positions2DAsArrayPackedSignedNormalized<Vector2b>,
              &MeshDataTest::positions2DAsArrayPackedSignedNormalized<Vector2s>,
              &MeshDataTest::positions2DAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::positions2DAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::positions2DIntoArrayInvalidSize,
              &MeshDataTest::positions3DAsArray<Vector2>,
              &MeshDataTest::positions3DAsArray<Vector2h>,
              &MeshDataTest::positions3DAsArray<Vector3>,
              &MeshDataTest::positions3DAsArray<Vector3h>,
              &MeshDataTest::positions3DAsArrayPackedUnsigned<Vector2ub>,
              &MeshDataTest::positions3DAsArrayPackedUnsigned<Vector2us>,
              &MeshDataTest::positions3DAsArrayPackedUnsigned<Vector3ub>,
              &MeshDataTest::positions3DAsArrayPackedUnsigned<Vector3us>,
              &MeshDataTest::positions3DAsArrayPackedSigned<Vector2b>,
              &MeshDataTest::positions3DAsArrayPackedSigned<Vector2s>,
              &MeshDataTest::positions3DAsArrayPackedSigned<Vector3b>,
              &MeshDataTest::positions3DAsArrayPackedSigned<Vector3s>,
              &MeshDataTest::positions3DAsArrayPackedUnsignedNormalized<Vector2ub>,
              &MeshDataTest::positions3DAsArrayPackedUnsignedNormalized<Vector2us>,
              &MeshDataTest::positions3DAsArrayPackedUnsignedNormalized<Vector3ub>,
              &MeshDataTest::positions3DAsArrayPackedUnsignedNormalized<Vector3us>,
              &MeshDataTest::positions3DAsArrayPackedSignedNormalized<Vector2b>,
              &MeshDataTest::positions3DAsArrayPackedSignedNormalized<Vector2s>,
              &MeshDataTest::positions3DAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::positions3DAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::positions3DIntoArrayInvalidSize,
              &MeshDataTest::tangentsAsArray<Vector3>,
              &MeshDataTest::tangentsAsArray<Vector3h>,
              &MeshDataTest::tangentsAsArray<Vector4>,
              &MeshDataTest::tangentsAsArray<Vector4h>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector4b>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector4s>,
              &MeshDataTest::tangentsIntoArrayInvalidSize,
              &MeshDataTest::bitangentSignsAsArray<Float>,
              &MeshDataTest::bitangentSignsAsArray<Half>,
              &MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized<Byte>,
              &MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized<Short>,
              &MeshDataTest::bitangentSignsAsArrayNotFourComponent,
              &MeshDataTest::bitangentSignsIntoArrayInvalidSize,
              &MeshDataTest::bitangentsAsArray<Vector3>,
              &MeshDataTest::bitangentsAsArray<Vector3h>,
              &MeshDataTest::bitangentsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::bitangentsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::bitangentsIntoArrayInvalidSize,
              &MeshDataTest::normalsAsArray<Vector3>,
              &MeshDataTest::normalsAsArray<Vector3h>,
              &MeshDataTest::normalsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::normalsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::normalsIntoArrayInvalidSize,
              &MeshDataTest::textureCoordinates2DAsArray<Vector2>,
              &MeshDataTest::textureCoordinates2DAsArray<Vector2h>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned<Vector2ub>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned<Vector2us>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSigned<Vector2b>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSigned<Vector2s>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized<Vector2ub>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized<Vector2us>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized<Vector2b>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized<Vector2s>,
              &MeshDataTest::textureCoordinates2DIntoArrayInvalidSize,
              &MeshDataTest::colorsAsArray<Color3>,
              &MeshDataTest::colorsAsArray<Color3h>,
              &MeshDataTest::colorsAsArray<Color4>,
              &MeshDataTest::colorsAsArray<Color4h>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color3ub>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color3us>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color4ub>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color4us>,
              &MeshDataTest::colorsIntoArrayInvalidSize,
              &MeshDataTest::objectIdsAsArray<UnsignedByte>,
              &MeshDataTest::objectIdsAsArray<UnsignedShort>,
              &MeshDataTest::objectIdsAsArray<UnsignedInt>,
              &MeshDataTest::objectIdsIntoArrayInvalidSize,

              &MeshDataTest::implementationSpecificVertexFormat,
              &MeshDataTest::implementationSpecificVertexFormatWrongAccess,
              &MeshDataTest::implementationSpecificVertexFormatNotContained,

              &MeshDataTest::arrayAttribute,
              &MeshDataTest::arrayAttributeWrongAccess,

              &MeshDataTest::mutableAccessNotAllowed,

              &MeshDataTest::indicesNotIndexed,
              &MeshDataTest::indicesWrongType,

              &MeshDataTest::attributeNotFound,
              &MeshDataTest::attributeWrongType,

              &MeshDataTest::releaseIndexData,
              &MeshDataTest::releaseAttributeData,
              &MeshDataTest::releaseVertexData});
}

void MeshDataTest::customAttributeName() {
    CORRADE_VERIFY(!isMeshAttributeCustom(MeshAttribute::Position));
    CORRADE_VERIFY(!isMeshAttributeCustom(MeshAttribute(32767)));
    CORRADE_VERIFY(isMeshAttributeCustom(MeshAttribute::Custom));
    CORRADE_VERIFY(isMeshAttributeCustom(MeshAttribute(65535)));

    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(0)), 32768);
    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(8290)), 41058);
    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(32767)), 65535);

    CORRADE_COMPARE(meshAttributeCustom(MeshAttribute::Custom), 0);
    CORRADE_COMPARE(meshAttributeCustom(MeshAttribute(41058)), 8290);
    CORRADE_COMPARE(meshAttributeCustom(MeshAttribute(65535)), 32767);

    constexpr bool is = isMeshAttributeCustom(MeshAttribute(41058));
    CORRADE_VERIFY(is);
    constexpr MeshAttribute a = meshAttributeCustom(8290);
    CORRADE_COMPARE(UnsignedShort(a), 41058);
    constexpr UnsignedShort b = meshAttributeCustom(a);
    CORRADE_COMPARE(b, 8290);
}

void MeshDataTest::customAttributeNameTooLarge() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    meshAttributeCustom(32768);
    CORRADE_COMPARE(out.str(), "Trade::meshAttributeCustom(): index 32768 too large\n");
}

void MeshDataTest::customAttributeNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    meshAttributeCustom(MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(out.str(), "Trade::meshAttributeCustom(): Trade::MeshAttribute::TextureCoordinates is not custom\n");
}

void MeshDataTest::debugAttributeName() {
    std::ostringstream out;
    Debug{&out} << MeshAttribute::Position << meshAttributeCustom(73) << MeshAttribute(0x73);
    CORRADE_COMPARE(out.str(), "Trade::MeshAttribute::Position Trade::MeshAttribute::Custom(73) Trade::MeshAttribute(0x73)\n");
}

using namespace Math::Literals;

constexpr UnsignedByte IndexBytes[]{25, 132, 3};
constexpr UnsignedShort IndexShorts[]{2575, 13224, 3};
constexpr UnsignedInt IndexInts[]{2110122, 132257, 3};

void MeshDataTest::constructIndex() {
    {
        const UnsignedByte indexData[]{25, 132, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(indices.data().data(), indexData);

        constexpr MeshIndexData cindices{IndexBytes};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::ArrayView<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(data.data(), IndexBytes);
    } {
        const UnsignedShort indexData[]{2575, 13224, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(indices.data().data(), indexData);

        constexpr MeshIndexData cindices{IndexShorts};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::ArrayView<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(data.data(), IndexShorts);
    } {
        const UnsignedInt indexData[]{2110122, 132257, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(indices.data().data(), indexData);

        constexpr MeshIndexData cindices{IndexInts};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::ArrayView<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(data.data(), IndexInts);
    }
}

void MeshDataTest::constructIndexTypeErased() {
    const char indexData[3*2]{};
    MeshIndexData indices{MeshIndexType::UnsignedShort, indexData};
    CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
    CORRADE_VERIFY(indices.data().data() == indexData);
}

void MeshDataTest::constructIndexTypeErasedWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char indexData[3*2]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshIndexData{MeshIndexType::UnsignedInt, indexData};
    CORRADE_COMPARE(out.str(), "Trade::MeshIndexData: view size 6 does not correspond to MeshIndexType::UnsignedInt\n");
}

void MeshDataTest::constructIndex2D() {
    {
        const UnsignedByte indexData[]{25, 132, 3};
        MeshIndexData indices{Containers::arrayCast<2, const char>(Containers::stridedArrayView(indexData))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(indices.data().data(), indexData);
    } {
        const UnsignedShort indexData[]{2575, 13224, 3};
        MeshIndexData indices{Containers::arrayCast<2, const char>(Containers::stridedArrayView(indexData))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(indices.data().data(), indexData);
    } {
        const UnsignedInt indexData[]{2110122, 132257, 3};
        MeshIndexData indices{Containers::arrayCast<2, const char>(Containers::stridedArrayView(indexData))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(indices.data().data(), indexData);
    }
}

void MeshDataTest::constructIndex2DNotIndexed() {
    MeshIndexData indices{Containers::StridedArrayView2D<const char>{}};
    CORRADE_COMPARE(indices.type(), MeshIndexType{});
    CORRADE_COMPARE(indices.data().data(), nullptr);
}

void MeshDataTest::constructIndex2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char data[3*3]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView2D<const char>{data, {3, 3}}};
    CORRADE_COMPARE(out.str(), "Trade::MeshIndexData: expected index type size 1, 2 or 4 but got 3\n");
}

void MeshDataTest::constructIndex2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char data[3*4]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView2D<const char>{data, {3, 2}, {4, 2}}};
    CORRADE_COMPARE(out.str(), "Trade::MeshIndexData: view is not contiguous\n");
}

void MeshDataTest::constructIndexNullptr() {
    /* Just verify it's not ambiguous */
    MeshIndexData data{nullptr};
    CORRADE_VERIFY(!data.data());
}

constexpr Vector2 Positions[] {
    {1.2f, 0.2f},
    {2.2f, 1.1f},
    {-0.2f, 7.2f}
};

void MeshDataTest::constructAttribute() {
    const Vector2 positionData[3];
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(positionData)};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.offset(positionData), 0);
    CORRADE_COMPARE(positions.stride(), sizeof(Vector2));
    CORRADE_VERIFY(positions.data().data() == positionData);
    /* This is allowed too for simplicity, it just ignores the parameter */
    CORRADE_VERIFY(positions.data(positionData).data() == positionData);

    constexpr MeshAttributeData cpositions{MeshAttribute::Position, Containers::arrayView(Positions)};
    constexpr bool isOffsetOnly = cpositions.isOffsetOnly();
    constexpr UnsignedShort arraySize = cpositions.arraySize();
    constexpr MeshAttribute name = cpositions.name();
    constexpr VertexFormat format = cpositions.format();
    constexpr Short stride = cpositions.stride();
    constexpr Containers::StridedArrayView1D<const void> data = cpositions.data();
    CORRADE_VERIFY(!isOffsetOnly);
    CORRADE_COMPARE(arraySize, 0);
    CORRADE_COMPARE(name, MeshAttribute::Position);
    CORRADE_COMPARE(format, VertexFormat::Vector2);
    CORRADE_COMPARE(stride, sizeof(Vector2));
    CORRADE_COMPARE(data.data(), Positions);
}

void MeshDataTest::constructAttributeDefault() {
    MeshAttributeData data;
    CORRADE_COMPARE(data.name(), MeshAttribute{});
    CORRADE_COMPARE(data.format(), VertexFormat{});

    constexpr MeshAttributeData cdata;
    CORRADE_COMPARE(cdata.name(), MeshAttribute{});
    CORRADE_COMPARE(cdata.format(), VertexFormat{});
}

void MeshDataTest::constructAttributeCustom() {
    const Short idData[3]{};
    MeshAttributeData ids{meshAttributeCustom(13), Containers::arrayView(idData)};
    CORRADE_COMPARE(ids.name(), meshAttributeCustom(13));
    CORRADE_COMPARE(ids.format(), VertexFormat::Short);
    CORRADE_VERIFY(ids.data().data() == idData);
}

void MeshDataTest::constructAttribute2D() {
    char positionData[4*sizeof(Vector2)]{};
    auto positionView = Containers::StridedArrayView2D<char>{positionData,
        {4, sizeof(Vector2)}}.every(2);

    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, positionView};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.data().data(), positionView.data());
}

void MeshDataTest::constructAttribute2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char positionData[4*sizeof(Vector2)]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3,
        Containers::StridedArrayView2D<char>{positionData,
            {4, sizeof(Vector2)}}.every(2)};
    CORRADE_COMPARE(out.str(), "Trade::MeshAttributeData: second view dimension size 8 doesn't match VertexFormat::Vector3\n");
}

void MeshDataTest::constructAttribute2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char positionData[4*sizeof(Vector2)]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{positionData,
            {2, sizeof(Vector2)*2}}.every({1, 2})};
    CORRADE_COMPARE(out.str(), "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructAttributeTypeErased() {
    const Vector3 positionData[3]{};
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, Containers::arrayCast<const char>(Containers::stridedArrayView(positionData))};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector3);
    CORRADE_VERIFY(positions.data().data() == positionData);
}

void MeshDataTest::constructAttributeNullptr() {
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_VERIFY(!positions.data().data());
}

void MeshDataTest::constructAttributePadding() {
    MeshAttributeData padding{-35};
    CORRADE_VERIFY(!padding.isOffsetOnly());
    CORRADE_COMPARE(padding.arraySize(), 0);
    CORRADE_COMPARE(padding.name(), MeshAttribute{});
    CORRADE_COMPARE(padding.format(), VertexFormat{});
    CORRADE_COMPARE(padding.data().size(), 0);
    CORRADE_COMPARE(padding.data().stride(), -35);
    CORRADE_VERIFY(!padding.data());
}

void MeshDataTest::constructAttributeNonOwningArray() {
    const MeshAttributeData data[3];
    Containers::Array<MeshAttributeData> array = meshAttributeDataNonOwningArray(data);
    CORRADE_COMPARE(array.size(), 3);
    CORRADE_COMPARE(static_cast<const void*>(array.data()), data);
}

void MeshDataTest::constructAttributeOffsetOnly() {
    struct {
        Vector2 position;
        Vector2 textureCoordinates;
    } vertexData[] {
        {{}, {1.0f, 0.3f}},
        {{}, {0.5f, 0.7f}},
    };

    MeshAttributeData a{MeshAttribute::TextureCoordinates, VertexFormat::Vector2, sizeof(Vector2), 2, 2*sizeof(Vector2)};
    CORRADE_VERIFY(a.isOffsetOnly());
    CORRADE_COMPARE(a.arraySize(), 0);
    CORRADE_COMPARE(a.name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(a.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(a.offset(vertexData), sizeof(Vector2));
    CORRADE_COMPARE(a.stride(), 2*sizeof(Vector2));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data(vertexData)),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);

    constexpr MeshAttributeData ca{MeshAttribute::TextureCoordinates, VertexFormat::Vector2, sizeof(Vector2), 2, 2*sizeof(Vector2)};
    CORRADE_VERIFY(ca.isOffsetOnly());
    CORRADE_COMPARE(ca.arraySize(), 0);
    CORRADE_COMPARE(ca.name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(ca.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(ca.offset(vertexData), sizeof(Vector2));
    CORRADE_COMPARE(ca.stride(), 2*sizeof(Vector2));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data(vertexData)),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructAttributeImplementationSpecificFormat() {
    Vector2 positions[]{{1.0f, 0.3f}, {0.5f, 0.7f}};

    /* This should not fire any asserts */
    MeshAttributeData a{MeshAttribute::TextureCoordinates, vertexFormatWrap(0x3a), positions};
    CORRADE_COMPARE(a.name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(a.format(), vertexFormatWrap(0x3a));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data()),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructAttributeWrongFormat() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 positionData[3];

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Color, Containers::arrayView(positionData)};
    MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector2, 0, 3, sizeof(Vector2)};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshAttributeData: VertexFormat::Vector2 is not a valid format for Trade::MeshAttribute::Color\n"
        "Trade::MeshAttributeData: VertexFormat::Vector2 is not a valid format for Trade::MeshAttribute::Color\n");
}

void MeshDataTest::constructAttributeWrongStride() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char positionData[3*sizeof(Vector3)]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, Containers::arrayCast<const char>(positionData)};
    MeshAttributeData{meshAttributeCustom(1), VertexFormat::Float, Containers::arrayCast<const Vector3>(positionData), 4};
    /* We need this one to be constexpr, which means there can't be a warning
       about stride not matching the size */
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, 0, 3*sizeof(Vector3), 1};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, Containers::StridedArrayView1D<const void>{positionData, 0, -16}};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, Containers::StridedArrayView1D<const void>{positionData, 0, 65000}};
    MeshAttributeData{65000};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshAttributeData: expected stride to be positive and enough to fit VertexFormat::Vector3, got 1\n"
        "Trade::MeshAttributeData: expected stride to be positive and enough to fit VertexFormat::Float[4], got 12\n"
        "Trade::MeshAttributeData: expected stride to be positive and at most 32k, got -16\n"
        "Trade::MeshAttributeData: expected stride to be positive and at most 32k, got 65000\n"
        "Trade::MeshAttributeData: at most 32k padding supported, got 65000\n"
    );
}

void MeshDataTest::constructAttributeWrongDataAccess() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 positionData[3];
    MeshAttributeData a{MeshAttribute::Position, Containers::arrayView(positionData)};
    MeshAttributeData b{MeshAttribute::Position, VertexFormat::Vector2, 0, 3, sizeof(Vector2)};
    CORRADE_VERIFY(!a.isOffsetOnly());
    CORRADE_VERIFY(b.isOffsetOnly());

    std::ostringstream out;
    Error redirectError{&out};
    b.data();
    CORRADE_COMPARE(out.str(),
        "Trade::MeshAttributeData::data(): the attribute is a relative offset, supply a data array\n");
}

constexpr Vector2 ArrayVertexData[3*4]
    /* MSVC 2015 needs an explicit initializer. GCC 4.8 *doesn't*. */
    #ifdef CORRADE_MSVC2015_COMPATIBILITY
    {}
    #endif
    ;

void MeshDataTest::constructArrayAttribute() {
    Vector2 vertexData[3*4];
    Containers::StridedArrayView2D<Vector2> attribute{vertexData, {3, 4}};
    MeshAttributeData data{meshAttributeCustom(35), attribute};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);

    constexpr Containers::StridedArrayView2D<const Vector2> cattribute{ArrayVertexData, {3, 4}};
    constexpr MeshAttributeData cdata{meshAttributeCustom(35), cattribute};
    CORRADE_VERIFY(!cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_VERIFY(cdata.data().data() == ArrayVertexData);
    CORRADE_COMPARE(cdata.data().size(), 3);
    CORRADE_COMPARE(cdata.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 vertexData[4*3]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35),
        Containers::StridedArrayView2D<Vector2>{vertexData,
            {4, 3}}.every({1, 2})};
    CORRADE_COMPARE(out.str(), "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructArrayAttribute2D() {
    char vertexData[3*4*sizeof(Vector2)];
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, Containers::StridedArrayView2D<char>{vertexData, {3, 4*sizeof(Vector2)}}, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttribute2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char vertexData[3*4*sizeof(Vector2)]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{vertexData,
            {3, 4*sizeof(Vector2)}}, 3};
    CORRADE_COMPARE(out.str(), "Trade::MeshAttributeData: second view dimension size 32 doesn't match VertexFormat::Vector2 and array size 3\n");
}

void MeshDataTest::constructArrayAttribute2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char vertexData[4*3*sizeof(Vector2)]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{vertexData,
            {3, sizeof(Vector2)*4}}.every({1, 2}), 2};
    CORRADE_COMPARE(out.str(), "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructArrayAttributeTypeErased() {
    Vector2 vertexData[3*4];
    Containers::StridedArrayView1D<Vector2> attribute{vertexData, 3, 4*sizeof(Vector2)};
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, attribute, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeNullptr() {
    MeshAttributeData positions{meshAttributeCustom(35), VertexFormat::Vector2, nullptr, 4};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 4);
    CORRADE_COMPARE(positions.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_VERIFY(!positions.data().data());
}

void MeshDataTest::constructArrayAttributeOffsetOnly() {
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, sizeof(Vector2), 3, sizeof(Vector2), 4};
    CORRADE_VERIFY(data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);

    Vector2 vertexData[1 + 3*4];
    CORRADE_VERIFY(data.data(vertexData).data() == vertexData + 1);
    CORRADE_COMPARE(data.data(vertexData).size(), 3);
    CORRADE_COMPARE(data.data(vertexData).stride(), sizeof(Vector2));

    constexpr MeshAttributeData cdata{meshAttributeCustom(35), VertexFormat::Vector2, sizeof(Vector2), 3, sizeof(Vector2), 4};
    CORRADE_VERIFY(cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
}

void MeshDataTest::constructArrayAttributeNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 positionData[3*3];
    Containers::ArrayView<Vector2> positions{positionData};
    Containers::StridedArrayView2D<Vector2> positions2D{positionData, {3, 3}};
    auto positions2Dchar = Containers::arrayCast<2, const char>(positions2D);

    /* This is all fine */
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, positions, 0};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 3, 6*sizeof(Vector2), 0};
    MeshAttributeData{meshAttributeCustom(35), vertexFormatWrap(0xdead), positions, 0};
    MeshAttributeData{meshAttributeCustom(35), positions2D};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2, positions2Dchar, 3};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2, 0, 3, 6*sizeof(Vector2), 3};

    /* This is not */
    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2b, Containers::arrayView(positionData), 3};
    MeshAttributeData{meshAttributeCustom(35), vertexFormatWrap(0xdead), Containers::arrayView(positionData), 3};
    MeshAttributeData{MeshAttribute::Position, positions2D};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, positions2Dchar, 3};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 3, 6*sizeof(Vector2), 3};
    MeshAttributeData{meshAttributeCustom(35), vertexFormatWrap(0xdead), 0, 3, 6*sizeof(Vector2), 3};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: array attributes can't have an implementation-specific format\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: array attributes can't have an implementation-specific format\n");
}

void MeshDataTest::construct() {
    auto&& instanceData = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 textureCoordinate;
        Short id;
    };

    Containers::Array<char> indexData{8*sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData).slice(1, 7);
    indexView[0] = 0;
    indexView[1] = 1;
    indexView[2] = 2;
    indexView[3] = 0;
    indexView[4] = 2;
    indexView[5] = 1;

    /* Enough vertex data to fit also the case with large explicit vertex count
       (but fill just the first 3, as those are only tested) */
    Containers::Array<char> vertexData{17*sizeof(Vertex)};
    auto vertexView = Containers::arrayCast<Vertex>(vertexData).prefix(3);
    vertexView[0].position = {0.1f, 0.2f, 0.3f};
    vertexView[1].position = {0.4f, 0.5f, 0.6f};
    vertexView[2].position = {0.7f, 0.8f, 0.9f};
    vertexView[0].normal = Vector3::xAxis();
    vertexView[1].normal = Vector3::yAxis();
    vertexView[2].normal = Vector3::zAxis();
    vertexView[0].textureCoordinate = {0.000f, 0.125f};
    vertexView[1].textureCoordinate = {0.250f, 0.375f};
    vertexView[2].textureCoordinate = {0.500f, 0.625f};
    vertexView[0].id = 15;
    vertexView[1].id = -374;
    vertexView[2].id = 22;

    if(instanceData.vertexCount < 3)
        vertexView = vertexView.prefix(instanceData.vertexCount);

    int importerState;
    MeshIndexData indices{indexView};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::StridedArrayView1D<Vector3>{vertexData, &vertexView[0].position, vertexView.size(), sizeof(Vertex)}};
    /* Using a relative offset */
    MeshAttributeData normals{MeshAttribute::Normal,
        VertexFormat::Vector3, offsetof(Vertex, normal),
        UnsignedInt(vertexView.size()), sizeof(Vertex)};
    MeshAttributeData textureCoordinates{MeshAttribute::TextureCoordinates,
        Containers::StridedArrayView1D<Vector2>{vertexData, &vertexView[0].textureCoordinate, vertexView.size(), sizeof(Vertex)}};
    MeshAttributeData ids{meshAttributeCustom(13),
        Containers::StridedArrayView1D<Short>{vertexData, &vertexView[0].id, vertexView.size(), sizeof(Vertex)}};
    MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), indices,
        /* Texture coordinates deliberately twice (though aliased) */
        std::move(vertexData), {positions, textureCoordinates, normals, textureCoordinates, ids}, instanceData.vertexCount, &importerState};

    /* Basics */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!data.attributeData().empty());
    CORRADE_COMPARE(static_cast<const void*>(data.indexData() + 2), indexView.data());
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertexView.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableIndexData() + 2), indexView.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableVertexData()), vertexView.data());
    CORRADE_COMPARE(data.importerState(), &importerState);

    /* Index access */
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 6);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indexOffset(), 2);

    /* Typeless index access with a cast later */
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(data.indices())[1]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(data.indices())[3]), 0);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(data.indices())[4]), 2);

    /* Typed index access */
    CORRADE_COMPARE(data.indices<UnsignedShort>()[0], 0);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[2], 2);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[5], 1);

    /* Attribute access by ID */
    CORRADE_COMPARE(data.vertexCount(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeCount(), 5);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeName(1), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeName(2), MeshAttribute::Normal);
    CORRADE_COMPARE(data.attributeName(3), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeName(4), meshAttributeCustom(13));
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(1), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(2), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(3), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(4), VertexFormat::Short);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeOffset(1), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(2), sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(3), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(4), 2*sizeof(Vector3) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeStride(0), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(1), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(2), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(3), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeArraySize(0), 0);
    CORRADE_COMPARE(data.attributeArraySize(1), 0);
    CORRADE_COMPARE(data.attributeArraySize(2), 0);
    CORRADE_COMPARE(data.attributeArraySize(3), 0);
    CORRADE_COMPARE(data.attributeArraySize(4), 0);

    /* Typeless access by ID with a cast later */
    CORRADE_COMPARE(data.attribute(0).size()[0], instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute(0).size()[0], instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.attribute(0))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(1))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.attribute(2))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(3))[1]), (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Short>(
            data.attribute(4))[0]), 15);
        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(0))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(1))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(2))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(3))[1]), (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Short>(
            data.mutableAttribute(4))[0]), 15);
    }

    /* Typed access by ID */
    CORRADE_COMPARE(data.attribute<Vector3>(0).size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute<Vector3>(0).size(), instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3>(0)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.attribute<Vector2>(1)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.attribute<Vector3>(2)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.attribute<Vector2>(3)[1], (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE(data.attribute<Short>(4)[1], -374);
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(0)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(1)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(2)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(3)[1], (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE(data.mutableAttribute<Short>(4)[1], -374);
    }

    /* Raw attribute data access by ID */
    CORRADE_COMPARE(data.attributeData(3).name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeData(3).format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeData(3).data().size(), instanceData.expectedVertexCount);
    if(instanceData.vertexCount)
        CORRADE_COMPARE(Containers::arrayCast<const Vector2>(data.attributeData(3).data())[1], (Vector2{0.250f, 0.375f}));

    /* Attribute access by name */
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::Position));
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::Normal));
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::TextureCoordinates));
    CORRADE_VERIFY(data.hasAttribute(meshAttributeCustom(13)));
    CORRADE_VERIFY(!data.hasAttribute(MeshAttribute::Color));
    CORRADE_VERIFY(!data.hasAttribute(meshAttributeCustom(23)));
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Position), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Normal), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::TextureCoordinates), 2);
    CORRADE_COMPARE(data.attributeCount(meshAttributeCustom(13)), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Color), 0);
    CORRADE_COMPARE(data.attributeCount(meshAttributeCustom(23)), 0);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Normal), 2);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::TextureCoordinates), 1);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::TextureCoordinates, 1), 3);
    CORRADE_COMPARE(data.attributeId(meshAttributeCustom(13)), 4);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position),
        VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Normal),
        VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::TextureCoordinates, 0),
        VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::TextureCoordinates, 1),
        VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(meshAttributeCustom(13)),
        VertexFormat::Short);
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Normal), sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::TextureCoordinates, 0), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::TextureCoordinates, 1), 2*sizeof(Vector3));    CORRADE_COMPARE(data.attributeOffset(meshAttributeCustom(13)), 2*sizeof(Vector3) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Position), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Normal), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::TextureCoordinates, 0), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::TextureCoordinates, 1), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(meshAttributeCustom(13)), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Normal), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::TextureCoordinates, 0), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::TextureCoordinates, 1), 0);
    CORRADE_COMPARE(data.attributeArraySize(meshAttributeCustom(13)), 0);

    /* Typeless access by name with a cast later */
    CORRADE_COMPARE(data.attribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.attribute(MeshAttribute::Position))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.attribute(MeshAttribute::Normal))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(MeshAttribute::TextureCoordinates, 0))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(MeshAttribute::TextureCoordinates, 1))[1]), (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Short>(
            data.attribute(meshAttributeCustom(13)))[1]), -374);
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.mutableAttribute(MeshAttribute::Position))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(MeshAttribute::Normal))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(MeshAttribute::TextureCoordinates, 0))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(MeshAttribute::TextureCoordinates, 1))[1]), (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Short>(
            data.mutableAttribute(meshAttributeCustom(13)))[1]), -374);
    }

    /* Typed access by name */
    CORRADE_COMPARE(data.attribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3>(MeshAttribute::Position)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.attribute<Vector3>(MeshAttribute::Normal)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::TextureCoordinates, 0)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::TextureCoordinates, 1)[1], (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE(data.attribute<Short>(meshAttributeCustom(13))[2], 22);
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(MeshAttribute::Position)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(MeshAttribute::Normal)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::TextureCoordinates, 0)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::TextureCoordinates, 1)[1], (Vector2{0.250f, 0.375f}));
        CORRADE_COMPARE(data.attribute<Short>(meshAttributeCustom(13))[2], 22);
    }
}

void MeshDataTest::constructZeroIndices() {
    /* This is a valid use case because this could be an empty slice of a
       well-defined indexed mesh. Explicitly use a non-null zero-sized array
       to check the importer is checking size and not pointer. */
    Containers::Array<char> vertexData{3*sizeof(Vector3)};
    auto vertices = Containers::arrayCast<Vector3>(vertexData);
    char i;
    Containers::Array<char> indexData{&i, 0, [](char*, std::size_t){}};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    MeshAttributeData positions{MeshAttribute::Position, vertices};
    MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), MeshIndexData{indices},
        std::move(vertexData), {positions}};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE(data.indexCount(), 0);
    CORRADE_COMPARE(data.indices().size(), (Containers::StridedArrayView2D<std::size_t>::Size{0, 4}));
    CORRADE_COMPARE(data.mutableIndices().size(), (Containers::StridedArrayView2D<std::size_t>::Size{0, 4}));
    CORRADE_COMPARE(data.vertexCount(), 3);
}

void MeshDataTest::constructZeroAttributes() {
    /* This is a valid use case because e.g. the index/vertex data can be
       shared by multiple meshes and this particular one is just a plain
       index array */
    Containers::Array<char> indexData{3*sizeof(UnsignedInt)};
    Containers::Array<char> vertexData{3};
    auto indexView = Containers::arrayCast<UnsignedInt>(indexData);
    MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), MeshIndexData{indexView},
        std::move(vertexData), {}, 15};

    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.attributeCount(), 0);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.vertexData().size(), 3);
    CORRADE_COMPARE(data.vertexCount(), 15);
}

void MeshDataTest::constructZeroVertices() {
    /* This is a valid use case because this could be an empty slice of a
       well-defined indexed mesh */
    Containers::Array<char> indexData{3*sizeof(UnsignedInt)};
    auto indexView = Containers::arrayCast<UnsignedInt>(indexData);
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, nullptr};
    MeshData data{MeshPrimitive::Triangles,
        std::move(indexData), MeshIndexData{indexView},
        nullptr, {positions}};

    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attribute<Vector3>(0).size(), 0);
    CORRADE_VERIFY(!data.vertexData());
    CORRADE_COMPARE(data.vertexCount(), 0);
}

void MeshDataTest::constructIndexless() {
    Containers::Array<char> vertexData{3*sizeof(Vector2)};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);
    vertexView[0] = {0.1f, 0.2f};
    vertexView[1] = {0.4f, 0.5f};
    vertexView[2] = {0.7f, 0.8f};

    int importerState;
    MeshAttributeData positions{MeshAttribute::Position, vertexView};
    MeshData data{MeshPrimitive::LineLoop, std::move(vertexData), {positions}, MeshData::ImplicitVertexCount, &importerState};
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    /* These are empty so it doesn't matter, but this is a nice non-restrictive
       default */
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(data.importerState(), &importerState);

    /* Access to indexData() and typeless access to (mutable)indices() is
       allowed, to allow creation of MeshData instances referencing other
       MeshData without having to branch on isIndexed(). */
    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.indexData(), nullptr);
    CORRADE_COMPARE(data.indices().data(), nullptr);
    CORRADE_COMPARE(data.indices().size(), (Containers::StridedArrayView2D<std::size_t>::Size{0, 0}));
    CORRADE_COMPARE(data.mutableIndices().data(), nullptr);
    CORRADE_COMPARE(data.mutableIndices().size(), (Containers::StridedArrayView2D<std::size_t>::Size{0, 0}));

    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::Position)[1], (Vector2{0.4f, 0.5f}));
}

void MeshDataTest::constructIndexlessZeroVertices() {
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr};
    MeshData data{MeshPrimitive::LineLoop, nullptr, {positions}};
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(data.indexData(), nullptr);
    CORRADE_COMPARE(data.vertexData(), nullptr);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position), VertexFormat::Vector2);
}

void MeshDataTest::constructAttributeless() {
    Containers::Array<char> indexData{6*sizeof(UnsignedInt)};
    auto indexView = Containers::arrayCast<UnsignedInt>(indexData);
    indexView[0] = 0;
    indexView[1] = 1;
    indexView[2] = 2;
    indexView[3] = 0;
    indexView[4] = 2;
    indexView[5] = 1;

    int importerState;
    MeshIndexData indices{indexView};
    MeshData data{MeshPrimitive::TriangleStrip, std::move(indexData), indices, 3, &importerState};
    /* These are empty so it doesn't matter, but this is a nice non-restrictive
       default */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.vertexData(), nullptr);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 6);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE(data.indices<UnsignedInt>()[0], 0);
    CORRADE_COMPARE(data.indices<UnsignedInt>()[2], 2);
    CORRADE_COMPARE(data.indices<UnsignedInt>()[5], 1);

    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

void MeshDataTest::constructNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    UnsignedShort indexData[]{0, 1, 0};
    Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};
    MeshData data{MeshPrimitive::Triangles, instanceData.indexDataFlags, Containers::arrayView(indexData), indices, instanceData.vertexDataFlags, Containers::arrayView(vertexData), {positions}, MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), instanceData.indexDataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.vertexDataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), +indexData);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), +vertexData);
    if(instanceData.indexDataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), +indexData);
    if(instanceData.vertexDataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), +vertexData);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[2], 0);
    if(instanceData.indexDataFlags & DataFlag::Mutable) {
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[1], 1);
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[2], 0);
    }

    CORRADE_COMPARE(data.vertexCount(), 2);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeStride(0), sizeof(Vector2));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
    if(instanceData.vertexDataFlags & DataFlag::Mutable) {
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
    }
}

void MeshDataTest::constructIndicesNotOwned() {
    auto&& instanceData = SingleNotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    UnsignedShort indexData[]{0, 1, 0};
    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);
    vertexView[0] = {0.1f, 0.2f};
    vertexView[1] = {0.4f, 0.5f};

    int importerState;
    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, vertexView};
    MeshData data{MeshPrimitive::Triangles, instanceData.dataFlags, Containers::arrayView(indexData), indices, std::move(vertexData), {positions}, MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), +indexData);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertexView.data());
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), +indexData);
    CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), vertexView.data());
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[2], 0);
    if(instanceData.dataFlags & DataFlag::Mutable) {
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[1], 1);
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[2], 0);
    }

    CORRADE_COMPARE(data.vertexCount(), 2);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeStride(0), sizeof(Vector2));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
    CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
}

void MeshDataTest::constructVerticesNotOwned() {
    auto&& instanceData = SingleNotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    Containers::Array<char> indexData{3*sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    indexView[0] = 0;
    indexView[1] = 1;
    indexView[2] = 0;
    Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshIndexData indices{indexView};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};
    MeshData data{MeshPrimitive::Triangles, std::move(indexData), indices, instanceData.dataFlags, Containers::arrayView(vertexData), {positions}, MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), indexView.data());
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), +vertexData);
    CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), indexView.data());
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), +vertexData);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[2], 0);
    CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[2], 0);

    CORRADE_COMPARE(data.vertexCount(), 2);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeStride(0), sizeof(Vector2));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(data.attribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
    if(instanceData.dataFlags & DataFlag::Mutable) {
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));
    }
}

void MeshDataTest::constructIndexlessNotOwned() {
    auto&& instanceData = SingleNotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};
    MeshData data{MeshPrimitive::LineLoop, instanceData.dataFlags, vertexData, {positions}, MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(data.indexData(), nullptr);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(data.mutableIndexData(), nullptr);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 2);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::Position)[1], (Vector2{0.4f, 0.5f}));
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::Position)[1], (Vector2{0.4f, 0.5f}));
}

void MeshDataTest::constructAttributelessNotOwned() {
    auto&& instanceData = SingleNotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    UnsignedShort indexData[]{0, 1, 0};

    int importerState;
    MeshIndexData indices{indexData};
    MeshData data{MeshPrimitive::TriangleStrip, instanceData.dataFlags, indexData, indices, 5, &importerState};
    CORRADE_COMPARE(data.indexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.vertexData(), nullptr);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(data.mutableVertexData(), nullptr);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[0], 0);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(data.indices<UnsignedShort>()[2], 0);
    if(instanceData.dataFlags & DataFlag::Mutable) {
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[0], 0);
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[1], 1);
        CORRADE_COMPARE(data.mutableIndices<UnsignedShort>()[2], 0);
    }

    CORRADE_COMPARE(data.vertexCount(), 5);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

void MeshDataTest::constructIndexlessAttributeless() {
    int importerState;
    MeshData data{MeshPrimitive::TriangleStrip, 37, &importerState};
    /* These are both empty so it doesn't matter, but this is a nice
       non-restrictive default */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.indexData(), nullptr);
    CORRADE_COMPARE(data.vertexData(), nullptr);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 37);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

void MeshDataTest::constructIndexlessAttributelessZeroVertices() {
    int importerState;
    MeshData data{MeshPrimitive::TriangleStrip, 0, &importerState};
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.indexData(), nullptr);
    CORRADE_COMPARE(data.vertexData(), nullptr);
    CORRADE_COMPARE(data.importerState(), &importerState);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

void MeshDataTest::constructIndexDataButNotIndexed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> indexData{6};

    std::ostringstream out;
    Error redirectError{&out};
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr};
    MeshData{MeshPrimitive::Points, std::move(indexData), MeshIndexData{}, nullptr, {positions}};
    CORRADE_COMPARE(out.str(), "Trade::MeshData: indexData passed for a non-indexed mesh\n");
}

void MeshDataTest::constructAttributelessImplicitVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Points, nullptr, {}};
    CORRADE_COMPARE(out.str(), "Trade::MeshData: vertex count can't be implicit if there are no attributes\n");
}

void MeshDataTest::constructIndicesNotContained() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> indexData{reinterpret_cast<char*>(0xbadda9), 6, [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> indexData2{reinterpret_cast<UnsignedShort*>(0xdead), 3};
    MeshIndexData indices{indexData2};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Triangles, std::move(indexData), indices, 1};
    MeshData{MeshPrimitive::Triangles, nullptr, indices, 1};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: indices [0xdead:0xdeb3] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"
        "Trade::MeshData: indices [0xdead:0xdeb3] are not contained in passed indexData array [0x0:0x0]\n");
}

void MeshDataTest::constructAttributeNotContained() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{reinterpret_cast<char*>(0xbadda9), 24, [](char*, std::size_t){}};
    Containers::ArrayView<Vector2> vertexData2{reinterpret_cast<Vector2*>(0xdead), 3};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayCast<Vector2>(vertexData)};
    MeshAttributeData positions2{MeshAttribute::Position, Containers::arrayView(vertexData2)};
    MeshAttributeData positions3{MeshAttribute::Position, VertexFormat::Vector2, 1, 3, 8};
    /* See implementationSpecificVertexFormatNotContained() below for
       implementation-specific formats */

    /* Here the original positions array is shrunk from 3 items to 2 and the
       vertex data too, which should work without asserting -- comparing just
       the original view would not pass, which is wrong */
    MeshData{MeshPrimitive::Triangles, {}, vertexData.prefix(16), {positions}, 2};

    std::ostringstream out;
    Error redirectError{&out};
    /* Here the original positions array is extended from 3 items to 4, which
       makes it not fit anymore, and thus an assert should hit -- comparing
       just the original view would pass, which is wrong */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {positions}, 4};
    MeshData{MeshPrimitive::Triangles, std::move(vertexData), {positions, positions2}};
    MeshData{MeshPrimitive::Triangles, nullptr, {positions}};
    MeshData{MeshPrimitive::Triangles, Containers::Array<char>{24}, {positions3}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: attribute 0 [0xbadda9:0xbaddc9] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 1 [0xdead:0xdec5] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 0 [0xbadda9:0xbaddc1] is not contained in passed vertexData array [0x0:0x0]\n"
        "Trade::MeshData: offset attribute 0 spans 25 bytes but passed vertexData array has only 24\n");
}

void MeshDataTest::constructInconsitentVertexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{136};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayCast<Vector2>(vertexData).prefix(3)};
    MeshAttributeData positions2{MeshAttribute::Position, Containers::arrayCast<Vector2>(vertexData).prefix(2)};

    std::ostringstream out;
    Error redirectError{&out};
    /* The explicit vertex count should be ignored for the assertion message,
       we only check that all passed attribute arrays have the same vertex
       count. However, the actual "containment" of the attribute views is
       checked with the explicit vertex count -- see the
       constructAttributeNotContained() test above. */
    MeshData{MeshPrimitive::Triangles, std::move(vertexData), {positions, positions2}, 17};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: attribute 1 has 2 vertices but 3 expected\n");
}

void MeshDataTest::constructNotOwnedIndexFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort indexData[]{0, 1, 0};
    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, {}, vertexData, {positions}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructNotOwnedVertexFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort indexData[]{0, 1, 0};
    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, {}, indexData, indices, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructIndicesNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedShort indexData[]{0, 1, 0};
    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);
    vertexView[0] = {0.1f, 0.2f};
    vertexView[1] = {0.4f, 0.5f};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, vertexView};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, std::move(vertexData), {positions}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructVerticesNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> indexData{3*sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    indexView[0] = 0;
    indexView[1] = 1;
    indexView[2] = 0;
    Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexView};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, std::move(indexData), indices, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructIndexlessNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructAttributelessNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort indexData[]{0, 1, 0};
    MeshIndexData indices{indexData};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, 2};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructInvalidAttributeData() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshAttributeData a;
    MeshAttributeData b{3};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Triangles, nullptr, {a}};
    MeshData{MeshPrimitive::Triangles, nullptr, {b}};
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData: attribute 0 doesn't specify anything\n"
        "Trade::MeshData: attribute 0 doesn't specify anything\n");
}

void MeshDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<MeshData, const MeshData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<MeshData, const MeshData&>{}));
}

void MeshDataTest::constructMove() {
    Containers::Array<char> indexData{3*sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    indexView[0] = 0;
    indexView[1] = 1;
    indexView[2] = 0;

    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);
    vertexView[0] = {0.1f, 0.2f};
    vertexView[1] = {0.4f, 0.5f};

    int importerState;
    MeshIndexData indices{indexView};
    MeshAttributeData positions{MeshAttribute::Position, vertexView};
    MeshData a{MeshPrimitive::Triangles, std::move(indexData), indices, std::move(vertexData), {positions}, MeshData::ImplicitVertexCount, &importerState};

    MeshData b{std::move(a)};

    CORRADE_COMPARE(b.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(b.indexData()), indexView.data());
    CORRADE_COMPARE(static_cast<const void*>(b.vertexData()), vertexView.data());
    CORRADE_COMPARE(b.importerState(), &importerState);

    CORRADE_VERIFY(b.isIndexed());
    CORRADE_COMPARE(b.indexCount(), 3);
    CORRADE_COMPARE(b.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(b.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(b.indices<UnsignedShort>()[2], 0);

    CORRADE_COMPARE(b.vertexCount(), 2);
    CORRADE_COMPARE(b.attributeCount(), 1);
    CORRADE_COMPARE(b.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(b.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(b.attributeOffset(0), 0);
    CORRADE_COMPARE(b.attributeStride(0), sizeof(Vector2));
    CORRADE_COMPARE(b.attribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(b.attribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));

    MeshData c{MeshPrimitive::LineLoop, 37};
    c = std::move(b);

    CORRADE_COMPARE(c.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(c.indexData()), indexView.data());
    CORRADE_COMPARE(static_cast<const void*>(c.vertexData()), vertexView.data());
    CORRADE_COMPARE(c.importerState(), &importerState);

    CORRADE_VERIFY(c.isIndexed());
    CORRADE_COMPARE(c.indexCount(), 3);
    CORRADE_COMPARE(c.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(c.indices<UnsignedShort>()[1], 1);
    CORRADE_COMPARE(c.indices<UnsignedShort>()[2], 0);

    CORRADE_COMPARE(c.vertexCount(), 2);
    CORRADE_COMPARE(c.attributeCount(), 1);
    CORRADE_COMPARE(c.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(c.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE(c.attributeOffset(0), 0);
    CORRADE_COMPARE(c.attributeStride(0), sizeof(Vector2));
    CORRADE_COMPARE(c.attribute<Vector2>(0)[0], (Vector2{0.1f, 0.2f}));
    CORRADE_COMPARE(c.attribute<Vector2>(0)[1], (Vector2{0.4f, 0.5f}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<MeshData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<MeshData>::value);
}

template<class> struct NameTraits;
#define _c(format) template<> struct NameTraits<format> {                   \
        static const char* name() { return #format; }                       \
    };
_c(Vector2)
_c(Vector2h)
_c(Vector2ub)
_c(Vector2b)
_c(Vector2us)
_c(Vector2s)
_c(Vector3)
_c(Vector3h)
_c(Vector3ub)
_c(Vector3b)
_c(Vector3us)
_c(Vector3s)
_c(Vector4)
_c(Vector4h)
_c(Vector4b)
_c(Vector4s)
_c(Color3)
_c(Color3h)
_c(Color3ub)
_c(Color3us)
_c(Color4)
_c(Color4h)
_c(Color4ub)
_c(Color4us)
#undef _c

template<class T> void MeshDataTest::indicesAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Containers::Array<char> indexData{3*sizeof(T)};
    auto indexView = Containers::arrayCast<T>(indexData);
    indexView[0] = 75;
    indexView[1] = 131;
    indexView[2] = 240;

    MeshData data{MeshPrimitive::Points, std::move(indexData), MeshIndexData{indexView}, 241};
    CORRADE_COMPARE_AS(data.indicesAsArray(),
        Containers::arrayView<UnsignedInt>({75, 131, 240}),
        TestSuite::Compare::Container);
}

void MeshDataTest::indicesIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> indexData{3*sizeof(UnsignedInt)};
    MeshData data{MeshPrimitive::Points, std::move(indexData), MeshIndexData{Containers::arrayCast<UnsignedInt>(indexData)}, 1};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    data.indicesInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::indicesInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::positions2DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector2<U>{U(2.0f), U(1.0f)});
    positionsView[1] = T::pad(Math::Vector2<U>{U(0.0f), U(-1.0f)});
    positionsView[2] = T::pad(Math::Vector2<U>{U(-2.0f), U(3.0f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions2DAsArray(),
        Containers::arrayView<Vector2>({{2.0f, 1.0f}, {0.0f, -1.0f}, {-2.0f, 3.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector2<typename T::Type>{2, 1});
    positionsView[1] = T::pad(Math::Vector2<typename T::Type>{0, 15});
    positionsView[2] = T::pad(Math::Vector2<typename T::Type>{22, 3});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions2DAsArray(),
        Containers::arrayView<Vector2>({{2.0f, 1.0f}, {0.0f, 15.0f}, {22.0f, 3.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector2<typename T::Type>{2, 1});
    positionsView[1] = T::pad(Math::Vector2<typename T::Type>{0, -15});
    positionsView[2] = T::pad(Math::Vector2<typename T::Type>{-22, 3});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions2DAsArray(),
        Containers::arrayView<Vector2>({{2.0f, 1.0f}, {0.0f, -15.0f}, {-22.0f, 3.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector2<typename T::Type>{Math::pack<typename T::Type>(1.0f), 0});
    positionsView[1] = T::pad(Math::Vector2<typename T::Type>{0, Math::pack<typename T::Type>(1.0f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        positionsView}}};
    CORRADE_COMPARE_AS(data.positions2DAsArray(),
        Containers::arrayView<Vector2>({{1.0f, 0.0f}, {0.0f, 1.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector2<typename T::Type>{Math::pack<typename T::Type>(1.0f), 0});
    positionsView[1] = T::pad(Math::Vector2<typename T::Type>{0, Math::pack<typename T::Type>(-1.0f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        positionsView}}};
    CORRADE_COMPARE_AS(data.positions2DAsArray(),
        Containers::arrayView<Vector2>({{1.0f, 0.0f}, {0.0f, -1.0f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::positions2DIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector2)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, Containers::arrayCast<Vector2>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector2 destination[2];
    data.positions2DInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::positions2DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::positions3DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    /* Needs to be sufficiently representable to have the test work also for
       half floats */
    positionsView[0] = T::pad(Math::Vector3<U>{U(2.0f), U(1.0f), U(0.75f)});
    positionsView[1] = T::pad(Math::Vector3<U>{U(0.0f), U(-1.0f), U(1.25f)});
    positionsView[2] = T::pad(Math::Vector3<U>{U(-2.0f), U(3.0f), U(2.5f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, 0.75f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -1.0f, 1.25f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{-2.0f, 3.0f, 2.5f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector3<typename T::Type>{2, 1, 135});
    positionsView[1] = T::pad(Math::Vector3<typename T::Type>{0, 15, 2});
    positionsView[2] = T::pad(Math::Vector3<typename T::Type>{22, 3, 192});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, 135.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, 15.0f, 2.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{22.0f, 3.0f, 192.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector3<typename T::Type>{2, 1, -117});
    positionsView[1] = T::pad(Math::Vector3<typename T::Type>{0, -15, 2});
    positionsView[2] = T::pad(Math::Vector3<typename T::Type>{-22, 3, 86});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, positionsView}}};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, -117.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -15.0f, 2.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{-22.0f, 3.0f, 86.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector3<typename T::Type>{Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f)});
    positionsView[1] = T::pad(Math::Vector3<typename T::Type>{0, Math::pack<typename T::Type>(1.0f), 0});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        positionsView}}};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{1.0f, 0.0f, 1.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, 1.0f, 0.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto positionsView = Containers::arrayCast<T>(vertexData);
    positionsView[0] = T::pad(Math::Vector3<typename T::Type>{Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f)});
    positionsView[1] = T::pad(Math::Vector3<typename T::Type>{0, Math::pack<typename T::Type>(-1.0f), 0});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        positionsView}}};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{1.0f, 0.0f, 1.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -1.0f, 0.0f}))
    }), TestSuite::Compare::Container);
}

void MeshDataTest::positions3DIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector3)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Position, Containers::arrayCast<Vector3>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.positions3DInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::positions3DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::tangentsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto tangentView = Containers::arrayCast<T>(vertexData);
    /* Needs to be sufficiently representable to have the test work also for
       half floats */
    tangentView[0] = T::pad(Math::Vector3<U>{U(2.0f), U(1.0f), U(0.75f)});
    tangentView[1] = T::pad(Math::Vector3<U>{U(0.0f), U(-1.0f), U(1.25f)});
    tangentView[2] = T::pad(Math::Vector3<U>{U(-2.0f), U(3.0f), U(2.5f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent, tangentView}}};
    CORRADE_COMPARE_AS(data.tangentsAsArray(), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::tangentsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto tangentsView = Containers::arrayCast<T>(vertexData);
    tangentsView[0] = T::pad(Math::Vector3<U>{Math::pack<U>(1.0f), 0, Math::pack<U>(1.0f)});
    tangentsView[1] = T::pad(Math::Vector3<U>{0, Math::pack<U>(-1.0f), 0});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        tangentsView}}};
    CORRADE_COMPARE_AS(data.tangentsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::tangentsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector3)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent, Containers::arrayCast<Vector3>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.tangentsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::tangentsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::bitangentSignsAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(Math::Vector4<T>)};
    auto tangentView = Containers::arrayCast<Math::Vector4<T>>(vertexData);
    /* Needs to be sufficiently representable to have the test work also for
       half floats */
    tangentView[0] = {T(2.0f), T(1.0f), T(0.75f), T(-1.0f)};
    tangentView[1] = {T(0.0f), T(-1.0f), T(1.25f), T(1.0f)};
    tangentView[2] = {T(-2.0f), T(3.0f), T(2.5f), T(-1.0f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent, tangentView}}};
    CORRADE_COMPARE_AS(data.bitangentSignsAsArray(), Containers::arrayView<Float>({
        -1.0f, 1.0f, -1.0f
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(Math::Vector4<T>)};
    auto bitangentsView = Containers::arrayCast<Math::Vector4<T>>(vertexData);
    bitangentsView[0] = {Math::pack<T>(1.0f), 0, Math::pack<T>(1.0f), Math::pack<T>(-1.0f)};
    bitangentsView[1] = {0, Math::pack<T>(-1.0f), 0, Math::pack<T>(1.0f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<Math::Vector4<T>>()) + 1),
        bitangentsView}}};
    CORRADE_COMPARE_AS(data.bitangentSignsAsArray(), Containers::arrayView<Float>({
        -1.0f, 1.0f
    }), TestSuite::Compare::Container);
}

void MeshDataTest::bitangentSignsAsArrayNotFourComponent() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector3s)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent, VertexFormat::Vector3sNormalized, Containers::arrayCast<Vector3s>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Float destination[3];
    data.bitangentSignsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::bitangentSignsInto(): expected four-component tangents, but got VertexFormat::Vector3sNormalized\n");
}

void MeshDataTest::bitangentSignsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector4)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Tangent, Containers::arrayCast<Vector4>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Float destination[2];
    data.bitangentSignsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::bitangentSignsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::bitangentsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto bitangentsView = Containers::arrayCast<T>(vertexData);
    /* Needs to be sufficiently representable to have the test work also for
       half floats */
    bitangentsView[0] = {U(2.0f), U(1.0f), U(0.75f)};
    bitangentsView[1] = {U(0.0f), U(-1.0f), U(1.25f)};
    bitangentsView[2] = {U(-2.0f), U(3.0f), U(2.5f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Bitangent, bitangentsView}}};
    CORRADE_COMPARE_AS(data.bitangentsAsArray(), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::bitangentsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto bitangentsView = Containers::arrayCast<T>(vertexData);
    bitangentsView[0] = {Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f)};
    bitangentsView[1] = {0, Math::pack<typename T::Type>(-1.0f), 0};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Bitangent,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        bitangentsView}}};
    CORRADE_COMPARE_AS(data.bitangentsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::bitangentsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector3)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Bitangent, Containers::arrayCast<Vector3>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.bitangentsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::bitangentsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::normalsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto normalsView = Containers::arrayCast<T>(vertexData);
    /* Needs to be sufficiently representable to have the test work also for
       half floats */
    normalsView[0] = {U(2.0f), U(1.0f), U(0.75f)};
    normalsView[1] = {U(0.0f), U(-1.0f), U(1.25f)};
    normalsView[2] = {U(-2.0f), U(3.0f), U(2.5f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Normal, normalsView}}};
    CORRADE_COMPARE_AS(data.normalsAsArray(), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::normalsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto normalsView = Containers::arrayCast<T>(vertexData);
    normalsView[0] = {Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f)};
    normalsView[1] = {0, Math::pack<typename T::Type>(-1.0f), 0};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Normal,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        normalsView}}};
    CORRADE_COMPARE_AS(data.normalsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::normalsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector3)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Normal, Containers::arrayCast<Vector3>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.normalsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::normalsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::textureCoordinates2DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto textureCoordinatesView = Containers::arrayCast<T>(vertexData);
    textureCoordinatesView[0] = {U(2.0f), U(1.0f)};
    textureCoordinatesView[1] = {U(0.0f), U(-1.0f)};
    textureCoordinatesView[2] = {U(-2.0f), U(3.0f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates, textureCoordinatesView}}};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, -1.0f}, {-2.0f, 3.0f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto textureCoordinatesView = Containers::arrayCast<T>(vertexData);
    textureCoordinatesView[0] = {2, 1};
    textureCoordinatesView[1] = {0, 15};
    textureCoordinatesView[2] = {22, 3};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates, textureCoordinatesView}}};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(),
        Containers::arrayView<Vector2>({{2.0f, 1.0f}, {0.0f, 15.0f}, {22.0f, 3.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto textureCoordinatesView = Containers::arrayCast<T>(vertexData);
    textureCoordinatesView[0] = {2, 1};
    textureCoordinatesView[1] = {0, -15};
    textureCoordinatesView[2] = {-22, 3};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates, textureCoordinatesView}}};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(),
        Containers::arrayView<Vector2>({{2.0f, 1.0f}, {0.0f, -15.0f}, {-22.0f, 3.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto textureCoordinatesView = Containers::arrayCast<T>(vertexData);
    textureCoordinatesView[0] = {Math::pack<typename T::Type>(1.0f), 0};
    textureCoordinatesView[1] = {0, Math::pack<typename T::Type>(1.0f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        textureCoordinatesView}}};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(),
        Containers::arrayView<Vector2>({{1.0f, 0.0f}, {0.0f, 1.0f}}),
        TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto textureCoordinatesView = Containers::arrayCast<T>(vertexData);
    textureCoordinatesView[0] = {Math::pack<typename T::Type>(1.0f), 0};
    textureCoordinatesView[1] = {0, Math::pack<typename T::Type>(-1.0f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates,
        /* Assuming the normalized enum is always after the non-normalized */
        VertexFormat(UnsignedInt(Implementation::vertexFormatFor<T>()) + 1),
        textureCoordinatesView}}};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(),
        Containers::arrayView<Vector2>({{1.0f, 0.0f}, {0.0f, -1.0f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::textureCoordinates2DIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Vector2)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::TextureCoordinates, Containers::arrayCast<Vector2>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Vector2 destination[2];
    data.textureCoordinates2DInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::textureCoordinates2DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::colorsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());
    typedef typename T::Type U;

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto colorsView = Containers::arrayCast<T>(vertexData);
    /* Can't use e.g. 0xff3366_rgbf because that's not representable in
       half-floats */
    colorsView[0] = {U(2.0f), U(1.0f), U(0.75f)};
    colorsView[1] = {U(0.0f), U(-1.0f), U(1.25f)};
    colorsView[2] = {U(-2.0f), U(3.0f), U(2.5f)};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Color, colorsView}}};
    CORRADE_COMPARE_AS(data.colorsAsArray(), Containers::arrayView<Color4>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::colorsAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    Containers::Array<char> vertexData{2*sizeof(T)};
    auto colorsView = Containers::arrayCast<T>(vertexData);
    colorsView[0] = T::pad(Math::Color4<typename T::Type>{Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f), 0});
    colorsView[1] = T::pad(Math::Color4<typename T::Type>{0, Math::pack<typename T::Type>(1.0f), 0, Math::pack<typename T::Type>(1.0f)});

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Color, colorsView}}};
    CORRADE_COMPARE_AS(data.colorsAsArray(), Containers::arrayView<Color4>({
        Color4::pad(Math::Vector<T::Size, Float>::pad(Vector4{1.0f, 0.0f, 1.0f, 0.0f}), 1.0f),
        Color4::pad(Math::Vector<T::Size, Float>::pad(Vector4{0.0f, 1.0f, 0.0f, 1.0f}), 1.0f)
    }), TestSuite::Compare::Container);
}

void MeshDataTest::colorsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(Color4)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::Color, Containers::arrayCast<Color4>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    Color4 destination[2];
    data.colorsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::colorsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::objectIdsAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Containers::Array<char> vertexData{3*sizeof(T)};
    auto objectIdsView = Containers::arrayCast<T>(vertexData);
    /* Can't use e.g. 0xff3366_rgbf because that's not representable in
       half-floats */
    objectIdsView[0] = {157};
    objectIdsView[1] = {24};
    objectIdsView[2] = {1};

    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::ObjectId, objectIdsView}}};
    CORRADE_COMPARE_AS(data.objectIdsAsArray(), Containers::arrayView<UnsignedInt>({
        157, 24, 1
    }), TestSuite::Compare::Container);
}

void MeshDataTest::objectIdsIntoArrayInvalidSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{3*sizeof(UnsignedInt)};
    MeshData data{MeshPrimitive::Points, std::move(vertexData), {MeshAttributeData{MeshAttribute::ObjectId, Containers::arrayCast<UnsignedInt>(vertexData)}}};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    data.objectIdsInto(destination);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::objectIdsInto(): expected a view with 3 elements but got 2\n");
}

/* MSVC 2015 doesn't like anonymous bitfields in inline structs, so putting the
   declaration outside */
struct VertexWithImplementationSpecificData {
    Long:64;
    long double thing;
};

void MeshDataTest::implementationSpecificVertexFormat() {
    VertexWithImplementationSpecificData vertexData[] {
        {456.0l},
        {456.0l}
    };

    /* Constructing should work w/o asserts */
    Containers::StridedArrayView1D<long double> attribute{vertexData,
        &vertexData[0].thing, 2, sizeof(VertexWithImplementationSpecificData)};
    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormatWrap(0xdead1), attribute},
        MeshAttributeData{MeshAttribute::Normal,
            vertexFormatWrap(0xdead2), attribute},
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            vertexFormatWrap(0xdead3), attribute},
        MeshAttributeData{MeshAttribute::Color,
            vertexFormatWrap(0xdead4), attribute}}};

    /* Getting typeless attribute should work also */
    UnsignedInt format = 0xdead1;
    for(MeshAttribute name: {MeshAttribute::Position,
                             MeshAttribute::Normal,
                             MeshAttribute::TextureCoordinates,
                             MeshAttribute::Color}) {
        CORRADE_ITERATION(name);
        CORRADE_COMPARE(data.attributeFormat(name), vertexFormatWrap(format++));

        /* The actual type size is unknown, so this will use the full stride */
        CORRADE_COMPARE(data.attribute(name).size()[1], sizeof(VertexWithImplementationSpecificData));

        CORRADE_COMPARE_AS((Containers::arrayCast<1, const long double>(
            data.attribute(name).prefix({2, sizeof(long double)}))),
            attribute, TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const long double>(
            data.mutableAttribute(name).prefix({2, sizeof(long double)}))),
            attribute, TestSuite::Compare::Container);
    }
}

void MeshDataTest::implementationSpecificVertexFormatWrongAccess() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    VertexWithImplementationSpecificData vertexData[] {
        {456.0l},
        {456.0l}
    };

    Containers::StridedArrayView1D<long double> attribute{vertexData,
        &vertexData[0].thing, 2, sizeof(VertexWithImplementationSpecificData)};
    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormatWrap(0xdead1), attribute},
        MeshAttributeData{MeshAttribute::Tangent,
            vertexFormatWrap(0xdead2), attribute},
        MeshAttributeData{MeshAttribute::Bitangent,
            vertexFormatWrap(0xdead2), attribute},
        MeshAttributeData{MeshAttribute::Normal,
            vertexFormatWrap(0xdead2), attribute},
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            vertexFormatWrap(0xdead3), attribute},
        MeshAttributeData{MeshAttribute::Color,
            vertexFormatWrap(0xdead4), attribute},
        MeshAttributeData{MeshAttribute::ObjectId,
            vertexFormatWrap(0xdead4), attribute}}};

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<Float>(MeshAttribute::Position);
    data.attribute<Float>(MeshAttribute::Normal);
    data.attribute<Float>(MeshAttribute::TextureCoordinates);
    data.attribute<Float>(MeshAttribute::Color);
    data.mutableAttribute<Float>(MeshAttribute::Position);
    data.mutableAttribute<Float>(MeshAttribute::Normal);
    data.mutableAttribute<Float>(MeshAttribute::TextureCoordinates);
    data.mutableAttribute<Float>(MeshAttribute::Color);
    data.positions2DAsArray();
    data.positions3DAsArray();
    data.tangentsAsArray();
    data.bitangentSignsAsArray();
    data.bitangentsAsArray();
    data.normalsAsArray();
    data.textureCoordinates2DAsArray();
    data.colorsAsArray();
    data.objectIdsAsArray();
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead3\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead3\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::positions2DInto(): can't extract data out of an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::positions3DInto(): can't extract data out of an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::tangentsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::bitangentSignsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::bitangentsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::normalsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::textureCoordinatesInto(): can't extract data out of an implementation-specific vertex format 0xdead3\n"
        "Trade::MeshData::colorsInto(): can't extract data out of an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::objectIdsInto(): can't extract data out of an implementation-specific vertex format 0xdead4\n");
}

void MeshDataTest::implementationSpecificVertexFormatNotContained() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> vertexData{reinterpret_cast<char*>(0xbadda9), 3, [](char*, std::size_t){}};
    Containers::ArrayView<char> vertexData2{reinterpret_cast<char*>(0xdead), 3};
    MeshAttributeData positions{MeshAttribute::Position, vertexFormatWrap(0x3a), vertexData};
    MeshAttributeData positions2{MeshAttribute::Position, vertexFormatWrap(0x3a), vertexData2};

    std::ostringstream out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Triangles, std::move(vertexData), {positions, positions2}};
    CORRADE_COMPARE(out.str(),
        /* Assumes size of the type is 0, so the diagnostic is different from
           constructAttributeNotContained() */
        "Trade::MeshData: attribute 1 [0xdead:0xdeaf] is not contained in passed vertexData array [0xbadda9:0xbaddac]\n");
}

void MeshDataTest::arrayAttribute() {
    Vector2 vertexData[3*4]{
        {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}, {7.0f, 8.0f},
        {1.1f, 2.2f}, {3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f},
        {0.1f, 0.2f}, {0.3f, 0.4f}, {0.5f, 0.6f}, {0.7f, 0.8f},
    };
    Containers::StridedArrayView2D<Vector2> positions2D{vertexData, {3, 4}};

    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{meshAttributeCustom(35), positions2D}
    }};

    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeArraySize(meshAttributeCustom(35)), 4);

    /* Raw access is "as usual" */
    auto attribute = Containers::arrayCast<2, const Vector2>(data.attribute(0));
    auto attributeByName = Containers::arrayCast<2, const Vector2>(data.attribute(meshAttributeCustom(35)));
    auto mutableAttribute = Containers::arrayCast<2, Vector2>(data.mutableAttribute(0));
    auto mutableAttributeByName = Containers::arrayCast<2, Vector2>(data.mutableAttribute(meshAttributeCustom(35)));
    CORRADE_COMPARE(attribute.size()[0], 3);
    CORRADE_COMPARE(attributeByName.size()[0], 3);
    CORRADE_COMPARE(mutableAttribute.size()[0], 3);
    CORRADE_COMPARE(mutableAttributeByName.size()[0], 3);
    for(std::size_t i = 0; i != 3; ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE_AS(attribute[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(attributeByName[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mutableAttribute[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mutableAttributeByName[i], positions2D[i],
            TestSuite::Compare::Container);
    }

    /* Typed access */
    attribute = data.attribute<Vector2[]>(0);
    attributeByName = data.attribute<Vector2[]>(meshAttributeCustom(35));
    mutableAttribute = data.mutableAttribute<Vector2[]>(0);
    mutableAttributeByName = data.mutableAttribute<Vector2[]>(meshAttributeCustom(35));
    CORRADE_COMPARE(attribute.size()[0], 3);
    CORRADE_COMPARE(attributeByName.size()[0], 3);
    CORRADE_COMPARE(mutableAttribute.size()[0], 3);
    CORRADE_COMPARE(mutableAttributeByName.size()[0], 3);
    for(std::size_t i = 0; i != 3; ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE_AS(attribute[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(attributeByName[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mutableAttribute[i], positions2D[i],
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mutableAttributeByName[i], positions2D[i],
            TestSuite::Compare::Container);
    }
}

void MeshDataTest::arrayAttributeWrongAccess() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 vertexData[3*4]{
        {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}, {7.0f, 8.0f},
        {1.1f, 2.2f}, {3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f},
        {0.1f, 0.2f}, {0.3f, 0.4f}, {0.5f, 0.6f}, {0.7f, 0.8f},
    };
    Containers::StridedArrayView1D<Vector2> positions{vertexData, 3, 4*sizeof(Vector2)};
    Containers::StridedArrayView2D<Vector2> positions2D{vertexData, {3, 4}};

    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{MeshAttribute::Position, positions},
        MeshAttributeData{meshAttributeCustom(35), positions2D}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<Vector2[]>(0);
    data.attribute<Vector2>(1);
    data.mutableAttribute<Vector2[]>(0);
    data.mutableAttribute<Vector2>(1);
    data.attribute<Vector2[]>(MeshAttribute::Position);
    data.attribute<Vector2>(meshAttributeCustom(35));
    data.mutableAttribute<Vector2[]>(MeshAttribute::Position);
    data.mutableAttribute<Vector2>(meshAttributeCustom(35));
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::attribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::attribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::mutableAttribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::mutableAttribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::attribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::attribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::mutableAttribute(): use T[] to access an array attribute\n"
        "Trade::MeshData::mutableAttribute(): use T[] to access an array attribute\n");
}

void MeshDataTest::mutableAccessNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort indexData[]{0, 1, 0};
    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(vertexData)};
    MeshData data{MeshPrimitive::Triangles, {}, indexData, indices, {}, vertexData, {positions}};
    CORRADE_COMPARE(data.indexDataFlags(), DataFlags{});
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlags{});

    std::ostringstream out;
    Error redirectError{&out};
    data.mutableIndexData();
    data.mutableVertexData();
    data.mutableIndices();
    data.mutableIndices<UnsignedShort>();
    data.mutableAttribute(0);
    data.mutableAttribute<Vector2>(0);
    data.mutableAttribute(MeshAttribute::Position);
    data.mutableAttribute<Vector2>(MeshAttribute::Position);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::mutableIndexData(): index data not mutable\n"
        "Trade::MeshData::mutableVertexData(): vertex data not mutable\n"
        "Trade::MeshData::mutableIndices(): index data not mutable\n"
        "Trade::MeshData::mutableIndices(): index data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n");
}

void MeshDataTest::indicesNotIndexed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshData data{MeshPrimitive::Triangles, 37};

    std::ostringstream out;
    Error redirectError{&out};
    data.indexCount();
    data.indexType();
    data.indexOffset();
    data.indices<UnsignedInt>();
    data.mutableIndices<UnsignedShort>();
    data.indicesAsArray();
    UnsignedInt a[1];
    data.indicesInto(a);
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::indexCount(): the mesh is not indexed\n"
        "Trade::MeshData::indexType(): the mesh is not indexed\n"
        "Trade::MeshData::indexOffset(): the mesh is not indexed\n"
        "Trade::MeshData::indices(): the mesh is not indexed\n"
        "Trade::MeshData::mutableIndices(): the mesh is not indexed\n"
        "Trade::MeshData::indicesAsArray(): the mesh is not indexed\n"
        "Trade::MeshData::indicesInto(): the mesh is not indexed\n");
}

void MeshDataTest::indicesWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> indexData{sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    indexView[0] = 57616;
    MeshData data{MeshPrimitive::Points, std::move(indexData), MeshIndexData{indexView}, 57617};

    std::ostringstream out;
    Error redirectError{&out};
    data.indices<UnsignedByte>();
    CORRADE_COMPARE(out.str(), "Trade::MeshData::indices(): improper type requested for MeshIndexType::UnsignedShort\n");
}

void MeshDataTest::attributeNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshAttributeData colors1{MeshAttribute::Color, VertexFormat::Vector3, nullptr};
    MeshAttributeData colors2{MeshAttribute::Color, VertexFormat::Vector4, nullptr};
    MeshData data{MeshPrimitive::Points, nullptr, {colors1, colors2}};

    std::ostringstream out;
    Error redirectError{&out};
    data.attributeName(2);
    data.attributeFormat(2);
    data.attributeOffset(2);
    data.attributeStride(2);
    data.attributeArraySize(2);
    data.attribute(2);
    data.attribute<Vector2>(2);
    data.attributeId(MeshAttribute::Position);
    data.attributeId(MeshAttribute::Color, 2);
    data.attributeFormat(MeshAttribute::Position);
    data.attributeFormat(MeshAttribute::Color, 2);
    data.attributeOffset(MeshAttribute::Position);
    data.attributeOffset(MeshAttribute::Color, 2);
    data.attributeStride(MeshAttribute::Position);
    data.attributeStride(MeshAttribute::Color, 2);
    data.attributeArraySize(MeshAttribute::Position);
    data.attributeArraySize(MeshAttribute::Color, 2);
    data.attribute(MeshAttribute::Position);
    data.attribute(MeshAttribute::Color, 2);
    data.attribute<Vector2>(MeshAttribute::Position);
    data.attribute<Vector2>(MeshAttribute::Color, 2);
    data.positions2DAsArray();
    data.positions3DAsArray();
    data.tangentsAsArray();
    data.bitangentSignsAsArray();
    data.bitangentsAsArray();
    data.normalsAsArray();
    data.textureCoordinates2DAsArray();
    data.colorsAsArray(2);
    data.objectIdsAsArray();
    CORRADE_COMPARE(out.str(),
        "Trade::MeshData::attributeName(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attributeFormat(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attributeOffset(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attributeStride(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attributeArraySize(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 attributes\n"
        "Trade::MeshData::attributeId(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeId(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeFormat(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeFormat(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeOffset(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeOffset(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeStride(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeStride(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeArraySize(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeArraySize(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::positions2DInto(): index 0 out of range for 0 position attributes\n"
        "Trade::MeshData::positions3DInto(): index 0 out of range for 0 position attributes\n"
        "Trade::MeshData::tangentsInto(): index 0 out of range for 0 tangent attributes\n"
        "Trade::MeshData::bitangentSignsInto(): index 0 out of range for 0 tangent attributes\n"
        "Trade::MeshData::bitangentsInto(): index 0 out of range for 0 bitangent attributes\n"
        "Trade::MeshData::normalsInto(): index 0 out of range for 0 normal attributes\n"
        "Trade::MeshData::textureCoordinates2DInto(): index 0 out of range for 0 texture coordinate attributes\n"
        "Trade::MeshData::colorsInto(): index 2 out of range for 2 color attributes\n"
        "Trade::MeshData::objectIdsInto(): index 0 out of range for 0 object ID attributes\n");
}

void MeshDataTest::attributeWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, nullptr};
    MeshData data{MeshPrimitive::Points, nullptr, {positions}};

    std::ostringstream out;
    Error redirectError{&out};
    data.attribute<Vector4>(MeshAttribute::Position);
    CORRADE_COMPARE(out.str(), "Trade::MeshData::attribute(): improper type requested for Trade::MeshAttribute::Position of format VertexFormat::Vector3\n");
}

void MeshDataTest::releaseIndexData() {
    Containers::Array<char> indexData{23};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData.slice(6, 12));

    MeshData data{MeshPrimitive::TriangleStrip, std::move(indexData), MeshIndexData{indexView}, 10};
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexOffset(), 6);

    Containers::Array<char> released = data.releaseIndexData();
    CORRADE_COMPARE(static_cast<void*>(released.data() + 6), indexView.data());
    /* This is not null as we still need the value for calculating offsets */
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), released.data());
    CORRADE_COMPARE(data.indexData().size(), 0);
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 0);
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.indexOffset(), 6);
}

void MeshDataTest::releaseAttributeData() {
    Containers::Array<char> vertexData{16};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);

    MeshAttributeData positions{MeshAttribute::Position, vertexView};
    MeshData data{MeshPrimitive::LineLoop, std::move(vertexData), {positions, positions}};
    CORRADE_COMPARE(data.attributeCount(), 2);

    Containers::Array<MeshAttributeData> released = data.releaseAttributeData();
    CORRADE_COMPARE(released.size(), 2);
    CORRADE_COMPARE(static_cast<const void*>(released[0].data().data()), vertexView.data());
    CORRADE_COMPARE(released[0].data().size(), 2);
    /* Unlike the other two, this is null as we don't need the value for
       calculating anything */
    CORRADE_COMPARE(static_cast<const void*>(data.attributeData()), nullptr);
    CORRADE_COMPARE(data.attributeCount(), 0);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertexView);
    CORRADE_COMPARE(data.vertexCount(), 2);
}

void MeshDataTest::releaseVertexData() {
    Containers::Array<char> vertexData{80};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData.slice(48, 72));

    MeshAttributeData positions{MeshAttribute::Position, vertexView};
    MeshData data{MeshPrimitive::LineLoop, std::move(vertexData), {positions, positions}};
    CORRADE_COMPARE(data.attributeCount(), 2);
    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeOffset(0), 48);

    Containers::Array<char> released = data.releaseVertexData();
    CORRADE_VERIFY(data.attributeData());
    CORRADE_COMPARE(data.attributeCount(), 2);
    CORRADE_COMPARE(static_cast<const void*>(static_cast<const char*>(data.attribute(0).data())), vertexView.data());
    CORRADE_COMPARE(static_cast<const void*>(static_cast<const char*>(data.mutableAttribute(0).data())), vertexView.data());
    /* Returned views should be patched to have zero size (but not the direct
       access, there it stays as it's an internal API really) */
    CORRADE_COMPARE(data.attribute(0).size()[0], 0);
    CORRADE_COMPARE(data.mutableAttribute(0).size()[0], 0);
    CORRADE_COMPARE(data.attributeData()[0].data().size(), 3);
    CORRADE_COMPARE(static_cast<void*>(released.data() + 48), vertexView.data());
    /* This is not null as we still need the value for calculating offsets */
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), released.data());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeOffset(0), 48);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MeshDataTest)
