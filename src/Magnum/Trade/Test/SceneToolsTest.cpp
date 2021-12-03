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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Trade/Implementation/sceneTools.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneToolsTest: TestSuite::Tester {
    explicit SceneToolsTest();

    void combine();
    void combineAlignment();
    void combineObjectsShared();
    void combineObjectsPlaceholderFieldPlaceholder();
    void combineObjectSharedFieldPlaceholder();

    void convertToSingleFunctionObjects();
};

struct {
    const char* name;
    SceneMappingType objectType;
} CombineData[]{
    {"UnsignedByte output", SceneMappingType::UnsignedByte},
    {"UnsignedShort output", SceneMappingType::UnsignedShort},
    {"UnsignedInt output", SceneMappingType::UnsignedInt},
    {"UnsignedLong output", SceneMappingType::UnsignedLong},
};

struct {
    const char* name;
    UnsignedLong originalObjectCount;
    UnsignedLong expectedObjectCount;
} ConvertToSingleFunctionObjectsData[]{
    {"original object count smaller than new", 64, 67},
    {"original object count larger than new", 96, 96}
};

SceneToolsTest::SceneToolsTest() {
    addInstancedTests({&SceneToolsTest::combine},
        Containers::arraySize(CombineData));

    addTests({&SceneToolsTest::combineAlignment,
              &SceneToolsTest::combineObjectsShared,
              &SceneToolsTest::combineObjectsPlaceholderFieldPlaceholder,
              &SceneToolsTest::combineObjectSharedFieldPlaceholder});

    addInstancedTests({&SceneToolsTest::convertToSingleFunctionObjects},
        Containers::arraySize(ConvertToSingleFunctionObjectsData));
}

using namespace Math::Literals;

