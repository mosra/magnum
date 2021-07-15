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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneDataTest: TestSuite::Tester {
    explicit SceneDataTest();

    void customFieldName();
    void customFieldNameTooLarge();
    void customFieldNameNotCustom();
    void debugFieldName();

    void fieldTypeSize();
    void fieldTypeSizeInvalid();
    void debugFieldType();

    void objectTypeSize();
    void objectTypeSizeInvalid();
    void debugObjectType();

    void constructField();
    void constructFieldDefault();
    void constructFieldCustom();
    void constructField2D();
    void constructFieldTypeErased();
    void constructFieldNonOwningArray();
    void constructFieldOffsetOnly();
    void constructArrayField();
    void constructArrayField2D();
    void constructArrayFieldTypeErased();
    void constructArrayFieldOffsetOnly();

    void constructFieldWrongType();
    void constructFieldInconsistentViewSize();
    void constructFieldTooLargeFieldStride();
    void constructFieldTooLargeObjectStride();
    void constructFieldWrongDataAccess();
    void constructField2DWrongSize();
    void constructField2DNonContiguous();
    void constructArrayFieldNonContiguous();
    void constructArrayFieldNotAllowed();
    void constructArrayField2DWrongSize();
    void constructArrayField2DNonContiguous();

    void construct();
    void constructZeroFields();
    void constructZeroObjects();

    void constructNotOwned();

    void constructDuplicateField();
    void constructInconsistentIndexType();
    void constructNotContained();
    void constructIndexTypeTooSmall();
    void constructNotOwnedFlagOwned();
    void constructMismatchedTRSViews();
    void constructMismatchedMaterialView();

    void constructCopy();
    void constructMove();

    template<class T> void fieldObjectMappingAsArray();
    void fieldObjectMappingAsArrayLongType();
    void fieldObjectMappingIntoArrayInvalidSize();
    template<class T> void parentsAsArray();
    void parentsAsArrayLongType();
    void parentsIntoArrayInvalidSize();
    template<class T> void transformations2DAsArray();
    template<class T> void transformations2DAsArrayTRS();
    void transformations2DAsArrayBut3DType();
    void transformations2DAsArrayBut3DTypeTRS();
    void transformations2DIntoArrayInvalidSize();
    template<class T> void transformations3DAsArray();
    template<class T> void transformationsTRS3DAsArray();
    void transformations3DIntoArrayInvalidSize();
    void transformations3DBut2DType();
    void transformations3DButTRS2DType();
    template<class T> void meshIdsAsArray();
    void meshIdsIntoArrayInvalidSize();
    template<class T> void meshMaterialIdsAsArray();
    void meshMaterialIdsIntoArrayInvalidSize();
    template<class T> void lightIdsAsArray();
    void lightIdsIntoArrayInvalidSize();
    template<class T> void cameraIdsAsArray();
    void cameraIdsIntoArrayInvalidSize();
    template<class T> void skinIdsAsArray();
    void skinIdsIntoArrayInvalidSize();

    void arrayAttribute();
    void arrayAttributeWrongAccess();

    void mutableAccessNotAllowed();

    void fieldObjectMappingNotFound();
    void fieldObjectMappingWrongType();

    void fieldNotFound();
    void fieldWrongType();

    void releaseData();
    void releaseFieldData();
};

