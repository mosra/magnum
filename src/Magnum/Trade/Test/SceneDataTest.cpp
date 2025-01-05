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

/* Including first to verify the StridedBitArrayView include is not needed */
#include "Magnum/Trade/SceneData.h"

#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Range.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/GrowableArray.h>
#endif

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneDataTest: TestSuite::Tester {
    explicit SceneDataTest();

    void mappingTypeSizeAlignment();
    void mappingTypeSizeAlignmentInvalid();
    void debugMappingType();
    void debugMappingTypePacked();

    void customFieldName();
    void customFieldNameTooLarge();
    void customFieldNameNotCustom();
    void debugFieldName();
    void debugFieldNamePacked();

    void fieldTypeSizeAlignment();
    void fieldTypeSizeAlignmentInvalid();
    void debugFieldType();
    void debugFieldTypePacked();

    void debugFieldFlag();
    void debugFieldFlagPacked();
    void debugFieldFlags();
    void debugFieldFlagsPacked();
    void debugFieldFlagsSupersets();

    void constructField();
    void constructFieldDefault();
    void constructFieldCustom();
    void constructFieldBit();
    void constructFieldString();
    void constructFieldStringNegativeStride();
    void constructFieldStringNegativeOffset();
    void constructFieldTypeErased();
    void constructFieldTypeErasedBit();
    void constructFieldTypeErasedString();
    void constructFieldTypeErased2D();
    void constructFieldTypeErased2DBit();
    void constructFieldTypeErased2DString();
    void constructFieldNonOwningArray();
    void constructFieldOffsetOnly();
    void constructFieldOffsetOnlyBit();
    void constructFieldOffsetOnlyString();
    void constructFieldOffsetOnlyStringNegativeStride();
    void constructFieldArray();
    void constructFieldArrayBit();
    void constructFieldArrayTypeErased();
    void constructFieldArrayTypeErasedBit();
    /* no constructFieldArrayTypeErasedString(), strings can't be arrays */
    void constructFieldArrayTypeErased2D();
    void constructFieldArrayTypeErased2DBit();
    /* no constructFieldArrayTypeErased2DString(), strings can't be arrays */
    void constructFieldArrayOffsetOnly();
    void constructFieldArrayOffsetOnlyBit();
    /* no constructFieldArrayOffsetOnlyString(), strings can't be arrays */

    void constructFieldWrongType();
    void constructFieldWrongTypeBit();
    void constructFieldWrongTypeString();
    void constructFieldInconsistentViewSize();
    void constructFieldTooLargeMappingStride();
    void constructFieldTooLargeFieldStride();
    void constructFieldFlagNotAllowed();
    void constructFieldWrongOffsetOnlyDataAccess();
    void constructFieldWrongBitDataAccess();
    void constructFieldWrongStringDataAccess();
    void constructFieldTypeErased2DWrongSize();
    void constructFieldTypeErased2DNonContiguous();
    void constructFieldArrayNonContiguous();
    void constructFieldArrayNotAllowed();
    void constructFieldArrayTypeErased2DWrongSize();
    void constructFieldArrayTypeErased2DNonContiguous();
    void constructFieldBitTooLargeBitOffset();
    void constructFieldBitTooLargeSize();
    #ifndef CORRADE_TARGET_32BIT
    void constructFieldStringDataTooFarApart();
    #endif

    void construct();
    void constructZeroFields();
    void constructZeroObjects();
    void constructNotOwned();
    void constructBit();
    template<class T> void constructString();
    void constructSpecialStrides();
    void constructSpecialStridesBit();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructDeprecated();
    void constructDeprecatedBoth2DAnd3D();
    #endif

    #ifndef CORRADE_TARGET_32BIT
    void constructMappingOver4GB();
    void constructFieldOver4GB();
    #endif

    void constructDuplicateField();
    void constructDuplicateCustomField();
    void constructInconsistentMappingType();
    void constructMappingDataNotContained();
    void constructFieldDataNotContained();
    void constructBitFieldDataNotContained();
    void constructStringDataNotContained();
    void constructMappingTypeTooSmall();
    void constructNotOwnedFlagOwned();
    void constructMismatchedTRSViews();
    template<class T> void constructMismatchedTRSDimensionality();
    void constructMismatchedMeshMaterialView();
    void constructAmbiguousSkinDimensions();

    void constructCopy();
    void constructMove();

    void findFieldId();
    template<class T> void findFieldObjectOffset();
    void findFieldObjectOffsetInvalidOffset();
    void fieldObjectOffsetNotFound();

    template<class T> void mappingAsArrayByIndex();
    template<class T> void mappingAsArrayByName();
    void mappingIntoArrayByIndex();
    void mappingIntoArrayByName();
    void mappingIntoArrayInvalidSizeOrOffset();

    template<class T> void parentsAsArray();
    void parentsIntoArray();
    void parentsIntoArrayInvalidSizeOrOffset();
    template<class T> void transformations2DAsArray();
    template<class T, class U, class V> void transformations2DAsArrayTRS();
    void transformations2DAsArrayBut3DType();
    void transformations2DIntoArray();
    void transformations2DTRSIntoArray();
    void transformations2DIntoArrayTRS();
    void transformations2DIntoArrayInvalidSizeOrOffset();
    void transformations2DIntoArrayInvalidSizeOrOffsetTRS();
    template<class T> void transformations3DAsArray();
    template<class T, class U, class V> void transformations3DAsArrayTRS();
    void transformations3DAsArrayBut2DType();
    void transformations3DIntoArray();
    void transformations3DTRSIntoArray();
    void transformations3DIntoArrayTRS();
    void transformations3DIntoArrayInvalidSizeOrOffset();
    void transformations3DIntoArrayInvalidSizeOrOffsetTRS();
    template<class T, class U> void meshesMaterialsAsArray();
    void meshesMaterialsIntoArray();
    void meshesMaterialsIntoArrayInvalidSizeOrOffset();
    template<class T> void lightsAsArray();
    void lightsIntoArray();
    void lightsIntoArrayInvalidSizeOrOffset();
    template<class T> void camerasAsArray();
    void camerasIntoArray();
    void camerasIntoArrayInvalidSizeOrOffset();
    template<class T> void skinsAsArray();
    void skinsIntoArray();
    void skinsIntoArrayInvalidSizeOrOffset();
    template<class T> void importerStateAsArray();
    void importerStateIntoArray();
    void importerStateIntoArrayInvalidSizeOrOffset();

    void mutableAccessNotAllowed();

    void mappingNotFound();
    void mappingWrongType();

    void fieldNotFound();
    void fieldWrongType();
    void fieldWrongPointerType();
    void fieldWrongArrayAccess();

    void parentFor();
    void parentForTrivialParent();
    void childrenFor();
    void childrenForTrivialParent();
    void transformation2DFor();
    void transformation2DForTRS();
    void transformation2DForBut3DType();
    void transformation3DFor();
    void transformation3DForTRS();
    void transformation3DForBut2DType();
    void meshesMaterialsFor();
    void lightsFor();
    void camerasFor();
    void skinsFor();
    void importerStateFor();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void childrenDeprecated();
    #endif

    void fieldForFieldMissing();
    void findFieldObjectOffsetInvalidObject();

    void releaseFieldData();
    void releaseData();
};

const struct {
    const char* name;
    DataFlags dataFlags;
} NotOwnedData[]{
    {"", {}},
    {"mutable", DataFlag::Mutable}
};

const struct {
    const char* name;
    SceneFieldFlags flags;
    UnsignedInt mapping[5];
    UnsignedInt object;
    UnsignedInt offset;
    Containers::Optional<std::size_t> expected;
} FindFieldObjectOffsetData[]{
    {"", {},
        {4, 2, 1, 0, 2}, 2, 0, 1},
    {"not found", {},
        {4, 2, 1, 0, 2}, 3, 0, Containers::NullOpt},
    {"offset", {},
        {4, 2, 1, 0, 2}, 2, 2, 4},
    {"offset, not found", {},
        {4, 2, 1, 0, 2}, 2, 5, Containers::NullOpt},

    {"ordered", SceneFieldFlag::OrderedMapping,
        {1, 3, 4, 4, 5}, 4, 0, 2},
    {"ordered, not found", SceneFieldFlag::OrderedMapping,
        /* It *is* there but the binary search expects an ordered range and thus
           should not even see it */
        {1, 3, 4, 4, 2}, 2, 0, Containers::NullOpt},
    {"ordered, not found, too small", SceneFieldFlag::OrderedMapping,
        {1, 3, 4, 4, 5}, 0, 0, Containers::NullOpt},
    {"ordered, not found, too large", SceneFieldFlag::OrderedMapping,
        {1, 3, 4, 4, 5}, 6, 0, Containers::NullOpt},
    {"ordered, offset", SceneFieldFlag::OrderedMapping,
        {1, 3, 4, 4, 5}, 4, 3, 3},
    {"ordered, offset, not found", SceneFieldFlag::OrderedMapping,
        {1, 3, 4, 4, 5}, 4, 4, Containers::NullOpt},

    {"implicit", SceneFieldFlag::ImplicitMapping,
        /* Not there but the assumption is that the ID matches the offset */
        {5, 5, 5, 5, 5}, 3, 0, 3},
    {"implicit, not found", SceneFieldFlag::ImplicitMapping,
        /* Is there but the assumption is that the ID matches the offset, which
           is out of range */
        {5, 5, 5, 5, 5}, 5, 0, Containers::NullOpt},
    {"implicit, offset", SceneFieldFlag::ImplicitMapping,
        /* Not there but the assumption is that the ID matches the offset;
           verifying that the offset is properly accounted for */
        {5, 5, 5, 5, 5}, 3, 3, 3},
    {"implicit, offset, not found, less than offset", SceneFieldFlag::ImplicitMapping,
        /* Cerifying that the offset is properly accounted for -- it's never
           found if offset > id */
        {5, 5, 5, 5, 5}, 3, 4, Containers::NullOpt},
    {"implicit, offset, not found, out of range", SceneFieldFlag::ImplicitMapping,
        {5, 5, 5, 5, 5}, 5, 4, Containers::NullOpt}
};

const struct {
    const char* name;
    std::size_t offset;
    std::size_t size;
    std::size_t expectedSize;
} IntoArrayOffsetData[]{
    {"whole", 0, 3, 3},
    {"one element in the middle", 1, 1, 1},
    {"suffix to a larger array", 2, 10, 1},
    {"offset at the end", 3, 10, 0}
};

const struct {
    const char* name;
    std::size_t offset;
    std::size_t size;
    std::size_t expectedSize;
    bool mapping, field;
} IntoArrayOffset1Data[]{
    {"whole", 0, 3, 3, true, true},
    {"one element in the middle", 1, 1, 1, true, true},
    {"suffix to a larger array", 2, 10, 1, true, true},
    {"offset at the end", 3, 10, 0, true, true},
    {"only mapping", 0, 3, 3, true, false},
    {"only field", 0, 3, 3, false, true},
    {"neither", 0, 3, 0, false, false}
};

const struct {
    const char* name;
    std::size_t offset;
    std::size_t size;
    std::size_t expectedSize;
    bool mapping, field1, field2;
} IntoArrayOffset2Data[]{
    {"whole", 0, 3, 3, true, true, true},
    {"one element in the middle", 1, 1, 1, true, true, true},
    {"suffix to a larger array", 2, 10, 1, true, true, true},
    {"offset at the end", 3, 10, 0, true, true, true},
    {"only mapping", 0, 3, 3, true, false, false},
    {"only fields", 0, 3, 3, false, true, true},
    {"only first field", 0, 3, 3, false, true, false},
    {"only second field", 0, 3, 3, false, false, true},
    {"none", 0, 3, 0, false, false, false}
};

const struct {
    const char* name;
    std::size_t offset;
    std::size_t size;
    std::size_t expectedSize;
    bool mapping, field1, field2, field3;
} IntoArrayOffset3Data[]{
    {"whole", 0, 3, 3, true, true, true, true},
    {"one element in the middle", 1, 1, 1, true, true, true, true},
    {"suffix to a larger array", 2, 10, 1, true, true, true, true},
    {"offset at the end", 3, 10, 0, true, true, true, true},
    {"only mapping", 0, 3, 3, true, false, false, true},
    {"only fields", 0, 3, 3, false, true, true, true},
    {"only first field", 0, 3, 3, false, true, false, false},
    {"only second field", 0, 3, 3, false, false, true, false},
    {"only third field", 0, 3, 3, false, false, false, true},
    {"none", 0, 3, 0, false, false, false, false}
};

const struct {
    const char* name;
    std::ptrdiff_t stride;
    std::size_t offset, bitOffset, arrayBitOffset;
    UnsignedByte expectedBits;
    UnsignedByte expectedArrayBits[2];
} ConstructSpecialStridesBitData[]{
    {"negative stride", -2*8, 3*2, 3, 5,
        0x0d /* 0b1101 */,
        {0x03 /* 0b0011 */, 0x0d /* 0b1101 */}},
    {"zero stride, all ones", 0, 0, 3, 5,
        0x0f, {0x00, 0x0f}},
    /* No change for the array, as it exhibits both values */
    {"zero stride, all zeros", 0, 0, 2, 5,
        0x00, {0x00, 0x0f}},
};

#ifdef MAGNUM_BUILD_DEPRECATED
const struct {
    const char* name;
    bool is2D;
    bool is3D;
    bool skipParent;
} ChildrenDeprecatedData[]{
    {"2D", true, false, false},
    {"2D with no parent", true, false, true},
    {"3D", false, true, false},
    {"3D with no parent", false, true, true},
    {"neither", false, false, false},
    {"neither with no parent", false, false, true}
};
#endif

SceneDataTest::SceneDataTest() {
    addTests({&SceneDataTest::mappingTypeSizeAlignment,
              &SceneDataTest::mappingTypeSizeAlignmentInvalid,
              &SceneDataTest::debugMappingType,
              &SceneDataTest::debugMappingTypePacked,

              &SceneDataTest::customFieldName,
              &SceneDataTest::customFieldNameTooLarge,
              &SceneDataTest::customFieldNameNotCustom,
              &SceneDataTest::debugFieldName,
              &SceneDataTest::debugFieldNamePacked,

              &SceneDataTest::fieldTypeSizeAlignment,
              &SceneDataTest::fieldTypeSizeAlignmentInvalid,
              &SceneDataTest::debugFieldType,
              &SceneDataTest::debugFieldTypePacked,

              &SceneDataTest::debugFieldFlag,
              &SceneDataTest::debugFieldFlagPacked,
              &SceneDataTest::debugFieldFlags,
              &SceneDataTest::debugFieldFlagsPacked,
              &SceneDataTest::debugFieldFlagsSupersets,

              &SceneDataTest::constructField,
              &SceneDataTest::constructFieldDefault,
              &SceneDataTest::constructFieldCustom,
              &SceneDataTest::constructFieldBit,
              &SceneDataTest::constructFieldString,
              &SceneDataTest::constructFieldStringNegativeStride,
              &SceneDataTest::constructFieldStringNegativeOffset,
              &SceneDataTest::constructFieldTypeErased,
              &SceneDataTest::constructFieldTypeErasedBit,
              &SceneDataTest::constructFieldTypeErasedString,
              &SceneDataTest::constructFieldTypeErased2D,
              &SceneDataTest::constructFieldTypeErased2DBit,
              &SceneDataTest::constructFieldTypeErased2DString,
              &SceneDataTest::constructFieldNonOwningArray,
              &SceneDataTest::constructFieldOffsetOnly,
              &SceneDataTest::constructFieldOffsetOnlyBit,
              &SceneDataTest::constructFieldOffsetOnlyString,
              &SceneDataTest::constructFieldOffsetOnlyStringNegativeStride,
              &SceneDataTest::constructFieldArray,
              &SceneDataTest::constructFieldArrayBit,
              &SceneDataTest::constructFieldArrayTypeErased,
              &SceneDataTest::constructFieldArrayTypeErasedBit,
              &SceneDataTest::constructFieldArrayTypeErased2D,
              &SceneDataTest::constructFieldArrayTypeErased2DBit,
              &SceneDataTest::constructFieldArrayOffsetOnly,
              &SceneDataTest::constructFieldArrayOffsetOnlyBit,

              &SceneDataTest::constructFieldWrongType,
              &SceneDataTest::constructFieldWrongTypeBit,
              &SceneDataTest::constructFieldWrongTypeString,
              &SceneDataTest::constructFieldInconsistentViewSize,
              &SceneDataTest::constructFieldTooLargeMappingStride,
              &SceneDataTest::constructFieldTooLargeFieldStride,
              &SceneDataTest::constructFieldFlagNotAllowed,
              &SceneDataTest::constructFieldWrongOffsetOnlyDataAccess,
              &SceneDataTest::constructFieldWrongBitDataAccess,
              &SceneDataTest::constructFieldWrongStringDataAccess,
              &SceneDataTest::constructFieldTypeErased2DWrongSize,
              &SceneDataTest::constructFieldTypeErased2DNonContiguous,
              &SceneDataTest::constructFieldArrayNonContiguous,
              &SceneDataTest::constructFieldArrayNotAllowed,
              &SceneDataTest::constructFieldArrayTypeErased2DWrongSize,
              &SceneDataTest::constructFieldArrayTypeErased2DNonContiguous,
              &SceneDataTest::constructFieldBitTooLargeBitOffset,
              &SceneDataTest::constructFieldBitTooLargeSize,
              #ifndef CORRADE_TARGET_32BIT
              &SceneDataTest::constructFieldStringDataTooFarApart,
              #endif

              &SceneDataTest::construct,
              &SceneDataTest::constructZeroFields,
              &SceneDataTest::constructZeroObjects});

    addInstancedTests({&SceneDataTest::constructNotOwned},
        Containers::arraySize(NotOwnedData));

    addTests({&SceneDataTest::constructBit,
              &SceneDataTest::constructString<UnsignedByte>,
              &SceneDataTest::constructString<UnsignedShort>,
              &SceneDataTest::constructString<UnsignedInt>,
              &SceneDataTest::constructString<UnsignedLong>,
              &SceneDataTest::constructSpecialStrides});

    addInstancedTests({&SceneDataTest::constructSpecialStridesBit},
        Containers::arraySize(ConstructSpecialStridesBitData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&SceneDataTest::constructDeprecated},
        Containers::arraySize(ChildrenDeprecatedData));

    addTests({&SceneDataTest::constructDeprecatedBoth2DAnd3D});
    #endif

    #ifndef CORRADE_TARGET_32BIT
    addTests({&SceneDataTest::constructMappingOver4GB,
              &SceneDataTest::constructFieldOver4GB});
    #endif

    addTests({&SceneDataTest::constructDuplicateField,
              &SceneDataTest::constructDuplicateCustomField,
              &SceneDataTest::constructInconsistentMappingType,
              &SceneDataTest::constructMappingDataNotContained,
              &SceneDataTest::constructFieldDataNotContained,
              &SceneDataTest::constructBitFieldDataNotContained,
              &SceneDataTest::constructStringDataNotContained,
              &SceneDataTest::constructMappingTypeTooSmall,
              &SceneDataTest::constructNotOwnedFlagOwned,
              &SceneDataTest::constructMismatchedTRSViews,
              &SceneDataTest::constructMismatchedTRSDimensionality<Float>,
              &SceneDataTest::constructMismatchedTRSDimensionality<Double>,
              &SceneDataTest::constructMismatchedMeshMaterialView,
              &SceneDataTest::constructAmbiguousSkinDimensions,

              &SceneDataTest::constructCopy,
              &SceneDataTest::constructMove,

              &SceneDataTest::findFieldId});

    addInstancedTests<SceneDataTest>({
        &SceneDataTest::findFieldObjectOffset<UnsignedByte>,
        &SceneDataTest::findFieldObjectOffset<UnsignedShort>,
        &SceneDataTest::findFieldObjectOffset<UnsignedInt>,
        &SceneDataTest::findFieldObjectOffset<UnsignedLong>
    }, Containers::arraySize(FindFieldObjectOffsetData));

    addTests({&SceneDataTest::findFieldObjectOffsetInvalidOffset,
              &SceneDataTest::fieldObjectOffsetNotFound,

              &SceneDataTest::mappingAsArrayByIndex<UnsignedByte>,
              &SceneDataTest::mappingAsArrayByIndex<UnsignedShort>,
              &SceneDataTest::mappingAsArrayByIndex<UnsignedInt>,
              &SceneDataTest::mappingAsArrayByIndex<UnsignedLong>,
              &SceneDataTest::mappingAsArrayByName<UnsignedByte>,
              &SceneDataTest::mappingAsArrayByName<UnsignedShort>,
              &SceneDataTest::mappingAsArrayByName<UnsignedInt>,
              &SceneDataTest::mappingAsArrayByName<UnsignedLong>});

    addInstancedTests({&SceneDataTest::mappingIntoArrayByIndex,
                       &SceneDataTest::mappingIntoArrayByName},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::mappingIntoArrayInvalidSizeOrOffset,

              &SceneDataTest::parentsAsArray<Byte>,
              &SceneDataTest::parentsAsArray<Short>,
              &SceneDataTest::parentsAsArray<Int>,
              &SceneDataTest::parentsAsArray<Long>});

    addInstancedTests({&SceneDataTest::parentsIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addTests({&SceneDataTest::parentsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations2DAsArray<Matrix3>,
              &SceneDataTest::transformations2DAsArray<Matrix3d>,
              &SceneDataTest::transformations2DAsArray<Matrix3x2>,
              &SceneDataTest::transformations2DAsArray<Matrix3x2d>,
              &SceneDataTest::transformations2DAsArray<DualComplex>,
              &SceneDataTest::transformations2DAsArray<DualComplexd>,
              &SceneDataTest::transformations2DAsArrayTRS<Float, Float, Double>,
              &SceneDataTest::transformations2DAsArrayTRS<Double, Double, Float>,
              &SceneDataTest::transformations2DAsArrayBut3DType});

    addInstancedTests({&SceneDataTest::transformations2DIntoArray,
                       &SceneDataTest::transformations2DTRSIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addInstancedTests({&SceneDataTest::transformations2DIntoArrayTRS},
        Containers::arraySize(IntoArrayOffset3Data));

    addTests({&SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffsetTRS,
              &SceneDataTest::transformations3DAsArray<Matrix4>,
              &SceneDataTest::transformations3DAsArray<Matrix4d>,
              &SceneDataTest::transformations3DAsArray<Matrix4x3>,
              &SceneDataTest::transformations3DAsArray<Matrix4x3d>,
              &SceneDataTest::transformations3DAsArray<DualQuaternion>,
              &SceneDataTest::transformations3DAsArray<DualQuaterniond>,
              &SceneDataTest::transformations3DAsArrayTRS<Float, Double, Double>,
              &SceneDataTest::transformations3DAsArrayTRS<Double, Float, Float>,
              &SceneDataTest::transformations3DAsArrayBut2DType});

    addInstancedTests({&SceneDataTest::transformations3DIntoArray,
                       &SceneDataTest::transformations3DTRSIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addInstancedTests({&SceneDataTest::transformations3DIntoArrayTRS},
        Containers::arraySize(IntoArrayOffset3Data));

    addTests({&SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffsetTRS,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedByte, Int>,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedShort, Byte>,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedInt, Short>});

    addInstancedTests({&SceneDataTest::meshesMaterialsIntoArray},
        Containers::arraySize(IntoArrayOffset2Data));

    addTests({&SceneDataTest::meshesMaterialsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::lightsAsArray<UnsignedByte>,
              &SceneDataTest::lightsAsArray<UnsignedShort>,
              &SceneDataTest::lightsAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::lightsIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addTests({&SceneDataTest::lightsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::camerasAsArray<UnsignedByte>,
              &SceneDataTest::camerasAsArray<UnsignedShort>,
              &SceneDataTest::camerasAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::camerasIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addTests({&SceneDataTest::camerasIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::skinsAsArray<UnsignedByte>,
              &SceneDataTest::skinsAsArray<UnsignedShort>,
              &SceneDataTest::skinsAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::skinsIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addTests({&SceneDataTest::skinsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::importerStateAsArray<const void*>,
              &SceneDataTest::importerStateAsArray<void*>});

    addInstancedTests({&SceneDataTest::importerStateIntoArray},
        Containers::arraySize(IntoArrayOffset1Data));

    addTests({&SceneDataTest::importerStateIntoArrayInvalidSizeOrOffset,

              &SceneDataTest::mutableAccessNotAllowed,

              &SceneDataTest::mappingNotFound,
              &SceneDataTest::mappingWrongType,

              &SceneDataTest::fieldNotFound,
              &SceneDataTest::fieldWrongType,
              &SceneDataTest::fieldWrongPointerType,
              &SceneDataTest::fieldWrongArrayAccess,

              &SceneDataTest::parentFor,
              &SceneDataTest::parentForTrivialParent,
              &SceneDataTest::childrenFor,
              &SceneDataTest::childrenForTrivialParent,
              &SceneDataTest::transformation2DFor,
              &SceneDataTest::transformation2DForTRS,
              &SceneDataTest::transformation2DForBut3DType,
              &SceneDataTest::transformation3DFor,
              &SceneDataTest::transformation3DForTRS,
              &SceneDataTest::transformation3DForBut2DType,
              &SceneDataTest::meshesMaterialsFor,
              &SceneDataTest::lightsFor,
              &SceneDataTest::camerasFor,
              &SceneDataTest::skinsFor,
              &SceneDataTest::importerStateFor});

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&SceneDataTest::childrenDeprecated},
        Containers::arraySize(ChildrenDeprecatedData));
    #endif

    addTests({&SceneDataTest::fieldForFieldMissing,
              &SceneDataTest::findFieldObjectOffsetInvalidObject,

              &SceneDataTest::releaseFieldData,
              &SceneDataTest::releaseData});
}

using namespace Containers::Literals;
using namespace Math::Literals;

void SceneDataTest::mappingTypeSizeAlignment() {
    CORRADE_COMPARE(sceneMappingTypeSize(SceneMappingType::UnsignedByte), 1);
    CORRADE_COMPARE(sceneMappingTypeAlignment(SceneMappingType::UnsignedByte), 1);
    CORRADE_COMPARE(sceneMappingTypeSize(SceneMappingType::UnsignedShort), 2);
    CORRADE_COMPARE(sceneMappingTypeAlignment(SceneMappingType::UnsignedShort), 2);
    CORRADE_COMPARE(sceneMappingTypeSize(SceneMappingType::UnsignedInt), 4);
    CORRADE_COMPARE(sceneMappingTypeAlignment(SceneMappingType::UnsignedInt), 4);
    CORRADE_COMPARE(sceneMappingTypeSize(SceneMappingType::UnsignedLong), 8);
    CORRADE_COMPARE(sceneMappingTypeAlignment(SceneMappingType::UnsignedLong), 8);
}

void SceneDataTest::mappingTypeSizeAlignmentInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    sceneMappingTypeSize(SceneMappingType{});
    sceneMappingTypeAlignment(SceneMappingType{});
    sceneMappingTypeSize(SceneMappingType(0x73));
    sceneMappingTypeAlignment(SceneMappingType(0x73));

    CORRADE_COMPARE(out,
        "Trade::sceneMappingTypeSize(): invalid type Trade::SceneMappingType(0x0)\n"
        "Trade::sceneMappingTypeAlignment(): invalid type Trade::SceneMappingType(0x0)\n"
        "Trade::sceneMappingTypeSize(): invalid type Trade::SceneMappingType(0x73)\n"
        "Trade::sceneMappingTypeAlignment(): invalid type Trade::SceneMappingType(0x73)\n");
}

void SceneDataTest::debugMappingType() {
    Containers::String out;
    Debug{&out} << SceneMappingType::UnsignedLong << SceneMappingType(0x73);
    CORRADE_COMPARE(out, "Trade::SceneMappingType::UnsignedLong Trade::SceneMappingType(0x73)\n");
}

void SceneDataTest::debugMappingTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneMappingType::UnsignedLong << Debug::packed << SceneMappingType(0x73) << SceneMappingType::UnsignedInt;
    CORRADE_COMPARE(out, "UnsignedLong 0x73 Trade::SceneMappingType::UnsignedInt\n");
}

void SceneDataTest::customFieldName() {
    CORRADE_VERIFY(!isSceneFieldCustom(SceneField::Rotation));
    CORRADE_VERIFY(!isSceneFieldCustom(SceneField(0x0fffffffu)));
    CORRADE_VERIFY(isSceneFieldCustom(SceneField(Implementation::SceneFieldCustom)));
    CORRADE_VERIFY(isSceneFieldCustom(SceneField(0x80000000u)));

    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0)), 0x80000000u);
    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0xabcd)), 0x8000abcdu);
    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0x7fffffff)), 0xffffffffu);

    CORRADE_COMPARE(sceneFieldCustom(SceneField(Implementation::SceneFieldCustom)), 0);
    CORRADE_COMPARE(sceneFieldCustom(SceneField(0x8000abcdu)), 0xabcd);
    CORRADE_COMPARE(sceneFieldCustom(SceneField(0xffffffffu)), 0x7fffffffu);

    constexpr bool is = isSceneFieldCustom(SceneField(0x8000abcdu));
    CORRADE_VERIFY(is);
    constexpr SceneField a = sceneFieldCustom(0xabcd);
    CORRADE_COMPARE(UnsignedInt(a), 0x8000abcdu);
    constexpr UnsignedInt b = sceneFieldCustom(a);
    CORRADE_COMPARE(b, 0xabcd);
}

void SceneDataTest::customFieldNameTooLarge() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    sceneFieldCustom(1u << 31);
    CORRADE_COMPARE(out, "Trade::sceneFieldCustom(): index 2147483648 too large\n");
}

void SceneDataTest::customFieldNameNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    sceneFieldCustom(SceneField::Transformation);
    CORRADE_COMPARE(out, "Trade::sceneFieldCustom(): Trade::SceneField::Transformation is not custom\n");
}

void SceneDataTest::debugFieldName() {
    Containers::String out;
    Debug{&out} << SceneField::Transformation << sceneFieldCustom(73) << SceneField(0xdeadda7);
    CORRADE_COMPARE(out, "Trade::SceneField::Transformation Trade::SceneField::Custom(73) Trade::SceneField(0xdeadda7)\n");
}

void SceneDataTest::debugFieldNamePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneField::Transformation << Debug::packed << sceneFieldCustom(73) << Debug::packed << SceneField(0xdeadda7) << SceneField::Parent;
    CORRADE_COMPARE(out, "Transformation Custom(73) 0xdeadda7 Trade::SceneField::Parent\n");
}

void SceneDataTest::fieldTypeSizeAlignment() {
    /* Test at least one of every size */
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Byte), sizeof(Byte));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Degh), sizeof(Degh));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Vector3ub), sizeof(Vector3ub));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Range1Dh), sizeof(Range1Dh));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Vector3s), sizeof(Vector3s));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Long), sizeof(Long));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x2h), sizeof(Matrix3x2h));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix4x2h), sizeof(Matrix4x2h));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x3h), sizeof(Matrix3x3h));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Range3Di), sizeof(Range3Di));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::DualQuaternion), sizeof(DualQuaternion));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x3), sizeof(Matrix3x3));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x2d), sizeof(Matrix3x2d));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::DualQuaterniond), sizeof(DualQuaterniond));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x3d), sizeof(Matrix3x3d));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix3x4d), sizeof(Matrix3x4d));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Matrix4x4d), sizeof(Matrix4x4d));
    CORRADE_COMPARE(sceneFieldTypeSize(SceneFieldType::Pointer), sizeof(const void*));

    /* Test at least one of every alignment */
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::Vector3ub), alignof(UnsignedByte));
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::Matrix3x3h), alignof(Half));
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::Range3Di), alignof(UnsignedInt));
    /* alignof(Double) is 4 on Android x86, which is stupid, so hardcode 8 */
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::DualComplexd), 8);
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::Pointer), alignof(const void*));
}

void SceneDataTest::fieldTypeSizeAlignmentInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    sceneFieldTypeSize(SceneFieldType{});
    sceneFieldTypeAlignment(SceneFieldType{});
    sceneFieldTypeSize(SceneFieldType(0xdead));
    sceneFieldTypeAlignment(SceneFieldType(0xdead));
    sceneFieldTypeSize(SceneFieldType::Bit);
    sceneFieldTypeAlignment(SceneFieldType::Bit);

    CORRADE_COMPARE(out,
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0x0)\n"
        "Trade::sceneFieldTypeAlignment(): invalid type Trade::SceneFieldType(0x0)\n"
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0xdead)\n"
        "Trade::sceneFieldTypeAlignment(): invalid type Trade::SceneFieldType(0xdead)\n"
        "Trade::sceneFieldTypeSize(): can't use with Trade::SceneFieldType::Bit\n"
        "Trade::sceneFieldTypeAlignment(): can't use with Trade::SceneFieldType::Bit\n");
}

