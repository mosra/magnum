/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>

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
    void debugAttributeNamePacked();

    void constructIndexContiguous();
    void constructIndexStrided();
    void constructIndexStridedWrongStride();
    void constructIndexTypeErasedContiguous();
    void constructIndexTypeErasedContiguousImplementationSpecificFormat();
    void constructIndexTypeErasedContiguousWrongSize();
    void constructIndexTypeErasedStrided();
    void constructIndexTypeErasedStridedImplementationSpecificFormat();
    void constructIndexTypeErasedStridedWrongStride();
    void constructIndex2D();
    void constructIndex2DNotIndexed();
    void constructIndex2DWrongSize();
    void constructIndex2DWrongStride();
    void constructIndex2DNonContiguous();
    void constructIndexNullptr();

    void constructAttribute();
    void constructAttributeMorphTarget();
    void constructAttributeDefault();
    void constructAttributeCustom();
    void constructAttribute2D();
    void constructAttribute2DMorphTarget();
    void constructAttribute2DWrongSize();
    void constructAttribute2DNonContiguous();
    void constructAttributeTypeErased();
    void constructAttributeTypeErasedMorphTarget();
    template<class T> void constructAttributeTypeErasedCharAmbiguity();
    void constructAttributeNullptr();
    void constructAttributeNullptrMorphTarget();
    void constructAttributePadding();
    void constructAttributeNonOwningArray();
    void constructAttributeOffsetOnly();
    void constructAttributeOffsetOnlyMorphTarget();
    void constructAttributeImplementationSpecificFormat();
    void constructAttributeWrongFormat();
    #ifndef CORRADE_TARGET_32BIT
    void constructAttributeWrongSize();
    #endif
    void constructAttributeWrongStride();
    void constructAttributeWrongMorphTargetId();
    void constructAttributeMorphTargetNotAllowed();
    void constructAttributeOnlyArrayAllowed();
    void constructAttributeWrongDataAccess();

    void constructArrayAttribute();
    void constructArrayAttributeMorphTarget();
    void constructArrayAttributeNonContiguous();
    void constructArrayAttribute2D();
    void constructArrayAttribute2DMorphTarget();
    void constructArrayAttribute2DWrongSize();
    void constructArrayAttribute2DNonContiguous();
    void constructArrayAttributeTypeErased();
    void constructArrayAttributeTypeErasedMorphTarget();
    void constructArrayAttributeNullptr();
    void constructArrayAttributeNullptrMorphTarget();
    void constructArrayAttributeOffsetOnly();
    void constructArrayAttributeOffsetOnlyMorphTarget();
    void constructArrayAttributeImplementationSpecificFormat();
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

    void constructImplementationSpecificIndexType();
    void constructImplementationSpecificVertexFormat();
    void constructSpecialIndexStrides();
    void constructSpecialIndexStridesImplementationSpecificIndexType();
    void constructSpecialAttributeStrides();
    void constructSpecialAttributeStridesImplementationSpecificVertexFormat();

    void constructNotOwned();
    void constructIndicesNotOwned();
    void constructVerticesNotOwned();
    void constructIndexlessNotOwned();
    void constructAttributelessNotOwned();

    #ifndef CORRADE_TARGET_32BIT
    void constructIndicesOver4GB();
    void constructAttributeOver4GB();
    #endif

    void constructIndexDataButNotIndexed();
    void constructAttributelessImplicitVertexCount();
    void constructIndicesNotContained();
    void constructAttributeNotContained();
    void constructInconsitentVertexCount();
    void constructDifferentJointIdWeightCount();
    void constructInconsistentJointIdWeightArraySizes();
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
    template<class T> void jointIdsAsArray();
    void jointIdsIntoArrayInvalidSizeStride();
    template<class T> void weightsAsArray();
    template<class T> void weightsAsArrayPackedUnsignedNormalized();
    void weightsIntoArrayInvalidSizeStride();
    template<class T> void objectIdsAsArray();
    void objectIdsIntoArrayInvalidSize();

    void implementationSpecificIndexTypeWrongAccess();
    void implementationSpecificVertexFormatWrongAccess();

    void mutableAccessNotAllowed();

    void indicesNotIndexed();
    void indicesWrongType();

    void attributeNotFound();
    void attributeWrongType();
    void attributeWrongArrayAccess();

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

const struct {
    const char* name;
    UnsignedInt id;
    Int morphTargetId;
} AsArrayData[]{
    {"", 1, -1},
    {"morph target", 0, 37}
};

MeshDataTest::MeshDataTest() {
    addTests({&MeshDataTest::customAttributeName,
              &MeshDataTest::customAttributeNameTooLarge,
              &MeshDataTest::customAttributeNameNotCustom,
              &MeshDataTest::debugAttributeName,
              &MeshDataTest::debugAttributeNamePacked,

              &MeshDataTest::constructIndexContiguous,
              &MeshDataTest::constructIndexStrided,
              &MeshDataTest::constructIndexStridedWrongStride,
              &MeshDataTest::constructIndexTypeErasedContiguous,
              &MeshDataTest::constructIndexTypeErasedContiguousImplementationSpecificFormat,
              &MeshDataTest::constructIndexTypeErasedContiguousWrongSize,
              &MeshDataTest::constructIndexTypeErasedStrided,
              &MeshDataTest::constructIndexTypeErasedStridedImplementationSpecificFormat,
              &MeshDataTest::constructIndexTypeErasedStridedWrongStride,
              &MeshDataTest::constructIndex2D,
              &MeshDataTest::constructIndex2DNotIndexed,
              &MeshDataTest::constructIndex2DWrongSize,
              &MeshDataTest::constructIndex2DWrongStride,
              &MeshDataTest::constructIndex2DNonContiguous,
              &MeshDataTest::constructIndexNullptr,

              &MeshDataTest::constructAttribute,
              &MeshDataTest::constructAttributeMorphTarget,
              &MeshDataTest::constructAttributeDefault,
              &MeshDataTest::constructAttributeCustom,
              &MeshDataTest::constructAttribute2D,
              &MeshDataTest::constructAttribute2DMorphTarget,
              &MeshDataTest::constructAttribute2DWrongSize,
              &MeshDataTest::constructAttribute2DNonContiguous,
              &MeshDataTest::constructAttributeTypeErased,
              &MeshDataTest::constructAttributeTypeErasedMorphTarget,
              &MeshDataTest::constructAttributeTypeErasedCharAmbiguity<char>,
              &MeshDataTest::constructAttributeTypeErasedCharAmbiguity<const char>,
              &MeshDataTest::constructAttributeNullptr,
              &MeshDataTest::constructAttributeNullptrMorphTarget,
              &MeshDataTest::constructAttributePadding,
              &MeshDataTest::constructAttributeNonOwningArray,
              &MeshDataTest::constructAttributeOffsetOnly,
              &MeshDataTest::constructAttributeOffsetOnlyMorphTarget,
              &MeshDataTest::constructAttributeImplementationSpecificFormat,
              &MeshDataTest::constructAttributeWrongFormat,
              #ifndef CORRADE_TARGET_32BIT
              &MeshDataTest::constructAttributeWrongSize,
              #endif
              &MeshDataTest::constructAttributeWrongStride,
              &MeshDataTest::constructAttributeWrongMorphTargetId,
              &MeshDataTest::constructAttributeMorphTargetNotAllowed,
              &MeshDataTest::constructAttributeOnlyArrayAllowed,
              &MeshDataTest::constructAttributeWrongDataAccess,

              &MeshDataTest::constructArrayAttribute,
              &MeshDataTest::constructArrayAttributeMorphTarget,
              &MeshDataTest::constructArrayAttributeNonContiguous,
              &MeshDataTest::constructArrayAttribute2D,
              &MeshDataTest::constructArrayAttribute2DMorphTarget,
              &MeshDataTest::constructArrayAttribute2DWrongSize,
              &MeshDataTest::constructArrayAttribute2DNonContiguous,
              &MeshDataTest::constructArrayAttributeTypeErased,
              &MeshDataTest::constructArrayAttributeTypeErasedMorphTarget,
              &MeshDataTest::constructArrayAttributeNullptr,
              &MeshDataTest::constructArrayAttributeNullptrMorphTarget,
              &MeshDataTest::constructArrayAttributeOffsetOnly,
              &MeshDataTest::constructArrayAttributeOffsetOnlyMorphTarget,
              &MeshDataTest::constructArrayAttributeImplementationSpecificFormat,
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
              &MeshDataTest::constructIndexlessAttributelessZeroVertices,

              &MeshDataTest::constructImplementationSpecificIndexType,
              &MeshDataTest::constructImplementationSpecificVertexFormat,
              &MeshDataTest::constructSpecialIndexStrides,
              &MeshDataTest::constructSpecialIndexStridesImplementationSpecificIndexType,
              &MeshDataTest::constructSpecialAttributeStrides,
              &MeshDataTest::constructSpecialAttributeStridesImplementationSpecificVertexFormat});

    addInstancedTests({&MeshDataTest::constructNotOwned},
        Containers::arraySize(NotOwnedData));
    addInstancedTests({&MeshDataTest::constructIndicesNotOwned,
                       &MeshDataTest::constructVerticesNotOwned,
                       &MeshDataTest::constructIndexlessNotOwned,
                       &MeshDataTest::constructAttributelessNotOwned},
        Containers::arraySize(SingleNotOwnedData));

    #ifndef CORRADE_TARGET_32BIT
    addTests({&MeshDataTest::constructIndicesOver4GB,
              &MeshDataTest::constructAttributeOver4GB});
    #endif

    addTests({&MeshDataTest::constructIndexDataButNotIndexed,
              &MeshDataTest::constructAttributelessImplicitVertexCount,
              &MeshDataTest::constructIndicesNotContained,
              &MeshDataTest::constructAttributeNotContained,
              &MeshDataTest::constructInconsitentVertexCount,
              &MeshDataTest::constructDifferentJointIdWeightCount,
              &MeshDataTest::constructInconsistentJointIdWeightArraySizes,
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
              &MeshDataTest::indicesIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::positions2DAsArray<Vector2>,
        &MeshDataTest::positions2DAsArray<Vector2h>,
        &MeshDataTest::positions2DAsArray<Vector3>,
        &MeshDataTest::positions2DAsArray<Vector3h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::positions2DAsArrayPackedUnsigned<Vector2ub>,
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
              &MeshDataTest::positions2DIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::positions3DAsArray<Vector2>,
        &MeshDataTest::positions3DAsArray<Vector2h>,
        &MeshDataTest::positions3DAsArray<Vector3>,
        &MeshDataTest::positions3DAsArray<Vector3h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::positions3DAsArrayPackedUnsigned<Vector2ub>,
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
              &MeshDataTest::positions3DIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::tangentsAsArray<Vector3>,
        &MeshDataTest::tangentsAsArray<Vector3h>,
        &MeshDataTest::tangentsAsArray<Vector4>,
        &MeshDataTest::tangentsAsArray<Vector4h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector4b>,
              &MeshDataTest::tangentsAsArrayPackedSignedNormalized<Vector4s>,
              &MeshDataTest::tangentsIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::bitangentSignsAsArray<Float>,
        &MeshDataTest::bitangentSignsAsArray<Half>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized<Byte>,
              &MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized<Short>,
              &MeshDataTest::bitangentSignsAsArrayNotFourComponent,
              &MeshDataTest::bitangentSignsIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::bitangentsAsArray<Vector3>,
        &MeshDataTest::bitangentsAsArray<Vector3h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::bitangentsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::bitangentsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::bitangentsIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::normalsAsArray<Vector3>,
        &MeshDataTest::normalsAsArray<Vector3h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::normalsAsArrayPackedSignedNormalized<Vector3b>,
              &MeshDataTest::normalsAsArrayPackedSignedNormalized<Vector3s>,
              &MeshDataTest::normalsIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::textureCoordinates2DAsArray<Vector2>,
        &MeshDataTest::textureCoordinates2DAsArray<Vector2h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned<Vector2ub>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned<Vector2us>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSigned<Vector2b>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSigned<Vector2s>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized<Vector2ub>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized<Vector2us>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized<Vector2b>,
              &MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized<Vector2s>,
              &MeshDataTest::textureCoordinates2DIntoArrayInvalidSize});

    addInstancedTests<MeshDataTest>({
        &MeshDataTest::colorsAsArray<Color3>,
        &MeshDataTest::colorsAsArray<Color3h>,
        &MeshDataTest::colorsAsArray<Color4>,
        &MeshDataTest::colorsAsArray<Color4h>},
        Containers::arraySize(AsArrayData));

    addTests({&MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color3ub>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color3us>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color4ub>,
              &MeshDataTest::colorsAsArrayPackedUnsignedNormalized<Color4us>,
              &MeshDataTest::colorsIntoArrayInvalidSize,
              &MeshDataTest::jointIdsAsArray<UnsignedInt>,
              &MeshDataTest::jointIdsAsArray<UnsignedByte>,
              &MeshDataTest::jointIdsAsArray<UnsignedShort>,
              &MeshDataTest::jointIdsIntoArrayInvalidSizeStride,
              &MeshDataTest::weightsAsArray<Float>,
              &MeshDataTest::weightsAsArray<Half>,
              &MeshDataTest::weightsAsArrayPackedUnsignedNormalized<UnsignedByte>,
              &MeshDataTest::weightsAsArrayPackedUnsignedNormalized<UnsignedShort>,
              &MeshDataTest::weightsIntoArrayInvalidSizeStride,
              &MeshDataTest::objectIdsAsArray<UnsignedByte>,
              &MeshDataTest::objectIdsAsArray<UnsignedShort>,
              &MeshDataTest::objectIdsAsArray<UnsignedInt>,
              &MeshDataTest::objectIdsIntoArrayInvalidSize,

              &MeshDataTest::implementationSpecificIndexTypeWrongAccess,
              &MeshDataTest::implementationSpecificVertexFormatWrongAccess,

              &MeshDataTest::mutableAccessNotAllowed,

              &MeshDataTest::indicesNotIndexed,
              &MeshDataTest::indicesWrongType,

              &MeshDataTest::attributeNotFound,
              &MeshDataTest::attributeWrongType,
              &MeshDataTest::attributeWrongArrayAccess,

              &MeshDataTest::releaseIndexData,
              &MeshDataTest::releaseAttributeData,
              &MeshDataTest::releaseVertexData});
}

void MeshDataTest::customAttributeName() {
    CORRADE_VERIFY(!isMeshAttributeCustom(MeshAttribute::Position));
    CORRADE_VERIFY(!isMeshAttributeCustom(MeshAttribute(32767)));
    CORRADE_VERIFY(isMeshAttributeCustom(MeshAttribute(Implementation::MeshAttributeCustom)));
    CORRADE_VERIFY(isMeshAttributeCustom(MeshAttribute(65535)));

    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(0)), 32768);
    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(8290)), 41058);
    CORRADE_COMPARE(UnsignedShort(meshAttributeCustom(32767)), 65535);

    CORRADE_COMPARE(meshAttributeCustom(MeshAttribute(Implementation::MeshAttributeCustom)), 0);
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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    meshAttributeCustom(32768);
    CORRADE_COMPARE(out, "Trade::meshAttributeCustom(): index 32768 too large\n");
}

void MeshDataTest::customAttributeNameNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    meshAttributeCustom(MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(out, "Trade::meshAttributeCustom(): Trade::MeshAttribute::TextureCoordinates is not custom\n");
}

void MeshDataTest::debugAttributeName() {
    Containers::String out;
    Debug{&out} << MeshAttribute::Position << meshAttributeCustom(73) << MeshAttribute(0x73);
    CORRADE_COMPARE(out, "Trade::MeshAttribute::Position Trade::MeshAttribute::Custom(73) Trade::MeshAttribute(0x73)\n");
}

void MeshDataTest::debugAttributeNamePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << MeshAttribute::Position << Debug::packed << meshAttributeCustom(73) << Debug::packed << MeshAttribute(0x73) << MeshAttribute::Normal;
    CORRADE_COMPARE(out, "Position Custom(73) 0x73 Trade::MeshAttribute::Normal\n");
}

using namespace Math::Literals;

constexpr UnsignedByte IndexBytes[]{25, 132, 3};
constexpr UnsignedShort IndexShorts[]{2575, 13224, 3};
constexpr UnsignedInt IndexInts[]{2110122, 132257, 3};

void MeshDataTest::constructIndexContiguous() {
    {
        const UnsignedByte indexData[]{25, 132, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(indices.data().data(), indexData);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), 1);

        constexpr MeshIndexData cindices{IndexBytes};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(data.data(), IndexBytes);
        CORRADE_COMPARE(data.size(), 3);
        CORRADE_COMPARE(data.stride(), 1);
    } {
        const UnsignedShort indexData[]{2575, 13224, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(indices.data().data(), indexData);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), 2);

        constexpr MeshIndexData cindices{IndexShorts};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(data.data(), IndexShorts);
        CORRADE_COMPARE(data.size(), 3);
        CORRADE_COMPARE(data.stride(), 2);
    } {
        const UnsignedInt indexData[]{2110122, 132257, 3};
        MeshIndexData indices{indexData};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(indices.data().data(), indexData);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), 4);

        constexpr MeshIndexData cindices{IndexInts};
        constexpr MeshIndexType type = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> data = cindices.data();
        CORRADE_COMPARE(type, MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(data.data(), IndexInts);
        CORRADE_COMPARE(data.size(), 3);
        CORRADE_COMPARE(data.stride(), 4);
    }
}

constexpr struct IndexStruct {
    UnsignedByte byteIndex;
    UnsignedShort shortIndex;
    UnsignedInt intIndex;
} IndexStructData[3]{
    {25, 2575, 2110122},
    {132, 13224, 132257},
    {3, 3, 3}
};