SceneDataTest::SceneDataTest() {
    addTests({&SceneDataTest::customFieldName,
              &SceneDataTest::customFieldNameTooLarge,
              &SceneDataTest::customFieldNameNotCustom,
              &SceneDataTest::debugFieldName,

              &SceneDataTest::fieldTypeSize,
              &SceneDataTest::fieldTypeSizeInvalid,
              &SceneDataTest::debugFieldType,

              &SceneDataTest::objectTypeSize,
              &SceneDataTest::objectTypeSizeInvalid,
              &SceneDataTest::debugObjectType,

              &SceneDataTest::constructField,
              &SceneDataTest::constructFieldDefault,
              &SceneDataTest::constructFieldCustom,
              &SceneDataTest::constructField2D,
              &SceneDataTest::constructFieldTypeErased,
              &SceneDataTest::constructFieldNonOwningArray,
              &SceneDataTest::constructFieldOffsetOnly,
              &SceneDataTest::constructArrayField,
              &SceneDataTest::constructArrayField2D,
              &SceneDataTest::constructArrayFieldTypeErased,
              &SceneDataTest::constructArrayFieldOffsetOnly,

              &SceneDataTest::constructFieldWrongType,
              &SceneDataTest::constructFieldInconsistentViewSize,
              &SceneDataTest::constructFieldTooLargeFieldStride,
              &SceneDataTest::constructFieldTooLargeObjectStride,
              &SceneDataTest::constructFieldWrongDataAccess,
              &SceneDataTest::constructField2DWrongSize,
              &SceneDataTest::constructField2DNonContiguous,
              &SceneDataTest::constructArrayFieldNonContiguous,
              &SceneDataTest::constructArrayFieldNotAllowed,
              &SceneDataTest::constructArrayField2DWrongSize,
              &SceneDataTest::constructArrayField2DNonContiguous,
    });
}

using namespace Math::Literals;

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

void SceneDataTest::fieldTypeSize() {
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
}

void SceneDataTest::fieldTypeSizeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    sceneFieldTypeSize(SceneFieldType{});
    sceneFieldTypeSize(SceneFieldType(0xdead));

    CORRADE_COMPARE(out.str(),
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0x0)\n"
        "Trade::sceneFieldTypeSize(): invalid type Trade::SceneFieldType(0xdead)\n");
}

void SceneDataTest::debugFieldType() {
    std::ostringstream out;
    Debug{&out} << SceneFieldType::Matrix3x4h << SceneFieldType(0xdead);
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldType::Matrix3x4h Trade::SceneFieldType(0xdead)\n");
}

void SceneDataTest::objectTypeSize() {
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedByte), 1);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedShort), 2);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedInt), 4);
    CORRADE_COMPARE(sceneObjectTypeSize(SceneObjectType::UnsignedLong), 8);
}

void SceneDataTest::objectTypeSizeInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    sceneObjectTypeSize(SceneObjectType{});
    sceneObjectTypeSize(SceneObjectType(0x73));

    CORRADE_COMPARE(out.str(),
        "Trade::sceneObjectTypeSize(): invalid type Trade::SceneObjectType(0x0)\n"
        "Trade::sceneObjectTypeSize(): invalid type Trade::SceneObjectType(0x73)\n");
}