void SceneDataTest::debugFieldType() {
    Containers::String out;
    Debug{&out} << SceneFieldType::Matrix3x4h << SceneFieldType(0xdead);
    CORRADE_COMPARE(out, "Trade::SceneFieldType::Matrix3x4h Trade::SceneFieldType(0xdead)\n");
}

void SceneDataTest::debugFieldTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneFieldType::Matrix3x4h << Debug::packed << SceneFieldType(0xdead) << SceneFieldType::Float;
    CORRADE_COMPARE(out, "Matrix3x4h 0xdead Trade::SceneFieldType::Float\n");
}

void SceneDataTest::debugFieldFlag() {
    Containers::String out;
    Debug{&out} << SceneFieldFlag::OffsetOnly << SceneFieldFlag(0xbe);
    CORRADE_COMPARE(out, "Trade::SceneFieldFlag::OffsetOnly Trade::SceneFieldFlag(0xbe)\n");
}

void SceneDataTest::debugFieldFlagPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << SceneFieldFlag::OffsetOnly << Debug::packed << SceneFieldFlag(0xbe) << SceneFieldFlag::ImplicitMapping;
    CORRADE_COMPARE(out, "OffsetOnly 0xbe Trade::SceneFieldFlag::ImplicitMapping\n");
}

void SceneDataTest::debugFieldFlags() {
    Containers::String out;
    Debug{&out} << (SceneFieldFlag::OffsetOnly|SceneFieldFlag(0xe0)) << SceneFieldFlags{};
    CORRADE_COMPARE(out, "Trade::SceneFieldFlag::OffsetOnly|Trade::SceneFieldFlag(0xe0) Trade::SceneFieldFlags{}\n");
}

void SceneDataTest::debugFieldFlagsPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (SceneFieldFlag::OffsetOnly|SceneFieldFlag(0xe0)) << Debug::packed << SceneFieldFlags{} << (SceneFieldFlag::OffsetOnly|SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(out, "OffsetOnly|0xe0 {} Trade::SceneFieldFlag::OffsetOnly|Trade::SceneFieldFlag::ImplicitMapping\n");
}

void SceneDataTest::debugFieldFlagsSupersets() {
    /* ImplicitMapping is a superset of OrderedMapping, so only one should be
       printed */
    {
        Containers::String out;
        Debug{&out} << (SceneFieldFlag::ImplicitMapping|SceneFieldFlag::OrderedMapping);
        CORRADE_COMPARE(out, "Trade::SceneFieldFlag::ImplicitMapping\n");
    }
}

constexpr UnsignedShort RotationMapping2D[3]{};
constexpr Complexd RotationField2D[3]
    /* Can't use {} because GCC 4.8 then complains that "array must be
       initialized with a brace-enclosed initializer". OTOH, can't use this
       without {} because MSVC then thinks it's not a constant expression. */
    #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
    {}
    #endif
    ;

