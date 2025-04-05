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

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/SceneTools/Filter.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct FilterTest: TestSuite::Tester {
    explicit FilterTest();

    void fields();
    void fieldsRvalue();
    void fieldsWrongBitCount();

    void onlyFields();
    void onlyFieldsNoFieldData();
    void onlyFieldsRvalue();

    void exceptFields();
    void exceptFieldsNoFieldData();
    void exceptFieldsRvalue();

    void fieldEntries();
    void fieldEntriesFieldNotFound();
    void fieldEntriesDuplicated();
    void fieldEntriesWrongBitCount();
    void fieldEntriesStringField();
    void fieldEntriesBitField();

    void fieldEntriesSharedMapping();
    void fieldEntriesSharedMappingInvalid();

    template<class T> void objects();
    void objectsUnchangedFields();
    void objectsSharedMapping();
    void objectsSharedMappingAllRemoved();
    void objectsWrongBitCount();
};

using namespace Math::Literals;

const struct {
    const char* name;
    Trade::DataFlags dataFlags, expectedDataFlags;
} FieldsRvalueData[]{
    /* The Global or ExternallyOwned flags are not preserved, because
       reference() doesn't preserve them either */
    {"not owned",
        Trade::DataFlag::Global|Trade::DataFlag::ExternallyOwned,
        {}},
    {"owned",
        Trade::DataFlag::Owned,
        Trade::DataFlag::Owned|Trade::DataFlag::Mutable}
};

const struct {
    const char* name;
    bool byName;
} FieldEntriesData[]{
    {"by ID", false},
    {"by name", true}
};

FilterTest::FilterTest() {
    addTests({&FilterTest::fields});

    addInstancedTests({&FilterTest::fieldsRvalue},
        Containers::arraySize(FieldsRvalueData));

    addTests({&FilterTest::fieldsWrongBitCount,

              &FilterTest::onlyFields,
              &FilterTest::onlyFieldsNoFieldData,
              &FilterTest::onlyFieldsRvalue,

              &FilterTest::exceptFields,
              &FilterTest::exceptFieldsNoFieldData,
              &FilterTest::exceptFieldsRvalue});

    addInstancedTests({&FilterTest::fieldEntries},
        Containers::arraySize(FieldEntriesData));

    addTests({&FilterTest::fieldEntriesFieldNotFound,
              &FilterTest::fieldEntriesDuplicated,
              &FilterTest::fieldEntriesWrongBitCount,
              &FilterTest::fieldEntriesStringField,
              &FilterTest::fieldEntriesBitField,

              &FilterTest::fieldEntriesSharedMapping,
              &FilterTest::fieldEntriesSharedMappingInvalid,

              &FilterTest::objects<UnsignedByte>,
              &FilterTest::objects<UnsignedShort>,
              &FilterTest::objects<UnsignedInt>,
              &FilterTest::objects<UnsignedLong>,
              &FilterTest::objectsUnchangedFields,
              &FilterTest::objectsSharedMapping,
              &FilterTest::objectsSharedMappingAllRemoved,
              &FilterTest::objectsWrongBitCount});
}

