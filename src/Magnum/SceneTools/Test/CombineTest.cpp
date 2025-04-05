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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/SceneTools/Combine.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Test { namespace {

struct CombineTest: TestSuite::Tester {
    explicit CombineTest();

    void fields();
    template<class T> void fieldsStrings();
    void fieldsAlignment();
    void fieldsMappingShared();
    void fieldsMappingSharedPartial();
    void fieldsMappingPlaceholderFieldPlaceholder();
    void fieldsMappingSharedFieldPlaceholder();
    void fieldsMappingSharedTRSPlaceholder();
    void fieldsMappingSharedMeshMaterialPlaceholder();

    void fieldsSharedMappingExpected();
    void fieldsStringPlaceholder();
    void fieldsOffsetOnly();
    void fieldsFromDataOffsetOnly();
};

using namespace Containers::Literals;

const struct {
    const char* name;
    Trade::SceneMappingType objectType;
} FieldsData[]{
    {"UnsignedByte output", Trade::SceneMappingType::UnsignedByte},
    {"UnsignedShort output", Trade::SceneMappingType::UnsignedShort},
    {"UnsignedInt output", Trade::SceneMappingType::UnsignedInt},
    {"UnsignedLong output", Trade::SceneMappingType::UnsignedLong},
};

const struct {
    const char* name;
    bool translationPresent, rotationPresent, scalingPresent;
    /* Either all or none can be placeholders */
    bool placeholders;
} FieldsMappingSharedTRSPlaceholderData[]{
    {"all",
        true, true, true,
        false},
    {"all, placeholders",
        true, true, true,
        true},
    {"rotation & scaling",
        false, true, true,
        false},
    {"rotation & scaling, placeholders",
        false, true, true,
        true},
    {"translation & scaling",
        true, false, true,
        false},
    {"translation & scaling, placeholders",
        true, false, true,
        true},
    {"translation & rotation",
        true, true, false,
        false},
    {"translation & rotation, placeholders",
        true, true, false,
        true},
    {"translation",
        true, false, false,
        false},
    {"translation, placeholder",
        true, false, false,
        true},
    {"rotation",
        false, true, false,
        false},
    {"rotation, placeholder",
        false, true, false,
        true},
    {"scaling",
        false, false, true,
        false},
    {"scaling, placeholder",
        false, false, true,
        true},
};

const struct {
    const char* name;
    bool meshPresent, meshMaterialPresent;
    /* Either all or none can be placeholders */
    bool placeholders;
} FieldsMappingSharedMeshMaterialPlaceholderData[]{
    {"both placeholders",
        true, true,
        false},
    {"no placeholders",
        true, true,
        true},
    {"just mesh present, placeholder",
        true, false,
        false},
    {"just mesh present, not a placeholder",
        true, false,
        true},
    {"just mesh material present, placeholder",
        false, true,
        false},
    {"just mesh material present, not a placeholder",
        false, true,
        true},
};

CombineTest::CombineTest() {
    addInstancedTests({&CombineTest::fields},
        Containers::arraySize(FieldsData));

    addTests({&CombineTest::fieldsStrings<UnsignedByte>,
              &CombineTest::fieldsStrings<UnsignedShort>,
              &CombineTest::fieldsStrings<UnsignedInt>,
              &CombineTest::fieldsStrings<UnsignedLong>,

              &CombineTest::fieldsAlignment,
              &CombineTest::fieldsMappingShared,
              &CombineTest::fieldsMappingSharedPartial,
              &CombineTest::fieldsMappingPlaceholderFieldPlaceholder,
              &CombineTest::fieldsMappingSharedFieldPlaceholder});

    addInstancedTests({&CombineTest::fieldsMappingSharedTRSPlaceholder},
        Containers::arraySize(FieldsMappingSharedTRSPlaceholderData));

    addInstancedTests({&CombineTest::fieldsMappingSharedMeshMaterialPlaceholder},
        Containers::arraySize(FieldsMappingSharedMeshMaterialPlaceholderData));

    addTests({&CombineTest::fieldsSharedMappingExpected,
              &CombineTest::fieldsStringPlaceholder,
              &CombineTest::fieldsOffsetOnly,
              &CombineTest::fieldsFromDataOffsetOnly});
}

using namespace Math::Literals;

void CombineTest::fields() {
    auto&& data = FieldsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Testing the four possible object types, it should be possible to combine
       them. Make them all non-contiguous to catch accidents in the internal
       casting / copying code.*/

    const struct Mesh {
        UnsignedInt mapping;
        UnsignedByte mesh;
    } meshData[]{
        {45, 3},
        {78, 5},
        {23, 17}
    };
    auto meshes = Containers::stridedArrayView(meshData);

    const struct Parent {
        UnsignedShort mapping;
        Short parent;
    } parentData[]{
        {0, -1},
        {1, 0}
    };
    auto parents = Containers::stridedArrayView(parentData);

    const struct Translation {
        UnsignedByte mapping;
        Vector2d translation;
    } translationData[]{
        {16, {1.5, -0.5}}
    };
    auto translations = Containers::stridedArrayView(translationData);

    const struct Foo {
        UnsignedLong mapping;
        Int foo[2];
    } fooData[]{
        {15, {0, 1}},
        {23, {2, 3}}
    };
    auto foos = Containers::stridedArrayView(fooData);

    const struct Bool {
        UnsignedShort mapping;
        bool bit;
    } boolData[]{
        {23, false},
        {24, true},
        {25, false},
        {26, true}
    };
    auto bools = Containers::stridedArrayView(boolData);

    const struct Bits {
        UnsignedByte mapping;
        UnsignedByte bits;
    } bitsData[]{
        {13, 1 << 3 | 1 << 4},
        {25, 1 << 4},
        {77, 1 << 1 | 1 << 2 | 1 << 3},
    };
    auto bits = Containers::stridedArrayView(bitsData);

    Trade::SceneData scene = combineFields(data.objectType, 167, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            meshes.slice(&Mesh::mapping),
            meshes.slice(&Mesh::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            parents.slice(&Parent::mapping),
            parents.slice(&Parent::parent),
            Trade::SceneFieldFlag::ImplicitMapping},
        Trade::SceneFieldData{Trade::SceneField::Translation,
            translations.slice(&Translation::mapping),
            translations.slice(&Translation::translation)},
        /* Array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            foos.slice(&Foo::mapping),
            Containers::StridedArrayView2D<const Int>{foos.slice(&Foo::foo)},
            Trade::SceneFieldFlag::OrderedMapping},
        /* Empty field */
        Trade::SceneFieldData{Trade::SceneField::Camera, Containers::ArrayView<const UnsignedByte>{}, Containers::ArrayView<const UnsignedShort>{}},
        /* Bit field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(16),
            bools.slice(&Bool::mapping),
            bools.slice(&Bool::bit).sliceBit(0),
            Trade::SceneFieldFlag::ImplicitMapping},
        /* Bit array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(17),
            bits.slice(&Bits::mapping),
            Containers::StridedBitArrayView2D{Containers::BitArrayView{bitsData}, &bitsData[0].bits, 1, {3, 4}, {sizeof(Bits)*8, 1}},
            Trade::SceneFieldFlag::OrderedMapping},
    });

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), data.objectType);
    CORRADE_COMPARE(scene.mappingBound(), 167);
    CORRADE_COMPARE(scene.fieldCount(), 7);

    CORRADE_COMPARE(scene.fieldName(0), Trade::SceneField::Mesh);
    CORRADE_COMPARE(scene.fieldFlags(0), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(0), Trade::SceneFieldType::UnsignedByte);
    CORRADE_COMPARE(scene.fieldArraySize(0), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(0), Containers::arrayView<UnsignedInt>({
        45, 78, 23
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(0),
        meshes.slice(&Mesh::mesh),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(1), Trade::SceneField::Parent);
    CORRADE_COMPARE(scene.fieldFlags(1), Trade::SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(scene.fieldType(1), Trade::SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldArraySize(1), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(1), Containers::arrayView<UnsignedInt>({
        0, 1
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Short>(1),
        parents.slice(&Parent::parent),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(2), Trade::SceneField::Translation);
    CORRADE_COMPARE(scene.fieldFlags(2), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(2), Trade::SceneFieldType::Vector2d);
    CORRADE_COMPARE(scene.fieldArraySize(2), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(2),
        Containers::arrayView<UnsignedInt>({16}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Vector2d>(2),
        translations.slice(&Translation::translation),
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
        (Containers::StridedArrayView2D<const Int>{foos.slice(&Foo::foo)})[0],
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<Int[]>(3)[1],
        (Containers::StridedArrayView2D<const Int>{foos.slice(&Foo::foo)})[1],
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(4), Trade::SceneField::Camera);
    CORRADE_COMPARE(scene.fieldFlags(4), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(4), Trade::SceneFieldType::UnsignedShort);
    CORRADE_COMPARE(scene.fieldSize(4), 0);
    CORRADE_COMPARE(scene.fieldArraySize(4), 0);

    CORRADE_COMPARE(scene.fieldName(5), Trade::sceneFieldCustom(16));
    CORRADE_COMPARE(scene.fieldFlags(5), Trade::SceneFieldFlag::ImplicitMapping);
    CORRADE_COMPARE(scene.fieldType(5), Trade::SceneFieldType::Bit);
    CORRADE_COMPARE(scene.fieldArraySize(5), 0);
    CORRADE_COMPARE_AS(scene.mappingAsArray(5),
        Containers::arrayView({23u, 24u, 25u, 26u}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldBits(5),
        Containers::stridedArrayView({false, true, false, true}).sliceBit(0),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(6), Trade::sceneFieldCustom(17));
    CORRADE_COMPARE(scene.fieldFlags(6), Trade::SceneFieldFlag::OrderedMapping);
    CORRADE_COMPARE(scene.fieldType(6), Trade::SceneFieldType::Bit);
    CORRADE_COMPARE(scene.fieldArraySize(6), 4);
    CORRADE_COMPARE_AS(scene.mappingAsArray(6),
        Containers::arrayView({13u, 25u, 77u}),
        TestSuite::Compare::Container);
    /** @todo clean up once it's possible to compare multidimensional
        containers */
    CORRADE_COMPARE_AS(scene.fieldBitArrays(6)[0],
        Containers::stridedArrayView({false, false, true, true}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldBitArrays(6)[1],
        Containers::stridedArrayView({false, false, false, true}).sliceBit(0),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldBitArrays(6)[2],
        Containers::stridedArrayView({true, true, true, false}).sliceBit(0),
        TestSuite::Compare::Container);
}

/* Taken from SceneDataTest */
template<class T> struct StringFieldTraits;
template<> struct StringFieldTraits<UnsignedByte> {
    static const char* name() { return "8"; }
    static Trade::SceneFieldType offsetType() { return Trade::SceneFieldType::StringOffset8; }
    static Trade::SceneFieldType rangeType() { return Trade::SceneFieldType::StringRange8; }
    static Trade::SceneFieldType rangeNullTerminatedType() {
        return Trade::SceneFieldType::StringRangeNullTerminated8;
    }
};
template<> struct StringFieldTraits<UnsignedShort> {
    static const char* name() { return "16"; }
    static Trade::SceneFieldType offsetType() { return Trade::SceneFieldType::StringOffset16; }
    static Trade::SceneFieldType rangeType() { return Trade::SceneFieldType::StringRange16; }
    static Trade::SceneFieldType rangeNullTerminatedType() {
        return Trade::SceneFieldType::StringRangeNullTerminated16;
    }
};
template<> struct StringFieldTraits<UnsignedInt> {
    static const char* name() { return "32"; }
    static Trade::SceneFieldType offsetType() { return Trade::SceneFieldType::StringOffset32; }
    static Trade::SceneFieldType rangeType() { return Trade::SceneFieldType::StringRange32; }
    static Trade::SceneFieldType rangeNullTerminatedType() {
        return Trade::SceneFieldType::StringRangeNullTerminated32;
    }
};
template<> struct StringFieldTraits<UnsignedLong> {
    static const char* name() { return "64"; }
    static Trade::SceneFieldType offsetType() { return Trade::SceneFieldType::StringOffset64; }
    static Trade::SceneFieldType rangeType() { return Trade::SceneFieldType::StringRange64; }
    static Trade::SceneFieldType rangeNullTerminatedType() {
        return Trade::SceneFieldType::StringRangeNullTerminated64;
    }
};

template<class T> void CombineTest::fieldsStrings() {
    setTestCaseTemplateName(StringFieldTraits<T>::name());

    /* Null-terminated ranges */
    Containers::StringView tagStrings =
        "SOFT\0"        /* 0 */
        "mouldy!"_s;    /* 5, assumes it's stored null-terminated */
    /* With null termination it's 13 bytes. If only 12 would be copied, the
       next ArrayTuple item (likely Name::mapping) would get aligned right
       after, failing the null terminator check */
    CORRADE_COMPARE(tagStrings.size(), 12);

    const struct Tag {
        UnsignedByte mapping;
        T rangeNullTerminated;
    } tagsData[]{
        {3, 0},
        {7, 5},
        {7, 0},
        {1, 0}
    };
    auto tags = Containers::stridedArrayView(tagsData);

    /* Non-null-terminated offsets */
    Containers::StringView nameStrings =
        "Chair"         /* 5 */
        "Lampshade"     /* 14 */
        "Sofa37"_s;     /* 20 */
    CORRADE_COMPARE(nameStrings.size(), 20);

    const struct Name {
        UnsignedByte mapping;
        T offset;
    } namesData[]{
        {3, 5},
        {7, 14},
        {1, 20}
    };
    auto names = Containers::stridedArrayView(namesData);

    /* Null-terminated offsets */
    Containers::StringView keyStrings =
        "color\0"       /* 6 */
        "age\0"         /* 10 */
        "age"_s;        /* 14, assumes it's stored null-terminated */

    const struct Key {
        UnsignedByte mapping;
        T offsetNullTerminated;
    } keysData[]{
        {11, 6},
        {3, 10},
        {12, 14}
    };
    auto keys = Containers::stridedArrayView(keysData);

    Containers::StringView valueStrings =
        "light\0brown"  /* 0, 11 */
        "ancient"       /* 11, 7 */
        "new"_s;        /* 18, 3 */

    /* Non-null-terminated ranges */
    const struct Value {
        UnsignedByte mapping;
        Containers::Pair<T, T> range;
    } valuesData[]{
        {3, {18, 3}},
        {12, {11, 7}},
        {7, {18, 3}},
        {11, {0, 11}}
    };
    auto values = Containers::stridedArrayView(valuesData);

    /* Using just 8-bit mapping to not have any extra padding between things
       and thus better catch accidentally forgotten null termination and
       such */
    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedByte, 167, {
        Trade::SceneFieldData{Trade::sceneFieldCustom(0),
            tags.slice(&Tag::mapping),
            tagStrings.data(), StringFieldTraits<T>::rangeNullTerminatedType(),
            tags.slice(&Tag::rangeNullTerminated)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(1),
            names.slice(&Name::mapping),
            nameStrings.data(), StringFieldTraits<T>::offsetType(),
            names.slice(&Name::offset)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(2),
            keys.slice(&Key::mapping),
            keyStrings.data(), StringFieldTraits<T>::offsetType(),
            keys.slice(&Key::offsetNullTerminated),
            Trade::SceneFieldFlag::NullTerminatedString},
        Trade::SceneFieldData{Trade::sceneFieldCustom(3),
            values.slice(&Value::mapping),
            valueStrings.data(), StringFieldTraits<T>::rangeType(),
            values.slice(&Value::range)},
        /* Empty string field, shouldn't crash or anything */
        Trade::SceneFieldData{Trade::sceneFieldCustom(4),
            Containers::ArrayView<const UnsignedByte>{},
            nullptr, StringFieldTraits<T>::offsetType(),
            Containers::ArrayView<const T>{}},
    });

    CORRADE_COMPARE(scene.fieldName(0), Trade::sceneFieldCustom(0));
    CORRADE_COMPARE(scene.fieldFlags(0), Trade::SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(scene.fieldType(0), StringFieldTraits<T>::rangeNullTerminatedType());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedByte>(0),
        tags.slice(&Tag::mapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<T>(0),
        tags.slice(&Tag::rangeNullTerminated),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldStrings(0),
        (Containers::StringIterable{"SOFT", "mouldy!", "SOFT", "SOFT"}),
        TestSuite::Compare::Container);
    /* All should stay null-terminated -- i.e., the null terminator included in
       the size calculation when the string gets copied */
    for(Containers::StringView i: scene.fieldStrings(0)) {
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }

    CORRADE_COMPARE(scene.fieldName(1), Trade::sceneFieldCustom(1));
    CORRADE_COMPARE(scene.fieldFlags(1), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(1), StringFieldTraits<T>::offsetType());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedByte>(1),
        names.slice(&Name::mapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<T>(1),
        names.slice(&Name::offset),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldStrings(1),
        (Containers::StringIterable{"Chair", "Lampshade", "Sofa37"}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(2), Trade::sceneFieldCustom(2));
    CORRADE_COMPARE(scene.fieldFlags(2), Trade::SceneFieldFlag::NullTerminatedString);
    CORRADE_COMPARE(scene.fieldType(2), StringFieldTraits<T>::offsetType());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedByte>(2),
        keys.slice(&Key::mapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.field<T>(2),
        keys.slice(&Key::offsetNullTerminated),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldStrings(2),
        (Containers::StringIterable{"color", "age", "age"}),
        TestSuite::Compare::Container);
    /* All should stay null-terminated -- i.e., the null terminator included in
       the size calculation when the string gets copied */
    for(Containers::StringView i: scene.fieldStrings(2)) {
        CORRADE_COMPARE(i.flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(i[i.size()], '\0');
    }

    CORRADE_COMPARE(scene.fieldName(3), Trade::sceneFieldCustom(3));
    CORRADE_COMPARE(scene.fieldFlags(3), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(3), StringFieldTraits<T>::rangeType());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedByte>(3),
        values.slice(&Value::mapping),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<Containers::Pair<T, T>>(3)),
        values.slice(&Value::range),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldStrings(3),
        (Containers::StringIterable{"new", "ancient", "new", "light\0brown"_s}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(scene.fieldName(4), Trade::sceneFieldCustom(4));
    CORRADE_COMPARE(scene.fieldFlags(4), Trade::SceneFieldFlags{});
    CORRADE_COMPARE(scene.fieldType(4), StringFieldTraits<T>::offsetType());
    CORRADE_COMPARE_AS(scene.mapping<UnsignedByte>(4),
        Containers::ArrayView<const UnsignedByte>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((scene.field<T>(4)),
        Containers::ArrayView<const T>{},
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(scene.fieldStrings(4),
        Containers::StringIterable{},
        TestSuite::Compare::Container);
}

void CombineTest::fieldsAlignment() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};
    const UnsignedShort translationMappingData[]{5}; /* 1 byte padding before */
    const Vector2d translationFieldData[]{{1.5, 3.0}}; /* 4 byte padding before */

    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedShort, 167, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Translation,
            Containers::arrayView(translationMappingData),
            Containers::arrayView(translationFieldData)}
    });

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

void CombineTest::fieldsMappingShared() {
    const UnsignedShort meshMappingData[3]{};
    const UnsignedByte meshFieldData[3]{};
    const Int meshMaterialFieldData[3]{};

    const UnsignedShort translationRotationMappingData[2]{};
    const Vector2 translationFieldData[2]{};
    const Complex rotationFieldData[2]{};

    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedInt, 173, {
        /* Deliberately in an arbitrary order to avoid false assumptions like
           fields sharing the same object mapping always being after each
           other */
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Translation,
            Containers::arrayView(translationRotationMappingData),
            Containers::arrayView(translationFieldData)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshMaterialFieldData)},
        Trade::SceneFieldData{Trade::SceneField::Rotation,
            Containers::arrayView(translationRotationMappingData),
            Containers::arrayView(rotationFieldData)}
    });

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

void CombineTest::fieldsMappingSharedPartial() {
    const UnsignedShort mappingData[]{15, 23, 47, 26, 3};

    /* Field data don't have any special treatment so their values aren't
       tested */
    const UnsignedByte meshData[3]{};
    const UnsignedShort lightData[2]{};
    const Int parentData[3]{};

    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedInt, 173, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(mappingData).prefix(3),
            Containers::arrayView(meshData)},
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::arrayView(mappingData).prefix(2),
            Containers::arrayView(lightData)},
        Trade::SceneFieldData{Trade::SceneField::Parent,
            Containers::stridedArrayView(mappingData).every(2),
            Containers::arrayView(parentData)},
    });

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedInt);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 3);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(Trade::SceneField::Mesh),
        Containers::arrayView({15u, 23u, 47u}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView({15u, 23u}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(scene.mapping<UnsignedInt>(Trade::SceneField::Parent),
        Containers::arrayView({15u, 47u, 3u}),
        TestSuite::Compare::Container);

    /* All mappings should be deinterleaved */
    for(UnsignedInt i = 0; i != scene.fieldCount(); ++i) {
        CORRADE_ITERATION(scene.fieldName(i));
        CORRADE_COMPARE(scene.mapping<UnsignedInt>(i).stride(), sizeof(UnsignedInt));
    }
}

void CombineTest::fieldsMappingPlaceholderFieldPlaceholder() {
    const UnsignedShort meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedShort, 173, {
        Trade::SceneFieldData{Trade::SceneField::Camera,
            Containers::ArrayView<UnsignedByte>{nullptr, 1},
            Containers::ArrayView<UnsignedShort>{nullptr, 1}},
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshFieldData)},
        /* Looks like sharing object mapping with the Camera field, but
           actually both are placeholders */
        Trade::SceneFieldData{Trade::SceneField::Light,
            Containers::ArrayView<UnsignedShort>{nullptr, 2},
            Containers::ArrayView<UnsignedInt>{nullptr, 2}},
        /* Array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(15),
            Containers::ArrayView<UnsignedShort>{nullptr, 2},
            Containers::StridedArrayView2D<Short>{{nullptr, 16}, {2, 4}}},
        /* Bit array field */
        Trade::SceneFieldData{Trade::sceneFieldCustom(16),
            Containers::ArrayView<UnsignedLong>{nullptr, 3},
            Containers::StridedBitArrayView2D{{nullptr, 1, 8}, {3, 2}}},
    });

    CORRADE_COMPARE(scene.dataFlags(), Trade::DataFlag::Owned|Trade::DataFlag::Mutable);
    CORRADE_COMPARE(scene.mappingType(), Trade::SceneMappingType::UnsignedShort);
    CORRADE_COMPARE(scene.mappingBound(), 173);
    CORRADE_COMPARE(scene.fieldCount(), 5);

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
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Light).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1);
    CORRADE_COMPARE(scene.mapping(Trade::SceneField::Light).stride()[0], 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Light).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2);
    CORRADE_COMPARE(scene.field(Trade::SceneField::Light).stride()[0], 4);

    CORRADE_COMPARE(scene.fieldType(Trade::sceneFieldCustom(15)), Trade::SceneFieldType::Short);
    CORRADE_COMPARE(scene.fieldSize(Trade::sceneFieldCustom(15)), 2);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::sceneFieldCustom(15)), 4);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(15)).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(15)).stride()[0], 2);
    CORRADE_COMPARE(scene.field(Trade::sceneFieldCustom(15)).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4 + 2*2);
    CORRADE_COMPARE(scene.field(Trade::sceneFieldCustom(15)).stride()[0], 4*2);

    CORRADE_COMPARE(scene.fieldType(Trade::sceneFieldCustom(16)), Trade::SceneFieldType::Bit);
    CORRADE_COMPARE(scene.fieldSize(Trade::sceneFieldCustom(16)), 3);
    CORRADE_COMPARE(scene.fieldArraySize(Trade::sceneFieldCustom(16)), 2);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(16)).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4 + 2*2 + 2*8);
    CORRADE_COMPARE(scene.mapping(Trade::sceneFieldCustom(16)).stride()[0], 2);
    CORRADE_COMPARE(scene.fieldBitArrays(Trade::sceneFieldCustom(16)).data(),
        scene.data() + 2 + 2 + 3*2 + 3 + 1 + 2*2 + 2 + 2*4 + 2*2 + 2*8 + 3*2);
    CORRADE_COMPARE(scene.fieldBitArrays(Trade::sceneFieldCustom(16)).offset(), 0);
    CORRADE_COMPARE(scene.fieldBitArrays(Trade::sceneFieldCustom(16)).stride()[0], 2);
}