void SceneDataTest::constructField() {
    const UnsignedShort rotationMappingData[3]{};
    const Complexd rotationFieldData[3];

    SceneFieldData rotations{SceneField::Rotation, Containers::arrayView(rotationMappingData), Containers::arrayView(rotationFieldData), SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(rotations.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.size(), 3);
    CORRADE_COMPARE(rotations.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(rotations.mappingData().size(), 3);
    CORRADE_COMPARE(rotations.mappingData().stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(rotations.mappingData().data(), rotationMappingData);
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.fieldArraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), sizeof(Complexd));
    CORRADE_COMPARE(rotations.fieldData().data(), rotationFieldData);

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(Complexd)];
    CORRADE_COMPARE(rotations.mappingData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.mappingData(someArray).stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(rotations.mappingData(someArray).data(), rotationMappingData);
    CORRADE_COMPARE(rotations.fieldData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.fieldData(someArray).stride(), sizeof(Complexd));
    CORRADE_COMPARE(rotations.fieldData(someArray).data(), rotationFieldData);

    constexpr SceneFieldData crotations{SceneField::Rotation, Containers::arrayView(RotationMapping2D), Containers::arrayView(RotationField2D), SceneFieldFlag::ImplicitMapping};
    constexpr SceneField name = crotations.name();
    constexpr SceneFieldFlags flags = crotations.flags();
    constexpr std::size_t size = crotations.size();
    constexpr SceneMappingType mappingType = crotations.mappingType();
    CORRADE_COMPARE(name, SceneField::Rotation);
    CORRADE_COMPARE(flags, SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedShort);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(crotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(crotations.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(crotations.mappingData().size(), 3);
    CORRADE_COMPARE(crotations.mappingData().stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(crotations.mappingData().data(), RotationMapping2D);
    CORRADE_COMPARE(crotations.fieldData().size(), 3);
    CORRADE_COMPARE(crotations.fieldData().stride(), sizeof(Complexd));
    CORRADE_COMPARE(crotations.fieldData().data(), RotationField2D);
}

void SceneDataTest::constructFieldDefault() {
    SceneFieldData data;
    CORRADE_COMPARE(data.name(), SceneField{});
    CORRADE_COMPARE(data.fieldType(), SceneFieldType{});
    CORRADE_COMPARE(data.mappingType(), SceneMappingType{});

    constexpr SceneFieldData cdata;
    CORRADE_COMPARE(cdata.name(), SceneField{});
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType{});
    CORRADE_COMPARE(cdata.mappingType(), SceneMappingType{});
}

void SceneDataTest::constructFieldCustom() {
    /* Verifying it doesn't hit any assertion about disallowed type for given
       attribute */

    const UnsignedByte rangeMappingData[3]{};
    const Range2Dh rangeFieldData[3];
    SceneFieldData ranges{sceneFieldCustom(13), Containers::arrayView(rangeMappingData), Containers::arrayView(rangeFieldData)};
    CORRADE_COMPARE(ranges.name(), sceneFieldCustom(13));
    CORRADE_COMPARE(ranges.mappingType(), SceneMappingType::UnsignedByte);
    CORRADE_VERIFY(ranges.mappingData().data() == rangeMappingData);
    CORRADE_COMPARE(ranges.fieldType(), SceneFieldType::Range2Dh);
    CORRADE_VERIFY(ranges.fieldData().data() == rangeFieldData);
}

constexpr UnsignedShort HiddenMapping[8]{};
constexpr char HiddenField[5]{};

void SceneDataTest::constructFieldBit() {
    const UnsignedShort hiddenMappingData[8]{};
    const char hiddenFieldData[5]{};

    SceneFieldData hidden{sceneFieldCustom(773), Containers::arrayView(hiddenMappingData), Containers::StridedBitArrayView1D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, 8, 3}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(hidden.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(hidden.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(hidden.size(), 8);
    CORRADE_COMPARE(hidden.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(hidden.mappingData().size(), 8);
    CORRADE_COMPARE(hidden.mappingData().stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(hidden.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(hidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(hidden.fieldArraySize(), 0);
    CORRADE_COMPARE(hidden.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(hidden.fieldBitData().offset(), 5);
    CORRADE_COMPARE(hidden.fieldBitData().size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(hidden.fieldBitData().stride(), (Containers::Stride2D{3, 1}));

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[8*2];
    CORRADE_COMPARE(hidden.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(hidden.mappingData(someArray).size(), 8);
    CORRADE_COMPARE(hidden.mappingData(someArray).stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(hidden.mappingData(someArray).data(), hiddenMappingData);
    CORRADE_COMPARE(hidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(hidden.fieldArraySize(), 0);
    CORRADE_COMPARE(hidden.fieldBitData(someArray).data(), hiddenFieldData + 1);
    CORRADE_COMPARE(hidden.fieldBitData(someArray).offset(), 5);
    CORRADE_COMPARE(hidden.fieldBitData(someArray).size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(hidden.fieldBitData(someArray).stride(), (Containers::Stride2D{3, 1}));

    constexpr SceneFieldData chidden{sceneFieldCustom(773), Containers::arrayView(HiddenMapping), Containers::StridedBitArrayView1D{Containers::BitArrayView{HiddenField + 1, 5, 24}, 8, 3}, SceneFieldFlag::ImplicitMapping};
    constexpr SceneField name = chidden.name();
    constexpr SceneFieldFlags flags = chidden.flags();
    constexpr std::size_t size = chidden.size();
    constexpr SceneMappingType mappingType = chidden.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(size, 8);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedShort);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(chidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(chidden.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(chidden.mappingData(someArray).size(), 8);
    CORRADE_COMPARE(chidden.mappingData(someArray).stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(chidden.mappingData(someArray).data(), HiddenMapping);
    CORRADE_COMPARE(chidden.fieldBitData(someArray).data(), HiddenField + 1);
    CORRADE_COMPARE(chidden.fieldBitData(someArray).offset(), 5);
    CORRADE_COMPARE(chidden.fieldBitData(someArray).size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(chidden.fieldBitData(someArray).stride(), (Containers::Stride2D{3, 1}));
}

void SceneDataTest::constructFieldString() {
    const UnsignedLong nameMappingData[3]{};

    /* The string data is after the field data so the stored offset is
       positive. Needs to be put into a struct as otherwise the compiler may
       reorder it however it wants. Negative offset is tested in
       constructFieldStringNegativeOffset() below. */
    const struct {
        Containers::Pair<UnsignedShort, UnsignedShort> nameField[3];
        char nameString[15];
    } data{};
    CORRADE_VERIFY(static_cast<const void*>(data.nameString) > static_cast<const void*>(data.nameField));

    SceneFieldData names{sceneFieldCustom(25), Containers::arrayView(nameMappingData), data.nameString, SceneFieldType::StringRange16, Containers::arrayView(data.nameField), SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(names.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(names.name(), sceneFieldCustom(25));
    CORRADE_COMPARE(names.size(), 3);
    CORRADE_COMPARE(names.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(names.mappingData().size(), 3);
    CORRADE_COMPARE(names.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(names.mappingData().data(), nameMappingData);
    CORRADE_COMPARE(names.fieldType(), SceneFieldType::StringRange16);
    CORRADE_COMPARE(names.fieldArraySize(), 0);
    CORRADE_COMPARE(names.fieldData().size(), 3);
    CORRADE_COMPARE(names.fieldData().stride(), sizeof(Containers::Pair<UnsignedShort, UnsignedShort>));
    CORRADE_COMPARE(names.fieldData().data(), data.nameField);
    CORRADE_COMPARE(names.stringData(), static_cast<const void*>(data.nameString));

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(UnsignedLong)];
    CORRADE_COMPARE(names.mappingData(someArray).size(), 3);
    CORRADE_COMPARE(names.mappingData(someArray).stride(), sizeof(UnsignedLong));
    CORRADE_VERIFY(names.mappingData(someArray).data() == nameMappingData);
    CORRADE_COMPARE(names.fieldData(someArray).size(), 3);
    CORRADE_COMPARE(names.fieldData(someArray).stride(), sizeof(Containers::Pair<UnsignedShort, UnsignedShort>));
    CORRADE_COMPARE(names.fieldData(someArray).data(), data.nameField);
    CORRADE_COMPARE(names.stringData(someArray), static_cast<const void*>(data.nameString));

    /* Construction of a string field is not constexpr due to arithmetic on two
       (differently cast) pointers */
}

void SceneDataTest::constructFieldStringNegativeStride() {
    const UnsignedLong nameMappingData[3]{};

    /* The string data is after the field data so the stored offset is
       positive. Needs to be put into a struct as otherwise the compiler may
       reorder it however it wants. Negative offset is tested in
       constructFieldStringNegativeOffset() below. */
    const struct {
        UnsignedInt nameField[3];
        char nameString[15];
    } data{};
    CORRADE_VERIFY(static_cast<const void*>(data.nameString) > static_cast<const void*>(data.nameField));

    SceneFieldData names{sceneFieldCustom(25), Containers::arrayView(nameMappingData), data.nameString, SceneFieldType::StringRangeNullTerminated32, Containers::stridedArrayView(data.nameField).flipped<0>(), SceneFieldFlag::OrderedMapping};
    /* NullTerminatedString added implicitly for this type */
    CORRADE_COMPARE(names.flags(), SceneFieldFlag::OrderedMapping|SceneFieldFlag::NullTerminatedString);

    /* Testing only the properties related to the stride/offset unpack */
    CORRADE_COMPARE(names.fieldData().size(), 3);
    /* Uhh C++ WHAT THE HELL? "error C4146: unary minus operator applied to
       unsigned type, result still unsigned" */
    CORRADE_COMPARE(names.fieldData().stride(), -std::ptrdiff_t(sizeof(UnsignedInt)));
    CORRADE_COMPARE(names.fieldData().data(), data.nameField + 2);
    CORRADE_COMPARE(names.stringData(), static_cast<const void*>(data.nameString));

    /* This is a separate code path, but should do the same */
    char someArray[3*sizeof(UnsignedLong)];
    CORRADE_COMPARE(names.fieldData(someArray).size(), 3);
    /* Uhh C++ WHAT THE HELL? "error C4146: unary minus operator applied to
       unsigned type, result still unsigned" */
    CORRADE_COMPARE(names.fieldData(someArray).stride(), -std::ptrdiff_t(sizeof(UnsignedInt)));
    CORRADE_COMPARE(names.fieldData(someArray).data(), data.nameField + 2);
    CORRADE_COMPARE(names.stringData(someArray), static_cast<const void*>(data.nameString));
}

void SceneDataTest::constructFieldStringNegativeOffset() {
    const UnsignedLong nameMappingData[3]{};

    /* Compared to constructFieldString(), the string data is before the field
       data so the stored offset is negative. Needd to be put into a struct as
       otherwise the compiler may reorder it however it wants. */
    const struct {
        char nameString[15];
        Containers::Pair<UnsignedShort, UnsignedShort> nameField[3];
    } data{};
    CORRADE_VERIFY(static_cast<const void*>(data.nameString) < static_cast<const void*>(data.nameField));

    SceneFieldData names{sceneFieldCustom(25), Containers::arrayView(nameMappingData), data.nameString, SceneFieldType::StringRange16, Containers::arrayView(data.nameField), SceneFieldFlag::NullTerminatedString|SceneFieldFlag::OrderedMapping};

    /* Testing only the properties related to the stride/offset unpack */
    CORRADE_COMPARE(names.fieldData().size(), 3);
    CORRADE_COMPARE(names.fieldData().stride(), sizeof(Containers::Pair<UnsignedShort, UnsignedShort>));
    CORRADE_COMPARE(names.fieldData().data(), data.nameField);
    CORRADE_COMPARE(names.stringData(), static_cast<const void*>(data.nameString));

    /* This is a separate code path, but should do the same */
    char someArray[3*sizeof(UnsignedLong)];
    CORRADE_COMPARE(names.fieldData(someArray).size(), 3);
    CORRADE_COMPARE(names.fieldData(someArray).stride(), sizeof(Containers::Pair<UnsignedShort, UnsignedShort>));
    CORRADE_COMPARE(names.fieldData(someArray).data(), data.nameField);
    CORRADE_COMPARE(names.stringData(someArray), static_cast<const void*>(data.nameString));
}

constexpr char ScalingMappingData[3*sizeof(UnsignedLong)]{};
constexpr char ScalingFieldData[3*sizeof(Vector3)]{};

void SceneDataTest::constructFieldTypeErased() {
    const UnsignedLong scalingMappingData[3]{};
    const Vector3 scalingFieldData[3];
    SceneFieldData scalings{SceneField::Scaling, SceneMappingType::UnsignedLong, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingMappingData)), SceneFieldType::Vector3, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingFieldData)), SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(scalings.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scalings.name(), SceneField::Scaling);
    CORRADE_COMPARE(scalings.size(), 3);
    CORRADE_COMPARE(scalings.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(scalings.mappingData().size(), 3);
    CORRADE_COMPARE(scalings.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(scalings.mappingData().data(), scalingMappingData);
    CORRADE_COMPARE(scalings.fieldType(), SceneFieldType::Vector3);
    CORRADE_COMPARE(scalings.fieldArraySize(), 0);
    CORRADE_COMPARE(scalings.fieldData().size(), 3);
    CORRADE_COMPARE(scalings.fieldData().stride(), sizeof(Vector3));
    CORRADE_COMPARE(scalings.fieldData().data(), scalingFieldData);

    constexpr SceneFieldData cscalings{SceneField::Scaling, SceneMappingType::UnsignedLong, Containers::StridedArrayView1D<const void>{ScalingMappingData, 3, sizeof(UnsignedLong)}, SceneFieldType::Vector3, Containers::StridedArrayView1D<const void>{ScalingFieldData, 3, sizeof(Vector3)}, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = cscalings.name();
    constexpr SceneFieldFlags flags = cscalings.flags();
    constexpr std::size_t size = cscalings.size();
    constexpr SceneMappingType mappingType = cscalings.mappingType();
    CORRADE_COMPARE(name, SceneField::Scaling);
    CORRADE_COMPARE(flags, SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedLong);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldTypeErasedString()) */
    CORRADE_COMPARE(cscalings.fieldType(), SceneFieldType::Vector3);
    CORRADE_COMPARE(cscalings.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(cscalings.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(cscalings.mappingData().size(), 3);
    CORRADE_COMPARE(cscalings.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(cscalings.mappingData().data(), ScalingMappingData);
    CORRADE_COMPARE(cscalings.fieldData().size(), 3);
    CORRADE_COMPARE(cscalings.fieldData().stride(), sizeof(Vector3));
    CORRADE_COMPARE(cscalings.fieldData().data(), ScalingFieldData);
}

void SceneDataTest::constructFieldTypeErasedBit() {
    const UnsignedShort hiddenMappingData[8]{};
    const char hiddenFieldData[5]{};

    SceneFieldData hidden{sceneFieldCustom(773), SceneMappingType::UnsignedShort, Containers::arrayCast<const char>(Containers::stridedArrayView(hiddenMappingData)), Containers::StridedBitArrayView1D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, 8, 3}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(hidden.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(hidden.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(hidden.size(), 8);
    CORRADE_COMPARE(hidden.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(hidden.mappingData().size(), 8);
    CORRADE_COMPARE(hidden.mappingData().stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(hidden.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(hidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(hidden.fieldArraySize(), 0);
    CORRADE_COMPARE(hidden.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(hidden.fieldBitData().offset(), 5);
    CORRADE_COMPARE(hidden.fieldBitData().size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(hidden.fieldBitData().stride(), (Containers::Stride2D{3, 1}));

    constexpr SceneFieldData chidden{sceneFieldCustom(773), SceneMappingType::UnsignedShort, Containers::StridedArrayView1D<const void>{HiddenMapping, 8, sizeof(UnsignedShort)}, Containers::StridedBitArrayView1D{Containers::BitArrayView{HiddenField + 1, 5, 24}, 8, 3}, SceneFieldFlag::ImplicitMapping};
    constexpr SceneField name = chidden.name();
    constexpr SceneFieldFlags flags = chidden.flags();
    constexpr std::size_t size = chidden.size();
    constexpr SceneMappingType mappingType = chidden.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(size, 8);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedShort);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(chidden.mappingData().size(), 8);
    CORRADE_COMPARE(chidden.mappingData().stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(chidden.mappingData().data(), HiddenMapping);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(chidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(chidden.fieldArraySize(), 0);
    CORRADE_COMPARE(chidden.fieldBitData().data(), HiddenField + 1);
    CORRADE_COMPARE(chidden.fieldBitData().offset(), 5);
    CORRADE_COMPARE(chidden.fieldBitData().size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(chidden.fieldBitData().stride(), (Containers::Stride2D{3, 1}));
}

void SceneDataTest::constructFieldTypeErasedString() {
    const UnsignedLong nameMappingData[3]{};
    const char nameStringData[15]{};
    const Containers::Pair<UnsignedShort, UnsignedShort> nameFieldData[3]{};

    SceneFieldData names{sceneFieldCustom(25), SceneMappingType::UnsignedLong, Containers::arrayCast<const char>(Containers::stridedArrayView(nameMappingData)), nameStringData, SceneFieldType::StringRange16, Containers::arrayCast<const char>(Containers::stridedArrayView(nameFieldData)), SceneFieldFlag::NullTerminatedString|SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(names.flags(), SceneFieldFlag::OrderedMapping|SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(names.name(), sceneFieldCustom(25));
    CORRADE_COMPARE(names.size(), 3);
    CORRADE_COMPARE(names.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(names.mappingData().size(), 3);
    CORRADE_COMPARE(names.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(names.mappingData().data(), nameMappingData);
    CORRADE_COMPARE(names.fieldType(), SceneFieldType::StringRange16);
    CORRADE_COMPARE(names.fieldArraySize(), 0);
    CORRADE_COMPARE(names.fieldData().size(), 3);
    CORRADE_COMPARE(names.fieldData().stride(), sizeof(UnsignedShort)*2);
    CORRADE_COMPARE(names.fieldData().data(), nameFieldData);
    CORRADE_COMPARE(names.stringData(), static_cast<const void*>(nameStringData));

    /* Construction of a string field is not constexpr due to arithmetic on two
       (differently cast) pointers */
}

void SceneDataTest::constructFieldTypeErased2D() {
    char rotationMappingData[6*sizeof(UnsignedShort)];
    char rotationFieldData[6*sizeof(Complexd)];
    auto rotationMappingView = Containers::StridedArrayView2D<char>{rotationMappingData, {6, sizeof(UnsignedShort)}}.every(2);
    auto rotationFieldView = Containers::StridedArrayView2D<char>{rotationFieldData, {6, sizeof(Complexd)}}.every(2);

    SceneFieldData rotations{SceneField::Rotation, rotationMappingView, SceneFieldType::Complexd, rotationFieldView, SceneFieldFlag::ImplicitMapping};
    CORRADE_COMPARE(rotations.flags(), SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.size(), 3);
    CORRADE_COMPARE(rotations.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(rotations.mappingData().size(), 3);
    CORRADE_COMPARE(rotations.mappingData().stride(), 2*sizeof(UnsignedShort));
    CORRADE_COMPARE(rotations.mappingData().data(), rotationMappingView.data());
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.fieldArraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), 2*sizeof(Complexd));
    CORRADE_COMPARE(rotations.fieldData().data(), rotationFieldView.data());

    /* 2D type-erased construction is not constexpr due to branching to select
       a corresponding SceneMappingType */
}

void SceneDataTest::constructFieldTypeErased2DBit() {
    char hiddenMappingData[16*sizeof(UnsignedShort)]{};
    char hiddenFieldData[5]{};
    auto hiddenMappingView = Containers::StridedArrayView2D<char>{hiddenMappingData, {16, sizeof(UnsignedShort)}}.every(2);
    /* No type-erased 2D view for the bit field data -- the 2D view constructor
       is the array constructor which is tested in constructFieldArrayBit() */

    SceneFieldData hidden{sceneFieldCustom(773), hiddenMappingView, Containers::StridedBitArrayView1D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, 8, 3}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(hidden.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(hidden.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(hidden.size(), 8);
    CORRADE_COMPARE(hidden.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(hidden.mappingData().size(), 8);
    CORRADE_COMPARE(hidden.mappingData().stride(), 2*sizeof(UnsignedShort));
    CORRADE_COMPARE(hidden.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(hidden.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(hidden.fieldArraySize(), 0);
    CORRADE_COMPARE(hidden.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(hidden.fieldBitData().offset(), 5);
    CORRADE_COMPARE(hidden.fieldBitData().size(), (Containers::Size2D{8, 1}));
    CORRADE_COMPARE(hidden.fieldBitData().stride(), (Containers::Stride2D{3, 1}));

    /* 2D type-erased construction is not constexpr due to branching to select
       a corresponding SceneMappingType */
}

void SceneDataTest::constructFieldTypeErased2DString() {
    char nameMappingData[6*sizeof(UnsignedLong)]{};
    char nameFieldData[6*sizeof(Containers::Pair<UnsignedShort, UnsignedShort>)]{};
    auto nameMappingView = Containers::StridedArrayView2D<char>{nameMappingData, {6, sizeof(UnsignedLong)}}.every(2);
    auto nameFieldView = Containers::StridedArrayView2D<char>{nameFieldData, {6, sizeof(Containers::Pair<UnsignedShort, UnsignedShort>)}}.every(2);
    const char nameStringData[15]{};

    SceneFieldData names{sceneFieldCustom(25), nameMappingView, nameStringData, SceneFieldType::StringRange16, nameFieldView, SceneFieldFlag::NullTerminatedString|SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(names.flags(), SceneFieldFlag::OrderedMapping|SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(names.name(), sceneFieldCustom(25));
    CORRADE_COMPARE(names.size(), 3);
    CORRADE_COMPARE(names.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(names.mappingData().size(), 3);
    CORRADE_COMPARE(names.mappingData().stride(), 2*sizeof(UnsignedLong));
    CORRADE_COMPARE(names.mappingData().data(), nameMappingView.data());
    CORRADE_COMPARE(names.fieldType(), SceneFieldType::StringRange16);
    CORRADE_COMPARE(names.fieldArraySize(), 0);
    CORRADE_COMPARE(names.fieldData().size(), 3);
    CORRADE_COMPARE(names.fieldData().stride(), 2*sizeof(UnsignedShort)*2);
    CORRADE_COMPARE(names.fieldData().data(), nameFieldView.data());
    CORRADE_COMPARE(names.stringData(), static_cast<const void*>(nameStringData));

    /* 2D type-erased construction is not constexpr due to branching to select
       a corresponding SceneMappingType */
}

void SceneDataTest::constructFieldNonOwningArray() {
    const SceneFieldData data[3];
    Containers::Array<SceneFieldData> array = sceneFieldDataNonOwningArray(data);
    CORRADE_COMPARE(array.size(), 3);
    CORRADE_COMPARE(static_cast<const void*>(array.data()), data);
}

void SceneDataTest::constructFieldOffsetOnly() {
    struct Data {
        Byte parent;
        UnsignedShort object;
        Vector2 translation;
    } data[]{
        {0, 2, {2.0f, 3.0f}},
        {0, 15, {67.0f, -1.1f}}
    };

    SceneFieldData a{SceneField::Translation, 2, SceneMappingType::UnsignedShort, offsetof(Data, object), sizeof(Data), SceneFieldType::Vector2, offsetof(Data, translation), sizeof(Data), SceneFieldFlag::ImplicitMapping};
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(a.name(), SceneField::Translation);
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(a.mappingData(data).size(), 2);
    CORRADE_COMPARE(a.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(a.mappingData(data)),
        Containers::arrayView<UnsignedShort>({2, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Vector2);
    CORRADE_COMPARE(a.fieldArraySize(), 0);
    CORRADE_COMPARE(a.fieldData(data).size(), 2);
    CORRADE_COMPARE(a.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.fieldData(data)),
        Containers::arrayView<Vector2>({{2.0f, 3.0f}, {67.0f, -1.1f}}),
        TestSuite::Compare::Container);

    constexpr SceneFieldData ca{SceneField::Translation, 2, SceneMappingType::UnsignedShort, offsetof(Data, object), sizeof(Data), SceneFieldType::Vector2, offsetof(Data, translation), sizeof(Data), SceneFieldFlag::ImplicitMapping};
    constexpr SceneField name = ca.name();
    constexpr SceneFieldFlags flags = ca.flags();
    constexpr std::size_t size = ca.size();
    constexpr SceneMappingType mappingType = ca.mappingType();
    CORRADE_COMPARE(name, SceneField::Translation);
    CORRADE_COMPARE(flags, SceneFieldFlag::OffsetOnly|SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(size, 2);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedShort);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(ca.fieldType(), SceneFieldType::Vector2);
    CORRADE_COMPARE(ca.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(ca.mappingData(data).size(), 2);
    CORRADE_COMPARE(ca.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(ca.mappingData(data)),
        Containers::arrayView<UnsignedShort>({2, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(ca.fieldData(data).size(), 2);
    CORRADE_COMPARE(ca.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(ca.fieldData(data)),
        Containers::arrayView<Vector2>({{2.0f, 3.0f}, {67.0f, -1.1f}}),
        TestSuite::Compare::Container);
}

void SceneDataTest::constructFieldOffsetOnlyBit() {
    struct Data {
        Byte parent;
        UnsignedInt object;
        char yesInFifthBit;
    } data[]{
        {0, 2, '\x10'},
        {0, 15, '\xef'},
        {0, 22, '\x10'}
    };

    SceneFieldData a{sceneFieldCustom(773), 3, SceneMappingType::UnsignedInt, offsetof(Data, object), sizeof(Data), offsetof(Data, yesInFifthBit), 4, sizeof(Data)*8, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(a.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.mappingType(), SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(a.mappingData(data).size(), 3);
    CORRADE_COMPARE(a.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedInt>(a.mappingData(data)),
        Containers::arrayView<UnsignedInt>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(a.fieldArraySize(), 0);
    CORRADE_COMPARE(a.fieldBitData(data).data(), &data[0].yesInFifthBit);
    CORRADE_COMPARE(a.fieldBitData(data).offset(), 4);
    CORRADE_COMPARE(a.fieldBitData(data).size(), (Containers::Size2D{3, 1}));
    CORRADE_COMPARE(a.fieldBitData(data).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
    CORRADE_COMPARE_AS((a.fieldBitData(data).transposed<0, 1>())[0],
        Containers::stridedArrayView({true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);

    constexpr SceneFieldData ca{sceneFieldCustom(773), 3, SceneMappingType::UnsignedInt, offsetof(Data, object), sizeof(Data), offsetof(Data, yesInFifthBit), 4, sizeof(Data)*8, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = ca.name();
    constexpr SceneFieldFlags flags = ca.flags();
    constexpr std::size_t size = ca.size();
    constexpr SceneMappingType mappingType = ca.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedInt);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(ca.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(ca.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(ca.mappingData(data).size(), 3);
    CORRADE_COMPARE(ca.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedInt>(ca.mappingData(data)),
        Containers::arrayView<UnsignedInt>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(ca.fieldBitData(data).offset(), 4);
    CORRADE_COMPARE(ca.fieldBitData(data).size(), (Containers::Size2D{3, 1}));
    CORRADE_COMPARE(ca.fieldBitData(data).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
    CORRADE_COMPARE_AS((ca.fieldBitData(data).transposed<0, 1>())[0],
        Containers::stridedArrayView({true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);
}

void SceneDataTest::constructFieldOffsetOnlyString() {
    const char string[] = "NAMES:eyehandnoseleg";
    struct Data {
        Byte parent;
        UnsignedLong object;
        Containers::Pair<UnsignedByte, UnsignedByte> nameRange;
    } data[]{
        {0, 2, {3, 4}},
        {0, 15, {11, 3}}
    };

    SceneFieldData a{sceneFieldCustom(36), 2, SceneMappingType::UnsignedLong, offsetof(Data, object), sizeof(Data), 6, SceneFieldType::StringRange8, offsetof(Data, nameRange), sizeof(Data), SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(a.name(), sceneFieldCustom(36));
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(a.mappingData(data).size(), 2);
    CORRADE_COMPARE(a.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedLong>(a.mappingData(data)),
        Containers::arrayView<UnsignedLong>({2, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::StringRange8);
    CORRADE_COMPARE(a.fieldArraySize(), 0);
    CORRADE_COMPARE(a.fieldData(data).size(), 2);
    CORRADE_COMPARE(a.fieldData(data).stride(), sizeof(Data));

    auto fieldData = Containers::arrayCast<const Containers::Pair<UnsignedByte, UnsignedByte>>(a.fieldData(data));
    CORRADE_COMPARE_AS(fieldData, (Containers::arrayView<Containers::Pair<UnsignedByte, UnsignedByte>>({
        {3, 4}, {11, 3}
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE(a.stringData(string), "eyehandnoseleg"_s);
    CORRADE_COMPARE((Containers::StringView{a.stringData(string) + fieldData[0].first(), fieldData[0].second()}), "hand");
    CORRADE_COMPARE((Containers::StringView{a.stringData(string) + fieldData[1].first(), fieldData[1].second()}), "leg");

    constexpr SceneFieldData ca{sceneFieldCustom(36), 2, SceneMappingType::UnsignedLong, offsetof(Data, object), sizeof(Data), 6, SceneFieldType::StringRange8, offsetof(Data, nameRange), sizeof(Data), SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = ca.name();
    constexpr SceneFieldFlags flags = ca.flags();
    constexpr std::size_t size = ca.size();
    constexpr SceneMappingType mappingType = ca.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(36));
    CORRADE_COMPARE(flags, SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 2);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedLong);
    /* These are not marked constexpr because it wouldn't work for string
       fields due to the type/size stored in an union */
    CORRADE_COMPARE(ca.fieldType(), SceneFieldType::StringRange8);
    CORRADE_COMPARE(ca.fieldArraySize(), 0);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(ca.mappingData(data).size(), 2);
    CORRADE_COMPARE(ca.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedLong>(ca.mappingData(data)),
        Containers::arrayView<UnsignedLong>({2, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(ca.fieldData(data).size(), 2);
    CORRADE_COMPARE(ca.fieldData(data).stride(), sizeof(Data));
}

void SceneDataTest::constructFieldOffsetOnlyStringNegativeStride() {
    const char string[] = "NAMES:eye\0hand\0nose\0leg";
    struct Data {
        UnsignedLong object;
        UnsignedShort nameRange;
    } data[]{
        {2, 4},
        {15, 14}
    };

    SceneFieldData a{sceneFieldCustom(36), 2, SceneMappingType::UnsignedLong, offsetof(Data, object), sizeof(Data), 6, SceneFieldType::StringRangeNullTerminated16, offsetof(Data, nameRange) + sizeof(Data), -std::ptrdiff_t(sizeof(Data)), SceneFieldFlag::OrderedMapping};

    /* NullTerminatedString added implicitly for this type */
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping|SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::StringRangeNullTerminated16);
    CORRADE_COMPARE(a.fieldArraySize(), 0);
    CORRADE_COMPARE(a.fieldData(data).size(), 2);
    /* Uhh C++ WHAT THE HELL? "error C4146: unary minus operator applied to
       unsigned type, result still unsigned" */
    CORRADE_COMPARE(a.fieldData(data).stride(), -std::ptrdiff_t(sizeof(Data)));

    /* Order flipped compared to constructFieldOffsetOnlyString() */
    auto fieldData = Containers::arrayCast<const UnsignedShort>(a.fieldData(data));
    CORRADE_COMPARE_AS(fieldData, Containers::arrayView<UnsignedShort>({
        14, 4,
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(a.stringData(string), "eye"_s); /* after \0 it's lost */
    CORRADE_COMPARE(a.stringData(string) + fieldData[0], "leg"_s);
    CORRADE_COMPARE(a.stringData(string) + fieldData[1], "hand"_s);
}

constexpr UnsignedByte ArrayOffsetMappingData[3]{};
constexpr Int ArrayOffsetFieldData[3*4]{};

void SceneDataTest::constructFieldArray() {
    UnsignedByte offsetMappingData[3];
    Int offsetFieldData[3*4];
    SceneFieldData data{sceneFieldCustom(34), Containers::arrayView(offsetMappingData), Containers::StridedArrayView2D<Int>{offsetFieldData, {3, 4}}, SceneFieldFlag::ImplicitMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.mappingData().data() == offsetMappingData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetFieldData);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), Containers::arrayView(ArrayOffsetMappingData), Containers::StridedArrayView2D<const Int>{ArrayOffsetFieldData, {3, 4}}, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = cdata.name();
    constexpr SceneFieldFlags flags = cdata.flags();
    constexpr std::size_t size = cdata.size();
    constexpr SceneMappingType mappingType = cdata.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(34));
    CORRADE_COMPARE(flags, SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedByte);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(cdata.mappingData().size(), 3);
    CORRADE_COMPARE(cdata.mappingData().stride(), sizeof(UnsignedByte));
    CORRADE_COMPARE(cdata.mappingData().data(), ArrayOffsetMappingData);
    CORRADE_COMPARE(cdata.fieldData().size(), 3);
    CORRADE_COMPARE(cdata.fieldData().stride(), 4*sizeof(Int));
    CORRADE_COMPARE(cdata.fieldData().data(), ArrayOffsetFieldData);
}

constexpr UnsignedLong ArrayHiddenMapping[3]{};
constexpr char ArrayHiddenField[5]{};

void SceneDataTest::constructFieldArrayBit() {
    const UnsignedLong hiddenMappingData[3]{};
    const char hiddenFieldData[5]{};

    SceneFieldData data{sceneFieldCustom(773), Containers::arrayView(hiddenMappingData), Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, {3, 4}, {8, 1}}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(data.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(data.fieldBitData().offset(), 5);
    CORRADE_COMPARE(data.fieldBitData().size(), (Containers::Size2D{3, 4}));
    CORRADE_COMPARE(data.fieldBitData().stride(), (Containers::Stride2D{8, 1}));

    constexpr SceneFieldData cdata{sceneFieldCustom(773), Containers::arrayView(ArrayHiddenMapping), Containers::StridedBitArrayView2D{Containers::BitArrayView{ArrayHiddenField + 1, 5, 24}, {3, 4}, {8, 1}}, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = cdata.name();
    constexpr SceneFieldFlags flags = cdata.flags();
    constexpr std::size_t size = cdata.size();
    constexpr SceneMappingType mappingType = cdata.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedLong);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(cdata.mappingData().size(), 3);
    CORRADE_COMPARE(cdata.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(cdata.mappingData().data(), ArrayHiddenMapping);
    CORRADE_COMPARE(cdata.fieldBitData().data(), ArrayHiddenField + 1);
    CORRADE_COMPARE(cdata.fieldBitData().offset(), 5);
    CORRADE_COMPARE(cdata.fieldBitData().size(), (Containers::Size2D{3, 4}));
    CORRADE_COMPARE(cdata.fieldBitData().stride(), (Containers::Stride2D{8, 1}));
}

void SceneDataTest::constructFieldArrayTypeErased() {
    UnsignedByte offsetMappingData[3];
    Int offsetFieldData[3*4];
    Containers::StridedArrayView1D<Int> offset{offsetFieldData, 3, 4*sizeof(Int)};
    SceneFieldData data{sceneFieldCustom(34), SceneMappingType::UnsignedByte, Containers::arrayCast<const char>(Containers::stridedArrayView(offsetMappingData)), SceneFieldType::Int, Containers::arrayCast<const char>(offset), 4, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.mappingData().data() == offsetMappingData);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetFieldData);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), SceneMappingType::UnsignedByte, Containers::StridedArrayView1D<const void>{ArrayOffsetMappingData}, SceneFieldType::Int, Containers::StridedArrayView1D<const void>{ArrayOffsetFieldData, 3, 4*sizeof(Int)}, 4, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = cdata.name();
    constexpr SceneFieldFlags flags = cdata.flags();
    constexpr std::size_t size = cdata.size();
    constexpr SceneMappingType mappingType = cdata.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(34));
    CORRADE_COMPARE(flags, SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedByte);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(cdata.mappingData().size(), 3);
    CORRADE_COMPARE(cdata.mappingData().stride(), sizeof(UnsignedByte));
    CORRADE_COMPARE(cdata.mappingData().data(), ArrayOffsetMappingData);
    CORRADE_COMPARE(cdata.fieldData().size(), 3);
    CORRADE_COMPARE(cdata.fieldData().stride(), 4*sizeof(Int));
    CORRADE_COMPARE(cdata.fieldData().data(), ArrayOffsetFieldData);
}

void SceneDataTest::constructFieldArrayTypeErasedBit() {
    const UnsignedLong hiddenMappingData[3]{};
    const char hiddenFieldData[5]{};

    SceneFieldData data{sceneFieldCustom(773), SceneMappingType::UnsignedLong, Containers::arrayCast<const char>(Containers::stridedArrayView(hiddenMappingData)), Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, {3, 4}, {8, 1}}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(data.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(data.fieldBitData().offset(), 5);
    CORRADE_COMPARE(data.fieldBitData().size(), (Containers::Size2D{3, 4}));
    CORRADE_COMPARE(data.fieldBitData().stride(), (Containers::Stride2D{8, 1}));

    constexpr SceneFieldData cdata{sceneFieldCustom(773), SceneMappingType::UnsignedLong, Containers::StridedArrayView1D<const void>{ArrayHiddenMapping}, Containers::StridedBitArrayView2D{Containers::BitArrayView{ArrayHiddenField + 1, 5, 24}, {3, 4}, {8, 1}}, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = cdata.name();
    constexpr SceneFieldFlags flags = cdata.flags();
    constexpr std::size_t size = cdata.size();
    constexpr SceneMappingType mappingType = cdata.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedLong);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(cdata.mappingData().size(), 3);
    CORRADE_COMPARE(cdata.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(cdata.mappingData().data(), ArrayHiddenMapping);
    CORRADE_COMPARE(cdata.fieldBitData().data(), ArrayHiddenField + 1);
    CORRADE_COMPARE(cdata.fieldBitData().offset(), 5);
    CORRADE_COMPARE(cdata.fieldBitData().size(), (Containers::Size2D{3, 4}));
    CORRADE_COMPARE(cdata.fieldBitData().stride(), (Containers::Stride2D{8, 1}));
}

void SceneDataTest::constructFieldArrayTypeErased2D() {
    char offsetMappingData[3*sizeof(UnsignedByte)];
    char offsetFieldData[3*4*sizeof(Int)];
    SceneFieldData data{sceneFieldCustom(34), Containers::StridedArrayView2D<char>{offsetMappingData, {3, sizeof(UnsignedByte)}}, SceneFieldType::Int, Containers::StridedArrayView2D<char>{offsetFieldData, {3, 4*sizeof(Int)}}, 4, SceneFieldFlag::ImplicitMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.mappingData().data() == offsetMappingData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetFieldData);

    /* 2D type-erased construction is not constexpr due to branching to select
       a corresponding SceneMappingType */
}

void SceneDataTest::constructFieldArrayTypeErased2DBit() {
    char hiddenMappingData[3*sizeof(UnsignedLong)]{};
    char hiddenFieldData[5]{};

    SceneFieldData data{sceneFieldCustom(773), Containers::StridedArrayView2D<const char>{hiddenMappingData, {3, sizeof(UnsignedLong)}}, Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData + 1, 5, 24}, {3, 4}, {8, 1}}, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(data.flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(data.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.mappingType(), SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(data.mappingData().size(), 3);
    CORRADE_COMPARE(data.mappingData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(data.mappingData().data(), hiddenMappingData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldBitData().data(), hiddenFieldData + 1);
    CORRADE_COMPARE(data.fieldBitData().offset(), 5);
    CORRADE_COMPARE(data.fieldBitData().size(), (Containers::Size2D{3, 4}));
    CORRADE_COMPARE(data.fieldBitData().stride(), (Containers::Stride2D{8, 1}));

    /* 2D type-erased construction is not constexpr due to branching to select
       a corresponding SceneMappingType */
}

void SceneDataTest::constructFieldArrayOffsetOnly() {
    struct Data {
        Byte parent;
        UnsignedByte object;
        Int offset[2];
    } data[]{
        {0, 2, {0, 1}},
        {0, 15, {2, 3}},
        {0, 22, {4, 5}}
    };

    SceneFieldData a{sceneFieldCustom(34), 3, SceneMappingType::UnsignedByte, offsetof(Data, object), sizeof(Data), SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), 2, SceneFieldFlag::ImplicitMapping};
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(a.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.mappingType(), SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(a.mappingData(data).size(), 3);
    CORRADE_COMPARE(a.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(a.mappingData(data)),
        Containers::arrayView<UnsignedByte>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(a.fieldArraySize(), 2);
    CORRADE_COMPARE(a.fieldData(data).size(), 3);
    CORRADE_COMPARE(a.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS((Containers::arrayCast<2, const Int>(a.fieldData(data), 2).transposed<0, 1>())[0],
        Containers::arrayView({0, 2, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<2, const Int>(a.fieldData(data), 2).transposed<0, 1>())[1],
        Containers::arrayView({1, 3, 5}),
        TestSuite::Compare::Container);

    constexpr SceneFieldData ca{sceneFieldCustom(34), 3, SceneMappingType::UnsignedByte, offsetof(Data, object), sizeof(Data), SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), 2, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = ca.name();
    constexpr SceneFieldFlags flags = ca.flags();
    constexpr std::size_t size = ca.size();
    constexpr SceneMappingType mappingType = ca.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(34));
    CORRADE_COMPARE(flags, SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedByte);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(ca.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(ca.fieldArraySize(), 2);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(ca.mappingData(data).size(), 3);
    CORRADE_COMPARE(ca.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(ca.mappingData(data)),
        Containers::arrayView<UnsignedByte>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(ca.fieldData(data).size(), 3);
    CORRADE_COMPARE(ca.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS((Containers::arrayCast<2, const Int>(ca.fieldData(data), 2).transposed<0, 1>())[0],
        Containers::arrayView({0, 2, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<2, const Int>(ca.fieldData(data), 2).transposed<0, 1>())[1],
        Containers::arrayView({1, 3, 5}),
        TestSuite::Compare::Container);
}

void SceneDataTest::constructFieldArrayOffsetOnlyBit() {
    struct Data {
        Byte parent;
        UnsignedInt object;
        char yesNoInUpperBits;
    } data[]{
        {0, 2, '\x80'},
        {0, 15, '\x7f'},
        {0, 22, '\x80'}
    };

    SceneFieldData a{sceneFieldCustom(773), 3, SceneMappingType::UnsignedInt, offsetof(Data, object), sizeof(Data), offsetof(Data, yesNoInUpperBits), 6, sizeof(Data)*8, 2, SceneFieldFlag::OrderedMapping};
    CORRADE_COMPARE(a.flags(), SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(a.name(), sceneFieldCustom(773));
    CORRADE_COMPARE(a.size(), 3);
    CORRADE_COMPARE(a.mappingType(), SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(a.mappingData(data).size(), 3);
    CORRADE_COMPARE(a.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedInt>(a.mappingData(data)),
        Containers::arrayView<UnsignedInt>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(a.fieldArraySize(), 2);
    CORRADE_COMPARE(a.fieldBitData(data).offset(), 6);
    CORRADE_COMPARE(a.fieldBitData(data).size(), (Containers::Size2D{3, 2}));
    CORRADE_COMPARE(a.fieldBitData(data).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
    CORRADE_COMPARE_AS((a.fieldBitData(data).transposed<0, 1>())[0],
        Containers::stridedArrayView({false, true, false}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((a.fieldBitData(data).transposed<0, 1>())[1],
        Containers::stridedArrayView({true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);

    constexpr SceneFieldData ca{sceneFieldCustom(773), 3, SceneMappingType::UnsignedInt, offsetof(Data, object), sizeof(Data), offsetof(Data, yesNoInUpperBits), 6, sizeof(Data)*8, 2, SceneFieldFlag::OrderedMapping};
    constexpr SceneField name = ca.name();
    constexpr SceneFieldFlags flags = ca.flags();
    constexpr std::size_t size = ca.size();
    constexpr SceneMappingType mappingType = ca.mappingType();
    CORRADE_COMPARE(name, sceneFieldCustom(773));
    CORRADE_COMPARE(flags, SceneFieldFlag::OffsetOnly|SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(size, 3);
    CORRADE_COMPARE(mappingType, SceneMappingType::UnsignedInt);
    /* These are not marked constexpr because it'd work only partially, not for
       string fields (tested in constructFieldOffsetOnlyString()) */
    CORRADE_COMPARE(ca.fieldType(), SceneFieldType::Bit);
    CORRADE_COMPARE(ca.fieldArraySize(), 2);
    /* These are deliberately not constexpr to save header size a bit --
       compared to SceneField APIs they get used very little and it's mostly
       useless in a constexpr context anyway */
    CORRADE_COMPARE(ca.mappingData(data).size(), 3);
    CORRADE_COMPARE(ca.mappingData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedInt>(ca.mappingData(data)),
        Containers::arrayView<UnsignedInt>({2, 15, 22}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(ca.fieldBitData(data).offset(), 6);
    CORRADE_COMPARE(ca.fieldBitData(data).size(), (Containers::Size2D{3, 2}));
    CORRADE_COMPARE(ca.fieldBitData(data).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
    CORRADE_COMPARE_AS((ca.fieldBitData(data).transposed<0, 1>())[0],
        Containers::stridedArrayView({false, true, false}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((ca.fieldBitData(data).transposed<0, 1>())[1],
        Containers::stridedArrayView({true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);
}

void SceneDataTest::constructFieldInconsistentViewSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort mappingData[3]{};
    const Complexd rotationFieldData[2];
    const char hiddenFieldData[1]{};
    const char helloStringData[5]{};
    const UnsignedLong helloOffsetsData[2]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData)};
    SceneFieldData{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 2}};
    SceneFieldData{sceneFieldCustom(32), Containers::arrayView(mappingData), helloStringData, SceneFieldType::StringOffset64, Containers::arrayView(helloOffsetsData)};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: expected Trade::SceneField::Rotation mapping and field view to have the same size but got 3 and 2\n"
        "Trade::SceneFieldData: expected Trade::SceneField::Custom(773) mapping and field view to have the same size but got 3 and 2\n"
        "Trade::SceneFieldData: expected Trade::SceneField::Custom(32) mapping and field view to have the same size but got 3 and 2\n");
}

void SceneDataTest::constructFieldWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort rotationMappingData[3]{};
    const Quaternion rotationFieldData[3];

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Transformation, Containers::arrayView(rotationMappingData), Containers::arrayView(rotationFieldData)};
    SceneFieldData{SceneField::Transformation, 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), SceneFieldType::Quaternion, 0, sizeof(Quaternion)};
    /** @todo test also builtin bit and string fields with non-string types
        once there are any */
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n"
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n");
}

void SceneDataTest::constructFieldWrongTypeBit() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort hiddenMappingData[3]{};
    const bool hiddenFieldData[3]{};

    Containers::String out;
    Error redirectError{&out};
    /* Non-bit constructors with SceneFieldType::Bit. Only type-erased, 2D and
       offset-only construction, the regular and array constructor from typed
       views have SceneFieldType implicit. */
    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedShort, Containers::arrayView(hiddenMappingData), SceneFieldType::Bit, Containers::arrayView(hiddenFieldData)};
    SceneFieldData{sceneFieldCustom(773), Containers::arrayCast<2, const char>(Containers::arrayView(hiddenMappingData)), SceneFieldType::Bit, Containers::arrayCast<2, const char>(Containers::arrayView(hiddenFieldData))};
    SceneFieldData{sceneFieldCustom(773), 3, SceneMappingType::UnsignedShort, 0, 2, SceneFieldType::Bit, 0, 1};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: use a bit constructor for Trade::SceneFieldType::Bit\n"
        "Trade::SceneFieldData: use a bit constructor for Trade::SceneFieldType::Bit\n"
        "Trade::SceneFieldData: use a bit constructor for Trade::SceneFieldType::Bit\n");
}

void SceneDataTest::constructFieldWrongTypeString() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort mappingData[3]{};
    const Complexd rotationFieldData[3];
    const char helloStringData[5]{};
    const UnsignedLong helloFieldData[3]{};

    Containers::String out;
    Error redirectError{&out};
    /* Non-string constructors with a string SceneFieldType. Only type-erased,
       2D and offset-only construction, the regular and array constructor from
       typed views have SceneFieldType implicit. */
    SceneFieldData{sceneFieldCustom(32), SceneMappingType::UnsignedShort, Containers::arrayView(mappingData), SceneFieldType::StringOffset64, Containers::arrayView(helloFieldData)};
    SceneFieldData{sceneFieldCustom(32), Containers::arrayCast<2, const char>(Containers::arrayView(mappingData)), SceneFieldType::StringOffset64, Containers::arrayCast<2, const char>(Containers::arrayView(helloFieldData))};
    SceneFieldData{sceneFieldCustom(32), 3, SceneMappingType::UnsignedShort, 0, 2, SceneFieldType::StringRange16, 0, 8};

    /* String constructors with a non-string SceneFieldType. */
    SceneFieldData{SceneField::Rotation, SceneMappingType::UnsignedShort, Containers::arrayView(mappingData), nullptr, SceneFieldType::Complexd, Containers::arrayView(rotationFieldData)};
    SceneFieldData{SceneField::Rotation, Containers::arrayCast<2, const char>(Containers::arrayView(mappingData)), helloStringData, SceneFieldType::Complexd, Containers::arrayCast<2, const char>(Containers::arrayView(rotationFieldData))};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), helloStringData, SceneFieldType::Complexd, Containers::arrayView(rotationFieldData)};
    SceneFieldData{SceneField::Rotation, 3, SceneMappingType::UnsignedLong, 0, 8, 0, SceneFieldType::Quaternion, 0, 16};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: use a string constructor for Trade::SceneFieldType::StringOffset64\n"
        "Trade::SceneFieldData: use a string constructor for Trade::SceneFieldType::StringOffset64\n"
        "Trade::SceneFieldData: use a string constructor for Trade::SceneFieldType::StringRange16\n"

        "Trade::SceneFieldData: can't use a string constructor for Trade::SceneFieldType::Complexd\n"
        "Trade::SceneFieldData: can't use a string constructor for Trade::SceneFieldType::Complexd\n"
        "Trade::SceneFieldData: can't use a string constructor for Trade::SceneFieldType::Complexd\n"
        "Trade::SceneFieldData: can't use a string constructor for Trade::SceneFieldType::Quaternion\n");
}

void SceneDataTest::constructFieldTooLargeMappingStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];
    const char helloStringData[5]{};

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}, SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>(), SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 32767, SceneFieldType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 65536, -32768, SceneFieldType::UnsignedInt, 0, 4};

    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}, Containers::BitArrayView{enough, 0, 2}};
    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>(), Containers::BitArrayView{enough, 0, 2}};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 32767, 0, 0, 4};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 65536, -32768, 0, 0, 4};

    SceneFieldData{sceneFieldCustom(25), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}, helloStringData, SceneFieldType::StringOffset32, enough};
    SceneFieldData{sceneFieldCustom(25), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>(), helloStringData, SceneFieldType::StringOffset32, enough};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 32767, 0, SceneFieldType::StringOffset32, 0, 4};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 65536, -32768, 0, SceneFieldType::StringOffset32, 0, 4};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}, SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>(), SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 32768, SceneFieldType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 65538, -32769, SceneFieldType::UnsignedInt, 0, 4};

    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}, Containers::BitArrayView{enough, 0, 2}};
    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>(), Containers::BitArrayView{enough, 0, 2}};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 32768, 0, 0, 4};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 65538, -32769, 0, 0, 4};

    SceneFieldData{sceneFieldCustom(25), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}, helloStringData, SceneFieldType::StringOffset32, enough};
    SceneFieldData{sceneFieldCustom(25), SceneMappingType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>(), helloStringData, SceneFieldType::StringOffset32, enough};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 32768, 0, SceneFieldType::StringOffset32, 0, 4};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 65538, -32769, 0, SceneFieldType::StringOffset32, 0, 4};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n"

        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n"

        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got -32769\n");
}

void SceneDataTest::constructFieldTooLargeFieldStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];
    const char helloStringData[5]{};

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32767};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65536, -32768};

    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, enough, Containers::StridedBitArrayView1D{Containers::BitArrayView{toomuch}, 2, 32767}};
    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, enough, Containers::StridedBitArrayView1D{Containers::BitArrayView{toomuch}, 2, 32768}.flipped<0>()};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32767};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65536, -32768};

    SceneFieldData{sceneFieldCustom(35), SceneMappingType::UnsignedInt, enough, helloStringData, SceneFieldType::StringRangeNullTerminated32, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}};
    SceneFieldData{sceneFieldCustom(35), SceneMappingType::UnsignedInt, enough, helloStringData, SceneFieldType::StringRangeNullTerminated32, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>()};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 4, 0, SceneFieldType::StringRangeNullTerminated32, 0, 32767};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 4, 0, SceneFieldType::StringRangeNullTerminated32, 65536, -32768};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}};
    SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32768};
    SceneFieldData{SceneField::Mesh, 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65538, -32769};

    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, enough, Containers::StridedBitArrayView1D{Containers::BitArrayView{toomuch}, 2, 32768}};
    SceneFieldData{sceneFieldCustom(773), SceneMappingType::UnsignedInt, enough, Containers::StridedBitArrayView1D{Containers::BitArrayView{toomuch}, 2, 32769}.flipped<0>()};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32768};
    SceneFieldData{sceneFieldCustom(773), 2, SceneMappingType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65538, -32769};

    SceneFieldData{sceneFieldCustom(35), SceneMappingType::UnsignedInt, enough, helloStringData, SceneFieldType::StringRangeNullTerminated32, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}};
    SceneFieldData{sceneFieldCustom(35), SceneMappingType::UnsignedInt, enough, helloStringData, SceneFieldType::StringRangeNullTerminated32, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>()};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 4, 0, SceneFieldType::StringRangeNullTerminated32, 0, 32768};
    SceneFieldData{sceneFieldCustom(35), 2, SceneMappingType::UnsignedInt, 0, 4, 0, SceneFieldType::StringRangeNullTerminated32, 65538, -32769};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n"

        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n"

        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got -32769\n");
}

void SceneDataTest::constructFieldFlagNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort mappingData[3]{};
    const Quaternion rotationFieldData[3];
    const char hiddenFieldData[1]{};
    const char helloStringData[5]{};
    const UnsignedShort helloFieldData[3]{};

    /* These are fine */
    SceneFieldData{SceneField::Rotation, 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), SceneFieldType::Quaternion, 0, sizeof(Quaternion), SceneFieldFlag::OffsetOnly};
    SceneFieldData{SceneField::Mesh, Containers::arrayView(mappingData), Containers::arrayView(helloFieldData), SceneFieldFlag::MultiEntry};
    SceneFieldData{sceneFieldCustom(773), 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), 0, 0, sizeof(Quaternion), SceneFieldFlag::OffsetOnly|SceneFieldFlag::MultiEntry};
    SceneFieldData{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 3}, SceneFieldFlag::MultiEntry};
    SceneFieldData{sceneFieldCustom(24), Containers::arrayView(mappingData), helloStringData, SceneFieldType::StringOffset32, helloFieldData, SceneFieldFlag::NullTerminatedString|SceneFieldFlag::MultiEntry};
    SceneFieldData{sceneFieldCustom(24), 3, SceneMappingType::UnsignedShort, 0, 2, 0, SceneFieldType::StringOffset32, 0, 4, SceneFieldFlag::NullTerminatedString|SceneFieldFlag::MultiEntry};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData), SceneFieldFlag::OffsetOnly};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData), SceneFieldFlag::MultiEntry};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData), SceneFieldFlag::NullTerminatedString};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData), SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString|SceneFieldFlag::MultiEntry};
    SceneFieldData{SceneField::Rotation, 3, SceneMappingType::UnsignedShort, 0, 2, SceneFieldType::Quaternion, 0, 16, SceneFieldFlag::MultiEntry};
    SceneFieldData{SceneField::Rotation, 3, SceneMappingType::UnsignedShort, 0, 2, SceneFieldType::Quaternion, 0, 16, SceneFieldFlag::NullTerminatedString};
    SceneFieldData{SceneField::Rotation, 3, SceneMappingType::UnsignedShort, 0, 2, SceneFieldType::Quaternion, 0, 16, SceneFieldFlag::MultiEntry|SceneFieldFlag::NullTerminatedString};

    SceneFieldData{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 3}, SceneFieldFlag::OffsetOnly};
    SceneFieldData{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 3}, SceneFieldFlag::NullTerminatedString};
    SceneFieldData{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 3}, SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString};
    SceneFieldData{sceneFieldCustom(773), 3, SceneMappingType::UnsignedShort, 0, 2, 0, 0, 16, SceneFieldFlag::NullTerminatedString};

    SceneFieldData{sceneFieldCustom(24), Containers::arrayView(mappingData), helloStringData, SceneFieldType::StringOffset32, helloFieldData, SceneFieldFlag::OffsetOnly};
    CORRADE_COMPARE_AS(out,
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly for a Trade::SceneField::Rotation view of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::MultiEntry for a Trade::SceneField::Rotation view of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::NullTerminatedString for a Trade::SceneField::Rotation view of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly|Trade::SceneFieldFlag::MultiEntry|Trade::SceneFieldFlag::NullTerminatedString for a Trade::SceneField::Rotation view of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::MultiEntry for Trade::SceneField::Rotation of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::NullTerminatedString for Trade::SceneField::Rotation of Trade::SceneFieldType::Quaternion\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::MultiEntry|Trade::SceneFieldFlag::NullTerminatedString for Trade::SceneField::Rotation of Trade::SceneFieldType::Quaternion\n"

        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly for a Trade::SceneField::Custom(773) view of Trade::SceneFieldType::Bit\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::NullTerminatedString for a Trade::SceneField::Custom(773) view of Trade::SceneFieldType::Bit\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly|Trade::SceneFieldFlag::NullTerminatedString for a Trade::SceneField::Custom(773) view of Trade::SceneFieldType::Bit\n"
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::NullTerminatedString for Trade::SceneField::Custom(773) of Trade::SceneFieldType::Bit\n"

        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly for a view\n",
        TestSuite::Compare::String);
}

void SceneDataTest::constructFieldWrongOffsetOnlyDataAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort rotationMappingData[3]{};
    const Quaternion rotationFieldData[3];
    const char hello[5]{};

    SceneFieldData a{SceneField::Rotation, Containers::arrayView(rotationMappingData), Containers::arrayView(rotationFieldData)};
    SceneFieldData b{SceneField::Rotation, 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), SceneFieldType::Quaternion, 0, sizeof(Quaternion)};
    SceneFieldData c{sceneFieldCustom(773), 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), 0, 0, 1};
    SceneFieldData d{sceneFieldCustom(25), 3, SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort), 0, SceneFieldType::StringRange32, 0, 64};
    CORRADE_COMPARE(a.flags(), SceneFieldFlags{});
    CORRADE_COMPARE(b.flags(), SceneFieldFlag::OffsetOnly);
    CORRADE_COMPARE(c.flags(), SceneFieldFlag::OffsetOnly);
    CORRADE_COMPARE(d.flags(), SceneFieldFlag::OffsetOnly);

    a.mappingData(rotationMappingData); /* This is fine, no asserts */
    b.fieldData(rotationFieldData);
    c.fieldBitData(hello);
    d.stringData(hello);

    Containers::String out;
    Error redirectError{&out};
    b.mappingData();
    b.fieldData();
    d.fieldBitData();
    d.stringData();
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData::mappingData(): the field is offset-only, supply a data array\n"
        "Trade::SceneFieldData::fieldData(): the field is offset-only, supply a data array\n"
        "Trade::SceneFieldData::fieldBitData(): the field is offset-only, supply a data array\n"
        "Trade::SceneFieldData::stringData(): the field is offset-only, supply a data array\n");
}

void SceneDataTest::constructFieldWrongBitDataAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort mappingData[3]{};
    const char hiddenFieldData[3]{};
    const Quaternion rotationFieldData[3];

    SceneFieldData a{sceneFieldCustom(773), Containers::arrayView(mappingData), Containers::BitArrayView{hiddenFieldData, 0, 3}};
    SceneFieldData b{SceneField::Rotation, Containers::arrayView(mappingData), Containers::arrayView(rotationFieldData)};

    Containers::String out;
    Error redirectError{&out};
    a.fieldData();
    a.fieldData(hiddenFieldData);
    b.fieldBitData();
    b.fieldBitData(rotationFieldData);
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData::fieldData(): the field is Trade::SceneFieldType::Bit, use fieldBitData() instead\n"
        "Trade::SceneFieldData::fieldData(): the field is Trade::SceneFieldType::Bit, use fieldBitData() instead\n"
        "Trade::SceneFieldData::fieldBitData(): the field is Trade::SceneFieldType::Quaternion, not a bit\n"
        "Trade::SceneFieldData::fieldBitData(): the field is Trade::SceneFieldType::Quaternion, not a bit\n");
}

void SceneDataTest::constructFieldWrongStringDataAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const UnsignedShort rotationMappingData[3]{};
    const Quaternion rotationFieldData[3];

    SceneFieldData a{SceneField::Rotation, Containers::arrayView(rotationMappingData), Containers::arrayView(rotationFieldData)};

    Containers::String out;
    Error redirectError{&out};
    a.stringData();
    a.stringData(rotationFieldData);
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData::stringData(): the field is Trade::SceneFieldType::Quaternion, not a string\n"
        "Trade::SceneFieldData::stringData(): the field is Trade::SceneFieldType::Quaternion, not a string\n");
}

void SceneDataTest::constructFieldTypeErased2DWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char mappingData[5*sizeof(UnsignedInt)];
    char rotationFieldData[4*sizeof(Complex)];
    char hiddenFieldData[1]{};
    char helloStringData[3]{};
    char helloFieldData[4*sizeof(UnsignedShort)]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{mappingData, {4, 5}}.every(2),
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2)};
    SceneFieldData{sceneFieldCustom(773),
        Containers::StridedArrayView2D<char>{mappingData, {4, 5}}.every(2),
        Containers::StridedBitArrayView1D{Containers::BitArrayView{hiddenFieldData}, 4}.every(2)};
    SceneFieldData{sceneFieldCustom(773),
        Containers::StridedArrayView2D<char>{mappingData, {4, 5}}.every(2),
        Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData}, {4, 2}}.every(2)};
    SceneFieldData{sceneFieldCustom(32),
        Containers::StridedArrayView2D<char>{mappingData, {4, 5}}.every(2),
        helloStringData,
        SceneFieldType::StringOffset16,
        Containers::StridedArrayView2D<char>{helloFieldData, {4, sizeof(UnsignedShort)}}.every(2)};
    SceneFieldData{SceneField::Translation,
        Containers::StridedArrayView2D<char>{mappingData, {4, sizeof(UnsignedInt)}}.every(2),
        SceneFieldType::Vector3,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2)};
    /* All second field dimension sizes are fine for SceneFieldType::Bit */
    SceneFieldData{sceneFieldCustom(32),
        Containers::StridedArrayView2D<char>{mappingData, {4, sizeof(UnsignedInt)}}.every(2),
        helloStringData,
        SceneFieldType::StringRange16,
        Containers::StridedArrayView2D<char>{helloFieldData, {4, sizeof(UnsignedShort)}}.every(2)};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: expected second mapping view dimension size 1, 2, 4 or 8 but got 5\n"
        "Trade::SceneFieldData: expected second mapping view dimension size 1, 2, 4 or 8 but got 5\n"
        "Trade::SceneFieldData: expected second mapping view dimension size 1, 2, 4 or 8 but got 5\n"
        "Trade::SceneFieldData: expected second mapping view dimension size 1, 2, 4 or 8 but got 5\n"
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Vector3\n"
        "Trade::SceneFieldData: second field view dimension size 2 doesn't match Trade::SceneFieldType::StringRange16\n");
}

void SceneDataTest::constructFieldTypeErased2DNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char mappingData[8*sizeof(UnsignedInt)];
    char rotationFieldData[8*sizeof(Complex)];
    char hiddenFieldData[1]{};
    char helloStringData[3]{};
    char helloFieldData[8*sizeof(UnsignedShort)]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{mappingData, {4, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}};
    SceneFieldData{sceneFieldCustom(773),
        Containers::StridedArrayView2D<char>{mappingData, {4, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        Containers::StridedBitArrayView1D{Containers::BitArrayView{hiddenFieldData}, 4}};
    SceneFieldData{sceneFieldCustom(32),
        Containers::StridedArrayView2D<char>{mappingData, {4, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        helloStringData,
        SceneFieldType::StringOffset16,
        Containers::StridedArrayView2D<char>{helloFieldData, {4, sizeof(UnsignedShort)}}};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{mappingData, {4, sizeof(UnsignedInt)}},
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, 2*sizeof(Complex)}}.every({1, 2})};
    /* SceneFieldType::Bit has no type-erased 2D field case, the constructor
       taking a 2D bit array view is tested in
       constructFieldArrayNonContiguous() below */
    SceneFieldData{sceneFieldCustom(32),
        /* Just to cover the case of a 1-byte mapping type (lazy) */
        Containers::StridedArrayView2D<char>{mappingData, {4, sizeof(UnsignedByte)}, {4, 1}},
        helloStringData,
        SceneFieldType::StringOffset8,
        Containers::StridedArrayView2D<char>{helloFieldData, {4, 2*sizeof(UnsignedByte)}}.every({1, 2})};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: second mapping view dimension is not contiguous\n"
        "Trade::SceneFieldData: second mapping view dimension is not contiguous\n"
        "Trade::SceneFieldData: second mapping view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructFieldArrayNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedByte mappingData[3];
    Int offsetFieldData[3*4];
    char hiddenFieldData[2]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData data{sceneFieldCustom(34), Containers::arrayView(mappingData), Containers::StridedArrayView2D<Int>{offsetFieldData, {3, 4}}.every({1, 2})};
    SceneFieldData{sceneFieldCustom(773),
        Containers::arrayView(mappingData),
        Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData}, {3, 4}}.every({1, 2})};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: second field view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructFieldArrayNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort rotationMappingData[3]{};
    Quaternion rotationFieldData[3];
    Containers::ArrayView<UnsignedShort> rotationMapping = rotationMappingData;
    Containers::ArrayView<Quaternion> rotationField = rotationFieldData;
    Containers::StridedArrayView2D<Quaternion> rotationFields2D{rotationFieldData, {3, 3}, {0, sizeof(Quaternion)}};
    auto rotationMappingChar = Containers::arrayCast<2, const char>(rotationMapping);
    auto rotationField2DChar = Containers::arrayCast<2, const char>(rotationFields2D);

    /* This is all fine -- builtin fields can be created with the array
       constructors if the array size is set to 0, custom fields of any type
       can do anything */
    SceneFieldData{SceneField::Rotation,
        SceneMappingType::UnsignedShort, rotationMapping,
        SceneFieldType::Quaternion, rotationField, 0};
    SceneFieldData{SceneField::Rotation, 3,
        SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 0};
    SceneFieldData{sceneFieldCustom(37),
        rotationMapping,
        rotationFields2D};
    SceneFieldData{sceneFieldCustom(37),
        rotationMappingChar,
        SceneFieldType::Quaternion, rotationField2DChar, 3};
    SceneFieldData{sceneFieldCustom(37), 3,
        SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 3};

    /* This is not */
    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        SceneMappingType::UnsignedShort, rotationMapping,
        SceneFieldType::Quaternion, rotationField, 3};
    SceneFieldData{SceneField::Rotation, 3,
        SceneMappingType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 3};
    SceneFieldData{SceneField::Rotation,
        rotationMapping,
        rotationFields2D};
    SceneFieldData{SceneField::Rotation,
        rotationMappingChar,
        SceneFieldType::Quaternion, rotationField2DChar, 3};
    /* String fields can't be arrays, but for those the constructor doesn't
       even offer the array size; and constructing them with the regular
       constructor will fail as tested in constructFieldWrongTypeString() */
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n");
}

void SceneDataTest::constructFieldArrayTypeErased2DWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char rotationMappingData[4*sizeof(UnsignedInt)];
    char rotationFieldData[4*sizeof(Complex)];

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{rotationMappingData, {4, sizeof(UnsignedInt)}}.every(2),
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2), 3};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Int and field array size 3\n");
}

void SceneDataTest::constructFieldArrayTypeErased2DNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char mappingData[18*sizeof(UnsignedInt)];
    char offsetFieldData[18*sizeof(Int)];
    char hiddenFieldData[2]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{mappingData, {3, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetFieldData, {3, 3*sizeof(Int)}}, 3};
    SceneFieldData{sceneFieldCustom(773),
        Containers::StridedArrayView2D<char>{mappingData, {3, sizeof(UnsignedInt)}}.every({1, 2}),
        Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData}, {3, 4}}};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{mappingData, {3, sizeof(UnsignedInt)}},
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetFieldData, {3, 6*sizeof(Int)}}.every({1, 2}), 3};
    SceneFieldData{sceneFieldCustom(773),
        Containers::StridedArrayView2D<char>{mappingData, {3, sizeof(UnsignedInt)}},
        Containers::StridedBitArrayView2D{Containers::BitArrayView{hiddenFieldData}, {3, 4}}.every({1, 2})};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: second mapping view dimension is not contiguous\n"
        "Trade::SceneFieldData: second mapping view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructFieldBitTooLargeBitOffset() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    /* Only checked in the offset-only constructor, the StridedBitArrayView
       checks this on its own already. There it's a debug-only assert, be
       consistent and have it debug-only here as well. */

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(773), 3, SceneMappingType::UnsignedInt, 0, sizeof(UnsignedInt), 0, 8, 1};
    CORRADE_COMPARE(out, "Trade::SceneFieldData: bit offset expected to be smaller than 8, got 8\n");
}