void FilterTest::fields() {
    const struct Data {
        UnsignedShort meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedShort lightMapping[3];
        UnsignedInt light[3];
        UnsignedShort visibilityMapping[2];
        bool visible[2];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, Trade::DataFlag::Mutable, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        /* Offset-only */
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial, 5,
            Trade::SceneMappingType::UnsignedShort, offsetof(Data, meshMaterialMapping), sizeof(UnsignedShort),
            Trade::SceneFieldType::Byte, offsetof(Data, meshMaterial), sizeof(UnsignedByte)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
        /* Bit */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->visibilityMapping),
            Containers::stridedArrayView(data->visible).sliceBit(0)},
    }};

    Containers::BitArray attributesToKeep{ValueInit, scene.fieldCount()};
    attributesToKeep.set(0);
    attributesToKeep.set(1);
    attributesToKeep.set(3);

    Trade::SceneData filtered = filterFields(scene, attributesToKeep);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 76);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(data));
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});

    /* Testing just the pointers if they match expectations, the SceneFieldData
       get copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.fieldCount(), 3);
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::Mesh);
    CORRADE_COMPARE(filtered.field(0).data(), data->mesh);
    CORRADE_COMPARE(filtered.fieldName(1), Trade::SceneField::MeshMaterial);
    CORRADE_COMPARE(filtered.mapping(1).data(), data->meshMaterialMapping);
    CORRADE_COMPARE(filtered.fieldName(2), Trade::sceneFieldCustom(15));
    CORRADE_COMPARE(filtered.fieldBits(2).data(), data->visible);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::SceneFieldData> fieldData = filtered.releaseFieldData();
    CORRADE_VERIFY(!fieldData.deleter());
}

void FilterTest::fieldsRvalue() {
    auto&& data = FieldsRvalueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Subset of attributes() verifying data ownership transfer behavior */

    struct Data {
        UnsignedShort meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedShort lightVisibilityMapping[3];
        UnsignedInt light[3];
        bool visible[3];
    };
    Containers::Array<char> sceneData{sizeof(Data)};
    Data& d = *reinterpret_cast<Data*>(sceneData.data());
    Containers::Array<Trade::SceneFieldData> fields{InPlaceInit, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(d.lightVisibilityMapping),
            Containers::arrayView(d.light)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(d.lightVisibilityMapping),
            Containers::stridedArrayView(d.visible).sliceBit(0)},
    }};

    Containers::Optional<Trade::SceneData> scene;
    if(data.dataFlags >= Trade::DataFlag::Owned)
        scene = Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 76, Utility::move(sceneData), Utility::move(fields)};
    else
        scene = Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 76, data.dataFlags, sceneData, Utility::move(fields)};

    Containers::BitArray attributesToKeep{ValueInit, scene->fieldCount()};
    attributesToKeep.set(0);
    attributesToKeep.set(1);
    attributesToKeep.set(3);

    /* The data ownership should be transferred if possible */
    Trade::SceneData filtered = filterFields(Utility::move(*scene), attributesToKeep);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 76);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(&d));
    CORRADE_COMPARE(filtered.dataFlags(), data.expectedDataFlags);

    /* Just checking that the fields get actually filtered instead of being
       passed through verbatim, the actual verification is done in fields()
       above */
    CORRADE_COMPARE(filtered.fieldCount(), 3);
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::Mesh);
}

void FilterTest::fieldsWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt data[3];

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 15, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Camera,
            Containers::arrayView(data),
            Containers::arrayView(data)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data),
            Containers::arrayView(data)},
    }};

    Containers::String out;
    Error redirectError{&out};
    filterFields(scene, Containers::BitArray{ValueInit, 3});
    CORRADE_COMPARE(out, "SceneTools::filterFields(): expected 2 bits but got 3\n");
}

void FilterTest::onlyFields() {
    const struct {
        UnsignedByte meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedByte lightMapping[3];
        UnsignedInt light[3];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 133, Trade::DataFlag::Mutable, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
    }};

    Trade::SceneData filtered = filterOnlyFields(scene, {
        Trade::SceneField::Light,
        Trade::SceneField::Camera, /* not present in the scene, ignored */
        Trade::SceneField::MeshMaterial,
        Trade::SceneField::Light, /* listed twice, ignored */
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(filtered.mappingBound(), 133);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(data));
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});

    /* Testing just the pointers if they match expectations, the SceneFieldData
       get copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.fieldCount(), 2);
    /* The original order stays even though MeshMaterial was specified after
       Light in the list */
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::MeshMaterial);
    CORRADE_COMPARE(filtered.field(0).data(), data->meshMaterial);
    CORRADE_COMPARE(filtered.fieldName(1), Trade::SceneField::Light);
    CORRADE_COMPARE(filtered.mapping(1).data(), data->lightMapping);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::SceneFieldData> fieldData = filtered.releaseFieldData();
    CORRADE_VERIFY(!fieldData.deleter());
}

void FilterTest::onlyFieldsNoFieldData() {
    /* Just to verify it doesn't crash */

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 331, nullptr, {}};
    Trade::SceneData filtered = filterOnlyFields(scene, {
        Trade::SceneField::MeshMaterial
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 331);
    CORRADE_COMPARE(filtered.data().data(), nullptr);
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});
}

void FilterTest::onlyFieldsRvalue() {
    /* Subset of onlyFields() verifying data ownership transfer behavior. All
       cases of ownership transfer are verified in fieldsRvalue(), this only
       checks that the r-value gets correctly passed through all overloads to
       keep the data owned. */

    struct Data {
        UnsignedByte meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedByte lightMapping[3];
        UnsignedInt light[3];
    };
    Containers::Array<char> data{sizeof(Data)};
    Data& d = *reinterpret_cast<Data*>(data.data());

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 133, Utility::move(data), {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(d.lightMapping),
            Containers::arrayView(d.light)},
    }};

    Trade::SceneData filtered = filterOnlyFields(Utility::move(scene), {
        Trade::SceneField::Light,
        Trade::SceneField::MeshMaterial
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE(filtered.mappingBound(), 133);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(&d));
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(filtered.fieldCount(), 2);
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::MeshMaterial);
}