void CombineTest::fieldsMappingSharedFieldPlaceholder() {
    const UnsignedInt meshMappingData[]{15, 23, 47};
    const UnsignedByte meshFieldData[]{0, 1, 2};

    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedInt, 173, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::arrayView(meshMappingData),
            Containers::arrayView(meshFieldData)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::arrayView(meshMappingData),
            Containers::ArrayView<Int>{nullptr, 3}},
    });

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

void CombineTest::fieldsMappingSharedTRSPlaceholder() {
    auto&& data = FieldsMappingSharedTRSPlaceholderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const UnsignedShort mapping[]{13, 31, 67};

    const Vector2 translationData[]{{1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}};
    const Complex rotationData[]{Complex::rotation(30.0_degf), Complex::rotation(60.0_degf), Complex::rotation(90.0_degf)};
    const Vector2 scalingData[]{{7.0f, -1.0f}, {8.0f, -2.0f}, {9.0f, -3.0f}};
    const UnsignedByte meshData[]{5, 7, 119};

    Containers::Array<Trade::SceneFieldData> fields;
    if(data.translationPresent) arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Translation,
        data.placeholders ?
            Containers::ArrayView<UnsignedShort>{nullptr, 3} :
            Containers::arrayView(mapping),
        Containers::arrayView(translationData));
    if(data.rotationPresent) arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Rotation,
        data.placeholders ?
            Containers::ArrayView<UnsignedShort>{nullptr, 3} :
            Containers::arrayView(mapping),
        Containers::arrayView(rotationData));

    /* Add a placeholder mapping field from another share group with the same
       pointer/size/stride to verify they don't get shared by accident; add it
       among the other fields to avoid them accidentally being treated as always
       together */
    arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Mesh,
        Containers::ArrayView<UnsignedShort>{nullptr, 3},
        Containers::arrayView(meshData));

    if(data.scalingPresent) arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Scaling,
        data.placeholders ?
            Containers::ArrayView<UnsignedShort>{nullptr, 3} :
            Containers::arrayView(mapping),
        Containers::arrayView(scalingData));

    /* The assertions inside SceneData will verify that the mapping is
       shared */
    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedInt, 173, fields);

    Containers::StridedArrayView1D<const UnsignedInt> mappingData;
    if(data.translationPresent) {
        CORRADE_VERIFY(scene.hasField(Trade::SceneField::Translation));
        CORRADE_COMPARE_AS(scene.field<Vector2>(Trade::SceneField::Translation),
            Containers::arrayView(translationData),
            TestSuite::Compare::Container);
        mappingData = scene.mapping<UnsignedInt>(Trade::SceneField::Translation);
    }
    if(data.rotationPresent) {
        CORRADE_VERIFY(scene.hasField(Trade::SceneField::Rotation));
        CORRADE_COMPARE_AS(scene.field<Complex>(Trade::SceneField::Rotation),
            Containers::arrayView(rotationData),
            TestSuite::Compare::Container);
        mappingData = scene.mapping<UnsignedInt>(Trade::SceneField::Rotation);
    }
    if(data.scalingPresent) {
        CORRADE_VERIFY(scene.hasField(Trade::SceneField::Scaling));
        CORRADE_COMPARE_AS(scene.field<Vector2>(Trade::SceneField::Scaling),
            Containers::arrayView(scalingData),
            TestSuite::Compare::Container);
        mappingData = scene.mapping<UnsignedInt>(Trade::SceneField::Scaling);
    }

    if(!data.placeholders)
        CORRADE_COMPARE_AS(mappingData,
            Containers::arrayView({13u, 31u, 67u}),
            TestSuite::Compare::Container);

    /* The other field should be copied as well, but with its own mapping
       data */
    CORRADE_VERIFY(scene.hasField(Trade::SceneField::Mesh));
    CORRADE_VERIFY(scene.mapping(Trade::SceneField::Mesh).data() != mappingData.data());
    CORRADE_COMPARE_AS(scene.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView(meshData),
        TestSuite::Compare::Container);
}

