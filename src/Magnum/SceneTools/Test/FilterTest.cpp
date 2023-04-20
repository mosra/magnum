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

#include <sstream>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneTools/Filter.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct FilterTest: TestSuite::Tester {
    explicit FilterTest();

    void fields();
    void fieldsWrongBitCount();

    void onlyFields();
    void onlyFieldsNoFieldData();

    void exceptFields();
    void exceptFieldsNoFieldData();
};

FilterTest::FilterTest() {
    addTests({&FilterTest::fields,
              &FilterTest::fieldsWrongBitCount,

              &FilterTest::onlyFields,
              &FilterTest::onlyFieldsNoFieldData,

              &FilterTest::exceptFields,
              &FilterTest::exceptFieldsNoFieldData});
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

    std::ostringstream out;
    Error redirectError{&out};
    filterFields(scene, Containers::BitArray{ValueInit, 3});
    CORRADE_COMPARE(out.str(), "SceneTools::filterFields(): expected 2 bits but got 3\n");
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

    Trade::SceneData filtered = filterOnlyFields(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 331, nullptr, {}}, {
        Trade::SceneField::MeshMaterial,
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 331);
    CORRADE_COMPARE(filtered.data().data(), nullptr);
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});
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

    Trade::SceneData filtered = filterOnlyFields(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 331, nullptr, {}}, {
        Trade::SceneField::MeshMaterial,
    });
    CORRADE_COMPARE(filtered.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(filtered.mappingBound(), 331);
    CORRADE_COMPARE(filtered.data().data(), nullptr);
    CORRADE_COMPARE(filtered.dataFlags(), Trade::DataFlags{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::FilterTest)