void MeshDataTest::constructIndexStrided() {
    const IndexStruct data[3]{
        {25, 2575, 2110122},
        {132, 13224, 132257},
        {3, 3, 3}
    };
    Containers::StridedArrayView1D<const IndexStruct> view = data;

    {
        MeshIndexData indices{view.slice(&IndexStruct::byteIndex)};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(indices.data().data(), &data[0].byteIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));

        constexpr MeshIndexData cindices{Containers::stridedArrayView(IndexStructData, &IndexStructData[0].byteIndex, 3, sizeof(IndexStruct))};
        constexpr MeshIndexType ctype = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> cdata = cindices.data();
        CORRADE_COMPARE(ctype, MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(cdata.data(), &IndexStructData[0].byteIndex);
        CORRADE_COMPARE(cdata.size(), 3);
        CORRADE_COMPARE(cdata.stride(), sizeof(IndexStruct));
    } {
        MeshIndexData indices{view.slice(&IndexStruct::shortIndex)};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(indices.data().data(), &data[0].shortIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));

        constexpr MeshIndexData cindices{Containers::stridedArrayView(IndexStructData, &IndexStructData[0].shortIndex, 3, sizeof(IndexStruct))};
        constexpr MeshIndexType ctype = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> cdata = cindices.data();
        CORRADE_COMPARE(ctype, MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(cdata.data(), &IndexStructData[0].shortIndex);
        CORRADE_COMPARE(cdata.size(), 3);
        CORRADE_COMPARE(cdata.stride(), sizeof(IndexStruct));
    } {
        MeshIndexData indices{view.slice(&IndexStruct::intIndex)};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(indices.data().data(), &data[0].intIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));

        constexpr MeshIndexData cindices{Containers::stridedArrayView(IndexStructData, &IndexStructData[0].intIndex, 3, sizeof(IndexStruct))};
        constexpr MeshIndexType ctype = cindices.type();
        constexpr Containers::StridedArrayView1D<const void> cdata = cindices.data();
        CORRADE_COMPARE(ctype, MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(cdata.data(), &IndexStructData[0].intIndex);
        CORRADE_COMPARE(cdata.size(), 3);
        CORRADE_COMPARE(cdata.stride(), sizeof(IndexStruct));
    }
}

void MeshDataTest::constructIndexStridedWrongStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char toomuch[2*(32768 + 1)];

    /* These should be fine */
    MeshIndexData{Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32767}};
    MeshIndexData{Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32768}.flipped<0>()};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32768}};
    MeshIndexData{Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32769}.flipped<0>()};
    CORRADE_COMPARE(out,
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got 32768\n"
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got -32769\n");
}

void MeshDataTest::constructIndexTypeErasedContiguous() {
    const char indexData[3*2]{};
    MeshIndexData indices{MeshIndexType::UnsignedShort, indexData};
    CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(indices.data().data(), indexData);
    CORRADE_COMPARE(indices.data().size(), 3);
    CORRADE_COMPARE(indices.data().stride(), 2);
}

void MeshDataTest::constructIndexTypeErasedContiguousImplementationSpecificFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indexData[3*2]{};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{meshIndexTypeWrap(0xcaca), indexData};
    CORRADE_COMPARE(out, "Trade::MeshIndexData: can't create index data from a contiguous view and an implementation-specific type 0xcaca, pass a strided view instead\n");
}

void MeshDataTest::constructIndexTypeErasedContiguousWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indexData[3*2]{};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{MeshIndexType::UnsignedInt, indexData};
    CORRADE_COMPARE(out, "Trade::MeshIndexData: view size 6 does not correspond to MeshIndexType::UnsignedInt\n");
}

constexpr const char IndexData[3*4]{};

void MeshDataTest::constructIndexTypeErasedStrided() {
    const char indexData[3*4]{};
    MeshIndexData indices{MeshIndexType::UnsignedShort, {indexData, 3, 4}};
    CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
    CORRADE_VERIFY(indices.data().data() == indexData);
    CORRADE_COMPARE(indices.data().size(), 3);
    CORRADE_COMPARE(indices.data().stride(), 4);

    constexpr MeshIndexData cindices{MeshIndexType::UnsignedShort, {IndexData, 3, 4}};
    constexpr MeshIndexType type = cindices.type();
    constexpr Containers::StridedArrayView1D<const void> data = cindices.data();
    CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(data.data(), IndexData);
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.stride(), 4);
}

void MeshDataTest::constructIndexTypeErasedStridedImplementationSpecificFormat() {
    const char indexData[3*2]{};

    MeshIndexData indices{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const char>{indexData, 3, 2}};
    CORRADE_COMPARE(indices.type(), meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(indices.data().data(), indexData);
    CORRADE_COMPARE(indices.data().size(), 3);
    CORRADE_COMPARE(indices.data().stride(), 2);
}

void MeshDataTest::constructIndexTypeErasedStridedWrongStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char toomuch[2*(32768 + 1)]{};

    /* These should be fine */
    MeshIndexData{MeshIndexType::UnsignedByte, Containers::StridedArrayView1D<const void>{toomuch, 2, 32767}};
    MeshIndexData{MeshIndexType::UnsignedByte, Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32768}.flipped<0>()};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{MeshIndexType::UnsignedByte, Containers::StridedArrayView1D<const void>{toomuch, 2, 32768}};
    MeshIndexData{MeshIndexType::UnsignedByte, Containers::StridedArrayView1D<UnsignedByte>{Containers::arrayCast<UnsignedByte>(toomuch), 2, 32769}.flipped<0>()};
    CORRADE_COMPARE(out,
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got 32768\n"
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got -32769\n");
}

void MeshDataTest::constructIndex2D() {
    const IndexStruct data[3]{
        {25, 2575, 2110122},
        {132, 13224, 132257},
        {3, 3, 3}
    };
    Containers::StridedArrayView1D<const IndexStruct> view = data;

    {
        MeshIndexData indices{Containers::arrayCast<2, const char>(view.slice(&IndexStruct::byteIndex))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedByte);
        CORRADE_COMPARE(indices.data().data(), &data[0].byteIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));
    } {
        MeshIndexData indices{Containers::arrayCast<2, const char>(view.slice(&IndexStruct::shortIndex))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE(indices.data().data(), &data[0].shortIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));
    } {
        MeshIndexData indices{Containers::arrayCast<2, const char>(view.slice(&IndexStruct::intIndex))};
        CORRADE_COMPARE(indices.type(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE(indices.data().data(), &data[0].intIndex);
        CORRADE_COMPARE(indices.data().size(), 3);
        CORRADE_COMPARE(indices.data().stride(), sizeof(IndexStruct));
    }
}

void MeshDataTest::constructIndex2DNotIndexed() {
    MeshIndexData indices{Containers::StridedArrayView2D<const char>{}};
    CORRADE_COMPARE(indices.type(), MeshIndexType{});
    CORRADE_COMPARE(indices.data().data(), nullptr);
}

void MeshDataTest::constructIndex2DWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char data[3*3]{};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView2D<const char>{data, {3, 3}}};
    CORRADE_COMPARE(out, "Trade::MeshIndexData: expected index type size 1, 2 or 4 but got 3\n");
}

void MeshDataTest::constructIndex2DWrongStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char toomuch[2*(32768 + 1)];

    /* These should be fine */
    MeshIndexData{Containers::StridedArrayView2D<char>{toomuch, {2, 1}, {32767, 1}}};
    MeshIndexData{Containers::StridedArrayView2D<char>{toomuch, {2, 1}, {32768, 1}}.flipped<0>()};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView2D<char>{toomuch, {2, 1}, {32768, 1}}};
    MeshIndexData{Containers::StridedArrayView2D<char>{toomuch, {2, 1}, {32769, 1}}.flipped<0>()};
    CORRADE_COMPARE(out,
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got 32768\n"
        "Trade::MeshIndexData: expected stride to fit into 16 bits but got -32769\n");
}

void MeshDataTest::constructIndex2DNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char data[3*4]{};

    /* This should be fine */
    MeshIndexData{Containers::StridedArrayView2D<const char>{data, {3, 2}, {4, 1}}};

    Containers::String out;
    Error redirectError{&out};
    MeshIndexData{Containers::StridedArrayView2D<const char>{data, {3, 2}, {4, 2}}};
    CORRADE_COMPARE(out, "Trade::MeshIndexData: second view dimension is not contiguous\n");
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
    CORRADE_COMPARE(positions.morphTargetId(), -1);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.offset(positionData), 0);
    CORRADE_COMPARE(positions.stride(), sizeof(Vector2));
    CORRADE_VERIFY(positions.data().data() == positionData);

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(Vector2)];
    CORRADE_VERIFY(positions.data(someArray).data() == positionData);

    constexpr MeshAttributeData cpositions{MeshAttribute::Position, Containers::arrayView(Positions)};
    constexpr bool isOffsetOnly = cpositions.isOffsetOnly();
    constexpr UnsignedShort arraySize = cpositions.arraySize();
    constexpr Int morphTargetId = cpositions.morphTargetId();
    constexpr MeshAttribute name = cpositions.name();
    constexpr VertexFormat format = cpositions.format();
    constexpr Short stride = cpositions.stride();
    constexpr Containers::StridedArrayView1D<const void> data = cpositions.data();
    CORRADE_VERIFY(!isOffsetOnly);
    CORRADE_COMPARE(arraySize, 0);
    CORRADE_COMPARE(morphTargetId, -1);
    CORRADE_COMPARE(name, MeshAttribute::Position);
    CORRADE_COMPARE(format, VertexFormat::Vector2);
    CORRADE_COMPARE(stride, sizeof(Vector2));
    CORRADE_COMPARE(data.data(), Positions);
}

void MeshDataTest::constructAttributeMorphTarget() {
    const Vector2 positionData[3];
    MeshAttributeData positions{MeshAttribute::Position, Containers::arrayView(positionData), 15};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), 15);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.offset(positionData), 0);
    CORRADE_COMPARE(positions.stride(), sizeof(Vector2));
    CORRADE_VERIFY(positions.data().data() == positionData);

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(Vector2)];
    CORRADE_VERIFY(positions.data(someArray).data() == positionData);

    constexpr MeshAttributeData cpositions{MeshAttribute::Position, Containers::arrayView(Positions), 15};
    constexpr bool isOffsetOnly = cpositions.isOffsetOnly();
    constexpr UnsignedShort arraySize = cpositions.arraySize();
    constexpr Int morphTargetId = cpositions.morphTargetId();
    constexpr MeshAttribute name = cpositions.name();
    constexpr VertexFormat format = cpositions.format();
    constexpr Short stride = cpositions.stride();
    constexpr Containers::StridedArrayView1D<const void> data = cpositions.data();
    CORRADE_VERIFY(!isOffsetOnly);
    CORRADE_COMPARE(arraySize, 0);
    CORRADE_COMPARE(morphTargetId, 15);
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
    /* Verifying it doesn't hit any assertion about disallowed type for given
       attribute */

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
    CORRADE_COMPARE(positions.morphTargetId(), -1);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.data().data(), positionView.data());
}

void MeshDataTest::constructAttribute2DMorphTarget() {
    char positionData[4*sizeof(Vector2)]{};
    auto positionView = Containers::StridedArrayView2D<char>{positionData,
        {4, sizeof(Vector2)}}.every(2);

    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, positionView, 0, 33};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), 33);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(positions.data().data(), positionView.data());
}

void MeshDataTest::constructAttribute2DWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char positionData[4*sizeof(Vector2)]{};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3,
        Containers::StridedArrayView2D<char>{positionData,
            {4, sizeof(Vector2)}}.every(2)};
    CORRADE_COMPARE(out, "Trade::MeshAttributeData: second view dimension size 8 doesn't match VertexFormat::Vector3\n");
}

void MeshDataTest::constructAttribute2DNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char positionData[4*sizeof(Vector2)]{};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{positionData,
            {2, sizeof(Vector2)*2}}.every({1, 2})};
    CORRADE_COMPARE(out, "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructAttributeTypeErased() {
    const Vector3 positionData[3]{};
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, positionData};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), -1);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector3);
    CORRADE_VERIFY(positions.data().data() == positionData);
}

void MeshDataTest::constructAttributeTypeErasedMorphTarget() {
    const Vector3 positionData[3]{};
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector3, positionData, 0, 101};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), 101);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector3);
    CORRADE_VERIFY(positions.data().data() == positionData);
}

template<class> struct NameTraits;
template<> struct NameTraits<char> {
    static const char* name() { return "char"; }
};
template<> struct NameTraits<const char> {
    static const char* name() { return "const char"; }
};

template<class T> void MeshDataTest::constructAttributeTypeErasedCharAmbiguity() {
    setTestCaseTemplateName(NameTraits<T>::name());

    /* StridedArrayView1D<[const ]char> is convertible to both
       StridedArrayView1D<const void> and StridedArrayView2D<const char>,
       verify the void is preferred. 2D conversion would result in the size
       being {1, 3} which doesn't make sense. */
    char data[3]{};
    Containers::StridedArrayView1D<T> view = data;
    MeshAttributeData attribute{meshAttributeCustom(15), VertexFormat::Byte, view, 0, 33};
    CORRADE_VERIFY(!attribute.isOffsetOnly());
    CORRADE_COMPARE(attribute.arraySize(), 0);
    CORRADE_COMPARE(attribute.morphTargetId(), 33);
    CORRADE_COMPARE(attribute.name(), meshAttributeCustom(15));
    CORRADE_COMPARE(attribute.format(), VertexFormat::Byte);
    /* If the delegation would be wrong, size would be 1 */
    CORRADE_COMPARE(attribute.data().size(), 3);
    CORRADE_VERIFY(attribute.data().data() == data);
}

void MeshDataTest::constructAttributeNullptr() {
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), -1);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_VERIFY(!positions.data().data());
}

void MeshDataTest::constructAttributeNullptrMorphTarget() {
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr, 0, 67};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 0);
    CORRADE_COMPARE(positions.morphTargetId(), 67);
    CORRADE_COMPARE(positions.name(), MeshAttribute::Position);
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_VERIFY(!positions.data().data());
}

void MeshDataTest::constructAttributePadding() {
    MeshAttributeData padding{-35};
    CORRADE_VERIFY(!padding.isOffsetOnly());
    CORRADE_COMPARE(padding.arraySize(), 0);
    CORRADE_COMPARE(padding.morphTargetId(), -1);
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
    CORRADE_COMPARE(a.morphTargetId(), -1);
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
    CORRADE_COMPARE(ca.morphTargetId(), -1);
    CORRADE_COMPARE(ca.name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(ca.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(ca.offset(vertexData), sizeof(Vector2));
    CORRADE_COMPARE(ca.stride(), 2*sizeof(Vector2));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data(vertexData)),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructAttributeOffsetOnlyMorphTarget() {
    struct {
        Vector2 position;
        Vector2 textureCoordinates;
    } vertexData[] {
        {{}, {1.0f, 0.3f}},
        {{}, {0.5f, 0.7f}},
    };

    MeshAttributeData a{MeshAttribute::TextureCoordinates, VertexFormat::Vector2, sizeof(Vector2), 2, 2*sizeof(Vector2), 0, 92};
    CORRADE_VERIFY(a.isOffsetOnly());
    CORRADE_COMPARE(a.arraySize(), 0);
    CORRADE_COMPARE(a.morphTargetId(), 92);
    CORRADE_COMPARE(a.name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(a.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(a.offset(vertexData), sizeof(Vector2));
    CORRADE_COMPARE(a.stride(), 2*sizeof(Vector2));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data(vertexData)),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);

    constexpr MeshAttributeData ca{MeshAttribute::TextureCoordinates, VertexFormat::Vector2, sizeof(Vector2), 2, 2*sizeof(Vector2), 0, 92};
    CORRADE_VERIFY(ca.isOffsetOnly());
    CORRADE_COMPARE(ca.arraySize(), 0);
    CORRADE_COMPARE(ca.morphTargetId(), 92);
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
    MeshAttributeData a{MeshAttribute::Position, vertexFormatWrap(0x3a), positions};
    CORRADE_COMPARE(a.name(), MeshAttribute::Position);
    CORRADE_COMPARE(a.format(), vertexFormatWrap(0x3a));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data()),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructAttributeWrongFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 positionData[3];

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Color, Containers::arrayView(positionData)};
    MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector2, 0, 3, sizeof(Vector2)};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: VertexFormat::Vector2 is not a valid format for Trade::MeshAttribute::Color\n"
        "Trade::MeshAttributeData: VertexFormat::Vector2 is not a valid format for Trade::MeshAttribute::Color\n");
}

#ifndef CORRADE_TARGET_32BIT
void MeshDataTest::constructAttributeWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This should be fine */
    MeshAttributeData{MeshAttribute::Position, Containers::ArrayView<Vector2>{nullptr, 0xffffffffu}};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, Containers::ArrayView<Vector2>{nullptr, 0x100000000ull}};
    /* The offset-only constructors takes the count as an UnsignedInt already,
       nothing to check there */
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: expected vertex count to fit into 32 bits but got 4294967296\n");
}
#endif

void MeshDataTest::constructAttributeWrongStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char toomuch[2*(32768 + sizeof(Vector2))];

    /* These should be fine */
    MeshAttributeData{MeshAttribute::Position, Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32767}};
    MeshAttributeData{MeshAttribute::Position, Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32768}.flipped<0>()};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, 32767};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 65536, 1, -32768};
    MeshAttributeData{32767};
    MeshAttributeData{-32768};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32768}};
    MeshAttributeData{MeshAttribute::Position, Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32769}.flipped<0>()};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, 32768};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 65536, 1, -32769};
    MeshAttributeData{32768};
    MeshAttributeData{-32769};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got 32768\n"
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got -32769\n"
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got 32768\n"
        "Trade::MeshAttributeData: expected stride to fit into 16 bits but got -32769\n"
        "Trade::MeshAttributeData: expected padding to fit into 16 bits but got 32768\n"
        "Trade::MeshAttributeData: expected padding to fit into 16 bits but got -32769\n");
}

void MeshDataTest::constructAttributeWrongMorphTargetId() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 positions[1];

    /* These should be fine */
    MeshAttributeData{MeshAttribute::Position, Containers::arrayView(positions), -1};
    MeshAttributeData{MeshAttribute::Position, Containers::arrayView(positions), 127};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, sizeof(Vector2), 0, -1};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, sizeof(Vector2), 0, 127};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, Containers::arrayView(positions), -56};
    MeshAttributeData{MeshAttribute::Position, Containers::arrayView(positions), 128};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, sizeof(Vector2), 0, -56};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 1, sizeof(Vector2), 0, 128};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got -56\n"
        "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got 128\n"
        "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got -56\n"
        "Trade::MeshAttributeData: expected morph target ID to be either -1 or less than 128 but got 128\n");
}

