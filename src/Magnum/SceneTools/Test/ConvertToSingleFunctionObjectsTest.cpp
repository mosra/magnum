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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/SceneTools/Combine.h"
#include "Magnum/SceneTools/Implementation/convertToSingleFunctionObjects.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct ConvertToSingleFunctionObjectsTest: TestSuite::Tester {
    explicit ConvertToSingleFunctionObjectsTest();

    void test();
    void fieldsToCopy();
};

struct {
    const char* name;
    UnsignedLong originalObjectCount;
    UnsignedLong expectedObjectCount;
    Trade::SceneFieldFlags parentFieldFlagsInput;
    Trade::SceneFieldFlags parentFieldFlagsExpected;
} TestData[]{
    {"original object count smaller than new", 64, 70, {}, {}},
    {"original object count larger than new", 96, 96, {}, {}},
    {"parent field with ordered mapping", 64, 70,
        Trade::SceneFieldFlag::OrderedMapping, Trade::SceneFieldFlag::OrderedMapping},
    {"parent field with implicit mapping", 64, 70,
        /* The mapping is *not* implicit but we're not using the flag for
           anything so this should work */
        Trade::SceneFieldFlag::ImplicitMapping, Trade::SceneFieldFlag::OrderedMapping}
};

ConvertToSingleFunctionObjectsTest::ConvertToSingleFunctionObjectsTest() {
    addInstancedTests({&ConvertToSingleFunctionObjectsTest::test},
        Containers::arraySize(TestData));

    addTests({&ConvertToSingleFunctionObjectsTest::fieldsToCopy});
}

using namespace Math::Literals;