void FilterTest::exceptFields() {
    const struct {
        UnsignedLong meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedLong lightMapping[3];
        UnsignedInt light[3];
        UnsignedLong visibilityMapping[2];
        bool visible[2];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedLong, 1, Trade::DataFlag::Mutable, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->visibilityMapping),
            Containers::stridedArrayView(data->visible).sliceBit(0)},
    }};

    Trade::SceneData filtered = filterExceptFields(scene, {
        Trade::SceneField::Light,
        Trade::SceneField::Camera, /* not present in the scene, ignored */
        Trade::SceneField::MeshMaterial,
        Trade::SceneField::Light, /* listed twice, ignored */
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(filtered.mappingBound(), 1);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(data));
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});

    /* Testing just the pointers if they match expectations, the SceneFieldData
       get copied directly so no metadata should get lost */
    CORRADE_COMPARE(filtered.fieldCount(), 2);
    /* The original order stays even though MeshMaterial was specified after
       Light in the list */
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::Mesh);
    CORRADE_COMPARE(filtered.field(0).data(), data->mesh);
    CORRADE_COMPARE(filtered.mapping(0).data(), data->meshMaterialMapping);
    CORRADE_COMPARE(filtered.fieldName(1), Trade::sceneFieldCustom(15));
    CORRADE_COMPARE(filtered.fieldBits(1).data(), data->visible);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::SceneFieldData> fieldData = filtered.releaseFieldData();
    CORRADE_VERIFY(!fieldData.deleter());
}

void FilterTest::exceptFieldsNoFieldData() {
    /* Just to verify it doesn't crash */

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 331, nullptr, {}};
    Trade::SceneData filtered = filterOnlyFields(scene, {
        Trade::SceneField::MeshMaterial,
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 331);
    CORRADE_COMPARE(filtered.data().data(), nullptr);
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});
}

void FilterTest::exceptFieldsRvalue() {
    /* Subset of exceptFields() verifying data ownership transfer behavior. All
       cases of ownership transfer are verified in fieldsRvalue(), this only
       checks that the r-value gets correctly passed through all overloads to
       keep the data owned. */

    struct Data {
        UnsignedLong meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedLong lightVisibilityMapping[3];
        UnsignedInt light[3];
        bool visible[3];
    };
    Containers::Array<char> data{sizeof(Data)};
    Data& d = *reinterpret_cast<Data*>(data.data());

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedLong, 12, Utility::move(data), {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(d.meshMaterialMapping),
            Containers::arrayView(d.meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(d.lightVisibilityMapping),
            Containers::arrayView(d.light)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(d.lightVisibilityMapping),
            Containers::stridedArrayView(d.visible).sliceBit(0)},
    }};

    Trade::SceneData filtered = filterExceptFields(Utility::move(scene), {
        Trade::SceneField::Light,
        Trade::SceneField::MeshMaterial
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedLong);
    CORRADE_COMPARE(filtered.mappingBound(), 12);
    CORRADE_COMPARE(filtered.data().data(), static_cast<const void*>(&d));
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(filtered.fieldCount(), 2);
    CORRADE_COMPARE(filtered.fieldName(0), Trade::SceneField::Mesh);
}

