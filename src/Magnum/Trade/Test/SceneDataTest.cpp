/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Trade/SceneData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#include <Corrade/Containers/GrowableArray.h>
#endif

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneDataTest: TestSuite::Tester {
    explicit SceneDataTest();

    void objectTypeSizeAlignment();
    void objectTypeSizeAlignmentInvalid();
    void debugObjectType();

    void customFieldName();
    void customFieldNameTooLarge();
    void customFieldNameNotCustom();
    void debugFieldName();

    void fieldTypeSizeAlignment();
    void fieldTypeSizeAlignmentInvalid();
    void debugFieldType();

    void constructField();
    void constructFieldDefault();
    void constructFieldCustom();
    void constructField2D();
    void constructFieldTypeErased();
    void constructFieldNonOwningArray();
    void constructFieldOffsetOnly();
    void constructFieldArray();
    void constructFieldArray2D();
    void constructFieldArrayTypeErased();
    void constructFieldArrayOffsetOnly();

    void constructFieldWrongType();
    void constructFieldInconsistentViewSize();
    void constructFieldTooLargeObjectStride();
    void constructFieldTooLargeFieldStride();
    void constructFieldWrongDataAccess();
    void constructField2DWrongSize();
    void constructField2DNonContiguous();
    void constructFieldArrayNonContiguous();
    void constructFieldArrayNotAllowed();
    void constructFieldArray2DWrongSize();
    void constructFieldArray2DNonContiguous();

    void construct();
    void constructZeroFields();
    void constructZeroObjects();
    void constructNotOwned();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructDeprecated();
    void constructDeprecatedBoth2DAnd3D();
    #endif

    void constructDuplicateField();
    void constructDuplicateCustomField();
    void constructInconsistentObjectType();
    void constructObjectDataNotContained();
    void constructFieldDataNotContained();
    void constructObjectTypeTooSmall();
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

    template<class T> void objectsAsArrayByIndex();
    template<class T> void objectsAsArrayByName();
    void objectsAsArrayLongType();
    void objectsIntoArrayByIndex();
    void objectsIntoArrayByName();
    void objectsIntoArrayInvalidSizeOrOffset();
    template<class T> void parentsAsArray();
    #ifndef CORRADE_TARGET_32BIT
    void parentsAsArrayLongType();
    #endif
    void parentsIntoArray();
    void parentsIntoArrayInvalidSizeOrOffset();
    template<class T> void transformations2DAsArray();
    template<class T, class U, class V> void transformations2DAsArrayTRS();
    void transformations2DAsArrayBut3DType();
    void transformations2DIntoArray();
    void transformations2DIntoArrayTRS();
    void transformations2DIntoArrayInvalidSizeOrOffset();
    void transformations2DIntoArrayInvalidSizeOrOffsetTRS();
    template<class T> void transformations3DAsArray();
    template<class T, class U, class V> void transformations3DAsArrayTRS();
    void transformations3DAsArrayBut2DType();
    void transformations3DIntoArray();
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

    void objectsNotFound();
    void objectsWrongType();

    void fieldNotFound();
    void fieldWrongType();
    void fieldWrongPointerType();
    void fieldWrongArrayAccess();

    void parentFor();
    void childrenFor();
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
    std::size_t offset;
    std::size_t size;
    std::size_t expectedSize;
} IntoArrayOffsetData[]{
    {"whole", 0, 3, 3},
    {"one element in the middle", 1, 1, 1},
    {"suffix to a larger array", 2, 10, 1},
    {"offset at the end", 3, 10, 0}
};

#ifdef MAGNUM_BUILD_DEPRECATED
const struct {
    const char* name;
    bool is2D;
    bool is3D;
} ChildrenDeprecatedData[]{
    {"2D", true, false},
    {"3D", false, true},
    {"neither", false, false}
};
#endif

SceneDataTest::SceneDataTest() {
    addTests({&SceneDataTest::objectTypeSizeAlignment,
              &SceneDataTest::objectTypeSizeAlignmentInvalid,
              &SceneDataTest::debugObjectType,

              &SceneDataTest::customFieldName,
              &SceneDataTest::customFieldNameTooLarge,
              &SceneDataTest::customFieldNameNotCustom,
              &SceneDataTest::debugFieldName,

              &SceneDataTest::fieldTypeSizeAlignment,
              &SceneDataTest::fieldTypeSizeAlignmentInvalid,
              &SceneDataTest::debugFieldType,

              &SceneDataTest::constructField,
              &SceneDataTest::constructFieldDefault,
              &SceneDataTest::constructFieldCustom,
              &SceneDataTest::constructField2D,
              &SceneDataTest::constructFieldTypeErased,
              &SceneDataTest::constructFieldNonOwningArray,
              &SceneDataTest::constructFieldOffsetOnly,
              &SceneDataTest::constructFieldArray,
              &SceneDataTest::constructFieldArray2D,
              &SceneDataTest::constructFieldArrayTypeErased,
              &SceneDataTest::constructFieldArrayOffsetOnly,

              &SceneDataTest::constructFieldWrongType,
              &SceneDataTest::constructFieldInconsistentViewSize,
              &SceneDataTest::constructFieldTooLargeObjectStride,
              &SceneDataTest::constructFieldTooLargeFieldStride,
              &SceneDataTest::constructFieldWrongDataAccess,
              &SceneDataTest::constructField2DWrongSize,
              &SceneDataTest::constructField2DNonContiguous,
              &SceneDataTest::constructFieldArrayNonContiguous,
              &SceneDataTest::constructFieldArrayNotAllowed,
              &SceneDataTest::constructFieldArray2DWrongSize,
              &SceneDataTest::constructFieldArray2DNonContiguous,

              &SceneDataTest::construct,
              &SceneDataTest::constructZeroFields,
              &SceneDataTest::constructZeroObjects});

    addInstancedTests({&SceneDataTest::constructNotOwned},
        Containers::arraySize(NotOwnedData));

    #ifdef MAGNUM_BUILD_DEPRECATED
    addInstancedTests({&SceneDataTest::constructDeprecated},
        Containers::arraySize(ChildrenDeprecatedData));

    addTests({&SceneDataTest::constructDeprecatedBoth2DAnd3D});
    #endif

    addTests({&SceneDataTest::constructDuplicateField,
              &SceneDataTest::constructDuplicateCustomField,
              &SceneDataTest::constructInconsistentObjectType,
              &SceneDataTest::constructObjectDataNotContained,
              &SceneDataTest::constructFieldDataNotContained,
              &SceneDataTest::constructObjectTypeTooSmall,
              &SceneDataTest::constructNotOwnedFlagOwned,
              &SceneDataTest::constructMismatchedTRSViews,
              &SceneDataTest::constructMismatchedTRSDimensionality<Float>,
              &SceneDataTest::constructMismatchedTRSDimensionality<Double>,
              &SceneDataTest::constructMismatchedMeshMaterialView,
              &SceneDataTest::constructAmbiguousSkinDimensions,

              &SceneDataTest::constructCopy,
              &SceneDataTest::constructMove,

              &SceneDataTest::findFieldId,
              &SceneDataTest::findFieldObjectOffset<UnsignedByte>,
              &SceneDataTest::findFieldObjectOffset<UnsignedShort>,
              &SceneDataTest::findFieldObjectOffset<UnsignedInt>,
              &SceneDataTest::findFieldObjectOffset<UnsignedLong>,
              &SceneDataTest::findFieldObjectOffsetInvalidOffset,
              &SceneDataTest::fieldObjectOffsetNotFound,

              &SceneDataTest::objectsAsArrayByIndex<UnsignedByte>,
              &SceneDataTest::objectsAsArrayByIndex<UnsignedShort>,
              &SceneDataTest::objectsAsArrayByIndex<UnsignedInt>,
              &SceneDataTest::objectsAsArrayByIndex<UnsignedLong>,
              &SceneDataTest::objectsAsArrayByName<UnsignedByte>,
              &SceneDataTest::objectsAsArrayByName<UnsignedShort>,
              &SceneDataTest::objectsAsArrayByName<UnsignedInt>,
              &SceneDataTest::objectsAsArrayByName<UnsignedLong>,
              &SceneDataTest::objectsAsArrayLongType});

    addInstancedTests({&SceneDataTest::objectsIntoArrayByIndex,
                       &SceneDataTest::objectsIntoArrayByName},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::objectsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::parentsAsArray<Byte>,
              &SceneDataTest::parentsAsArray<Short>,
              &SceneDataTest::parentsAsArray<Int>,
              &SceneDataTest::parentsAsArray<Long>,
              #ifndef CORRADE_TARGET_32BIT
              &SceneDataTest::parentsAsArrayLongType,
              #endif
              });

    addInstancedTests({&SceneDataTest::parentsIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::parentsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations2DAsArray<Matrix3>,
              &SceneDataTest::transformations2DAsArray<Matrix3d>,
              &SceneDataTest::transformations2DAsArray<DualComplex>,
              &SceneDataTest::transformations2DAsArray<DualComplexd>,
              &SceneDataTest::transformations2DAsArrayTRS<Float, Float, Double>,
              &SceneDataTest::transformations2DAsArrayTRS<Double, Double, Float>,
              &SceneDataTest::transformations2DAsArrayBut3DType});

    addInstancedTests({&SceneDataTest::transformations2DIntoArray,
                       &SceneDataTest::transformations2DIntoArrayTRS},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffsetTRS,
              &SceneDataTest::transformations3DAsArray<Matrix4>,
              &SceneDataTest::transformations3DAsArray<Matrix4d>,
              &SceneDataTest::transformations3DAsArray<DualQuaternion>,
              &SceneDataTest::transformations3DAsArray<DualQuaterniond>,
              &SceneDataTest::transformations3DAsArrayTRS<Float, Double, Double>,
              &SceneDataTest::transformations3DAsArrayTRS<Double, Float, Float>,
              &SceneDataTest::transformations3DAsArrayBut2DType});

    addInstancedTests({&SceneDataTest::transformations3DIntoArray,
                       &SceneDataTest::transformations3DIntoArrayTRS},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffsetTRS,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedByte, Int>,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedShort, Byte>,
              &SceneDataTest::meshesMaterialsAsArray<UnsignedInt, Short>});

    addInstancedTests({&SceneDataTest::meshesMaterialsIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::meshesMaterialsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::lightsAsArray<UnsignedByte>,
              &SceneDataTest::lightsAsArray<UnsignedShort>,
              &SceneDataTest::lightsAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::lightsIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::lightsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::camerasAsArray<UnsignedByte>,
              &SceneDataTest::camerasAsArray<UnsignedShort>,
              &SceneDataTest::camerasAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::camerasIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::camerasIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::skinsAsArray<UnsignedByte>,
              &SceneDataTest::skinsAsArray<UnsignedShort>,
              &SceneDataTest::skinsAsArray<UnsignedInt>});

    addInstancedTests({&SceneDataTest::skinsIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::skinsIntoArrayInvalidSizeOrOffset,
              &SceneDataTest::importerStateAsArray<const void*>,
              &SceneDataTest::importerStateAsArray<void*>});

    addInstancedTests({&SceneDataTest::importerStateIntoArray},
        Containers::arraySize(IntoArrayOffsetData));

    addTests({&SceneDataTest::importerStateIntoArrayInvalidSizeOrOffset,

              &SceneDataTest::mutableAccessNotAllowed,

              &SceneDataTest::objectsNotFound,
              &SceneDataTest::objectsWrongType,

              &SceneDataTest::fieldNotFound,
              &SceneDataTest::fieldWrongType,
              &SceneDataTest::fieldWrongPointerType,
              &SceneDataTest::fieldWrongArrayAccess,

              &SceneDataTest::parentFor,
              &SceneDataTest::childrenFor,
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

using namespace Math::Literals;

void SceneDataTest::objectTypeSizeAlignment() {
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedByte), 1);
    CORRADE_COMPARE(sceneObjectTypeAlignment(SceneObjectType::UnsignedByte), 1);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedShort), 2);
    CORRADE_COMPARE(sceneObjectTypeAlignment(SceneObjectType::UnsignedShort), 2);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedInt), 4);
    CORRADE_COMPARE(sceneObjectTypeAlignment(SceneObjectType::UnsignedInt), 4);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedLong), 8);
    CORRADE_COMPARE(sceneObjectTypeAlignment(SceneObjectType::UnsignedLong), 8);
}

void SceneDataTest::objectTypeSizeAlignmentInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    sceneObjectTypeSize(SceneObjectType{});
    sceneObjectTypeAlignment(SceneObjectType{});
    sceneObjectTypeSize(SceneObjectType(0x73));
    sceneObjectTypeAlignment(SceneObjectType(0x73));

    CORRADE_COMPARE(out.str(),
        "Trade::sceneObjectTypeSize(): invalid type Trade::SceneObjectType(0x0)\n"
        "Trade::sceneObjectTypeAlignment(): invalid type Trade::SceneObjectType(0x0)\n"
        "Trade::sceneObjectTypeSize(): invalid type Trade::SceneObjectType(0x73)\n"
        "Trade::sceneObjectTypeAlignment(): invalid type Trade::SceneObjectType(0x73)\n");
}

void SceneDataTest::debugObjectType() {
    std::ostringstream out;
    Debug{&out} << SceneObjectType::UnsignedLong << SceneObjectType(0x73);
    CORRADE_COMPARE(out.str(), "Trade::SceneObjectType::UnsignedLong Trade::SceneObjectType(0x73)\n");
}

void SceneDataTest::customFieldName() {
    CORRADE_VERIFY(!isSceneFieldCustom(SceneField::Rotation));
    CORRADE_VERIFY(!isSceneFieldCustom(SceneField(0x0fffffffu)));
    CORRADE_VERIFY(isSceneFieldCustom(SceneField::Custom));
    CORRADE_VERIFY(isSceneFieldCustom(SceneField(0x80000000u)));

    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0)), 0x80000000u);
    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0xabcd)), 0x8000abcdu);
    CORRADE_COMPARE(UnsignedInt(sceneFieldCustom(0x7fffffff)), 0xffffffffu);

    CORRADE_COMPARE(sceneFieldCustom(SceneField::Custom), 0);
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    sceneFieldCustom(1u << 31);
    CORRADE_COMPARE(out.str(), "Trade::sceneFieldCustom(): index 2147483648 too large\n");
}