void MeshDataTest::constructAttributeMorphTargetNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt ids[4];

    /* -1 is allowed */
    MeshAttributeData{MeshAttribute::ObjectId, Containers::arrayView(ids), -1};
    MeshAttributeData{MeshAttribute::JointIds, Containers::stridedArrayView(ids).expanded<0>(Containers::Size2D{1, 4}), -1};
    MeshAttributeData{MeshAttribute::ObjectId, VertexFormat::UnsignedInt, 0, 4, sizeof(UnsignedInt), 0, -1};
    MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedInt, 0, 1, sizeof(UnsignedInt), 4, -1};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::ObjectId, Containers::arrayView(ids), 37};
    MeshAttributeData{MeshAttribute::JointIds, Containers::stridedArrayView(ids).expanded<0>(Containers::Size2D{1, 4}), 37};
    MeshAttributeData{MeshAttribute::ObjectId, VertexFormat::UnsignedInt, 0, 4, sizeof(UnsignedInt), 0, 37};
    MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedInt, 0, 1, sizeof(UnsignedInt), 4, 37};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: morph target not allowed for Trade::MeshAttribute::ObjectId\n"
        "Trade::MeshAttributeData: morph target not allowed for Trade::MeshAttribute::JointIds\n"
        "Trade::MeshAttributeData: morph target not allowed for Trade::MeshAttribute::ObjectId\n"
        "Trade::MeshAttributeData: morph target not allowed for Trade::MeshAttribute::JointIds\n");
}

void MeshDataTest::constructAttributeOnlyArrayAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 data[3];

    /* These should be fine */
    MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float, data, 2};
    MeshAttributeData{meshAttributeCustom(25), VertexFormat::Vector2, data};
    MeshAttributeData{meshAttributeCustom(25), VertexFormat::Float, data, 2};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float, data};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: Trade::MeshAttribute::Weights has to be an array attribute\n");
}

void MeshDataTest::constructAttributeWrongDataAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 positionData[3];
    MeshAttributeData a{MeshAttribute::Position, Containers::arrayView(positionData)};
    MeshAttributeData b{MeshAttribute::Position, VertexFormat::Vector2, 0, 3, sizeof(Vector2)};
    CORRADE_VERIFY(!a.isOffsetOnly());
    CORRADE_VERIFY(b.isOffsetOnly());

    a.data(positionData); /* This is fine, no asserts */

    Containers::String out;
    Error redirectError{&out};
    b.data();
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData::data(): the attribute is offset-only, supply a data array\n");
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
    CORRADE_COMPARE(data.morphTargetId(), -1);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);

    constexpr Containers::StridedArrayView2D<const Vector2> cattribute{ArrayVertexData, {3, 4}};
    constexpr MeshAttributeData cdata{meshAttributeCustom(35), cattribute};
    CORRADE_VERIFY(!cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_COMPARE(cdata.morphTargetId(), -1);
    CORRADE_VERIFY(cdata.data().data() == ArrayVertexData);
    CORRADE_COMPARE(cdata.data().size(), 3);
    CORRADE_COMPARE(cdata.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeMorphTarget() {
    Vector2 vertexData[3*4];
    Containers::StridedArrayView2D<Vector2> attribute{vertexData, {3, 4}};
    MeshAttributeData data{meshAttributeCustom(35), attribute, 23};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), 23);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);

    constexpr Containers::StridedArrayView2D<const Vector2> cattribute{ArrayVertexData, {3, 4}};
    constexpr MeshAttributeData cdata{meshAttributeCustom(35), cattribute, 23};
    CORRADE_VERIFY(!cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_COMPARE(cdata.morphTargetId(), 23);
    CORRADE_VERIFY(cdata.data().data() == ArrayVertexData);
    CORRADE_COMPARE(cdata.data().size(), 3);
    CORRADE_COMPARE(cdata.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 vertexData[4*3]{};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35),
        Containers::StridedArrayView2D<Vector2>{vertexData,
            {4, 3}}.every({1, 2})};
    CORRADE_COMPARE(out, "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructArrayAttribute2D() {
    char vertexData[3*4*sizeof(Vector2)];
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, Containers::StridedArrayView2D<char>{vertexData, {3, 4*sizeof(Vector2)}}, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), -1);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttribute2DMorphTarget() {
    char vertexData[3*4*sizeof(Vector2)];
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, Containers::StridedArrayView2D<char>{vertexData, {3, 4*sizeof(Vector2)}}, 4, 77};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), 77);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttribute2DWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char vertexData[3*4*sizeof(Vector2)]{};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{vertexData,
            {3, 4*sizeof(Vector2)}}, 3};
    CORRADE_COMPARE(out, "Trade::MeshAttributeData: second view dimension size 32 doesn't match VertexFormat::Vector2 and array size 3\n");
}

void MeshDataTest::constructArrayAttribute2DNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char vertexData[4*3*sizeof(Vector2)]{};

    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{meshAttributeCustom(35), VertexFormat::Vector2,
        Containers::StridedArrayView2D<char>{vertexData,
            {3, sizeof(Vector2)*4}}.every({1, 2}), 2};
    CORRADE_COMPARE(out, "Trade::MeshAttributeData: second view dimension is not contiguous\n");
}

void MeshDataTest::constructArrayAttributeTypeErased() {
    Vector2 vertexData[3*4];
    Containers::StridedArrayView1D<Vector2> attribute{vertexData, 3, 4*sizeof(Vector2)};
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, attribute, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), -1);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeTypeErasedMorphTarget() {
    Vector2 vertexData[3*4];
    Containers::StridedArrayView1D<Vector2> attribute{vertexData, 3, 4*sizeof(Vector2)};
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, attribute, 4, 99};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), 99);
    CORRADE_VERIFY(data.data().data() == vertexData);
    CORRADE_COMPARE(data.data().size(), 3);
    CORRADE_COMPARE(data.data().stride(), sizeof(Vector2)*4);
}

void MeshDataTest::constructArrayAttributeNullptr() {
    MeshAttributeData positions{meshAttributeCustom(35), VertexFormat::Vector2, nullptr, 4};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 4);
    CORRADE_COMPARE(positions.morphTargetId(), -1);
    CORRADE_COMPARE(positions.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(positions.format(), VertexFormat::Vector2);
    CORRADE_VERIFY(!positions.data().data());
}