void FilterTest::fieldEntries() {
    auto&& data = FieldEntriesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const struct Data {
        UnsignedShort meshMapping[5]{7, 8, 900, 1000, 11000};
        UnsignedByte mesh[5]{2, 3, 222, 1, 222};
        UnsignedShort lightMapping[4]{0, 1, 2, 3};
        UnsignedInt light[4]{12, 23, 32, 31};
        UnsignedShort arrayMapping[3]{0, 1, 2};
        Float array[3][2]{{77.0f, 88.0f}, {1.0f, 2.0f}, {3.0f, 4.0f}};
        UnsignedShort visibilityMapping[2]{12, 33};
        bool visible[2]{true, false};
        UnsignedShort parentMapping[3]{};
        Short parents[3]{};
    } sceneData[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, sceneData, {
        /* The ordered flag should get preserved as removing items preserves
           order*/
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->meshMapping),
            Containers::arrayView(sceneData->mesh),
            Trade::SceneFieldFlag::OrderedMapping},
        /* Offset-only, to verify it get converted to absolute when it reaches
           combine() at the end. The implicit flag gets preserved because the
           field isn't touched in any way. */
        Trade::SceneFieldData{Trade::SceneField::Light, 4,
            Trade::SceneMappingType::UnsignedShort, offsetof(Data, lightMapping), sizeof(UnsignedShort),
            Trade::SceneFieldType::UnsignedInt, offsetof(Data, light), sizeof(UnsignedInt),
            Trade::SceneFieldFlag::ImplicitMapping},
        /* Array. Here the flag gets downgraded to just an OrderedMapping
           because items are removed. */
        Trade::SceneFieldData{Trade::sceneFieldCustom(333),
            Containers::arrayView(sceneData->arrayMapping),
            Containers::StridedArrayView2D<const Float>{Containers::stridedArrayView(sceneData->array)},
            Trade::SceneFieldFlag::ImplicitMapping},
        /* Bit field. Should cause no assert as it's just passed through. */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(sceneData->visibilityMapping),
            Containers::stridedArrayView(sceneData->visible).sliceBit(0)},
        /* This one gets all entries removed */
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(sceneData->parentMapping),
            Containers::arrayView(sceneData->parents)},
        /* This one is already empty */
        Trade::SceneFieldData{Trade::SceneField::Camera,
            Containers::ArrayView<UnsignedShort>{},
            Containers::ArrayView<UnsignedByte>{}},
    }};

    Containers::BitArray meshesToKeep{DirectInit, Containers::arraySize(sceneData->mesh), true};
    meshesToKeep.reset(2);
    meshesToKeep.reset(4);

    Containers::BitArray arraysToKeep{DirectInit, Containers::arraySize(sceneData->array), true};
    arraysToKeep.reset(0);

    Containers::BitArray parentsToKeep{DirectInit, Containers::arraySize(sceneData->parents), false};

    Containers::BitArray camerasToKeep;

    Trade::SceneData filtered = data.byName ?
        filterFieldEntries(scene, {
            {Trade::sceneFieldCustom(333), arraysToKeep},
            {Trade::SceneField::Parent, parentsToKeep},
            {Trade::SceneField::Mesh, meshesToKeep},
            {Trade::SceneField::Camera, camerasToKeep}
        }) :
        filterFieldEntries(scene, {
            {2, arraysToKeep},
            {4, parentsToKeep},
            {0, meshesToKeep},
            {5, camerasToKeep}
        });

    CORRADE_COMPARE(filtered.fieldCount(), 6);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 76);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    /* The flag gets preserved here */
    CORRADE_COMPARE(filtered.fieldFlags(Trade::SceneField::Mesh), Trade::SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedShort>({7, 8, 1000}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({2, 3, 1}),
        TestSuite::Compare::Container);

    /* Lights weren't listed and thus stayed untouched */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Light));
    /* The flag gets preserved here as well as the field wasn't touched */
    CORRADE_COMPARE(filtered.fieldFlags(Trade::SceneField::Light), Trade::SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Light),
        Containers::arrayView(sceneData->lightMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView(sceneData->light),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::sceneFieldCustom(333)));
    /* The field isn't implicitly mapped anymore */
    CORRADE_COMPARE(filtered.fieldFlags(Trade::sceneFieldCustom(333)), Trade::SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::sceneFieldCustom(333)),
        Containers::arrayView<UnsignedShort>({1, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2>(filtered.field<Float[]>(Trade::sceneFieldCustom(333)))),
        Containers::arrayView<Vector2>({{1.0f, 2.0f}, {3.0f, 4.0f}}),
        TestSuite::Compare::Container);

    /* Bits weren't listed and thus stayed untouched */
    CORRADE_VERIFY(filtered.hasField(Trade::sceneFieldCustom(15)));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::sceneFieldCustom(15)),
        Containers::arrayView(sceneData->visibilityMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.fieldBits(Trade::sceneFieldCustom(15)),
        Containers::stridedArrayView(sceneData->visible).sliceBit(0),
        TestSuite::Compare::Container);

    /* Parents are all removed */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Parent));
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::Parent), 0);

    /* Cameras were empty before already */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Camera));
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::Camera), 0);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::SceneFieldData> fieldData = filtered.releaseFieldData();
    CORRADE_VERIFY(!fieldData.deleter());
}

void FilterTest::fieldEntriesFieldNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMapping[5];
        UnsignedByte mesh[5];
        UnsignedShort lightMapping[4];
        UnsignedInt light[4];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
    }};

    Containers::String out;
    Error redirectError{&out};
    filterFieldEntries(scene, {
        {Trade::SceneField::Light, Containers::BitArray{ValueInit, 4}},
        {Trade::SceneField::Parent, {}}
    });
    filterFieldEntries(scene, {
        {1, Containers::BitArray{ValueInit, 4}},
        {2, {}}
    });
    CORRADE_COMPARE(out,
        "SceneTools::filterFieldEntries(): field Trade::SceneField::Parent not found\n"
        "SceneTools::filterFieldEntries(): index 2 out of range for 2 fields\n");
}