void SceneDataTest::customFieldNameNotCustom() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    sceneFieldCustom(SceneField::Transformation);
    CORRADE_COMPARE(out.str(), "Trade::sceneFieldCustom(): Trade::SceneField::Transformation is not custom\n");
}

void SceneDataTest::debugFieldName() {
    std::ostringstream out;
    Debug{&out} << SceneField::Transformation << sceneFieldCustom(73) << SceneField(0xdeadda7);
    CORRADE_COMPARE(out.str(), "Trade::SceneField::Transformation Trade::SceneField::Custom(73) Trade::SceneField(0xdeadda7)\n");
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
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::DualComplexd), alignof(Double));
    CORRADE_COMPARE(sceneFieldTypeAlignment(SceneFieldType::Pointer), alignof(const void*));
}

void SceneDataTest::fieldTypeSizeAlignmentInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    sceneFieldTypeSize(SceneFieldType{});
    sceneFieldTypeAlignment(SceneFieldType{});
    sceneFieldTypeSize(SceneFieldType(0xdead));
    sceneFieldTypeAlignment(SceneFieldType(0xdead));

    CORRADE_COMPARE(out.str(),
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0x0)\n"
        "Trade::sceneFieldTypeAlignment(): invalid type Trade::SceneFieldType(0x0)\n"
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0xdead)\n"
        "Trade::sceneFieldTypeAlignment(): invalid type Trade::SceneFieldType(0xdead)\n");
}

void SceneDataTest::debugFieldType() {
    std::ostringstream out;
    Debug{&out} << SceneFieldType::Matrix3x4h << SceneFieldType(0xdead);
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldType::Matrix3x4h Trade::SceneFieldType(0xdead)\n");
}

constexpr Complexd Rotations2D[3] {
    Complexd{Constantsd::sqrtHalf(), Constantsd::sqrtHalf()}, /* 45° */
    Complexd{1.0, 0.0}, /* 0° */
    Complexd{0.0, 1.0}, /* 90° */
};
const UnsignedShort RotationObjects2D[3] {
    17,
    35,
    98
};

void SceneDataTest::constructField() {
    const UnsignedShort rotationObjectData[3]{};
    const Complexd rotationFieldData[3];

    SceneFieldData rotations{SceneField::Rotation, Containers::arrayView(rotationObjectData), Containers::arrayView(rotationFieldData)};
    CORRADE_VERIFY(!rotations.isOffsetOnly());
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.size(), 3);
    CORRADE_COMPARE(rotations.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(rotations.objectData().size(), 3);
    CORRADE_COMPARE(rotations.objectData().stride(), sizeof(UnsignedShort));
    CORRADE_VERIFY(rotations.objectData().data() == rotationObjectData);
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.fieldArraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), sizeof(Complexd));
    CORRADE_VERIFY(rotations.fieldData().data() == rotationFieldData);

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(Complexd)];
    CORRADE_COMPARE(rotations.fieldData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.fieldData(someArray).stride(), sizeof(Complexd));
    CORRADE_VERIFY(rotations.fieldData(someArray).data() == rotationFieldData);
    CORRADE_COMPARE(rotations.objectData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.objectData(someArray).stride(), sizeof(UnsignedShort));
    CORRADE_VERIFY(rotations.objectData(someArray).data() == rotationObjectData);

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Won't bother anymore */
    constexpr SceneFieldData crotations{SceneField::Rotation, Containers::arrayView(RotationObjects2D), Containers::arrayView(Rotations2D)};
    constexpr bool isOffsetOnly = crotations.isOffsetOnly();
    constexpr SceneField name = crotations.name();
    constexpr SceneObjectType objectType = crotations.objectType();
    constexpr Containers::StridedArrayView1D<const void> objectData = crotations.objectData();
    constexpr SceneFieldType fieldType = crotations.fieldType();
    constexpr UnsignedShort fieldArraySize = crotations.fieldArraySize();
    constexpr Containers::StridedArrayView1D<const void> fieldData = crotations.fieldData();
    CORRADE_VERIFY(!isOffsetOnly);
    CORRADE_COMPARE(name, SceneField::Rotation);
    CORRADE_COMPARE(objectType, SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(objectData.size(), 3);
    CORRADE_COMPARE(objectData.stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(objectData.data(), RotationObjects2D);
    CORRADE_COMPARE(fieldType, SceneFieldType::Complexd);
    CORRADE_COMPARE(fieldArraySize, 0);
    CORRADE_COMPARE(fieldData.size(), 3);
    CORRADE_COMPARE(fieldData.stride(), sizeof(Complexd));
    CORRADE_COMPARE(fieldData.data(), Rotations2D);
    #endif
}

void SceneDataTest::constructFieldDefault() {
    SceneFieldData data;
    CORRADE_COMPARE(data.name(), SceneField{});
    CORRADE_COMPARE(data.fieldType(), SceneFieldType{});
    CORRADE_COMPARE(data.objectType(), SceneObjectType{});

    constexpr SceneFieldData cdata;
    CORRADE_COMPARE(cdata.name(), SceneField{});
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType{});
    CORRADE_COMPARE(cdata.objectType(), SceneObjectType{});
}

void SceneDataTest::constructFieldCustom() {
    /* Verifying it doesn't hit any assertion about disallowed type for given
       attribute */

    const UnsignedByte rangeObjectData[3]{};
    const Range2Dh rangeFieldData[3];
    SceneFieldData ranges{sceneFieldCustom(13), Containers::arrayView(rangeObjectData), Containers::arrayView(rangeFieldData)};
    CORRADE_COMPARE(ranges.name(), sceneFieldCustom(13));
    CORRADE_COMPARE(ranges.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_VERIFY(ranges.objectData().data() == rangeObjectData);
    CORRADE_COMPARE(ranges.fieldType(), SceneFieldType::Range2Dh);
    CORRADE_VERIFY(ranges.fieldData().data() == rangeFieldData);
}

void SceneDataTest::constructField2D() {
    char rotationObjectData[6*sizeof(UnsignedShort)];
    char rotationFieldData[6*sizeof(Complexd)];
    auto rotationObjectView = Containers::StridedArrayView2D<char>{rotationObjectData, {6, sizeof(UnsignedShort)}}.every(2);
    auto rotationFieldView = Containers::StridedArrayView2D<char>{rotationFieldData, {6, sizeof(Complexd)}}.every(2);

    SceneFieldData rotations{SceneField::Rotation, rotationObjectView, SceneFieldType::Complexd, rotationFieldView};
    CORRADE_VERIFY(!rotations.isOffsetOnly());
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.size(), 3);
    CORRADE_COMPARE(rotations.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(rotations.objectData().size(), 3);
    CORRADE_COMPARE(rotations.objectData().stride(), 2*sizeof(UnsignedShort));
    CORRADE_COMPARE(rotations.objectData().data(), rotationObjectView.data());
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.fieldArraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), 2*sizeof(Complexd));
    CORRADE_COMPARE(rotations.fieldData().data(), rotationFieldView.data());
}

void SceneDataTest::constructFieldTypeErased() {
    const UnsignedLong scalingObjectData[3]{};
    const Vector3 scalingFieldData[3];
    SceneFieldData scalings{SceneField::Scaling, SceneObjectType::UnsignedLong, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingObjectData)), SceneFieldType::Vector3, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingFieldData))};
    CORRADE_VERIFY(!scalings.isOffsetOnly());
    CORRADE_COMPARE(scalings.name(), SceneField::Scaling);
    CORRADE_COMPARE(scalings.size(), 3);
    CORRADE_COMPARE(scalings.objectType(), SceneObjectType::UnsignedLong);
    CORRADE_COMPARE(scalings.objectData().size(), 3);
    CORRADE_COMPARE(scalings.objectData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(scalings.objectData().data(), scalingObjectData);
    CORRADE_COMPARE(scalings.fieldType(), SceneFieldType::Vector3);
    CORRADE_COMPARE(scalings.fieldArraySize(), 0);
    CORRADE_COMPARE(scalings.fieldData().size(), 3);
    CORRADE_COMPARE(scalings.fieldData().stride(), sizeof(Vector3));
    CORRADE_COMPARE(scalings.fieldData().data(), scalingFieldData);
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
    } data[] {
        {0, 2, {2.0f, 3.0f}},
        {0, 15, {67.0f, -1.1f}}
    };

    SceneFieldData a{SceneField::Translation, 2, SceneObjectType::UnsignedShort, offsetof(Data, object), sizeof(Data), SceneFieldType::Vector2, offsetof(Data, translation), sizeof(Data)};
    CORRADE_VERIFY(a.isOffsetOnly());
    CORRADE_COMPARE(a.name(), SceneField::Translation);
    CORRADE_COMPARE(a.size(), 2);
    CORRADE_COMPARE(a.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(a.objectData(data).size(), 2);
    CORRADE_COMPARE(a.objectData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(a.objectData(data)),
        Containers::arrayView<UnsignedShort>({2, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Vector2);
    CORRADE_COMPARE(a.fieldArraySize(), 0);
    CORRADE_COMPARE(a.fieldData(data).size(), 2);
    CORRADE_COMPARE(a.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.fieldData(data)),
        Containers::arrayView<Vector2>({{2.0f, 3.0f}, {67.0f, -1.1f}}),
        TestSuite::Compare::Container);
}

constexpr UnsignedByte ArrayOffsetObjectData[3]{};
constexpr Int ArrayOffsetFieldData[3*4]{};

void SceneDataTest::constructFieldArray() {
    UnsignedByte offsetObjectData[3];
    Int offsetFieldData[3*4];
    SceneFieldData data{sceneFieldCustom(34), Containers::arrayView(offsetObjectData), Containers::StridedArrayView2D<Int>{offsetFieldData, {3, 4}}};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetFieldData);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), Containers::arrayView(ArrayOffsetObjectData), Containers::StridedArrayView2D<const Int>{ArrayOffsetFieldData, {3, 4}}};
    CORRADE_VERIFY(!cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(cdata.size(), 3);
    CORRADE_COMPARE(cdata.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(cdata.objectData().size(), 3);
    CORRADE_COMPARE(cdata.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(cdata.objectData().data() == ArrayOffsetObjectData);
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
    CORRADE_COMPARE(cdata.fieldData().size(), 3);
    CORRADE_COMPARE(cdata.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(cdata.fieldData().data() == ArrayOffsetFieldData);
}

void SceneDataTest::constructFieldArray2D() {
    char offsetObjectData[3*sizeof(UnsignedByte)];
    char offsetFieldData[3*4*sizeof(Int)];
    SceneFieldData data{sceneFieldCustom(34), Containers::StridedArrayView2D<char>{offsetObjectData, {3, sizeof(UnsignedByte)}}, SceneFieldType::Int, Containers::StridedArrayView2D<char>{offsetFieldData, {3, 4*sizeof(Int)}}, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetFieldData);
}

void SceneDataTest::constructFieldArrayTypeErased() {
    Int offsetData[3*4];
    Containers::StridedArrayView1D<Int> offset{offsetData, 3, 4*sizeof(Int)};
    UnsignedByte offsetObjectData[3];
    SceneFieldData data{sceneFieldCustom(34), SceneObjectType::UnsignedByte, Containers::arrayCast<const char>(Containers::stridedArrayView(offsetObjectData)), SceneFieldType::Int, Containers::arrayCast<const char>(offset), 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);
    CORRADE_COMPARE(data.fieldArraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetData);
}

void SceneDataTest::constructFieldArrayOffsetOnly() {
    struct Data {
        Byte parent;
        UnsignedByte object;
        Int offset[4];
    };

    SceneFieldData data{sceneFieldCustom(34), 3, SceneObjectType::UnsignedByte, offsetof(Data, object), sizeof(Data), SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), 4};
    CORRADE_VERIFY(data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.size(), 3);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.fieldArraySize(), 4);

    Data actual[3];
    CORRADE_COMPARE(data.fieldData(actual).size(), 3);
    CORRADE_COMPARE(data.fieldData(actual).stride(), sizeof(Data));
    CORRADE_VERIFY(data.fieldData(actual).data() == &actual[0].offset);
    CORRADE_COMPARE(data.objectData(actual).size(), 3);
    CORRADE_COMPARE(data.objectData(actual).stride(), sizeof(Data));
    CORRADE_VERIFY(data.objectData(actual).data() == &actual[0].object);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), 3, SceneObjectType::UnsignedByte, offsetof(Data, object), sizeof(Data), SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), 4};
    CORRADE_VERIFY(cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(cdata.size(), 3);
    CORRADE_COMPARE(cdata.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.fieldArraySize(), 4);
}

void SceneDataTest::constructFieldInconsistentViewSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort rotationObjectData[3]{};
    const Complexd rotationFieldData[2];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(rotationObjectData), Containers::arrayView(rotationFieldData)};
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldData: expected object and field view to have the same size but got 3 and 2\n");
}

void SceneDataTest::constructFieldWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort rotationObjectData[3]{};
    const Quaternion rotationFieldData[3];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Transformation, Containers::arrayView(rotationObjectData), Containers::arrayView(rotationFieldData)};
    SceneFieldData{SceneField::Transformation, 3, SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort), SceneFieldType::Quaternion, 0, sizeof(Quaternion)};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n"
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n");
}

void SceneDataTest::constructFieldTooLargeObjectStride() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}, SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>(), SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 32767, SceneFieldType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 65536, -32768, SceneFieldType::UnsignedInt, 0, 4};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}, SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>(), SceneFieldType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 32768, SceneFieldType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 65538, -32769, SceneFieldType::UnsignedInt, 0, 4};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got -32769\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got -32769\n");
}

void SceneDataTest::constructFieldTooLargeFieldStride() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32767};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65536, -32768};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}};
    SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, enough, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 0, 32768};
    SceneFieldData{SceneField::Mesh, 2, SceneObjectType::UnsignedInt, 0, 4, SceneFieldType::UnsignedInt, 65538, -32769};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got -32769\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got -32769\n");
}