void MeshDataTest::constructArrayAttributeNullptrMorphTarget() {
    MeshAttributeData positions{meshAttributeCustom(35), VertexFormat::Vector2, nullptr, 4, 37};
    CORRADE_VERIFY(!positions.isOffsetOnly());
    CORRADE_COMPARE(positions.arraySize(), 4);
    CORRADE_COMPARE(positions.morphTargetId(), 37);
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
    CORRADE_COMPARE(data.morphTargetId(), -1);

    Vector2 vertexData[1 + 3*4];
    CORRADE_VERIFY(data.data(vertexData).data() == vertexData + 1);
    CORRADE_COMPARE(data.data(vertexData).size(), 3);
    CORRADE_COMPARE(data.data(vertexData).stride(), sizeof(Vector2));

    constexpr MeshAttributeData cdata{meshAttributeCustom(35), VertexFormat::Vector2, sizeof(Vector2), 3, sizeof(Vector2), 4};
    CORRADE_VERIFY(cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_COMPARE(cdata.morphTargetId(), -1);
}

void MeshDataTest::constructArrayAttributeOffsetOnlyMorphTarget() {
    MeshAttributeData data{meshAttributeCustom(35), VertexFormat::Vector2, sizeof(Vector2), 3, sizeof(Vector2), 4, 44};
    CORRADE_VERIFY(data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(data.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.morphTargetId(), 44);

    Vector2 vertexData[1 + 3*4];
    CORRADE_VERIFY(data.data(vertexData).data() == vertexData + 1);
    CORRADE_COMPARE(data.data(vertexData).size(), 3);
    CORRADE_COMPARE(data.data(vertexData).stride(), sizeof(Vector2));

    constexpr MeshAttributeData cdata{meshAttributeCustom(35), VertexFormat::Vector2, sizeof(Vector2), 3, sizeof(Vector2), 4, 44};
    CORRADE_VERIFY(cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(cdata.format(), VertexFormat::Vector2);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_COMPARE(cdata.morphTargetId(), 44);
}

void MeshDataTest::constructArrayAttributeImplementationSpecificFormat() {
    Vector2 positions[]{{1.0f, 0.3f}, {0.5f, 0.7f}};

    /* This should not fire any asserts */
    MeshAttributeData a{meshAttributeCustom(35), vertexFormatWrap(0x3a), positions, 2};
    CORRADE_COMPARE(a.name(), meshAttributeCustom(35));
    CORRADE_COMPARE(a.format(), vertexFormatWrap(0x3a));
    CORRADE_COMPARE(a.arraySize(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.data()),
        Containers::arrayView<Vector2>({{1.0f, 0.3f}, {0.5f, 0.7f}}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructArrayAttributeNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

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
    MeshAttributeData{meshAttributeCustom(35), vertexFormatWrap(0xdead), positions, 3};
    MeshAttributeData{meshAttributeCustom(35), vertexFormatWrap(0xdead), 0, 3, 6*sizeof(Vector2), 3};

    /* This is not */
    Containers::String out;
    Error redirectError{&out};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2b, positions, 3};
    MeshAttributeData{MeshAttribute::Position, positions2D};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, positions2Dchar, 3};
    MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 0, 3, 6*sizeof(Vector2), 3};
    CORRADE_COMPARE(out,
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n"
        "Trade::MeshAttributeData: Trade::MeshAttribute::Position can't be an array attribute\n");
}

void MeshDataTest::construct() {
    auto&& instanceData = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 textureCoordinate;
        Short id[2];
        Vector2 positionMorphTarget;
        Byte idMorphTarget[3];
    };

    Containers::Array<char> indexData{8*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData).slice(1, 7);
    Utility::copy({0, 1, 2, 0, 2, 1}, indices);

    /* Enough vertex data to fit also the case with large explicit vertex count
       (but fill just the first 3, as those are only tested) */
    Containers::Array<char> vertexData{17*sizeof(Vertex)};
    auto vertices = stridedArrayView(Containers::arrayCast<Vertex>(vertexData)).prefix(3);
    Utility::copy({
        {{0.1f, 0.2f, 0.3f},
         Vector3::xAxis(),
         {0.000f, 0.125f},
         {15, 74},
         {0.3f, 0.1f},
         {74, 0, 15}},
        {{0.4f, 0.5f, 0.6f},
         Vector3::yAxis(),
         {0.250f, 0.375f},
         {-374, 2},
         {0.6f, 0.4f},
         {2, 0, -37}},
        {{0.7f, 0.8f, 0.9f},
         Vector3::zAxis(),
         {0.500f, 0.625f},
         {22, -1},
         {0.9f, 0.7f},
         {-1, 0, 22}}
    }, vertices);

    if(instanceData.vertexCount < 3)
        vertices = vertices.prefix(instanceData.vertexCount);

    int importerState;
    MeshData data{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        /* Texture coordinates deliberately twice (though aliased) */
        Utility::move(vertexData), {
            MeshAttributeData{MeshAttribute::Position,
                vertices.slice(&Vertex::position)},
            MeshAttributeData{MeshAttribute::TextureCoordinates,
                vertices.slice(&Vertex::textureCoordinate)},
            /* Offset-only */
            MeshAttributeData{MeshAttribute::Normal,
                VertexFormat::Vector3, offsetof(Vertex, normal),
                UnsignedInt(vertices.size()), sizeof(Vertex)},
            /* Texture coordinates deliberately twice (though aliased) */
            MeshAttributeData{MeshAttribute::TextureCoordinates,
                vertices.slice(&Vertex::textureCoordinate)},
            /* Custom & array */
            MeshAttributeData{meshAttributeCustom(13),
                Containers::arrayCast<2, Short>(vertices.slice(&Vertex::id))},
            /* Positions as a morph target */
            MeshAttributeData{MeshAttribute::Position,
                vertices.slice(&Vertex::positionMorphTarget), 37},
            /* Array as a morph target */
            MeshAttributeData{meshAttributeCustom(13),
                Containers::arrayCast<2, Byte>(vertices.slice(&Vertex::idMorphTarget)), 37},
            /* Positions as a morph target again */
            MeshAttributeData{MeshAttribute::Position,
                vertices.slice(&Vertex::positionMorphTarget), 37},
        },
        instanceData.vertexCount, &importerState};

    /* Basics */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(!data.attributeData().isEmpty());
    CORRADE_COMPARE(static_cast<const void*>(data.indexData() + 2), indices.data());
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertices.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableIndexData() + 2), indices.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableVertexData()), vertices.data());
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
    CORRADE_COMPARE(data.attributeCount(), 8);
    CORRADE_COMPARE(data.attributeCount(-1), 5);
    CORRADE_COMPARE(data.attributeCount(1), 0);
    CORRADE_COMPARE(data.attributeCount(37), 3);
    CORRADE_COMPARE(data.attributeName(0), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeName(1), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeName(2), MeshAttribute::Normal);
    CORRADE_COMPARE(data.attributeName(3), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeName(4), meshAttributeCustom(13));
    CORRADE_COMPARE(data.attributeName(5), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeName(6), meshAttributeCustom(13));
    CORRADE_COMPARE(data.attributeName(7), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeId(0), 0);
    CORRADE_COMPARE(data.attributeId(1), 0);
    CORRADE_COMPARE(data.attributeId(2), 0);
    CORRADE_COMPARE(data.attributeId(3), 1);
    CORRADE_COMPARE(data.attributeId(4), 0);
    CORRADE_COMPARE(data.attributeId(5), 0);
    CORRADE_COMPARE(data.attributeId(6), 0);
    CORRADE_COMPARE(data.attributeId(7), 1);
    CORRADE_COMPARE(data.attributeFormat(0), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(1), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(2), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeFormat(3), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(4), VertexFormat::Short);
    CORRADE_COMPARE(data.attributeFormat(5), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(6), VertexFormat::Byte);
    CORRADE_COMPARE(data.attributeFormat(7), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeOffset(1), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(2), sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(3), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(4), 2*sizeof(Vector3) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeOffset(5), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short));
    CORRADE_COMPARE(data.attributeOffset(6), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeOffset(7), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short));
    CORRADE_COMPARE(data.attributeStride(0), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(1), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(2), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(3), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(4), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(5), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(6), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(7), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeArraySize(0), 0);
    CORRADE_COMPARE(data.attributeArraySize(1), 0);
    CORRADE_COMPARE(data.attributeArraySize(2), 0);
    CORRADE_COMPARE(data.attributeArraySize(3), 0);
    CORRADE_COMPARE(data.attributeArraySize(4), 2);
    CORRADE_COMPARE(data.attributeArraySize(5), 0);
    CORRADE_COMPARE(data.attributeArraySize(6), 3);
    CORRADE_COMPARE(data.attributeArraySize(7), 0);
    CORRADE_COMPARE(data.attributeMorphTargetId(0), -1);
    CORRADE_COMPARE(data.attributeMorphTargetId(1), -1);
    CORRADE_COMPARE(data.attributeMorphTargetId(2), -1);
    CORRADE_COMPARE(data.attributeMorphTargetId(3), -1);
    CORRADE_COMPARE(data.attributeMorphTargetId(4), -1);
    CORRADE_COMPARE(data.attributeMorphTargetId(5), 37);
    CORRADE_COMPARE(data.attributeMorphTargetId(6), 37);
    CORRADE_COMPARE(data.attributeMorphTargetId(7), 37);

    /* Raw attribute data access by ID */
    CORRADE_COMPARE(data.attributeData(1).name(), MeshAttribute::TextureCoordinates);
    CORRADE_COMPARE(data.attributeData(1).format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeData(1).data().size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeData(1).arraySize(), 0);
    CORRADE_COMPARE(data.attributeData(1).morphTargetId(), -1);
    if(instanceData.vertexCount)
        CORRADE_COMPARE(Containers::arrayCast<const Vector2>(data.attributeData(1).data())[1], (Vector2{0.250f, 0.375f}));
    /* Offset-only */
    CORRADE_COMPARE(data.attributeData(2).name(), MeshAttribute::Normal);
    CORRADE_COMPARE(data.attributeData(2).format(), VertexFormat::Vector3);
    CORRADE_COMPARE(data.attributeData(2).data().size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeData(2).arraySize(), 0);
    CORRADE_COMPARE(data.attributeData(2).morphTargetId(), -1);
    if(instanceData.vertexCount)
        CORRADE_COMPARE(Containers::arrayCast<const Vector3>(data.attributeData(2).data())[1], Vector3::yAxis());
    /* Array */
    CORRADE_COMPARE(data.attributeData(4).name(), meshAttributeCustom(13));
    CORRADE_COMPARE(data.attributeData(4).format(), VertexFormat::Short);
    CORRADE_COMPARE(data.attributeData(4).data().size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeData(4).arraySize(), 2);
    CORRADE_COMPARE(data.attributeData(4).morphTargetId(), -1);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(data.attributeData(4).data(), 2))[1][0], -374);
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(data.attributeData(4).data(), 2))[1][1], 2);
    }
    /* Morph target. No special treatment in this case. */
    CORRADE_COMPARE(data.attributeData(5).name(), MeshAttribute::Position);
    CORRADE_COMPARE(data.attributeData(5).format(), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeData(5).data().size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeData(5).arraySize(), 0);
    CORRADE_COMPARE(data.attributeData(5).morphTargetId(), 37);
    if(instanceData.vertexCount)
        CORRADE_COMPARE(Containers::arrayCast<const Vector2>(data.attributeData(5).data())[1], (Vector2{0.6f, 0.4f}));
    /* Morph target array. No special treatment in this case. */
    CORRADE_COMPARE(data.attributeData(6).name(), meshAttributeCustom(13));
    CORRADE_COMPARE(data.attributeData(6).format(), VertexFormat::Byte);
    CORRADE_COMPARE(data.attributeData(6).data().size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.attributeData(6).arraySize(), 3);
    CORRADE_COMPARE(data.attributeData(6).morphTargetId(), 37);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(data.attributeData(6).data(), 3))[1][0], 2);
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(data.attributeData(6).data(), 3))[1][2], -37);
    }

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
        /* Array */
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(
            data.attribute(4))[0])[0], 15);
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(
            data.attribute(4))[0])[1], 74);
        /* Morph target. No special treatment in case of by ID access. */
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(5))[2]), (Vector2{0.9f, 0.7f}));
        /* Morph target array. No special treatment in case of by ID access. */
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.attribute(6))[0])[0], 74);
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.attribute(6))[0])[2], 15);

        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(0))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(1))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(2))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(3))[1]), (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE((Containers::arrayCast<2, Short>(
            data.mutableAttribute(4))[0])[0], 15);
        CORRADE_COMPARE((Containers::arrayCast<2, Short>(
            data.mutableAttribute(4))[0])[1], 74);
        /* Morph target. No special treatment in case of by ID access. */
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(5))[2]), (Vector2{0.9f, 0.7f}));
        /* Morph target array. No special treatment in case of by ID access. */
        CORRADE_COMPARE((Containers::arrayCast<2, Byte>(
            data.mutableAttribute(6))[0])[0], 74);
        CORRADE_COMPARE((Containers::arrayCast<2, Byte>(
            data.mutableAttribute(6))[0])[2], 15);
    }

    /* Typed access by ID */
    CORRADE_COMPARE(data.attribute<Vector3>(0).size(), instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute<Vector3>(0).size(), instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3>(0)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.attribute<Vector2>(1)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.attribute<Vector3>(2)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.attribute<Vector2>(3)[1], (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE(data.attribute<Short[]>(4)[1][0], -374);
        CORRADE_COMPARE(data.attribute<Short[]>(4)[1][1], 2);
        /* Morph target. No special treatment in case of by ID access. */
        CORRADE_COMPARE(data.attribute<Vector2>(5)[2], (Vector2{0.9f, 0.7f}));
        /* Morph target array. No special treatment in case of by ID access. */
        CORRADE_COMPARE(data.attribute<Byte[]>(6)[1][0], 2);
        CORRADE_COMPARE(data.attribute<Byte[]>(6)[1][2], -37);

        CORRADE_COMPARE(data.mutableAttribute<Vector3>(0)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(1)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(2)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(3)[1], (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE(data.mutableAttribute<Short[]>(4)[1][0], -374);
        CORRADE_COMPARE(data.mutableAttribute<Short[]>(4)[1][1], 2);
        /* Morph target. No special treatment in this case. */
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(5)[2], (Vector2{0.9f, 0.7f}));
        /* Morph target array. No special treatment in case of by ID access. */
        CORRADE_COMPARE(data.mutableAttribute<Byte[]>(6)[1][0], 2);
        CORRADE_COMPARE(data.mutableAttribute<Byte[]>(6)[1][2], -37);
    }

    /* Accessing a non-array attribute as an array should be possible as well
       -- the second dimension is then just 1 */
    CORRADE_COMPARE(data.attribute<Vector3[]>(0).size(), (Containers::Size2D{instanceData.expectedVertexCount, 1}));
    CORRADE_COMPARE(data.mutableAttribute<Vector3[]>(0).size(), (Containers::Size2D{instanceData.expectedVertexCount, 1}));
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3[]>(0)[1][0], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3[]>(0)[1][0], (Vector3{0.4f, 0.5f, 0.6f}));
    }

    /* Attribute access by name */
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::Position));
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::Normal));
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::TextureCoordinates));
    CORRADE_VERIFY(data.hasAttribute(meshAttributeCustom(13)));
    CORRADE_VERIFY(data.hasAttribute(MeshAttribute::Position, 37));
    CORRADE_VERIFY(data.hasAttribute(meshAttributeCustom(13), 37));
    CORRADE_VERIFY(!data.hasAttribute(MeshAttribute::Color));
    CORRADE_VERIFY(!data.hasAttribute(meshAttributeCustom(23)));
    CORRADE_VERIFY(!data.hasAttribute(MeshAttribute::TextureCoordinates, 37));
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Position), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Normal), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::TextureCoordinates), 2);
    CORRADE_COMPARE(data.attributeCount(meshAttributeCustom(13)), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Position, 37), 2);
    CORRADE_COMPARE(data.attributeCount(meshAttributeCustom(13), 37), 1);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Color), 0);
    CORRADE_COMPARE(data.attributeCount(meshAttributeCustom(23)), 0);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::TextureCoordinates, 37), 0);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Normal), 2);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Normal), 2);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::TextureCoordinates), 1);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::TextureCoordinates), 1);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::TextureCoordinates, 1), 3);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::TextureCoordinates, 1), 3);
    CORRADE_COMPARE(data.findAttributeId(meshAttributeCustom(13)), 4);
    CORRADE_COMPARE(data.attributeId(meshAttributeCustom(13)), 4);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Position, 0, 37), 5);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Position, 0, 37), 5);
    CORRADE_COMPARE(data.findAttributeId(meshAttributeCustom(13), 0, 37), 6);
    CORRADE_COMPARE(data.attributeId(meshAttributeCustom(13), 0, 37), 6);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Position, 1, 37), 7);
    CORRADE_COMPARE(data.attributeId(MeshAttribute::Position, 1, 37), 7);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Color), Containers::NullOpt);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::TextureCoordinates, 2), Containers::NullOpt);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Position, 2, 37), Containers::NullOpt);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::TextureCoordinates, 0, 37), Containers::NullOpt);
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
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position, 0, 37),
        VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeFormat(meshAttributeCustom(13), 0, 37),
        VertexFormat::Byte);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position, 1, 37),
        VertexFormat::Vector2);
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Normal), sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::TextureCoordinates, 0), 2*sizeof(Vector3));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::TextureCoordinates, 1), 2*sizeof(Vector3));    CORRADE_COMPARE(data.attributeOffset(meshAttributeCustom(13)), 2*sizeof(Vector3) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Position, 0, 37), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short)); CORRADE_COMPARE(data.attributeOffset(meshAttributeCustom(13), 0, 37), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short) + sizeof(Vector2));
    CORRADE_COMPARE(data.attributeOffset(MeshAttribute::Position, 1, 37), 2*sizeof(Vector3) + sizeof(Vector2) + 2*sizeof(Short));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Position), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Normal), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::TextureCoordinates, 0), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::TextureCoordinates, 1), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(meshAttributeCustom(13)), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Position, 0, 37), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(meshAttributeCustom(13), 0, 37), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeStride(MeshAttribute::Position, 1, 37), sizeof(Vertex));
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Normal), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::TextureCoordinates, 0), 0);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::TextureCoordinates, 1), 0);
    CORRADE_COMPARE(data.attributeArraySize(meshAttributeCustom(13)), 2);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Position, 0, 37), 0);
    CORRADE_COMPARE(data.attributeArraySize(meshAttributeCustom(13), 0, 37), 3);
    CORRADE_COMPARE(data.attributeArraySize(MeshAttribute::Position, 1, 37), 0);

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
        /* Array */
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(
            data.attribute(meshAttributeCustom(13)))[1])[0], -374);
        CORRADE_COMPARE((Containers::arrayCast<2, const Short>(
            data.attribute(meshAttributeCustom(13)))[1])[1], 2);
        /* Morph target */
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(MeshAttribute::Position, 0, 37))[1]), (Vector2{0.6f, 0.4f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.attribute(MeshAttribute::Position, 1, 37))[2]), (Vector2{0.9f, 0.7f}));
        /* Array morph target */
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.attribute(meshAttributeCustom(13), 0, 37))[1])[0], 2);
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.attribute(meshAttributeCustom(13), 0, 37))[1])[2], -37);

        CORRADE_COMPARE((Containers::arrayCast<1, const Vector3>(
            data.mutableAttribute(MeshAttribute::Position))[1]), (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector3>(
            data.mutableAttribute(MeshAttribute::Normal))[2]), Vector3::zAxis());
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(MeshAttribute::TextureCoordinates, 0))[0]), (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(
            data.mutableAttribute(MeshAttribute::TextureCoordinates, 1))[1]), (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE((Containers::arrayCast<2, Short>(
            data.mutableAttribute(meshAttributeCustom(13)))[1])[0], -374);
        CORRADE_COMPARE((Containers::arrayCast<2, Short>(
            data.mutableAttribute(meshAttributeCustom(13)))[1])[1], 2);
        /* Morph target */
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.mutableAttribute(MeshAttribute::Position, 0, 37))[1]), (Vector2{0.6f, 0.4f}));
        CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(
            data.mutableAttribute(MeshAttribute::Position, 1, 37))[2]), (Vector2{0.9f, 0.7f}));
        /* Array morph target */
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.mutableAttribute(meshAttributeCustom(13), 0, 37))[1])[0], 2);
        CORRADE_COMPARE((Containers::arrayCast<2, const Byte>(
            data.mutableAttribute(meshAttributeCustom(13), 0, 37))[1])[2], -37);
    }

    /* Typed access by name */
    CORRADE_COMPARE(data.attribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    CORRADE_COMPARE(data.mutableAttribute(MeshAttribute::Position).size()[0], instanceData.expectedVertexCount);
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3>(MeshAttribute::Position)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.attribute<Vector3>(MeshAttribute::Normal)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::TextureCoordinates, 0)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::TextureCoordinates, 1)[1], (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE(data.attribute<Short[]>(meshAttributeCustom(13))[2][0], 22);
        CORRADE_COMPARE(data.attribute<Short[]>(meshAttributeCustom(13))[2][1], -1);
        /* Morph target */
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::Position, 0, 37)[0], (Vector2{0.3f, 0.1f}));
        CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::Position, 1, 37)[2], (Vector2{0.9f, 0.7f}));
        /* Morph target array */
        CORRADE_COMPARE(data.attribute<Byte[]>(meshAttributeCustom(13), 0, 37)[2][0], -1);
        CORRADE_COMPARE(data.attribute<Byte[]>(meshAttributeCustom(13), 0, 37)[2][2], 22);

        CORRADE_COMPARE(data.mutableAttribute<Vector3>(MeshAttribute::Position)[1], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3>(MeshAttribute::Normal)[2], Vector3::zAxis());
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::TextureCoordinates, 0)[0], (Vector2{0.000f, 0.125f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::TextureCoordinates, 1)[1], (Vector2{0.250f, 0.375f}));
        /* Array */
        CORRADE_COMPARE(data.mutableAttribute<Short[]>(meshAttributeCustom(13))[2][0], 22);
        CORRADE_COMPARE(data.mutableAttribute<Short[]>(meshAttributeCustom(13))[2][1], -1);
        /* Morph target */
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::Position, 0, 37)[0], (Vector2{0.3f, 0.1f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector2>(MeshAttribute::Position, 1, 37)[2], (Vector2{0.9f, 0.7f}));
        /* Morph target array */
        CORRADE_COMPARE(data.mutableAttribute<Byte[]>(meshAttributeCustom(13), 0, 37)[2][0], -1);
        CORRADE_COMPARE(data.mutableAttribute<Byte[]>(meshAttributeCustom(13), 0, 37)[2][2], 22);
    }

    /* Accessing a non-array attribute as an array should be possible as well
       -- the second dimension is then just 1 */
    CORRADE_COMPARE(data.attribute<Vector3[]>(MeshAttribute::Position).size(), (Containers::Size2D{instanceData.expectedVertexCount, 1}));
    CORRADE_COMPARE(data.mutableAttribute<Vector3[]>(MeshAttribute::Position).size(), (Containers::Size2D{instanceData.expectedVertexCount, 1}));
    if(instanceData.vertexCount) {
        CORRADE_COMPARE(data.attribute<Vector3[]>(MeshAttribute::Position)[1][0], (Vector3{0.4f, 0.5f, 0.6f}));
        CORRADE_COMPARE(data.mutableAttribute<Vector3[]>(MeshAttribute::Position)[1][0], (Vector3{0.4f, 0.5f, 0.6f}));
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
    MeshData data{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        Utility::move(vertexData), {
            MeshAttributeData{MeshAttribute::Position, vertices}
        }};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE(data.indexCount(), 0);
    CORRADE_COMPARE(data.indices().size(), (Containers::Size2D{0, 4}));
    CORRADE_COMPARE(data.mutableIndices().size(), (Containers::Size2D{0, 4}));
    CORRADE_COMPARE(data.vertexCount(), 3);
}

void MeshDataTest::constructZeroAttributes() {
    /* This is a valid use case because e.g. the index/vertex data can be
       shared by multiple meshes and this particular one is just a plain
       index array */
    Containers::Array<char> indexData{3*sizeof(UnsignedInt)};
    Containers::Array<char> vertexData{3};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    MeshData data{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        Utility::move(vertexData), {}, 15};

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
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    MeshData data{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        nullptr, {
            MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        }};

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
    auto vertices = Containers::arrayCast<Vector2>(vertexData);
    Utility::copy({
        {0.1f, 0.2f},
        {0.4f, 0.5f},
        {0.7f, 0.8f}
    }, vertices);

    int importerState;
    MeshData data{MeshPrimitive::LineLoop, Utility::move(vertexData), {
        MeshAttributeData{MeshAttribute::Position, vertices}
    }, MeshData::ImplicitVertexCount, &importerState};
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
    CORRADE_COMPARE(data.indexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.indices().data(), nullptr);
    CORRADE_COMPARE(data.indices().size(), (Containers::Size2D{0, 0}));
    CORRADE_COMPARE(data.mutableIndices().data(), nullptr);
    CORRADE_COMPARE(data.mutableIndices().size(), (Containers::Size2D{0, 0}));

    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position), VertexFormat::Vector2);
    CORRADE_COMPARE(data.attribute<Vector2>(MeshAttribute::Position)[1], (Vector2{0.4f, 0.5f}));
}

void MeshDataTest::constructIndexlessZeroVertices() {
    MeshData data{MeshPrimitive::LineLoop, nullptr, {
        MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, nullptr}
    }};
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(data.indexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.vertexData(), static_cast<const void*>(nullptr));

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeCount(), 1);
    CORRADE_COMPARE(data.attributeFormat(MeshAttribute::Position), VertexFormat::Vector2);
}

void MeshDataTest::constructAttributeless() {
    Containers::Array<char> indexData{6*sizeof(UnsignedInt)};
    auto indices = Containers::arrayCast<UnsignedInt>(indexData);
    Utility::copy({0, 1, 2, 0, 2, 1}, indices);

    int importerState;
    MeshData data{MeshPrimitive::TriangleStrip,
        Utility::move(indexData), MeshIndexData{indices},
        3, &importerState};
    /* These are empty so it doesn't matter, but this is a nice non-restrictive
       default */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.vertexData(), static_cast<const void*>(nullptr));
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

void MeshDataTest::constructIndexlessAttributeless() {
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    MeshData data{MeshPrimitive::TriangleStrip, 37, &state};
    /* These are both empty so it doesn't matter, but this is a nice
       non-restrictive default */
    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.indexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.vertexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 37);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

void MeshDataTest::constructIndexlessAttributelessZeroVertices() {
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    MeshData data{MeshPrimitive::TriangleStrip, 0, &state};
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.indexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.vertexData(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.importerState(), &state);

    CORRADE_VERIFY(!data.isIndexed());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeCount(), 0);
}

/* MSVC 2015 doesn't like anonymous bitfields in inline structs, so putting the
   declaration outside */
struct VertexWithImplementationSpecificData {
    Long:64;
    /* Using some definitely not a vertex format to test there's no weird
       compile-time assertion preventing this */
    long double thing;
};

void MeshDataTest::constructImplementationSpecificIndexType() {
    /* Using some definitely not an index type to test there's no weird
       compile-time assertion preventing this. Also using a strided view to
       have the same case as with implementation-specific vertex formats
       below -- for an implementation-specific type it's always strided,
       anyway. */
    VertexWithImplementationSpecificData indexData[]{{12.3l}, {34.5l}, {45.6l}};

    /* Constructing should work w/o asserts */
    Containers::StridedArrayView1D<long double> indices{indexData,
        &indexData[0].thing, 3, sizeof(VertexWithImplementationSpecificData)};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Mutable, indexData,
        MeshIndexData{meshIndexTypeWrap(0xcaca), indices}, 1};

    /* Getting typeless indices should work also */
    CORRADE_COMPARE(data.indexType(), meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexStride(), sizeof(VertexWithImplementationSpecificData));

    /* The actual type size is unknown, so this will use the full stride */
    CORRADE_COMPARE(data.indices().size()[1], sizeof(VertexWithImplementationSpecificData));

    CORRADE_COMPARE_AS((Containers::arrayCast<1, const long double>(
        data.indices().prefix({3, sizeof(long double)}))),
        indices, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const long double>(
        data.mutableIndices().prefix({3, sizeof(long double)}))),
        indices, TestSuite::Compare::Container);
}

