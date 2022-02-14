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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/SceneTools/Implementation/combine.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct CombineTest: TestSuite::Tester {
    explicit CombineTest();

    void test();
    void alignment();
    void objectsShared();
    void objectsPlaceholderFieldPlaceholder();
    void objectSharedFieldPlaceholder();
};

struct {
    const char* name;
    Trade::SceneMappingType objectType;
} TestData[]{
    {"UnsignedByte output", Trade::SceneMappingType::UnsignedByte},
    {"UnsignedShort output", Trade::SceneMappingType::UnsignedShort},
    {"UnsignedInt output", Trade::SceneMappingType::UnsignedInt},
    {"UnsignedLong output", Trade::SceneMappingType::UnsignedLong},
};

CombineTest::CombineTest() {
    addInstancedTests({&CombineTest::test},
        Containers::arraySize(TestData));

    addTests({&CombineTest::alignment,
              &CombineTest::objectsShared,
              &CombineTest::objectsPlaceholderFieldPlaceholder,
              &CombineTest::objectSharedFieldPlaceholder});
}

using namespace Math::Literals;

void CombineTest::test() {
    auto&& data = TestData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Testing the four possible object types, it should be possible to combine
       them */

    const UnsignedInt meshMappingData[]{45, 78, 23};
    const UnsignedByte meshFieldData[]{3, 5, 17};

    const UnsignedShort parentMappingData[]{0, 1};
    const Short parentData[]{-1, 0};

    const UnsignedByte translationMappingData[]{16};
    const Vector2d translationFieldData[]{{1.5, -0.5}};

    const UnsignedLong fooMappingData[]{15, 23};
    const Int fooFieldData[]{0, 1, 2, 3};

    Trade::SceneData scene = Implementation::combine(data.objectType, 167, Containers::arrayView({
        Trade::SceneFieldData{Trade::SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Parent, Containers::arrayView(parentMappingData), Containers::arrayView(parentData), Trade::SceneFieldFlag::ImplicitMapping},
        Trade::SceneFieldData{Trade::SceneField::Translation, Containers::arrayView(translationMappingData), Containers::arrayView(translationFieldData)},
        /* Array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15), Containers::arrayView(fooMappingData), Containers::StridedArrayView2D<const Int>{fooFieldData, {2, 2}}, Trade::SceneFieldFlag::OrderedMapping},
        /* Empty field */
        Trade::SceneFieldData{Trade::SceneField::Camera, Containers::ArrayView<const UnsignedByte>{}, Containers::ArrayView<const UnsignedShort>{}}
    }));

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), data.objectType);
    CORRADE_COMPARE(scene.mappingBound(), 167);
    CORRADE_COMPARE(scene.fieldCount(), 5);

    CORRADE_COMPARE(scene.fieldName(0), Trade::SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldFlags(0), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(0), Trade::SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(0), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(0), Containers::arrayView<UnsignedInt>({
        45, 78, 23
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(1), Trade::SceneField::Parent);
    CORRADE_COMPARE(scene.fieldFlags(1), Trade::SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(scene.fieldType(1), Trade::SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldArraySize(1), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(1), Containers::arrayView<UnsignedInt>({
        0, 1
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Short>(1),
        Containers::arrayView(parentData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(2), Trade::SceneField::Translation);
    CORRADE_COMPARE(scene.fieldFlags(2), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(2), Trade::SceneFieldType::Vector2d);
    CORRADE_COMPARE(scene.fieldArraySize(2), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(2),
        Containers::arrayView<UnsignedInt>({16}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Vector2d>(2),
        Containers::arrayView(translationFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(3), Trade::sceneFieldCustom(15));
    CORRADE_COMPARE(scene.fieldFlags(3), Trade::SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldType(3), Trade::SceneFieldType::Int);
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

    CORRADE_COMPARE(scene.fieldName(4), Trade::SceneField::Camera);
    CORRADE_COMPARE(scene.fieldFlags(4), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(4), Trade::SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldSize(4), 0);
    CORRADE_COMPARE(scene.fieldArraySize(4), 0);
}

void CombineTest::alignment() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};
    const UnsignedShort translationMappingData[]{5}; /* 1 byte padding before */
    const Vector2d translationFieldData[]{{1.5, 3.0}}; /* 4 byte padding before */

    Trade::SceneData scene = Implementation::combine(Trade::SceneMappingType::UnsignedShort, 167, Containers::arrayView({
        Trade::SceneFieldData{Trade::SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Translation, Containers::arrayView(translationMappingData), Containers::arrayView(translationFieldData)}
    }));

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.mappingBound(), 167);
    CORRADE_COMPARE(scene.fieldCount(), 2);

    CORRADE_COMPARE(scene.fieldName(0), Trade::SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldType(0), Trade::SceneFieldType::UnsignedByte);
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

    CORRADE_COMPARE(scene.fieldName(1), Trade::SceneField::Translation);
    CORRADE_COMPARE(scene.fieldType(1), Trade::SceneFieldType::Vector2d);
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

void CombineTest::objectsShared() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};
    const Int meshMaterialFieldData[]{72, -1, 23};

    const UnsignedShort translationRotationMappingData[]{14, 22};
    const Vector2 translationFieldData[]{{-1.0f, 25.3f}, {2.2f, 2.1f}};
    const Complex rotationFieldData[]{Complex::rotation(35.0_degf), Complex::rotation(22.5_degf)};

    Trade::SceneData scene = Implementation::combine(Trade::SceneMappingType::UnsignedInt, 173, Containers::arrayView({
        /* Deliberately in an arbitrary order to avoid false assumptions like
           fields sharing the same object mapping always being after each
           other */
        Trade::SceneFieldData{Trade::SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Translation, Containers::arrayView(translationRotationMappingData), Containers::arrayView(translationFieldData)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial, Containers::arrayView(meshMappingData), Containers::arrayView(meshMaterialFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Rotation, Containers::arrayView(translationRotationMappingData), Containers::arrayView(rotationFieldData)}
    }));

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 4);

    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::Mesh), 3);
    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::MeshMaterial), 3);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Mesh).data(), scene.mapping(Trade::SceneField::MeshMaterial).data());

    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::Translation), 2);
    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::Rotation), 2);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Translation).data(), scene.mapping(Trade::SceneField::Rotation).data());
}

