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

#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/TypeTraits.h"
#include "Magnum/SceneTools/Map.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct MapTest: TestSuite::Tester {
    explicit MapTest();

    template<class T> void indexField();
    template<class T> void indexFieldSigned();
    void indexFieldOffsetOnly();
    void indexFieldFieldNotFound();
    void indexFieldInvalidType();
    void indexFieldArrayField();
    void indexFieldIndexOutOfBounds();
    void indexFieldMappingNotRepresentable();

    void indexFieldRvalue();
    void indexFieldRvalueSigned();
    void indexFieldRvalueNotOwned();
    void indexFieldRvalueNotFullType();
};

const struct {
    const char* name;
    bool inPlace;
    bool byName;
} IndexFieldData[]{
    {"in place, by ID", true, false},
    {"in place, by name", true, true},
    {"by ID", false, false},
    {"by name", false, true}
};

const struct {
    const char* name;
    bool byName;
} IndexFieldRvalueData[]{
    {"by ID", false},
    {"by name", true}
};

MapTest::MapTest() {
    addInstancedTests<MapTest>({
        &MapTest::indexField<UnsignedInt>,
        &MapTest::indexField<UnsignedShort>,
        &MapTest::indexField<UnsignedByte>,
        &MapTest::indexFieldSigned<Int>,
        &MapTest::indexFieldSigned<Short>,
        &MapTest::indexFieldSigned<Byte>},
        Containers::arraySize(IndexFieldData));

    addTests({&MapTest::indexFieldOffsetOnly,
              &MapTest::indexFieldFieldNotFound,
              &MapTest::indexFieldInvalidType,
              &MapTest::indexFieldArrayField,
              &MapTest::indexFieldIndexOutOfBounds,
              &MapTest::indexFieldMappingNotRepresentable});

    addInstancedTests({&MapTest::indexFieldRvalue,
                       &MapTest::indexFieldRvalueSigned},
        Containers::arraySize(IndexFieldRvalueData));

    addTests({&MapTest::indexFieldRvalueNotOwned,
              &MapTest::indexFieldRvalueNotFullType});
}

template<class> struct IndexFieldTraits;

template<> struct IndexFieldTraits<UnsignedInt> {
    enum: UnsignedInt { Max = 0xffffffffu };
};
template<> struct IndexFieldTraits<Int> {
    enum: Int { Max = 0x7fffffffu };
};
template<> struct IndexFieldTraits<UnsignedShort> {
    enum: UnsignedShort { Max = 0xffff };
};
template<> struct IndexFieldTraits<Short> {
    enum: Short { Max = 0x7fff };
};
template<> struct IndexFieldTraits<UnsignedByte> {
    enum: UnsignedByte { Max = 0xff };
};
template<> struct IndexFieldTraits<Byte> {
    enum: Byte { Max = 0x7f };
};