void SceneDataTest::constructFieldWrongDataAccess() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const UnsignedShort rotationObjectData[3]{};
    const Quaternion rotationFieldData[3];
    SceneFieldData a{SceneField::Rotation, Containers::arrayView(rotationObjectData), Containers::arrayView(rotationFieldData)};
    SceneFieldData b{SceneField::Rotation, 3, SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort), SceneFieldType::Quaternion, 0, sizeof(Quaternion)};
    CORRADE_VERIFY(!a.isOffsetOnly());
    CORRADE_VERIFY(b.isOffsetOnly());

    a.objectData(rotationObjectData); /* This is fine, no asserts */
    a.fieldData(rotationFieldData);

    std::ostringstream out;
    Error redirectError{&out};
    b.objectData();
    b.fieldData();
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData::objectData(): the field is offset-only, supply a data array\n"
        "Trade::SceneFieldData::fieldData(): the field is offset-only, supply a data array\n");
}

void SceneDataTest::constructField2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationFieldData[5*sizeof(Complex)];
    char rotationObjectData[5*sizeof(UnsignedInt)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, 5}}.every(2),
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2)};
    SceneFieldData{SceneField::Translation,
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}}.every(2),
        SceneFieldType::Vector3,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2)};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: expected second object view dimension size 1, 2, 4 or 8 but got 5\n"
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Vector3\n");
}

void SceneDataTest::constructField2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationObjectData[8*sizeof(UnsignedInt)];
    char rotationFieldData[8*sizeof(Complex)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}};
    SceneFieldData{SceneField::Rotation,
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}},
        SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, 2*sizeof(Complex)}}.every({1, 2})};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second object view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructFieldArrayNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedByte offsetObjectData[3];
    Int offsetFieldData[3*4];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData data{sceneFieldCustom(34), Containers::arrayView(offsetObjectData), Containers::StridedArrayView2D<Int>{offsetFieldData, {3, 4}}.every({1, 2})};
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructFieldArrayNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedShort rotationObjectData[3]{};
    Quaternion rotationFieldData[3];
    Containers::ArrayView<UnsignedShort> rotationObjects = rotationObjectData;
    Containers::ArrayView<Quaternion> rotationFields = rotationFieldData;
    Containers::StridedArrayView2D<Quaternion> rotationFields2D{rotationFieldData, {3, 3}, {0, sizeof(Quaternion)}};
    auto rotationFields2DChar = Containers::arrayCast<2, const char>(rotationFields2D);
    auto rotationObjectsChar = Containers::arrayCast<2, const char>(rotationObjects);

    /* This is all fine */
    SceneFieldData{SceneField::Rotation,
        SceneObjectType::UnsignedShort, rotationObjects,
        SceneFieldType::Quaternion, rotationFields, 0};
    SceneFieldData{SceneField::Rotation, 3,
        SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 0};
    SceneFieldData{sceneFieldCustom(37),
        rotationObjects,
        rotationFields2D};
    SceneFieldData{sceneFieldCustom(37),
        rotationObjectsChar,
        SceneFieldType::Quaternion, rotationFields2DChar, 3};
    SceneFieldData{sceneFieldCustom(37), 3,
        SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 3};

    /* This is not */
    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        SceneObjectType::UnsignedShort, rotationObjects,
        SceneFieldType::Quaternion, rotationFields, 3};
    SceneFieldData{SceneField::Rotation, 3,
        SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort),
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), 3};
    SceneFieldData{SceneField::Rotation,
        rotationObjects,
        rotationFields2D};
    SceneFieldData{SceneField::Rotation,
        rotationObjectsChar,
        SceneFieldType::Quaternion, rotationFields2DChar, 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n");
}

void SceneDataTest::constructFieldArray2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationObjectData[4*sizeof(UnsignedInt)];
    char rotationFieldData[4*sizeof(Complex)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}}.every(2),
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{rotationFieldData, {4, sizeof(Complex)}}.every(2), 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Int and field array size 3\n");
}

void SceneDataTest::constructFieldArray2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char offsetObjectData[18*sizeof(UnsignedInt)];
    char offsetFieldData[18*sizeof(Int)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{offsetObjectData, {3, 2*sizeof(UnsignedInt)}}.every({1, 2}),
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetFieldData, {3, 3*sizeof(Int)}}, 3};
    SceneFieldData{sceneFieldCustom(37),
        Containers::StridedArrayView2D<char>{offsetObjectData, {3, sizeof(UnsignedInt)}},
        SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetFieldData, {3, 6*sizeof(Int)}}.every({1, 2}), 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second object view dimension is not contiguous\n"
        "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::construct() {
    struct TransformParent {
        UnsignedShort object;
        Matrix4 transformation;
        Int parent;
    };

    Containers::StridedArrayView1D<TransformParent> transformsParentFieldObjectData;
    Containers::StridedArrayView1D<UnsignedByte> meshFieldData;
    Containers::StridedArrayView1D<Vector2> radiusFieldData;
    Containers::StridedArrayView1D<UnsignedShort> materialMeshRadiusObjectData;
    Containers::ArrayTuple data{
        {NoInit, 5, transformsParentFieldObjectData},
        {NoInit, 2, meshFieldData},
        {NoInit, 2, radiusFieldData},
        {NoInit, 2, materialMeshRadiusObjectData},
    };
    transformsParentFieldObjectData[0].object = 4;
    transformsParentFieldObjectData[0].transformation = Matrix4::translation(Vector3::xAxis(5.0f));
    transformsParentFieldObjectData[0].parent = -1;

    transformsParentFieldObjectData[1].object = 2;
    transformsParentFieldObjectData[1].transformation = Matrix4::translation(Vector3::yAxis(5.0f));
    transformsParentFieldObjectData[1].parent = 0;

    transformsParentFieldObjectData[2].object = 3;
    transformsParentFieldObjectData[2].transformation = Matrix4::translation(Vector3::zAxis(5.0f));
    transformsParentFieldObjectData[2].parent = 2;

    transformsParentFieldObjectData[3].object = 0;
    transformsParentFieldObjectData[3].transformation = Matrix4::translation(Vector3::yScale(5.0f));
    transformsParentFieldObjectData[3].parent = 1;

    transformsParentFieldObjectData[4].object = 1;
    transformsParentFieldObjectData[4].transformation = Matrix4::translation(Vector3::zScale(5.0f));
    transformsParentFieldObjectData[4].parent = -1;

    meshFieldData[0] = 5;
    radiusFieldData[0] = {37.5f, 1.5f};
    materialMeshRadiusObjectData[0] = 2;

    meshFieldData[1] = 7;
    radiusFieldData[1] = {22.5f, 0.5f};
    materialMeshRadiusObjectData[1] = 6;

    int importerState;
    SceneFieldData transformations{SceneField::Transformation,
        transformsParentFieldObjectData.slice(&TransformParent::object),
        transformsParentFieldObjectData.slice(&TransformParent::transformation)};
    /* Offset-only */
    SceneFieldData parents{SceneField::Parent, 5,
        SceneObjectType::UnsignedShort, offsetof(TransformParent, object), sizeof(TransformParent),
        SceneFieldType::Int, offsetof(TransformParent, parent), sizeof(TransformParent)};
    SceneFieldData meshes{SceneField::Mesh,
        materialMeshRadiusObjectData,
        meshFieldData};
    /* Custom & array */
    SceneFieldData radiuses{sceneFieldCustom(37),
        materialMeshRadiusObjectData,
        Containers::arrayCast<2, Float>(radiusFieldData)};
    SceneData scene{SceneObjectType::UnsignedShort, 8, std::move(data), {
        transformations, parents, meshes, radiuses
    }, &importerState};

    /* Basics */
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!scene.fieldData().empty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), transformsParentFieldObjectData.data());
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), transformsParentFieldObjectData.data());
    CORRADE_COMPARE(scene.objectCount(), 8);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 4);
    CORRADE_COMPARE(scene.importerState(), &importerState);

    /* is2D() / is3D() exhaustively tested in transformations*DAsArray[TRS]()
       and constructZeroFields() */

    /* Field property access by ID */
    CORRADE_COMPARE(scene.fieldName(0), SceneField::Transformation);
    CORRADE_COMPARE(scene.fieldName(1), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldName(2), SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldName(3), sceneFieldCustom(37));
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
    CORRADE_COMPARE(scene.fieldData(2).objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(2).objectData())[1], 6);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedByte>(scene.fieldData(2).fieldData())[1], 7);
    CORRADE_VERIFY(!scene.fieldData(2).isOffsetOnly());
    CORRADE_COMPARE(scene.fieldData(2).fieldType(), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldData(2).fieldArraySize(), 0);
    /* Offset-only */
    CORRADE_COMPARE(scene.fieldData(1).name(), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldData(1).size(), 5);
    CORRADE_COMPARE(scene.fieldData(1).objectType(), SceneObjectType::UnsignedShort);
    CORRADE_VERIFY(!scene.fieldData(1).isOffsetOnly());
    CORRADE_COMPARE(scene.fieldData(1).fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldData(1).fieldArraySize(), 0);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(1).objectData())[4], 1);
    CORRADE_COMPARE(Containers::arrayCast<const Int>(scene.fieldData(1).fieldData())[4], -1);
    /* Array */
    CORRADE_COMPARE(scene.fieldData(3).name(), sceneFieldCustom(37));
    CORRADE_COMPARE(scene.fieldData(3).size(), 2);
    CORRADE_COMPARE(scene.fieldData(3).objectType(), SceneObjectType::UnsignedShort);
    CORRADE_VERIFY(!scene.fieldData(3).isOffsetOnly());
    CORRADE_COMPARE(scene.fieldData(3).fieldType(), SceneFieldType::Float);
    CORRADE_COMPARE(scene.fieldData(3).fieldArraySize(), 2);
    CORRADE_COMPARE(Containers::arrayCast<const UnsignedShort>(scene.fieldData(3).objectData())[0], 2);
    CORRADE_COMPARE(Containers::arrayCast<const Vector2>(scene.fieldData(3).fieldData())[0], (Vector2{37.5f, 1.5f}));

    /* Typeless object access by ID with a cast later */
    CORRADE_COMPARE(scene.objects(0).size()[0], 5);
    CORRADE_COMPARE(scene.objects(1).size()[0], 5);
    CORRADE_COMPARE(scene.objects(2).size()[0], 2);
    CORRADE_COMPARE(scene.objects(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableObjects(0).size()[0], 5);
    CORRADE_COMPARE(scene.mutableObjects(1).size()[0], 5);
    CORRADE_COMPARE(scene.mutableObjects(2).size()[0], 2);
    CORRADE_COMPARE(scene.mutableObjects(3).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(0))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(1))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(3))[0]), 2);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(0))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(1))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(3))[0]), 2);

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

    /* Typed object access by ID */
    CORRADE_COMPARE(scene.objects<UnsignedShort>(0).size(), 5);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(1).size(), 5);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(2).size(), 2);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(3).size(), 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(0).size(), 5);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(1).size(), 5);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(2).size(), 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(3).size(), 2);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(0)[2], 3);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(1)[4], 1);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(2)[1], 6);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(3)[0], 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(0)[2], 3);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(1)[4], 1);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(2)[1], 6);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(3)[0], 2);

    /* Typed field access by ID */
    CORRADE_COMPARE(scene.field<Matrix4>(0).size(), 5);
    CORRADE_COMPARE(scene.field<Int>(1).size(), 5);
    CORRADE_COMPARE(scene.field<UnsignedByte>(2).size(), 2);
    CORRADE_COMPARE(scene.field<Float[]>(3).size()[0], 2);
    CORRADE_COMPARE(scene.field<Float[]>(3).size()[1], 2);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(0).size(), 5);
    CORRADE_COMPARE(scene.mutableField<Int>(1).size(), 5);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(2).size(), 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(3).size()[1], 2);
    CORRADE_COMPARE(scene.field<Matrix4>(0)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.field<Int>(1)[4], -1);
    CORRADE_COMPARE(scene.field<UnsignedByte>(2)[1], 7);
    CORRADE_COMPARE(scene.field<Float[]>(3)[0][0], 37.5f);
    CORRADE_COMPARE(scene.field<Float[]>(3)[0][1], 1.5f);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(0)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.mutableField<Int>(1)[4], -1);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(2)[1], 7);
    CORRADE_COMPARE(scene.mutableField<Float[]>(3)[0][0], 37.5f);
    CORRADE_COMPARE(scene.mutableField<Float[]>(3)[0][1], 1.5f);

    /* Field property access by name */
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

    /* Typeless object access by name with a cast later */
    CORRADE_COMPARE(scene.objects(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.objects(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.objects(2).size()[0], 2);
    CORRADE_COMPARE(scene.objects(3).size()[0], 2);
    CORRADE_COMPARE(scene.mutableObjects(SceneField::Transformation).size()[0], 5);
    CORRADE_COMPARE(scene.mutableObjects(SceneField::Parent).size()[0], 5);
    CORRADE_COMPARE(scene.mutableObjects(2).size()[0], 2);
    CORRADE_COMPARE(scene.mutableObjects(3).size()[0], 2);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(SceneField::Transformation))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(SceneField::Parent))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, const UnsignedShort>(scene.objects(3))[0]), 2);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(SceneField::Transformation))[2]), 3);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(SceneField::Parent))[4]), 1);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(2))[1]), 6);
    CORRADE_COMPARE((Containers::arrayCast<1, UnsignedShort>(scene.mutableObjects(3))[0]), 2);

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

    /* Typed object access by name */
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(sceneFieldCustom(37)).size(), 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(sceneFieldCustom(37)).size(), 2);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Transformation)[2], 3);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Parent)[4], 1);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(SceneField::Mesh)[1], 6);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(sceneFieldCustom(37))[0], 2);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Transformation)[2], 3);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Parent)[4], 1);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(SceneField::Mesh)[1], 6);
    CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(sceneFieldCustom(37))[0], 2);

    /* Typed field access by name */
    CORRADE_COMPARE(scene.field<Matrix4>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.field<Int>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.field<UnsignedByte>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.field<Float[]>(sceneFieldCustom(37)).size()[0], 2);
    CORRADE_COMPARE(scene.field<Float[]>(sceneFieldCustom(37)).size()[1], 2);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(SceneField::Transformation).size(), 5);
    CORRADE_COMPARE(scene.mutableField<Int>(SceneField::Parent).size(), 5);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(SceneField::Mesh).size(), 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(sceneFieldCustom(37)).size()[0], 2);
    CORRADE_COMPARE(scene.mutableField<Float[]>(sceneFieldCustom(37)).size()[1], 2);
    CORRADE_COMPARE(scene.field<Matrix4>(SceneField::Transformation)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.field<Int>(SceneField::Parent)[4], -1);
    CORRADE_COMPARE(scene.field<UnsignedByte>(SceneField::Mesh)[1], 7);
    CORRADE_COMPARE(scene.field<Float[]>(sceneFieldCustom(37))[0][0], 37.5f);
    CORRADE_COMPARE(scene.field<Float[]>(sceneFieldCustom(37))[0][1], 1.5f);
    CORRADE_COMPARE(scene.mutableField<Matrix4>(SceneField::Transformation)[2], Matrix4::translation(Vector3::zAxis(5.0f)));
    CORRADE_COMPARE(scene.mutableField<Int>(SceneField::Parent)[4], -1);
    CORRADE_COMPARE(scene.mutableField<UnsignedByte>(SceneField::Mesh)[1], 7);
    CORRADE_COMPARE(scene.mutableField<Float[]>(sceneFieldCustom(37))[0][0], 37.5f);
    CORRADE_COMPARE(scene.mutableField<Float[]>(sceneFieldCustom(37))[0][1], 1.5f);
}