void FilterTest::fieldEntriesDuplicated() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMapping[5];
        UnsignedByte mesh[5];
        UnsignedShort lightMapping[4];
        UnsignedInt light[4];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
    }};

    Containers::String out;
    Error redirectError{&out};
    /* The name-based variant just delegates to this one, no need to test it
       as well */
    filterFieldEntries(scene, {
        {1, Containers::BitArray{ValueInit, 4}},
        {0, Containers::BitArray{ValueInit, 5}},
        {1, Containers::BitArray{ValueInit, 4}},
    });
    CORRADE_COMPARE(out, "SceneTools::filterFieldEntries(): field Trade::SceneField::Light listed more than once\n");
}

void FilterTest::fieldEntriesWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMapping[5];
        UnsignedByte mesh[5];
        UnsignedShort lightMapping[4];
        UnsignedInt light[4];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
    }};

    Containers::String out;
    Error redirectError{&out};
    /* The name-based variant just delegates to this one, no need to test it
       as well */
    filterFieldEntries(scene, {
        {1, Containers::BitArray{ValueInit, 4}},
        {0, Containers::BitArray{ValueInit, 6}}
    });
    CORRADE_COMPARE(out, "SceneTools::filterFieldEntries(): expected 5 bits for Trade::SceneField::Mesh but got 6\n");
}

void FilterTest::fieldEntriesBitField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMapping[5];
        UnsignedByte mesh[5];
        UnsignedShort visibilityMapping[2];
        bool visible[2];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->visibilityMapping),
            Containers::stridedArrayView(data->visible).sliceBit(0)},
    }};

    Containers::String out;
    Error redirectError{&out};
    /* The name-based variant just delegates to this one, no need to test it
       as well */
    filterFieldEntries(scene, {
        {0, Containers::BitArray{ValueInit, 5}},
        {1, Containers::BitArray{ValueInit, 2}}
    });
    CORRADE_COMPARE(out, "SceneTools::filterFieldEntries(): filtering bit fields is not implemented yet, sorry\n");
}

void FilterTest::fieldEntriesStringField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMapping[5];
        UnsignedByte mesh[5];
        UnsignedShort nameMapping[2];
        UnsignedInt nameRangeNullTerminated[2];
        char nameString[1];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->nameMapping),
            data->nameString, Trade::SceneFieldType::StringRangeNullTerminated32,
            Containers::arrayView(data->nameRangeNullTerminated)},
    }};

    Containers::String out;
    Error redirectError{&out};
    /* The name-based variant just delegates to this one, no need to test it
       as well */
    filterFieldEntries(scene, {
        {0, Containers::BitArray{ValueInit, 5}},
        {1, Containers::BitArray{ValueInit, 2}}
    });
    CORRADE_COMPARE(out, "SceneTools::filterFieldEntries(): filtering string fields is not implemented yet, sorry\n");
}