template<class T> void MapTest::indexField() {
    auto&& data = IndexFieldData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    struct {
        UnsignedLong parentMapping[5];
        Int parent[5];
        UnsignedLong meshMaterialMapping[4];
        T mesh[4];
        Short custom[4][2];
    } sceneData[]{{
        {0, 11, 22, 33, 44},
        {-1, -1, 1, 4, 0},
        {0, 33, 2, 2},
        {5, 9, 1, 0}, /* this one gets mapped */
        {{9, 2}, {-1, 3}, {5, 6}, {0, 1}},
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedLong, 5, Trade::DataFlag::Mutable, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(sceneData->parentMapping),
            Containers::arrayView(sceneData->parent)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->mesh),
            /* Verify that the flags get preserved */
            Trade::SceneFieldFlag::MultiEntry},
        /* Verify that array fields are supported in non-mapped fields */
        Trade::SceneFieldData{Trade::sceneFieldCustom(1),
            Trade::SceneMappingType::UnsignedLong,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Trade::SceneFieldType::Short,
            Containers::arrayView(sceneData->custom), 2},
    }};

    /* The 0xffffffffu values shouldn't be used for anything */
    const UnsignedInt mapping[]{
        12, 0, 0xffffffffu, 0xffffffffu, 0xffffffffu,
        /* If not doing an in-place mapping, the output doesn't have to fit
           into the original type */
        data.inPlace ? UnsignedInt(IndexFieldTraits<T>::Max) : 1000000u,
        0xffffffffu, 0xffffffffu, 0xffffffffu, 3
    };

    Trade::SceneData output{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}};
    if(data.inPlace) {
        if(data.byName)
            mapIndexFieldInPlace(scene, Trade::SceneField::Mesh, mapping);
        else
            mapIndexFieldInPlace(scene, 1, mapping);
    } else {
        if(data.byName)
            output = mapIndexField(scene, Trade::SceneField::Mesh, mapping);
        else
            output = mapIndexField(scene, 1, mapping);
    }
    const Trade::SceneData& result = data.inPlace ? scene : output;

    /* Mapping should stay untouched */
    CORRADE_COMPARE(result.mappingBound(), 5);
    CORRADE_COMPARE(result.mappingType(), Trade::SceneMappingType::UnsignedLong);
    CORRADE_COMPARE_AS(result.mapping<UnsignedLong>(0), Containers::arrayView<UnsignedLong>({
        0, 11, 22, 33, 44
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(result.mapping<UnsignedLong>(1), Containers::arrayView<UnsignedLong>({
        0, 33, 2, 2
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(result.mapping<UnsignedLong>(2), Containers::arrayView<UnsignedLong>({
        0, 33, 2, 2
    }), TestSuite::Compare::Container);

    /* All fields except the mesh should stay the same as before. With the
       in-place variant the type should stay the same, otherwise expanded to
       32-bit. */
    CORRADE_COMPARE_AS(result.field<Int>(0), Containers::arrayView<Int>({
        -1, -1, 1, 4, 0
    }), TestSuite::Compare::Container);
    if(data.inPlace) {
        CORRADE_COMPARE(result.fieldType(1), Trade::Implementation::SceneFieldTypeFor<T>::type());
        CORRADE_COMPARE_AS(result.field<T>(1), Containers::arrayView<T>({
            IndexFieldTraits<T>::Max, 3, 0, 12
        }), TestSuite::Compare::Container);
    } else {
        CORRADE_COMPARE(result.fieldType(1), Trade::SceneFieldType::UnsignedInt);
        CORRADE_COMPARE_AS(result.field<UnsignedInt>(1), Containers::arrayView<UnsignedInt>({
            1000000, 3, 0, 12
        }), TestSuite::Compare::Container);
    }
    /* The flags should be preserved */
    CORRADE_COMPARE(result.fieldFlags(1), Trade::SceneFieldFlag::MultiEntry);
    /* Non-mapped array field should be copied as-is without any assert */
    CORRADE_COMPARE_AS((result.field<Short[]>(2).transposed<0, 1>()[0]), Containers::arrayView<Short>({
        9, -1, 5, 0
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((result.field<Short[]>(2).transposed<0, 1>()[1]), Containers::arrayView<Short>({
        2, 3, 6, 1
    }), TestSuite::Compare::Container);
}

template<class T> void MapTest::indexFieldSigned() {
    auto&& data = IndexFieldData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Similar to indexField(), except that here the meshMaterial gets mapped
       instead */

    struct {
        UnsignedByte parentMapping[5];
        Int parent[5];
        UnsignedByte meshMaterialMapping[4];
        T meshMaterial[4];
        UnsignedShort mesh[4];
    } sceneData[]{{
        {0, 11, 22, 33, 44},
        {-1, -1, 1, 4, 0},
        {0, 33, 2, 2},
        {9, -1, 5, 1}, /* this one gets mapped */
        {5, 9, 1, 0},
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 55, Trade::DataFlag::Mutable, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(sceneData->parentMapping),
            Containers::arrayView(sceneData->parent)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->mesh)},
    }};

    /* The 0xffffffffu values shouldn't be used for anything */
    const UnsignedInt mapping[]{
        0xffffffffu, 12, 0xffffffffu, 0xffffffffu, 0xffffffffu,
        IndexFieldTraits<T>::Max, 0xffffffffu, 0xffffffffu, 0xffffffffu, 3
    };

    Trade::SceneData output{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}};
    if(data.inPlace) {
        if(data.byName)
            mapIndexFieldInPlace(scene, Trade::SceneField::MeshMaterial, mapping);
        else
            mapIndexFieldInPlace(scene, 1, mapping);
    } else {
        if(data.byName)
            output = mapIndexField(scene, Trade::SceneField::MeshMaterial, mapping);
        else
            output = mapIndexField(scene, 1, mapping);
    }
    const Trade::SceneData& result = data.inPlace ? scene : output;

    /* Mapping should stay untouched */
    CORRADE_COMPARE(result.mappingBound(), 55);
    CORRADE_COMPARE(result.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE_AS(result.mapping<UnsignedByte>(0), Containers::arrayView<UnsignedByte>({
        0, 11, 22, 33, 44
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(result.mapping<UnsignedByte>(1), Containers::arrayView<UnsignedByte>({
        0, 33, 2, 2
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(result.mapping<UnsignedByte>(2), Containers::arrayView<UnsignedByte>({
        0, 33, 2, 2
    }), TestSuite::Compare::Container);

    /* All fields except the mesh material should stay the same as before. With
       the in-place variant the type should stay the same, otherwise expanded
       to 32-bit. */
    CORRADE_COMPARE_AS(result.field<Int>(0), Containers::arrayView<Int>({
        -1, -1, 1, 4, 0
    }), TestSuite::Compare::Container);
    if(data.inPlace) {
        CORRADE_COMPARE(result.fieldType(1), Trade::Implementation::SceneFieldTypeFor<T>::type());
        CORRADE_COMPARE_AS(result.field<T>(1), Containers::arrayView<T>({
            3, -1, IndexFieldTraits<T>::Max, 12
        }), TestSuite::Compare::Container);
    } else {
        CORRADE_COMPARE(result.fieldType(1), Trade::SceneFieldType::Int);
        CORRADE_COMPARE_AS(result.field<Int>(1), Containers::arrayView<Int>({
            3, -1, IndexFieldTraits<T>::Max, 12
        }), TestSuite::Compare::Container);
    }
    CORRADE_COMPARE_AS(result.field<UnsignedShort>(2), Containers::arrayView<UnsignedShort>({
        5, 9, 1, 0
    }), TestSuite::Compare::Container);
}

void MapTest::indexFieldOffsetOnly() {
    /* Subset of indexField() with the mapped field being specified as
       offset-only. Should "just work" without any special treatment needed in
       the implementation. */

    struct SceneData {
        UnsignedShort meshMaterialMapping[4];
        Byte meshMaterial[4];
        UnsignedShort mesh[4];
    } sceneData[]{{
        {0, 33, 2, 2},
        {9, -1, 5, 1},
        {5, 9, 1, 0}, /* this one gets mapped */
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 5, Trade::DataFlag::Mutable, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Mesh, 4,
            Trade::SceneMappingType::UnsignedShort, offsetof(SceneData, meshMaterialMapping), sizeof(UnsignedShort),
            Trade::SceneFieldType::UnsignedShort, offsetof(SceneData, mesh), sizeof(UnsignedShort)},
    }};

    /* The 0xffffffffu values shouldn't be used for anything */
    const UnsignedInt mapping[]{
        12, 0, 0xffffffffu, 0xffffffffu, 0xffffffffu,
        0xffff, 0xffffffffu, 0xffffffffu, 0xffffffffu, 3
    };
    mapIndexFieldInPlace(scene, Trade::SceneField::Mesh, mapping);
    CORRADE_COMPARE_AS(scene.field<UnsignedShort>(1), Containers::arrayView<UnsignedShort>({
        0xffff, 3, 0, 12
    }), TestSuite::Compare::Container);
}

void MapTest::indexFieldFieldNotFound() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::Int, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::UnsignedInt, nullptr},
    }};

    UnsignedInt mapping[5]{};

    Containers::String out;
    Error redirectError{&out};
    mapIndexField(scene, 2, mapping);
    mapIndexFieldInPlace(scene, 2, mapping);
    mapIndexField(scene, Trade::SceneField::MeshMaterial, mapping);
    mapIndexFieldInPlace(scene, Trade::SceneField::MeshMaterial, mapping);
    CORRADE_COMPARE_AS(out,
        "SceneTools::mapIndexField(): index 2 out of range for 2 fields\n"
        "SceneTools::mapIndexFieldInPlace(): index 2 out of range for 2 fields\n"
        "SceneTools::mapIndexField(): field Trade::SceneField::MeshMaterial not found\n"
        "SceneTools::mapIndexFieldInPlace(): field Trade::SceneField::MeshMaterial not found\n",
        TestSuite::Compare::String);
}

void MapTest::indexFieldInvalidType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::Long, nullptr},
    }};

    UnsignedInt mapping[5]{};

    Containers::String out;
    Error redirectError{&out};
    mapIndexField(scene, 1, mapping);
    mapIndexFieldInPlace(scene, 1, mapping);
    CORRADE_COMPARE_AS(out,
        "SceneTools::mapIndexField(): unsupported field type Trade::SceneFieldType::Long\n"
        "SceneTools::mapIndexFieldInPlace(): unsupported field type Trade::SceneFieldType::Long\n",
        TestSuite::Compare::String);
}

void MapTest::indexFieldArrayField() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::UnsignedInt, nullptr},
        Trade::SceneFieldData{Trade::sceneFieldCustom(0x1337),
            Trade::SceneMappingType::UnsignedInt, nullptr,
            Trade::SceneFieldType::Byte, nullptr, 3},
    }};

    UnsignedInt mapping[5]{};

    Containers::String out;
    Error redirectError{&out};
    mapIndexField(scene, 1, mapping);
    mapIndexFieldInPlace(scene, 1, mapping);
    CORRADE_COMPARE_AS(out,
        "SceneTools::mapIndexField(): array field mapping isn't supported\n"
        "SceneTools::mapIndexFieldInPlace(): array field mapping isn't supported\n",
        TestSuite::Compare::String);
}