void SceneDataTest::constructZeroFields() {
    int importerState;
    SceneData scene{SceneObjectType::UnsignedShort, 37563, nullptr, {}, &importerState};
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(scene.fieldData().empty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), nullptr);
    CORRADE_COMPARE(scene.importerState(), &importerState);
    CORRADE_COMPARE(scene.objectCount(), 37563);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 0);
    CORRADE_VERIFY(!scene.is2D());
    CORRADE_VERIFY(!scene.is3D());
}

void SceneDataTest::constructZeroObjects() {
    int importerState;
    SceneFieldData meshes{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr};
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {meshes, materials}, &importerState};
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!scene.fieldData().empty());
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);
    CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), nullptr);
    CORRADE_COMPARE(scene.importerState(), &importerState);
    CORRADE_COMPARE(scene.objectCount(), 0);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedInt);
    CORRADE_COMPARE(scene.fieldCount(), 2);

    /* Field property access by name */
    CORRADE_COMPARE(scene.fieldType(SceneField::Mesh), SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldType(SceneField::MeshMaterial), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Mesh), 0);
    CORRADE_COMPARE(scene.fieldSize(SceneField::MeshMaterial), 0);
    CORRADE_COMPARE(scene.objects(SceneField::Mesh).data(), nullptr);
    CORRADE_COMPARE(scene.objects(SceneField::MeshMaterial).data(), nullptr);
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
    SceneData scene{SceneObjectType::UnsignedShort, 7, instanceData.dataFlags, Containers::arrayView(data), {mesh}, &importerState};

    CORRADE_COMPARE(scene.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), +data);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<void*>(scene.mutableData()), static_cast<void*>(data));
    CORRADE_COMPARE(scene.objectCount(), 7);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldCount(), 1);
    CORRADE_COMPARE(scene.importerState(), &importerState);

    CORRADE_COMPARE(scene.objects<UnsignedShort>(0).size(), 3);
    CORRADE_COMPARE(scene.objects<UnsignedShort>(0)[2], 2);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(scene.mutableObjects<UnsignedShort>(0)[2], 2);

    CORRADE_COMPARE(scene.field<UnsignedByte>(0).size(), 3);
    CORRADE_COMPARE(scene.field<UnsignedByte>(0)[2], 0);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(scene.mutableField<UnsignedByte>(0)[2], 0);
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
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedInt);
    if(data.is2D || data.is3D)
        CORRADE_COMPARE(scene.objectCount(), 37);
    else
        CORRADE_COMPARE(scene.objectCount(), 0);
    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Mutable|DataFlag::Owned);
    CORRADE_COMPARE(scene.importerState(), &a);
    CORRADE_COMPARE(scene.fieldCount(), 1);
    CORRADE_COMPARE(scene.fieldName(0), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldType(0), SceneFieldType::Int);
    if(data.is2D || data.is3D) {
        CORRADE_COMPARE_AS(scene.objects<UnsignedInt>(0),
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_IGNORE_DEPRECATED_PUSH
    SceneData scene{{5, 17}, {36, 22}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(out.str(), "Trade::SceneData: it's no longer possible to have a scene with both 2D and 3D objects\n");
}
#endif

void SceneDataTest::constructDuplicateField() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Builtin fields are checked using a bitfield, as they have monotonic
       numbering */
    SceneFieldData meshes{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr};
    SceneFieldData meshesAgain{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {meshes, materials, meshesAgain}};
    CORRADE_COMPARE(out.str(), "Trade::SceneData: duplicate field Trade::SceneField::Mesh\n");
}

void SceneDataTest::constructDuplicateCustomField() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* These are checked in an O(n^2) way, separately from builtin fields.
       Can't use a bitfield since the field index can be anything. */
    SceneFieldData customA{sceneFieldCustom(37), SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData customB{sceneFieldCustom(1038576154), SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};
    SceneFieldData customAAgain{sceneFieldCustom(37), SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {customA, customB, customAAgain}};
    CORRADE_COMPARE(out.str(), "Trade::SceneData: duplicate field Trade::SceneField::Custom(37)\n");
}

void SceneDataTest::constructInconsistentObjectType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SceneFieldData meshes{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedShort, nullptr};
    SceneFieldData materials{SceneField::MeshMaterial, SceneObjectType::UnsignedShort, nullptr, SceneFieldType::Int, nullptr};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {meshes, materials}};
    CORRADE_COMPARE(out.str(), "Trade::SceneData: inconsistent object type, got Trade::SceneObjectType::UnsignedShort for field 1 but expected Trade::SceneObjectType::UnsignedInt\n");
}

void SceneDataTest::constructObjectDataNotContained() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<char> data{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> dataIn{reinterpret_cast<UnsignedShort*>(0xbadda9), 5};
    Containers::ArrayView<UnsignedShort> dataSlightlyOut{reinterpret_cast<UnsignedShort*>(0xbaddaa), 5};
    Containers::ArrayView<UnsignedShort> dataOut{reinterpret_cast<UnsignedShort*>(0xdead), 5};

    std::ostringstream out;
    Error redirectError{&out};
    /* First a "slightly off" view that exceeds the original by one byte */
    SceneData{SceneObjectType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, dataSlightlyOut, dataIn}
    }};
    /* Second a view that's in a completely different location */
    SceneData{SceneObjectType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataOut, dataIn}
    }};
    /* Verify the owning constructor does the checks as well */
    SceneData{SceneObjectType::UnsignedShort, 5, std::move(data), {
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataOut, dataIn}
    }};
    /* And if we have no data at all, it doesn't try to dereference them but
       still checks properly */
    SceneData{SceneObjectType::UnsignedShort, 5, nullptr, {
        SceneFieldData{SceneField::Mesh, dataOut, dataIn}
    }};
    /* Finally, offset-only fields with a different message */
    SceneData{SceneObjectType::UnsignedByte, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneObjectType::UnsignedByte, 4, 4, SceneFieldType::UnsignedByte, 0, 4}
    }};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: object data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: object data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: object data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: object data [0xdead:0xdeb7] of field 0 are not contained in passed data array [0x0:0x0]\n"

        "Trade::SceneData: offset-only object data of field 0 span 25 bytes but passed data array has only 24\n");
}

void SceneDataTest::constructFieldDataNotContained() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Mostly the same as constructObjectDataNotContained() with object and
       field views swapped, and added checks for array fields */

    Containers::Array<char> data{reinterpret_cast<char*>(0xbadda9), 10, [](char*, std::size_t){}};
    Containers::ArrayView<UnsignedShort> dataIn{reinterpret_cast<UnsignedShort*>(0xbadda9), 5};
    Containers::ArrayView<UnsignedShort> dataSlightlyOut{reinterpret_cast<UnsignedShort*>(0xbaddaa), 5};
    Containers::ArrayView<UnsignedShort> dataOut{reinterpret_cast<UnsignedShort*>(0xdead), 5};

    std::ostringstream out;
    Error redirectError{&out};
    /* First a "slightly off" view that exceeds the original by one byte */
    SceneData{SceneObjectType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Mesh, dataIn, dataSlightlyOut}
    }};
    /* Second a view that's in a completely different location */
    SceneData{SceneObjectType::UnsignedShort, 5, {}, data, {
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataIn, dataOut}
    }};
    /* Verify array size is taken into account as well. If not, the data would
       span only 7 bytes out of 10 (instead of 12), which is fine. */
    SceneData{SceneObjectType::UnsignedShort, 5, {}, data, {
        SceneFieldData{sceneFieldCustom(37), dataIn.prefix(2), Containers::StridedArrayView2D<UnsignedByte>{Containers::ArrayView<UnsignedByte>{reinterpret_cast<UnsignedByte*>(0xbadda9), 12}, {2, 6}}}
    }};
    /* Verify the owning constructor does the checks as well */
    SceneData{SceneObjectType::UnsignedShort, 5, std::move(data), {
        SceneFieldData{SceneField::Light, dataIn, dataIn},
        SceneFieldData{SceneField::Mesh, dataIn, dataOut}
    }};
    /* Not checking for nullptr data, since that got checked for object view
       already and there's no way to trigger it for fields */
    /* Finally, offset-only fields with a different message */
    SceneData{SceneObjectType::UnsignedShort, 6, Containers::Array<char>{24}, {
        SceneFieldData{SceneField::Mesh, 6, SceneObjectType::UnsignedShort, 0, 4, SceneFieldType::UnsignedByte, 4, 4}
    }};
    /* This again spans 21 bytes if array size isn't taken into account, and 25
       if it is */
    SceneData{SceneObjectType::UnsignedShort, 5, Containers::Array<char>{24}, {
        SceneFieldData{sceneFieldCustom(37), 5, SceneObjectType::UnsignedShort, 0, 5, SceneFieldType::UnsignedByte, 0, 5, 5}
    }};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: field data [0xbaddaa:0xbaddb4] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xbadda9:0xbaddb5] of field 0 are not contained in passed data array [0xbadda9:0xbaddb3]\n"
        "Trade::SceneData: field data [0xdead:0xdeb7] of field 1 are not contained in passed data array [0xbadda9:0xbaddb3]\n"

        "Trade::SceneData: offset-only field data of field 0 span 25 bytes but passed data array has only 24\n"
        "Trade::SceneData: offset-only field data of field 0 span 25 bytes but passed data array has only 24\n");
}

void SceneDataTest::constructObjectTypeTooSmall() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* This is fine */
    SceneData{SceneObjectType::UnsignedByte, 0xff, nullptr, {}};
    SceneData{SceneObjectType::UnsignedShort, 0xffff, nullptr, {}};
    SceneData{SceneObjectType::UnsignedInt, 0xffffffffu, nullptr, {}};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedByte, 0x100, nullptr, {}};
    SceneData{SceneObjectType::UnsignedShort, 0x10000, nullptr, {}};
    SceneData{SceneObjectType::UnsignedInt, 0x100000000ull, nullptr, {}};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: Trade::SceneObjectType::UnsignedByte is too small for 256 objects\n"
        "Trade::SceneData: Trade::SceneObjectType::UnsignedShort is too small for 65536 objects\n"
        "Trade::SceneData: Trade::SceneObjectType::UnsignedInt is too small for 4294967296 objects\n");
}

void SceneDataTest::constructNotOwnedFlagOwned() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const char data[32]{};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedByte, 5, DataFlag::Owned, Containers::arrayView(data), {}};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: can't construct with non-owned data but Trade::DataFlag::Owned\n");
}

