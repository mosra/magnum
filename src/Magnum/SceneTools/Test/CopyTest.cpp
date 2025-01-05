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
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/SceneTools/Copy.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct CopyTest: TestSuite::Tester {
    explicit CopyTest();

    void copy();

    void copyRvalueNotOwned();
    void copyRvalueDataFieldsOwned();
    void copyRvalueDataOwned();
    void copyRvalueFieldsOwned();

    void reference();
    void referenceNoDataFieldData();

    void mutableReference();
    void mutableReferenceNoDataFieldData();
    void mutableReferenceNotMutable();
};

CopyTest::CopyTest() {
    addTests({&CopyTest::copy,

              &CopyTest::copyRvalueNotOwned,
              &CopyTest::copyRvalueDataFieldsOwned,
              &CopyTest::copyRvalueDataOwned,
              &CopyTest::copyRvalueFieldsOwned,

              &CopyTest::reference,
              &CopyTest::referenceNoDataFieldData,

              &CopyTest::mutableReference,
              &CopyTest::mutableReferenceNoDataFieldData,
              &CopyTest::mutableReferenceNotMutable});
}

void CopyTest::copy() {
    const struct Data {
        UnsignedShort parentMeshMapping[4];
        Long parent[4];
        UnsignedInt mesh[4];
        UnsignedShort bitsMapping[3];
        bool bit[3];
        UnsignedInt bits[3];
        UnsignedShort stringMapping[2];
        char stringData[7];
        UnsignedByte strings[2];
    } data[]{{
        {1, 3, 5, 2},
        {-1, 1, 3, -1},
        {6667, 29862, 12393, 31222},
        {16, 12, 10},
        {false, true, false},
        {0xa, 0x4, 0xe},
        {100, 200},
        {'N', 'O', '\0', 'y', 'e', 's', '\0'},
        {3, 7}
    }};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 201, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(data->parentMeshMapping),
            Containers::arrayView(data->parent)},
        /* Offset-only field */
        Trade::SceneFieldData{Trade::SceneField::Mesh, 4,
            Trade::SceneMappingType::UnsignedShort, offsetof(Data, parentMeshMapping), sizeof(UnsignedShort),
            Trade::SceneFieldType::UnsignedInt, offsetof(Data, mesh), sizeof(UnsignedInt)},
        /* Bit field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(37),
            Containers::arrayView(data->bitsMapping),
            Containers::stridedArrayView(data->bit).sliceBit(0)},
        /* Bit array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(38),
            Containers::arrayView(data->bitsMapping),
            Containers::StridedBitArrayView2D{Containers::BitArrayView{data->bits}, data->bits, 1, {3, 3}, {sizeof(UnsignedInt)*8, 1}}},
        /* String field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(664),
            Containers::arrayView(data->stringMapping),
            data->stringData, Trade::SceneFieldType::StringOffset8,
            Containers::arrayView(data->strings), Trade::SceneFieldFlag::NullTerminatedString}
    }};

    Trade::SceneData copy = SceneTools::copy(scene);
    CORRADE_COMPARE(copy.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(copy.mappingBound(), 201);

    CORRADE_COMPARE(copy.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(copy.fieldCount(), 5);

    for(std::size_t i = 0; i != copy.fieldCount(); ++i) {
        CORRADE_ITERATION(i);

        CORRADE_COMPARE(copy.fieldName(i), scene.fieldName(i));
        CORRADE_COMPARE(copy.fieldSize(i), scene.fieldSize(i));
        CORRADE_COMPARE(copy.fieldArraySize(i), scene.fieldArraySize(i));
        CORRADE_COMPARE(copy.fieldType(i), scene.fieldType(i));
        /* Checks also for offset-only flags */
        CORRADE_COMPARE(copy.fieldFlags(i), scene.fieldFlags(i));
    }

    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView(data->parentMeshMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<Long>(Trade::SceneField::Parent),
        Containers::arrayView(data->parent),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Mesh),
        Containers::arrayView(data->parentMeshMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<UnsignedInt>(Trade::SceneField::Mesh),
        Containers::arrayView(data->mesh),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::sceneFieldCustom(37)),
        Containers::arrayView(data->bitsMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.fieldBits(Trade::sceneFieldCustom(37)),
        Containers::stridedArrayView(data->bit).sliceBit(0),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::sceneFieldCustom(38)),
        Containers::arrayView(data->bitsMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.fieldBitArrays(Trade::sceneFieldCustom(38))[0],
        Containers::stridedArrayView({true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.fieldBitArrays(Trade::sceneFieldCustom(38))[1],
        Containers::stridedArrayView({false, true, false}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.fieldBitArrays(Trade::sceneFieldCustom(38))[2],
        Containers::stridedArrayView({true, true, true}).sliceBit(0),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::sceneFieldCustom(664)),
        Containers::arrayView(data->stringMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.fieldStrings(Trade::sceneFieldCustom(664)),
        (Containers::StringIterable{"NO", "yes"}),
        TestSuite::Compare::Container);

    /* The data layout should be the same and thus the raw data should match
       as well */
    CORRADE_COMPARE_AS(copy.data(),
        scene.data(),
        TestSuite::Compare::Container);

    /* The data should have a default deleter to make this usable in plugins */
    Containers::Array<char> sceneData = copy.releaseData();
    Containers::Array<Trade::SceneFieldData> fieldData = copy.releaseFieldData();
    CORRADE_VERIFY(!sceneData.deleter());
    CORRADE_VERIFY(!fieldData.deleter());
}

void CopyTest::copyRvalueNotOwned() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    } data[]{{
        {1, 3},
        {-1, 1}
    }};
    const Trade::SceneFieldData fields[]{
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(data->parentMapping),
            Containers::arrayView(data->parent)}
    };

    Trade::SceneData copy = SceneTools::copy(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 12,
        Trade::DataFlag::Mutable|Trade::DataFlag::ExternallyOwned, data,
        Trade::sceneFieldDataNonOwningArray(fields)});

    CORRADE_COMPARE(copy.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(copy.mappingBound(), 12);
    CORRADE_COMPARE(copy.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(copy.fieldCount(), 1);
    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView(data->parentMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(data->parent),
        TestSuite::Compare::Container);

    /* Nothing should be copied in this case */
    CORRADE_VERIFY(copy.data().data() != static_cast<void*>(data));
    CORRADE_VERIFY(copy.fieldData().data() != fields);
}

void CopyTest::copyRvalueDataFieldsOwned() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    };
    Containers::Array<char> data{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(data);
    Utility::copy({{
        {1, 3},
        {-1, 1}
    }}, view);

    /* InPlaceInit causes a non-default deleter to be used, which would cause
       a copy to be made internally */
    Containers::Array<Trade::SceneFieldData> fields{ValueInit, 1};
    fields[0] = Trade::SceneFieldData{Trade::SceneField::Parent,
        Containers::arrayView(view[0].parentMapping),
        Containers::arrayView(view[0].parent)};
    const Trade::SceneFieldData* originalFields = fields;

    Trade::SceneData copy = SceneTools::copy(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 12,
        Utility::move(data), Utility::move(fields)});

    CORRADE_COMPARE(copy.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(copy.mappingBound(), 12);
    CORRADE_COMPARE(copy.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(copy.fieldCount(), 1);
    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView(view[0].parentMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(view[0].parent),
        TestSuite::Compare::Container);

    /* Both data should be transferred without any copy */
    CORRADE_COMPARE(copy.data().data(), view.data());
    CORRADE_COMPARE(copy.fieldData().data(), originalFields);
}

void CopyTest::copyRvalueDataOwned() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    };
    Containers::Array<char> data{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(data);
    Utility::copy({{
        {1, 3},
        {-1, 1}
    }}, view);

    Trade::SceneFieldData fields[]{
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(view[0].parentMapping),
            Containers::arrayView(view[0].parent)}
    };

    Trade::SceneData copy = SceneTools::copy(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 12,
        Utility::move(data), Trade::sceneFieldDataNonOwningArray(fields)});

    CORRADE_COMPARE(copy.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(copy.mappingBound(), 12);
    CORRADE_COMPARE(copy.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(copy.fieldCount(), 1);
    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView(view[0].parentMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(view[0].parent),
        TestSuite::Compare::Container);

    /* Only data should be transferred, fields copied */
    CORRADE_COMPARE(copy.data().data(), view.data());
    CORRADE_VERIFY(copy.fieldData().data() != fields);
}

void CopyTest::copyRvalueFieldsOwned() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    } data[]{{
        {1, 3},
        {-1, 1}
    }};

    /* InPlaceInit causes a non-default deleter to be used, which would cause
       a copy to be made internally */
    Containers::Array<Trade::SceneFieldData> fields{ValueInit, 1};
    fields[0] = Trade::SceneFieldData{Trade::SceneField::Parent,
        Containers::arrayView(data->parentMapping),
        Containers::arrayView(data->parent)};
    const Trade::SceneFieldData* originalFields = fields;

    Trade::SceneData copy = SceneTools::copy(Trade::SceneData{Trade::SceneMappingType::UnsignedShort, 12,
        Trade::DataFlag::ExternallyOwned, data, Utility::move(fields)});

    CORRADE_COMPARE(copy.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(copy.mappingBound(), 12);
    CORRADE_COMPARE(copy.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);

    CORRADE_COMPARE(copy.fieldCount(), 1);
    CORRADE_COMPARE_AS(copy.mapping<UnsignedShort>(Trade::SceneField::Parent),
        Containers::arrayView(data->parentMapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(copy.field<Int>(Trade::SceneField::Parent),
        Containers::arrayView(data->parent),
        TestSuite::Compare::Container);

    /* Data should be copied */
    CORRADE_VERIFY(copy.data().data() != static_cast<void*>(data));
    {
        CORRADE_EXPECT_FAIL("Field data currently get copied always when they need to be modified.");
        CORRADE_COMPARE(copy.fieldData().data(), originalFields);
    }
}

void CopyTest::reference() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    };
    Containers::Array<char> data{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(data);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 31, Utility::move(data), {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(view[0].parentMapping),
            Containers::arrayView(view[0].parent)}
    }};

    Trade::SceneData reference = SceneTools::reference(scene);
    CORRADE_COMPARE(reference.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(reference.mappingBound(), 31);
    CORRADE_COMPARE(reference.dataFlags(), Trade::DataFlags{});
    CORRADE_COMPARE(static_cast<const void*>(reference.data().data()), scene.data().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.fieldData().data()), scene.fieldData().data());
}