void SceneDataTest::constructFieldBitTooLargeSize() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    /* Only checked in the offset-only constructor, the StridedBitArrayView
       checks this on its own already. There it's a debug-only assert, be
       consistent and have it debug-only here as well. */

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(773), std::size_t{1} << (sizeof(std::size_t)*8 - 3), SceneMappingType::UnsignedInt, 0, sizeof(UnsignedInt), 0, 0, 1};
    #ifndef CORRADE_TARGET_32BIT
    CORRADE_COMPARE(out, "Trade::SceneFieldData: size expected to be smaller than 2^61 bits, got 2305843009213693952\n");
    #else
    CORRADE_COMPARE(out, "Trade::SceneFieldData: size expected to be smaller than 2^29 bits, got 536870912\n");
    #endif
}

#ifndef CORRADE_TARGET_32BIT
void SceneDataTest::constructFieldStringDataTooFarApart() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedShort mappingData[3]{};

    Containers::String out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(166),
        Containers::arrayView(mappingData),
        reinterpret_cast<const char*>(0xfeedbeefull), SceneFieldType::StringOffset8,
        Containers::arrayView(reinterpret_cast<const UnsignedByte*>(0x8000feedbeefull), 3)};
    SceneFieldData{sceneFieldCustom(166),
        Containers::arrayView(mappingData),
        reinterpret_cast<const char*>(0x8000feedbeefull), SceneFieldType::StringOffset8,
        Containers::arrayView(reinterpret_cast<const UnsignedByte*>(0xfeedbeefull), 3)};
    SceneFieldData{sceneFieldCustom(661), 3,
        SceneMappingType::UnsignedShort, 1725676, 2,
        0x800000000000ull, SceneFieldType::StringOffset8, 72567654, 8};
    CORRADE_COMPARE(out,
        "Trade::SceneFieldData: (signed) distance between string data and field data expected to fit into 48 bits but got 0xfeedbeef and 0x8000feedbeef\n"
        "Trade::SceneFieldData: (signed) distance between string data and field data expected to fit into 48 bits but got 0x8000feedbeef and 0xfeedbeef\n"
        "Trade::SceneFieldData: expected string data offset to fit into 48 bits but got 140737488355328\n");
}
#endif

void SceneDataTest::construct() {
    struct TransformParent {
        UnsignedShort object;
        Matrix4 transformation;
        Int parent;
    };

    Containers::StridedArrayView1D<TransformParent> transformsParentFieldMappingData;
    Containers::StridedArrayView1D<UnsignedByte> meshFieldData;
    Containers::StridedArrayView1D<Vector2> radiusFieldData;
    Containers::StridedArrayView1D<UnsignedShort> materialMeshRadiusMappingData;
    Containers::ArrayTuple data{
        {NoInit, 5, transformsParentFieldMappingData},
        {NoInit, 2, meshFieldData},
        {NoInit, 2, radiusFieldData},
        {NoInit, 2, materialMeshRadiusMappingData},
    };

    Utility::copy({
        {4, Matrix4::translation(Vector3::xAxis(5.0f)), -1},
        {2, Matrix4::translation(Vector3::yAxis(5.0f)), 0},
        {3, Matrix4::translation(Vector3::zAxis(5.0f)), 2},
        {0, Matrix4::translation(Vector3::yScale(5.0f)), 1},
        {1, Matrix4::translation(Vector3::zScale(5.0f)), -1},
    }, transformsParentFieldMappingData);
    Utility::copy({5, 7}, meshFieldData);
    Utility::copy({{37.5f, 1.5f}, {22.5f, 0.5f}}, radiusFieldData);
    Utility::copy({2, 6}, materialMeshRadiusMappingData);

    int importerState;
    SceneFieldData transformations{SceneField::Transformation,
        transformsParentFieldMappingData.slice(&TransformParent::object),
        transformsParentFieldMappingData.slice(&TransformParent::transformation)};
    /* Offset-only */
    SceneFieldData parents{SceneField::Parent, 5,
        SceneMappingType::UnsignedShort, offsetof(TransformParent, object), sizeof(TransformParent),
        SceneFieldType::Int, offsetof(TransformParent, parent), sizeof(TransformParent)};
    SceneFieldData meshes{SceneField::Mesh,
        materialMeshRadiusMappingData,
        meshFieldData, SceneFieldFlag::OrderedMapping};
    /* Custom & array */
    SceneFieldData radiuses{sceneFieldCustom(37),
        materialMeshRadiusMappingData,
        Containers::arrayCast<2, Float>(radiusFieldData), SceneFieldFlag::OrderedMapping};
    SceneData scene{SceneMappingType::UnsignedShort, 8, Utility::move(data), {
        transformations, parents, meshes, radiuses
    }, &importerState};

    /* Basics */
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!scene.fieldData().isEmpty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), transformsParentFieldMappingData.data());
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), transformsParentFieldMappingData.data());
    CORRADE_COMPARE(scene.mappingBound(), 8);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 4);
    CORRADE_COMPARE(scene.fieldSizeBound(), 5);
    CORRADE_COMPARE(scene.importerState(), &importerState);

    /* is2D() / is3D() exhaustively tested in transformations*DAsArray[TRS]()
       and constructZeroFields() */

    /* transformationFieldSize() exhaustively tested in
       transformations*DAsArray[TRS]() */

    /* Field property access by ID */
    CORRADE_COMPARE(scene.fieldName(0), SceneField::Transformation);
    CORRADE_COMPARE(scene.fieldName(1), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldName(2), SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldName(3), sceneFieldCustom(37));
    CORRADE_COMPARE(scene.fieldFlags(0), SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldFlags(1), SceneFieldFlag::OffsetOnly);
    CORRADE_COMPARE(scene.fieldFlags(2), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldFlags(3), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldType(0), SceneFieldType::Matrix4x4);
    CORRADE_COMPARE(scene.fieldType(1), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldType(2), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldType(3), SceneFieldType::Float);
    CORRADE_COMPARE(scene.fieldSize(0), 5);
    CORRADE_COMPARE(scene.fieldSize(1), 5);
    CORRADE_COMPARE(scene.fieldSize(2), 2);
    CORRADE_COMPARE(scene.fieldSize(3), 2);
    CORRADE_COMPARE(scene.fieldArraySize(0), 0);
    CORRADE_COMPARE(scene.fieldArraySize(1), 0);
    CORRADE_COMPARE(scene.fieldArraySize(2), 0);
    CORRADE_COMPARE(scene.fieldArraySize(3), 2);

    /* Raw field data access by ID */
    CORRADE_COMPARE(scene.fieldData(2).name(), SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldData(2).size(), 2);
    CORRADE_COMPARE(scene.fieldData(2).mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(2).mappingData())[1], 6);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedByte>(scene.fieldData(2).fieldData())[1], 7);
    CORRADE_COMPARE(scene.fieldData(2).flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldData(2).fieldType(), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldData(2).fieldArraySize(), 0);
    /* Offset-only */
    CORRADE_COMPARE(scene.fieldData(1).name(), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldData(1).size(), 5);
    CORRADE_COMPARE(scene.fieldData(1).mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldData(1).flags(), SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldData(1).fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldData(1).fieldArraySize(), 0);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(1).mappingData())[4], 1);
    CORRADE_COMPARE(Containers::arrayCast<const Int>(scene.fieldData(1).fieldData())[4], -1);
    /* Array */
    CORRADE_COMPARE(scene.fieldData(3).name(), sceneFieldCustom(37));
    CORRADE_COMPARE(scene.fieldData(3).size(), 2);
    CORRADE_COMPARE(scene.fieldData(3).mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldData(3).flags(), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldData(3).fieldType(), SceneFieldType::Float);
    CORRADE_COMPARE(scene.fieldData(3).fieldArraySize(), 2);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(3).mappingData())[0], 2);
    CORRADE_COMPARE(Containers::arrayCast<const Vector2>(scene.fieldData(3).fieldData())[0], (Vector2{37.5f, 1.5f}));

    /* Typeless mapping access by ID with a cast later */
    CORRADE_COMPARE(scene.mapping(0).size()[0], 5);
    CORRADE_COMPARE(scene.mapping(1).size()[0], 5);
    CORRADE_COMPARE(scene.mapping(2).size()[0], 2);
    CORRADE_COMPARE(scene.mapping(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableMapping(0).size()[0], 5);
    CORRADE_COMPARE(scene.mutableMapping(1).size()[0], 5);
    CORRADE_COMPARE(scene.mutableMapping(2).size()[0], 2);
    CORRADE_COMPARE(scene.mutableMapping(3).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(0))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(1))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(3))[0]), 2);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(0))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(1))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(3))[0]), 2);

    /* Typeless field access by ID with a cast later */
    CORRADE_COMPARE(scene.field(0).size()[0], 5);
    CORRADE_COMPARE(scene.field(1).size()[0], 5);
    CORRADE_COMPARE(scene.field(2).size()[0], 2);
    CORRADE_COMPARE(scene.field(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField(0).size()[0], 5);
    CORRADE_COMPARE(scene.mutableField(1).size()[0], 5);
    CORRADE_COMPARE(scene.mutableField(2).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField(3).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const Matrix4>(scene.field(0))[2]), Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE((Containers::arrayCast<1, const Int>(scene.field(1))[4]), -1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedByte>(scene.field(2))[1]), 7);
    CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(scene.field(3))[0]), (Vector2{37.5f, 1.5f}));
    CORRADE_COMPARE((Containers::arrayCast<1, Matrix4>(scene.mutableField(0))[2]), Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE((Containers::arrayCast<1, Int>(scene.mutableField(1))[4]), -1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedByte>(scene.mutableField(2))[1]), 7);
    CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(scene.mutableField(3))[0]), (Vector2{37.5f, 1.5f}));

    /* Typed mapping access by ID */
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(0).size(), 5);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(1).size(), 5);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(2).size(), 2);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(3).size(), 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(0).size(), 5);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(1).size(), 5);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(2).size(), 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(3).size(), 2);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(0)[2], 3);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(1)[4], 1);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(2)[1], 6);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(3)[0], 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(0)[2], 3);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(1)[4], 1);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(2)[1], 6);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(3)[0], 2);

    /* Typed field access by ID */
    CORRADE_COMPARE(scene.field<Matrix4>(0).size(), 5);
    CORRADE_COMPARE(scene.field<Int>(1).size(), 5);
    CORRADE_COMPARE(scene.field<UnsignedByte>(2).size(), 2);
    CORRADE_COMPARE(scene.field<Float[]>(3).size(), (Containers::Size2D{2, 2}));
    CORRADE_COMPARE(scene.mutableField<Matrix4>(0).size(), 5);
    CORRADE_COMPARE(scene.mutableField<Int>(1).size(), 5);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(2).size(), 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(3).size(), (Containers::Size2D{2, 2}));
    CORRADE_COMPARE(scene.field<Matrix4>(0)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.field<Int>(1)[4], -1);
    CORRADE_COMPARE(scene.field<UnsignedByte>(2)[1], 7);
    CORRADE_COMPARE_AS(scene.field<Float[]>(3)[0],
        Containers::arrayView({37.5f, 1.5f}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(0)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.mutableField<Int>(1)[4], -1);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(2)[1], 7);
    CORRADE_COMPARE_AS(scene.mutableField<Float[]>(3)[0],
        Containers::stridedArrayView({37.5f, 1.5f}),
        TestSuite::Compare::Container);

    /* Accessing a non-array field as an array should be possible as well --
       the second dimension is then just 1 */
    CORRADE_COMPARE(scene.field<UnsignedByte[]>(2).size(), (Containers::Size2D{2, 1}));
    CORRADE_COMPARE(scene.mutableField<UnsignedByte[]>(2).size(), (Containers::Size2D{2, 1}));
    CORRADE_COMPARE(scene.field<UnsignedByte[]>(2)[1][0], 7);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte[]>(2)[1][0], 7);

    /* Field property access by name */
    CORRADE_COMPARE(scene.fieldFlags(SceneField::Transformation), SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldFlags(SceneField::Parent), SceneFieldFlag::OffsetOnly);
    CORRADE_COMPARE(scene.fieldFlags(SceneField::Mesh), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldFlags(sceneFieldCustom(37)), SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldType(SceneField::Transformation), SceneFieldType::Matrix4x4);
    CORRADE_COMPARE(scene.fieldType(SceneField::Parent), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldType(SceneField::Mesh), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldType(sceneFieldCustom(37)), SceneFieldType::Float);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Transformation), 5);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Parent), 5);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Mesh), 2);
    CORRADE_COMPARE(scene.fieldSize(sceneFieldCustom(37)), 2);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Transformation), 0);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Parent), 0);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Mesh), 0);
    CORRADE_COMPARE(scene.fieldArraySize(sceneFieldCustom(37)), 2);

    /* Typeless mapping access by name with a cast later */
    CORRADE_COMPARE(scene.mapping(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.mapping(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.mapping(2).size()[0], 2);
    CORRADE_COMPARE(scene.mapping(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableMapping(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.mutableMapping(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.mutableMapping(2).size()[0], 2);
    CORRADE_COMPARE(scene.mutableMapping(3).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(SceneField::Transformation))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(SceneField::Parent))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.mapping(3))[0]), 2);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(SceneField::Transformation))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(SceneField::Parent))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableMapping(3))[0]), 2);

    /* Typeless field access by name with a cast later */
    CORRADE_COMPARE(scene.field(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.field(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.field(SceneField::Mesh).size()[0], 2);
    CORRADE_COMPARE(scene.field(sceneFieldCustom(37)).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.mutableField(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.mutableField(SceneField::Mesh).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField(sceneFieldCustom(37)).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const Matrix4>(scene.field(SceneField::Transformation))[2]), Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE((Containers::arrayCast<1, const Int>(scene.field(SceneField::Parent))[4]), -1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedByte>(scene.field(SceneField::Mesh))[1]), 7);
    CORRADE_COMPARE((Containers::arrayCast<1, const Vector2>(scene.field(sceneFieldCustom(37)))[0]), (Vector2{37.5f, 1.5f}));
    CORRADE_COMPARE((Containers::arrayCast<1, Matrix4>(scene.mutableField(SceneField::Transformation))[2]), Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE((Containers::arrayCast<1, Int>(scene.mutableField(SceneField::Parent))[4]), -1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedByte>(scene.mutableField(SceneField::Mesh))[1]), 7);
    CORRADE_COMPARE((Containers::arrayCast<1, Vector2>(scene.mutableField(sceneFieldCustom(37)))[0]), (Vector2{37.5f, 1.5f}));

    /* Typed mapping access by name */
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(sceneFieldCustom(37)).size(), 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(sceneFieldCustom(37)).size(), 2);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Transformation)[2], 3);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Parent)[4], 1);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(SceneField::Mesh)[1], 6);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(sceneFieldCustom(37))[0], 2);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Transformation)[2], 3);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Parent)[4], 1);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(SceneField::Mesh)[1], 6);
    CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(sceneFieldCustom(37))[0], 2);

    /* Typed field access by name */
    CORRADE_COMPARE(scene.field<Matrix4>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.field<Int>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.field<UnsignedByte>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.field<Float[]>(sceneFieldCustom(37)).size(), (Containers::Size2D{2, 2}));
    CORRADE_COMPARE(scene.mutableField<Matrix4>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.mutableField<Int>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(sceneFieldCustom(37)).size(), (Containers::Size2D{2, 2}));
    CORRADE_COMPARE(scene.field<Matrix4>(SceneField::Transformation)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.field<Int>(SceneField::Parent)[4], -1);
    CORRADE_COMPARE(scene.field<UnsignedByte>(SceneField::Mesh)[1], 7);
    CORRADE_COMPARE_AS(scene.field<Float[]>(sceneFieldCustom(37))[0],
        Containers::arrayView({37.5f, 1.5f}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(SceneField::Transformation)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.mutableField<Int>(SceneField::Parent)[4], -1);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(SceneField::Mesh)[1], 7);
    CORRADE_COMPARE_AS(scene.mutableField<Float[]>(sceneFieldCustom(37))[0],
        Containers::stridedArrayView({37.5f, 1.5f}),
        TestSuite::Compare::Container);

    /* Accessing a non-array field as an array should be possible as well --
       the second dimension is then just 1 */
    CORRADE_COMPARE(scene.field<UnsignedByte[]>(SceneField::Mesh).size(), (Containers::Size2D{2, 1}));
    CORRADE_COMPARE(scene.mutableField<UnsignedByte[]>(SceneField::Mesh).size(), (Containers::Size2D{2, 1}));
    CORRADE_COMPARE(scene.field<UnsignedByte[]>(SceneField::Mesh)[1][0], 7);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte[]>(SceneField::Mesh)[1][0], 7);
}

void SceneDataTest::constructZeroFields() {
    /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    int importerState{};
    SceneData scene{SceneMappingType::UnsignedShort, 37563, nullptr, {}, &importerState};
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(scene.fieldData().isEmpty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), nullptr);
    CORRADE_COMPARE(scene.importerState(), &importerState);
    CORRADE_COMPARE(scene.mappingBound(), 37563);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 0);
    CORRADE_COMPARE(scene.fieldSizeBound(), 0);
    CORRADE_VERIFY(!scene.is2D());
    CORRADE_VERIFY(!scene.is3D());
}

void SceneDataTest::constructZeroObjects() {
    /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    int importerState{};
    SceneFieldData meshes{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr};
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {meshes, materials}, &importerState};
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!scene.fieldData().isEmpty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), nullptr);
    CORRADE_COMPARE(scene.importerState(), &importerState);
    CORRADE_COMPARE(scene.mappingBound(), 0);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.fieldCount(), 2);
    CORRADE_COMPARE(scene.fieldSizeBound(), 0);

    /* Field property access by name */
    CORRADE_COMPARE(scene.fieldType(SceneField::Mesh), SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldType(SceneField::MeshMaterial), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Mesh), 0);
    CORRADE_COMPARE(scene.fieldSize(SceneField::MeshMaterial), 0);
    CORRADE_COMPARE(scene.mapping(SceneField::Mesh).data(), nullptr);
    CORRADE_COMPARE(scene.mapping(SceneField::MeshMaterial).data(), nullptr);
}

void SceneDataTest::constructNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    struct Data {
        UnsignedShort object;
        UnsignedByte mesh;
    } data[]{
        {0, 2},
        {1, 1},
        {2, 0}
    };

    int importerState;
    SceneFieldData mesh{SceneField::Mesh,
        Containers::stridedArrayView(data).slice(&Data::object),
        Containers::stridedArrayView(data).slice(&Data::mesh)};
    SceneData scene{SceneMappingType::UnsignedShort, 7, instanceData.dataFlags, Containers::arrayView(data), {mesh}, &importerState};

    CORRADE_COMPARE(scene.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), +data);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), static_cast<void*>(data));
    CORRADE_COMPARE(scene.mappingBound(), 7);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 1);
    CORRADE_COMPARE(scene.importerState(), &importerState);

    CORRADE_COMPARE(scene.mapping<UnsignedShort>(0).size(), 3);
    CORRADE_COMPARE(scene.mapping<UnsignedShort>(0)[2], 2);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(scene.mutableMapping<UnsignedShort>(0)[2], 2);

    CORRADE_COMPARE(scene.field<UnsignedByte>(0).size(), 3);
    CORRADE_COMPARE(scene.field<UnsignedByte>(0)[2], 0);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(scene.mutableField<UnsignedByte>(0)[2], 0);
}

void SceneDataTest::constructBit() {
    struct Data {
        UnsignedByte object;
        /* Bit 3 is one field, 5-7 the other; both are then included as both
           direct and offset-only fields, the array then also with a negative
           stride */
        UnsignedByte bits;
    } data[]{
        {4, 0x01 << 3 | 0x02 << 5},
        {0, 0x01 << 3 | 0x02 << 5},
        {9, 0x00 << 3 | 0x05 << 5},
        {2, 0x01 << 3 | 0x07 << 5},
    };
    auto view = Containers::stridedArrayView(data);

    constexpr SceneField bitField = sceneFieldCustom(0);
    constexpr SceneField arrayField = sceneFieldCustom(1);
    constexpr SceneField bitFieldOffsetOnly = sceneFieldCustom(10);
    constexpr SceneField arrayFieldOffsetOnly = sceneFieldCustom(11);

    SceneData scene{SceneMappingType::UnsignedByte, 10, DataFlag::Mutable, data, {
        SceneFieldData{bitField, view.slice(&Data::object),
            Containers::StridedBitArrayView1D{Containers::BitArrayView{data}, &data[0].bits, 3, 4, sizeof(Data)*8}, SceneFieldFlag::ImplicitMapping},
        SceneFieldData{arrayField, view.slice(&Data::object),
            Containers::StridedBitArrayView2D{Containers::BitArrayView{data}, &data[0].bits, 5, {4, 3}, {sizeof(Data)*8, 1}}, SceneFieldFlag::OrderedMapping},
        /* The two above, just as offset-only */
        SceneFieldData{bitFieldOffsetOnly, 4,
            SceneMappingType::UnsignedByte, 0, sizeof(Data),
            offsetof(Data, bits), 3, sizeof(Data)*8, SceneFieldFlag::ImplicitMapping},
        SceneFieldData{arrayFieldOffsetOnly, 4,
            SceneMappingType::UnsignedByte, 0, sizeof(Data),
            offsetof(Data, bits), 5, sizeof(Data)*8, 3, SceneFieldFlag::OrderedMapping},
    }};

    /* Raw field data access has special handling for bits, as offset-only
       fields are always converted to real views */
    for(UnsignedInt i: {0, 2}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldData(i).flags(), SceneFieldFlag::ImplicitMapping);
        CORRADE_COMPARE(scene.fieldData(i).mappingType(), SceneMappingType::UnsignedByte);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().data(), &data[0].object);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().size(), 4);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().stride(), sizeof(Data));
        CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(scene.fieldData(i).mappingData()),
            Containers::arrayView<UnsignedByte>({4, 0, 9, 2}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE(scene.fieldData(i).fieldType(), SceneFieldType::Bit);
        CORRADE_COMPARE(scene.fieldData(i).fieldArraySize(), 0);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().data(), &data[0].bits);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().offset(), 3);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        CORRADE_COMPARE_AS((scene.fieldData(i).fieldBitData().transposed<0, 1>())[0], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }
    for(UnsignedInt i: {1, 3}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldData(i).flags(), SceneFieldFlag::OrderedMapping);
        CORRADE_COMPARE(scene.fieldData(i).mappingType(), SceneMappingType::UnsignedByte);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().data(), &data[0].object);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().size(), 4);
        CORRADE_COMPARE(scene.fieldData(i).mappingData().stride(), sizeof(Data));
        CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(scene.fieldData(i).mappingData()),
            Containers::arrayView<UnsignedByte>({4, 0, 9, 2}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE(scene.fieldData(i).fieldType(), SceneFieldType::Bit);
        CORRADE_COMPARE(scene.fieldData(i).fieldArraySize(), 3);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().data(), &data[0].bits);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().offset(), 5);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        /* Testing just the first slice, should be enough. The whole data is
           tested properly below */
        CORRADE_COMPARE_AS((scene.fieldData(i).fieldBitData().transposed<0, 1>())[0], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }

    /* Field propery access -- nothing special is done for bits, so just verify
       roughly that the calls work */
    CORRADE_COMPARE(scene.fieldFlags(bitFieldOffsetOnly), SceneFieldFlag::OffsetOnly|SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(scene.fieldType(arrayField), SceneFieldType::Bit);
    CORRADE_COMPARE(scene.fieldArraySize(arrayFieldOffsetOnly), 3);

    /* Single-bit field access using an ID and a name. The view and offset-only
       variants should give the same results, const and mutable variant as
       well, the array variant should working here too. */
    for(UnsignedInt i: {0, 2}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBits(i).size(), 4);
        CORRADE_COMPARE(scene.fieldBits(i).stride(), sizeof(Data)*8);
        CORRADE_COMPARE_AS(scene.fieldBits(i), Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBits(i).size(), 4);
        CORRADE_COMPARE(scene.mutableFieldBits(i).stride(), sizeof(Data)*8);
        CORRADE_COMPARE_AS(scene.mutableFieldBits(i), Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.fieldBitArrays(i).size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.fieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }
    for(SceneField i: {bitField, bitFieldOffsetOnly}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBits(i).size(), 4);
        CORRADE_COMPARE(scene.fieldBits(i).stride(), sizeof(Data)*8);
        CORRADE_COMPARE_AS(scene.fieldBits(i), Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBits(i).size(), 4);
        CORRADE_COMPARE(scene.mutableFieldBits(i).stride(), sizeof(Data)*8);
        CORRADE_COMPARE_AS(scene.mutableFieldBits(i), Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.fieldBitArrays(i).size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.fieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }

    /* Bit array field access using ID and name. The view and offset-only
       variants should give the same results, const and mutable variant as
       well. */
    for(UnsignedInt i: {1, 3}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.fieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        /* Taking all first array items, all second etc, because that's less
           comparisons than comparing 3 bits 4 times */
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[1], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[2], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        /* Taking all first array items, all second etc, because that's less
           comparisons than comparing 3 bits 4 times */
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[1], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[2], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }
    for(SceneField i: {arrayField, arrayFieldOffsetOnly}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.fieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        /* Taking all first array items, all second etc, because that's less
           comparisons than comparing 3 bits 4 times */
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[1], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[2], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).stride(), (Containers::Stride2D{sizeof(Data)*8, 1}));
        /* Taking all first array items, all second etc, because that's less
           comparisons than comparing 3 bits 4 times */
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[0], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[1], Containers::stridedArrayView({
            true, true, false, true
        }).sliceBit(0), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[2], Containers::stridedArrayView({
            false, false, true, true
        }).sliceBit(0), TestSuite::Compare::Container);
    }
}