void SceneDataTest::constructMismatchedTRSViews() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::ArrayView<const char> data{reinterpret_cast<char*>(0xcafe0000),
        /* Three entries, each having a 2D TRS and 3 object IDs */
        3*(24 + 12)};
    Containers::ArrayView<const UnsignedInt> translationObjectData{
        reinterpret_cast<const UnsignedInt*>(data.data()), 3};
    Containers::ArrayView<const Vector2> translationFieldData{
        reinterpret_cast<const Vector2*>(data.data() + 0x0c), 3};
    Containers::ArrayView<const UnsignedInt> rotationObjectData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 0x24), 3};
    Containers::ArrayView<const Complex> rotationFieldData{
        reinterpret_cast<const Complex*>(data.data() + 0x30), 3};
    Containers::ArrayView<const UnsignedInt> scalingObjectData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 0x48), 3};
    Containers::ArrayView<const Vector2> scalingFieldData{
        reinterpret_cast<const Vector2*>(data.data() + 0x54), 3};

    SceneFieldData translations{SceneField::Translation, translationObjectData, translationFieldData};
    SceneFieldData rotationsDifferent{SceneField::Rotation, rotationObjectData, rotationFieldData};
    SceneFieldData scalingsDifferent{SceneField::Scaling, scalingObjectData, scalingFieldData};
    SceneFieldData rotationsSameButLess{SceneField::Rotation, translationObjectData.except(1), rotationFieldData.except(1)};
    SceneFieldData scalingsSameButLess{SceneField::Scaling, translationObjectData.except(2), scalingFieldData.except(2)};

    /* Test that all pairs get checked */
    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {translations, rotationsDifferent}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {translations, scalingsDifferent}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {rotationsDifferent, scalingsDifferent}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {translations, rotationsSameButLess}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {translations, scalingsSameButLess}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {rotationsSameButLess, scalingsSameButLess}};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: Trade::SceneField::Rotation object data [0xcafe0024:0xcafe0030] is different from Trade::SceneField::Translation object data [0xcafe0000:0xcafe000c]\n"
        "Trade::SceneData: Trade::SceneField::Scaling object data [0xcafe0048:0xcafe0054] is different from Trade::SceneField::Translation object data [0xcafe0000:0xcafe000c]\n"
        "Trade::SceneData: Trade::SceneField::Scaling object data [0xcafe0048:0xcafe0054] is different from Trade::SceneField::Rotation object data [0xcafe0024:0xcafe0030]\n"
        "Trade::SceneData: Trade::SceneField::Rotation object data [0xcafe0000:0xcafe0008] is different from Trade::SceneField::Translation object data [0xcafe0000:0xcafe000c]\n"
        "Trade::SceneData: Trade::SceneField::Scaling object data [0xcafe0000:0xcafe0004] is different from Trade::SceneField::Translation object data [0xcafe0000:0xcafe000c]\n"
        "Trade::SceneData: Trade::SceneField::Scaling object data [0xcafe0000:0xcafe0004] is different from Trade::SceneField::Rotation object data [0xcafe0000:0xcafe0008]\n");
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
_c(Matrix4)
_c(Matrix4d)
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

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SceneFieldData transformationMatrices2D{SceneField::Transformation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix3<T>>::type(), nullptr};
    SceneFieldData transformations2D{SceneField::Transformation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::DualComplex<T>>::type(), nullptr};
    SceneFieldData transformationMatrices3D{SceneField::Transformation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Matrix4<T>>::type(), nullptr};
    SceneFieldData transformations3D{SceneField::Transformation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::DualQuaternion<T>>::type(), nullptr};
    SceneFieldData translations2D{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector2<T>>::type(), nullptr};
    SceneFieldData translations3D{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector3<T>>::type(), nullptr};
    SceneFieldData rotations2D{SceneField::Rotation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Complex<T>>::type(), nullptr};
    SceneFieldData rotations3D{SceneField::Rotation, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Quaternion<T>>::type(), nullptr};
    SceneFieldData scalings2D{SceneField::Scaling, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector2<T>>::type(), nullptr};
    SceneFieldData scalings3D{SceneField::Scaling, SceneObjectType::UnsignedInt, nullptr, Implementation::SceneFieldTypeFor<Math::Vector3<T>>::type(), nullptr};

    /* Test that all pairs get checked */
    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices2D, translations3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices2D, rotations3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices2D, scalings3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations2D, translations3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations2D, rotations3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations2D, scalings3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {translations2D, rotations3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {translations2D, scalings3D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {rotations2D, scalings3D}};

    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices3D, translations2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices3D, rotations2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformationMatrices3D, scalings2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations3D, translations2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations3D, rotations2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {transformations3D, scalings2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {translations3D, rotations2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {translations3D, scalings2D}};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {rotations3D, scalings2D}};
    CORRADE_COMPARE(out.str(), Utility::formatString(
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
        "Trade::SceneData: expected a 3D rotation field but got Trade::SceneFieldType::{3}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n"
        "Trade::SceneData: expected a 3D scaling field but got Trade::SceneFieldType::{2}\n",
        NameTraits<Math::Vector3<T>>::name(),
        NameTraits<Math::Quaternion<T>>::name(),
        NameTraits<Math::Vector2<T>>::name(),
        NameTraits<Math::Complex<T>>::name()));
}

void SceneDataTest::constructMismatchedMeshMaterialView() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::ArrayView<const char> data{reinterpret_cast<char*>(0xcafe0000),
        /* Three entries, each having mesh/material ID and 2 object IDs */
        3*(8 + 8)};
    Containers::ArrayView<const UnsignedInt> meshObjectData{
        reinterpret_cast<const UnsignedInt*>(data.data()), 3};
    Containers::ArrayView<const UnsignedInt> meshFieldData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 0x0c), 3};
    Containers::ArrayView<const UnsignedInt> meshMaterialObjectData{
        reinterpret_cast<const UnsignedInt*>(data.data() + 0x18), 3};
    Containers::ArrayView<const Int> meshMaterialFieldData{
        reinterpret_cast<const Int*>(data.data() + 0x24), 3};

    SceneFieldData meshes{SceneField::Mesh, meshObjectData, meshFieldData};
    SceneFieldData meshMaterialsDifferent{SceneField::MeshMaterial, meshMaterialObjectData, meshMaterialFieldData};
    SceneFieldData meshMaterialsSameButLess{SceneField::MeshMaterial, meshObjectData.except(1), meshMaterialFieldData.except(1)};

    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {meshes, meshMaterialsDifferent}};
    SceneData{SceneObjectType::UnsignedInt, 3, {}, data, {meshes, meshMaterialsSameButLess}};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData: Trade::SceneField::MeshMaterial object data [0xcafe0018:0xcafe0024] is different from Trade::SceneField::Mesh object data [0xcafe0000:0xcafe000c]\n"
        "Trade::SceneData: Trade::SceneField::MeshMaterial object data [0xcafe0000:0xcafe0008] is different from Trade::SceneField::Mesh object data [0xcafe0000:0xcafe000c]\n");
}

void SceneDataTest::constructAmbiguousSkinDimensions() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    SceneData{SceneObjectType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Skin, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr}
    }};
    CORRADE_COMPARE(out.str(), "Trade::SceneData: a skin field requires some transformation field to be present in order to disambiguate between 2D and 3D\n");
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
    SceneData a{SceneObjectType::UnsignedShort, 15, std::move(data), {meshes}, &importerState};

    SceneData b{std::move(a)};
    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(b.objectCount(), 15);
    CORRADE_COMPARE(b.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(b.fieldCount(), 1);
    CORRADE_COMPARE(b.importerState(), &importerState);
    CORRADE_COMPARE(static_cast<const void*>(b.data()), meshData.data());
    CORRADE_COMPARE(b.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(b.fieldType(0), SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(b.fieldSize(0), 3);
    CORRADE_COMPARE(b.fieldArraySize(0), 0);
    CORRADE_COMPARE(b.objects<UnsignedShort>(0)[2], 122);
    CORRADE_COMPARE(b.field<UnsignedInt>(0)[2], 2);

    SceneData c{SceneObjectType::UnsignedByte, 76, nullptr, {}};
    c = std::move(b);
    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(c.objectCount(), 15);
    CORRADE_COMPARE(c.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(c.fieldCount(), 1);
    CORRADE_COMPARE(c.importerState(), &importerState);
    CORRADE_COMPARE(static_cast<const void*>(c.data()), meshData.data());
    CORRADE_COMPARE(c.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(c.fieldType(0), SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(c.fieldSize(0), 3);
    CORRADE_COMPARE(c.fieldArraySize(0), 0);
    CORRADE_COMPARE(c.objects<UnsignedShort>(0)[2], 122);
    CORRADE_COMPARE(c.field<UnsignedInt>(0)[2], 2);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SceneData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SceneData>::value);
}

void SceneDataTest::findFieldId() {
    SceneData scene{SceneObjectType::UnsignedInt, 0, {}, nullptr, {
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedByte, nullptr}
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

    /** @todo update once field flags describing object order are present */

    struct Field {
        T object;
        UnsignedInt mesh;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4},
        {0, 5},
        {2, 5}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{Implementation::sceneObjectTypeFor<T>(), 7, {}, fields, {
        /* Test also with a completely empty field */
        SceneFieldData{SceneField::Parent, Implementation::sceneObjectTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    CORRADE_COMPARE(scene.findFieldObjectOffset(0, 4), Containers::NullOpt);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Parent, 4), Containers::NullOpt);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 4), 0);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 1), 1);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 2), 2);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 2, 3), 4);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 2, 5), Containers::NullOpt);
    CORRADE_COMPARE(scene.findFieldObjectOffset(1, 3), Containers::NullOpt);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 4), 0);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 1), 1);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 2), 2);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 2, 3), 4);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 2, 5), Containers::NullOpt);
    CORRADE_COMPARE(scene.findFieldObjectOffset(SceneField::Mesh, 3), Containers::NullOpt);

    CORRADE_COMPARE(scene.fieldObjectOffset(1, 4), 0);
    CORRADE_COMPARE(scene.fieldObjectOffset(1, 1), 1);
    CORRADE_COMPARE(scene.fieldObjectOffset(1, 2), 2);
    CORRADE_COMPARE(scene.fieldObjectOffset(1, 2, 3), 4);
    CORRADE_COMPARE(scene.fieldObjectOffset(SceneField::Mesh, 4), 0);
    CORRADE_COMPARE(scene.fieldObjectOffset(SceneField::Mesh, 1), 1);
    CORRADE_COMPARE(scene.fieldObjectOffset(SceneField::Mesh, 2), 2);
    CORRADE_COMPARE(scene.fieldObjectOffset(SceneField::Mesh, 2, 3), 4);

    CORRADE_VERIFY(!scene.hasFieldObject(0, 4));
    CORRADE_VERIFY(!scene.hasFieldObject(SceneField::Parent, 4));
    CORRADE_VERIFY(scene.hasFieldObject(1, 4));
    CORRADE_VERIFY(scene.hasFieldObject(1, 1));
    CORRADE_VERIFY(scene.hasFieldObject(1, 2));
    CORRADE_VERIFY(!scene.hasFieldObject(1, 3));
    CORRADE_VERIFY(scene.hasFieldObject(SceneField::Mesh, 4));
    CORRADE_VERIFY(scene.hasFieldObject(SceneField::Mesh, 1));
    CORRADE_VERIFY(scene.hasFieldObject(SceneField::Mesh, 2));
    CORRADE_VERIFY(!scene.hasFieldObject(SceneField::Mesh, 3));
}

void SceneDataTest::findFieldObjectOffsetInvalidOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[]{
        {4, 1},
        {1, 3},
        {2, 4}
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.findFieldObjectOffset(0, 1, 4);
    scene.findFieldObjectOffset(SceneField::Mesh, 1, 4);
    scene.fieldObjectOffset(0, 1, 4);
    scene.fieldObjectOffset(SceneField::Mesh, 1, 4);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::findFieldObjectOffset(): offset 4 out of bounds for a field of size 3\n"
        "Trade::SceneData::findFieldObjectOffset(): offset 4 out of bounds for a field of size 3\n"
        "Trade::SceneData::fieldObjectOffset(): offset 4 out of bounds for a field of size 3\n"
        "Trade::SceneData::fieldObjectOffset(): offset 4 out of bounds for a field of size 3\n");
}

void SceneDataTest::fieldObjectOffsetNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
        /* Test also with a completely empty field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.fieldObjectOffset(0, 4);
    scene.fieldObjectOffset(SceneField::Parent, 4);
    scene.fieldObjectOffset(1, 1, 2);
    scene.fieldObjectOffset(SceneField::Mesh, 1, 2);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::fieldObjectOffset(): object 4 not found in field Trade::SceneField::Parent starting at offset 0\n"
        "Trade::SceneData::fieldObjectOffset(): object 4 not found in field Trade::SceneField::Parent starting at offset 0\n"
        "Trade::SceneData::fieldObjectOffset(): object 1 not found in field Trade::SceneField::Mesh starting at offset 2\n"
        "Trade::SceneData::fieldObjectOffset(): object 1 not found in field Trade::SceneField::Mesh starting at offset 2\n");
}

template<class T> void SceneDataTest::objectsAsArrayByIndex() {
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

    SceneData scene{Implementation::sceneObjectTypeFor<T>(), 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, Implementation::sceneObjectTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    CORRADE_COMPARE_AS(scene.objectsAsArray(1),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

template<class T> void SceneDataTest::objectsAsArrayByName() {
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

    SceneData scene{Implementation::sceneObjectTypeFor<T>(), 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, Implementation::sceneObjectTypeFor<T>(), nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    CORRADE_COMPARE_AS(scene.objectsAsArray(SceneField::Mesh),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

void SceneDataTest::objectsAsArrayLongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedLong object;
        UnsignedByte mesh;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedLong, 0x100000000ull, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    /* AsArray calls into IntoArray, which then has the assert, so this tests
       both */
    std::ostringstream out;
    Error redirectError{&out};
    scene.objectsAsArray(0);
    scene.objectsAsArray(SceneField::Mesh);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::objectsInto(): indices for up to 4294967296 objects can't fit into a 32-bit type, access them directly via objects() instead\n"
        "Trade::SceneData::objectsInto(): indices for up to 4294967296 objects can't fit into a 32-bit type, access them directly via objects() instead\n");
}

void SceneDataTest::objectsIntoArrayByIndex() {
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

    SceneData scene{SceneObjectType::UnsignedInt, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.objectsInto(1, out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::object),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.objectsInto(1, data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::objectsIntoArrayByName() {
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

    SceneData scene{SceneObjectType::UnsignedInt, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.objectsInto(SceneField::Mesh, out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::object),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.objectsInto(SceneField::Mesh, data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::object)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::objectsIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedByte mesh;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    scene.objectsInto(0, destination);
    scene.objectsInto(SceneField::Mesh, destination);
    scene.objectsInto(0, 4, destination);
    scene.objectsInto(SceneField::Mesh, 4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::objectsInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::objectsInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::objectsInto(): offset 4 out of bounds for a field of size 3\n"
        "Trade::SceneData::objectsInto(): offset 4 out of bounds for a field of size 3\n");
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

    SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::UnsignedInt, nullptr},
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    CORRADE_COMPARE_AS(scene.parentsAsArray(),
        Containers::arrayView({15, -1, 44}),
        TestSuite::Compare::Container);
}

#ifndef CORRADE_TARGET_32BIT
void SceneDataTest::parentsAsArrayLongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedLong object;
        Long parent;
    };

    Containers::Array<char> data{nullptr, 0x100000000ull*sizeof(Field), [](char*, std::size_t) {}};
    Containers::StridedArrayView1D<Field> view = Containers::arrayCast<Field>(data);

    SceneData scene{SceneObjectType::UnsignedLong, 0x100000000ull, std::move(data), {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    /* AsArray calls into IntoArray, which then has the assert, so this tests
       both */
    std::ostringstream out;
    Error redirectError{&out};
    scene.parentsAsArray();
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::parentsInto(): parent indices for up to 4294967296 objects can't fit into a 32-bit type, access them directly via field() instead\n");
}
#endif

void SceneDataTest::parentsIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Mesh, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::UnsignedInt, nullptr},
        SceneFieldData{SceneField::Parent,
            view.slice(&Field::object),
            view.slice(&Field::parent)},
    }};

    /* The offset-less overload should give back all data */
    {
        Int out[3];
        scene.parentsInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::parent),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<Int> out{data.size};
        CORRADE_COMPARE(scene.parentsInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::parent)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::parentsIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    Int destination[2];
    scene.parentsInto(destination);
    scene.parentsInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::parentsInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::parentsInto(): offset 4 out of bounds for a field of size 3\n");
}