void SceneDataTest::debugObjectType() {
    std::ostringstream out;
    Debug{&out} << SceneObjectType::UnsignedLong << SceneObjectType(0x73);
    CORRADE_COMPARE(out.str(), "Trade::SceneObjectType::UnsignedLong Trade::SceneObjectType(0x73)\n");
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
    const Complexd rotationData[3];
    const UnsignedShort rotationObjectData[3]{};

    SceneFieldData rotations{SceneField::Rotation, Containers::arrayView(rotationData), Containers::arrayView(rotationObjectData)};
    CORRADE_VERIFY(!rotations.isOffsetOnly());
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(rotations.arraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), sizeof(Complexd));
    CORRADE_VERIFY(rotations.fieldData().data() == rotationData);
    CORRADE_COMPARE(rotations.objectData().size(), 3);
    CORRADE_COMPARE(rotations.objectData().stride(), sizeof(UnsignedShort));
    CORRADE_VERIFY(rotations.objectData().data() == rotationObjectData);

    /* This is allowed too for simplicity, the parameter has to be large enough
       tho */
    char someArray[3*sizeof(Complexd)];
    CORRADE_COMPARE(rotations.fieldData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.fieldData(someArray).stride(), sizeof(Complexd));
    CORRADE_VERIFY(rotations.fieldData(someArray).data() == rotationData);
    CORRADE_COMPARE(rotations.objectData(someArray).size(), 3);
    CORRADE_COMPARE(rotations.objectData(someArray).stride(), sizeof(UnsignedShort));
    CORRADE_VERIFY(rotations.objectData(someArray).data() == rotationObjectData);

    constexpr SceneFieldData crotations{SceneField::Rotation, Containers::arrayView(Rotations2D), Containers::arrayView(RotationObjects2D)};
    constexpr bool isOffsetOnly = crotations.isOffsetOnly();
    constexpr SceneField name = crotations.name();
    constexpr SceneFieldType fieldType = crotations.fieldType();
    constexpr SceneObjectType objectType = crotations.objectType();
    constexpr UnsignedShort arraySize = crotations.arraySize();
    constexpr Containers::StridedArrayView1D<const void> fieldData = crotations.fieldData();
    constexpr Containers::StridedArrayView1D<const void> objectData = crotations.objectData();
    CORRADE_VERIFY(!isOffsetOnly);
    CORRADE_COMPARE(name, SceneField::Rotation);
    CORRADE_COMPARE(fieldType, SceneFieldType::Complexd);
    CORRADE_COMPARE(objectType, SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(arraySize, 0);
    CORRADE_COMPARE(fieldData.size(), 3);
    CORRADE_COMPARE(fieldData.stride(), sizeof(Complexd));
    CORRADE_COMPARE(fieldData.data(), Rotations2D);
    CORRADE_COMPARE(objectData.size(), 3);
    CORRADE_COMPARE(objectData.stride(), sizeof(UnsignedShort));
    CORRADE_COMPARE(objectData.data(), RotationObjects2D);
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

    const Range2Dh rangeData[3];
    const UnsignedByte rangeObjectData[3]{};
    SceneFieldData ranges{sceneFieldCustom(13), Containers::arrayView(rangeData), Containers::arrayView(rangeObjectData)};
    CORRADE_COMPARE(ranges.name(), sceneFieldCustom(13));
    CORRADE_COMPARE(ranges.fieldType(), SceneFieldType::Range2Dh);
    CORRADE_COMPARE(ranges.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_VERIFY(ranges.fieldData().data() == rangeData);
    CORRADE_VERIFY(ranges.objectData().data() == rangeObjectData);
}

void SceneDataTest::constructField2D() {
    char rotationData[6*sizeof(Complexd)];
    char rotationObjectData[6*sizeof(UnsignedShort)];
    auto rotationView = Containers::StridedArrayView2D<char>{rotationData, {6, sizeof(Complexd)}}.every(2);
    auto rotationObjectView = Containers::StridedArrayView2D<char>{rotationObjectData, {6, sizeof(UnsignedShort)}}.every(2);

    SceneFieldData rotations{SceneField::Rotation, SceneFieldType::Complexd, rotationView, rotationObjectView};
    CORRADE_VERIFY(!rotations.isOffsetOnly());
    CORRADE_COMPARE(rotations.name(), SceneField::Rotation);
    CORRADE_COMPARE(rotations.fieldType(), SceneFieldType::Complexd);
    CORRADE_COMPARE(rotations.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(rotations.arraySize(), 0);
    CORRADE_COMPARE(rotations.fieldData().size(), 3);
    CORRADE_COMPARE(rotations.fieldData().stride(), 2*sizeof(Complexd));
    CORRADE_COMPARE(rotations.fieldData().data(), rotationView.data());
    CORRADE_COMPARE(rotations.objectData().size(), 3);
    CORRADE_COMPARE(rotations.objectData().stride(), 2*sizeof(UnsignedShort));
    CORRADE_COMPARE(rotations.objectData().data(), rotationObjectView.data());
}

void SceneDataTest::constructFieldTypeErased() {
    const Vector3 scalingData[3];
    const UnsignedLong scalingObjectData[3]{};
    SceneFieldData scalings{SceneField::Scaling, SceneFieldType::Vector3, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingData)), SceneObjectType::UnsignedLong, Containers::arrayCast<const char>(Containers::stridedArrayView(scalingObjectData))};
    CORRADE_VERIFY(!scalings.isOffsetOnly());
    CORRADE_COMPARE(scalings.name(), SceneField::Scaling);
    CORRADE_COMPARE(scalings.fieldType(), SceneFieldType::Vector3);
    CORRADE_COMPARE(scalings.objectType(), SceneObjectType::UnsignedLong);
    CORRADE_COMPARE(scalings.arraySize(), 0);
    CORRADE_COMPARE(scalings.fieldData().size(), 3);
    CORRADE_COMPARE(scalings.fieldData().stride(), sizeof(Vector3));
    CORRADE_COMPARE(scalings.fieldData().data(), scalingData);
    CORRADE_COMPARE(scalings.objectData().size(), 3);
    CORRADE_COMPARE(scalings.objectData().stride(), sizeof(UnsignedLong));
    CORRADE_COMPARE(scalings.objectData().data(), scalingObjectData);
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
        {{}, 2, {2.0f, 3.0f}},
        {{}, 15, {67.0f, -1.1f}}
    };

    SceneFieldData a{SceneField::Translation, 2, SceneFieldType::Vector2, offsetof(Data, translation), sizeof(Data), SceneObjectType::UnsignedShort, offsetof(Data, object), sizeof(Data)};
    CORRADE_VERIFY(a.isOffsetOnly());
    CORRADE_COMPARE(a.name(), SceneField::Translation);
    CORRADE_COMPARE(a.fieldType(), SceneFieldType::Vector2);
    CORRADE_COMPARE(a.objectType(), SceneObjectType::UnsignedShort);
    CORRADE_COMPARE(a.arraySize(), 0);
    CORRADE_COMPARE(a.fieldData(data).size(), 2);
    CORRADE_COMPARE(a.fieldData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(a.fieldData(data)),
        Containers::arrayView<Vector2>({{2.0f, 3.0f}, {67.0f, -1.1f}}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(a.objectData(data).size(), 2);
    CORRADE_COMPARE(a.objectData(data).stride(), sizeof(Data));
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(a.objectData(data)),
        Containers::arrayView<UnsignedShort>({2, 15}),
        TestSuite::Compare::Container);
}