template<class T> struct StringFieldTraits;
template<> struct StringFieldTraits<UnsignedByte> {
    static const char* name() { return "8"; }
    static SceneFieldType offsetType() { return SceneFieldType::StringOffset8; }
    static SceneFieldType rangeType() { return SceneFieldType::StringRange8; }
    static SceneFieldType rangeNullTerminatedType() {
        return SceneFieldType::StringRangeNullTerminated8;
    }
};
template<> struct StringFieldTraits<UnsignedShort> {
    static const char* name() { return "16"; }
    static SceneFieldType offsetType() { return SceneFieldType::StringOffset16; }
    static SceneFieldType rangeType() { return SceneFieldType::StringRange16; }
    static SceneFieldType rangeNullTerminatedType() {
        return SceneFieldType::StringRangeNullTerminated16;
    }
};
template<> struct StringFieldTraits<UnsignedInt> {
    static const char* name() { return "32"; }
    static SceneFieldType offsetType() { return SceneFieldType::StringOffset32; }
    static SceneFieldType rangeType() { return SceneFieldType::StringRange32; }
    static SceneFieldType rangeNullTerminatedType() {
        return SceneFieldType::StringRangeNullTerminated32;
    }
};
template<> struct StringFieldTraits<UnsignedLong> {
    static const char* name() { return "64"; }
    static SceneFieldType offsetType() { return SceneFieldType::StringOffset64; }
    static SceneFieldType rangeType() { return SceneFieldType::StringRange64; }
    static SceneFieldType rangeNullTerminatedType() {
        return SceneFieldType::StringRangeNullTerminated64;
    }
};

template<class T> void SceneDataTest::constructString() {
    setTestCaseTemplateName(StringFieldTraits<T>::name());

    /* Assumption is that these will be populated by some helper in SceneTools,
       with offsets/sizes remembered. Things tested:

        -   names are offsets w/o null termination, thus have to be referenced
            only once and in the order they are in the string
        -   keys are null-terminated ranges, thus can be referenced multiple
            times, the only complication is that there's an extra \0 after
            which doesn't count into the size
        -   values are non-null-terminated (offset, size) ranges, nothing
            special about these, can be also referenced multiple times
            including subsets (such as "brown" out of "lightbrown"
        -   files are offsets w/ null terminated, thus again have to be
            referenced only once an in the order they are in the string, the
            implicit size has to exclude the null terminator
        -   keys are null terminated (offset, implicit size) ranges, are
            defined at the end of the string in order to verify a null
            terminator gets added by ArrayTuple after, compared to plain
            offsets the reference order doesn't matter and can be referenced
            multiple times */

    Containers::StringView namesKeysValues =
        "Chair"         /* 5 */
        "Lampshade"     /* 14 */
        "color\0"       /* 14, 5 */
        "age\0"         /* 20, 3 */
        "lightbrown"    /* 24, 10; 29, 5 */
        "old"           /* 34, 3 */
        "new"_s;        /* 37, 3 doesn't assume null termination */
    CORRADE_COMPARE(namesKeysValues.size(), 37 + 3);

    Containers::StringView filesTags =
        "chair.glb\0"       /* 0 */
        "empty.obj\0"       /* 10 */
        "lampshade.fbx\0"   /* 20 */
        "MAPPABLE\0"        /* 34 */
        "STRANGE"_s;        /* 43, assumes it's stored null-terminated */
    CORRADE_COMPARE(filesTags.size(), 43 + 7);

    struct Name {
        UnsignedShort object;
        T nameOffset;
    };

    struct KeyValue {
        UnsignedShort object;
        Containers::Pair<T, T> keyRangeNullTerminated;
        Containers::Pair<T, T> valueRange;
    };

    struct FileTag {
        UnsignedShort object;
        T fileOffsetNullTerminated;
        T tagRangeNullTerminated;
    };

    Containers::StridedArrayView1D<Name> nameData;
    Containers::StridedArrayView1D<KeyValue> keyValueData;
    Containers::MutableStringView nameKeyValueStringData;
    Containers::StridedArrayView1D<FileTag> fileTagData;
    Containers::MutableStringView fileTagStringData;
    Containers::ArrayTuple data{
        {NoInit, 2, nameData},
        {NoInit, 4, keyValueData},
        {NoInit, filesTags.size(), fileTagStringData, Containers::StringViewFlag::NullTerminated},
        {NoInit, 3, fileTagData},
        {NoInit, namesKeysValues.size(), nameKeyValueStringData}
    };

    /* The offset has to be monotonically increasing, so the view is flipped in
       the SceneFieldData */
    Utility::copy({
        {3, 14}, /* Chair */
        {1, 5}   /* Lampshade */
    }, nameData);

    Utility::copy({
        {3, {20, 3}, {37, 3}},  /* age=new */
        {3, {14, 5}, {24, 10}}, /* color=lightbrown */
        {1, {20, 3}, {34, 3}},  /* age=old */
        {1, {14, 5}, {29, 5}}   /* color=brown */
    }, keyValueData);

    Utility::copy({
        {1, 10, 34},    /* chair.glb, MAPPABLE */
        {2, 20, 43},    /* empty.obj, STRANGE */
        {3, 34, 43},    /* lampshade.fbx, STRANGE */
    }, fileTagData);

    Utility::copy(namesKeysValues, nameKeyValueStringData);
    Utility::copy(filesTags, fileTagStringData);

    const SceneField nameField = sceneFieldCustom(5);
    const SceneField keyField = sceneFieldCustom(6);
    const SceneField valueField = sceneFieldCustom(7);
    const SceneField fileField = sceneFieldCustom(8);
    const SceneField tagField = sceneFieldCustom(9);

    /* Calculate offsets for the offset-only field before the data is moved
       out */
    const std::size_t keyValueDataOffset = static_cast<char*>(keyValueData.data()) - data.data();
    const std::size_t nameKeyValueStringDataOffset = nameKeyValueStringData.data() - data.data();

    /* The fileTagStringData should be before the file/tag field data in order
       to test storing negative string data offset */
    CORRADE_VERIFY(fileTagStringData.data() < fileTagData.data());

    SceneData scene{SceneMappingType::UnsignedShort, 4, Utility::move(data), {
        /* Has a negative stride */
        SceneFieldData{nameField, nameData.slice(&Name::object).template flipped<0>(),
            nameKeyValueStringData.data(), StringFieldTraits<T>::offsetType(),
            nameData.slice(&Name::nameOffset).template flipped<0>()},
        SceneFieldData{keyField, keyValueData.slice(&KeyValue::object),
            nameKeyValueStringData.data(), StringFieldTraits<T>::rangeType(),
            keyValueData.slice(&KeyValue::keyRangeNullTerminated),
            SceneFieldFlag::NullTerminatedString},
        /* Offset-only */
        SceneFieldData{valueField, 4,
            SceneMappingType::UnsignedShort, keyValueDataOffset + offsetof(KeyValue, object), sizeof(KeyValue),
            nameKeyValueStringDataOffset,
            StringFieldTraits<T>::rangeType(), keyValueDataOffset + offsetof(KeyValue, valueRange), sizeof(KeyValue)},
        /* These two have the string data defined *before* the field data, thus
           storing a negative string data offset */
        SceneFieldData{fileField, fileTagData.slice(&FileTag::object),
            fileTagStringData.data(), StringFieldTraits<T>::offsetType(),
            fileTagData.slice(&FileTag::fileOffsetNullTerminated),
            SceneFieldFlag::NullTerminatedString},
        SceneFieldData{tagField, fileTagData.slice(&FileTag::object),
            fileTagStringData.data(), StringFieldTraits<T>::rangeNullTerminatedType(),
            fileTagData.slice(&FileTag::tagRangeNullTerminated)},
    }};

    /* Raw field data access to verify it correctly special-cases */
    CORRADE_COMPARE(scene.fieldData(1).mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldData(1).mappingData().data(), keyValueData.data());
    CORRADE_COMPARE(scene.fieldData(1).stringData(), nameKeyValueStringData.data());
    CORRADE_COMPARE(scene.fieldData(1).fieldType(), StringFieldTraits<T>::rangeType());
    CORRADE_COMPARE(scene.fieldData(1).fieldData().data(), keyValueData.slice(&KeyValue::keyRangeNullTerminated).data());
    CORRADE_COMPARE(scene.fieldData(1).fieldArraySize(), 0);
    CORRADE_COMPARE(scene.fieldData(1).flags(), SceneFieldFlag::NullTerminatedString);

    /* Field property access, to verify it correctly special-cases */
    CORRADE_COMPARE(scene.fieldType(keyField), StringFieldTraits<T>::rangeType());
    CORRADE_COMPARE(scene.fieldArraySize(keyField), 0);

    /* Field flags should contain the string-specific flags */
    CORRADE_COMPARE(scene.fieldFlags(nameField), SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldFlags(keyField), SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(scene.fieldFlags(valueField), SceneFieldFlag::OffsetOnly);
    CORRADE_COMPARE(scene.fieldFlags(fileField), SceneFieldFlag::NullTerminatedString);
    /* This one is added implicitly */
    CORRADE_COMPARE(scene.fieldFlags(tagField), SceneFieldFlag::NullTerminatedString);

    /* Mapping access should correctly special-case the string type stored in
       the same byte */
    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(nameField), Containers::arrayView<UnsignedShort>({
        1, 3
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.mutableMapping<UnsignedShort>(nameField), Containers::stridedArrayView<UnsignedShort>({
        1, 3
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.mappingAsArray(nameField), Containers::arrayView<UnsignedInt>({
        1, 3
    }), TestSuite::Compare::Container);

    /* Raw field data access. ID vs name of this API tested thoroughly enough
       in construct(). */
    CORRADE_COMPARE_AS(scene.field<T>(nameField), Containers::arrayView<T>({
        5, 14
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<Containers::Pair<T, T>>(keyField)), (Containers::arrayView<Containers::Pair<T, T>>({
        {20, 3}, {14, 5}, {20, 3}, {14, 5}
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<Containers::Pair<T, T>>(valueField)), (Containers::arrayView<Containers::Pair<T, T>>({
        {37, 3}, {24, 10}, {34, 3}, {29, 5}
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<T>(fileField), Containers::arrayView<T>({
        10, 20, 34
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<T>(tagField), Containers::arrayView<T>({
        34, 43, 43
    }), TestSuite::Compare::Container);

    /* Raw string data access using an ID */
    CORRADE_COMPARE(scene.fieldStringData(0), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(1), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(2), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(3), static_cast<const void*>(fileTagStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(4), static_cast<const void*>(fileTagStringData.data()));

    /* Raw string data access using a name */
    CORRADE_COMPARE(scene.fieldStringData(nameField), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(keyField), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(valueField), static_cast<const void*>(nameKeyValueStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(fileField), static_cast<const void*>(fileTagStringData.data()));
    CORRADE_COMPARE(scene.fieldStringData(tagField), static_cast<const void*>(fileTagStringData.data()));

    /* String access using an ID */
    CORRADE_COMPARE_AS(scene.fieldStrings(0), Containers::arrayView({
        "Chair"_s, "Lampshade"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(0)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlags{});
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(1), Containers::arrayView({
        "age"_s, "color"_s, "age"_s, "color"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(1)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(2), Containers::arrayView({
        "new"_s, "lightbrown"_s, "old"_s, "brown"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(2)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlags{});
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(3), Containers::arrayView({
        "chair.glb"_s, "empty.obj"_s, "lampshade.fbx"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(3)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(4), Containers::arrayView({
        "MAPPABLE"_s, "STRANGE"_s, "STRANGE"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(4)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }

    /* String access using a name */
    CORRADE_COMPARE_AS(scene.fieldStrings(nameField), Containers::arrayView({
        "Chair"_s, "Lampshade"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(nameField)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlags{});
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(keyField), Containers::arrayView({
        "age"_s, "color"_s, "age"_s, "color"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(keyField)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(valueField), Containers::arrayView({
        "new"_s, "lightbrown"_s, "old"_s, "brown"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(valueField)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlags{});
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(fileField), Containers::arrayView({
        "chair.glb"_s, "empty.obj"_s, "lampshade.fbx"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(fileField)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }
    CORRADE_COMPARE_AS(scene.fieldStrings(tagField), Containers::arrayView({
        "MAPPABLE"_s, "STRANGE"_s, "STRANGE"_s
    }), TestSuite::Compare::Container);
    for(Containers::StringView i: scene.fieldStrings(tagField)) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }
}

void SceneDataTest::constructSpecialStrides() {
    Containers::StridedArrayView1D<UnsignedShort> broadcastedData;
    Containers::StridedArrayView1D<UnsignedShort> nonBroadcastedData;
    Containers::ArrayTuple data{
        {NoInit, 1, broadcastedData},
        {NoInit, 4, nonBroadcastedData}
    };

    broadcastedData[0] = 15;
    Utility::copy({1, 2, 3, 4}, nonBroadcastedData);

    SceneFieldData broadcastedMapping{sceneFieldCustom(38),
        broadcastedData.broadcasted<0>(4), nonBroadcastedData};
    SceneFieldData broadcastedField{sceneFieldCustom(39),
        nonBroadcastedData, broadcastedData.broadcasted<0>(4)};
    SceneFieldData flippedField{sceneFieldCustom(40),
        nonBroadcastedData.flipped<0>(), nonBroadcastedData.flipped<0>()};
    SceneFieldData flippedFieldOffsetOnly{sceneFieldCustom(41),
        4, SceneMappingType::UnsignedShort, std::size_t(static_cast<char*>(nonBroadcastedData.flipped<0>().data()) - data.data()), -2,
        SceneFieldType::UnsignedShort, std::size_t(static_cast<char*>(nonBroadcastedData.flipped<0>().data()) - data.data()), -2};
    SceneData scene{SceneMappingType::UnsignedShort, 8, Utility::move(data), {
        broadcastedMapping, broadcastedField, flippedField, flippedFieldOffsetOnly
    }};

    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(0),
        Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedShort>(0),
        Containers::arrayView<UnsignedShort>({1, 2, 3, 4}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(1),
        Containers::arrayView<UnsignedShort>({1, 2, 3, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedShort>(1),
        Containers::arrayView<UnsignedShort>({15, 15, 15, 15}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(2),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedShort>(2),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(3),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedShort>(3),
        Containers::arrayView<UnsignedShort>({4, 3, 2, 1}),
        TestSuite::Compare::Container);
}

void SceneDataTest::constructSpecialStridesBit() {
    auto&& instanceData = ConstructSpecialStridesBitData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Similar to constructBit(), except that the strides are negative and
       zero, and only properties related to those are tested */

    struct Data {
        UnsignedByte object;
        /* Bit 3 is one field, 5-7 the other; both are then included as both
           direct and offset-only fields, the array then also with a negative
           stride */
        UnsignedByte bits;
    } data[]{
        {4, 0x01 << 3 | 0x02 << 5},
        {0, 0x01 << 3 | 0x02 << 5},
        {9, 0x00 << 3 | 0x05 << 5},
        {2, 0x01 << 3 | 0x07 << 5},
    };
    auto view = Containers::stridedArrayView(data);

    constexpr SceneField bitField = sceneFieldCustom(0);
    constexpr SceneField arrayField = sceneFieldCustom(1);
    constexpr SceneField bitFieldOffsetOnly = sceneFieldCustom(10);
    constexpr SceneField arrayFieldOffsetOnly = sceneFieldCustom(11);

    SceneData scene{SceneMappingType::UnsignedByte, 10, DataFlag::Mutable, data, {
        SceneFieldData{bitField, view.slice(&Data::object),
            Containers::StridedBitArrayView1D{Containers::BitArrayView{data}, &data[0].bits + instanceData.offset, instanceData.bitOffset, 4, instanceData.stride}, SceneFieldFlag::ImplicitMapping},
        SceneFieldData{arrayField, view.slice(&Data::object),
            Containers::StridedBitArrayView2D{Containers::BitArrayView{data}, &data[0].bits + instanceData.offset, instanceData.arrayBitOffset, {4, 3}, {instanceData.stride, 1}}, SceneFieldFlag::OrderedMapping},
        /* The two above, just as offset-only */
        SceneFieldData{bitFieldOffsetOnly, 4,
            SceneMappingType::UnsignedByte, 0, sizeof(Data),
            offsetof(Data, bits) + instanceData.offset, instanceData.bitOffset, instanceData.stride, SceneFieldFlag::ImplicitMapping},
        SceneFieldData{arrayFieldOffsetOnly, 4,
            SceneMappingType::UnsignedByte, 0, sizeof(Data),
            offsetof(Data, bits) + instanceData.offset, instanceData.arrayBitOffset, instanceData.stride, 3, SceneFieldFlag::OrderedMapping},
    }};

    /* Raw field data access */
    for(UnsignedInt i: {0, 2}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().size(), (Containers::Size2D{4, 1}));
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().stride(), (Containers::Stride2D{instanceData.stride, 1}));
        CORRADE_COMPARE_AS((scene.fieldData(i).fieldBitData().transposed<0, 1>())[0],
            (Containers::BitArrayView{&instanceData.expectedBits, 0, 4}),
            TestSuite::Compare::Container);
    }
    for(UnsignedInt i: {1, 3}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.fieldData(i).fieldBitData().stride(), (Containers::Stride2D{instanceData.stride, 1}));
        /* Testing just the first slice, should be enough. The whole data is
           tested properly below */
        CORRADE_COMPARE_AS((scene.fieldData(i).fieldBitData().transposed<0, 1>())[0],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[0], 0, 4}),
            TestSuite::Compare::Container);
    }

    /* Bit fields */
    for(SceneField i: {bitField, bitFieldOffsetOnly}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBits(i).stride(), instanceData.stride);
        CORRADE_COMPARE_AS(scene.fieldBits(i),
            (Containers::BitArrayView{&instanceData.expectedBits, 0, 4}),
            TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBits(i).stride(), instanceData.stride);
        CORRADE_COMPARE_AS(scene.mutableFieldBits(i),
            (Containers::BitArrayView{&instanceData.expectedBits, 0, 4}),
            TestSuite::Compare::Container<Containers::StridedBitArrayView1D>);
    }

    /* Bit array fields */
    for(SceneField i: {arrayField, arrayFieldOffsetOnly}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(scene.fieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.fieldBitArrays(i).stride(), (Containers::Stride2D{instanceData.stride, 1}));
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[0],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[0], 0, 4}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[1],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[1], 0, 4}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((scene.fieldBitArrays(i).transposed<0, 1>())[2],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[0], 0, 4}),
            TestSuite::Compare::Container);

        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).size(), (Containers::Size2D{4, 3}));
        CORRADE_COMPARE(scene.mutableFieldBitArrays(i).stride(), (Containers::Stride2D{instanceData.stride, 1}));
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[0],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[0], 0, 4}),
            TestSuite::Compare::Container<Containers::StridedBitArrayView1D>);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[1],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[1], 0, 4}),
            TestSuite::Compare::Container<Containers::StridedBitArrayView1D>);
        CORRADE_COMPARE_AS((scene.mutableFieldBitArrays(i).transposed<0, 1>())[2],
            (Containers::BitArrayView{&instanceData.expectedArrayBits[0], 0, 4}),
            TestSuite::Compare::Container<Containers::StridedBitArrayView1D>);
    }
}

#ifdef MAGNUM_BUILD_DEPRECATED
void SceneDataTest::constructDeprecated() {
    auto&& data = ChildrenDeprecatedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    int a;
    CORRADE_IGNORE_DEPRECATED_PUSH
    SceneData scene{
        data.is2D ? std::vector<UnsignedInt>{5, 17, 36, 22} : std::vector<UnsignedInt>{},
        data.is3D ? std::vector<UnsignedInt>{5, 17, 36, 22} : std::vector<UnsignedInt>{},
        &a};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedInt);
    if(data.is2D || data.is3D)
        CORRADE_COMPARE(scene.mappingBound(), 37);
    else
        CORRADE_COMPARE(scene.mappingBound(), 0);
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Mutable|DataFlag::Owned);
    CORRADE_COMPARE(scene.importerState(), &a);
    CORRADE_COMPARE(scene.fieldCount(), 1);
    CORRADE_COMPARE(scene.fieldName(0), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldFlags(0), SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(0), SceneFieldType::Int);
    if(data.is2D || data.is3D) {
        CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(0),
            Containers::arrayView<UnsignedInt>({5, 17, 36, 22}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.field<Int>(0),
            Containers::arrayView<Int>({-1, -1, -1, -1}),
            TestSuite::Compare::Container);
    } else CORRADE_COMPARE(scene.fieldSize(SceneField::Parent), 0);
    /* There's no transformation field that would disambiguate this, the state
       is set directly */
    CORRADE_COMPARE(scene.is2D(), data.is2D);
    CORRADE_COMPARE(scene.is3D(), data.is3D);

    /* The deleters have to be trivial, otherwise this instance wouldn't be
       usable from an AbstractImporter */
    CORRADE_VERIFY(!scene.releaseFieldData().deleter());
    CORRADE_VERIFY(!scene.releaseData().deleter());
}

void SceneDataTest::constructDeprecatedBoth2DAnd3D() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    SceneData scene{{5, 17}, {36, 22}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out, "Trade::SceneData: it's no longer possible to have a scene with both 2D and 3D objects\n");
}
#endif

#ifndef CORRADE_TARGET_32BIT
void SceneDataTest::constructMappingOver4GB() {
    Containers::ArrayView<UnsignedInt> mappingData{reinterpret_cast<UnsignedInt*>(std::size_t{0xdeadbeef}), 3000ull*1000*1000};
    Containers::StridedArrayView1D<UnsignedByte> fieldData{mappingData, reinterpret_cast<UnsignedByte*>(mappingData.data()), 3000ull*1000*1000, 0};

    SceneData data{SceneMappingType::UnsignedInt, 1, {}, mappingData, {
        SceneFieldData{sceneFieldCustom(15), mappingData, fieldData},
    }};
    CORRADE_COMPARE(data.mapping(0).data(), mappingData.begin());
    CORRADE_COMPARE(data.mapping<UnsignedInt>(0).size(), mappingData.size());
}

void SceneDataTest::constructFieldOver4GB() {
    Containers::ArrayView<UnsignedInt> fieldData{reinterpret_cast<UnsignedInt*>(std::size_t{0xdeadbeef}), 3000ull*1000*1000};
    Containers::StridedArrayView1D<UnsignedByte> mappingData{fieldData, reinterpret_cast<UnsignedByte*>(fieldData.data()), 3000ull*1000*1000, 0};

    SceneData data{SceneMappingType::UnsignedByte, 1, {}, fieldData, {
        SceneFieldData{sceneFieldCustom(15), mappingData, fieldData},
    }};
    CORRADE_COMPARE(data.field(0).data(), fieldData.begin());
    CORRADE_COMPARE(data.field<UnsignedInt>(0).size(), fieldData.size());
}
#endif

void SceneDataTest::constructDuplicateField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Builtin fields are checked using a bitfield, as they have monotonic
       numbering */
    SceneFieldData meshes{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr};
    SceneFieldData meshesAgain{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};

    Containers::String out;
    Error redirectError{&out};
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {meshes, materials, meshesAgain}};
    CORRADE_COMPARE(out, "Trade::SceneData: duplicate field Trade::SceneField::Mesh\n");
}

void SceneDataTest::constructDuplicateCustomField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* These are checked in an O(n^2) way, separately from builtin fields.
       Can't use a bitfield since the field index can be anything. */
    SceneFieldData customA{sceneFieldCustom(37), SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData customB{sceneFieldCustom(1038576154), SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};
    SceneFieldData customAAgain{sceneFieldCustom(37), SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};

    Containers::String out;
    Error redirectError{&out};
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {customA, customB, customAAgain}};
    CORRADE_COMPARE(out, "Trade::SceneData: duplicate field Trade::SceneField::Custom(37)\n");
}

void SceneDataTest::constructInconsistentMappingType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    SceneFieldData meshes{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneMappingType::UnsignedShort, nullptr, SceneFieldType::Int, nullptr};

    Containers::String out;
    Error redirectError{&out};
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {meshes, materials}};
    CORRADE_COMPARE(out, "Trade::SceneData: inconsistent mapping type, got Trade::SceneMappingType::UnsignedShort for field 1 but expected Trade::SceneMappingType::UnsignedInt\n");
}

void SceneDataTest::constructMappingDataNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const Containers::Array<char> data{reinterpret_cast<char*>(0xbadda9), 5*sizeof(UnsignedShort), [](char*, std::size_t){}};
    Containers::Array<char> sameDataButMovable{reinterpret_cast<char*>(0xbadda9), 5*sizeof(UnsignedShort), [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> dataIn{reinterpret_cast<UnsignedShort*>(0xbadda9), 5};
    Containers::ArrayView<UnsignedShort> dataSlightlyOut{reinterpret_cast<UnsignedShort*>(0xbaddaa), 5};
    Containers::ArrayView<UnsignedShort> dataOut{reinterpret_cast<UnsignedShort*>(0xdead), 5};

    Containers::String out;
    Error redirectError{&out};
    /* Basic "obviously wrong" case with owned data */
    SceneData{SceneMappingType::UnsignedShort, 5, Utility::move(sameDataButMovable), {
        /* This is here to test that not just the first field gets checked and
           that the message shows proper ID */
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataOut, dataIn}
    }};
    /* A "slightly off" view that exceeds the original by one byte */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, dataSlightlyOut, dataIn}
    }};
    /* And if we have no data at all, it doesn't try to dereference them but
       still checks properly */
    SceneData{SceneMappingType::UnsignedShort, 5, nullptr, {
        SceneFieldData{SceneField::Mesh, dataOut, dataIn}
    }};
    /* Offset-only fields with a different message */
    SceneData{SceneMappingType::UnsignedByte, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneMappingType::UnsignedByte, 4, 4, SceneFieldType::UnsignedByte, 0, 4}
    }};
    /* And the final boss, negative strides. Both only caught if the element
       size gets properly added to the larger offset, not just the "end". */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, stridedArrayView(dataSlightlyOut).flipped<0>(), dataIn}
    }};
    SceneData{SceneMappingType::UnsignedByte, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneMappingType::UnsignedByte, 24, -4, SceneFieldType::UnsignedByte, 0, 4}
    }};
    CORRADE_COMPARE(out,
        "Trade::SceneData: mapping data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: mapping data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: mapping data [0xdead:0xdeb7] of field 0 are not contained in passed data array [0x0:0x0]\n"

        "Trade::SceneData: offset-only mapping data of field 0 span 25 bytes but passed data array has only 24\n"

        "Trade::SceneData: mapping data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: offset-only mapping data of field 0 span 25 bytes but passed data array has only 24\n");
}

void SceneDataTest::constructFieldDataNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Mostly the same as constructMappingDataNotContained() with mapping and
       field views swapped, and added checks for array fields. Bit fields have
       many special cases on their own and are tested in
       constructBitFieldDataNotContained() below. */

    const Containers::Array<char> data{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::Array<char> sameDataButMovable{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> dataIn{reinterpret_cast<UnsignedShort*>(0xbadda9), 5};
    Containers::ArrayView<UnsignedShort> dataSlightlyOut{reinterpret_cast<UnsignedShort*>(0xbaddaa), 5};
    Containers::ArrayView<UnsignedShort> dataOut{reinterpret_cast<UnsignedShort*>(0xdead), 5};

    Containers::String out;
    Error redirectError{&out};
    /* Basic "obviously wrong" case with owned data */
    SceneData{SceneMappingType::UnsignedShort, 5, Utility::move(sameDataButMovable), {
        /* This is here to test that not just the first attribute gets checked
           and that the message shows proper ID */
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataIn, dataOut}
    }};
    /* A "slightly off" view that exceeds the original by one byte */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, dataIn, dataSlightlyOut}
    }};
    /* Verify array size is taken into account as well. If not, the data would
       span only 7 bytes out of 10 (instead of 12), which is fine. */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{sceneFieldCustom(37), dataIn.prefix(2), Containers::StridedArrayView2D<UnsignedByte>{Containers::ArrayView<UnsignedByte>{reinterpret_cast<UnsignedByte*>(0xbadda9), 12}, {2, 6}}}
    }};
    /* Not checking for nullptr data, since that got checked for mapping view
       already and there's no way to trigger it for fields */
    /* Offset-only fields with a different message */
    SceneData{SceneMappingType::UnsignedShort, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneMappingType::UnsignedShort, 0, 4, SceneFieldType::UnsignedByte, 4, 4}
    }};
    /* This again spans 21 bytes if array size isn't taken into account, and 25
       if it is */
    SceneData{SceneMappingType::UnsignedShort, 5, Containers::Array<char>{24}, {
        SceneFieldData{sceneFieldCustom(37), 5, SceneMappingType::UnsignedShort, 0, 5, SceneFieldType::UnsignedByte, 0, 5, 5}
    }};
    /* And the final boss, negative strides. Both only caught if the element
       size gets properly added to the larger offset, not just the "end". */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, dataIn, stridedArrayView(dataSlightlyOut).flipped<0>()}
    }};
    SceneData{SceneMappingType::UnsignedByte, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneMappingType::UnsignedByte, 0, 4, SceneFieldType::UnsignedByte, 24, -4}
    }};
    CORRADE_COMPARE(out,
        "Trade::SceneData: field data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda9:0xbaddb5] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"

        "Trade::SceneData: offset-only field data of field 0 span 25 bytes but passed data array has only 24\n"
        "Trade::SceneData: offset-only field data of field 0 span 25 bytes but passed data array has only 24\n"

        "Trade::SceneData: field data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: offset-only field data of field 0 span 25 bytes but passed data array has only 24\n");
}

void SceneDataTest::constructBitFieldDataNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Tested separately from constructFieldDataNotContained() as there's
       enough bit-related cases on its own. It goes through the same variants,
       but explicitly checks also non-zero bit offset. */

    const Containers::Array<char> data{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::Array<char> sameDataButMovable{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedByte> mappingData{reinterpret_cast<UnsignedByte*>(0xbadda9), 10};
    Containers::StridedBitArrayView1D dataOneByteOut{Containers::BitArrayView{reinterpret_cast<void*>(std::size_t{0xbaddaa}), 0, 80}, 10, 8};
    Containers::StridedBitArrayView1D dataTwoBitsOut{Containers::BitArrayView{reinterpret_cast<void*>(std::size_t{0xbadda9}), 0, 90}, 10, 9};
    Containers::StridedBitArrayView1D dataOneBitOffsetOut{Containers::BitArrayView{reinterpret_cast<void*>(std::size_t{0xbadda9}), 4, 95}, 5, 19};
    Containers::StridedBitArrayView1D dataOneBitOffsetBeforeOut{Containers::BitArrayView{reinterpret_cast<void*>(std::size_t{0xbadda8}), 7, 80}, 10, 8};
    Containers::BitArrayView dataOut{reinterpret_cast<void*>(std::size_t{0xdead}), 7, 10};

    Containers::String out;
    Error redirectError{&out};
    /* Basic "obviously wrong" case with owned data */
    SceneData{SceneMappingType::UnsignedByte, 10, Utility::move(sameDataButMovable), {
        /* This is here to test that not just the first attribute gets checked
           and that the message shows proper ID */
        SceneFieldData{SceneField::Light, mappingData, mappingData},
        SceneFieldData{sceneFieldCustom(773), mappingData, dataOut}
    }};
    /* A "slightly off" view that exceeds the original by one byte and two bits,
       respectively */
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData, dataOneByteOut}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData, dataTwoBitsOut}
    }};
    /* Verify the bit offset is taken into account. On begin it shouldn't be
       rounded up, otherwise the second variant would pass.*/
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData.prefix(5), dataOneBitOffsetOut}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData, dataOneBitOffsetBeforeOut}
    }};
    /* Verify array size is taken into account as well. If not, the data would
       span only 9 bytes instead of 11, which would pass. */
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData.prefix(9), Containers::StridedBitArrayView2D{Containers::BitArrayView{reinterpret_cast<void*>(std::size_t{0xbadda9}), 0, 81}, {9, 9}}}
    }};
    /* Not checking for nullptr data, since that got checked for mapping view
       already and there's no way to trigger it for fields */
    /* Offset-only fields with a different message, again both one byte and one
       bit off, one bit with offset */
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 10, SceneMappingType::UnsignedByte, 0, 1, 1, 0, 8}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 10, SceneMappingType::UnsignedByte, 0, 1, 0, 0, 9}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 5, SceneMappingType::UnsignedByte, 0, 1, 0, 4, 19}
    }};
    /* One with array */
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 9, SceneMappingType::UnsignedByte, 0, 1, 0, 0, 9, 9}
    }};
    /* And the final boss, negative strides -- one byte off and two bits off.
       Both only caught if the element size gets properly added to the larger
       offset, not just the "end". */
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData, dataOneByteOut.flipped<0>()}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, {}, data, {
        SceneFieldData{sceneFieldCustom(773), mappingData, dataTwoBitsOut.flipped<0>()}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 10, SceneMappingType::UnsignedByte, 0, 1, 10, 0, -8}
    }};
    SceneData{SceneMappingType::UnsignedByte, 10, Containers::Array<char>{10}, {
        SceneFieldData{sceneFieldCustom(773), 9, SceneMappingType::UnsignedByte, 0, 1, 10, 0, -9}
    }};
    CORRADE_COMPARE(out,
        "Trade::SceneData: field data [0xdead:0xdeb0] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"

        "Trade::SceneData: field data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda9:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda9:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda8:0xbaddb2] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"

        "Trade::SceneData: field data [0xbadda9:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"

        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"
        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"
        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"

        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"

        "Trade::SceneData: field data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda9:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"
        "Trade::SceneData: offset-only field data of field 0 span 11 bytes but passed data array has only 10\n"
    );
}