void MeshDataTest::constructImplementationSpecificVertexFormat() {
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
            vertexFormatWrap(0xdead4), attribute},
        MeshAttributeData{meshAttributeCustom(35),
            vertexFormatWrap(0xdead5), attribute, 27}
    }};

    /* Getting typeless attribute should work also */
    UnsignedInt format = 0xdead1;
    for(MeshAttribute name: {MeshAttribute::Position,
                             MeshAttribute::Normal,
                             MeshAttribute::TextureCoordinates,
                             MeshAttribute::Color,
                             meshAttributeCustom(35)}) {
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

void MeshDataTest::constructSpecialIndexStrides() {
    /* Every second index */
    {
        Containers::Array<char> indexData{sizeof(UnsignedShort)*8};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        Utility::copy({1, 0, 2, 0, 3, 0, 4, 0}, indices);
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{indices.every(2)}, 1};

        CORRADE_COMPARE(mesh.indexStride(), 4);

        /* Type-erased access with a cast later */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.indices())),
            Containers::arrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableIndices())),
            Containers::stridedArrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);

        /* Typed access */
        CORRADE_COMPARE_AS(mesh.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.mutableIndices<UnsignedShort>(),
            Containers::stridedArrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);

        /* Convenience accessor. This uses the indicesInto() internally so it
           verifies both. */
        CORRADE_COMPARE_AS(mesh.indicesAsArray(),
            Containers::arrayView<UnsignedInt>({1, 2, 3, 4}),
            TestSuite::Compare::Container);

    /* Zero stride. Not sure how useful like this. */
    } {
        Containers::Array<char> indexData{sizeof(UnsignedShort)};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        indices[0] = 15;
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{indices.broadcasted<0>(4)}, 1};

        CORRADE_COMPARE(mesh.indexStride(), 0);

        /* Type-erased access with a cast later */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.indices())),
            Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableIndices())),
            Containers::stridedArrayView<UnsignedShort>({15, 15, 15, 15}),
            TestSuite::Compare::Container);

        /* Typed access */
        CORRADE_COMPARE_AS(mesh.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.mutableIndices<UnsignedShort>(),
            Containers::stridedArrayView<UnsignedShort>({15, 15, 15, 15}),
            TestSuite::Compare::Container);

        /* The convenience accessor should work as well, as it consumes output
           of the type-erased one */
        CORRADE_COMPARE_AS(mesh.indicesAsArray(),
            Containers::arrayView<UnsignedInt>({15, 15, 15, 15}),
            TestSuite::Compare::Container);

    /* Negative stride */
    } {
        Containers::Array<char> indexData{sizeof(UnsignedShort)*4};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        Utility::copy({1, 2, 3, 4}, indices);
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{indices.flipped<0>()}, 1};

        CORRADE_COMPARE(mesh.indexStride(), -2);

        /* Type-erased access with a cast later */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.indices())),
            Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableIndices())),
            Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);

        /* Typed access */
        CORRADE_COMPARE_AS(mesh.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.mutableIndices<UnsignedShort>(),
            Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);

        /* The convenience accessor should work as well, as it consumes output
           of the type-erased one */
        CORRADE_COMPARE_AS(mesh.indicesAsArray(),
            Containers::arrayView<UnsignedInt>({4, 3, 2, 1}),
            TestSuite::Compare::Container);
    }
}

void MeshDataTest::constructSpecialIndexStridesImplementationSpecificIndexType() {
    /* Same as constructSpecialIndexStrides() except for custom index types,
       which causes the indices() to return the full stride in second
       dimension */

    /* Every second index */
    {
        Containers::Array<char> indexData{sizeof(UnsignedShort)*8};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        Utility::copy({1, 0, 2, 0, 3, 0, 4, 0}, indices);
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{meshIndexTypeWrap(0xcaca), indices.every(2)}, 1};

        CORRADE_COMPARE(mesh.indexStride(), 4);

        /* Type-erased access with a cast later. The size is the whole stride,
           so we need to take just the prefix we want. */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.indices().prefix({mesh.indexCount(), 2}))),
            Containers::arrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableIndices().prefix({mesh.indexCount(), 2}))),
            Containers::stridedArrayView<UnsignedShort>({1, 2, 3, 4}),
            TestSuite::Compare::Container);

        /* Typed access and convenience accessors won't work here due to the
           implementation-specific format */

    /* Zero stride. The element size is zero as well, meaning there's no way to
       access anything except for directly interpreting the data pointer. Which
       is actually as desired for implementation-specific index types. */
    } {
        Containers::Array<char> indexData{sizeof(UnsignedShort)};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        indices[0] = 15;
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{meshIndexTypeWrap(0xcaca), indices.broadcasted<0>(4)}, 1};

        CORRADE_COMPARE(mesh.indexStride(), 0);

        CORRADE_COMPARE(mesh.indices().size(), (Containers::Size2D{4, 0}));
        CORRADE_COMPARE(mesh.mutableIndices().size(), (Containers::Size2D{4, 0}));
        CORRADE_COMPARE(mesh.indices().stride(), (Containers::Stride2D{0, 1}));
        CORRADE_COMPARE(mesh.mutableIndices().stride(), (Containers::Stride2D{0, 1}));
        CORRADE_COMPARE(*reinterpret_cast<const UnsignedShort*>(mesh.indices().data()), 15);
        CORRADE_COMPARE(*reinterpret_cast<UnsignedShort*>(mesh.mutableIndices().data()), 15);

        /* Typed access and convenience accessors won't work here due to the
           implementation-specific format */

    /* Negative stride */
    } {
        Containers::Array<char> indexData{sizeof(UnsignedShort)*4};
        Containers::StridedArrayView1D<UnsignedShort> indices = Containers::arrayCast<UnsignedShort>(indexData);
        Utility::copy({1, 2, 3, 4}, indices);
        MeshData mesh{MeshPrimitive::Points, Utility::move(indexData),
            MeshIndexData{meshIndexTypeWrap(0xcaca), indices.flipped<0>()}, 1};

        CORRADE_COMPARE(mesh.indexStride(), -2);

        /* Type-erased access with a cast later */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.indices())),
            Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableIndices())),
            Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
            TestSuite::Compare::Container);

        /* Typed access and convenience accessors won't work here due to the
           implementation-specific format */
    }
}

void MeshDataTest::constructSpecialAttributeStrides() {
    Containers::Array<char> vertexData{sizeof(UnsignedShort)*5};
    Containers::StridedArrayView1D<UnsignedShort> vertices = Containers::arrayCast<UnsignedShort>(vertexData);
    Utility::copy({15, 1, 2, 3, 4}, vertices);

    MeshData mesh{MeshPrimitive::Points, Utility::move(vertexData), {
        MeshAttributeData{MeshAttribute::ObjectId,
            vertices.prefix(1).broadcasted<0>(4)},
        MeshAttributeData{MeshAttribute::ObjectId,
            vertices.exceptPrefix(1).flipped<0>()},
    }};

    CORRADE_COMPARE(mesh.attributeStride(0), 0);
    CORRADE_COMPARE(mesh.attributeStride(1), -2);

    /* Type-erased access with a cast later */
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.attribute(0))),
        Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableAttribute(0))),
        Containers::stridedArrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.attribute(1))),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableAttribute(1))),
        Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);

    /* Typed access */
    CORRADE_COMPARE_AS(mesh.attribute<UnsignedShort>(0),
        Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.mutableAttribute<UnsignedShort>(0),
        Containers::stridedArrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.attribute<UnsignedShort>(1),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.mutableAttribute<UnsignedShort>(1),
        Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);

    /* All convenience accessors should work well also as they consume the
       output of the type-erased one. But just to be sure, test at least
       one. */
    CORRADE_COMPARE_AS(mesh.objectIdsAsArray(0),
        Containers::arrayView<UnsignedInt>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.objectIdsAsArray(1),
        Containers::arrayView<UnsignedInt>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
}

void MeshDataTest::constructSpecialAttributeStridesImplementationSpecificVertexFormat() {
    /* Same as constructSpecialAttributeStrides() except for custom vertex
       formats, which causes the attribute() to return the full stride in
       second dimension */

    Containers::Array<char> vertexData{sizeof(UnsignedShort)*5};
    Containers::StridedArrayView1D<UnsignedShort> vertices = Containers::arrayCast<UnsignedShort>(vertexData);
    Utility::copy({15, 1, 2, 3, 4}, vertices);

    MeshData mesh{MeshPrimitive::Points, Utility::move(vertexData), {
        MeshAttributeData{MeshAttribute::ObjectId, vertexFormatWrap(0xdead),
            vertices.prefix(1).broadcasted<0>(4)},
        MeshAttributeData{MeshAttribute::ObjectId, vertexFormatWrap(0xdead),
            vertices.exceptPrefix(1).flipped<0>()}
    }};

    CORRADE_COMPARE(mesh.attributeStride(0), 0);
    CORRADE_COMPARE(mesh.attributeStride(1), -2);

    /* Type-erased access with a cast later. For the zero-stride attribute the
       element size is zero as well, meaning there's no way to access anything
       except for directly interpreting the data pointer. Which is actually as
       desired for implementation-specific vertex formats. */
    CORRADE_COMPARE(mesh.attribute(0).size(), (Containers::Size2D{4, 0}));
    CORRADE_COMPARE(mesh.mutableAttribute(0).size(), (Containers::Size2D{4, 0}));
    CORRADE_COMPARE(mesh.attribute(0).stride(), (Containers::Stride2D{0, 1}));
    CORRADE_COMPARE(mesh.mutableAttribute(0).stride(), (Containers::Stride2D{0, 1}));
    CORRADE_COMPARE(*reinterpret_cast<const UnsignedShort*>(mesh.attribute(0).data()), 15);
    CORRADE_COMPARE(*reinterpret_cast<UnsignedShort*>(mesh.mutableAttribute(0).data()), 15);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const UnsignedShort>(mesh.attribute(1))),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, UnsignedShort>(mesh.mutableAttribute(1))),
        Containers::stridedArrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);

    /* Typed access and convenience accessors won't work here due to the
       implementation-specific format */
}

void MeshDataTest::constructNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    UnsignedShort indices[]{0, 1, 0};
    Vector2 vertices[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshData data{MeshPrimitive::Triangles,
        instanceData.indexDataFlags, indices, MeshIndexData{indices}, instanceData.vertexDataFlags, vertices, {
            MeshAttributeData{MeshAttribute::Position,
                Containers::arrayView(vertices)}
        },
        MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), instanceData.indexDataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.vertexDataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), +indices);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), +vertices);
    if(instanceData.indexDataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), +indices);
    if(instanceData.vertexDataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), +vertices);
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

    UnsignedShort indices[]{0, 1, 0};
    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertices = Containers::arrayCast<Vector2>(vertexData);
    Utility::copy({
        {0.1f, 0.2f},
        {0.4f, 0.5f},
    }, vertices);

    int importerState;
    MeshData data{MeshPrimitive::Triangles,
        instanceData.dataFlags, indices, MeshIndexData{indices},
        Utility::move(vertexData), {
            MeshAttributeData{MeshAttribute::Position, vertices}
        },
        MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), +indices);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertices.data());
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), +indices);
    CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), vertices.data());
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
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({0, 1, 0}, indices);
    Vector2 vertices[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshData data{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        instanceData.dataFlags, vertices, {
            MeshAttributeData{MeshAttribute::Position,
                Containers::arrayView(vertices)}
        },
        MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(data.indexData()), indices.data());
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), +vertices);
    CORRADE_COMPARE(static_cast<const void*>(data.mutableIndexData()), indices.data());
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableVertexData()), +vertices);
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

    Vector2 vertices[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    int importerState;
    MeshData data{MeshPrimitive::LineLoop,
        instanceData.dataFlags, vertices, {
            MeshAttributeData{MeshAttribute::Position,
                Containers::arrayView(vertices)}
        },
        MeshData::ImplicitVertexCount, &importerState};

    CORRADE_COMPARE(data.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.vertexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::LineLoop);
    CORRADE_COMPARE(data.indexData(), static_cast<const void*>(nullptr));
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(data.mutableIndexData(), static_cast<void*>(nullptr));
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

    UnsignedShort indices[]{0, 1, 0};

    int importerState;
    MeshData data{MeshPrimitive::TriangleStrip,
        instanceData.dataFlags, indices, MeshIndexData{indices},
        5, &importerState};
    CORRADE_COMPARE(data.indexDataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!data.attributeData());
    CORRADE_COMPARE(data.vertexData(), static_cast<const void*>(nullptr));
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(data.mutableVertexData(), static_cast<const void*>(nullptr));
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

#ifndef CORRADE_TARGET_32BIT
void MeshDataTest::constructIndicesOver4GB() {
    /* For some reason 2500 doesn't trigger an assertion, 3000 does */
    Containers::ArrayView<UnsignedInt> indices{reinterpret_cast<UnsignedInt*>(std::size_t{0xdeadbeef}), 3000ull*1000*1000};

    MeshData data{MeshPrimitive::Triangles,
        {}, indices, MeshIndexData{indices}, 5};
    CORRADE_COMPARE(data.indices().data(), indices.begin());
    CORRADE_COMPARE(data.indices<UnsignedInt>().size(), indices.size());
}

void MeshDataTest::constructAttributeOver4GB() {
    /* For some reason 2500 doesn't trigger an assertion, 3000 does */
    Containers::ArrayView<UnsignedInt> vertices{reinterpret_cast<UnsignedInt*>(std::size_t{0xdeadbeef}), 3000ull*1000*1000};

    MeshData data{MeshPrimitive::Triangles, {}, vertices, {
        MeshAttributeData{meshAttributeCustom(15), vertices}
    }};
    CORRADE_COMPARE(data.attribute(0).data(), vertices.begin());
    CORRADE_COMPARE(data.attribute<UnsignedInt>(0).size(), vertices.size());
}
#endif

void MeshDataTest::constructIndexDataButNotIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::Array<char> indexData{6};
    MeshAttributeData positions{MeshAttribute::Position, VertexFormat::Vector2, nullptr};

    Containers::String out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Points, Utility::move(indexData), MeshIndexData{}, nullptr, {positions}};
    CORRADE_COMPARE(out, "Trade::MeshData: indexData passed for a non-indexed mesh\n");
}

void MeshDataTest::constructAttributelessImplicitVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Points, nullptr, {}};
    CORRADE_COMPARE(out, "Trade::MeshData: vertex count can't be implicit if there are no attributes\n");
}

void MeshDataTest::constructIndicesNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const Containers::Array<char> indexData{reinterpret_cast<char*>(0xbadda9), 3*sizeof(UnsignedShort), [](char*, std::size_t){}};
    Containers::Array<char> sameIndexDataButMovable{reinterpret_cast<char*>(0xbadda9), 3*sizeof(UnsignedShort), [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> indexDataSlightlyOut{reinterpret_cast<UnsignedShort*>(0xbaddaa), 3};
    Containers::ArrayView<UnsignedShort> indexDataOut{reinterpret_cast<UnsignedShort*>(0xdead), 3};
    Containers::StridedArrayView1D<UnsignedShort> indexDataStridedOut{{reinterpret_cast<UnsignedShort*>(0xbadda9), 6}, 3, 4};

    /* "Obviously good" case */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{Containers::arrayCast<UnsignedShort>(indexData)}, 1};
    /* An implementation-specific index type has a size assumed to be 0, so
       even though the last element starts at 0xbaddaf it's fine */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<UnsignedShort>{reinterpret_cast<UnsignedShort*>(0xbadda9 + sizeof(UnsignedShort)), 3}}, 1};
    /* This has both stride and size zero, so it's treated as both starting and
       ending at 0xbaddaf */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<UnsignedShort>{{reinterpret_cast<UnsignedShort*>(0xbaddaf), 1}, 1}.broadcasted<0>(3)}, 1};

    Containers::String out;
    Error redirectError{&out};
    /* Basic "obviously wrong" case with owned index data */
    MeshData{MeshPrimitive::Triangles, Utility::move(sameIndexDataButMovable), MeshIndexData{indexDataOut}, 1};
    /* A "slightly off" view that exceeds the original by one byte */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{indexDataSlightlyOut}, 1};
    /* A strided index array which would pass if stride wasn't taken into
       account */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{indexDataStridedOut}, 1};
    /* Empty view which however begins outside */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{indexDataSlightlyOut.slice(3, 3)}, 1};
    /* If we have no data at all, it doesn't try to dereference them but still
       checks properly */
    MeshData{MeshPrimitive::Triangles, nullptr, MeshIndexData{indexDataOut}, 1};
    /* An implementation-specific index type has a size assumed to be 0, but
       even then this exceeds the data by one byte */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<UnsignedShort>{reinterpret_cast<UnsignedShort*>(0xbadda9 + sizeof(UnsignedShort) + 1), 3}}, 1};
    /* And the final boss, negative strides. Only caught if the element size
       gets properly added to the larger offset, not just the "end". */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{stridedArrayView(indexDataSlightlyOut).flipped<0>()}, 1};
    /* In this case the implementation-specific type is treated as having a
       zero size, and the stride is zero as well, but since it starts one byte
       after, it's wrong */
    MeshData{MeshPrimitive::Triangles, {}, indexData, MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<UnsignedShort>{{reinterpret_cast<UnsignedShort*>(0xbaddaf + 1), 1}, 1}.broadcasted<0>(3)}, 1};
    CORRADE_COMPARE(out,
        "Trade::MeshData: indices [0xdead:0xdeb3] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"
        "Trade::MeshData: indices [0xbaddaa:0xbaddb0] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"
        "Trade::MeshData: indices [0xbadda9:0xbaddb3] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"
        /* This scenario is invalid, just have it here for the record */
        "Trade::MeshData: indexData passed for a non-indexed mesh\n"
        "Trade::MeshData: indices [0xdead:0xdeb3] are not contained in passed indexData array [0x0:0x0]\n"

        "Trade::MeshData: indices [0xbaddac:0xbaddb0] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"

        "Trade::MeshData: indices [0xbaddaa:0xbaddb0] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n"
        "Trade::MeshData: indices [0xbaddb0:0xbaddb0] are not contained in passed indexData array [0xbadda9:0xbaddaf]\n");
}