constexpr Int ArrayOffsetData[3*4]{};
constexpr UnsignedByte ArrayOffsetObjectData[3]{};

void SceneDataTest::constructArrayField() {
    Int offsetData[3*4];
    UnsignedByte offsetObjectData[3];
    SceneFieldData data{sceneFieldCustom(34), Containers::StridedArrayView2D<Int>{offsetData, {3, 4}}, Containers::arrayView(offsetObjectData)};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetData);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), Containers::StridedArrayView2D<const Int>{ArrayOffsetData, {3, 4}}, Containers::arrayView(ArrayOffsetObjectData)};
    CORRADE_VERIFY(!cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(cdata.arraySize(), 4);
    CORRADE_COMPARE(cdata.fieldData().size(), 3);
    CORRADE_COMPARE(cdata.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(cdata.fieldData().data() == ArrayOffsetData);
    CORRADE_COMPARE(cdata.objectData().size(), 3);
    CORRADE_COMPARE(cdata.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(cdata.objectData().data() == ArrayOffsetObjectData);
}

void SceneDataTest::constructArrayField2D() {
    char offsetData[3*4*sizeof(Int)];
    char offsetObjectData[3*sizeof(UnsignedByte)];
    SceneFieldData data{sceneFieldCustom(34), SceneFieldType::Int, Containers::StridedArrayView2D<char>{offsetData, {3, 4*sizeof(Int)}}, Containers::StridedArrayView2D<char>{offsetObjectData, {3, sizeof(UnsignedByte)}}, 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetData);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);
}

void SceneDataTest::constructArrayFieldTypeErased() {
    Int offsetData[3*4];
    Containers::StridedArrayView1D<Int> offset{offsetData, 3, 4*sizeof(Int)};
    UnsignedByte offsetObjectData[3];
    SceneFieldData data{sceneFieldCustom(34), SceneFieldType::Int, Containers::arrayCast<const char>(offset), SceneObjectType::UnsignedByte, Containers::arrayCast<const char>(Containers::stridedArrayView(offsetObjectData)), 4};
    CORRADE_VERIFY(!data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.arraySize(), 4);
    CORRADE_COMPARE(data.fieldData().size(), 3);
    CORRADE_COMPARE(data.fieldData().stride(), 4*sizeof(Int));
    CORRADE_VERIFY(data.fieldData().data() == offsetData);
    CORRADE_COMPARE(data.objectData().size(), 3);
    CORRADE_COMPARE(data.objectData().stride(), sizeof(UnsignedByte));
    CORRADE_VERIFY(data.objectData().data() == offsetObjectData);
}

void SceneDataTest::constructArrayFieldOffsetOnly() {
    struct Data {
        Byte parent;
        UnsignedByte object;
        Int offset[4];
    };

    SceneFieldData data{sceneFieldCustom(34), 3, SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), SceneObjectType::UnsignedByte, offsetof(Data, object), sizeof(Data), 4};
    CORRADE_VERIFY(data.isOffsetOnly());
    CORRADE_COMPARE(data.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(data.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(data.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(data.arraySize(), 4);

    Data actual[3];
    CORRADE_COMPARE(data.fieldData(actual).size(), 3);
    CORRADE_COMPARE(data.fieldData(actual).stride(), sizeof(Data));
    CORRADE_VERIFY(data.fieldData(actual).data() == &actual[0].offset);
    CORRADE_COMPARE(data.objectData(actual).size(), 3);
    CORRADE_COMPARE(data.objectData(actual).stride(), sizeof(Data));
    CORRADE_VERIFY(data.objectData(actual).data() == &actual[0].object);

    constexpr SceneFieldData cdata{sceneFieldCustom(34), 3, SceneFieldType::Int, offsetof(Data, offset), sizeof(Data), SceneObjectType::UnsignedByte, offsetof(Data, object), sizeof(Data), 4};
    CORRADE_VERIFY(cdata.isOffsetOnly());
    CORRADE_COMPARE(cdata.name(), sceneFieldCustom(34));
    CORRADE_COMPARE(cdata.fieldType(), SceneFieldType::Int);
    CORRADE_COMPARE(cdata.objectType(), SceneObjectType::UnsignedByte);
    CORRADE_COMPARE(cdata.arraySize(), 4);
}

void SceneDataTest::constructFieldInconsistentViewSize() {
    const Complexd rotationData[2];
    const UnsignedShort rotationObjectData[3]{};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation, Containers::arrayView(rotationData), Containers::arrayView(rotationObjectData)};
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldData: expected field and object view to have the same size but got 2 and 3\n");
}

void SceneDataTest::constructFieldWrongType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const Quaternion rotationData[3];
    const UnsignedShort rotationObjectData[3]{};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Transformation, Containers::arrayView(rotationData), Containers::arrayView(rotationObjectData)};
    SceneFieldData{SceneField::Transformation, 3, SceneFieldType::Quaternion, 0, sizeof(Quaternion), SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort)};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n"
        "Trade::SceneFieldData: Trade::SceneFieldType::Quaternion is not a valid type for Trade::SceneField::Transformation\n");
}