void SceneDataTest::constructStringDataNotContained() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::ArrayView<char> data{reinterpret_cast<char*>(0xbadda9), 10};
    Containers::ArrayView<UnsignedShort> dataIn{reinterpret_cast<UnsignedShort*>(0xbadda9), 5};

    /* This should be fine even though it points to the very end (the string
       array could be empty) */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{sceneFieldCustom(35), dataIn,
            reinterpret_cast<char*>(0xbadda9 + 10), SceneFieldType::StringOffset16,
            dataIn}
    }};

    Containers::String out;
    Error redirectError{&out};
    /* Data too early */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        /* This is here to test that not just the first attribute gets checked
           and that the message shows proper ID */
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{sceneFieldCustom(35), dataIn,
            reinterpret_cast<char*>(0xbadda9 - 1), SceneFieldType::StringOffset16,
            dataIn}
    }};
    /* Data too late */
    SceneData{SceneMappingType::UnsignedShort, 5, {}, data, {
        SceneFieldData{sceneFieldCustom(35), dataIn,
            reinterpret_cast<char*>(0xbaddaa9 + 11), SceneFieldType::StringRange8,
            dataIn}
    }};
    CORRADE_COMPARE(out,
        "Trade::SceneData: field string data 0xbadda8 of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field string data 0xbaddab4 of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n");
}

void SceneDataTest::constructMappingTypeTooSmall() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This is fine */
    SceneData{SceneMappingType::UnsignedByte, 0xff, nullptr, {}};
    SceneData{SceneMappingType::UnsignedShort, 0xffff, nullptr, {}};
    SceneData{SceneMappingType::UnsignedInt, 0xffffffffu, nullptr, {}};

    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedByte, 0x100, nullptr, {}};
    SceneData{SceneMappingType::UnsignedShort, 0x10000, nullptr, {}};
    SceneData{SceneMappingType::UnsignedInt, 0x100000000ull, nullptr, {}};
    CORRADE_COMPARE(out,
        "Trade::SceneData: Trade::SceneMappingType::UnsignedByte is too small for 256 objects\n"
        "Trade::SceneData: Trade::SceneMappingType::UnsignedShort is too small for 65536 objects\n"
        "Trade::SceneData: Trade::SceneMappingType::UnsignedInt is too small for 4294967296 objects\n");
}

void SceneDataTest::constructNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char data[32]{};

    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedByte, 5, DataFlag::Owned, Containers::arrayView(data), {}};
    CORRADE_COMPARE(out,
        "Trade::SceneData: can't construct with non-owned data but Trade::DataFlag::Owned\n");
}

void SceneDataTest::constructMismatchedTRSViews() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::ArrayView<const char> data{reinterpret_cast<char*>(std::size_t{0xcafe0000}),
        4 + 3*24 + 3*4};
    Containers::ArrayView<const UnsignedInt> mappingData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 4 + 3*24), 3};
    Containers::ArrayView<const UnsignedInt> mappingDifferentPointerData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 3*24), 3};
    Containers::StridedArrayView1D<const UnsignedInt> mappingDifferentStrideData{
        data, reinterpret_cast<const UnsignedInt*>(data.data() + 4), 3, 8};
    Containers::ArrayView<const Vector2> translationFieldData{
        reinterpret_cast<const Vector2*>(data.data() + 4), 3};
    Containers::ArrayView<const Complex> rotationFieldData{
        reinterpret_cast<const Complex*>(data.data() + 4 + 8), 3};
    Containers::ArrayView<const Vector2> scalingFieldData{
        reinterpret_cast<const Vector2*>(data.data() + 4 + 12), 3};

    SceneFieldData translations{SceneField::Translation, mappingData, translationFieldData};
    SceneFieldData rotations{SceneField::Rotation, mappingData, rotationFieldData};
    SceneFieldData scalings{SceneField::Scaling, mappingData, scalingFieldData};
    SceneFieldData rotationsDifferentPointer{SceneField::Rotation, mappingDifferentPointerData, rotationFieldData};
    SceneFieldData scalingsDifferentPointer{SceneField::Scaling, mappingDifferentPointerData, scalingFieldData};
    SceneFieldData rotationsDifferentSize{SceneField::Rotation, mappingData.exceptSuffix(1), rotationFieldData.exceptSuffix(1)};
    SceneFieldData scalingsDifferentSize{SceneField::Scaling, mappingData.exceptSuffix(1), scalingFieldData.exceptSuffix(1)};
    SceneFieldData rotationsDifferentStride{SceneField::Rotation, mappingDifferentStrideData, rotationFieldData};
    SceneFieldData scalingsDifferentStride{SceneField::Scaling, mappingDifferentStrideData, scalingFieldData};

    SceneFieldData translationsOffsetOnly{SceneField::Translation, 3,
        SceneMappingType::UnsignedInt, 4 + 3*24, sizeof(UnsignedInt),
        SceneFieldType::Vector2, 4, sizeof(Vector2)};
    SceneFieldData rotationsOffsetOnly{SceneField::Rotation, 3,
        SceneMappingType::UnsignedInt, 4 + 3*24, sizeof(UnsignedInt),
        SceneFieldType::Complex, 4, sizeof(Complex)};
    SceneFieldData scalingsOffsetOnly{SceneField::Scaling, 3,
        SceneMappingType::UnsignedInt, 4 + 3*24, sizeof(UnsignedInt),
        SceneFieldType::Vector2, 4, sizeof(Vector2)};
    SceneFieldData rotationsDifferentPointerOffsetOnly{SceneField::Rotation, 3,
        SceneMappingType::UnsignedInt, 3*24, sizeof(UnsignedInt),
        SceneFieldType::Complex, 4, sizeof(Complex)};
    SceneFieldData scalingsDifferentPointerOffsetOnly{SceneField::Scaling, 3,
        SceneMappingType::UnsignedInt, 3*24, sizeof(UnsignedInt),
        SceneFieldType::Vector2, 4, sizeof(Vector2)};

    /* The matching offset-only variants should give no assert */
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, rotationsOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translationsOffsetOnly, rotations}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, scalingsOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translationsOffsetOnly, scalings}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotations, scalingsOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotationsOffsetOnly, scalings}};

    /* Test that all pairs get checked */
    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, rotationsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, rotationsDifferentPointerOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translationsOffsetOnly, rotationsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, rotationsDifferentSize}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, rotationsDifferentStride}};

    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, scalingsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, scalingsDifferentPointerOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translationsOffsetOnly, scalingsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, scalingsDifferentSize}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {translations, scalingsDifferentStride}};

    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotations, scalingsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotations, scalingsDifferentPointerOffsetOnly}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotationsOffsetOnly, scalingsDifferentPointer}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotations, scalingsDifferentSize}};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {rotations, scalingsDifferentStride}};
    CORRADE_COMPARE_AS(out,
        /* Different pointer, three variants with offset-only */
        "Trade::SceneData: Trade::SceneField::Rotation mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Rotation mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Rotation mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        /* Different size */
        "Trade::SceneData: Trade::SceneField::Rotation mapping data {0xcafe004c, 2, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        /* Different stride */
        "Trade::SceneData: Trade::SceneField::Rotation mapping data {0xcafe0004, 3, 8} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"

        /* Different pointer, three variants with offset-only */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        /* Different size */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe004c, 2, 4} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"
        /* Different stride */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0004, 3, 8} is different from Trade::SceneField::Translation mapping data {0xcafe004c, 3, 4}\n"

        /* Different pointer, three variants with offset-only */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Rotation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Rotation mapping data {0xcafe004c, 3, 4}\n"
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0048, 3, 4} is different from Trade::SceneField::Rotation mapping data {0xcafe004c, 3, 4}\n"
        /* Different size */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe004c, 2, 4} is different from Trade::SceneField::Rotation mapping data {0xcafe004c, 3, 4}\n"
        /* Different stride */
        "Trade::SceneData: Trade::SceneField::Scaling mapping data {0xcafe0004, 3, 8} is different from Trade::SceneField::Rotation mapping data {0xcafe004c, 3, 4}\n",
        TestSuite::Compare::String);
}

template<class> struct NameTraits;
#define _c(format) template<> struct NameTraits<format> {                   \
        static const char* name() { return #format; }                       \
    };
_c(UnsignedByte)
_c(Byte)
_c(UnsignedShort)
_c(Short)
_c(UnsignedInt)
_c(Int)
_c(UnsignedLong)
_c(Long)
_c(Float)
_c(Double)
_c(Vector2)
_c(Vector2d)
_c(Vector3)
_c(Vector3d)
_c(Matrix3)
_c(Matrix3d)
_c(Matrix3x2)
_c(Matrix3x2d)
_c(Matrix4)
_c(Matrix4d)
_c(Matrix4x3)
_c(Matrix4x3d)
_c(Complex)
_c(Complexd)
_c(Quaternion)
_c(Quaterniond)
_c(DualComplex)
_c(DualComplexd)
_c(DualQuaternion)
_c(DualQuaterniond)
template<class T> struct NameTraits<const T*> {
    static const char* name() { return "Pointer"; }
};
template<class T> struct NameTraits<T*> {
    static const char* name() { return "MutablePointer"; }
};
#undef _c

template<class T> void SceneDataTest::constructMismatchedTRSDimensionality() {
    setTestCaseTemplateName(NameTraits<T>::name());

    CORRADE_SKIP_IF_NO_ASSERT();

    SceneFieldData transformationMatrices2D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix3<T>>::type(), nullptr};
    SceneFieldData transformationRectangularMatrices2D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix3x2<T>>::type(), nullptr};
    SceneFieldData transformations2D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::DualComplex<T>>::type(), nullptr};
    SceneFieldData transformationMatrices3D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix4<T>>::type(), nullptr};
    SceneFieldData transformationRectangularMatrices3D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix4x3<T>>::type(), nullptr};
    SceneFieldData transformations3D{SceneField::Transformation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::DualQuaternion<T>>::type(), nullptr};
    SceneFieldData translations2D{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector2<T>>::type(), nullptr};
    SceneFieldData translations3D{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector3<T>>::type(), nullptr};
    SceneFieldData rotations2D{SceneField::Rotation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Complex<T>>::type(), nullptr};
    SceneFieldData rotations3D{SceneField::Rotation, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Quaternion<T>>::type(), nullptr};
    SceneFieldData scalings2D{SceneField::Scaling, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector2<T>>::type(), nullptr};
    SceneFieldData scalings3D{SceneField::Scaling, SceneMappingType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector3<T>>::type(), nullptr};

    /* Test that all pairs get checked */
    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices2D, translations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices2D, rotations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices2D, scalings3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices2D, translations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices2D, rotations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices2D, scalings3D}};

    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations2D, translations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations2D, rotations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations2D, scalings3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {translations2D, rotations3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {translations2D, scalings3D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {rotations2D, scalings3D}};

    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices3D, translations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices3D, rotations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationMatrices3D, scalings2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices3D, translations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices3D, rotations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformationRectangularMatrices3D, scalings2D}};

    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations3D, translations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations3D, rotations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {transformations3D, scalings2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {translations3D, rotations2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {translations3D, scalings2D}};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {rotations3D, scalings2D}};
    CORRADE_COMPARE(out, Utility::format(
        "Trade::SceneData: expected a 2D translation field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D rotation field but got Trade::SceneFieldType::{1}\n"
        "Trade::SceneData: expected a 2D scaling field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D translation field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D rotation field but got Trade::SceneFieldType::{1}\n"
        "Trade::SceneData: expected a 2D scaling field but got Trade::SceneFieldType::{0}\n"

        "Trade::SceneData: expected a 2D translation field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D rotation field but got Trade::SceneFieldType::{1}\n"
        "Trade::SceneData: expected a 2D scaling field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D rotation field but got Trade::SceneFieldType::{1}\n"
        "Trade::SceneData: expected a 2D scaling field but got Trade::SceneFieldType::{0}\n"
        "Trade::SceneData: expected a 2D scaling field but got Trade::SceneFieldType::{0}\n"

        "Trade::SceneData: expected a 3D translation field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D rotation field but got Trade::SceneFieldType::{3}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D translation field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D rotation field but got Trade::SceneFieldType::{3}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n"

        "Trade::SceneData: expected a 3D translation field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D rotation field but got Trade::SceneFieldType::{3}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D rotation field but got Trade::SceneFieldType::{3}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n",
        NameTraits<Math::Vector3<T>>::name(),
        NameTraits<Math::Quaternion<T>>::name(),
        NameTraits<Math::Vector2<T>>::name(),
        NameTraits<Math::Complex<T>>::name()));
}

void SceneDataTest::constructMismatchedMeshMaterialView() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Different sizes, strides and offset-only field handling tested
       thoroughly in constructMismatchedTRSViews() already */

    Containers::ArrayView<const char> data{reinterpret_cast<char*>(std::size_t{0xcafe0000}),
        4 + 3*8 + 3*4};
    Containers::ArrayView<const UnsignedInt> mappingData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 4 + 3*8), 3};
    Containers::ArrayView<const UnsignedInt> mappingDifferentPointerData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 3*8), 3};
    Containers::ArrayView<const UnsignedInt> meshFieldData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 4), 3};
    Containers::ArrayView<const Int> meshMaterialFieldData{
        reinterpret_cast<const Int*>(data.data() + 4 + 3*4), 3};

    SceneFieldData meshes{SceneField::Mesh, mappingData, meshFieldData};
    SceneFieldData meshMaterialsDifferent{SceneField::MeshMaterial, mappingDifferentPointerData, meshMaterialFieldData};

    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedInt, 3, {}, data, {meshes, meshMaterialsDifferent}};
    CORRADE_COMPARE(out,
        "Trade::SceneData: Trade::SceneField::MeshMaterial mapping data {0xcafe0018, 3, 4} is different from Trade::SceneField::Mesh mapping data {0xcafe001c, 3, 4}\n");
}

void SceneDataTest::constructAmbiguousSkinDimensions() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    SceneData{SceneMappingType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Skin, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr}
    }};
    CORRADE_COMPARE(out, "Trade::SceneData: a skin field requires some transformation field to be present in order to disambiguate between 2D and 3D\n");
}

void SceneDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<SceneData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<SceneData>{});
}

void SceneDataTest::constructMove() {
    struct Mesh {
        UnsignedShort object;
        UnsignedInt mesh;
    };

    Containers::Array<char> data{NoInit, 3*sizeof(Mesh)};
    auto meshData = Containers::arrayCast<Mesh>(data);
    meshData[0] = {0, 2};
    meshData[1] = {73, 1};
    meshData[2] = {122, 2};

    int importerState;
    SceneFieldData meshes{SceneField::Mesh, stridedArrayView(meshData).slice(&Mesh::object), stridedArrayView(meshData).slice(&Mesh::mesh)};
    SceneData a{SceneMappingType::UnsignedShort, 15, Utility::move(data), {meshes}, &importerState};

    SceneData b{Utility::move(a)};
    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.mappingBound(), 15);
    CORRADE_COMPARE(b.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(b.fieldCount(), 1);
    CORRADE_COMPARE(b.importerState(), &importerState);
    CORRADE_COMPARE(static_cast<const void*>(b.data()), meshData.data());
    CORRADE_COMPARE(b.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(b.fieldType(0), SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(b.fieldSize(0), 3);
    CORRADE_COMPARE(b.fieldArraySize(0), 0);
    CORRADE_COMPARE(b.mapping<UnsignedShort>(0)[2], 122);
    CORRADE_COMPARE(b.field<UnsignedInt>(0)[2], 2);

    SceneData c{SceneMappingType::UnsignedByte, 76, nullptr, {}};
    c = Utility::move(b);
    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.mappingBound(), 15);
    CORRADE_COMPARE(c.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(c.fieldCount(), 1);
    CORRADE_COMPARE(c.importerState(), &importerState);
    CORRADE_COMPARE(static_cast<const void*>(c.data()), meshData.data());
    CORRADE_COMPARE(c.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(c.fieldType(0), SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(c.fieldSize(0), 3);
    CORRADE_COMPARE(c.fieldArraySize(0), 0);
    CORRADE_COMPARE(c.mapping<UnsignedShort>(0)[2], 122);
    CORRADE_COMPARE(c.field<UnsignedInt>(0)[2], 2);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SceneData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SceneData>::value);
}

void SceneDataTest::findFieldId() {
    SceneData scene{SceneMappingType::UnsignedInt, 0, {}, nullptr, {
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedByte, nullptr}
    }};

    CORRADE_COMPARE(scene.findFieldId(SceneField::Parent), 0);
    CORRADE_COMPARE(scene.findFieldId(SceneField::Mesh), 1);
    CORRADE_COMPARE(scene.findFieldId(SceneField::MeshMaterial), Containers::NullOpt);

    CORRADE_COMPARE(scene.fieldId(SceneField::Parent), 0);
    CORRADE_COMPARE(scene.fieldId(SceneField::Mesh), 1);

    CORRADE_VERIFY(scene.hasField(SceneField::Parent));
    CORRADE_VERIFY(scene.hasField(SceneField::Mesh));
    CORRADE_VERIFY(!scene.hasField(SceneField::MeshMaterial));
}

template<class T> void SceneDataTest::findFieldObjectOffset() {
    setTestCaseTemplateName(NameTraits<T>::name());

    auto&& data = FindFieldObjectOffsetData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Field {
        T object;
        UnsignedInt mesh;
    } fields[5]{
        {T(data.mapping[0]), 0},
        {T(data.mapping[1]), 0},
        {T(data.mapping[2]), 0},
        {T(data.mapping[3]), 0},
        {T(data.mapping[4]), 0}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{Implementation::sceneMappingTypeFor<T>(), 7, {}, fields, {
        /* Test also with a completely empty field */
        SceneFieldData{SceneField::Parent, Implementation::sceneMappingTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh), data.flags}
    }};

    /* An empty field should not find anything for any query with any flags */
    if(data.offset == 0) {
        CORRADE_COMPARE(scene.findFieldObjectOffset(0, data.object), Containers::NullOpt);
        CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Parent, data.object), Containers::NullOpt);
        CORRADE_VERIFY(!scene.hasFieldObject(0, data.object));
        CORRADE_VERIFY(!scene.hasFieldObject(SceneField::Parent, data.object));
    }

    CORRADE_COMPARE(scene.findFieldObjectOffset(1, data.object, data.offset), data.expected);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, data.object, data.offset), data.expected);
    if(data.offset == 0) {
        CORRADE_COMPARE(scene.hasFieldObject(1, data.object), !!data.expected);
        CORRADE_COMPARE(scene.hasFieldObject(SceneField::Mesh, data.object), !!data.expected);
    }

    if(data.expected) {
        CORRADE_COMPARE(scene.fieldObjectOffset(1, data.object, data.offset), *data.expected);
        CORRADE_COMPARE(scene.fieldObjectOffset(SceneField::Mesh, data.object, data.offset), *data.expected);
    }
}

void SceneDataTest::findFieldObjectOffsetInvalidOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.findFieldObjectOffset(0, 1, 4);
    scene.findFieldObjectOffset(SceneField::Mesh, 1, 4);
    scene.fieldObjectOffset(0, 1, 4);
    scene.fieldObjectOffset(SceneField::Mesh, 1, 4);
    CORRADE_COMPARE(out,
        "Trade::SceneData::findFieldObjectOffset(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::findFieldObjectOffset(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::fieldObjectOffset(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::fieldObjectOffset(): offset 4 out of range for a field of size 3\n");
}

void SceneDataTest::fieldObjectOffsetNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4},
        {0, 5},
        {2, 5}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        /* Test also with a completely empty field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.fieldObjectOffset(0, 4);
    scene.fieldObjectOffset(SceneField::Parent, 4);
    scene.fieldObjectOffset(1, 1, 2);
    scene.fieldObjectOffset(SceneField::Mesh, 1, 2);
    CORRADE_COMPARE(out,
        "Trade::SceneData::fieldObjectOffset(): object 4 not found in field Trade::SceneField::Parent starting at offset 0\n"
        "Trade::SceneData::fieldObjectOffset(): object 4 not found in field Trade::SceneField::Parent starting at offset 0\n"
        "Trade::SceneData::fieldObjectOffset(): object 1 not found in field Trade::SceneField::Mesh starting at offset 2\n"
        "Trade::SceneData::fieldObjectOffset(): object 1 not found in field Trade::SceneField::Mesh starting at offset 2\n");
}

template<class T> void SceneDataTest::mappingAsArrayByIndex() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        T object;
        UnsignedByte mesh;
    } fields[]{
        {T(15), 0},
        {T(37), 1},
        {T(44), 15}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{Implementation::sceneMappingTypeFor<T>(), 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, Implementation::sceneMappingTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    CORRADE_COMPARE_AS(scene.mappingAsArray(1),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

template<class T> void SceneDataTest::mappingAsArrayByName() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        T object;
        UnsignedByte mesh;
    } fields[]{
        {T(15), 0},
        {T(37), 1},
        {T(44), 15}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{Implementation::sceneMappingTypeFor<T>(), 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, Implementation::sceneMappingTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    CORRADE_COMPARE_AS(scene.mappingAsArray(SceneField::Mesh),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

void SceneDataTest::mappingIntoArrayByIndex() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[] {
        {15, 0},
        {37, 1},
        {44, 15}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.mappingInto(1, out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::object),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.mappingInto(1, data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::mappingIntoArrayByName() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[] {
        {15, 0},
        {37, 1},
        {44, 15}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.mappingInto(SceneField::Mesh, out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::object),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.mappingInto(SceneField::Mesh, data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::mappingIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedByte mesh;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    scene.mappingInto(0, destination);
    scene.mappingInto(SceneField::Mesh, destination);
    scene.mappingInto(0, 4, destination);
    scene.mappingInto(SceneField::Mesh, 4, destination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::mappingInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::mappingInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::mappingInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::mappingInto(): offset 4 out of range for a field of size 3\n");
}

template<class T> void SceneDataTest::parentsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        UnsignedByte object;
        T parent;
    } fields[]{
        {0, T(15)},
        {1, T(-1)},
        {15, T(44)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::UnsignedInt, nullptr},
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE_AS(scene.parentsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        {0, 15},
        {1, -1},
        {15, 44}
    })), TestSuite::Compare::Container);
}

void SceneDataTest::parentsIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[] {
        {1, 15},
        {0, -1},
        {4, 44}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Mesh, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr},
        SceneFieldData{SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Int field[3];
        scene.parentsInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::parent),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Int> field{data.size};
        CORRADE_COMPARE(scene.parentsInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::parent)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::parentsIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    Int fieldDestinationCorrect[3];
    Int fieldDestination[2];
    scene.parentsInto(mappingDestination, fieldDestinationCorrect);
    scene.parentsInto(mappingDestinationCorrect, fieldDestination);
    scene.parentsInto(4, mappingDestination, fieldDestination);
    scene.parentsInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::parentsInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::parentsInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::parentsInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::parentsInto(): mapping and field destination views have different size, 3 vs 2\n");
}

template<class T> struct TransformationTypeFor {
    typedef T Type;
};
template<class T> struct TransformationTypeFor<Math::Matrix3x2<T>> {
    typedef Math::Matrix3<T> Type;
};
template<class T> struct TransformationTypeFor<Math::Matrix4x3<T>> {
    typedef Math::Matrix4<T> Type;
};

template<class T> void SceneDataTest::transformations2DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type U;
    typedef typename TransformationTypeFor<T>::Type TT;

    struct Transformation {
        UnsignedInt object;
        T transformation;
    };

    struct Component {
        UnsignedInt object;
        Vector2 translation;
        Vector2 scaling;
    };

    Containers::StridedArrayView1D<Transformation> transformations;
    Containers::StridedArrayView1D<Component> components;
    Containers::Array<char> data = Containers::ArrayTuple{
        {NoInit, 4, transformations},
        {NoInit, 2, components}
    };
    transformations[0] = {1, T{TT::translation({U(3.0), U(2.0)})}};
    transformations[1] = {0, T{TT::rotation(Math::Deg<U>(35.0))}};
    transformations[2] = {4, T{TT::translation({U(1.5), U(2.5)})*
                               TT::rotation(Math::Deg<U>(-15.0))}};
    transformations[3] = {5, T{TT::rotation(Math::Deg<U>(-15.0))*
                               TT::translation({U(1.5), U(2.5)})}};
    /* Object number 4 additionally has a scaling component (which isn't
       representable with dual complex numbers). It currently doesn't get added
       to the transformations returned from transformations2DInto() but that
       may change in the future for dual complex numbers). The translation
       component is then *assumed* to be equivalent to what's stored in the
       Transformation field and so applied neither. Here it's different, and
       that shouldn't affect anything. */
    components[0] = {4, {-1.5f, -2.5f}, {2.0f, 5.0f}};
    /* This is deliberately an error -- specifying a TRS for an object that
       doesn't have a Transformation field. Since there's no fast way to check
       for those and error/warn on those, they get just ignored. */
    components[1] = {2, {3.5f, -1.0f}, {1.0f, 1.5f}};

    SceneData scene{SceneMappingType::UnsignedInt, 6, Utility::move(data), {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            transformations.slice(&Transformation::object),
            transformations.slice(&Transformation::transformation)},
        SceneFieldData{SceneField::Translation,
            components.slice(&Component::object),
            components.slice(&Component::translation)},
        SceneFieldData{SceneField::Scaling,
            components.slice(&Component::object),
            components.slice(&Component::scaling)},
    }};

    CORRADE_VERIFY(scene.is2D());
    CORRADE_VERIFY(!scene.is3D());
    CORRADE_COMPARE(scene.transformationFieldSize(), 4);
    CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
        {1, Matrix3::translation({3.0f, 2.0f})},
        {0, Matrix3::rotation(35.0_degf)},
        {4, Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation(-15.0_degf)},
        {5, Matrix3::rotation(-15.0_degf)*Matrix3::translation({1.5f, 2.5f})}
    })), TestSuite::Compare::Container);
}