void MeshDataTest::constructAttributeNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* See implementationSpecificVertexFormatNotContained() below for
       implementation-specific formats */

    const Containers::Array<char> vertexData{reinterpret_cast<char*>(0xbadda9), 3*sizeof(Vector2), [](char*, std::size_t){}};
    Containers::Array<char> sameVertexDataButMovable{reinterpret_cast<char*>(0xbadda9), 3*sizeof(Vector2), [](char*, std::size_t){}};
    Containers::ArrayView<Vector2> vertexDataIn{reinterpret_cast<Vector2*>(0xbadda9), 3};
    Containers::ArrayView<Vector2> vertexDataSlightlyOut{reinterpret_cast<Vector2*>(0xbaddaa), 3};
    Containers::ArrayView<Vector2> vertexDataOut{reinterpret_cast<Vector2*>(0xdead), 3};
    MeshAttributeData{MeshAttribute::Position, Containers::arrayCast<Vector2>(vertexData)};

    /* "Obviously good" case */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexDataIn}
    }};
    /* Here the original positions array is shrunk from 3 items to 2 and the
       vertex data too, which should work without asserting -- comparing just
       the original view would not pass, which is wrong */
    MeshData{MeshPrimitive::Triangles, {}, vertexData.prefix(16), {
        MeshAttributeData{MeshAttribute::Position, vertexDataIn}
    }, 2};
    /* An implementation-specific vertex format has a size assumed to be 0, so
       even though the last element starts at 0xbaddc1 it's fine */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexFormatWrap(0xcaca), Containers::ArrayView<Vector2>{reinterpret_cast<Vector2*>(0xbadda9 + sizeof(Vector2)), 3}}
    }};
    /* This has both stride and size zero, so it's treated as both starting and
       ending at 0xbaddc1 */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexFormatWrap(0xcaca), Containers::StridedArrayView1D<Vector2>{{reinterpret_cast<Vector2*>(0xbaddc1), 1}, 1}.broadcasted<0>(3)}
    }};

    Containers::String out;
    Error redirectError{&out};
    /* Basic "obviously wrong" case with owned vertex data */
    MeshData{MeshPrimitive::Triangles, Utility::move(sameVertexDataButMovable), {
        /* This is here to test that not just the first attribute gets checked
           and that the message shows proper ID */
        MeshAttributeData{MeshAttribute::Position, vertexDataIn},
        MeshAttributeData{MeshAttribute::Position, Containers::arrayView(vertexDataOut)}
    }};
    /* A "slightly off" view that exceeds the original by one byte */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexDataSlightlyOut}
    }};
    /* Here the original positions array is extended from 3 items to 4, which
       makes it not fit anymore, and thus an assert should hit -- comparing
       just the original view would pass, which is wrong */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexDataIn}
    }, 4};
    /* Spanning 20 bytes originally, 25 when vertex count is changed to 5. If
       array size wouldn't be taken into account, it would span only 16 / 21,
       which  fits into the vertex data size and thus wouldn't fail */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{meshAttributeCustom(37), Containers::StridedArrayView2D<const UnsignedByte>{Containers::arrayCast<const UnsignedByte>(vertexData), {4, 5}, {5, 1}}}
    }, 5};
    /* And if we have no data at all, it doesn't try to dereference them but
       still checks properly */
    MeshData{MeshPrimitive::Triangles, nullptr, {
        MeshAttributeData{MeshAttribute::Position, vertexDataIn}
    }};
    /* Offset-only attributes with a different message */
    MeshData{MeshPrimitive::Triangles, Containers::Array<char>{24}, {
        MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector2, 1, 3, 8}
    }};
    /* This again spans 21 bytes if array size isn't taken into account, and 25
       if it is */
    MeshData{MeshPrimitive::Triangles, Containers::Array<char>{24}, {
        MeshAttributeData{meshAttributeCustom(37), VertexFormat::UnsignedByte, 0, 5, 5, 5}
    }};
    /* An implementation-specific vertex format has a size assumed to be 0, but
       even then this exceeds the data by one byte */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexFormatWrap(0xcaca), Containers::ArrayView<Vector2>{reinterpret_cast<Vector2*>(0xbadda9 + sizeof(Vector2) + 1), 3}}
    }};
    /* And the final boss, negative strides. Both only caught if the element
       size gets properly added to the larger offset, not just the "end". */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, stridedArrayView(vertexDataSlightlyOut).flipped<0>()}
    }};
    MeshData{MeshPrimitive::Triangles, Containers::Array<char>{24}, {
        MeshAttributeData{meshAttributeCustom(37), VertexFormat::UnsignedByte, 24, 3, -8}
    }};
    /* In this case the implementation-specific format is treated as having a
       zero size, and the stride is zero as well, but since it starts one byte
       after, it's wrong */
    MeshData{MeshPrimitive::Triangles, {}, vertexData, {
        MeshAttributeData{MeshAttribute::Position, vertexFormatWrap(0xcaca), Containers::StridedArrayView1D<Vector2>{{reinterpret_cast<Vector2*>(0xbaddc1 + 1), 1}, 1}.broadcasted<0>(3)}
    }};
    CORRADE_COMPARE(out,
        "Trade::MeshData: attribute 1 [0xdead:0xdec5] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 0 [0xbaddaa:0xbaddc2] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 0 [0xbadda9:0xbaddc9] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 0 [0xbadda9:0xbaddc2] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: attribute 0 [0xbadda9:0xbaddc1] is not contained in passed vertexData array [0x0:0x0]\n"

        "Trade::MeshData: offset-only attribute 0 spans 25 bytes but passed vertexData array has only 24\n"
        "Trade::MeshData: offset-only attribute 0 spans 25 bytes but passed vertexData array has only 24\n"

        "Trade::MeshData: attribute 0 [0xbaddb2:0xbaddc2] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"

        "Trade::MeshData: attribute 0 [0xbaddaa:0xbaddc2] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
        "Trade::MeshData: offset-only attribute 0 spans 25 bytes but passed vertexData array has only 24\n"
        "Trade::MeshData: attribute 0 [0xbaddc2:0xbaddc2] is not contained in passed vertexData array [0xbadda9:0xbaddc1]\n"
    );
}

void MeshDataTest::constructInconsitentVertexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::Array<char> vertexData{136};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData).prefix(3)};
    MeshAttributeData positions2{MeshAttribute::Position,
        Containers::arrayCast<Vector2>(vertexData).prefix(2)};

    Containers::String out;
    Error redirectError{&out};
    /* The explicit vertex count should be ignored for the assertion message,
       we only check that all passed attribute arrays have the same vertex
       count. However, the actual "containment" of the attribute views is
       checked with the explicit vertex count -- see the
       constructAttributeNotContained() test above. */
    MeshData{MeshPrimitive::Triangles, Utility::move(vertexData), {positions, positions2}, 17};
    CORRADE_COMPARE(out,
        "Trade::MeshData: attribute 1 has 2 vertices but 3 expected\n");
}

void MeshDataTest::constructDifferentJointIdWeightCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Vertex {
        /* Weights required to be here by the constructor */
        Float weights[2];
        UnsignedByte jointIds[2];
        UnsignedShort secondaryJointIds[4];
    } vertices[3]{};
    auto view = Containers::stridedArrayView(vertices);

    Containers::String out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Points, {}, vertices, {
        /* Weights required to be here by the constructor */
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float,
            view.slice(&Vertex::weights), 2},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedByte,
            view.slice(&Vertex::jointIds), 2},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedShort,
            view.slice(&Vertex::secondaryJointIds), 4}
    }};
    CORRADE_COMPARE(out, "Trade::MeshData: expected 2 weight attributes to match joint IDs but got 1\n");
}

void MeshDataTest::constructInconsistentJointIdWeightArraySizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Vertex {
        /* Weights required to be here by the constructor */
        Float weights[2];
        UnsignedByte jointIds[2];
        UnsignedShort secondaryWeights[3]; /* Half together with {} makes GCC 4.8 crash */
        UnsignedShort secondaryJointIds[4];
    } vertices[3]{};
    auto view = Containers::stridedArrayView(vertices);

    Containers::String out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Points, {}, vertices, {
        /* Weights required to be here by the constructor */
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float,
            view.slice(&Vertex::weights), 2},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedByte,
            view.slice(&Vertex::jointIds), 2},
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Half,
            view.slice(&Vertex::secondaryWeights), 3},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedShort,
            view.slice(&Vertex::secondaryJointIds), 4}
    }};
    CORRADE_COMPARE(out, "Trade::MeshData: expected 4 array items for weight attribute 1 to match joint IDs but got 3\n");
}

void MeshDataTest::constructNotOwnedIndexFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indexData[]{0, 1, 0};
    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::arrayView(vertexData)};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, {}, vertexData, {positions}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructNotOwnedVertexFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indexData[]{0, 1, 0};
    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::arrayView(vertexData)};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, {}, indexData, indices, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructIndicesNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort indexData[]{0, 1, 0};
    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertexView = Containers::arrayCast<Vector2>(vertexData);
    vertexView[0] = {0.1f, 0.2f};
    vertexView[1] = {0.4f, 0.5f};

    MeshIndexData indices{indexData};
    MeshAttributeData positions{MeshAttribute::Position, vertexView};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, Utility::move(vertexData), {positions}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructVerticesNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::Array<char> indexData{3*sizeof(UnsignedShort)};
    auto indexView = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({0, 1, 0}, indexView);
    Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};

    MeshIndexData indices{indexView};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::arrayView(vertexData)};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, Utility::move(indexData), indices, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructIndexlessNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const Vector2 vertexData[]{{0.1f, 0.2f}, {0.4f, 0.5f}};
    MeshAttributeData positions{MeshAttribute::Position,
        Containers::arrayView(vertexData)};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, vertexData, {positions}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned vertex data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructAttributelessNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indexData[]{0, 1, 0};
    MeshIndexData indices{indexData};

    Containers::String out;
    Error redirectError{&out};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Owned, indexData, indices, 2};
    CORRADE_COMPARE(out,
        "Trade::MeshData: can't construct with non-owned index data but Trade::DataFlag::Owned\n");
}

void MeshDataTest::constructInvalidAttributeData() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshAttributeData a;
    MeshAttributeData b{3};

    Containers::String out;
    Error redirectError{&out};
    MeshData{MeshPrimitive::Triangles, nullptr, {a}};
    MeshData{MeshPrimitive::Triangles, nullptr, {b}};
    CORRADE_COMPARE(out,
        "Trade::MeshData: attribute 0 doesn't specify anything\n"
        "Trade::MeshData: attribute 0 doesn't specify anything\n");
}

void MeshDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<MeshData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<MeshData>{});
}

void MeshDataTest::constructMove() {
    Containers::Array<char> indexData{3*sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    Utility::copy({0, 1, 0}, indices);

    Containers::Array<char> vertexData{2*sizeof(Vector2)};
    auto vertices = Containers::arrayCast<Vector2>(vertexData);
    Utility::copy({
        {0.1f, 0.2f},
        {0.4f, 0.5f}
    }, vertices);

    int importerState;
    MeshData a{MeshPrimitive::Triangles,
        Utility::move(indexData), MeshIndexData{indices},
        Utility::move(vertexData), {
            MeshAttributeData{MeshAttribute::Position, vertices}
        },
        MeshData::ImplicitVertexCount, &importerState};

    MeshData b{Utility::move(a)};

    CORRADE_COMPARE(b.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(b.indexData()), indices.data());
    CORRADE_COMPARE(static_cast<const void*>(b.vertexData()), vertices.data());
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
    c = Utility::move(b);

    CORRADE_COMPARE(c.indexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.vertexDataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(static_cast<const void*>(c.indexData()), indices.data());
    CORRADE_COMPARE(static_cast<const void*>(c.vertexData()), vertices.data());
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
/* Scalars are in Math::TypeTraits already */
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

    T indices[]{75, 131, 240};

    MeshData data{MeshPrimitive::Points, {}, indices, MeshIndexData{indices}, 241};
    CORRADE_COMPARE_AS(data.indicesAsArray(),
        Containers::arrayView<UnsignedInt>({75, 131, 240}),
        TestSuite::Compare::Container);
}

void MeshDataTest::indicesIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[3]{};
    MeshData data{MeshPrimitive::Points, {}, indices, MeshIndexData{indices}, 1};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    data.indicesInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::indicesInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::positions2DAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute */
    typedef typename T::Type TT;
    struct Vertex {
        Vector2 otherPosition;
        UnsignedShort objectId;
        T position;
    } vertices[]{
        {{}, 0, T::pad(Math::Vector2<TT>{TT(2.0f), TT(1.0f)})},
        {{}, 0, T::pad(Math::Vector2<TT>{TT(0.0f), TT(-1.0f)})},
        {{}, 0, T::pad(Math::Vector2<TT>{TT(-2.0f), TT(3.0f)})}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Position,
            view.slice(&Vertex::otherPosition)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Position,
            view.slice(&Vertex::position), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.positions2DAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, -1.0f}, {-2.0f, 3.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector2<TT>{2, 1}),
        T::pad(Math::Vector2<TT>{0, 15}),
        T::pad(Math::Vector2<TT>{22, 3})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions2DAsArray(), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, 15.0f}, {22.0f, 3.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector2<TT>{2, 1}),
        T::pad(Math::Vector2<TT>{0, -15}),
        T::pad(Math::Vector2<TT>{-22, 3})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions2DAsArray(), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, -15.0f}, {-22.0f, 3.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector2<TT>{Math::pack<TT>(1.0f), 0}),
        T::pad(Math::Vector2<TT>{0, Math::pack<TT>(1.0f)})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions2DAsArray(), Containers::arrayView<Vector2>({
        {1.0f, 0.0f}, {0.0f, 1.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions2DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector2<TT>{Math::pack<TT>(1.0f), 0}),
        T::pad(Math::Vector2<TT>{0, Math::pack<TT>(-1.0f)})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions2DAsArray(), Containers::arrayView<Vector2>({
        {1.0f, 0.0f}, {0.0f, -1.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::positions2DIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 positions[3]{};
    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector2 destination[2];
    data.positions2DInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::positions2DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::positions3DAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Vector3 otherPosition;
        UnsignedShort objectId;
        T position;
    } vertices[]{
        {{}, 0, T::pad(Math::Vector3<TT>{TT(2.0f), TT(1.0f), TT(0.75f)})},
        {{}, 0, T::pad(Math::Vector3<TT>{TT(0.0f), TT(-1.0f), TT(1.25f)})},
        {{}, 0, T::pad(Math::Vector3<TT>{TT(-2.0f), TT(3.0f), TT(2.5f)})}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Position,
            view.slice(&Vertex::otherPosition)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Position,
            view.slice(&Vertex::position), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.positions3DAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, 0.75f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -1.0f, 1.25f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{-2.0f, 3.0f, 2.5f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector3<TT>{2, 1, 135}),
        T::pad(Math::Vector3<TT>{0, 15, 2}),
        T::pad(Math::Vector3<TT>{22, 3, 192})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, 135.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, 15.0f, 2.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{22.0f, 3.0f, 192.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector3<TT>{2, 1, -117}),
        T::pad(Math::Vector3<TT>{0, -15, 2}),
        T::pad(Math::Vector3<TT>{-22, 3, 86})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{2.0f, 1.0f, -117.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -15.0f, 2.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{-22.0f, 3.0f, 86.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector3<TT>{Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)}),
        T::pad(Math::Vector3<TT>{0, Math::pack<TT>(1.0f), 0})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{1.0f, 0.0f, 1.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, 1.0f, 0.0f}))
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::positions3DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T positions[]{
        T::pad(Math::Vector3<TT>{Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)}),
        T::pad(Math::Vector3<TT>{0, Math::pack<TT>(-1.0f), 0})
    };

    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(positions)}
    }};
    CORRADE_COMPARE_AS(data.positions3DAsArray(), Containers::arrayView<Vector3>({
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{1.0f, 0.0f, 1.0f})),
        Vector3::pad(Math::Vector<T::Size, Float>::pad(Vector3{0.0f, -1.0f, 0.0f}))
    }), TestSuite::Compare::Container);
}