void CombineTest::fieldsMappingSharedMeshMaterialPlaceholder() {
    auto&& data = FieldsMappingSharedMeshMaterialPlaceholderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    const UnsignedShort mapping[]{13, 31, 67};

    const UnsignedByte meshData[]{5, 7, 119};
    const Int meshMaterialData[]{25, -1, 23};

    const Vector2 translationData[]{{1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f}};

    Containers::Array<Trade::SceneFieldData> fields;
    if(data.meshPresent) arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Mesh,
        data.placeholders ?
            Containers::ArrayView<UnsignedShort>{nullptr, 3} :
            Containers::arrayView(mapping),
        Containers::arrayView(meshData));

    /* Add a placeholder mapping field from another share group to verify
       they're not shared by accident; add it among the other fields to avoid
       them accidentally being treated as always together */
    arrayAppend(fields, InPlaceInit,
        Trade::SceneField::Translation,
        Containers::ArrayView<UnsignedShort>{nullptr, 3},
        Containers::arrayView(translationData));

    if(data.meshMaterialPresent) arrayAppend(fields, InPlaceInit,
        Trade::SceneField::MeshMaterial,
        data.placeholders ?
            Containers::ArrayView<UnsignedShort>{nullptr, 3} :
            Containers::arrayView(mapping),
        Containers::arrayView(meshMaterialData));

    /* The assertions inside SceneData will verify that the mapping is
       shared */
    Trade::SceneData scene = combineFields(Trade::SceneMappingType::UnsignedInt, 173, fields);

    Containers::StridedArrayView1D<const UnsignedInt> mappingData;
    if(data.meshPresent) {
        CORRADE_VERIFY(scene.hasField(Trade::SceneField::Mesh));
        CORRADE_COMPARE_AS(scene.field<UnsignedByte>(Trade::SceneField::Mesh),
            Containers::arrayView(meshData),
            TestSuite::Compare::Container);
        mappingData = scene.mapping<UnsignedInt>(Trade::SceneField::Mesh);
    }
    if(data.meshMaterialPresent) {
        CORRADE_VERIFY(scene.hasField(Trade::SceneField::MeshMaterial));
        CORRADE_COMPARE_AS(scene.field<Int>(Trade::SceneField::MeshMaterial),
            Containers::arrayView(meshMaterialData),
            TestSuite::Compare::Container);
        mappingData = scene.mapping<UnsignedInt>(Trade::SceneField::MeshMaterial);
    }

    if(!data.placeholders)
        CORRADE_COMPARE_AS(mappingData,
            Containers::arrayView({13u, 31u, 67u}),
            TestSuite::Compare::Container);

    /* The other field should be copied as well, but with its own mapping
       data */
    CORRADE_VERIFY(scene.hasField(Trade::SceneField::Translation));
    CORRADE_VERIFY(scene.mapping(Trade::SceneField::Translation).data() != mappingData.data());
    CORRADE_COMPARE_AS(scene.field<Vector2>(Trade::SceneField::Translation),
        Containers::arrayView(translationData),
        TestSuite::Compare::Container);
}