template<class T, class U, class V> void SceneDataTest::transformations2DAsArrayTRS() {
    setTestCaseTemplateName({NameTraits<T>::name(), NameTraits<U>::name(), NameTraits<V>::name()});

    struct Field {
        UnsignedInt object;
        Math::Vector2<T> translation;
        Math::Complex<U> rotation;
        Math::Vector2<V> scaling;
    } fields[]{
        {1, {T(3.0), T(2.0)},
            {},
            {V(1.0), V(1.0)}},
        {0, {},
            Math::Complex<U>::rotation(Math::Deg<U>{U(35.0)}),
            {V(1.0), V(1.0)}},
        {2, {}, /* Identity transformation here */
            {},
            {V(1.0), V(1.0)}},
        {4, {},
            {},
            {V(2.0), V(1.0)}},
        {7, {T(1.5), T(2.5)},
            Math::Complex<U>::rotation(Math::Deg<U>{U(-15.0)}),
            {V(-0.5), V(4.0)}},
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneFieldData translation{SceneField::Translation,
        view.slice(&Field::object),
        view.slice(&Field::translation)};
    SceneFieldData rotation{SceneField::Rotation,
        view.slice(&Field::object),
        view.slice(&Field::rotation)};
    SceneFieldData scaling{SceneField::Scaling,
        view.slice(&Field::object),
        view.slice(&Field::scaling)};

    /* Just one of translation / rotation / scaling */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3::translation({3.0f, 2.0f})},
            {0, Matrix3{Math::IdentityInit}},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3{Math::IdentityInit}},
            {7, Matrix3::translation({1.5f, 2.5f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{3.0f, 2.0f}, {}, Vector2{1.0f}}},
            {0, {{}, {}, Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, Vector2{1.0f}}},
            {7, {{1.5f, 2.5f}, {}, Vector2{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            rotation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3{Math::IdentityInit}},
            {0, Matrix3::rotation(35.0_degf)},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3{Math::IdentityInit}},
            {7, Matrix3::rotation(-15.0_degf)}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{}, {}, Vector2{1.0f}}},
            {0, {{}, Complex::rotation(35.0_degf), Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, Vector2{1.0f}}},
            {7, {{}, Complex::rotation(-15.0_degf), Vector2{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3{Math::IdentityInit}},
            {0, Matrix3{Math::IdentityInit}},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3::scaling({2.0f, 1.0f})},
            {7, Matrix3::scaling({-0.5f, 4.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{}, {}, Vector2{1.0f}}},
            {0, {{}, {}, Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f}}},
            {7, {{}, {}, {-0.5f, 4.0f}}},
        })), TestSuite::Compare::Container);
    }

    /* Pairs */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3::translation({3.0f, 2.0f})},
            {0, Matrix3::rotation(35.0_degf)},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3{Math::IdentityInit}},
            {7, Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation({-15.0_degf})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{3.0f, 2.0f}, {}, Vector2{1.0f}}},
            {0, {{}, Complex::rotation(35.0_degf), Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, Vector2{1.0f}}},
            {7, {{1.5f, 2.5f}, Complex::rotation(-15.0_degf), Vector2{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3::translation({3.0f, 2.0f})},
            {0, Matrix3{Math::IdentityInit}},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3::scaling({2.0f, 1.0f})},
            {7, Matrix3::translation({1.5f, 2.5f})*Matrix3::scaling({-0.5f, 4.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{3.0f, 2.0f}, {}, Vector2{1.0f}}},
            {0, {{}, {}, Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f}}},
            {7, {{1.5f, 2.5f}, {}, {-0.5f, 4.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            rotation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3{Math::IdentityInit}},
            {0, Matrix3::rotation(35.0_degf)},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3::scaling({2.0f, 1.0f})},
            {7, Matrix3::rotation({-15.0_degf})*Matrix3::scaling({-0.5f, 4.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{}, {}, Vector2{1.0f}}},
            {0, {{}, Complex::rotation(35.0_degf), Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f}}},
            {7, {{}, Complex::rotation(-15.0_degf), {-0.5f, 4.0f}}},
        })), TestSuite::Compare::Container);
    }

    /* All */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix3>>({
            {1, Matrix3::translation({3.0f, 2.0f})},
            {0, Matrix3::rotation(35.0_degf)},
            {2, Matrix3{Math::IdentityInit}},
            {4, Matrix3::scaling({2.0f, 1.0f})},
            {7, Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation({-15.0_degf})*Matrix3::scaling({-0.5f, 4.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>>({
            {1, {{3.0f, 2.0f}, {}, Vector2{1.0f}}},
            {0, {{}, Complex::rotation(35.0_degf), Vector2{1.0f}}},
            {2, {{}, {}, Vector2{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f}}},
            {7, {{1.5f, 2.5f}, Complex::rotation(-15.0_degf), {-0.5f, 4.0f}}},
        })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DAsArrayBut3DType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Because TRSAsArray() allocates an Array<Triple> and then calls
       TRSInto(), which skips views that are nullptr, we wouldn't get the
       assertion for translations, as those are at offset 0, which would be
       interpreted as an empty view if there were no elements. Thus using
       rotations instead. */
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Rotation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Quaternion, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.transformations2DAsArray();
    scene.translationsRotationsScalings2DAsArray();
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformations2DInto(): scene has a 3D transformation type\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): scene has a 3D transformation type\n");
}

void SceneDataTest::transformations2DIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        Matrix3 transformation;
    } fields[] {
        {1, Matrix3::translation({3.0f, 2.0f})*Matrix3::scaling({1.5f, 2.0f})},
        {0, Matrix3::rotation(35.0_degf)},
        {4, Matrix3::translation({3.0f, 2.0f})*Matrix3::rotation(35.0_degf)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Matrix3 field[3];
        scene.transformations2DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::transformation),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Matrix3> field{data.size};
        CORRADE_COMPARE(scene.transformations2DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::transformation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DTRSIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Same APIs tested as in transformations2DIntoArray(), but with a TRS
       input */

    struct Field {
        UnsignedInt object;
        Vector2 translation;
        Complex rotation;
        Vector2 scaling;
    } fields[] {
        {1, {3.0f, 2.0f}, {}, {1.5f, 2.0f}},
        {0, {}, Complex::rotation(35.0_degf), {1.0f, 1.0f}},
        {4, {3.0f, 2.0f}, Complex::rotation(35.0_degf), {1.0f, 1.0f}}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Translation,
            view.slice(&Field::object),
            view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation,
            view.slice(&Field::object),
            view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling,
            view.slice(&Field::object),
            view.slice(&Field::scaling)},
    }};

    Matrix3 expected[]{
        Matrix3::translation({3.0f, 2.0f})*Matrix3::scaling({1.5f, 2.0f}),
        Matrix3::rotation(35.0_degf),
        Matrix3::translation({3.0f, 2.0f})*Matrix3::rotation(35.0_degf)
    };

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Matrix3 field[3];
        scene.transformations2DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            Containers::arrayView(expected),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Matrix3> field{data.size};
        CORRADE_COMPARE(scene.transformations2DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            Containers::arrayView(expected).slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DIntoArrayTRS() {
    auto&& data = IntoArrayOffset3Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        Vector2 translation;
        Complex rotation;
        Vector2 scaling;
    } fields[] {
        {1, {3.0f, 2.0f}, {}, {1.5f, 2.0f}},
        {0, {}, Complex::rotation(35.0_degf), {1.0f, 1.0f}},
        {4, {3.0f, 2.0f}, Complex::rotation(35.0_degf), {1.0f, 1.0f}}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Translation,
            view.slice(&Field::object),
            view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation,
            view.slice(&Field::object),
            view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling,
            view.slice(&Field::object),
            view.slice(&Field::scaling)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Vector2 translations[3];
        Complex rotations[3];
        Vector2 scalings[3];
        scene.translationsRotationsScalings2DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field1 ? Containers::arrayView(translations) : nullptr,
            data.field2 ? Containers::arrayView(rotations) : nullptr,
            data.field3 ? Containers::arrayView(scalings) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(Containers::stridedArrayView(translations),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(Containers::stridedArrayView(rotations),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);
        if(data.field3) CORRADE_COMPARE_AS(Containers::stridedArrayView(scalings),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Vector2> translations{data.size};
        Containers::Array<Complex> rotations{data.size};
        Containers::Array<Vector2> scalings{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field1 ? arrayView(translations) : nullptr,
            data.field2 ? arrayView(rotations) : nullptr,
            data.field3 ? arrayView(scalings) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(translations.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(rotations.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field3) CORRADE_COMPARE_AS(scalings.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        Matrix3 transformation;
    } fields[3]; /* GCC 4.8 ICEs if I do a {} here */

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    Matrix3 fieldDestinationCorrect[3];
    Matrix3 fieldDestination[2];
    scene.transformations2DInto(mappingDestination, fieldDestinationCorrect);
    scene.transformations2DInto(mappingDestinationCorrect, fieldDestination);
    scene.transformations2DInto(4, mappingDestination, fieldDestination);
    scene.transformations2DInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformations2DInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::transformations2DInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::transformations2DInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::transformations2DInto(): mapping and field destination views have different size, 3 vs 2\n");
}

void SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffsetTRS() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        Vector2 translation;
    } fields[3]; /* GCC 4.8 ICEs if I do a {} here */

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    Vector2 translationDestinationCorrect[3];
    Vector2 translationDestination[2];
    Complex rotationDestinationCorrect[3];
    Complex rotationDestination[2];
    Vector2 scalingDestinationCorrect[3];
    Vector2 scalingDestination[2];
    scene.translationsRotationsScalings2DInto(mappingDestination, translationDestinationCorrect, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings2DInto(mappingDestinationCorrect, translationDestination, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings2DInto(mappingDestinationCorrect, translationDestinationCorrect, rotationDestination, scalingDestinationCorrect);
    scene.translationsRotationsScalings2DInto(mappingDestinationCorrect, translationDestinationCorrect, rotationDestinationCorrect, scalingDestination);
    scene.translationsRotationsScalings2DInto(4, mappingDestination, translationDestination, rotationDestination, scalingDestination);
    scene.translationsRotationsScalings2DInto(0, mappingDestinationCorrect, translationDestination, nullptr, nullptr);
    scene.translationsRotationsScalings2DInto(0, mappingDestinationCorrect, nullptr, rotationDestination, nullptr);
    scene.translationsRotationsScalings2DInto(0, mappingDestinationCorrect, nullptr, nullptr, scalingDestination);
    scene.translationsRotationsScalings2DInto(0, nullptr, translationDestinationCorrect, rotationDestination, nullptr);
    scene.translationsRotationsScalings2DInto(0, nullptr, translationDestinationCorrect, nullptr, scalingDestination);
    scene.translationsRotationsScalings2DInto(0, nullptr, nullptr, rotationDestinationCorrect, scalingDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected translation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected rotation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected scaling destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and translation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and rotation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): mapping and scaling destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and rotation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and scaling destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): rotation and scaling destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::transformations3DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type U;
    typedef typename TransformationTypeFor<T>::Type TT;

    struct Transformation {
        UnsignedInt object;
        T transformation;
    };

    struct Component {
        UnsignedInt object;
        Vector3 translation;
        Vector3 scaling;
    };

    Containers::StridedArrayView1D<Transformation> transformations;
    Containers::StridedArrayView1D<Component> components;
    Containers::Array<char> data = Containers::ArrayTuple{
        {NoInit, 4, transformations},
        {NoInit, 2, components}
    };
    transformations[0] = {1, T{TT::translation({U(3.0), U(2.0), U(-0.5)})}};
    transformations[1] = {0, T{TT::rotation(Math::Deg<U>(35.0),
                                            Math::Vector3<U>::yAxis())}};
    transformations[2] = {4, T{TT::translation({U(1.5), U(2.5), U(0.75)})*
                               TT::rotation(Math::Deg<U>(-15.0),
                                            Math::Vector3<U>::xAxis())}};
    transformations[3] = {5, T{TT::rotation(Math::Deg<U>(-15.0),
                                            Math::Vector3<U>::xAxis())*
                               TT::translation({U(1.5), U(2.5), U(0.75)})}};
    /* Object number 4 additionally has a scaling component (which isn't
       representable with dual quaternions). It currently doesn't get added
       to the transformations returned from transformations2DInto() but that
       may change in the future for dual quaternions). The translation
       component is then *assumed* to be equivalent to what's stored in the
       Transformation field and so applied neither. Here it's different, and
       that shouldn't affect anything. */
    components[0] = {4, {-1.5f, -2.5f, 5.5f}, {2.0f, 5.0f, 3.0f}};
    /* This is deliberately an error -- specifying a TRS for an object that
       doesn't have a Transformation field. Since there's no fast way to check
       for those and error/warn on those, they get just ignored. */
    components[1] = {2, {3.5f, -1.0f, 2.2f}, {1.0f, 1.5f, 1.0f}};

    SceneData scene{SceneMappingType::UnsignedInt, 6, Utility::move(data), {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            transformations.slice(&Transformation::object),
            transformations.slice(&Transformation::transformation)},
        SceneFieldData{SceneField::Translation,
            components.slice(&Component::object),
            components.slice(&Component::translation)},
        SceneFieldData{SceneField::Scaling,
            components.slice(&Component::object),
            components.slice(&Component::scaling)},
    }};

    CORRADE_VERIFY(!scene.is2D());
    CORRADE_VERIFY(scene.is3D());
    CORRADE_COMPARE(scene.transformationFieldSize(), 4);
    CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
        {1, Matrix4::translation({3.0f, 2.0f, -0.5f})},
        {0, Matrix4::rotationY(35.0_degf)},
        {4, Matrix4::translation({1.5f, 2.5f, 0.75f})*Matrix4::rotationX(-15.0_degf)},
        {5, Matrix4::rotationX(-15.0_degf)*Matrix4::translation({1.5f, 2.5f, 0.75f})}
    })), TestSuite::Compare::Container);
}

template<class T, class U, class V> void SceneDataTest::transformations3DAsArrayTRS() {
    setTestCaseTemplateName({NameTraits<T>::name(), NameTraits<U>::name(), NameTraits<V>::name()});

    struct Field {
        UnsignedInt object;
        Math::Vector3<T> translation;
        Math::Quaternion<U> rotation;
        Math::Vector3<V> scaling;
    } fields[]{
        {1, {T(3.0), T(2.0), T(1.0)},
            {},
            {V(1.0), V(1.0), V(1.0)}},
        {0, {},
            Math::Quaternion<U>::rotation(Math::Deg<U>{U(35.0)}, Math::Vector3<U>::yAxis()),
            {V(1.0), V(1.0), V(1.0)}},
        {2, {}, /* Identity transformation here */
            {},
            {V(1.0), V(1.0), V(1.0)}},
        {4, {},
            {},
            {V(2.0), V(1.0), V(0.0)}},
        {7, {T(1.5), T(2.5), T(3.5)},
            Math::Quaternion<U>::rotation(Math::Deg<U>{U(-15.0)}, Math::Vector3<U>::xAxis()),
            {V(-0.5), V(4.0), V(-16.0)}},
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneFieldData translation{SceneField::Translation,
        view.slice(&Field::object),
        view.slice(&Field::translation)};
    SceneFieldData rotation{SceneField::Rotation,
        view.slice(&Field::object),
        view.slice(&Field::rotation)};
    SceneFieldData scaling{SceneField::Scaling,
        view.slice(&Field::object),
        view.slice(&Field::scaling)};

    /* Just one of translation / rotation / scaling */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4::translation({3.0f, 2.0, 1.0f})},
            {0, Matrix4{Math::IdentityInit}},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4{Math::IdentityInit}},
            {7, Matrix4::translation({1.5f, 2.5f, 3.5f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}}},
            {0, {{}, {}, Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, Vector3{1.0f}}},
            {7, {{1.5f, 2.5f, 3.5f}, {}, Vector3{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            rotation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4{Math::IdentityInit}},
            {0, Matrix4::rotationY(35.0_degf)},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4{Math::IdentityInit}},
            {7, Matrix4::rotationX(-15.0_degf)}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{}, {}, Vector3{1.0f}}},
            {0, {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, Vector3{1.0f}}},
            {7, {{}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), Vector3{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4{Math::IdentityInit}},
            {0, Matrix4{Math::IdentityInit}},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4::scaling({2.0f, 1.0f, 0.0f})},
            {7, Matrix4::scaling({-0.5f, 4.0f, -16.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{}, {}, Vector3{1.0f}}},
            {0, {{}, {}, Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f, 0.0f}}},
            {7, {{}, {}, {-0.5f, 4.0f, -16.0f}}},
        })), TestSuite::Compare::Container);
    }

    /* Pairs */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4::translation({3.0f, 2.0, 1.0f})},
            {0, Matrix4::rotationY(35.0_degf)},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4{Math::IdentityInit}},
            {7, Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::rotationX(-15.0_degf)}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}}},
            {0, {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, Vector3{1.0f}}},
            {7, {{1.5f, 2.5f, 3.5f}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), Vector3{1.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4::translation({3.0f, 2.0, 1.0f})},
            {0, Matrix4{Math::IdentityInit}},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4::scaling({2.0f, 1.0f, 0.0f})},
            {7, Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}}},
            {0, {{}, {}, Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f, 0.0f}}},
            {7, {{1.5f, 2.5f, 3.5f}, {}, {-0.5f, 4.0f, -16.0f}}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            rotation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4{Math::IdentityInit}},
            {0, Matrix4::rotationY(35.0_degf)},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4::scaling({2.0f, 1.0f, 0.0f})},
            {7, Matrix4::rotationX({-15.0_degf})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{}, {}, Vector3{1.0f}}},
            {0, {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f, 0.0f}}},
            {7, {{}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), {-0.5f, 4.0f, -16.0f}}},
        })), TestSuite::Compare::Container);
    }

    /* All */
    {
        SceneData scene{SceneMappingType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE(scene.transformationFieldSize(), 5);
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Matrix4>>({
            {1, Matrix4::translation({3.0f, 2.0, 1.0f})},
            {0, Matrix4::rotationY(35.0_degf)},
            {2, Matrix4{Math::IdentityInit}},
            {4, Matrix4::scaling({2.0f, 1.0f, 0.0f})},
            {7, Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::rotationX({-15.0_degf})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})}
        })), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>>({
            {1, {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}}},
            {0, {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}}},
            {2, {{}, {}, Vector3{1.0f}}},
            {4, {{}, {}, {2.0f, 1.0f, 0.0f}}},
            {7, {{1.5f, 2.5f, 3.5f}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), {-0.5f, 4.0f, -16.0f}}},
        })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DAsArrayBut2DType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Because TRSAsArray() allocates an Array<Triple> and then calls
       TRSInto(), which skips views that are nullptr, we wouldn't get the
       assertion for translations, as those are at offset 0, which would be
       interpreted as an empty view if there were no elements. Thus using
       rotations instead. */
    SceneData scene{SceneMappingType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Rotation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Complex, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.transformations3DAsArray();
    scene.translationsRotationsScalings3DAsArray();
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformations3DInto(): scene has a 2D transformation type\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): scene has a 2D transformation type\n");
}

void SceneDataTest::transformations3DIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        Matrix4 transformation;
    } fields[] {
        {1, Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::scaling({1.5f, 2.0f, 4.5f})},
        {0, Matrix4::rotationX(35.0_degf)},
        {4, Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::rotationX(35.0_degf)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Matrix4 field[3];
        scene.transformations3DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::transformation),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Matrix4> field{data.size};
        CORRADE_COMPARE(scene.transformations3DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::transformation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DTRSIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Same APIs tested as in transformations3DIntoArray(), but with a TRS
       input */

    struct Field {
        UnsignedInt object;
        Vector3 translation;
        Quaternion rotation;
        Vector3 scaling;
    } fields[] {
        {1, {3.0f, 2.0f, 1.0f}, {}, {1.5f, 2.0f, 4.5f}},
        {0, {}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}},
        {4, {3.0f, 2.0f, 1.0f}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Translation,
            view.slice(&Field::object),
            view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation,
            view.slice(&Field::object),
            view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling,
            view.slice(&Field::object),
            view.slice(&Field::scaling)},
    }};

    Matrix4 expected[]{
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::scaling({1.5f, 2.0f, 4.5f}),
        Matrix4::rotationX(35.0_degf),
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::rotationX(35.0_degf)
    };

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Matrix4 field[3];
        scene.transformations3DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            Containers::arrayView(expected),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Matrix4> field{data.size};
        CORRADE_COMPARE(scene.transformations3DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            Containers::arrayView(expected).slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DIntoArrayTRS() {
    auto&& data = IntoArrayOffset3Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        Vector3 translation;
        Quaternion rotation;
        Vector3 scaling;
    } fields[] {
        {1, {3.0f, 2.0f, 1.0f}, {}, {1.5f, 2.0f, 4.5f}},
        {0, {}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}},
        {4, {3.0f, 2.0f, 1.0f}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Translation,
            view.slice(&Field::object),
            view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation,
            view.slice(&Field::object),
            view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling,
            view.slice(&Field::object),
            view.slice(&Field::scaling)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        Vector3 translations[3];
        Quaternion rotations[3];
        Vector3 scalings[3];
        scene.translationsRotationsScalings3DInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field1 ? Containers::arrayView(translations) : nullptr,
            data.field2 ? Containers::arrayView(rotations) : nullptr,
            data.field3 ? Containers::arrayView(scalings) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(Containers::stridedArrayView(translations),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(Containers::stridedArrayView(rotations),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);
        if(data.field3) CORRADE_COMPARE_AS(Containers::stridedArrayView(scalings),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<Vector3> translations{data.size};
        Containers::Array<Quaternion> rotations{data.size};
        Containers::Array<Vector3> scalings{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field1 ? arrayView(translations) : nullptr,
            data.field2 ? arrayView(rotations) : nullptr,
            data.field3 ? arrayView(scalings) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(translations.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(rotations.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field3) CORRADE_COMPARE_AS(scalings.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        Matrix4 transformation;
    } fields[3]; /* GCC 4.8 ICEs if I do a {} here */

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    Matrix4 fieldDestinationCorrect[3];
    Matrix4 fieldDestination[2];
    scene.transformations3DInto(mappingDestination, fieldDestinationCorrect);
    scene.transformations3DInto(mappingDestinationCorrect, fieldDestination);
    scene.transformations3DInto(4, mappingDestination, fieldDestination);
    scene.transformations3DInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformations3DInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::transformations3DInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::transformations3DInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::transformations3DInto(): mapping and field destination views have different size, 3 vs 2\n");
}

void SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffsetTRS() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        Vector2 translation;
    } fields[3]; /* GCC 4.8 ICEs if I do a {} here */

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    Vector3 translationDestinationCorrect[3];
    Vector3 translationDestination[2];
    Quaternion rotationDestinationCorrect[3];
    Quaternion rotationDestination[2];
    Vector3 scalingDestinationCorrect[3];
    Vector3 scalingDestination[2];
    scene.translationsRotationsScalings3DInto(mappingDestination, translationDestinationCorrect, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings3DInto(mappingDestinationCorrect, translationDestination, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings3DInto(mappingDestinationCorrect, translationDestinationCorrect, rotationDestination, scalingDestinationCorrect);
    scene.translationsRotationsScalings3DInto(mappingDestinationCorrect, translationDestinationCorrect, rotationDestinationCorrect, scalingDestination);
    scene.translationsRotationsScalings3DInto(4, mappingDestination, translationDestination, rotationDestination, scalingDestination);
    scene.translationsRotationsScalings3DInto(0, mappingDestinationCorrect, translationDestination, nullptr, nullptr);
    scene.translationsRotationsScalings3DInto(0, mappingDestinationCorrect, nullptr, rotationDestination, nullptr);
    scene.translationsRotationsScalings3DInto(0, mappingDestinationCorrect, nullptr, nullptr, scalingDestination);
    scene.translationsRotationsScalings3DInto(0, nullptr, translationDestinationCorrect, rotationDestination, nullptr);
    scene.translationsRotationsScalings3DInto(0, nullptr, translationDestinationCorrect, nullptr, scalingDestination);
    scene.translationsRotationsScalings3DInto(0, nullptr, nullptr, rotationDestinationCorrect, scalingDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected translation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected rotation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected scaling destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and translation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and rotation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): mapping and scaling destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and rotation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): translation and scaling destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): rotation and scaling destination views have different size, 3 vs 2\n");
}

template<class T, class U> void SceneDataTest::meshesMaterialsAsArray() {
    setTestCaseTemplateName({NameTraits<T>::name(), NameTraits<U>::name()});

    struct Field {
        UnsignedByte object;
        T mesh;
        U meshMaterial;
    } fields[]{
        {0, T(15), U(3)},
        {1, T(37), U(-1)},
        {15, T(44), U(25)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneFieldData meshes{SceneField::Mesh,
        view.slice(&Field::object),
        view.slice(&Field::mesh)};
    SceneFieldData meshMaterials{SceneField::MeshMaterial,
        view.slice(&Field::object),
        view.slice(&Field::meshMaterial)};

    /* Both meshes and materials */
    {
        SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
            /* To verify it isn't just picking the first ever field */
            SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
            meshes,
            meshMaterials
        }};

        CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>({
            {0, {15, 3}},
            {1, {37, -1}},
            {15, {44, 25}}
        })), TestSuite::Compare::Container);

    /* Only meshes */
    } {
        SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
            meshes
        }};

        CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>({
            {0, {15, -1}},
            {1, {37, -1}},
            {15, {44, -1}}
        })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::meshesMaterialsIntoArray() {
    auto&& data = IntoArrayOffset2Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
        Int meshMaterial;
    } fields[]{
        {1, 15, 3},
        {0, 37, -1},
        {4, 44, 22}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
        SceneFieldData{SceneField::MeshMaterial,
            view.slice(&Field::object),
            view.slice(&Field::meshMaterial)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        UnsignedInt meshes[3];
        Int meshMaterials[3];
        scene.meshesMaterialsInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field1 ? Containers::arrayView(meshes) : nullptr,
            data.field2 ? Containers::arrayView(meshMaterials) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(Containers::stridedArrayView(meshes),
            view.slice(&Field::mesh),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(Containers::stridedArrayView(meshMaterials),
            view.slice(&Field::meshMaterial),
            TestSuite::Compare::Container);

    /* The offset variant should give back only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<UnsignedInt> meshes{data.size};
        Containers::Array<Int> meshMaterials{data.size};
        CORRADE_COMPARE(scene.meshesMaterialsInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field1 ? arrayView(meshes) : nullptr,
            data.field2 ? arrayView(meshMaterials) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field1) CORRADE_COMPARE_AS(meshes.prefix(data.expectedSize),
            view.slice(&Field::mesh)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field2) CORRADE_COMPARE_AS(meshMaterials.prefix(data.expectedSize),
            view.slice(&Field::meshMaterial)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::meshesMaterialsIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    UnsignedInt meshDestinationCorrect[3];
    UnsignedInt meshDestination[2];
    Int meshMaterialDestinationCorrect[3];
    Int meshMaterialDestination[2];
    scene.meshesMaterialsInto(mappingDestination, meshDestinationCorrect, meshMaterialDestinationCorrect);
    scene.meshesMaterialsInto(mappingDestinationCorrect, meshDestination, meshMaterialDestinationCorrect);
    scene.meshesMaterialsInto(mappingDestinationCorrect, meshDestinationCorrect, meshMaterialDestination);
    scene.meshesMaterialsInto(4, mappingDestination, meshDestination, meshMaterialDestination);
    scene.meshesMaterialsInto(0, mappingDestinationCorrect, meshDestination, nullptr);
    scene.meshesMaterialsInto(0, mappingDestinationCorrect, nullptr, meshMaterialDestination);
    scene.meshesMaterialsInto(0, nullptr, meshDestinationCorrect, meshMaterialDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::meshesMaterialsInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::meshesMaterialsInto(): expected mesh destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::meshesMaterialsInto(): expected mesh material destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::meshesMaterialsInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::meshesMaterialsInto(): mapping and mesh destination views have different size, 3 vs 2\n"
        "Trade::SceneData::meshesMaterialsInto(): mapping and mesh material destination views have different size, 3 vs 2\n"
        "Trade::SceneData::meshesMaterialsInto(): mesh and mesh material destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::lightsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        UnsignedByte object;
        T light;
    } fields[]{
        {0, T(15)},
        {1, T(37)},
        {15, T(44)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Light, view.slice(&Field::object), view.slice(&Field::light)}
    }};

    CORRADE_COMPARE_AS(scene.lightsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {0, 15},
        {1, 37},
        {15, 44}
    })), TestSuite::Compare::Container);
}

void SceneDataTest::lightsIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt light;
    } fields[] {
        {1, 15},
        {0, 37},
        {4, 44}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Light,
            view.slice(&Field::object),
            view.slice(&Field::light)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        UnsignedInt field[3];
        scene.lightsInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::light),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<UnsignedInt> field{data.size};
        CORRADE_COMPARE(scene.lightsInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::light)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::lightsIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt light;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Light, view.slice(&Field::object), view.slice(&Field::light)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    UnsignedInt fieldDestinationCorrect[3];
    UnsignedInt fieldDestination[2];
    scene.lightsInto(mappingDestination, fieldDestinationCorrect);
    scene.lightsInto(mappingDestinationCorrect, fieldDestination);
    scene.lightsInto(4, mappingDestination, fieldDestination);
    scene.lightsInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::lightsInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::lightsInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::lightsInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::lightsInto(): mapping and field destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::camerasAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        UnsignedByte object;
        T camera;
    } fields[]{
        {0, T(15)},
        {1, T(37)},
        {15, T(44)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Camera, view.slice(&Field::object), view.slice(&Field::camera)}
    }};

    CORRADE_COMPARE_AS(scene.camerasAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {0, 15},
        {1, 37},
        {15, 44}
    })), TestSuite::Compare::Container);
}

void SceneDataTest::camerasIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt camera;
    } fields[]{
        {1, 15},
        {0, 37},
        {4, 44}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Camera,
            view.slice(&Field::object),
            view.slice(&Field::camera)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        UnsignedInt field[3];
        scene.camerasInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::camera),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<UnsignedInt> field{data.size};
        CORRADE_COMPARE(scene.camerasInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::camera)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::camerasIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt camera;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Camera, view.slice(&Field::object), view.slice(&Field::camera)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    UnsignedInt fieldDestinationCorrect[3];
    UnsignedInt fieldDestination[2];
    scene.camerasInto(mappingDestination, fieldDestinationCorrect);
    scene.camerasInto(mappingDestinationCorrect, fieldDestination);
    scene.camerasInto(4, mappingDestination, fieldDestination);
    scene.camerasInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::camerasInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::camerasInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::camerasInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::camerasInto(): mapping and field destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::skinsAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    struct Field {
        UnsignedByte object;
        T skin;
    } fields[]{
        {0, T(15)},
        {1, T(37)},
        {15, T(44)}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field; also to
           satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin, view.slice(&Field::object), view.slice(&Field::skin)}
    }};

    CORRADE_COMPARE_AS(scene.skinsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {0, 15},
        {1, 37},
        {15, 44}
    })), TestSuite::Compare::Container);
}

void SceneDataTest::skinsIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    struct Field {
        UnsignedInt object;
        UnsignedInt skin;
    } fields[] {
        {1, 15},
        {0, 37},
        {4, 44}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field; also to
           satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin,
            view.slice(&Field::object),
            view.slice(&Field::skin)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        UnsignedInt field[3];
        scene.skinsInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::skin),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<UnsignedInt> field{data.size};
        CORRADE_COMPARE(scene.skinsInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::skin)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::skinsIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt skin;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin, view.slice(&Field::object), view.slice(&Field::skin)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    UnsignedInt fieldDestinationCorrect[3];
    UnsignedInt fieldDestination[2];
    scene.skinsInto(mappingDestination, fieldDestinationCorrect);
    scene.skinsInto(mappingDestinationCorrect, fieldDestination);
    scene.skinsInto(4, mappingDestination, fieldDestination);
    scene.skinsInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::skinsInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::skinsInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::skinsInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::skinsInto(): mapping and field destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::importerStateAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    int a, b;

    struct Field {
        UnsignedByte object;
        T importerState;
    } fields[]{
        {0, &a},
        {1, nullptr},
        {15, &b}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::ImporterState, view.slice(&Field::object), view.slice(&Field::importerState)}
    }};

    CORRADE_COMPARE_AS(scene.importerStateAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, const void*>>({
        {0, &a},
        {1, nullptr},
        {15, &b}
    })), TestSuite::Compare::Container);
}

void SceneDataTest::importerStateIntoArray() {
    auto&& data = IntoArrayOffset1Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Both AsArray() and Into() share a common helper. The AsArray() test
       above verified handling of various data types and this checks the
       offset/size parameters of the Into() variant. */

    int a, b;

    struct Field {
        UnsignedInt object;
        const void* importerState;
    } fields[]{
        {1, &a},
        {0, nullptr},
        {4, &b}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::ImporterState,
            view.slice(&Field::object),
            view.slice(&Field::importerState)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt mapping[3];
        const void* field[3];
        scene.importerStateInto(
            data.mapping ? Containers::arrayView(mapping) : nullptr,
            data.field ? Containers::arrayView(field) : nullptr
        );
        if(data.mapping) CORRADE_COMPARE_AS(Containers::stridedArrayView(mapping),
            view.slice(&Field::object),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(Containers::stridedArrayView(field),
            view.slice(&Field::importerState),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> mapping{data.size};
        Containers::Array<const void*> field{data.size};
        CORRADE_COMPARE(scene.importerStateInto(data.offset,
            data.mapping ? arrayView(mapping) : nullptr,
            data.field ? arrayView(field) : nullptr
        ), data.expectedSize);
        if(data.mapping) CORRADE_COMPARE_AS(mapping.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        if(data.field) CORRADE_COMPARE_AS(field.prefix(data.expectedSize),
            view.slice(&Field::importerState)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::importerStateIntoArrayInvalidSizeOrOffset() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        const void* importerState;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::ImporterState, view.slice(&Field::object), view.slice(&Field::importerState)}
    }};

    Containers::String out;
    Error redirectError{&out};
    UnsignedInt mappingDestinationCorrect[3];
    UnsignedInt mappingDestination[2];
    const void* fieldDestinationCorrect[3];
    const void* fieldDestination[2];
    scene.importerStateInto(mappingDestination, fieldDestinationCorrect);
    scene.importerStateInto(mappingDestinationCorrect, fieldDestination);
    scene.importerStateInto(4, mappingDestination, fieldDestination);
    scene.importerStateInto(0, mappingDestinationCorrect, fieldDestination);
    CORRADE_COMPARE(out,
        "Trade::SceneData::importerStateInto(): expected mapping destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::importerStateInto(): expected field destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::importerStateInto(): offset 4 out of range for a field of size 3\n"
        "Trade::SceneData::importerStateInto(): mapping and field destination views have different size, 3 vs 2\n");
}

void SceneDataTest::mutableAccessNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{sceneFieldCustom(35),
            view.slice(&Field::object),
            view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.mutableData();
    scene.mutableMapping(0);
    scene.mutableMapping<UnsignedInt>(0);
    scene.mutableMapping(SceneField::Mesh);
    scene.mutableMapping<UnsignedInt>(SceneField::Mesh);
    scene.mutableField(0);
    scene.mutableField<UnsignedInt>(0);
    scene.mutableField<UnsignedInt[]>(1);
    scene.mutableField(SceneField::Mesh);
    scene.mutableField<UnsignedInt>(SceneField::Mesh);
    scene.mutableField<UnsignedInt[]>(sceneFieldCustom(35));
    CORRADE_COMPARE(out,
        "Trade::SceneData::mutableData(): data not mutable\n"
        "Trade::SceneData::mutableMapping(): data not mutable\n"
        "Trade::SceneData::mutableMapping(): data not mutable\n"
        "Trade::SceneData::mutableMapping(): data not mutable\n"
        "Trade::SceneData::mutableMapping(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n");
}

void SceneDataTest::mappingNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.mapping(2);
    scene.mapping<UnsignedInt>(2);
    scene.mutableMapping(2);
    scene.mutableMapping<UnsignedInt>(2);
    scene.mapping(sceneFieldCustom(666));
    scene.mapping<UnsignedInt>(sceneFieldCustom(666));
    scene.mutableMapping(sceneFieldCustom(666));
    scene.mutableMapping<UnsignedInt>(sceneFieldCustom(666));

    scene.mappingAsArray(2);
    scene.mappingAsArray(sceneFieldCustom(666));
    CORRADE_COMPARE(out,
        "Trade::SceneData::mapping(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mapping(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableMapping(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableMapping(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mapping(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mapping(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableMapping(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableMapping(): field Trade::SceneField::Custom(666) not found\n"

        "Trade::SceneData::mappingInto(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mappingInto(): field Trade::SceneField::Custom(666) not found\n");
}

void SceneDataTest::mappingWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedShort object;
        UnsignedShort foobar;
        UnsignedInt mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedShort, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.mapping<UnsignedByte>(1);
    scene.mutableMapping<UnsignedByte>(1);
    scene.mapping<UnsignedByte>(SceneField::Mesh);
    scene.mutableMapping<UnsignedByte>(SceneField::Mesh);
    CORRADE_COMPARE(out,
        "Trade::SceneData::mapping(): mapping is Trade::SceneMappingType::UnsignedShort but requested Trade::SceneMappingType::UnsignedByte\n"
        "Trade::SceneData::mutableMapping(): mapping is Trade::SceneMappingType::UnsignedShort but requested Trade::SceneMappingType::UnsignedByte\n"
        "Trade::SceneData::mapping(): mapping is Trade::SceneMappingType::UnsignedShort but requested Trade::SceneMappingType::UnsignedByte\n"
        "Trade::SceneData::mutableMapping(): mapping is Trade::SceneMappingType::UnsignedShort but requested Trade::SceneMappingType::UnsignedByte\n");
}

void SceneDataTest::fieldNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt foo, bar;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(34), view.slice(&Field::object), view.slice(&Field::foo)},
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::bar)},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.findFieldObjectOffset(2, 0);
    scene.fieldObjectOffset(2, 0);
    scene.hasFieldObject(2, 0);
    scene.fieldData(2);
    scene.fieldName(2);
    scene.fieldFlags(2);
    scene.fieldType(2);
    scene.fieldSize(2);
    scene.fieldArraySize(2);
    scene.field(2);
    scene.field<UnsignedInt>(2);
    scene.field<UnsignedInt[]>(2);
    scene.mutableField(2);
    scene.mutableField<UnsignedInt>(2);
    scene.mutableField<UnsignedInt[]>(2);

    scene.fieldBits(2);
    scene.fieldBitArrays(2);
    scene.mutableFieldBits(2);
    scene.mutableFieldBitArrays(2);

    scene.fieldStringData(2);
    scene.fieldStrings(2);

    scene.fieldId(sceneFieldCustom(666));
    scene.fieldFlags(sceneFieldCustom(666));
    scene.findFieldObjectOffset(sceneFieldCustom(666), 0);
    scene.fieldObjectOffset(sceneFieldCustom(666), 0);
    scene.hasFieldObject(sceneFieldCustom(666), 0);
    scene.fieldType(sceneFieldCustom(666));
    scene.fieldSize(sceneFieldCustom(666));
    scene.fieldArraySize(sceneFieldCustom(666));
    scene.field(sceneFieldCustom(666));
    scene.field<UnsignedInt>(sceneFieldCustom(666));
    scene.field<UnsignedInt[]>(sceneFieldCustom(666));
    scene.mutableField(sceneFieldCustom(666));
    scene.mutableField<UnsignedInt>(sceneFieldCustom(666));
    scene.mutableField<UnsignedInt[]>(sceneFieldCustom(666));

    scene.fieldBits(sceneFieldCustom(666));
    scene.fieldBitArrays(sceneFieldCustom(666));
    scene.mutableFieldBits(sceneFieldCustom(666));
    scene.mutableFieldBitArrays(sceneFieldCustom(666));

    scene.fieldStringData(sceneFieldCustom(666));
    scene.fieldStrings(sceneFieldCustom(666));

    scene.parentsAsArray();
    scene.parentsInto(nullptr, nullptr);
    scene.parentsInto(0, nullptr, nullptr);
    scene.transformationFieldSize();
    scene.transformations2DAsArray();
    scene.transformations2DInto(nullptr, nullptr);
    scene.transformations2DInto(0, nullptr, nullptr);
    scene.translationsRotationsScalings2DAsArray();
    scene.translationsRotationsScalings2DInto(nullptr, nullptr, nullptr, nullptr);
    scene.translationsRotationsScalings2DInto(0, nullptr, nullptr, nullptr, nullptr);
    scene.transformations3DAsArray();
    scene.transformations3DInto(nullptr, nullptr);
    scene.transformations3DInto(0, nullptr, nullptr);
    scene.translationsRotationsScalings3DAsArray();
    scene.translationsRotationsScalings3DInto(nullptr, nullptr, nullptr, nullptr);
    scene.translationsRotationsScalings3DInto(0, nullptr, nullptr, nullptr, nullptr);
    scene.meshesMaterialsAsArray();
    scene.meshesMaterialsInto(nullptr, nullptr, nullptr);
    scene.meshesMaterialsInto(0, nullptr, nullptr, nullptr);
    scene.lightsAsArray();
    scene.lightsInto(nullptr, nullptr);
    scene.lightsInto(0, nullptr, nullptr);
    scene.camerasAsArray();
    scene.camerasInto(nullptr, nullptr);
    scene.camerasInto(0, nullptr, nullptr);
    scene.skinsAsArray();
    scene.skinsInto(nullptr, nullptr);
    scene.skinsInto(0, nullptr, nullptr);
    scene.importerStateAsArray();
    scene.importerStateInto(nullptr, nullptr);
    scene.importerStateInto(0, nullptr, nullptr);
    CORRADE_COMPARE_AS(out,
        "Trade::SceneData::findFieldObjectOffset(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldObjectOffset(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::hasFieldObject(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldData(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldName(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldFlags(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldType(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldSize(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldArraySize(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"

        "Trade::SceneData::fieldBits(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldBitArrays(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableFieldBits(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableFieldBitArrays(): index 2 out of range for 2 fields\n"

        "Trade::SceneData::fieldStringData(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldStrings(): index 2 out of range for 2 fields\n"

        "Trade::SceneData::fieldId(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldFlags(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::findFieldObjectOffset(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldObjectOffset(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::hasFieldObject(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldType(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldSize(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldArraySize(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::field(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::field(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::field(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableField(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableField(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableField(): field Trade::SceneField::Custom(666) not found\n"

        "Trade::SceneData::fieldBits(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldBitArrays(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableFieldBits(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableFieldBitArrays(): field Trade::SceneField::Custom(666) not found\n"

        "Trade::SceneData::fieldStringData(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::fieldStrings(): field Trade::SceneField::Custom(666) not found\n"

        /* AsArray() and Into() each share a common helper but have different
           top-level code paths. They however have the same assertion messages
           to save binary size a bit. */
        "Trade::SceneData::parentsInto(): field not found\n"
        "Trade::SceneData::parentsInto(): field not found\n"
        "Trade::SceneData::parentsInto(): field not found\n"
        "Trade::SceneData::transformationFieldSize(): no transformation-related field found\n"
        "Trade::SceneData::transformations2DInto(): no transformation-related field found\n"
        "Trade::SceneData::transformations2DInto(): no transformation-related field found\n"
        "Trade::SceneData::transformations2DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): no transformation-related field found\n"
        "Trade::SceneData::transformations3DInto(): no transformation-related field found\n"
        "Trade::SceneData::transformations3DInto(): no transformation-related field found\n"
        "Trade::SceneData::transformations3DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): no transformation-related field found\n"
        "Trade::SceneData::meshesMaterialsInto(): field Trade::SceneField::Mesh not found\n"
        "Trade::SceneData::meshesMaterialsInto(): field Trade::SceneField::Mesh not found\n"
        "Trade::SceneData::meshesMaterialsInto(): field Trade::SceneField::Mesh not found\n"
        "Trade::SceneData::lightsInto(): field not found\n"
        "Trade::SceneData::lightsInto(): field not found\n"
        "Trade::SceneData::lightsInto(): field not found\n"
        "Trade::SceneData::camerasInto(): field not found\n"
        "Trade::SceneData::camerasInto(): field not found\n"
        "Trade::SceneData::camerasInto(): field not found\n"
        "Trade::SceneData::skinsInto(): field not found\n"
        "Trade::SceneData::skinsInto(): field not found\n"
        "Trade::SceneData::skinsInto(): field not found\n"
        "Trade::SceneData::importerStateInto(): field not found\n"
        "Trade::SceneData::importerStateInto(): field not found\n"
        "Trade::SceneData::importerStateInto(): field not found\n",
        TestSuite::Compare::String);
}