void MeshDataTest::positions3DIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 positions[3]{};
    MeshData data{MeshPrimitive::Points, {}, positions, {
        MeshAttributeData{MeshAttribute::Position,
            Containers::arrayView(positions)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.positions3DInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::positions3DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::tangentsAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Vector3 otherTangent;
        UnsignedShort objectId;
        T tangent;
    } vertices[]{
        {{}, 0, T::pad(Math::Vector3<TT>{TT(2.0f), TT(1.0f), TT(0.75f)})},
        {{}, 0, T::pad(Math::Vector3<TT>{TT(0.0f), TT(-1.0f), TT(1.25f)})},
        {{}, 0, T::pad(Math::Vector3<TT>{TT(-2.0f), TT(3.0f), TT(2.5f)})}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Tangent,
            view.slice(&Vertex::otherTangent)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Tangent,
            view.slice(&Vertex::tangent), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.tangentsAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::tangentsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T tangents[]{
        T::pad(Math::Vector3<TT>{Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)}),
        T::pad(Math::Vector3<TT>{0, Math::pack<TT>(-1.0f), 0})
    };

    MeshData data{MeshPrimitive::Points, {}, tangents, {
        MeshAttributeData{MeshAttribute::Tangent,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(tangents)}
    }};
    CORRADE_COMPARE_AS(data.tangentsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::tangentsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 tangents[3]{};
    MeshData data{MeshPrimitive::Points, {}, tangents, {
        MeshAttributeData{MeshAttribute::Tangent,
            Containers::arrayView(tangents)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.tangentsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::tangentsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::bitangentSignsAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    struct Vertex {
        Vector3 otherTangent;
        UnsignedShort objectId;
        Math::Vector4<T> tangent;
    } vertices[]{
        {{}, 0, {T(2.0f), T(1.0f), T(0.75f), T(-1.0f)}},
        {{}, 0, {T(0.0f), T(-1.0f), T(1.25f), T(1.0f)}},
        {{}, 0, {T(-2.0f), T(3.0f), T(2.5f), T(-1.0f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Tangent,
            view.slice(&Vertex::otherTangent)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Tangent,
            view.slice(&Vertex::tangent), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.bitangentSignsAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Float>({
        -1.0f, 1.0f, -1.0f
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::bitangentSignsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Math::Vector4<T> tangents[]{
        {Math::pack<T>(1.0f), 0, Math::pack<T>(1.0f), Math::pack<T>(-1.0f)},
        {0, Math::pack<T>(-1.0f), 0, Math::pack<T>(1.0f)}
    };

    MeshData data{MeshPrimitive::Points, {}, tangents, {
        MeshAttributeData{MeshAttribute::Tangent,
            vertexFormat(Implementation::vertexFormatFor<T>(), 4, true),
            Containers::arrayView(tangents)}
    }};
    CORRADE_COMPARE_AS(data.bitangentSignsAsArray(), Containers::arrayView<Float>({
        -1.0f, 1.0f
    }), TestSuite::Compare::Container);
}

void MeshDataTest::bitangentSignsAsArrayNotFourComponent() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3s tangents[3]{};
    MeshData data{MeshPrimitive::Points, {}, tangents, {
        MeshAttributeData{MeshAttribute::Tangent,
            VertexFormat::Vector3sNormalized, Containers::arrayView(tangents)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Float destination[3];
    data.bitangentSignsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::bitangentSignsInto(): expected four-component tangents, but got VertexFormat::Vector3sNormalized\n");
}

void MeshDataTest::bitangentSignsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector4 tangents[3]{};
    MeshData data{MeshPrimitive::Points, {}, tangents, {
        MeshAttributeData{MeshAttribute::Tangent,
            Containers::arrayView(tangents)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Float destination[2];
    data.bitangentSignsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::bitangentSignsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::bitangentsAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Vector3 otherBitangent;
        UnsignedShort objectId;
        T bitangent;
    } vertices[]{
        {{}, 0, {TT(2.0f), TT(1.0f), TT(0.75f)}},
        {{}, 0, {TT(0.0f), TT(-1.0f), TT(1.25f)}},
        {{}, 0, {TT(-2.0f), TT(3.0f), TT(2.5f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Bitangent,
            view.slice(&Vertex::otherBitangent)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Bitangent,
            view.slice(&Vertex::bitangent), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.bitangentsAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::bitangentsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T bitangents[]{
        {Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)},
        {0, Math::pack<TT>(-1.0f), 0}
    };

    MeshData data{MeshPrimitive::Points, {}, bitangents, {
        MeshAttributeData{MeshAttribute::Bitangent,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(bitangents)}
    }};
    CORRADE_COMPARE_AS(data.bitangentsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::bitangentsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 bitangents[3]{};
    MeshData data{MeshPrimitive::Points, {}, bitangents, {
        MeshAttributeData{MeshAttribute::Bitangent,
            Containers::arrayView(bitangents)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.bitangentsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::bitangentsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::normalsAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Vector3 otherNormal;
        UnsignedShort objectId;
        T normal;
    } vertices[]{
        {{}, 0, {TT(2.0f), TT(1.0f), TT(0.75f)}},
        {{}, 0, {TT(0.0f), TT(-1.0f), TT(1.25f)}},
        {{}, 0, {TT(-2.0f), TT(3.0f), TT(2.5f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Normal,
            view.slice(&Vertex::otherNormal)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Normal,
            view.slice(&Vertex::normal), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.normalsAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector3>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::normalsAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T normals[]{
        {Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)},
        {0, Math::pack<TT>(-1.0f), 0}
    };

    MeshData data{MeshPrimitive::Points, {}, normals, {
        MeshAttributeData{MeshAttribute::Normal,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(normals)}
    }};
    CORRADE_COMPARE_AS(data.normalsAsArray(), Containers::arrayView<Vector3>({
        {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::normalsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 normals[3]{};
    MeshData data{MeshPrimitive::Points, {}, normals, {
        MeshAttributeData{MeshAttribute::Normal, Containers::arrayView(normals)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector3 destination[2];
    data.normalsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::normalsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::textureCoordinates2DAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Vector2 otherTextureCoordinate;
        UnsignedShort objectId;
        T textureCoordinate;
    } vertices[]{
        {{}, 0, {TT(2.0f), TT(1.0f)}},
        {{}, 0, {TT(0.0f), TT(-1.0f)}},
        {{}, 0, {TT(-2.0f), TT(3.0f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            view.slice(&Vertex::otherTextureCoordinate)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            view.slice(&Vertex::textureCoordinate), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, -1.0f}, {-2.0f, 3.0f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedUnsigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    T textureCoordinates[]{
        {2, 1},
        {0, 15},
        {22, 3}
    };

    MeshData data{MeshPrimitive::Points, {}, textureCoordinates, {
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            Containers::arrayView(textureCoordinates)}
    }};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, 15.0f}, {22.0f, 3.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedSigned() {
    setTestCaseTemplateName(NameTraits<T>::name());

    T textureCoordinates[]{
        {2, 1},
        {0, -15},
        {-22, 3}
    };

    MeshData data{MeshPrimitive::Points, {}, textureCoordinates, {
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            Containers::arrayView(textureCoordinates)}
    }};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(), Containers::arrayView<Vector2>({
        {2.0f, 1.0f}, {0.0f, -15.0f}, {-22.0f, 3.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T textureCoordinates[]{
        {Math::pack<TT>(1.0f), 0},
        {0, Math::pack<TT>(1.0f)}
    };

    MeshData data{MeshPrimitive::Points, {}, textureCoordinates, {
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(textureCoordinates)}
    }};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(), Containers::arrayView<Vector2>({
        {1.0f, 0.0f}, {0.0f, 1.0f}
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::textureCoordinates2DAsArrayPackedSignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T textureCoordinates[]{
        {Math::pack<TT>(1.0f), 0},
        {0, Math::pack<TT>(-1.0f)}
    };

    MeshData data{MeshPrimitive::Points, {}, textureCoordinates, {
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            vertexFormat(Implementation::vertexFormatFor<T>(), T::Size, true),
            Containers::arrayView(textureCoordinates)}
    }};
    CORRADE_COMPARE_AS(data.textureCoordinates2DAsArray(), Containers::arrayView<Vector2>({
        {1.0f, 0.0f}, {0.0f, -1.0f}
    }), TestSuite::Compare::Container);
}

void MeshDataTest::textureCoordinates2DIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 textureCoordinates[3];
    MeshData data{MeshPrimitive::Points, {}, textureCoordinates, {
        MeshAttributeData{MeshAttribute::TextureCoordinates, Containers::arrayView(textureCoordinates)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Vector2 destination[2];
    data.textureCoordinates2DInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::textureCoordinates2DInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::colorsAsArray() {
    auto&& instanceData = AsArrayData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);
    setTestCaseTemplateName(NameTraits<T>::name());

    /* Testing also that it picks the correct attribute. Can't use e.g.
       0xff3366_rgbf because that's not representable in half-floats. */
    typedef typename T::Type TT;
    struct Vertex {
        Color4 otherColor;
        UnsignedShort objectId;
        T color;
    } vertices[]{
        {{}, 0, {TT(2.0f), TT(1.0f), TT(0.75f)}},
        {{}, 0, {TT(0.0f), TT(-1.0f), TT(1.25f)}},
        {{}, 0, {TT(-2.0f), TT(3.0f), TT(2.5f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::Color,
            view.slice(&Vertex::otherColor)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Color,
            view.slice(&Vertex::color), instanceData.morphTargetId}
    }};
    CORRADE_COMPARE_AS(data.colorsAsArray(instanceData.id, instanceData.morphTargetId), Containers::arrayView<Color4>({
        {2.0f, 1.0f, 0.75f}, {0.0f, -1.0f, 1.25f}, {-2.0f, 3.0f, 2.5f},
    }), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::colorsAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type TT;
    T colors[]{
        T::pad(Math::Color4<TT>{Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f), 0}),
        T::pad(Math::Color4<TT>{0, Math::pack<TT>(1.0f), 0, Math::pack<TT>(1.0f)})
    };

    MeshData data{MeshPrimitive::Points, {}, colors, {
        MeshAttributeData{MeshAttribute::Color, Containers::arrayView(colors)}
    }};
    CORRADE_COMPARE_AS(data.colorsAsArray(), Containers::arrayView<Color4>({
        Color4::pad(Math::Vector<T::Size, Float>::pad(Vector4{1.0f, 0.0f, 1.0f, 0.0f}), 1.0f),
        Color4::pad(Math::Vector<T::Size, Float>::pad(Vector4{0.0f, 1.0f, 0.0f, 1.0f}), 1.0f)
    }), TestSuite::Compare::Container);
}

void MeshDataTest::colorsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Color4 colors[3]{};
    MeshData data{MeshPrimitive::Points, {}, colors, {
        MeshAttributeData{MeshAttribute::Color, Containers::arrayView(colors)}
    }};

    Containers::String out;
    Error redirectError{&out};
    Color4 destination[2];
    data.colorsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::colorsInto(): expected a view with 3 elements but got 2\n");
}

template<class T> void MeshDataTest::jointIdsAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    struct Vertex {
        /* Weights required to be here by the constructor */
        Float otherWeights[3];
        Float weights[5];
        UnsignedInt otherJointIds[3];
        UnsignedShort objectId;
        T jointIds[5];
    } vertices[]{
        {{}, {}, {},
         0,
         {T(0), T(3), T(20), T(1), T(7)}},
        {{}, {}, {},
         0,
         {T(9), T(1), T(15), T(2), T(3)}},
        {{}, {}, {},
         0,
         {T(25), T(7), T(0), T(2), T(1)}},
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        /* Weights required to be here by the constructor */
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float,
            view.slice(&Vertex::otherWeights), 3},
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float,
            view.slice(&Vertex::weights), 5},
        MeshAttributeData{MeshAttribute::JointIds,
            Implementation::vertexFormatFor<T>(),
            view.slice(&Vertex::otherJointIds), 3},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::JointIds,
            Implementation::vertexFormatFor<T>(),
            view.slice(&Vertex::jointIds), 5},
    }};
    CORRADE_COMPARE_AS(data.jointIdsAsArray(1), (Containers::arrayView<UnsignedInt>({
        0, 3, 20, 1, 7,
        9, 1, 15, 2, 3,
        25, 7, 0, 2, 1
    })), TestSuite::Compare::Container);
}

void MeshDataTest::jointIdsIntoArrayInvalidSizeStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Vertex {
        /* Weights required to be here by the constructor */
        Float weights[2];
        UnsignedByte jointIds[2];
    } vertices[3]{};
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        /* Weights required to be here by the constructor */
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float,
            view.slice(&Vertex::weights), 2},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedByte,
            view.slice(&Vertex::jointIds), 2}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt jointIds1[3*3];
    UnsignedInt jointIds2[2*2];
    UnsignedInt jointIds3[3*4];
    data.jointIdsInto(Containers::StridedArrayView2D<UnsignedInt>{jointIds1, {3, 3}});
    data.jointIdsInto(Containers::StridedArrayView2D<UnsignedInt>{jointIds2, {2, 2}});
    data.jointIdsInto(Containers::StridedArrayView2D<UnsignedInt>{jointIds3, {3, 4}}.every({1, 2}));
    CORRADE_COMPARE(out,
        "Trade::MeshData::jointIdsInto(): expected a view with {3, 2} elements but got {3, 3}\n"
        "Trade::MeshData::jointIdsInto(): expected a view with {3, 2} elements but got {2, 2}\n"
        "Trade::MeshData::jointIdsInto(): second view dimension is not contiguous\n");
}

template<class T> void MeshDataTest::weightsAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Testing also that it picks the correct attribute. Needs to be
       sufficiently representable to have the test work also for half
       floats. */
    struct Vertex {
        /* Joint IDs required to be here by the constructor */
        UnsignedInt otherJointIds[3];
        UnsignedInt jointIds[5];
        Float otherWeights[3];
        UnsignedShort objectId;
        T weights[5];
    } vertices[]{
        {{}, {}, {},
         0,
         {T(2.0f), T(1.0f), T(0.75f), T(3.0f), T(1.75f)}},
        {{}, {}, {},
         0,
         {T(0.0f), T(-1.0f), T(1.25f), T(1.0f), T(2.25f)}},
        {{}, {}, {},
         0,
         {T(-2.0f), T(3.0f), T(2.5f), T(2.5f), T(0.25f)}},
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        /* Joint IDs required to be here by the constructor */
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedInt,
            view.slice(&Vertex::otherJointIds), 3},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedInt,
            view.slice(&Vertex::jointIds), 5},
        MeshAttributeData{MeshAttribute::Weights,
            Implementation::vertexFormatFor<T>(),
            view.slice(&Vertex::otherWeights), 3},
        MeshAttributeData{MeshAttribute::ObjectId, view.slice(&Vertex::objectId)},
        MeshAttributeData{MeshAttribute::Weights,
            Implementation::vertexFormatFor<T>(),
            view.slice(&Vertex::weights), 5}
    }};
    CORRADE_COMPARE_AS(data.weightsAsArray(1), (Containers::arrayView<Float>({
        2.0f, 1.0f, 0.75f, 3.0f, 1.75f,
        0.0f, -1.0f, 1.25f, 1.0f, 2.25f,
        -2.0f, 3.0f, 2.5f, 2.5f, 0.25f,
    })), TestSuite::Compare::Container);
}

template<class T> void MeshDataTest::weightsAsArrayPackedUnsignedNormalized() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    struct Vertex {
        /* Joint IDs required to be here by the constructor */
        UnsignedByte jointIds[2];
        T weights[2];
    } vertices[2]{
        {{}, {Math::pack<T>(1.0f), Math::pack<T>(0.8f)}},
        {{}, {0, Math::pack<T>(0.4f)}}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        /* Joint IDs required to be here by the constructor */
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedByte,
            view.slice(&Vertex::jointIds), 2},
        MeshAttributeData{MeshAttribute::Weights,
            vertexFormat(Implementation::vertexFormatFor<T>(), 1, true),
            view.slice(&Vertex::weights), 2}
    }};

    CORRADE_COMPARE_AS(data.weightsAsArray(), (Containers::arrayView<Float>({
        1.0f, 0.8f,
        0.0f, 0.4f
    })), TestSuite::Compare::Container);
}

void MeshDataTest::weightsIntoArrayInvalidSizeStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Vertex {
        /* Joint IDs required to be here by the constructor */
        UnsignedInt jointIds[2];
        UnsignedShort weights[2]; /* Half together with {} makes GCC 4.8 crash */
    } vertices[3]{};
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        /* Joint IDs required to be here by the constructor */
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedInt,
            view.slice(&Vertex::jointIds), 2},
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Half,
            view.slice(&Vertex::weights), 2}
    }};

    Containers::String out;
    Error redirectError{&out};
    Float weights1[3*3];
    Float weights2[2*2];
    Float weights3[3*4];
    data.weightsInto(Containers::StridedArrayView2D<Float>{weights1, {3, 3}});
    data.weightsInto(Containers::StridedArrayView2D<Float>{weights2, {2, 2}});
    data.weightsInto(Containers::StridedArrayView2D<Float>{weights3, {3, 4}}.every({1, 2}));
    CORRADE_COMPARE(out,
        "Trade::MeshData::weightsInto(): expected a view with {3, 2} elements but got {3, 3}\n"
        "Trade::MeshData::weightsInto(): expected a view with {3, 2} elements but got {2, 2}\n"
        "Trade::MeshData::weightsInto(): second view dimension is not contiguous\n");
}

template<class T> void MeshDataTest::objectIdsAsArray() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Testing also that it picks the correct attribute */
    struct Vertex {
        UnsignedByte otherObjectId;
        Vector2 position;
        T objectId;
    } vertices[]{
        {0, {}, 157},
        {0, {}, 24},
        {0, {}, 1}
    };
    auto view = Containers::stridedArrayView(vertices);

    MeshData data{MeshPrimitive::Points, {}, vertices, {
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::otherObjectId)},
        MeshAttributeData{MeshAttribute::Position,
            view.slice(&Vertex::position)},
        MeshAttributeData{MeshAttribute::ObjectId,
            view.slice(&Vertex::objectId)}
    }};
    CORRADE_COMPARE_AS(data.objectIdsAsArray(1), Containers::arrayView<UnsignedInt>({
        157, 24, 1
    }), TestSuite::Compare::Container);
}

void MeshDataTest::objectIdsIntoArrayInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt objectIds[3]{};
    MeshData data{MeshPrimitive::Points, {}, objectIds, {
        MeshAttributeData{MeshAttribute::ObjectId,
            Containers::arrayView(objectIds)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    data.objectIdsInto(destination);
    CORRADE_COMPARE(out,
        "Trade::MeshData::objectIdsInto(): expected a view with 3 elements but got 2\n");
}

void MeshDataTest::implementationSpecificIndexTypeWrongAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    VertexWithImplementationSpecificData indexData[3];

    Containers::StridedArrayView1D<long double> indices{indexData,
        &indexData[0].thing, 3, sizeof(VertexWithImplementationSpecificData)};
    MeshData data{MeshPrimitive::Triangles, DataFlag::Mutable, indexData,
        MeshIndexData{meshIndexTypeWrap(0xcaca), indices}, 1};

    Containers::String out;
    Error redirectError{&out};
    data.indices<UnsignedInt>();
    data.mutableIndices<UnsignedInt>();
    data.indicesAsArray();
    CORRADE_COMPARE(out,
        "Trade::MeshData::indices(): can't cast data from an implementation-specific index type 0xcaca\n"
        "Trade::MeshData::mutableIndices(): can't cast data from an implementation-specific index type 0xcaca\n"
        "Trade::MeshData::indicesInto(): can't extract data out of an implementation-specific index type 0xcaca\n");
}

void MeshDataTest::implementationSpecificVertexFormatWrongAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    VertexWithImplementationSpecificData vertexData[2];

    Containers::StridedArrayView1D<long double> attribute{vertexData,
        &vertexData[0].thing, 2, sizeof(VertexWithImplementationSpecificData)};
    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{MeshAttribute::Position,
            vertexFormatWrap(0xdead1), attribute},
        MeshAttributeData{MeshAttribute::Tangent,
            vertexFormatWrap(0xdead2), attribute},
        MeshAttributeData{MeshAttribute::Bitangent,
            vertexFormatWrap(0xdead3), attribute},
        MeshAttributeData{MeshAttribute::Normal,
            vertexFormatWrap(0xdead4), attribute},
        MeshAttributeData{MeshAttribute::TextureCoordinates,
            vertexFormatWrap(0xdead5), attribute},
        MeshAttributeData{MeshAttribute::Color,
            vertexFormatWrap(0xdead6), attribute},
        MeshAttributeData{MeshAttribute::JointIds,
            vertexFormatWrap(0xdead7), attribute, 2},
        MeshAttributeData{MeshAttribute::Weights,
            vertexFormatWrap(0xdead8), attribute, 2},
        MeshAttributeData{MeshAttribute::ObjectId,
            vertexFormatWrap(0xdead9), attribute}}};

    Containers::String out;
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
    data.jointIdsAsArray();
    data.weightsAsArray();
    data.objectIdsAsArray();
    CORRADE_COMPARE(out,
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead5\n"
        "Trade::MeshData::attribute(): can't cast data from an implementation-specific vertex format 0xdead6\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead5\n"
        "Trade::MeshData::mutableAttribute(): can't cast data from an implementation-specific vertex format 0xdead6\n"
        "Trade::MeshData::positions2DInto(): can't extract data out of an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::positions3DInto(): can't extract data out of an implementation-specific vertex format 0xdead1\n"
        "Trade::MeshData::tangentsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::bitangentSignsInto(): can't extract data out of an implementation-specific vertex format 0xdead2\n"
        "Trade::MeshData::bitangentsInto(): can't extract data out of an implementation-specific vertex format 0xdead3\n"
        "Trade::MeshData::normalsInto(): can't extract data out of an implementation-specific vertex format 0xdead4\n"
        "Trade::MeshData::textureCoordinatesInto(): can't extract data out of an implementation-specific vertex format 0xdead5\n"
        "Trade::MeshData::colorsInto(): can't extract data out of an implementation-specific vertex format 0xdead6\n"
        "Trade::MeshData::jointIdsInto(): can't extract data out of an implementation-specific vertex format 0xdead7\n"
        "Trade::MeshData::weightsInto(): can't extract data out of an implementation-specific vertex format 0xdead8\n"
        "Trade::MeshData::objectIdsInto(): can't extract data out of an implementation-specific vertex format 0xdead9\n");
}

void MeshDataTest::mutableAccessNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort indices[3]{};
    const Vector2 vertices[2]{};
    MeshData data{MeshPrimitive::Triangles,
        {}, indices, MeshIndexData{indices},
        {}, vertices, {
            MeshAttributeData{MeshAttribute::Position,
                Containers::arrayView(vertices)}
        }};
    CORRADE_COMPARE(data.indexDataFlags(), DataFlags{});
    CORRADE_COMPARE(data.vertexDataFlags(), DataFlags{});

    Containers::String out;
    Error redirectError{&out};
    data.mutableIndexData();
    data.mutableVertexData();
    data.mutableIndices();
    data.mutableIndices<UnsignedShort>();
    data.mutableAttribute(0);
    data.mutableAttribute<Vector2>(0);
    data.mutableAttribute<Vector2[]>(0);
    data.mutableAttribute(MeshAttribute::Position);
    data.mutableAttribute<Vector2>(MeshAttribute::Position);
    data.mutableAttribute<Vector2[]>(MeshAttribute::Position);
    CORRADE_COMPARE(out,
        "Trade::MeshData::mutableIndexData(): index data not mutable\n"
        "Trade::MeshData::mutableVertexData(): vertex data not mutable\n"
        "Trade::MeshData::mutableIndices(): index data not mutable\n"
        "Trade::MeshData::mutableIndices(): index data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n"
        "Trade::MeshData::mutableAttribute(): vertex data not mutable\n");
}

void MeshDataTest::indicesNotIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshData data{MeshPrimitive::Triangles, 37};

    Containers::String out;
    Error redirectError{&out};
    data.indexCount();
    data.indexType();
    data.indexOffset();
    data.indexStride();
    data.indices<UnsignedInt>();
    data.mutableIndices<UnsignedShort>();
    data.indicesAsArray();
    UnsignedInt a[1];
    data.indicesInto(a);
    CORRADE_COMPARE(out,
        "Trade::MeshData::indexCount(): the mesh is not indexed\n"
        "Trade::MeshData::indexType(): the mesh is not indexed\n"
        "Trade::MeshData::indexOffset(): the mesh is not indexed\n"
        "Trade::MeshData::indexStride(): the mesh is not indexed\n"
        "Trade::MeshData::indices(): the mesh is not indexed\n"
        "Trade::MeshData::mutableIndices(): the mesh is not indexed\n"
        "Trade::MeshData::indicesAsArray(): the mesh is not indexed\n"
        "Trade::MeshData::indicesInto(): the mesh is not indexed\n");
}

void MeshDataTest::indicesWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::Array<char> indexData{sizeof(UnsignedShort)};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData);
    indices[0] = 57616;
    MeshData data{MeshPrimitive::Points, Utility::move(indexData), MeshIndexData{indices}, 57617};

    Containers::String out;
    Error redirectError{&out};
    data.indices<UnsignedByte>();
    data.mutableIndices<UnsignedByte>();
    CORRADE_COMPARE(out,
        "Trade::MeshData::indices(): indices are MeshIndexType::UnsignedShort but requested MeshIndexType::UnsignedByte\n"
        "Trade::MeshData::mutableIndices(): indices are MeshIndexType::UnsignedShort but requested MeshIndexType::UnsignedByte\n");
}

void MeshDataTest::attributeNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshData data{MeshPrimitive::Points, nullptr, {
        MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector3, nullptr},
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::UnsignedByteNormalized, nullptr, 3},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedByte, nullptr, 3},
        MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector4, nullptr},
        MeshAttributeData{MeshAttribute::Weights, VertexFormat::Float, nullptr, 6},
        MeshAttributeData{MeshAttribute::JointIds, VertexFormat::UnsignedShort, nullptr, 6},
        /* Morph targets */
        MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector3ubNormalized, nullptr, 0, 37},
        MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector4usNormalized, nullptr, 0, 37},
        MeshAttributeData{MeshAttribute::Color, VertexFormat::Vector4usNormalized, nullptr, 0, 37},
    }};

    /* This is fine */
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Position), 0);
    CORRADE_COMPARE(data.attributeCount(MeshAttribute::Position, 37), 0);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Position), Containers::NullOpt);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Color, 2), Containers::NullOpt);
    CORRADE_COMPARE(data.findAttributeId(MeshAttribute::Color, 3, 37), Containers::NullOpt);

    Containers::String out;
    Error redirectError{&out};
    data.attributeData(9);
    data.attributeName(9);
    data.attributeId(9);
    data.attributeFormat(9);
    data.attributeOffset(9);
    data.attributeStride(9);
    data.attributeArraySize(9);
    data.attributeMorphTargetId(9);
    data.attribute(9);
    data.attribute<Vector2>(9);
    data.attribute<Vector2[]>(9);
    data.mutableAttribute(9);
    data.mutableAttribute<Vector2>(9);
    data.mutableAttribute<Vector2[]>(9);

    data.attributeId(MeshAttribute::Position);
    data.attributeId(MeshAttribute::Color, 2);
    data.attributeId(MeshAttribute::Color, 3, 37);
    data.attributeFormat(MeshAttribute::Position);
    data.attributeFormat(MeshAttribute::Color, 2);
    data.attributeFormat(MeshAttribute::Color, 3, 37);
    data.attributeOffset(MeshAttribute::Position);
    data.attributeOffset(MeshAttribute::Color, 2);
    data.attributeOffset(MeshAttribute::Color, 3, 37);
    data.attributeStride(MeshAttribute::Position);
    data.attributeStride(MeshAttribute::Color, 2);
    data.attributeStride(MeshAttribute::Color, 3, 37);
    data.attributeArraySize(MeshAttribute::Position);
    data.attributeArraySize(MeshAttribute::Color, 2);
    data.attributeArraySize(MeshAttribute::Color, 3, 37);
    data.attribute(MeshAttribute::Position);
    data.attribute(MeshAttribute::Color, 2);
    data.attribute(MeshAttribute::Color, 3, 37);
    data.attribute<Vector2>(MeshAttribute::Position);
    data.attribute<Vector2>(MeshAttribute::Color, 2);
    data.attribute<Vector2>(MeshAttribute::Color, 3, 37);
    data.attribute<Vector2[]>(MeshAttribute::Position);
    data.attribute<Vector2[]>(MeshAttribute::Color, 2);
    data.attribute<Vector2[]>(MeshAttribute::Color, 3, 37);
    data.mutableAttribute(MeshAttribute::Position);
    data.mutableAttribute(MeshAttribute::Color, 2);
    data.mutableAttribute(MeshAttribute::Color, 3, 37);
    data.mutableAttribute<Vector2>(MeshAttribute::Position);
    data.mutableAttribute<Vector2>(MeshAttribute::Color, 2);
    data.mutableAttribute<Vector2>(MeshAttribute::Color, 3, 37);
    data.mutableAttribute<Vector2[]>(MeshAttribute::Position);
    data.mutableAttribute<Vector2[]>(MeshAttribute::Color, 2);
    data.mutableAttribute<Vector2[]>(MeshAttribute::Color, 3, 37);

    data.positions2DAsArray();
    data.positions2DAsArray(0, 37);
    data.positions3DAsArray();
    data.positions3DAsArray(0, 37);
    data.tangentsAsArray();
    data.tangentsAsArray(0, 37);
    data.bitangentSignsAsArray();
    data.bitangentSignsAsArray(0, 37);
    data.bitangentsAsArray();
    data.bitangentsAsArray(0, 37);
    data.normalsAsArray();
    data.normalsAsArray(0, 37);
    data.textureCoordinates2DAsArray();
    data.textureCoordinates2DAsArray(0, 37);
    data.colorsAsArray(2);
    data.colorsAsArray(3, 37);
    /* jointIdsAsArray() and weightsAsArray() have their own assert in order to
       fetch array size, have to test also Into() for these. They have no morph
       targets however, so that's omitted for them. */
    data.jointIdsAsArray(2);
    data.jointIdsInto(nullptr, 2);
    data.weightsAsArray(2);
    data.weightsInto(nullptr, 2);
    /* Object IDs have no morph targets either */
    data.objectIdsAsArray();
    CORRADE_COMPARE_AS(out,
        "Trade::MeshData::attributeData(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeName(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeId(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeFormat(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeOffset(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeStride(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeArraySize(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attributeMorphTargetId(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attribute(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attribute(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::attribute(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::mutableAttribute(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::mutableAttribute(): index 9 out of range for 9 attributes\n"
        "Trade::MeshData::mutableAttribute(): index 9 out of range for 9 attributes\n"

        "Trade::MeshData::attributeId(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeId(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeId(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attributeFormat(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeFormat(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeFormat(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attributeOffset(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeOffset(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeOffset(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attributeStride(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeStride(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeStride(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attributeArraySize(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attributeArraySize(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attributeArraySize(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::attribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::attribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::attribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::mutableAttribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::mutableAttribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::mutableAttribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::mutableAttribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::mutableAttribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::mutableAttribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"
        "Trade::MeshData::mutableAttribute(): index 0 out of range for 0 Trade::MeshAttribute::Position attributes\n"
        "Trade::MeshData::mutableAttribute(): index 2 out of range for 2 Trade::MeshAttribute::Color attributes\n"
        "Trade::MeshData::mutableAttribute(): index 3 out of range for 3 Trade::MeshAttribute::Color attributes in morph target 37\n"

        "Trade::MeshData::positions2DInto(): index 0 out of range for 0 position attributes\n"
        "Trade::MeshData::positions2DInto(): index 0 out of range for 0 position attributes in morph target 37\n"
        "Trade::MeshData::positions3DInto(): index 0 out of range for 0 position attributes\n"
        "Trade::MeshData::positions3DInto(): index 0 out of range for 0 position attributes in morph target 37\n"
        "Trade::MeshData::tangentsInto(): index 0 out of range for 0 tangent attributes\n"
        "Trade::MeshData::tangentsInto(): index 0 out of range for 0 tangent attributes in morph target 37\n"
        "Trade::MeshData::bitangentSignsInto(): index 0 out of range for 0 tangent attributes\n"
        "Trade::MeshData::bitangentSignsInto(): index 0 out of range for 0 tangent attributes in morph target 37\n"
        "Trade::MeshData::bitangentsInto(): index 0 out of range for 0 bitangent attributes\n"
        "Trade::MeshData::bitangentsInto(): index 0 out of range for 0 bitangent attributes in morph target 37\n"
        "Trade::MeshData::normalsInto(): index 0 out of range for 0 normal attributes\n"
        "Trade::MeshData::normalsInto(): index 0 out of range for 0 normal attributes in morph target 37\n"
        "Trade::MeshData::textureCoordinates2DInto(): index 0 out of range for 0 texture coordinate attributes\n"
        "Trade::MeshData::textureCoordinates2DInto(): index 0 out of range for 0 texture coordinate attributes in morph target 37\n"
        "Trade::MeshData::colorsInto(): index 2 out of range for 2 color attributes\n"
        "Trade::MeshData::colorsInto(): index 3 out of range for 3 color attributes in morph target 37\n"
        "Trade::MeshData::jointIdsAsArray(): index 2 out of range for 2 joint ID attributes\n"
        "Trade::MeshData::jointIdsInto(): index 2 out of range for 2 joint ID attributes\n"
        "Trade::MeshData::weightsAsArray(): index 2 out of range for 2 weight attributes\n"
        "Trade::MeshData::weightsInto(): index 2 out of range for 2 weight attributes\n"
        "Trade::MeshData::objectIdsInto(): index 0 out of range for 0 object ID attributes\n",
        TestSuite::Compare::String);
}