void CopyTest::referenceNoDataFieldData() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 31, {}, {}};

    Trade::SceneData reference = SceneTools::reference(scene);
    CORRADE_COMPARE(reference.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(reference.mappingBound(), 31);
    CORRADE_COMPARE(reference.dataFlags(), Trade::DataFlags{});
    CORRADE_VERIFY(!static_cast<const void*>(reference.data().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.fieldData().data()));
}

void CopyTest::mutableReference() {
    struct Data {
        UnsignedShort parentMapping[2];
        Int parent[2];
    };
    Containers::Array<char> data{NoInit, sizeof(Data)};
    Containers::StridedArrayView1D<Data> view = Containers::arrayCast<Data>(data);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 31, Utility::move(data), {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(view[0].parentMapping),
            Containers::arrayView(view[0].parent)}
    }};

    Trade::SceneData reference = SceneTools::mutableReference(scene);
    CORRADE_COMPARE(reference.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(reference.mappingBound(), 31);
    CORRADE_COMPARE(reference.dataFlags(), Trade::DataFlag::Mutable);
    CORRADE_COMPARE(static_cast<const void*>(reference.data().data()), scene.data().data());
    CORRADE_COMPARE(static_cast<const void*>(reference.fieldData().data()), scene.fieldData().data());
}

void CopyTest::mutableReferenceNoDataFieldData() {
    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 31, {}, {}};

    Trade::SceneData reference = SceneTools::mutableReference(scene);
    CORRADE_COMPARE(reference.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(reference.mappingBound(), 31);
    CORRADE_COMPARE(reference.dataFlags(), Trade::DataFlag::Mutable);
    CORRADE_VERIFY(!static_cast<const void*>(reference.data().data()));
    CORRADE_VERIFY(!static_cast<const void*>(reference.fieldData().data()));
}

void CopyTest::mutableReferenceNotMutable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct {
        UnsignedShort parentMapping[2];
        Int parent[2];
    } data[1]{};

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedShort, 31, Trade::DataFlag::Global, data, {
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::arrayView(data[0].parentMapping),
            Containers::arrayView(data[0].parent)}
    }};
    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Global);

    Containers::String out;
    Error redirectError{&out};
    SceneTools::mutableReference(scene);
    CORRADE_COMPARE(out, "SceneTools::mutableReference(): data not mutable\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::CopyTest)