void CombineTest::objectsPlaceholderFieldPlaceholder() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    Trade::SceneData scene = Implementation::combine(Trade::SceneMappingType::UnsignedShort, 173, Containers::arrayView({
        Trade::SceneFieldData{Trade::SceneField::Camera, Containers::ArrayView<UnsignedByte>{nullptr, 1}, Containers::ArrayView<UnsignedShort>{nullptr, 1}},
        Trade::SceneFieldData{Trade::SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        /* Looks like sharing object mapping with the Camera field, but
           actually both are placeholders */
        Trade::SceneFieldData{Trade::SceneField::Light, Containers::ArrayView<UnsignedShort>{nullptr, 2}, Containers::ArrayView<UnsignedInt>{nullptr, 2}},
        /* Array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15), Containers::ArrayView<UnsignedShort>{nullptr, 2}, Containers::StridedArrayView2D<Short>{{nullptr, 16}, {2, 4}}},
    }));

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 4);

    CORRADE_COMPARE(scene.fieldType(Trade::SceneField::Camera), Trade::SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::Camera), 1);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::SceneField::Camera), 0);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Camera).data(), scene.data());
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Camera).stride()[0], 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Camera).data(), scene.data() + 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Camera).stride()[0], 2);

    CORRADE_COMPARE(scene.fieldType(Trade::SceneField::Mesh), Trade::SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::SceneField::Mesh), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldType(Trade::SceneField::Light), Trade::SceneFieldType::UnsignedInt);
    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::Light), 2);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::SceneField::Light), 0);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Light).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Light).stride()[0], 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Light).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Light).stride()[0], 4);

    CORRADE_COMPARE(scene.fieldType(Trade::sceneFieldCustom(15)), Trade::SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldSize(Trade::sceneFieldCustom(15)), 2);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::sceneFieldCustom(15)), 4);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(15)).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(15)).stride()[0], 2);
    CORRADE_COMPARE(scene.field(Trade::sceneFieldCustom(15)).data(), scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4 + 2*2);
    CORRADE_COMPARE(scene.field(Trade::sceneFieldCustom(15)).stride()[0], 4*2);
}

void CombineTest::objectSharedFieldPlaceholder() {
    const UnsignedInt meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    Trade::SceneData scene = Implementation::combine(Trade::SceneMappingType::UnsignedInt, 173, Containers::arrayView({
        Trade::SceneFieldData{Trade::SceneField::Mesh, Containers::arrayView(meshMappingData), Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial, Containers::arrayView(meshMappingData), Containers::ArrayView<Int>{nullptr, 3}},
    }));

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 2);

    CORRADE_COMPARE(scene.fieldType(Trade::SceneField::Mesh), Trade::SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::SceneField::Mesh), 0);
    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(0),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        Containers::arrayView(meshFieldData),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldType(Trade::SceneField::MeshMaterial), Trade::SceneFieldType::Int);
    CORRADE_COMPARE(scene.fieldSize(Trade::SceneField::MeshMaterial), 3);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::SceneField::MeshMaterial), 0);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::MeshMaterial).data(), scene.mapping(Trade::SceneField::Mesh).data());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(Trade::SceneField::MeshMaterial),
        Containers::arrayView(meshMappingData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.field(Trade::SceneField::MeshMaterial).data(), scene.data() + 3*4 + 3 + 1);
    CORRADE_COMPARE(scene.field(Trade::SceneField::MeshMaterial).stride()[0], 4);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::CombineTest)