void MapTest::indexFieldIndexOutOfBounds() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort meshMaterialMapping[4];
        Byte meshMaterial[4];
        UnsignedShort mesh[4];
    } sceneData[]{{
        {},
        {5, -1, 9, -2},
        {5, 10, 1, 0},
    }};
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 5, {}, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(sceneData->meshMaterialMapping),
            Containers::arrayView(sceneData->meshMaterial)},
    }};

    const UnsignedInt mapping9[9]{};
    const UnsignedInt mapping10[10]{};

    Containers::String out;
    Error redirectError{&out};
    mapIndexField(scene, Trade::SceneField::MeshMaterial, mapping10);
    mapIndexField(scene, Trade::SceneField::MeshMaterial, mapping9);
    mapIndexField(scene, Trade::SceneField::Mesh, mapping10);
    CORRADE_COMPARE_AS(out,
        "SceneTools::mapIndexFieldInPlace(): index -2 out of range for 10 mapping values\n"
        "SceneTools::mapIndexFieldInPlace(): index 9 out of range for 9 mapping values\n"
        "SceneTools::mapIndexFieldInPlace(): index 10 out of range for 10 mapping values\n",
        TestSuite::Compare::String);
}

void MapTest::indexFieldMappingNotRepresentable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    struct {
        UnsignedShort mapping[4];
        UnsignedShort mesh[4];
        UnsignedByte light[4];
        Int custom1[4];
        Short custom2[4];
        Byte meshMaterial[4];
    } sceneData[]{{
        {},
        {0, 4, 3, 1},
        {0, 4, 3, 1},
        {0, -1, 3, 1},
        {0, -1, 2, 1},
        {0, -1, 0, 1},
    }};
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 1, Trade::DataFlag::Mutable, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->light)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(1),
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->custom1)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(2),
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->custom2)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->meshMaterial)},
    }};

    UnsignedInt mappingUnsigned[]{
        /* Index 1 is too large for UnsignedByte, 2 isn't used, 3 is
           too large for UnsignedShort */
        1, 0x100, 0xffffffffu, 0x10000, 3
    };
    UnsignedInt mappingSigned[]{
        /* Index 1 is too large for a Byte, 3 too large for Int, 2 too large
           for Short */
        1, 0x80, 0x8000, 0x80000000u
    };

    /* These should all be okay as they expand to 32 bits */
    mapIndexField(scene, Trade::SceneField::Mesh, mappingUnsigned);
    mapIndexField(scene, Trade::SceneField::Light, mappingUnsigned);
    mapIndexField(scene, Trade::sceneFieldCustom(2), mappingSigned);
    mapIndexField(scene, Trade::SceneField::MeshMaterial, mappingSigned);

    Containers::String out;
    Error redirectError{&out};
    mapIndexFieldInPlace(scene, Trade::SceneField::Mesh, mappingUnsigned);
    mapIndexFieldInPlace(scene, Trade::SceneField::Light, mappingUnsigned);
    /* This one expands to 32 bits but is still signed which isn't enough */
    mapIndexField(scene, Trade::sceneFieldCustom(1), mappingSigned);
    mapIndexFieldInPlace(scene, Trade::sceneFieldCustom(1), mappingSigned);
    mapIndexFieldInPlace(scene, Trade::sceneFieldCustom(2), mappingSigned);
    mapIndexFieldInPlace(scene, Trade::SceneField::MeshMaterial, mappingSigned);
    CORRADE_COMPARE_AS(out,
        "SceneTools::mapIndexFieldInPlace(): mapping value 65536 not representable in Trade::SceneFieldType::UnsignedShort\n"
        "SceneTools::mapIndexFieldInPlace(): mapping value 65536 not representable in Trade::SceneFieldType::UnsignedByte\n"
        "SceneTools::mapIndexFieldInPlace(): mapping value 2147483648 not representable in Trade::SceneFieldType::Int\n"
        "SceneTools::mapIndexFieldInPlace(): mapping value 2147483648 not representable in Trade::SceneFieldType::Int\n"
        "SceneTools::mapIndexFieldInPlace(): mapping value 32768 not representable in Trade::SceneFieldType::Short\n"
        "SceneTools::mapIndexFieldInPlace(): mapping value 128 not representable in Trade::SceneFieldType::Byte\n",
        TestSuite::Compare::String);
}