void SceneDataTest::constructFieldTooLargeFieldStride() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}, SceneObjectType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>(), SceneObjectType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 32767, SceneObjectType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 65536, -32768, SceneObjectType::UnsignedInt, 0, 4};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}, SceneObjectType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>(), SceneObjectType::UnsignedInt, enough};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 32768, SceneObjectType::UnsignedInt, 0, 4};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 65538, -32769, SceneObjectType::UnsignedInt, 0, 4};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got -32769\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got -32769\n");
}

void SceneDataTest::constructFieldTooLargeObjectStride() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    UnsignedInt enough[2];
    char toomuch[2*(32768 + sizeof(UnsignedInt))];

    /* These should be fine */
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, enough, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32767}};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, enough, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 4, SceneObjectType::UnsignedInt, 0, 32767};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 4, SceneObjectType::UnsignedInt, 65536, -32768};

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, enough, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32768}};
    SceneFieldData{SceneField::Mesh, SceneFieldType::UnsignedInt, enough, SceneObjectType::UnsignedInt, Containers::StridedArrayView1D<UnsignedInt>{Containers::arrayCast<UnsignedInt>(toomuch), 2, 32769}.flipped<0>()};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 4, SceneObjectType::UnsignedInt, 0, 32768};
    SceneFieldData{SceneField::Mesh, 2, SceneFieldType::UnsignedInt, 0, 4, SceneObjectType::UnsignedInt, 65538, -32769};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got -32769\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got 32768\n"
        "Trade::SceneFieldData: expected object view stride to fit into 16 bits, but got -32769\n");
}