void ConvertToSingleFunctionObjectsTest::test() {
    auto&& data = TestData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Haha now I can use combineFields() to conveniently prepare the initial
       state here, without having to mess with an ArrayTuple */

    const UnsignedShort parentMappingData[]{2, 15, 21, 22, 23};
    const Byte parentFieldData[]{-1, -1, -1, 21, 22};

    /* Two objects have two and three mesh assignments respectively, meaning we
       need three extra */
    const UnsignedShort meshMappingData[]{15, 23, 23, 23, 2, 15, 21};
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
       need a fourth extra object */
    const UnsignedShort cameraMappingData[]{22, 2};
    const UnsignedInt cameraFieldData[]{1, 5};

    /* Lights don't conflict with anything so they *could* retain the
       ImplicitMapping flag */
    const UnsignedShort lightMappingData[]{0, 1};
    const UnsignedByte lightFieldData[]{15, 23};

    /* Object 0 and 1 has a light, 2 a mesh already, meaning we need a fifth,
       sixth and seventh extra object and we lose the ImplicitMapping flag. */
    const UnsignedShort fooMappingData[]{0, 1, 2, 3};
    const Float fooFieldData[]{1.0f, 2.0f, 3.0f, 4.0f};

    /* This field is not among the fields to convert so it should preserve the
       ImplicitMapping flag */
    const UnsignedShort foo2MappingData[]{0, 1};
    const Byte foo2FieldData[]{-5, -7};

    /* This field shares mapping with foo (and thus has the ImplicitMapping
       flag), but it's not among the fields to convert. Since the mapping gets
       changed, it should not retain the ImplicitMapping flag. */
    const Byte foo3FieldData[]{-1, -2, 7, 2};

    Trade::SceneData original = combineFields(Trade::SceneMappingType::UnsignedShort, data.originalObjectCount, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(parentMappingData),
            Containers::arrayView(parentFieldData),
            data.parentFieldFlagsInput},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::stridedArrayView(meshMaterialFieldData).slice(&Containers::Pair<UnsignedInt, Int>::first)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(meshMappingData),
            Containers::stridedArrayView(meshMaterialFieldData).slice(&Containers::Pair<UnsignedInt, Int>::second)},
        Trade::SceneFieldData{Trade::SceneField::Camera,
            Containers::arrayView(cameraMappingData),
            Containers::arrayView(cameraFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(lightMappingData),
            Containers::arrayView(lightFieldData),
            Trade::SceneFieldFlag::ImplicitMapping},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(fooMappingData),
            Containers::arrayView(fooFieldData),
            Trade::SceneFieldFlag::ImplicitMapping},
        Trade::SceneFieldData{Trade::sceneFieldCustom(16),
            Containers::arrayView(foo2MappingData),
            Containers::arrayView(foo2FieldData),
            Trade::SceneFieldFlag::ImplicitMapping},
        Trade::SceneFieldData{Trade::sceneFieldCustom(17),
            Containers::arrayView(fooMappingData),
            Containers::arrayView(foo3FieldData),
            Trade::SceneFieldFlag::ImplicitMapping}
    });

    Trade::SceneData scene = Implementation::convertToSingleFunctionObjects(original, Containers::arrayView({
        Trade::SceneField::Mesh,
        /* Deliberately not including MeshMaterial in the list -- these should
           get automatically updated as they share the same object mapping.
           OTOH including them would break the output. */
        Trade::SceneField::Camera,
        /* A field with implicit mapping that doesn't conflict with anything so
           it *could* retain the flag */
        Trade::SceneField::Light,
        /* A field with implicit mapping, which loses the flag because entries
           get reassigned */
        Trade::sceneFieldCustom(15),
        /* Include also a field that's not present -- it should get skipped */
        Trade::SceneField::ImporterState
    }), {}, 63);

    /* There should be three more objects, or the original count preserved if
       it's large enough */
    CORRADE_COMPARE(scene.mappingBound(), data.expectedObjectCount);

    /* Object 0 should have new children with "foo", as it has a light */
    CORRADE_COMPARE_AS(scene.childrenFor(0),
        Containers::arrayView<UnsignedLong>({67}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.lightsFor(0),
        Containers::arrayView<UnsignedInt>({15}),
        TestSuite::Compare::Container);

    /* Object 1 should have a new child with "foo", as it has a light */
    CORRADE_COMPARE_AS(scene.childrenFor(1),
        Containers::arrayView<UnsignedLong>({68}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.lightsFor(1),
        Containers::arrayView<UnsignedInt>({23}),
        TestSuite::Compare::Container);

    /* Object 2 should have a new child with the camera and "foo", as it has a
       mesh */
    CORRADE_COMPARE_AS(scene.childrenFor(2),
        Containers::arrayView<UnsignedLong>({66, 69}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.meshesMaterialsFor(2),
        (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({{7, 2}})),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(2),
        Containers::arrayView<UnsignedInt>({}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.camerasFor(66),
        Containers::arrayView<UnsignedInt>({5}),
        TestSuite::Compare::Container);

    /* Object 15 should have a new child that has the second mesh */
    CORRADE_COMPARE_AS(scene.childrenFor(15),
        Containers::arrayView<UnsignedLong>({65}),
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
        Containers::arrayView<UnsignedLong>({63, 64}),
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
       added, the rest is the same. Because new objects are added at the end,
       the ordered flag is preserved if present. */
    CORRADE_COMPARE_AS(scene.parentsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        {2, -1},
        {15, -1},
        {21, -1},
        {22, 21},
        {23, 22},
        {63, 23},
        {64, 23},
        {65, 15},
        {66, 2},
        {67, 0},
        {68, 1},
        {69, 2},
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::Parent), data.parentFieldFlagsExpected);

    /* Meshes / materials have certain objects reassigned, field data stay the
       same. There was no flag before so neither is after. */
    CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>({
        {15, {6, 4}},
        {23, {1, 0}},
        {63, {2, 3}},
        {64, {4, 2}},
        {2, {7, 2}},
        {65, {3, 1}},
        {21, {5, -1}}
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::Mesh), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::MeshMaterial), Trade::SceneFieldFlags{});

    /* Cameras have certain objects reassigned, field data stay the same. There
       was no flag before so neither is after. */
    CORRADE_COMPARE_AS(scene.camerasAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {22, 1},
        {66, 5}
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::Camera), Trade::SceneFieldFlags{});

    /* Lights stay the same, thus the implicit flag could be preserved. It's
       not currently, though. */
    CORRADE_COMPARE_AS(scene.lightsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {0, 15},
        {1, 23}
    })), TestSuite::Compare::Container);
    {
        CORRADE_EXPECT_FAIL("Logic for preserving flags of untouched fields is rather complex and thus not implemented yet.");
        CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::Light), Trade::SceneFieldFlag::ImplicitMapping);
    } {
        CORRADE_COMPARE(scene.fieldFlags(Trade::SceneField::Light), Trade::SceneFieldFlags{});
    }

    /* A custom field gets the last object reassigned, field data stay the
       same. The implicit flag gets turned to nothing after that. */
    CORRADE_COMPARE_AS(scene.mappingAsArray(Trade::sceneFieldCustom(15)), (Containers::arrayView<UnsignedInt>({
        67, 68, 69, 3
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Float>(Trade::sceneFieldCustom(15)),
        Containers::arrayView(fooFieldData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.fieldFlags(Trade::sceneFieldCustom(15)), Trade::SceneFieldFlags{});

    /* A custom field that is not among fields to convert so it preserves the
       flag */
    CORRADE_COMPARE_AS(scene.mappingAsArray(Trade::sceneFieldCustom(16)), (Containers::arrayView<UnsignedInt>({
        0, 1
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Byte>(Trade::sceneFieldCustom(16)),
        Containers::arrayView(foo2FieldData),
        TestSuite::Compare::Container);
    {
        CORRADE_EXPECT_FAIL("Logic for preserving flags of untouched fields is rather complex and thus not implemented yet.");
        CORRADE_COMPARE(scene.fieldFlags(Trade::sceneFieldCustom(16)), Trade::SceneFieldFlag::ImplicitMapping);
    } {
        CORRADE_COMPARE(scene.fieldFlags(Trade::sceneFieldCustom(16)), Trade::SceneFieldFlags{});
    }

    /* A custom field that is not among fields to convert but it shares the
       mapping with a field that is and that gets changed. The implicit flag
       should thus get removed here as well. */
    CORRADE_COMPARE_AS(scene.mappingAsArray(Trade::sceneFieldCustom(17)), (Containers::arrayView<UnsignedInt>({
        67, 68, 69, 3
    })), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Byte>(Trade::sceneFieldCustom(17)),
        Containers::arrayView(foo3FieldData),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(scene.fieldFlags(Trade::sceneFieldCustom(17)), Trade::SceneFieldFlags{});
}

void ConvertToSingleFunctionObjectsTest::fieldsToCopy() {
    const UnsignedShort parentMappingData[]{2, 15, 21, 22};
    const Byte parentFieldData[]{-1, -1, -1, 21};

    const UnsignedShort meshMappingData[]{15, 21, 21, 21, 22, 15};
    const UnsignedInt meshFieldData[]{6, 1, 2, 4, 7, 3};

    const UnsignedShort skinMappingData[]{22, 21};
    const UnsignedInt skinFieldData[]{5, 13};

    const UnsignedLong fooMappingData[]{15, 23, 15, 21};
    const Int fooFieldData[]{0, 1, 2, 3, 4, 5, 6, 7};

    Trade::SceneData original = combineFields(Trade::SceneMappingType::UnsignedShort, 50, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(parentMappingData),
            Containers::arrayView(parentFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Skin,
            Containers::arrayView(skinMappingData),
            Containers::arrayView(skinFieldData)},
        /* Array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(fooMappingData),
            Containers::StridedArrayView2D<const Int>{fooFieldData, {4, 2}}},
        /* Just to disambiguate between 2D and 3D */
        Trade::SceneFieldData{Trade::SceneField::Transformation,
            Trade::SceneMappingType::UnsignedShort, nullptr,
            Trade::SceneFieldType::Matrix4x4, nullptr}
    });

    Trade::SceneData scene = Implementation::convertToSingleFunctionObjects(original,
        Containers::arrayView({
            /* Include also a field that's not present -- it should get skipped */
            Trade::SceneField::ImporterState,
            /* Three additional mesh assignments that go to new objects */
            Trade::SceneField::Mesh
        }),
        Containers::arrayView({
            /* One assignment is to an object that has just one mesh, it should
               not be copied anywhere, the other should be duplicated two
               times */
            Trade::SceneField::Skin,
            /* Array field with multiple assignments per object -- all should
               be copied */
            Trade::sceneFieldCustom(15),
            /* Include also a field that's not present -- it should get skipped */
            Trade::SceneField::Camera
        }), 60);

    CORRADE_COMPARE_AS(scene.parentsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Int>>({
        {2, -1},
        {15, -1},
        {21, -1},
        {22, 21},
        {60, 21}, /* duplicated mesh assignment to object 21 */
        {61, 21}, /* duplicated mesh assignment to object 21 */
        {62, 15}  /* duplicated mesh assignment to object 15 */
    })), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.meshesMaterialsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>>({
        {15, {6, -1}},
        {21, {1, -1}},
        {60, {2, -1}}, /* duplicated mesh assignment to object 21 */
        {61, {4, -1}}, /* duplicated mesh assignment to object 21 */
        {22, {7, -1}},
        {62, {3, -1}}  /* duplicated mesh assignment to object 15 */
    })), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.skinsAsArray(), (Containers::arrayView<Containers::Pair<UnsignedInt, UnsignedInt>>({
        {22, 5},
        {21, 13},
        {60, 13}, /* duplicated from object 21 */
        {61, 13}, /* duplicated from object 21 */
    })), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(Trade::sceneFieldCustom(15)), Containers::arrayView<UnsignedInt>({
        15, 23, 15, 21,
        60, 61, /* duplicated from object 21 (two duplicates of one object) */
        62, 62, /* duplicated from object 15 (two entries for one object) */
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<Int[]>(Trade::sceneFieldCustom(15)).transposed<0, 1>()[0]), Containers::arrayView<Int>({
        0, 2, 4, 6,
        6, 6, /* duplicated from object 21 (two duplicates of one object) */
        0, 4, /* duplicated from object 15 (two entries for one object) */
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<Int[]>(Trade::sceneFieldCustom(15)).transposed<0, 1>()[1]), Containers::arrayView<Int>({
        1, 3, 5, 7,
        7, 7, /* duplicated from object 21 (two duplicates of one object) */
        1, 5, /* duplicated from object 15 (two entries for one object) */
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::ConvertToSingleFunctionObjectsTest)