void SceneToolsTest::combine() {
    auto&& data = CombineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Testing the four possible object types, it should be possible to combine
       them */

    const UnsignedInt meshMappingData[]{45, 78, 23};
    const UnsignedByte meshFieldData[]{3, 5, 17};

    const UnsignedShort parentMappingData[]{33, 25};
    const Short parentData[]{-1, 33};

    const UnsignedByte translationMappingData[]{16};
    const Vector2d translationFieldData[]{{1.5, -0.5}};

    const UnsignedLong fooMappingData[]{15, 23};
    const Int fooFieldData[]{0, 1, 2, 3};

    SceneData scene = Implementation::sceneCombine(data.objectType, 167, Containers::arrayView({
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        SceneFieldData{SceneField::Parent, Containers::arrayView(parentMappingData), Containers::arrayView(parentData)},
        SceneFieldData{SceneField::Translation, Containers::arrayView(translationMappingData), Containers::arrayView(translationFieldData)},
        /* Array field */
        SceneFieldData{sceneFieldCustom(15), Containers::arrayView(fooMappingData), Containers::StridedArrayView2D<const Int>{fooFieldData, {2, 2}}},
        /* Empty field */
        SceneFieldData{SceneField::Camera, Containers::ArrayView<const UnsignedByte>{}, Containers::ArrayView<const UnsignedShort>{}}
    }));

    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), data.objectType);
    CORRADE_COMPARE(scene.mappingBound(), 167);
    CORRADE_COMPARE(scene.fieldCount(), 5);

    CORRADE_COMPARE(scene.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldType(0), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(0), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(0), Containers::arrayView<UnsignedInt>({
        45, 78, 23
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(1), SceneField::Parent);
    CORRADE_COMPARE(scene.fieldType(1), SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldArraySize(1), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(1), Containers::arrayView<UnsignedInt>({
        33, 25
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Short>(1),
        Containers::arrayView(parentData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(2), SceneField::Translation);
    CORRADE_COMPARE(scene.fieldType(2), SceneFieldType::Vector2d);
    CORRADE_COMPARE(scene.fieldArraySize(2), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(2),
        Containers::arrayView<UnsignedInt>({16}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Vector2d>(2),
        Containers::arrayView(translationFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(3), sceneFieldCustom(15));
    CORRADE_COMPARE(scene.fieldType(3), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldArraySize(3), 2);
    CORRADE_COMPARE_AS(scene.mappingAsArray(3),
        Containers::arrayView<UnsignedInt>({15, 23}),
        TestSuite::Compare::Container);
    /** @todo clean up once it's possible to compare multidimensional
        containers */
    CORRADE_COMPARE_AS(scene.field<Int[]>(3)[0],
        (Containers::StridedArrayView2D<const Int>{fooFieldData, {2, 2}})[0],
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Int[]>(3)[1],
        (Containers::StridedArrayView2D<const Int>{fooFieldData, {2, 2}})[1],
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(4), SceneField::Camera);
    CORRADE_COMPARE(scene.fieldType(4), SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldSize(4), 0);
    CORRADE_COMPARE(scene.fieldArraySize(4), 0);
}

void SceneToolsTest::combineAlignment() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};
    const UnsignedShort translationMappingData[]{5}; /* 1 byte padding before */
    const Vector2d translationFieldData[]{{1.5, 3.0}}; /* 4 byte padding before */

    SceneData scene = Implementation::sceneCombine(SceneMappingType::UnsignedShort, 167, Containers::arrayView({
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        SceneFieldData{SceneField::Translation, Containers::arrayView(translationMappingData), Containers::arrayView(translationFieldData)}
    }));

    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.mappingBound(), 167);
    CORRADE_COMPARE(scene.fieldCount(), 2);

    CORRADE_COMPARE(scene.fieldName(0), SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldType(0), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(0), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(0),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(reinterpret_cast<std::ptrdiff_t>(scene.mapping(0).data()), 2, TestSuite::Compare::Divisible);
    CORRADE_COMPARE(scene.mapping(0).data(), scene.data());
    CORRADE_COMPARE(scene.mapping(0).stride()[0], 2);
    CORRADE_COMPARE_AS(reinterpret_cast<std::ptrdiff_t>(scene.field(0).data()), 1, TestSuite::Compare::Divisible);
    CORRADE_COMPARE(scene.field(0).data(), scene.data() + 3*2);
    CORRADE_COMPARE(scene.field(0).stride()[0], 1);

    CORRADE_COMPARE(scene.fieldName(1), SceneField::Translation);
    CORRADE_COMPARE(scene.fieldType(1), SceneFieldType::Vector2d);
    CORRADE_COMPARE(scene.fieldArraySize(1), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(1),
        Containers::arrayView(translationMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Vector2d>(1),
        Containers::arrayView(translationFieldData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(reinterpret_cast<std::ptrdiff_t>(scene.mapping(1).data()), 2, TestSuite::Compare::Divisible);
    CORRADE_COMPARE(scene.mapping(1).data(), scene.data() + 3*2 + 3 + 1);
    CORRADE_COMPARE(scene.mapping(1).stride()[0], 2);
    CORRADE_COMPARE_AS(reinterpret_cast<std::ptrdiff_t>(scene.field(1).data()), 8, TestSuite::Compare::Divisible);
    CORRADE_COMPARE(scene.field(1).data(), scene.data() + 3*2 + 3 + 1 + 2 + 4);
    CORRADE_COMPARE(scene.field(1).stride()[0], 16);
}

void SceneToolsTest::combineObjectsShared() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};
    const Int meshMaterialFieldData[]{72, -1, 23};

    const UnsignedShort translationRotationMappingData[]{14, 22};
    const Vector2 translationFieldData[]{{-1.0f, 25.3f}, {2.2f, 2.1f}};
    const Complex rotationFieldData[]{Complex::rotation(35.0_degf), Complex::rotation(22.5_degf)};

    SceneData scene = Implementation::sceneCombine(SceneMappingType::UnsignedInt, 173, Containers::arrayView({
        /* Deliberately in an arbitrary order to avoid false assumptions like
           fields sharing the same object mapping always being after each
           other */
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        SceneFieldData{SceneField::Translation, Containers::arrayView(translationRotationMappingData), Containers::arrayView(translationFieldData)},
        SceneFieldData{SceneField::MeshMaterial, Containers::arrayView(meshMappingData), Containers::arrayView(meshMaterialFieldData)},
        SceneFieldData{SceneField::Rotation, Containers::arrayView(translationRotationMappingData), Containers::arrayView(rotationFieldData)}
    }));

    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 4);

    CORRADE_COMPARE(scene.fieldSize(SceneField::Mesh), 3);
    CORRADE_COMPARE(scene.fieldSize(SceneField::MeshMaterial), 3);
    CORRADE_COMPARE(scene.mapping(SceneField::Mesh).data(), scene.mapping(SceneField::MeshMaterial).data());

    CORRADE_COMPARE(scene.fieldSize(SceneField::Translation), 2);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Rotation), 2);
    CORRADE_COMPARE(scene.mapping(SceneField::Translation).data(), scene.mapping(SceneField::Rotation).data());
}

void SceneToolsTest::combineObjectsPlaceholderFieldPlaceholder() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    SceneData scene = Implementation::sceneCombine(SceneMappingType::UnsignedShort, 173, Containers::arrayView({
        SceneFieldData{SceneField::Camera, Containers::ArrayView<UnsignedByte>{nullptr, 1}, Containers::ArrayView<UnsignedShort>{nullptr, 1}},
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        /* Looks like sharing object mapping with the Camera field, but
           actually both are placeholders */
        SceneFieldData{SceneField::Light, Containers::ArrayView<UnsignedShort>{nullptr, 2}, Containers::ArrayView<UnsignedInt>{nullptr, 2}},
        /* Array field */
        SceneFieldData{sceneFieldCustom(15), Containers::ArrayView<UnsignedShort>{nullptr, 2}, Containers::StridedArrayView2D<Short>{{nullptr, 16}, {2, 4}}},
    }));

    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 4);

    CORRADE_COMPARE(scene.fieldType(SceneField::Camera), SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Camera), 1);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Camera), 0);
    CORRADE_COMPARE(scene.mapping(SceneField::Camera).data(), scene.data());
    CORRADE_COMPARE(scene.mapping(SceneField::Camera).stride()[0], 2);
    CORRADE_COMPARE(scene.field(SceneField::Camera).data(), scene.data() + 2);
    CORRADE_COMPARE(scene.field(SceneField::Camera).stride()[0], 2);

    CORRADE_COMPARE(scene.fieldType(SceneField::Mesh), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Mesh), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(SceneField::Mesh),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(SceneField::Mesh),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldType(SceneField::Light), SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(scene.fieldSize(SceneField::Light), 2);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Light), 0);
    CORRADE_COMPARE(scene.mapping(SceneField::Light).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1);
    CORRADE_COMPARE(scene.mapping(SceneField::Light).stride()[0], 2);
    CORRADE_COMPARE(scene.field(SceneField::Light).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2);
    CORRADE_COMPARE(scene.field(SceneField::Light).stride()[0], 4);

    CORRADE_COMPARE(scene.fieldType(sceneFieldCustom(15)), SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldSize(sceneFieldCustom(15)), 2);
    CORRADE_COMPARE(scene.fieldArraySize(sceneFieldCustom(15)), 4);
    CORRADE_COMPARE(scene.mapping(sceneFieldCustom(15)).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4);
    CORRADE_COMPARE(scene.mapping(sceneFieldCustom(15)).stride()[0], 2);
    CORRADE_COMPARE(scene.field(sceneFieldCustom(15)).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4 + 2*2);
    CORRADE_COMPARE(scene.field(sceneFieldCustom(15)).stride()[0], 4*2);
}