void SceneDataTest::constructFieldWrongDataAccess() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    const Quaternion rotationData[3];
    const UnsignedShort rotationObjectData[3]{};
    SceneFieldData a{SceneField::Rotation, Containers::arrayView(rotationData), Containers::arrayView(rotationObjectData)};
    SceneFieldData b{SceneField::Rotation, 3, SceneFieldType::Quaternion, 0, sizeof(Quaternion), SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort)};
    CORRADE_VERIFY(!a.isOffsetOnly());
    CORRADE_VERIFY(b.isOffsetOnly());

    a.fieldData(rotationData); /* This is fine, no asserts */
    a.objectData(rotationObjectData);

    std::ostringstream out;
    Error redirectError{&out};
    b.fieldData();
    b.objectData();
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData::fieldData(): the field is offset-only, supply a data array\n"
        "Trade::SceneFieldData::objectData(): the field is offset-only, supply a data array\n");
}

void SceneDataTest::constructField2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationData[5*sizeof(Complex)];
    char rotationObjectData[5*sizeof(UnsignedInt)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Translation, SceneFieldType::Vector3,
        Containers::StridedArrayView2D<char>{rotationData, {4, sizeof(Complex)}}.every(2),
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}}.every(2)};
    SceneFieldData{SceneField::Rotation, SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationData, {4, sizeof(Complex)}}.every(2),
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, 5}}.every(2)};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Vector3\n"
        "Trade::SceneFieldData: expected second object view dimension size 1, 2, 4 or 8 but got 5\n");
}

void SceneDataTest::constructField2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationData[8*sizeof(Complex)];
    char rotationObjectData[8*sizeof(UnsignedInt)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation, SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationData, {4, 2*sizeof(Complex)}}.every({1, 2}),
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}}};
    SceneFieldData{SceneField::Rotation, SceneFieldType::Complex,
        Containers::StridedArrayView2D<char>{rotationData, {4, sizeof(Complex)}},
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, 2*sizeof(UnsignedInt)}}.every({1, 2})};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second field view dimension is not contiguous\n"
        "Trade::SceneFieldData: second object view dimension is not contiguous\n");
}

void SceneDataTest::constructArrayFieldNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Int offsetData[3*4];
    UnsignedByte offsetObjectData[3];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData data{sceneFieldCustom(34), Containers::StridedArrayView2D<Int>{offsetData, {3, 4}}.every({1, 2}), Containers::arrayView(offsetObjectData)};
    CORRADE_COMPARE(out.str(), "Trade::SceneFieldData: second field view dimension is not contiguous\n");
}