void FilterTest::fieldEntriesSharedMapping() {
    const struct {
        UnsignedShort meshMaterialMapping[5]{7, 8, 6666, 6666, 3};
        UnsignedByte mesh[5]{2, 3, 222, 222, 1};
        Byte meshMaterial[5]{-1, 7, 111, 111, 0};
        UnsignedShort trsMapping[5]{1, 6666, 7, 6666, 3};
        Vector2 translation[5]{
            {1.0f, 2.0f},
            {},
            {3.0f, 4.0f},
            {},
            {5.0f, 6.0f}
        };
        Complex rotation[5]{
            Complex::rotation(15.0_degf),
            {},
            Complex::rotation(30.0_degf),
            {},
            Complex::rotation(45.0_degf)
        };
        Float uniformScale[5]{10.0f, 0.0f, -5.0f, 0.0f, 555.0f};
        UnsignedInt light[2]{34, 25};
        Int parent[3]{-1, 0, 3};
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 176, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Translation,
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->translation)},
        Trade::SceneFieldData{Trade::SceneField::Rotation,
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->rotation)},
        /* Shares trsMapping, sharing should be preserved even though not
           enforced */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->uniformScale)},
        /* Shares a prefix of meshMaterialMapping, should not be preserved */
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->meshMaterialMapping).prefix(2),
            Containers::arrayView(data->light)},
        /* Shares every 2nd item of trsMapping, should not be preserved */
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(data->trsMapping).every(2),
            Containers::arrayView(data->parent)},
    }};

    Containers::BitArray meshesToKeep{DirectInit, Containers::arraySize(data->mesh), true};
    meshesToKeep.reset(2);
    meshesToKeep.reset(3);

    Containers::BitArray transformationsToKeep{DirectInit, Containers::arraySize(data->trsMapping), true};
    transformationsToKeep.reset(1);
    transformationsToKeep.reset(3);

    Trade::SceneData filtered = filterFieldEntries(scene, {
        /* All shared fields have to be listed with the same view */
        {Trade::SceneField::Mesh, meshesToKeep},
        {Trade::SceneField::MeshMaterial, meshesToKeep},

        {Trade::SceneField::Translation, transformationsToKeep},
        {Trade::SceneField::Rotation, transformationsToKeep},
        {Trade::sceneFieldCustom(15), transformationsToKeep},
    });

    CORRADE_COMPARE(filtered.fieldCount(), 7);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 176);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedShort>({7, 8, 3}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({2, 3, 1}),
        TestSuite::Compare::Container);

    /* Mapping shared with Mesh */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::MeshMaterial));
    CORRADE_COMPARE(filtered.mapping(Trade::SceneField::MeshMaterial).data(),
        filtered.mapping(Trade::SceneField::Mesh).data());
    CORRADE_COMPARE_AS(filtered.field<Byte>(Trade::SceneField::MeshMaterial),
        Containers::arrayView<Byte>({-1, 7, 0}),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Translation));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Translation),
        Containers::arrayView<UnsignedShort>({1, 7, 3}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<Vector2>(Trade::SceneField::Translation),
        Containers::arrayView<Vector2>({{1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}}),
        TestSuite::Compare::Container);

    /* Mapping shared with Translation */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Rotation));
    CORRADE_COMPARE(filtered.mapping(Trade::SceneField::Rotation).data(),
        filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Complex>(Trade::SceneField::Rotation),
        Containers::arrayView<Complex>({Complex::rotation(15.0_degf), Complex::rotation(30.0_degf), Complex::rotation(45.0_degf)}),
        TestSuite::Compare::Container);

    /* Mapping shared with Translation again */
    CORRADE_VERIFY(filtered.hasField(Trade::sceneFieldCustom(15)));
    CORRADE_COMPARE(filtered.mapping(Trade::sceneFieldCustom(15)).data(),
        filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Float>(Trade::sceneFieldCustom(15)),
        Containers::arrayView({10.0f, -5.0f, 555.0f}),
        TestSuite::Compare::Container);

    /* These fields are kept unfiltered and they don't share any mapping even
       though they could */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Light));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedShort>({7, 8}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(filtered.mapping(Trade::SceneField::Light).data() != filtered.mapping(Trade::SceneField::Mesh).data());
    CORRADE_COMPARE_AS(filtered.field<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView(data->light),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Parent));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView<UnsignedShort>({1, 7, 3}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(filtered.mapping(Trade::SceneField::Parent).data() != filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(data->parent),
        TestSuite::Compare::Container);
}

void FilterTest::fieldEntriesSharedMappingInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort lightMapping[4];
        UnsignedInt light[4];
        UnsignedShort meshMaterialMapping[5];
        UnsignedByte mesh[5];
        Byte meshMaterial[5];
        UnsignedLong meshIndexOffset[5];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 176, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(1),
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshIndexOffset)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
    }};

    Containers::BitArray meshesToKeep{ValueInit, 5};
    Containers::BitArray meshesToKeepDifferent{DirectInit, 5, true};

    Containers::String out;
    Error redirectError{&out};
    filterFieldEntries(scene, {
        {Trade::SceneField::MeshMaterial, meshesToKeep},
        {Trade::SceneField::Mesh, meshesToKeep},
        {Trade::sceneFieldCustom(1), meshesToKeepDifferent},
    });
    filterFieldEntries(scene, {
        {Trade::SceneField::Mesh, meshesToKeep},
        {Trade::SceneField::MeshMaterial, meshesToKeep},
    });
    filterFieldEntries(scene, {
        {Trade::sceneFieldCustom(1), meshesToKeep},
        {Trade::SceneField::MeshMaterial, meshesToKeep},
    });
    CORRADE_COMPARE(out,
        "SceneTools::filterFieldEntries(): field Trade::SceneField::Custom(1) shares mapping with Trade::SceneField::MeshMaterial but was passed a different mask view\n"
        "SceneTools::filterFieldEntries(): field Trade::SceneField::Mesh shares mapping with 3 fields but only 2 are filtered\n"
        "SceneTools::filterFieldEntries(): field Trade::SceneField::Custom(1) shares mapping with 3 fields but only 2 are filtered\n");
}