void CombineTest::fieldsSharedMappingExpected() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Tested thoroughly in SceneDataTest::constructMismatchedTRSViews() and
       constructMismatchedMeshMaterialViews(), here it uses the same internal
       utility so test just that it's correctly called */

    UnsignedInt meshes[3]{};
    Int materials[3]{};

    Containers::String out;
    Error redirectError{&out};
    combineFields(Trade::SceneMappingType::UnsignedInt, 3, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::ArrayView<UnsignedInt>{reinterpret_cast<UnsignedInt*>(0xdead), 3},
            Containers::arrayView(meshes)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::ArrayView<UnsignedInt>{reinterpret_cast<UnsignedInt*>(0xbeef), 2},
            Containers::arrayView(materials).prefix(2)},
    });
    combineFields(Trade::SceneMappingType::UnsignedInt, 3, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            Containers::ArrayView<UnsignedInt>{reinterpret_cast<UnsignedInt*>(0xdead), 3},
            Containers::arrayView(meshes)},
        Trade::SceneFieldData{Trade::SceneField::MeshMaterial,
            Containers::ArrayView<UnsignedInt>{nullptr, 3},
            Containers::arrayView(materials)},
    });
    CORRADE_COMPARE(out,
        "SceneTools::combineFields(): Trade::SceneField::MeshMaterial mapping data {0xbeef, 2, 4} is different from Trade::SceneField::Mesh mapping data {0xdead, 3, 4}\n"
        /* Placeholder mapping is also disallowed right now -- it has to be
           either all placeholders or none */
        "SceneTools::combineFields(): Trade::SceneField::MeshMaterial mapping data {0x0, 3, 4} is different from Trade::SceneField::Mesh mapping data {0xdead, 3, 4}\n");
}