void SceneDataTest::constructArrayFieldNotAllowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Quaternion rotationData[3];
    UnsignedShort rotationObjectData[3]{};
    Containers::ArrayView<Quaternion> rotations = rotationData;
    Containers::ArrayView<UnsignedShort> rotationObjects = rotationObjectData;
    Containers::StridedArrayView2D<Quaternion> rotations2D{rotationData, {3, 3}, {0, sizeof(Quaternion)}};
    auto rotations2DChar = Containers::arrayCast<2, const char>(rotations2D);
    auto rotationObjectsChar = Containers::arrayCast<2, const char>(rotationObjects);

    /* This is all fine */
    SceneFieldData{SceneField::Rotation,
        SceneFieldType::Quaternion, rotations,
        SceneObjectType::UnsignedShort, rotationObjects, 0};
    SceneFieldData{SceneField::Rotation, 3,
        SceneFieldType::Quaternion, 0, sizeof(Quaternion),
        SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort), 0};
    SceneFieldData{sceneFieldCustom(37),
        rotations2D,
        rotationObjects};
    SceneFieldData{sceneFieldCustom(37),
        SceneFieldType::Quaternion, rotations2DChar,
        rotationObjectsChar, 3};
    SceneFieldData{sceneFieldCustom(37), 3,
        SceneFieldType::Quaternion, 0, sizeof(Quaternion), SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort), 3};

    /* This is not */
    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{SceneField::Rotation,
        SceneFieldType::Quaternion, rotations,
        SceneObjectType::UnsignedShort, rotationObjects, 3};
    SceneFieldData{SceneField::Rotation, 3,
        SceneFieldType::Quaternion, 0, sizeof(Quaternion),
        SceneObjectType::UnsignedShort, 0, sizeof(UnsignedShort), 3};
    SceneFieldData{SceneField::Rotation,
        rotations2D,
        rotationObjects};
    SceneFieldData{SceneField::Rotation,
        SceneFieldType::Quaternion, rotations2DChar,
        rotationObjectsChar, 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n"
        "Trade::SceneFieldData: Trade::SceneField::Rotation can't be an array field\n");
}

void SceneDataTest::constructArrayField2DWrongSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char rotationData[4*sizeof(Complex)];
    char rotationObjectData[4*sizeof(UnsignedInt)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37), SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{rotationData, {4, sizeof(Complex)}}.every(2),
        Containers::StridedArrayView2D<char>{rotationObjectData, {4, sizeof(UnsignedInt)}}.every(2), 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second field view dimension size 8 doesn't match Trade::SceneFieldType::Int and array size 3\n");
}

void SceneDataTest::constructArrayField2DNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char offsetData[18*sizeof(Int)];
    char offsetObjectData[18*sizeof(UnsignedInt)];

    std::ostringstream out;
    Error redirectError{&out};
    SceneFieldData{sceneFieldCustom(37), SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetData, {3, 6*sizeof(Int)}}.every({1, 2}),
        Containers::StridedArrayView2D<char>{offsetObjectData, {3, sizeof(UnsignedInt)}}, 3};
    SceneFieldData{sceneFieldCustom(37), SceneFieldType::Int,
        Containers::StridedArrayView2D<char>{offsetData, {3, 3*sizeof(Int)}},
        Containers::StridedArrayView2D<char>{offsetObjectData, {3, 2*sizeof(UnsignedInt)}}.every({1, 2}), 3};
    CORRADE_COMPARE(out.str(),
        "Trade::SceneFieldData: second field view dimension is not contiguous\n"
        "Trade::SceneFieldData: second object view dimension is not contiguous\n");
}

void SceneDataTest::construct() {

}

void SceneDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<SceneData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<SceneData>{});
}

void SceneDataTest::constructMove() {

}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneDataTest)