template<class T> void SceneDataTest::transformations2DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type U;

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
    transformations[0] = {1, T::translation({U(3.0), U(2.0)})};
    transformations[1] = {0, T::rotation(Math::Deg<U>(35.0))};
    transformations[2] = {4, T::translation({U(1.5), U(2.5)})*
                             T::rotation(Math::Deg<U>(-15.0))};
    transformations[3] = {5, T::rotation(Math::Deg<U>(-15.0))*
                             T::translation({U(1.5), U(2.5)})};
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

    SceneData scene{SceneObjectType::UnsignedInt, 6, std::move(data), {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
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
    CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView({
        Matrix3::translation({3.0f, 2.0f}),
        Matrix3::rotation(35.0_degf),
        Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation(-15.0_degf),
        Matrix3::rotation(-15.0_degf)*Matrix3::translation({1.5f, 2.5f})
    }), TestSuite::Compare::Container);
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
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3::translation({3.0f, 2.0f}),
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3::translation({1.5f, 2.5f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{3.0f, 2.0f}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{1.5f, 2.5f}, {}, Vector2{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            rotation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3{Math::IdentityInit},
            Matrix3::rotation(35.0_degf),
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3::rotation(-15.0_degf)
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{}, {}, Vector2{1.0f}},
            {{}, Complex::rotation(35.0_degf), Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, Complex::rotation(-15.0_degf), Vector2{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3::scaling({2.0f, 1.0f}),
            Matrix3::scaling({-0.5f, 4.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, {2.0f, 1.0f}},
            {{}, {}, {-0.5f, 4.0f}},
        })), TestSuite::Compare::Container);
    }

    /* Pairs */
    {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3::translation({3.0f, 2.0f}),
            Matrix3::rotation(35.0_degf),
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation({-15.0_degf})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{3.0f, 2.0f}, {}, Vector2{1.0f}},
            {{}, Complex::rotation(35.0_degf), Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{1.5f, 2.5f}, Complex::rotation(-15.0_degf), Vector2{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3::translation({3.0f, 2.0f}),
            Matrix3{Math::IdentityInit},
            Matrix3{Math::IdentityInit},
            Matrix3::scaling({2.0f, 1.0f}),
            Matrix3::translation({1.5f, 2.5f})*Matrix3::scaling({-0.5f, 4.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{3.0f, 2.0f}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, {2.0f, 1.0f}},
            {{1.5f, 2.5f}, {}, {-0.5f, 4.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            rotation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3{Math::IdentityInit},
            Matrix3::rotation(35.0_degf),
            Matrix3{Math::IdentityInit},
            Matrix3::scaling({2.0f, 1.0f}),
            Matrix3::rotation({-15.0_degf})*Matrix3::scaling({-0.5f, 4.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{}, {}, Vector2{1.0f}},
            {{}, Complex::rotation(35.0_degf), Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, {2.0f, 1.0f}},
            {{}, Complex::rotation(-15.0_degf), {-0.5f, 4.0f}},
        })), TestSuite::Compare::Container);
    }

    /* All */
    {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation,
            scaling
        }};
        CORRADE_VERIFY(scene.is2D());
        CORRADE_VERIFY(!scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations2DAsArray(), Containers::arrayView<Matrix3>({
            Matrix3::translation({3.0f, 2.0f}),
            Matrix3::rotation(35.0_degf),
            Matrix3{Math::IdentityInit},
            Matrix3::scaling({2.0f, 1.0f}),
            Matrix3::translation({1.5f, 2.5f})*Matrix3::rotation({-15.0_degf})*Matrix3::scaling({-0.5f, 4.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings2DAsArray(), (Containers::arrayView<Containers::Triple<Vector2, Complex, Vector2>>({
            {{3.0f, 2.0f}, {}, Vector2{1.0f}},
            {{}, Complex::rotation(35.0_degf), Vector2{1.0f}},
            {{}, {}, Vector2{1.0f}},
            {{}, {}, {2.0f, 1.0f}},
            {{1.5f, 2.5f}, Complex::rotation(-15.0_degf), {-0.5f, 4.0f}},
        })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DAsArrayBut3DType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Because TRSAsArray() allocates an Array<Triple> and then calls
       TRSInto(), which skips views that are nullptr, we wouldn't get the
       assertion for translations, as those are at offset 0, which would be
       interpreted as an empty view if there were no elements. Thus using
       rotations instead. */
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Rotation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Quaternion, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.transformations2DAsArray();
    scene.translationsRotationsScalings2DAsArray();
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::transformations2DInto(): scene has a 3D transformation type\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): scene has a 3D transformation type\n");
}

void SceneDataTest::transformations2DIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)},
    }};

    /* The offset-less overload should give back all data */
    {
        Matrix3 out[3];
        scene.transformations2DInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::transformation),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<Matrix3> out{data.size};
        CORRADE_COMPARE(scene.transformations2DInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::transformation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations2DIntoArrayTRS() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
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
        Matrix3 out[3];
        scene.transformations2DInto(out);
        CORRADE_COMPARE_AS(Containers::arrayView(out),
            Containers::arrayView(expected),
            TestSuite::Compare::Container);

    /* Variant with TRS components */
    } {
        Vector2 translationsOut[3];
        Complex rotationsOut[3];
        Vector2 scalingsOut[3];
        scene.translationsRotationsScalings2DInto(translationsOut, rotationsOut, scalingsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(translationsOut),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(rotationsOut),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(scalingsOut),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* Variant with just translations */
    } {
        Vector2 translationsOut[3];
        scene.translationsRotationsScalings2DInto(translationsOut, nullptr, nullptr);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(translationsOut),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);

    /* Variant with just rotations */
    } {
        Complex rotationsOut[3];
        scene.translationsRotationsScalings2DInto(nullptr, rotationsOut, nullptr);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(rotationsOut),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);

    /* Variant with just scalings */
    } {
        Vector2 scalingsOut[3];
        scene.translationsRotationsScalings2DInto(nullptr, nullptr, scalingsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(scalingsOut),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        scene.translationsRotationsScalings2DInto(nullptr, nullptr, nullptr);

    /* The offset variant only a subset */
    } {
        Containers::Array<Matrix3> out{data.size};
        CORRADE_COMPARE(scene.transformations2DInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            Containers::arrayView(expected).slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with TRS components */
    } {
        Containers::Array<Vector2> translationsOut{data.size};
        Containers::Array<Complex> rotationsOut{data.size};
        Containers::Array<Vector2> scalingsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset, translationsOut, rotationsOut, scalingsOut), data.expectedSize);
        CORRADE_COMPARE_AS(translationsOut.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(rotationsOut.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scalingsOut.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just translations */
    } {
        Containers::Array<Vector2> translationsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset, translationsOut, nullptr, nullptr), data.expectedSize);
        CORRADE_COMPARE_AS(translationsOut.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just rotations */
    } {
        Containers::Array<Complex> rotationsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset, nullptr, rotationsOut, nullptr), data.expectedSize);
        CORRADE_COMPARE_AS(rotationsOut.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just scalings */
    } {
        Containers::Array<Vector2> scalingsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset, nullptr, nullptr, scalingsOut), data.expectedSize);
        CORRADE_COMPARE_AS(scalingsOut.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        CORRADE_COMPARE(scene.translationsRotationsScalings2DInto(data.offset, nullptr, nullptr, nullptr), 0);
    }
}

void SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Matrix3 transformation;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    Matrix3 destination[2];
    scene.transformations2DInto(destination);
    scene.transformations2DInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::transformations2DInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::transformations2DInto(): offset 4 out of bounds for a field of size 3\n");
}

void SceneDataTest::transformations2DIntoArrayInvalidSizeOrOffsetTRS() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Vector2 translation;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    Vector2 translationDestinationCorrect[3];
    Vector2 translationDestination[2];
    Complex rotationDestinationCorrect[3];
    Complex rotationDestination[2];
    Vector2 scalingDestinationCorrect[3];
    Vector2 scalingDestination[2];
    scene.translationsRotationsScalings2DInto(translationDestination, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings2DInto(translationDestinationCorrect, rotationDestination, scalingDestinationCorrect);
    scene.translationsRotationsScalings2DInto(translationDestinationCorrect, rotationDestinationCorrect, scalingDestination);
    scene.translationsRotationsScalings2DInto(4, translationDestination, rotationDestination, scalingDestination);
    scene.translationsRotationsScalings2DInto(0, translationDestinationCorrect, rotationDestination, nullptr);
    scene.translationsRotationsScalings2DInto(0, translationDestinationCorrect, nullptr, scalingDestination);
    scene.translationsRotationsScalings2DInto(0, nullptr, rotationDestinationCorrect, scalingDestination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected translation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected rotation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): expected scaling destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): offset 4 out of bounds for a field of size 3\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and rotation destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): translation and scaling destination views have different size, 3 vs 2\n"
        "Trade::SceneData::translationsRotationsScalings2DInto(): rotation and scaling destination views have different size, 3 vs 2\n");
}