void MapTest::indexFieldRvalue() {
    auto&& data = IndexFieldRvalueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Data {
        UnsignedByte mapping[4];
        Short meshMaterial[4];
        UnsignedInt mesh[4];
    };
    Containers::Array<char> sceneData{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(sceneData);
    Utility::copy({{
        {77, 33, 44, 66},
        {2, -1, 0, 1},
        {3, 4, 1, 0},
    }}, view);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 88, Utility::move(sceneData), {
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].mesh)},
    }};
    const Trade::SceneFieldData* originalFields = scene.fieldData();

    const UnsignedInt mapping[]{
        15, 16, 0xffffffffu, 7, 9
    };
    Trade::SceneData mapped = data.byName ?
        mapIndexField(Utility::move(scene), Trade::SceneField::Mesh, mapping) :
        mapIndexField(Utility::move(scene), 1, mapping);

    /* Mapping should stay untouched */
    CORRADE_COMPARE(mapped.mappingBound(), 88);
    CORRADE_COMPARE(mapped.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(0), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(1), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);

    /* Mesh should be mapped, materials should stay the same as before */
    CORRADE_COMPARE_AS(mapped.field<Short>(0), Containers::arrayView<Short>({
        2, -1, 0, 1
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.field<UnsignedInt>(1), Containers::arrayView<UnsignedInt>({
        7, 9, 16, 15
    }), TestSuite::Compare::Container);

    /* Both data should be transferred without any copy */
    CORRADE_COMPARE(mapped.data().data(), view.data());
    CORRADE_COMPARE(mapped.fieldData().data(), originalFields);
}

void MapTest::indexFieldRvalueSigned() {
    auto&& data = IndexFieldRvalueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Data {
        UnsignedByte mapping[4];
        UnsignedShort mesh[4];
        Int meshMaterial[4];
    };
    Containers::Array<char> sceneData{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(sceneData);
    Utility::copy({{
        {77, 33, 44, 66},
        {3, 4, 1, 0},
        {2, -1, 0, 3},
    }}, view);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 88, Utility::move(sceneData), {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].mesh)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].meshMaterial)},
    }};
    const Trade::SceneFieldData* originalFields = scene.fieldData();

    const UnsignedInt mapping[]{
        15, 0xffffffffu, 16, 7
    };
    Trade::SceneData mapped = data.byName ?
        mapIndexField(Utility::move(scene), Trade::SceneField::MeshMaterial, mapping) :
        mapIndexField(Utility::move(scene), 1, mapping);

    /* Mapping should stay untouched */
    CORRADE_COMPARE(mapped.mappingBound(), 88);
    CORRADE_COMPARE(mapped.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(0), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(1), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);

    /* Mesh should stay the same as before, materials should be mapped */
    CORRADE_COMPARE_AS(mapped.field<UnsignedShort>(0), Containers::arrayView<UnsignedShort>({
        3, 4, 1, 0
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.field<Int>(1), Containers::arrayView<Int>({
        16, -1, 15, 7
    }), TestSuite::Compare::Container);

    /* Both data should be transferred without any copy */
    CORRADE_COMPARE(mapped.data().data(), view.data());
    CORRADE_COMPARE(mapped.fieldData().data(), originalFields);
}

void MapTest::indexFieldRvalueNotOwned() {
    /* Like indexFieldRvalue(), but the data is not owned so it should perform
       a copy */

    struct {
        UnsignedByte mapping[4];
        Short meshMaterial[4];
        UnsignedInt mesh[4];
    } sceneData[]{{
        {77, 33, 44, 66},
        {2, -1, 0, 1},
        {3, 4, 1, 0},
    }};

    /* Mark the data as Mutable to test it isn't accidentally treated the same
       as Owned */
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 88, Trade::DataFlag::Mutable, sceneData, {
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(sceneData->mapping),
            Containers::arrayView(sceneData->mesh)},
    }};
    const Trade::SceneFieldData* originalFields = scene.fieldData();

    const UnsignedInt mapping[]{
        15, 16, 0xffffffffu, 7, 9
    };
    Trade::SceneData mapped = mapIndexField(Utility::move(scene), 1, mapping);

    /* Mapping should stay untouched */
    CORRADE_COMPARE(mapped.mappingBound(), 88);
    CORRADE_COMPARE(mapped.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(0), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(1), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);

    /* Mesh should be mapped, materials should stay the same as before */
    CORRADE_COMPARE_AS(mapped.field<Short>(0), Containers::arrayView<Short>({
        2, -1, 0, 1
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.field<UnsignedInt>(1), Containers::arrayView<UnsignedInt>({
        7, 9, 16, 15
    }), TestSuite::Compare::Container);

    /* Data should be copied */
    CORRADE_VERIFY(mapped.data().data() != static_cast<void*>(sceneData));
    CORRADE_VERIFY(mapped.fieldData().data() != originalFields);
}

void MapTest::indexFieldRvalueNotFullType() {
    /* Like indexFieldRvalue(), but the field is not a 32-bit type */

    struct Data {
        UnsignedByte mapping[4];
        Int meshMaterial[4];
        UnsignedShort mesh[4];
    };
    Containers::Array<char> sceneData{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(sceneData);
    Utility::copy({{
        {77, 33, 44, 66},
        {2, -1, 0, 1},
        {3, 4, 1, 0},
    }}, view);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedByte, 88, Utility::move(sceneData), {
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].meshMaterial)},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(view[0].mapping),
            Containers::arrayView(view[0].mesh)},
    }};
    const Trade::SceneFieldData* originalFields = scene.fieldData();

    const UnsignedInt mapping[]{
        15, 16, 0xffffffffu, 7, 9
    };
    Trade::SceneData mapped = mapIndexField(Utility::move(scene), 1, mapping);

    /* Mapping should stay untouched */
    CORRADE_COMPARE(mapped.mappingBound(), 88);
    CORRADE_COMPARE(mapped.mappingType(), Trade::SceneMappingType::UnsignedByte);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(0), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mapped.mapping<UnsignedByte>(1), Containers::arrayView<UnsignedByte>({
        77, 33, 44, 66
    }), TestSuite::Compare::Container);

    /* Mesh should be mapped, materials should stay the same as before. As a
       copy is performed, the type is expanded to 32 bits. */
    CORRADE_COMPARE_AS(mapped.field<Int>(0), Containers::arrayView<Int>({
        2, -1, 0, 1
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(mapped.fieldType(1), Trade::SceneFieldType::UnsignedInt);
    CORRADE_COMPARE_AS(mapped.field<UnsignedInt>(1), Containers::arrayView<UnsignedInt>({
        7, 9, 16, 15
    }), TestSuite::Compare::Container);

    /* Data should be copied */
    CORRADE_VERIFY(mapped.data().data() != static_cast<void*>(sceneData));
    CORRADE_VERIFY(mapped.fieldData().data() != originalFields);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::MapTest)