void CombineTest::fieldsStringPlaceholder() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct Data {
        UnsignedByte mapping;
        UnsignedByte mesh;
    } data[3]{};
    auto view = Containers::stridedArrayView(data);

    Containers::String out;
    Error redirectError{&out};
    /* A null string data pointer could work in this case (because it doesn't
       need to be accessed), but disallowing it always for consistency */
    combineFields(Trade::SceneMappingType::UnsignedByte, 167, {
        /* Just to verify it prints correct field IDs */
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            view.slice(&Data::mapping),
            view.slice(&Data::mesh)},
        Trade::SceneFieldData{Trade::sceneFieldCustom(16),
            view.slice(&Data::mapping),
            nullptr, Trade::SceneFieldType::StringOffset8,
            /* Have to fake a pointer because in some cases (ARM64 Linux) the
               distance between nullptr and stack-allocated memory (such as
               `data`) *may* be larger than what can fit into 48 bits,
               triggering an assert */
            Containers::arrayView(reinterpret_cast<UnsignedByte*>(0xfece5), 3)},
    });
    /* With placeholder field data it's impossible to know the actual string
       size */
    combineFields(Trade::SceneMappingType::UnsignedByte, 167, {
        Trade::SceneFieldData{Trade::sceneFieldCustom(16),
            view.slice(&Data::mapping),
            /* Have to fake a pointer because in some cases (ARM64 Linux) the
               distance between nullptr and stack-allocated memory (such as
               `data`) *may* be larger than what can fit into 48 bits,
               triggering an assert */
            reinterpret_cast<char*>(0xfece5), Trade::SceneFieldType::StringRangeNullTerminated16,
            Containers::StridedArrayView1D<const UnsignedShort>{{nullptr, 6}, 3}},
    });
    CORRADE_COMPARE(out,
        "SceneTools::combineFields(): string field 1 has a placeholder string data\n"
        "SceneTools::combineFields(): string field 0 has a placeholder data\n");
}