template<class T> void SceneDataTest::transformations3DAsArray() {
    setTestCaseTemplateName(NameTraits<T>::name());

    typedef typename T::Type U;

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
    transformations[0] = {1, T::translation({U(3.0), U(2.0), U(-0.5)})};
    transformations[1] = {0, T::rotation(Math::Deg<U>(35.0),
                                         Math::Vector3<U>::yAxis())};
    transformations[2] = {4, T::translation({U(1.5), U(2.5), U(0.75)})*
                             T::rotation(Math::Deg<U>(-15.0),
                                         Math::Vector3<U>::xAxis())};
    transformations[3] = {5, T::rotation(Math::Deg<U>(-15.0),
                                         Math::Vector3<U>::xAxis())*
                             T::translation({U(1.5), U(2.5), U(0.75)})};
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

    SceneData scene{SceneObjectType::UnsignedInt, 6, std::move(data), {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
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
    CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView({
        Matrix4::translation({3.0f, 2.0f, -0.5f}),
        Matrix4::rotationY(35.0_degf),
        Matrix4::translation({1.5f, 2.5f, 0.75f})*Matrix4::rotationX(-15.0_degf),
        Matrix4::rotationX(-15.0_degf)*Matrix4::translation({1.5f, 2.5f, 0.75f})
    }), TestSuite::Compare::Container);
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
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4::translation({3.0f, 2.0, 1.0f}),
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4::translation({1.5f, 2.5f, 3.5f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{1.5f, 2.5f, 3.5f}, {}, Vector3{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            rotation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4{Math::IdentityInit},
            Matrix4::rotationY(35.0_degf),
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4::rotationX(-15.0_degf)
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{}, {}, Vector3{1.0f}},
            {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), Vector3{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4::scaling({2.0f, 1.0f, 0.0f}),
            Matrix4::scaling({-0.5f, 4.0f, -16.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, {2.0f, 1.0f, 0.0f}},
            {{}, {}, {-0.5f, 4.0f, -16.0f}},
        })), TestSuite::Compare::Container);
    }

    /* Pairs */
    {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4::translation({3.0f, 2.0, 1.0f}),
            Matrix4::rotationY(35.0_degf),
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::rotationX(-15.0_degf)
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}},
            {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{1.5f, 2.5f, 3.5f}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), Vector3{1.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4::translation({3.0f, 2.0, 1.0f}),
            Matrix4{Math::IdentityInit},
            Matrix4{Math::IdentityInit},
            Matrix4::scaling({2.0f, 1.0f, 0.0f}),
            Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, {2.0f, 1.0f, 0.0f}},
            {{1.5f, 2.5f, 3.5f}, {}, {-0.5f, 4.0f, -16.0f}},
        })), TestSuite::Compare::Container);
    } {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            rotation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4{Math::IdentityInit},
            Matrix4::rotationY(35.0_degf),
            Matrix4{Math::IdentityInit},
            Matrix4::scaling({2.0f, 1.0f, 0.0f}),
            Matrix4::rotationX({-15.0_degf})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{}, {}, Vector3{1.0f}},
            {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, {2.0f, 1.0f, 0.0f}},
            {{}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), {-0.5f, 4.0f, -16.0f}},
        })), TestSuite::Compare::Container);
    }

    /* All */
    {
        SceneData scene{SceneObjectType::UnsignedInt, 8, {}, fields, {
            translation,
            rotation,
            scaling
        }};
        CORRADE_VERIFY(!scene.is2D());
        CORRADE_VERIFY(scene.is3D());
        CORRADE_COMPARE_AS(scene.transformations3DAsArray(), Containers::arrayView<Matrix4>({
            Matrix4::translation({3.0f, 2.0, 1.0f}),
            Matrix4::rotationY(35.0_degf),
            Matrix4{Math::IdentityInit},
            Matrix4::scaling({2.0f, 1.0f, 0.0f}),
            Matrix4::translation({1.5f, 2.5f, 3.5f})*Matrix4::rotationX({-15.0_degf})*Matrix4::scaling({-0.5f, 4.0f, -16.0f})
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scene.translationsRotationsScalings3DAsArray(), (Containers::arrayView<Containers::Triple<Vector3, Quaternion, Vector3>>({
            {{3.0f, 2.0, 1.0f}, {}, Vector3{1.0f}},
            {{}, Quaternion::rotation(35.0_degf, Vector3::yAxis()), Vector3{1.0f}},
            {{}, {}, Vector3{1.0f}},
            {{}, {}, {2.0f, 1.0f, 0.0f}},
            {{1.5f, 2.5f, 3.5f}, Quaternion::rotation(-15.0_degf, Vector3::xAxis()), {-0.5f, 4.0f, -16.0f}},
        })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DAsArrayBut2DType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    /* Because TRSAsArray() allocates an Array<Triple> and then calls
       TRSInto(), which skips views that are nullptr, we wouldn't get the
       assertion for translations, as those are at offset 0, which would be
       interpreted as an empty view if there were no elements. Thus using
       rotations instead. */
    SceneData scene{SceneObjectType::UnsignedInt, 0, nullptr, {
        SceneFieldData{SceneField::Rotation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Complex, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.transformations3DAsArray();
    scene.translationsRotationsScalings3DAsArray();
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::transformations3DInto(): scene has a 2D transformation type\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): scene has a 2D transformation type\n");
}

void SceneDataTest::transformations3DIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Transformation,
            view.slice(&Field::object),
            view.slice(&Field::transformation)},
    }};

    /* The offset-less overload should give back all data */
    {
        Matrix4 out[3];
        scene.transformations3DInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::transformation),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<Matrix4> out{data.size};
        CORRADE_COMPARE(scene.transformations3DInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::transformation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::transformations3DIntoArrayTRS() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
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
        Matrix4 out[3];
        scene.transformations3DInto(out);
        CORRADE_COMPARE_AS(Containers::arrayView(out),
            Containers::arrayView(expected),
            TestSuite::Compare::Container);

    /* Variant with TRS components */
    } {
        Vector3 translationsOut[3];
        Quaternion rotationsOut[3];
        Vector3 scalingsOut[3];
        scene.translationsRotationsScalings3DInto(translationsOut, rotationsOut, scalingsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(translationsOut),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(rotationsOut),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(scalingsOut),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* Variant with just translations */
    } {
        Vector3 translationsOut[3];
        scene.translationsRotationsScalings3DInto(translationsOut, nullptr, nullptr);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(translationsOut),
            view.slice(&Field::translation),
            TestSuite::Compare::Container);

    /* Variant with just rotations */
    } {
        Quaternion rotationsOut[3];
        scene.translationsRotationsScalings3DInto(nullptr, rotationsOut, nullptr);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(rotationsOut),
            view.slice(&Field::rotation),
            TestSuite::Compare::Container);

    /* Variant with just scalings */
    } {
        Vector3 scalingsOut[3];
        scene.translationsRotationsScalings3DInto(nullptr, nullptr, scalingsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(scalingsOut),
            view.slice(&Field::scaling),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        scene.translationsRotationsScalings3DInto(nullptr, nullptr, nullptr);

    /* The offset variant only a subset */
    } {
        Containers::Array<Matrix4> out{data.size};
        CORRADE_COMPARE(scene.transformations3DInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            Containers::arrayView(expected).slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with TRS components */
    } {
        Containers::Array<Vector3> translationsOut{data.size};
        Containers::Array<Quaternion> rotationsOut{data.size};
        Containers::Array<Vector3> scalingsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset, translationsOut, rotationsOut, scalingsOut), data.expectedSize);
        CORRADE_COMPARE_AS(translationsOut.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(rotationsOut.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(scalingsOut.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just translations */
    } {
        Containers::Array<Vector3> translationsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset, translationsOut, nullptr, nullptr), data.expectedSize);
        CORRADE_COMPARE_AS(translationsOut.prefix(data.expectedSize),
            view.slice(&Field::translation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just rotations */
    } {
        Containers::Array<Quaternion> rotationsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset, nullptr, rotationsOut, nullptr), data.expectedSize);
        CORRADE_COMPARE_AS(rotationsOut.prefix(data.expectedSize),
            view.slice(&Field::rotation)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just scalings */
    } {
        Containers::Array<Vector3> scalingsOut{data.size};
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset, nullptr, nullptr, scalingsOut), data.expectedSize);
        CORRADE_COMPARE_AS(scalingsOut.prefix(data.expectedSize),
            view.slice(&Field::scaling)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        CORRADE_COMPARE(scene.translationsRotationsScalings3DInto(data.offset, nullptr, nullptr, nullptr), 0);
    }
}

void SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Matrix4 transformation;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Transformation, view.slice(&Field::object), view.slice(&Field::transformation)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    Matrix4 destination[2];
    scene.transformations3DInto(destination);
    scene.transformations3DInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::transformations3DInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::transformations3DInto(): offset 4 out of bounds for a field of size 3\n");
}

void SceneDataTest::transformations3DIntoArrayInvalidSizeOrOffsetTRS() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        Vector2 translation;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Translation, view.slice(&Field::object), view.slice(&Field::translation)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    Vector3 translationDestinationCorrect[3];
    Vector3 translationDestination[2];
    Quaternion rotationDestinationCorrect[3];
    Quaternion rotationDestination[2];
    Vector3 scalingDestinationCorrect[3];
    Vector3 scalingDestination[2];
    scene.translationsRotationsScalings3DInto(translationDestination, rotationDestinationCorrect, scalingDestinationCorrect);
    scene.translationsRotationsScalings3DInto(translationDestinationCorrect, rotationDestination, scalingDestinationCorrect);
    scene.translationsRotationsScalings3DInto(translationDestinationCorrect, rotationDestinationCorrect, scalingDestination);
    scene.translationsRotationsScalings3DInto(4, translationDestination, rotationDestination, scalingDestination);
    scene.translationsRotationsScalings3DInto(0, translationDestinationCorrect, rotationDestination, nullptr);
    scene.translationsRotationsScalings3DInto(0, translationDestinationCorrect, nullptr, scalingDestination);
    scene.translationsRotationsScalings3DInto(0, nullptr, rotationDestinationCorrect, scalingDestination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected translation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected rotation destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): expected scaling destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::translationsRotationsScalings3DInto(): offset 4 out of bounds for a field of size 3\n"
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
        SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
            /* To verify it isn't just picking the first ever field */
            SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
            meshes,
            meshMaterials
        }};

        CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(),
            (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
                {15, 3},
                {37, -1},
                {44, 25}
            })), TestSuite::Compare::Container);

    /* Only meshes */
    } {
        SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
            meshes
        }};

        CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(),
            (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
                {15, -1},
                {37, -1},
                {44, -1}
            })), TestSuite::Compare::Container);
    }
}

void SceneDataTest::meshesMaterialsIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
        SceneFieldData{SceneField::MeshMaterial,
            view.slice(&Field::object),
            view.slice(&Field::meshMaterial)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt meshesOut[3];
        Int meshMaterialsOut[3];
        scene.meshesMaterialsInto(meshesOut, meshMaterialsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(meshesOut),
            view.slice(&Field::mesh),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(meshMaterialsOut),
            view.slice(&Field::meshMaterial),
            TestSuite::Compare::Container);

    /* Variant with just meshes */
    } {
        UnsignedInt meshesOut[3];
        scene.meshesMaterialsInto(meshesOut, nullptr);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(meshesOut),
            view.slice(&Field::mesh),
            TestSuite::Compare::Container);

    /* Variant with just materials */
    } {
        Int meshMaterialsOut[3];
        scene.meshesMaterialsInto(nullptr, meshMaterialsOut);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(meshMaterialsOut),
            view.slice(&Field::meshMaterial),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        scene.meshesMaterialsInto(nullptr, nullptr);

    /* The offset variant should give back only a subset */
    } {
        Containers::Array<UnsignedInt> meshesOut{data.size};
        Containers::Array<Int> meshMaterialsOut{data.size};
        CORRADE_COMPARE(scene.meshesMaterialsInto(data.offset, meshesOut, meshMaterialsOut), data.expectedSize);
        CORRADE_COMPARE_AS(meshesOut.prefix(data.expectedSize),
            view.slice(&Field::mesh)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(meshMaterialsOut.prefix(data.expectedSize),
            view.slice(&Field::meshMaterial)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just meshes */
    } {
        Containers::Array<UnsignedInt> meshesOut{data.size};
        CORRADE_COMPARE(scene.meshesMaterialsInto(data.offset, meshesOut, nullptr), data.expectedSize);
        CORRADE_COMPARE_AS(meshesOut.prefix(data.expectedSize),
            view.slice(&Field::mesh)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with just materials */
    } {
        Containers::Array<Int> meshMaterialsOut{data.size};
        CORRADE_COMPARE(scene.meshesMaterialsInto(data.offset, nullptr, meshMaterialsOut), data.expectedSize);
        CORRADE_COMPARE_AS(meshMaterialsOut.prefix(data.expectedSize),
            view.slice(&Field::meshMaterial)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);

    /* Variant with neither is stupid, but should work too */
    } {
        CORRADE_COMPARE(scene.meshesMaterialsInto(data.offset, nullptr, nullptr), 0);
    }
}

void SceneDataTest::meshesMaterialsIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt meshDestinationCorrect[3];
    UnsignedInt meshDestination[2];
    Int meshMaterialDestinationCorrect[3];
    Int meshMaterialDestination[2];
    scene.meshesMaterialsInto(meshDestination, meshMaterialDestinationCorrect);
    scene.meshesMaterialsInto(meshDestinationCorrect, meshMaterialDestination);
    scene.meshesMaterialsInto(4, meshDestination, meshMaterialDestination);
    scene.meshesMaterialsInto(0, meshDestinationCorrect, meshMaterialDestination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::meshesMaterialsInto(): expected mesh destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::meshesMaterialsInto(): expected mesh material destination view either empty or with 3 elements but got 2\n"
        "Trade::SceneData::meshesMaterialsInto(): offset 4 out of bounds for a field of size 3\n"
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

    SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Light, view.slice(&Field::object), view.slice(&Field::light)}
    }};

    CORRADE_COMPARE_AS(scene.lightsAsArray(),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

void SceneDataTest::lightsIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Light,
            view.slice(&Field::object),
            view.slice(&Field::light)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.lightsInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::light),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.lightsInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::light)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::lightsIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt light;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Light, view.slice(&Field::object), view.slice(&Field::light)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    scene.lightsInto(destination);
    scene.lightsInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::lightsInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::lightsInto(): offset 4 out of bounds for a field of size 3\n");
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

    SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Camera, view.slice(&Field::object), view.slice(&Field::camera)}
    }};

    CORRADE_COMPARE_AS(scene.camerasAsArray(),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

void SceneDataTest::camerasIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Camera,
            view.slice(&Field::object),
            view.slice(&Field::camera)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.camerasInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::camera),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.camerasInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::camera)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::camerasIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt camera;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::Camera, view.slice(&Field::object), view.slice(&Field::camera)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    scene.camerasInto(destination);
    scene.camerasInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::camerasInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::camerasInto(): offset 4 out of bounds for a field of size 3\n");
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

    SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field; also to
           satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin, view.slice(&Field::object), view.slice(&Field::skin)}
    }};

    CORRADE_COMPARE_AS(scene.skinsAsArray(),
        Containers::arrayView<UnsignedInt>({15, 37, 44}),
        TestSuite::Compare::Container);
}

void SceneDataTest::skinsIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field; also to
           satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin,
            view.slice(&Field::object),
            view.slice(&Field::skin)},
    }};

    /* The offset-less overload should give back all data */
    {
        UnsignedInt out[3];
        scene.skinsInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::skin),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<UnsignedInt> out{data.size};
        CORRADE_COMPARE(scene.skinsInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::skin)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::skinsIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt skin;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
        SceneFieldData{SceneField::Skin, view.slice(&Field::object), view.slice(&Field::skin)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    UnsignedInt destination[2];
    scene.skinsInto(destination);
    scene.skinsInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::skinsInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::skinsInto(): offset 4 out of bounds for a field of size 3\n");
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

    SceneData scene{SceneObjectType::UnsignedByte, 50, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::ImporterState, view.slice(&Field::object), view.slice(&Field::importerState)}
    }};

    CORRADE_COMPARE_AS(scene.importerStateAsArray(),
        Containers::arrayView<const void*>({&a, nullptr, &b}),
        TestSuite::Compare::Container);
}

void SceneDataTest::importerStateIntoArray() {
    auto&& data = IntoArrayOffsetData[testCaseInstanceId()];
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

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        /* To verify it isn't just picking the first ever field */
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::ImporterState,
            view.slice(&Field::object),
            view.slice(&Field::importerState)},
    }};

    /* The offset-less overload should give back all data */
    {
        const void* out[3];
        scene.importerStateInto(out);
        CORRADE_COMPARE_AS(Containers::stridedArrayView(out),
            view.slice(&Field::importerState),
            TestSuite::Compare::Container);

    /* The offset variant only a subset */
    } {
        Containers::Array<const void*> out{data.size};
        CORRADE_COMPARE(scene.importerStateInto(data.offset, out), data.expectedSize);
        CORRADE_COMPARE_AS(out.prefix(data.expectedSize),
            view.slice(&Field::importerState)
                .slice(data.offset, data.offset + data.expectedSize),
            TestSuite::Compare::Container);
    }
}

void SceneDataTest::importerStateIntoArrayInvalidSizeOrOffset() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        const void* importerState;
    } fields[3]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{SceneField::ImporterState, view.slice(&Field::object), view.slice(&Field::importerState)}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    const void* destination[2];
    scene.importerStateInto(destination);
    scene.importerStateInto(4, destination);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::importerStateInto(): expected a view with 3 elements but got 2\n"
        "Trade::SceneData::importerStateInto(): offset 4 out of bounds for a field of size 3\n");
}

void SceneDataTest::mutableAccessNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<const Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, {}, fields, {
        SceneFieldData{sceneFieldCustom(35),
            view.slice(&Field::object),
            view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh,
            view.slice(&Field::object),
            view.slice(&Field::mesh)},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.mutableData();
    scene.mutableObjects(0);
    scene.mutableObjects<UnsignedInt>(0);
    scene.mutableObjects(SceneField::Mesh);
    scene.mutableObjects<UnsignedInt>(SceneField::Mesh);
    scene.mutableField(0);
    scene.mutableField<UnsignedInt>(0);
    scene.mutableField<UnsignedInt[]>(1);
    scene.mutableField(SceneField::Mesh);
    scene.mutableField<UnsignedInt>(SceneField::Mesh);
    scene.mutableField<UnsignedInt[]>(sceneFieldCustom(35));
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::mutableData(): data not mutable\n"
        "Trade::SceneData::mutableObjects(): data not mutable\n"
        "Trade::SceneData::mutableObjects(): data not mutable\n"
        "Trade::SceneData::mutableObjects(): data not mutable\n"
        "Trade::SceneData::mutableObjects(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n"
        "Trade::SceneData::mutableField(): data not mutable\n");
}