void SceneToolsTest::combineObjectSharedFieldPlaceholder() {
    const UnsignedInt meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    SceneData scene = Implementation::sceneCombine(SceneMappingType::UnsignedInt, 173, Containers::arrayView({
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        SceneFieldData{SceneField::MeshMaterial, Containers::arrayView(meshMappingData), Containers::ArrayView<Int>{nullptr, 3}},
    }));

    CORRADE_COMPARE(scene.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 2);

    CORRADE_COMPARE(scene.fieldType(SceneField::Mesh), SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::Mesh), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(0),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldType(SceneField::MeshMaterial), SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldSize(SceneField::MeshMaterial), 3);
    CORRADE_COMPARE(scene.fieldArraySize(SceneField::MeshMaterial), 0);
    CORRADE_COMPARE(scene.mapping(SceneField::MeshMaterial).data(), scene.mapping(SceneField::Mesh).data());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(SceneField::MeshMaterial),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.field(SceneField::MeshMaterial).data(), scene.data() + 3*4 + 3 + 1);
    CORRADE_COMPARE(scene.field(SceneField::MeshMaterial).stride()[0], 4);
}

void SceneToolsTest::convertToSingleFunctionObjects() {
    auto&& data = ConvertToSingleFunctionObjectsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Haha now I can use sceneCombine() to conveniently prepare the initial
       state here, without having to mess with an ArrayTuple */

    const UnsignedShort parentMappingData[]{15, 21, 22, 23, 1};
    const Byte parentFieldData[]{-1, -1, 21, 22, -1};

    /* Two objects have two and three mesh assignments respectively, meaning we
       need three extra */
    const UnsignedShort meshMappingData[]{15, 23, 23, 23, 1, 15, 21};
    const Containers::Pair<UnsignedInt, Int> meshMaterialFieldData[]{
        {6, 4},
        {1, 0},
        {2, 3},
        {4, 2},
        {7, 2},
        {3, 1},
        {5, -1}
    };

    /* One camera is attached to an object that already has a mesh, meaning we
       need a third extra object */
    const UnsignedShort cameraMappingData[]{22, 1};
    const UnsignedInt cameraFieldData[]{1, 5};
    SceneData original = Implementation::sceneCombine(SceneMappingType::UnsignedShort, data.originalObjectCount, Containers::arrayView({
        SceneFieldData{SceneField::Parent, Containers::arrayView(parentMappingData), Containers::arrayView(parentFieldData)},
        SceneFieldData{SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::StridedArrayView1D<const UnsignedInt>{meshMaterialFieldData, &meshMaterialFieldData[0].first(), Containers::arraySize(meshMaterialFieldData), sizeof(meshMaterialFieldData[0])}},
        SceneFieldData{SceneField::MeshMaterial, Containers::arrayView(meshMappingData), Containers::StridedArrayView1D<const Int>{meshMaterialFieldData, &meshMaterialFieldData[0].second(), Containers::arraySize(meshMaterialFieldData), sizeof(meshMaterialFieldData[0])}},
        SceneFieldData{SceneField::Camera, Containers::arrayView(cameraMappingData), Containers::arrayView(cameraFieldData)},
    }));

    SceneData scene = Implementation::sceneConvertToSingleFunctionObjects(original, Containers::arrayView({
        SceneField::Mesh,
        /* Deliberately not including MeshMaterial in the list -- these should
           get automatically updated as they share the same object mapping.
           OTOH including them would break the output. */
        SceneField::Camera,
        /* Include also a field that's not present -- it should get skipped */
        SceneField::ImporterState
    }), 63);

    /* There should be three more objects, or the original count preserved if
       it's large enough */
    CORRADE_COMPARE(scene.mappingBound(), data.expectedObjectCount);

    /* Object 1 should have a new child that has the camera, as it has a mesh */
    CORRADE_COMPARE_AS(scene.childrenFor(1),
        Containers::arrayView<UnsignedInt>({66}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(1),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{7, 2}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(1),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(66),
        Containers::arrayView<UnsignedInt>({5}),
        TestSuite::Compare::Container);

    /* Object 15 should have a new child that has the second mesh */
    CORRADE_COMPARE_AS(scene.childrenFor(15),
        Containers::arrayView<UnsignedInt>({65}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(15),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{6, 4}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(65),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{3, 1}})),
        TestSuite::Compare::Container);

    /* Object 23 should have two new children that have the second and third
       mesh */
    CORRADE_COMPARE_AS(scene.childrenFor(23),
        Containers::arrayView<UnsignedInt>({63, 64}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(23),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{1, 0}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(63),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{2, 3}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(64),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{4, 2}})),
        TestSuite::Compare::Container);

    /* To be extra sure, verify the actual data. Parents have a few objects
       added, the rest is the same */
    CORRADE_COMPARE_AS(scene.parentsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        {15, -1},
        {21, -1},
        {22, 21},
        {23, 22},
        {1, -1},
        {63, 23},
        {64, 23},
        {65, 15},
        {66, 1}
    })), TestSuite::Compare::Container);

    /* Meshes / materials have certain objects reassigned, field data stay the
       same */
    CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>({
        {15, {6, 4}},
        {23, {1, 0}},
        {63, {2, 3}},
        {64, {4, 2}},
        {1, {7, 2}},
        {65, {3, 1}},
        {21, {5, -1}}
    })), TestSuite::Compare::Container);

    /* Cameras have certain objects reassigned, field data stay the same */
    CORRADE_COMPARE_AS(scene.camerasAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {22, 1},
        {66, 5}
    })), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneToolsTest)