void CombineTest::fieldsOffsetOnly() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const struct Field {
        UnsignedInt mapping;
        UnsignedByte mesh;
        UnsignedShort light;
    } data[]{
        {1, 3, 5},
        {4, 6, 8},
    };
    auto view = Containers::stridedArrayView(data);

    Containers::String out;
    Error redirectError{&out};
    combineFields(Trade::SceneMappingType::UnsignedInt, 173, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            view.slice(&Field::mapping),
            view.slice(&Field::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light, 2,
            Trade::SceneMappingType::UnsignedInt, offsetof(Field, mapping), sizeof(Field),
            Trade::SceneFieldType::UnsignedShort, offsetof(Field, light), sizeof(Field)}
    });
    CORRADE_COMPARE(out, "SceneTools::combineFields(): field 1 is offset-only\n");
}

void CombineTest::fieldsFromDataOffsetOnly() {
    /* Same as fieldFromData(), but wrapped in a Scene first, which makes it
       work */

    const struct Field {
        UnsignedInt mapping;
        UnsignedByte mesh;
        UnsignedShort light;
    } data[]{
        {1, 3, 5},
        {4, 6, 8},
    };
    auto view = Containers::stridedArrayView(data);

    Trade::SceneData scene{Trade::SceneMappingType::UnsignedInt, 22, {}, data, {
        Trade::SceneFieldData{Trade::SceneField::Mesh,
            view.slice(&Field::mapping),
            view.slice(&Field::mesh)},
        Trade::SceneFieldData{Trade::SceneField::Light, 2,
            Trade::SceneMappingType::UnsignedInt, offsetof(Field, mapping), sizeof(Field),
            Trade::SceneFieldType::UnsignedShort, offsetof(Field, light), sizeof(Field)}
    }};

    Trade::SceneData combined = combineFields(scene);
    /* Since it's tightly packed, it's less data now */
    CORRADE_COMPARE(combined.data().size(), 2*4 + 2*1 + 2*2);
    CORRADE_COMPARE_AS(combined.data().size(), sizeof(data),
        TestSuite::Compare::Less);

    /* The two mappings are shared */
    CORRADE_COMPARE_AS(combined.mapping<UnsignedInt>(Trade::SceneField::Mesh),
        Containers::arrayView({1u, 4u}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(combined.mapping<UnsignedInt>(Trade::SceneField::Light),
        Containers::arrayView({1u, 4u}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(combined.mapping(Trade::SceneField::Light).data(), combined.mapping(Trade::SceneField::Mesh).data());

    CORRADE_COMPARE_AS(combined.field<UnsignedByte>(Trade::SceneField::Mesh),
        Containers::arrayView<UnsignedByte>({3, 6}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(combined.field<UnsignedShort>(Trade::SceneField::Light),
        Containers::arrayView<UnsignedShort>({5, 8}),
        TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneTools::Test::CombineTest)