void SceneDataTest::objectsNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.objects(2);
    scene.objects<UnsignedInt>(2);
    scene.mutableObjects(2);
    scene.mutableObjects<UnsignedInt>(2);
    scene.objects(sceneFieldCustom(666));
    scene.objects<UnsignedInt>(sceneFieldCustom(666));
    scene.mutableObjects(sceneFieldCustom(666));
    scene.mutableObjects<UnsignedInt>(sceneFieldCustom(666));

    scene.objectsAsArray(2);
    scene.objectsAsArray(sceneFieldCustom(666));
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::objects(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::objects(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableObjects(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableObjects(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::objects(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::objects(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableObjects(): field Trade::SceneField::Custom(666) not found\n"
        "Trade::SceneData::mutableObjects(): field Trade::SceneField::Custom(666) not found\n"

        "Trade::SceneData::objectsInto(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::objectsInto(): field Trade::SceneField::Custom(666) not found\n");
}

void SceneDataTest::objectsWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedShort object;
        UnsignedShort foobar;
        UnsignedInt mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedShort, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.objects<UnsignedByte>(1);
    scene.mutableObjects<UnsignedByte>(1);
    scene.objects<UnsignedByte>(SceneField::Mesh);
    scene.mutableObjects<UnsignedByte>(SceneField::Mesh);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::objects(): objects are Trade::SceneObjectType::UnsignedShort but requested Trade::SceneObjectType::UnsignedByte\n"
        "Trade::SceneData::mutableObjects(): objects are Trade::SceneObjectType::UnsignedShort but requested Trade::SceneObjectType::UnsignedByte\n"
        "Trade::SceneData::objects(): objects are Trade::SceneObjectType::UnsignedShort but requested Trade::SceneObjectType::UnsignedByte\n"
        "Trade::SceneData::mutableObjects(): objects are Trade::SceneObjectType::UnsignedShort but requested Trade::SceneObjectType::UnsignedByte\n");
}

void SceneDataTest::fieldNotFound() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt foo, bar;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(34), view.slice(&Field::object), view.slice(&Field::foo)},
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::bar)},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.findFieldObjectOffset(2, 0);
    scene.fieldObjectOffset(2, 0);
    scene.hasFieldObject(2, 0);
    scene.fieldData(2);
    scene.fieldName(2);
    scene.fieldType(2);
    scene.fieldSize(2);
    scene.fieldArraySize(2);
    scene.field(2);
    scene.field<UnsignedInt>(2);
    scene.field<UnsignedInt[]>(2);
    scene.mutableField(2);
    scene.mutableField<UnsignedInt>(2);
    scene.mutableField<UnsignedInt[]>(2);

    scene.fieldId(sceneFieldCustom(666));
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

    scene.parentsAsArray();
    scene.parentsInto(nullptr);
    scene.parentsInto(0, nullptr);
    scene.transformations2DAsArray();
    scene.transformations2DInto(nullptr);
    scene.transformations2DInto(0, nullptr);
    scene.translationsRotationsScalings2DAsArray();
    scene.translationsRotationsScalings2DInto(nullptr, nullptr, nullptr);
    scene.translationsRotationsScalings2DInto(0, nullptr, nullptr, nullptr);
    scene.transformations3DAsArray();
    scene.transformations3DInto(nullptr);
    scene.transformations3DInto(0, nullptr);
    scene.translationsRotationsScalings3DAsArray();
    scene.translationsRotationsScalings3DInto(nullptr, nullptr, nullptr);
    scene.translationsRotationsScalings3DInto(0, nullptr, nullptr, nullptr);
    scene.meshesMaterialsAsArray();
    scene.meshesMaterialsInto(nullptr, nullptr);
    scene.meshesMaterialsInto(0, nullptr, nullptr);
    scene.lightsAsArray();
    scene.lightsInto(nullptr);
    scene.lightsInto(0, nullptr);
    scene.camerasAsArray();
    scene.camerasInto(nullptr);
    scene.camerasInto(0, nullptr);
    scene.skinsAsArray();
    scene.skinsInto(nullptr);
    scene.skinsInto(0, nullptr);
    scene.importerStateAsArray();
    scene.importerStateInto(nullptr);
    scene.importerStateInto(0, nullptr);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::findFieldObjectOffset(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldObjectOffset(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::hasFieldObject(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldData(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldName(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldType(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldSize(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::fieldArraySize(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::field(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"
        "Trade::SceneData::mutableField(): index 2 out of range for 2 fields\n"

        "Trade::SceneData::fieldId(): field Trade::SceneField::Custom(666) not found\n"
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

        /* AsArray() and Into() each share a common helper but have different
           top-level code paths. They however have the same assertion messages
           to save binary size a bit. */
        "Trade::SceneData::parentsInto(): field not found\n"
        "Trade::SceneData::parentsInto(): field not found\n"
        "Trade::SceneData::parentsInto(): field not found\n"
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
        "Trade::SceneData::importerStateInto(): field not found\n");
}

void SceneDataTest::fieldWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedShort foobar;
        UnsignedShort mesh;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), view.slice(&Field::foobar)},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.field<UnsignedByte>(1);
    scene.field<UnsignedByte[]>(1);
    scene.mutableField<UnsignedByte>(1);
    scene.mutableField<UnsignedByte[]>(1);
    scene.field<UnsignedByte>(SceneField::Mesh);
    scene.field<UnsignedByte[]>(SceneField::Mesh);
    scene.mutableField<UnsignedByte>(SceneField::Mesh);
    scene.mutableField<UnsignedByte[]>(SceneField::Mesh);
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is Trade::SceneFieldType::UnsignedShort but requested a type equivalent to Trade::SceneFieldType::UnsignedByte\n");
}

void SceneDataTest::fieldWrongPointerType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Thing {
        UnsignedInt object;
        Int* foobar;
        const Int* importerState;
    } things[2];
    Containers::StridedArrayView1D<Thing> view = things;

    SceneData scene{SceneObjectType::UnsignedInt, 5, DataFlag::Mutable, things, {
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

    std::ostringstream out;
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
    CORRADE_COMPARE(out.str(),
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct Field {
        UnsignedInt object;
        UnsignedInt mesh;
        UnsignedInt foobar;
    } fields[2]{};

    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 5, DataFlag::Mutable, fields, {
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)},
        SceneFieldData{sceneFieldCustom(35), view.slice(&Field::object), Containers::arrayCast<2, UnsignedInt>(view.slice(&Field::foobar))},
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.field<UnsignedInt[]>(0);
    scene.field<UnsignedInt>(1);
    scene.mutableField<UnsignedInt[]>(0);
    scene.mutableField<UnsignedInt>(1);
    scene.field<UnsignedInt[]>(SceneField::Mesh);
    scene.field<UnsignedInt>(sceneFieldCustom(35));
    scene.mutableField<UnsignedInt[]>(SceneField::Mesh);
    scene.mutableField<UnsignedInt>(sceneFieldCustom(35));
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::field(): Trade::SceneField::Mesh is not an array field, can't use T[] to access it\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is not an array field, can't use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::field(): Trade::SceneField::Mesh is not an array field, can't use T[] to access it\n"
        "Trade::SceneData::field(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Mesh is not an array field, can't use T[] to access it\n"
        "Trade::SceneData::mutableField(): Trade::SceneField::Custom(35) is an array field, use T[] to access it\n");
}

void SceneDataTest::parentFor() {
    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {3, -1},
        {4, 0},
        {2, 1},
        {4, 2} /* duplicate, ignored */
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

void SceneDataTest::childrenFor() {
    struct Field {
        UnsignedInt object;
        Int parent;
    } fields[]{
        {4, -1},
        {3, 0},
        {2, 1},
        {1, 0},
        {5, 0},
        {0, -1},
    };
    Containers::StridedArrayView1D<Field> view = fields;

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
        SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)}
    }};

    /* Just one child */
    CORRADE_COMPARE_AS(scene.childrenFor(3),
        Containers::arrayView<UnsignedInt>({2}),
        TestSuite::Compare::Container);

    /* More */
    CORRADE_COMPARE_AS(scene.childrenFor(-1),
        Containers::arrayView<UnsignedInt>({4, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.childrenFor(4),
        Containers::arrayView<UnsignedInt>({3, 1, 5}),
        TestSuite::Compare::Container);

    /* Object that is present in the parent array but has no children */
    CORRADE_COMPARE_AS(scene.childrenFor(5),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);

    /* Object that is not in the parent array at all */
    CORRADE_COMPARE_AS(scene.childrenFor(6),
        Containers::arrayView<UnsignedInt>({}),
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SceneData scene{SceneObjectType::UnsignedInt, 1, nullptr, {
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.transformation2DFor(0);
    scene.translationRotationScaling2DFor(0);
    CORRADE_COMPARE(out.str(),
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SceneData scene{SceneObjectType::UnsignedInt, 1, nullptr, {
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Vector2, nullptr}
    }};

    std::ostringstream out;
    Error redirectError{&out};
    scene.transformation3DFor(0);
    scene.translationRotationScaling3DFor(0);
    CORRADE_COMPARE(out.str(),
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
        /* To satisfy the requirement of having a transformation field to
           disambiguate the dimensionality */
        SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Vector3, nullptr},
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

    SceneData scene{SceneObjectType::UnsignedInt, 7, {}, fields, {
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
    arrayAppend(fieldData, SceneFieldData{SceneField::Parent, view.slice(&Field::object), view.slice(&Field::parent)});
    if(data.is2D)
        arrayAppend(fieldData, SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Vector2, nullptr});
    if(data.is3D)
        arrayAppend(fieldData, SceneFieldData{SceneField::Translation, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Vector3, nullptr});

    SceneData scene{SceneObjectType::UnsignedByte, 25, {}, fields, std::move(fieldData)};
    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(scene.children2D(),
        (data.is2D ? std::vector<UnsignedInt>{5, 0, 4} : std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.children3D(),
        (data.is3D ? std::vector<UnsignedInt>{5, 0, 4} : std::vector<UnsignedInt>{}),
        TestSuite::Compare::Container);
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

void SceneDataTest::fieldForFieldMissing() {
    SceneData scene{SceneObjectType::UnsignedInt, 7, nullptr, {}};

    CORRADE_COMPARE(scene.parentFor(6), Containers::NullOpt);
    CORRADE_COMPARE_AS(scene.childrenFor(6),
        Containers::arrayView<UnsignedInt>({}),
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    SceneData scene{SceneObjectType::UnsignedInt, 7, nullptr, {
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedInt, nullptr, SceneFieldType::Int, nullptr},
    }};

    std::ostringstream out;
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
    CORRADE_COMPARE(out.str(),
        "Trade::SceneData::findFieldObjectOffset(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::findFieldObjectOffset(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::fieldObjectOffset(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::fieldObjectOffset(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::hasFieldObject(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::hasFieldObject(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::parentFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::childrenFor(): object -2 out of bounds for 7 objects\n"
        "Trade::SceneData::childrenFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::transformation2DFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::translationRotationScaling2DFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::transformation3DFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::translationRotationScaling3DFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::meshesMaterialsFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::lightsFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::camerasFor(): object 7 out of bounds for 7 objects\n"
        "Trade::SceneData::skinsFor(): object 7 out of bounds for 7 objects\n");
}

void SceneDataTest::releaseFieldData() {
    struct Field {
        UnsignedByte object;
        UnsignedInt mesh;
    };

    Containers::Array<char> data{NoInit, 3*sizeof(Field)};
    Containers::StridedArrayView1D<Field> view = Containers::arrayCast<Field>(data);

    auto fields = Containers::array({
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    });
    SceneFieldData* originalFields = fields;

    SceneData scene{SceneObjectType::UnsignedByte, 50, std::move(data), std::move(fields)};

    Containers::Array<SceneFieldData> released = scene.releaseFieldData();
    CORRADE_COMPARE(released.data(), originalFields);
    CORRADE_COMPARE(released.size(), 2);

    /* Fields are all gone */
    CORRADE_COMPARE(static_cast<const void*>(scene.fieldData()), nullptr);
    CORRADE_COMPARE(scene.fieldCount(), 0);

    /* Data stays untouched, object count and type as well, as it con't result
       in any dangling data access */
    CORRADE_COMPARE(scene.data(), view.data());
    CORRADE_COMPARE(scene.objectCount(), 50);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedByte);
}

void SceneDataTest::releaseData() {
    struct Field {
        UnsignedByte object;
        UnsignedByte mesh;
    };

    Containers::Array<char> data{NoInit, 3*sizeof(Field)};
    Containers::StridedArrayView1D<Field> view = Containers::arrayCast<Field>(data);

    SceneData scene{SceneObjectType::UnsignedByte, 50, std::move(data), {
        SceneFieldData{SceneField::Parent, SceneObjectType::UnsignedByte, nullptr, SceneFieldType::Int, nullptr},
        SceneFieldData{SceneField::Mesh, view.slice(&Field::object), view.slice(&Field::mesh)}
    }};

    Containers::Array<char> released = scene.releaseData();
    CORRADE_COMPARE(released.data(), view.data());
    CORRADE_COMPARE(released.size(), 3*sizeof(Field));

    /* Both fields and data are all gone */
    CORRADE_COMPARE(static_cast<const void*>(scene.fieldData()), nullptr);
    CORRADE_COMPARE(scene.fieldCount(), 0);
    CORRADE_COMPARE(static_cast<const void*>(scene.data()), nullptr);

    /* Object count and type stays untouched, as it con't result in any
       dangling data access */
    CORRADE_COMPARE(scene.objectCount(), 50);
    CORRADE_COMPARE(scene.objectType(), SceneObjectType::UnsignedByte);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneDataTest)