template<class T> void FilterTest::objects() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const struct {
        T meshMapping[5]{7, 8, 15, 3, 2};
        UnsignedByte mesh[5]{2, 222, 3, 222, 222};
        T lightMapping[4]{2, 1, 3, 2};
        UnsignedInt light[4]{66666, 23, 66666, 66666};
        T parentMapping[3]{2, 3, 8};
        Short parents[3]{6666, 6666, 6666};
    } data[1];

    Trade::SceneData scene{Trade::Implementation::sceneMappingTypeFor<T>(), 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        /* This one has duplicate entries for an object, both will be removed */
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
        /* This one gets all entries removed. The flags should get preserved
           even in that case tho. */
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(data->parentMapping),
            Containers::arrayView(data->parents),
            Trade::SceneFieldFlag::OrderedMapping},
        /* This one is already empty */
        Trade::SceneFieldData{Trade::SceneField::Camera,
            Containers::ArrayView<T>{},
            Containers::ArrayView<UnsignedByte>{}},
    }};

    Containers::BitArray objectsToKeep{DirectInit, std::size_t(scene.mappingBound()), true};
    objectsToKeep.reset(8);
    objectsToKeep.reset(3);
    objectsToKeep.reset(2);

    Trade::SceneData filtered = filterObjects(scene, objectsToKeep);

    CORRADE_COMPARE(filtered.fieldCount(), 4);
    CORRADE_COMPARE(filtered.mappingType(), Trade::Implementation::sceneMappingTypeFor<T>());
    CORRADE_COMPARE(filtered.mappingBound(), 76);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    CORRADE_COMPARE_AS(filtered.mapping<T>(Trade::SceneField::Mesh),
        Containers::arrayView<T>({7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({2, 3}),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Light));
    CORRADE_COMPARE_AS(filtered.mapping<T>(Trade::SceneField::Light),
        Containers::arrayView<T>({1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedInt>({23}),
        TestSuite::Compare::Container);

    /* Parents are all removed, flags stay */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Parent));
    CORRADE_COMPARE(filtered.fieldFlags(Trade::SceneField::Parent), Trade::SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::Parent), 0);

    /* Cameras were empty before already */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Camera));
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::Camera), 0);

    /* The attribute data should not be a growable array to make this usable in
       plugins */
    Containers::Array<Trade::SceneFieldData> fieldData = filtered.releaseFieldData();
    CORRADE_VERIFY(!fieldData.deleter());
}

void FilterTest::objectsUnchangedFields() {
    /* Compared to above, this contains fields that don't have any objects
       that should be filtered out, which are thus passed through unchanged
       (and thus can be even of type that is unuspported by
       filterFieldEntries()) */

    const struct {
        UnsignedShort meshMapping[5]{7, 8, 15, 3, 2};
        UnsignedByte mesh[5]{2, 222, 3, 222, 222};
        UnsignedShort visibilityMapping[2]{22, 1};
        bool visible[2]{false, true};
    } data[1];

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->visibilityMapping),
            Containers::stridedArrayView(data->visible).sliceBit(0)},
    }};

    Containers::BitArray objectsToKeep{DirectInit, std::size_t(scene.mappingBound()), true};
    objectsToKeep.reset(8);
    objectsToKeep.reset(3);
    objectsToKeep.reset(2);

    Trade::SceneData filtered = filterObjects(scene, objectsToKeep);
    CORRADE_COMPARE(filtered.fieldCount(), 2);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 76);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedShort>({7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({2, 3}),
        TestSuite::Compare::Container);

    /* Bits weren't affected and thus were passed through unchanged */
    CORRADE_VERIFY(filtered.hasField(Trade::sceneFieldCustom(15)));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::sceneFieldCustom(15)),
        Containers::arrayView(data->visibilityMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.fieldBits(Trade::sceneFieldCustom(15)),
        Containers::stridedArrayView(data->visible).sliceBit(0),
        TestSuite::Compare::Container);
}