void MeshDataTest::attributeWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    MeshData data{MeshPrimitive::Points, nullptr, {
        MeshAttributeData{MeshAttribute::Position, VertexFormat::Vector3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Vector4>(MeshAttribute::Position);
    data.attribute<Vector4[]>(MeshAttribute::Position);
    data.mutableAttribute<Vector4>(MeshAttribute::Position);
    data.mutableAttribute<Vector4[]>(MeshAttribute::Position);
    CORRADE_COMPARE(out,
        "Trade::MeshData::attribute(): Trade::MeshAttribute::Position is VertexFormat::Vector3 but requested a type equivalent to VertexFormat::Vector4\n"
        "Trade::MeshData::attribute(): Trade::MeshAttribute::Position is VertexFormat::Vector3 but requested a type equivalent to VertexFormat::Vector4\n"
        "Trade::MeshData::mutableAttribute(): Trade::MeshAttribute::Position is VertexFormat::Vector3 but requested a type equivalent to VertexFormat::Vector4\n"
        "Trade::MeshData::mutableAttribute(): Trade::MeshAttribute::Position is VertexFormat::Vector3 but requested a type equivalent to VertexFormat::Vector4\n");
}

void MeshDataTest::attributeWrongArrayAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 vertexData[3*4]{
        {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}, {7.0f, 8.0f},
        {1.1f, 2.2f}, {3.3f, 4.4f}, {5.5f, 6.6f}, {7.7f, 8.8f},
        {0.1f, 0.2f}, {0.3f, 0.4f}, {0.5f, 0.6f}, {0.7f, 0.8f},
    };
    Containers::StridedArrayView2D<Vector2> positions2D{vertexData, {3, 4}};

    MeshData data{MeshPrimitive::TriangleFan, DataFlag::Mutable, vertexData, {
        MeshAttributeData{meshAttributeCustom(35), positions2D}
    }};

    /* Array access is allowed for non-array attributes (the second dimension
       is then always 1), tested directly in construct() */

    Containers::String out;
    Error redirectError{&out};
    data.attribute<Vector2>(0);
    data.mutableAttribute<Vector2>(0);
    data.attribute<Vector2>(meshAttributeCustom(35));
    data.mutableAttribute<Vector2>(meshAttributeCustom(35));
    CORRADE_COMPARE(out,
        "Trade::MeshData::attribute(): Trade::MeshAttribute::Custom(35) is an array attribute, use T[] to access it\n"
        "Trade::MeshData::mutableAttribute(): Trade::MeshAttribute::Custom(35) is an array attribute, use T[] to access it\n"
        "Trade::MeshData::attribute(): Trade::MeshAttribute::Custom(35) is an array attribute, use T[] to access it\n"
        "Trade::MeshData::mutableAttribute(): Trade::MeshAttribute::Custom(35) is an array attribute, use T[] to access it\n");
}

void MeshDataTest::releaseIndexData() {
    Containers::Array<char> indexData{23};
    auto indices = Containers::arrayCast<UnsignedShort>(indexData.slice(6, 12));

    MeshData data{MeshPrimitive::TriangleStrip, Utility::move(indexData), MeshIndexData{indices}, 10};
    CORRADE_VERIFY(data.isIndexed());
    CORRADE_COMPARE(data.indexCount(), 3);
    CORRADE_COMPARE(data.indexOffset(), 6);

    Containers::Array<char> released = data.releaseIndexData();
    CORRADE_COMPARE(static_cast<void*>(released.data() + 6), indices.data());
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
    auto vertices = Containers::arrayCast<Vector2>(vertexData);

    MeshData data{MeshPrimitive::LineLoop, Utility::move(vertexData), {
        MeshAttributeData{MeshAttribute::Position, vertices},
        MeshAttributeData{MeshAttribute::Position, vertices}
    }};
    CORRADE_COMPARE(data.attributeCount(), 2);

    Containers::Array<MeshAttributeData> released = data.releaseAttributeData();
    CORRADE_COMPARE(released.size(), 2);
    CORRADE_COMPARE(static_cast<const void*>(released[0].data().data()), vertices.data());
    CORRADE_COMPARE(released[0].data().size(), 2);
    /* Unlike the other two, this is null as we don't need the value for
       calculating anything */
    CORRADE_COMPARE(static_cast<const void*>(data.attributeData()), nullptr);
    CORRADE_COMPARE(data.attributeCount(), 0);
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), vertices);
    CORRADE_COMPARE(data.vertexCount(), 2);
}

void MeshDataTest::releaseVertexData() {
    Containers::Array<char> vertexData{80};
    auto vertices = Containers::arrayCast<Vector2>(vertexData.slice(48, 72));

    MeshData data{MeshPrimitive::LineLoop, Utility::move(vertexData), {
        MeshAttributeData{MeshAttribute::Position, vertices},
        MeshAttributeData{MeshAttribute::Position, vertices}
    }};
    CORRADE_COMPARE(data.attributeCount(), 2);
    CORRADE_COMPARE(data.vertexCount(), 3);
    CORRADE_COMPARE(data.attributeOffset(0), 48);

    Containers::Array<char> released = data.releaseVertexData();
    CORRADE_VERIFY(data.attributeData());
    CORRADE_COMPARE(data.attributeCount(), 2);
    CORRADE_COMPARE(static_cast<const void*>(static_cast<const char*>(data.attribute(0).data())), vertices.data());
    CORRADE_COMPARE(static_cast<const void*>(static_cast<const char*>(data.mutableAttribute(0).data())), vertices.data());
    /* Returned views should be patched to have zero size (but not the direct
       access, there it stays as it's an internal API really) */
    CORRADE_COMPARE(data.attribute(0).size()[0], 0);
    CORRADE_COMPARE(data.mutableAttribute(0).size()[0], 0);
    CORRADE_COMPARE(data.attributeData()[0].data().size(), 3);
    CORRADE_COMPARE(static_cast<void*>(released.data() + 48), vertices.data());
    /* This is not null as we still need the value for calculating offsets */
    CORRADE_COMPARE(static_cast<const void*>(data.vertexData()), released.data());
    CORRADE_COMPARE(data.vertexCount(), 0);
    CORRADE_COMPARE(data.attributeOffset(0), 48);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::MeshDataTest)