void SceneDataTest::fieldWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
        bool yes;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
        SceneFieldData{sceneFieldCustom(773), view.slice(&Field::object), view.slice(&Field::yes).sliceBit(0)},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.field(2);
    scene.mutableField(2);

    scene.field<UnsignedByte>(1);
    scene.field<UnsignedByte[]>(1);
    scene.mutableField<UnsignedByte>(1);
    scene.mutableField<UnsignedByte[]>(1);
    /* Accessing SceneFieldType::Bit through field<T>(2) etc. is handled by the
       same code path as accessing via a mismatched type, no need to test those
       explicitly */

    scene.fieldBits(1);
    scene.fieldBitArrays(1);
    scene.mutableFieldBits(1);
    scene.mutableFieldBitArrays(1);

    scene.fieldStringData(1);
    scene.fieldStrings(1);

    scene.field<UnsignedByte>(SceneField::Mesh);
    scene.field<UnsignedByte[]>(SceneField::Mesh);
    scene.mutableField<UnsignedByte>(SceneField::Mesh);
    scene.mutableField<UnsignedByte[]>(SceneField::Mesh);

    scene.field(sceneFieldCustom(773));
    scene.mutableField(sceneFieldCustom(773));
    /* Accessing SceneFieldType::Bit through field<T>(2) etc. is handled by the
       same code path as accessing via a mismatched type, no need to test those
       explicitly */

    scene.fieldBits(SceneField::Mesh);
    scene.fieldBitArrays(SceneField::Mesh);
    scene.mutableFieldBits(SceneField::Mesh);
    scene.mutableFieldBitArrays(SceneField::Mesh);

    scene.fieldStringData(SceneField::Mesh);
    scene.fieldStrings(SceneField::Mesh);

    CORRADE_COMPARE(out,
        "Trade::SceneData::field(): Trade::SceneField::Custom(773) is Trade::SceneFieldType::Bit, use fieldBits() or fieldBitArrays() to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(773) is Trade::SceneFieldType::Bit, use mutableFieldBits() or mutableFieldBitArrays() to access it\n"

        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"

        "Trade::SceneData::fieldBits(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::fieldBitArrays(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::mutableFieldBits(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::mutableFieldBitArrays(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"

        "Trade::SceneData::fieldStringData(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a string\n"
        "Trade::SceneData::fieldStrings(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a string\n"

        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"

        "Trade::SceneData::field(): Trade::SceneField::Custom(773) is Trade::SceneFieldType::Bit, use fieldBits() or fieldBitArrays() to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(773) is Trade::SceneFieldType::Bit, use mutableFieldBits() or mutableFieldBitArrays() to access it\n"

        "Trade::SceneData::fieldBits(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::fieldBitArrays(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::mutableFieldBits(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"
        "Trade::SceneData::mutableFieldBitArrays(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a bit\n"

        "Trade::SceneData::fieldStringData(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a string\n"
        "Trade::SceneData::fieldStrings(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort, not a string\n");
}

void SceneDataTest::fieldWrongPointerType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Thing {
        UnsignedInt object;
        Int* foobar;
        const Int* importerState;
    } things[2];
    Containers::StridedArrayView1D<Thing> view = things;

    SceneData scene{SceneMappingType::UnsignedInt, 5, DataFlag::Mutable, things, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Thing::object), Containers::arrayCast<2, Int*>(view.slice(&Thing::foobar))},
        SceneFieldData{SceneField::ImporterState, view.slice(&Thing::object), view.slice(&Thing::importerState)},
    }};

    /* These are fine (type is not checked) */
    scene.field<Float*[]>(0);
    scene.field<const Float*>(1);
    scene.mutableField<Float*[]>(0);
    scene.mutableField<const Float*>(1);
    scene.field<Float*[]>(sceneFieldCustom(35));
    scene.field<const Float*>(SceneField::ImporterState);
    scene.mutableField<Float*[]>(sceneFieldCustom(35));
    scene.mutableField<const Float*>(SceneField::ImporterState);

    Containers::String out;
    Error redirectError{&out};
    scene.field<Int>(0);
    scene.field<const Int*>(0);
    scene.field<const Int*[]>(0);
    scene.field<Int*>(1);
    scene.field<Int*[]>(1);
    scene.mutableField<Int>(0);
    scene.mutableField<const Int*>(0);
    scene.mutableField<const Int*[]>(0);
    scene.mutableField<Int*>(1);
    scene.mutableField<Int*[]>(1);
    scene.field<Int>(sceneFieldCustom(35));
    scene.field<const Int*>(sceneFieldCustom(35));
    scene.field<const Int*[]>(sceneFieldCustom(35));
    scene.field<Int*>(SceneField::ImporterState);
    scene.field<Int*>(SceneField::ImporterState);
    scene.mutableField<Int>(sceneFieldCustom(35));
    scene.mutableField<const Int*>(sceneFieldCustom(35));
    scene.mutableField<const Int*[]>(sceneFieldCustom(35));
    scene.mutableField<Int*>(SceneField::ImporterState);
    scene.mutableField<Int*[]>(SceneField::ImporterState);
    CORRADE_COMPARE(out,
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Int\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::field(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::field(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Int\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Int\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::field(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::field(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Int\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is Trade::SceneFieldType::MutablePointer but requested a type equivalent to Trade::SceneFieldType::Pointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::ImporterState is Trade::SceneFieldType::Pointer but requested a type equivalent to Trade::SceneFieldType::MutablePointer\n");
}

void SceneDataTest::fieldWrongArrayAccess() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct Field {
        UnsignedInt object;
        UnsignedInt foobar;
        bool yes;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), Containers::arrayCast<2, UnsignedInt>(view.slice(&Field::foobar))},
        SceneFieldData{sceneFieldCustom(773), view.slice(&Field::object), Containers::StridedBitArrayView2D{Containers::BitArrayView{fields}, &fields[0].yes, 0, {2, 3}, {sizeof(Field)*8, 1}}},
    }};

    /* Array access is allowed for non-array fields (the second dimension is
       then always 1), tested directly in construct() and constructBit() */

    Containers::String out;
    Error redirectError{&out};
    scene.field<UnsignedInt>(0);
    scene.mutableField<UnsignedInt>(0);
    scene.field<UnsignedInt>(sceneFieldCustom(35));
    scene.mutableField<UnsignedInt>(sceneFieldCustom(35));
    scene.fieldBits(1);
    scene.mutableFieldBits(1);
    scene.fieldBits(sceneFieldCustom(773));
    scene.mutableFieldBits(sceneFieldCustom(773));
    CORRADE_COMPARE(out,
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::fieldBits(): Trade::SceneField::Custom(773) is an array field, use fieldBitArrays() to access it\n"
        "Trade::SceneData::mutableFieldBits(): Trade::SceneField::Custom(773) is an array field, use fieldBitArrays() to access it\n"
        "Trade::SceneData::fieldBits(): Trade::SceneField::Custom(773) is an array field, use fieldBitArrays() to access it\n"
        "Trade::SceneData::mutableFieldBits(): Trade::SceneField::Custom(773) is an array field, use fieldBitArrays() to access it\n");
}

void SceneDataTest::parentFor() {
    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {3, -1},
        {4, 3},
        {2, 4},
        {4, 2} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE(scene.parentFor(2), 4);
    CORRADE_COMPARE(scene.parentFor(3), -1);

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.parentFor(4), 3);

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.parentFor(1), Containers::NullOpt);
}

void SceneDataTest::parentForTrivialParent() {
    /* Going a bit overboard with the arrays, it makes the view creation below
       easier tho */
    struct Field {
        UnsignedInt object[4];
        Int parent[1];
    } fields[]{{
        {3, 4, 2, 4 /* duplicate, ignored */}, {-1}
    }};

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Parent,
            Containers::stridedArrayView(fields->object), Containers::stridedArrayView(fields->parent).broadcasted<0>(4)}
    }};

    CORRADE_COMPARE(scene.parentFor(2), -1);
    CORRADE_COMPARE(scene.parentFor(3), -1);

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.parentFor(4), -1);

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.parentFor(1), Containers::NullOpt);
}

void SceneDataTest::childrenFor() {
    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {4, -1},
        {3, 4},
        {2, 3},
        {1, 4},
        {5, 4},
        {0, -1},
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    /* Just one child */
    CORRADE_COMPARE_AS(scene.childrenFor(3),
        Containers::arrayView<UnsignedLong>({2}),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.childrenFor(-1),
        Containers::arrayView<UnsignedLong>({4, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.childrenFor(4),
        Containers::arrayView<UnsignedLong>({3, 1, 5}),
        TestSuite::Compare::Container);

    /* Object that is present in the parent array but has no children */
    CORRADE_COMPARE_AS(scene.childrenFor(5),
        Containers::arrayView<UnsignedLong>({}),
        TestSuite::Compare::Container);

    /* Object that is not in the parent array at all */
    CORRADE_COMPARE_AS(scene.childrenFor(6),
        Containers::arrayView<UnsignedLong>({}),
        TestSuite::Compare::Container);
}

void SceneDataTest::childrenForTrivialParent() {
    /* Going a bit overboard with the arrays, it makes the view creation below
       easier tho */
    struct Field {
        UnsignedInt mapping[4];
        Int parent[1];
    } fields[]{{
        {3, 4, 2, 4 /* duplicate, gets put to the output */}, {-1}
    }};

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Parent,
            Containers::stridedArrayView(fields->mapping), Containers::stridedArrayView(fields->parent).broadcasted<0>(4)}
    }};

    /* Trivial children */
    CORRADE_COMPARE_AS(scene.childrenFor(-1),
        Containers::arrayView<UnsignedLong>({3, 4, 2, 4}),
        TestSuite::Compare::Container);

    /* Object that is present in the parent array but has no children */
    CORRADE_COMPARE_AS(scene.childrenFor(4),
        Containers::arrayView<UnsignedLong>({}),
        TestSuite::Compare::Container);

    /* Object that is not in the parent array */
    CORRADE_COMPARE_AS(scene.childrenFor(5),
        Containers::arrayView<UnsignedLong>({}),
        TestSuite::Compare::Container);
}

void SceneDataTest::transformation2DFor() {
    const struct Field {
        UnsignedInt object;
        Matrix3 transformation;
    } fields[] {
        {1, Matrix3::translation({3.0f, 2.0f})*Matrix3::scaling({1.5f, 2.0f})},
        {0, Matrix3::rotation(35.0_degf)},
        {4, Matrix3::translation({3.0f, 2.0f})*Matrix3::rotation(35.0_degf)},
        {1, Matrix3::translation({1.0f, 2.0f})} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    CORRADE_COMPARE(scene.transformation2DFor(4),
        Matrix3::translation({3.0f, 2.0f})*Matrix3::rotation(35.0_degf));
    CORRADE_COMPARE(scene.transformation2DFor(0),
        Matrix3::Matrix3::Matrix3::Matrix3::rotation(35.0_degf));

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.transformation2DFor(1),
        Matrix3::translation({3.0f, 2.0f})*Matrix3::scaling({1.5f, 2.0f}));

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.transformation2DFor(2),
        Containers::NullOpt);
}

void SceneDataTest::transformation2DForTRS() {
    const struct Field {
        UnsignedInt object;
        Vector2 translation;
        Complex rotation;
        Vector2 scaling;
    } fields[] {
        {1, {3.0f, 2.0f}, {}, {1.5f, 2.0f}},
        {0, {}, Complex::rotation(35.0_degf), {1.0f, 1.0f}},
        {4, {3.0f, 2.0f}, Complex::rotation(35.0_degf), {1.0f, 1.0f}},
        {1, {1.0f, 2.0f}, {}, {1.0f, 1.0f}} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation, view.slice(&Field::object), view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling, view.slice(&Field::object), view.slice(&Field::scaling)}
    }};

    CORRADE_COMPARE(scene.transformation2DFor(4),
        Matrix3::translation({3.0f, 2.0f})*Matrix3::rotation(35.0_degf));
    CORRADE_COMPARE(scene.translationRotationScaling2DFor(4),
        Containers::triple(Vector2{3.0f, 2.0f}, Complex::rotation(35.0_degf), Vector2{1.0f}));
    CORRADE_COMPARE(scene.transformation2DFor(0),
        Matrix3::rotation(35.0_degf));
    CORRADE_COMPARE(scene.translationRotationScaling2DFor(0),
        Containers::triple(Vector2{}, Complex::rotation(35.0_degf), Vector2{1.0f}));

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.transformation2DFor(1),
        Matrix3::translation({3.0f, 2.0f})*Matrix3::scaling({1.5f, 2.0f}));
    CORRADE_COMPARE(scene.translationRotationScaling2DFor(1),
        Containers::triple(Vector2{3.0f, 2.0f}, Complex{}, Vector2{1.5f, 2.0f}));

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.transformation2DFor(2),
        Containers::NullOpt);
    CORRADE_COMPARE(scene.translationRotationScaling2DFor(2),
        Containers::NullOpt);
}

void SceneDataTest::transformation2DForBut3DType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    SceneData scene{SceneMappingType::UnsignedInt, 1, nullptr, {
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.transformation2DFor(0);
    scene.translationRotationScaling2DFor(0);
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformation2DFor(): scene has a 3D transformation type\n"
        "Trade::SceneData::translationRotationScaling2DFor(): scene has a 3D transformation type\n");
}

void SceneDataTest::transformation3DFor() {
    const struct Field {
        UnsignedInt object;
        Matrix4 transformation;
    } fields[] {
        {1, Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::scaling({1.5f, 2.0f, 4.5f})},
        {0, Matrix4::rotationX(35.0_degf)},
        {4, Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::rotationX(35.0_degf)},
        {1, Matrix4::translation({1.0f, 2.0f, 3.0f})} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    CORRADE_COMPARE(scene.transformation3DFor(4),
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::rotationX(35.0_degf));
    CORRADE_COMPARE(scene.transformation3DFor(0),
        Matrix4::rotationX(35.0_degf));

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.transformation3DFor(1),
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::scaling({1.5f, 2.0f, 4.5f}));

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.transformation3DFor(2),
        Containers::NullOpt);
}

void SceneDataTest::transformation3DForTRS() {
    const struct Field {
        UnsignedInt object;
        Vector3 translation;
        Quaternion rotation;
        Vector3 scaling;
    } fields[] {
        {1, {3.0f, 2.0f, 1.0f}, {}, {1.5f, 2.0f, 4.5f}},
        {0, {}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}},
        {4, {3.0f, 2.0f, 1.0f}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), {1.0f, 1.0f, 1.0f}},
        {1, {1.0f, 2.0f, 3.0f}, Quaternion{}, Vector3{1.0f}} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)},
        SceneFieldData{SceneField::Rotation, view.slice(&Field::object), view.slice(&Field::rotation)},
        SceneFieldData{SceneField::Scaling, view.slice(&Field::object), view.slice(&Field::scaling)}
    }};

    CORRADE_COMPARE(scene.transformation3DFor(4),
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::rotationX(35.0_degf));
    CORRADE_COMPARE(scene.translationRotationScaling3DFor(4),
        Containers::triple(Vector3{3.0f, 2.0f, 1.0f}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), Vector3{1.0f}));
    CORRADE_COMPARE(scene.transformation3DFor(0),
        Matrix4::rotationX(35.0_degf));
    CORRADE_COMPARE(scene.translationRotationScaling3DFor(0),
        Containers::triple(Vector3{}, Quaternion::rotation(35.0_degf, Vector3::xAxis()), Vector3{1.0f}));

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.transformation3DFor(1),
        Matrix4::translation({3.0f, 2.0f, 1.0f})*Matrix4::scaling({1.5f, 2.0f, 4.5f}));
    CORRADE_COMPARE(scene.translationRotationScaling3DFor(1),
        Containers::triple(Vector3{3.0f, 2.0f, 1.0f}, Quaternion{}, Vector3{1.5f, 2.0f, 4.5f}));

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.transformation3DFor(2),
        Containers::NullOpt);
    CORRADE_COMPARE(scene.translationRotationScaling3DFor(2),
        Containers::NullOpt);
}

void SceneDataTest::transformation3DForBut2DType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    SceneData scene{SceneMappingType::UnsignedInt, 1, nullptr, {
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector2, nullptr}
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.transformation3DFor(0);
    scene.translationRotationScaling3DFor(0);
    CORRADE_COMPARE(out,
        "Trade::SceneData::transformation3DFor(): scene has a 2D transformation type\n"
        "Trade::SceneData::translationRotationScaling3DFor(): scene has a 2D transformation type\n");
}

void SceneDataTest::meshesMaterialsFor() {
    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
        Int meshMaterial;
    } fields[]{
        {4, 1, -1},
        {1, 3, 0},
        {2, 4, 1},
        {2, 5, -1},
        {2, 1, 0},
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
        SceneFieldData{SceneField::MeshMaterial, view.slice(&Field::object), view.slice(&Field::meshMaterial)}
    }};

    /* Just one */
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(1),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{3, 0}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(4),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{1, -1}})),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(2),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
            {4, 1}, {5, -1}, {1, 0}
        })), TestSuite::Compare::Container);

    /* Object that is not in the array at all */
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(6),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({})),
        TestSuite::Compare::Container);
}

void SceneDataTest::lightsFor() {
    struct Field {
        UnsignedInt object;
        UnsignedInt light;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4},
        {2, 5}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Light, view.slice(&Field::object), view.slice(&Field::light)}
    }};

    /* Just one */
    CORRADE_COMPARE_AS(scene.lightsFor(1),
        (Containers::arrayView<UnsignedInt>({3})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.lightsFor(4),
        (Containers::arrayView<UnsignedInt>({1})),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.lightsFor(2),
        (Containers::arrayView<UnsignedInt>({
            4, 5
        })), TestSuite::Compare::Container);

    /* Object that is not in the array at all */
    CORRADE_COMPARE_AS(scene.lightsFor(6),
        (Containers::arrayView<UnsignedInt>({})),
        TestSuite::Compare::Container);
}

void SceneDataTest::camerasFor() {
    struct Field {
        UnsignedInt object;
        UnsignedInt camera;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4},
        {2, 5}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Camera, view.slice(&Field::object), view.slice(&Field::camera)}
    }};

    /* Just one */
    CORRADE_COMPARE_AS(scene.camerasFor(1),
        (Containers::arrayView<UnsignedInt>({3})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(4),
        (Containers::arrayView<UnsignedInt>({1})),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.camerasFor(2),
        (Containers::arrayView<UnsignedInt>({
            4, 5
        })), TestSuite::Compare::Container);

    /* Object that is not in the array at all */
    CORRADE_COMPARE_AS(scene.camerasFor(6),
        (Containers::arrayView<UnsignedInt>({})),
        TestSuite::Compare::Container);
}

void SceneDataTest::skinsFor() {
    struct Field {
        UnsignedInt object;
        UnsignedInt skin;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4},
        {2, 5}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        /* To satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin, view.slice(&Field::object), view.slice(&Field::skin)}
    }};

    /* Just one */
    CORRADE_COMPARE_AS(scene.skinsFor(1),
        (Containers::arrayView<UnsignedInt>({3})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.skinsFor(4),
        (Containers::arrayView<UnsignedInt>({1})),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.skinsFor(2),
        (Containers::arrayView<UnsignedInt>({
            4, 5
        })), TestSuite::Compare::Container);

    /* Object that is not in the array at all */
    CORRADE_COMPARE_AS(scene.skinsFor(6),
        (Containers::arrayView<UnsignedInt>({})),
        TestSuite::Compare::Container);
}

void SceneDataTest::importerStateFor() {
    int a, b, c;

    struct Field {
        UnsignedInt object;
        const void* importerState;
    } fields[]{
        {3, &a},
        {4, &b},
        {2, nullptr},
        {4, &c}
    };

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneMappingType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::ImporterState, view.slice(&Field::object), view.slice(&Field::importerState)}
    }};

    CORRADE_COMPARE(scene.importerStateFor(2), nullptr);
    CORRADE_COMPARE(scene.importerStateFor(3), &a);

    /* Duplicate entries -- only the first one gets used, it doesn't traverse
       further */
    CORRADE_COMPARE(scene.importerStateFor(4), &b);

    /* Object that's not in the array at all */
    CORRADE_COMPARE(scene.importerStateFor(1), Containers::NullOpt);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void SceneDataTest::childrenDeprecated() {
    auto&& data = ChildrenDeprecatedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Field {
        UnsignedByte object;
        Short parent;
    } fields[]{
        {5, -1},
        {2, 0},
        {3, 0},
        {0, -1},
        {1, 2},
        {4, -1}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    Containers::Array<SceneFieldData> fieldData;
    if(!data.skipParent)
        arrayAppend(fieldData, SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)});
    if(data.is2D)
        arrayAppend(fieldData, SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Vector2, nullptr});
    if(data.is3D)
        arrayAppend(fieldData, SceneFieldData{SceneField::Translation, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Vector3, nullptr});

    SceneData scene{SceneMappingType::UnsignedByte, 25, {}, fields, Utility::move(fieldData)};

    if(!data.skipParent) {
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_COMPARE_AS(scene.children2D(),
            (data.is2D ? std::vector<UnsignedInt>{5, 0, 4} : std::vector<UnsignedInt>{}),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.children3D(),
            (data.is3D ? std::vector<UnsignedInt>{5, 0, 4} : std::vector<UnsignedInt>{}),
            TestSuite::Compare::Container);
        CORRADE_IGNORE_DEPRECATED_POP
    } else {
        Containers::String out;
        Warning redirectWarning{&out};
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_VERIFY(scene.children2D().empty());
        CORRADE_VERIFY(scene.children3D().empty());
        CORRADE_IGNORE_DEPRECATED_POP
        if(data.is2D)
            CORRADE_COMPARE(out, "Trade::SceneData::children2D(): no parent field present, returned array will be empty\n");
        else if(data.is3D)
            CORRADE_COMPARE(out, "Trade::SceneData::children3D(): no parent field present, returned array will be empty\n");
        else
            CORRADE_COMPARE(out, "");
    }
}
#endif

void SceneDataTest::fieldForFieldMissing() {
    SceneData scene{SceneMappingType::UnsignedInt, 7, nullptr, {}};

    CORRADE_COMPARE(scene.parentFor(6), Containers::NullOpt);
    CORRADE_COMPARE_AS(scene.childrenFor(6),
        Containers::arrayView<UnsignedLong>({}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.transformation2DFor(6), Containers::NullOpt);
    CORRADE_COMPARE(scene.translationRotationScaling2DFor(6), Containers::NullOpt);
    CORRADE_COMPARE(scene.transformation3DFor(6), Containers::NullOpt);
    CORRADE_COMPARE(scene.translationRotationScaling3DFor(6), Containers::NullOpt);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(6),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.lightsFor(6),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(6),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.skinsFor(6),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);
}

void SceneDataTest::findFieldObjectOffsetInvalidObject() {
    CORRADE_SKIP_IF_NO_ASSERT();

    SceneData scene{SceneMappingType::UnsignedInt, 7, nullptr, {
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
    }};

    Containers::String out;
    Error redirectError{&out};
    scene.findFieldObjectOffset(0, 7);
    scene.findFieldObjectOffset(SceneField::Parent, 7);
    scene.fieldObjectOffset(0, 7);
    scene.fieldObjectOffset(SceneField::Parent, 7);
    scene.hasFieldObject(0, 7);
    scene.hasFieldObject(SceneField::Parent, 7);
    scene.parentFor(7);
    scene.childrenFor(-2);
    scene.childrenFor(7);
    scene.transformation2DFor(7);
    scene.translationRotationScaling2DFor(7);
    scene.transformation3DFor(7);
    scene.translationRotationScaling3DFor(7);
    scene.meshesMaterialsFor(7);
    scene.lightsFor(7);
    scene.camerasFor(7);
    scene.skinsFor(7);
    CORRADE_COMPARE(out,
        "Trade::SceneData::findFieldObjectOffset(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::findFieldObjectOffset(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::fieldObjectOffset(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::fieldObjectOffset(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::hasFieldObject(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::hasFieldObject(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::parentFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::childrenFor(): object -2 out of range for 7 objects\n"
        "Trade::SceneData::childrenFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::transformation2DFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::translationRotationScaling2DFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::transformation3DFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::translationRotationScaling3DFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::meshesMaterialsFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::lightsFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::camerasFor(): object 7 out of range for 7 objects\n"
        "Trade::SceneData::skinsFor(): object 7 out of range for 7 objects\n");
}

void SceneDataTest::releaseFieldData() {
    struct Field {
        UnsignedByte object;
        UnsignedInt mesh;
    };

    Containers::Array<char> data{NoInit, 3*sizeof(Field)};
    Containers::StridedArrayView1D<Field> view = Containers::arrayCast<Field>(data);

    auto fields = Containers::array({
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    });
    SceneFieldData* originalFields = fields;

    SceneData scene{SceneMappingType::UnsignedByte, 50, Utility::move(data), Utility::move(fields)};

    Containers::Array<SceneFieldData> released = scene.releaseFieldData();
    CORRADE_COMPARE(released.data(), originalFields);
    CORRADE_COMPARE(released.size(), 2);

    /* Fields are all gone */
    CORRADE_COMPARE(static_cast<const void*>(scene.fieldData()), nullptr);
    CORRADE_COMPARE(scene.fieldCount(), 0);

    /* Data stays untouched, object count and type as well, as it con't result
       in any dangling data access */
    CORRADE_COMPARE(scene.data().data(), view.data());
    CORRADE_COMPARE(scene.mappingBound(), 50);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedByte);
}

void SceneDataTest::releaseData() {
    struct Field {
        UnsignedByte object;
        UnsignedByte mesh;
    };

    Containers::Array<char> data{NoInit, 3*sizeof(Field)};
    Containers::StridedArrayView1D<Field> view = Containers::arrayCast<Field>(data);

    SceneData scene{SceneMappingType::UnsignedByte, 50, Utility::move(data), {
        SceneFieldData{SceneField::Parent, SceneMappingType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::Array<char> released = scene.releaseData();
    CORRADE_COMPARE(released.data(), view.data());
    CORRADE_COMPARE(released.size(), 3*sizeof(Field));

    /* Data are gone */
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);

    /* Fields stay untouched so it's possible to release them separately
       without being forced to order these two releases in any way */
    CORRADE_VERIFY(scene.fieldData().data());
    CORRADE_COMPARE(scene.fieldCount(), 2);

    /* Object count and type stays untouched, as it con't result in any
       dangling data access */
    CORRADE_COMPARE(scene.mappingBound(), 50);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedByte);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneDataTest)