void FilterTest::objectsSharedMapping() {
    const struct {
        UnsignedShort meshMaterialMapping[5]{7, 8, 15, 3, 2};
        UnsignedByte mesh[5]{2, 222, 3, 222, 222};
        Byte meshMaterial[5]{-1, 111, 7, 111, 111};
        UnsignedShort trsMapping[5]{1, 8, 7, 2, 15};
        Vector2 translation[5]{
            {1.0f, 2.0f},
            {},
            {3.0f, 4.0f},
            {},
            {5.0f, 6.0f}
        };
        Complex rotation[5]{
            Complex::rotation(15.0_degf),
            {},
            Complex::rotation(30.0_degf),
            {},
            Complex::rotation(45.0_degf)
        };
        Float uniformScale[5]{10.0f, 0.0f, -5.0f, 0.0f, 555.0f};
        UnsignedInt light[2]{34, 25};
        Int parent[3]{-1, 0, 3};
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 176, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Translation,
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->translation)},
        Trade::SceneFieldData{Trade::SceneField::Rotation,
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->rotation)},
        /* Shares trsMapping, sharing should be preserved even though not
           enforced */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::arrayView(data->trsMapping),
            Containers::arrayView(data->uniformScale)},
        /* Shares a prefix of meshMaterialMapping, should not be preserved */
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->meshMaterialMapping).prefix(2),
            Containers::arrayView(data->light)},
        /* Shares every 2nd item of trsMapping, should not be preserved */
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(data->trsMapping).every(2),
            Containers::arrayView(data->parent)},
    }};

    Containers::BitArray objectsToKeep{DirectInit, std::size_t(scene.mappingBound()), true};
    objectsToKeep.reset(8);
    objectsToKeep.reset(3);
    objectsToKeep.reset(2);

    Trade::SceneData filtered = filterObjects(scene, objectsToKeep);
    CORRADE_COMPARE(filtered.fieldCount(), 7);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 176);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedShort>({7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({2, 3}),
        TestSuite::Compare::Container);

    /* Mapping shared with Mesh */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::MeshMaterial));
    CORRADE_COMPARE(filtered.mapping(Trade::SceneField::MeshMaterial).data(),
        filtered.mapping(Trade::SceneField::Mesh).data());
    CORRADE_COMPARE_AS(filtered.field<Byte>(Trade::SceneField::MeshMaterial),
        Containers::arrayView<Byte>({-1, 7}),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Translation));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Translation),
        Containers::arrayView<UnsignedShort>({1, 7, 15}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<Vector2>(Trade::SceneField::Translation),
        Containers::arrayView<Vector2>({{1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}}),
        TestSuite::Compare::Container);

    /* Mapping shared with Translation */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Rotation));
    CORRADE_COMPARE(filtered.mapping(Trade::SceneField::Rotation).data(),
        filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Complex>(Trade::SceneField::Rotation),
        Containers::arrayView<Complex>({Complex::rotation(15.0_degf), Complex::rotation(30.0_degf), Complex::rotation(45.0_degf)}),
        TestSuite::Compare::Container);

    /* Mapping shared with Translation again */
    CORRADE_VERIFY(filtered.hasField(Trade::sceneFieldCustom(15)));
    CORRADE_COMPARE(filtered.mapping(Trade::sceneFieldCustom(15)).data(),
        filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Float>(Trade::sceneFieldCustom(15)),
        Containers::arrayView({10.0f, -5.0f, 555.0f}),
        TestSuite::Compare::Container);

    /* These fields don't share any mapping even though they could */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Light));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedShort>({7}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(filtered.mapping(Trade::SceneField::Light).data() != filtered.mapping(Trade::SceneField::Mesh).data());
    CORRADE_COMPARE_AS(filtered.field<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedInt>({34}),
        TestSuite::Compare::Container);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Parent));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView<UnsignedShort>({1, 7, 15}),
        TestSuite::Compare::Container);
    CORRADE_VERIFY(filtered.mapping(Trade::SceneField::Parent).data() != filtered.mapping(Trade::SceneField::Translation).data());
    CORRADE_COMPARE_AS(filtered.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(data->parent),
        TestSuite::Compare::Container);
}

void FilterTest::objectsSharedMappingAllRemoved() {
    const struct {
        UnsignedShort meshMaterialMapping[3]{8, 3, 2};
        UnsignedByte mesh[3]{};
        UnsignedShort lightMapping[3]{2, 1, 3};
        UnsignedInt light[3]{66666, 23, 66666};
        Byte meshMaterial[3]{};
    } data[1];

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 76, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(data->lightMapping),
            Containers::arrayView(data->light)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(data->meshMaterialMapping),
            Containers::arrayView(data->meshMaterial)},
    }};

    Containers::BitArray objectsToKeep{DirectInit, std::size_t(scene.mappingBound()), true};
    objectsToKeep.reset(8);
    objectsToKeep.reset(3);
    objectsToKeep.reset(2);

    Trade::SceneData filtered = filterObjects(scene, objectsToKeep);
    CORRADE_COMPARE(filtered.fieldCount(), 3);
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 76);

    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Mesh));
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::Mesh), 0);

    /* This one should reuse the (emptied) Mesh mapping instead of going
       through everything again */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::MeshMaterial));
    CORRADE_COMPARE(filtered.fieldSize(Trade::SceneField::MeshMaterial), 0);

    /* Other fields get filtered as usual */
    CORRADE_VERIFY(filtered.hasField(Trade::SceneField::Light));
    CORRADE_COMPARE_AS(filtered.mapping<UnsignedShort>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedShort>({1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(filtered.field<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedInt>({23}),
        TestSuite::Compare::Container);
}

void FilterTest::objectsWrongBitCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 176, nullptr, {}};

    Containers::String out;
    Error redirectError{&out};
    filterObjects(scene, Containers::BitArray{ValueInit, 177});
    CORRADE_COMPARE(out, "SceneTools::filterObjects(): expected 176 bits but got 177\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::FilterTest)
