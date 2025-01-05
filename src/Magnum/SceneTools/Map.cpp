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

#include "Map.h"

#include <Corrade/Containers/Optional.h>

#include "Magnum/Math/PackingBatch.h"
#include "Magnum/SceneTools/Combine.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Trade::SceneData mapIndexField(const Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::mapIndexField(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields",
        (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    const Trade::SceneFieldType fieldType = scene.fieldType(fieldId);
    Containers::Array<Trade::SceneFieldData> fields{NoInit, scene.fieldCount()};
    for(std::size_t i = 0; i != scene.fieldCount(); ++i) {
        if(i == fieldId) {
            CORRADE_ASSERT(scene.fieldArraySize(i) == 0,
                "SceneTools::mapIndexField(): array field mapping isn't supported",
                (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

            Trade::SceneFieldType outputFieldType;
            if(fieldType == Trade::SceneFieldType::UnsignedInt ||
               fieldType == Trade::SceneFieldType::UnsignedShort ||
               fieldType == Trade::SceneFieldType::UnsignedByte)
                outputFieldType = Trade::SceneFieldType::UnsignedInt;
            else if(fieldType == Trade::SceneFieldType::Int ||
                    fieldType == Trade::SceneFieldType::Short ||
                    fieldType == Trade::SceneFieldType::Byte)
                outputFieldType = Trade::SceneFieldType::Int;
            else CORRADE_ASSERT_UNREACHABLE("SceneTools::mapIndexField(): unsupported field type" << fieldType,
                (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

            fields[i] = Trade::SceneFieldData{scene.fieldName(i),
                scene.mapping(i), outputFieldType,
                Containers::StridedArrayView2D<const char>{{nullptr, ~std::size_t{}}, {scene.fieldSize(i), 4}},
                /* We aren't removing any field entries from the scene nor
                   modifying the mapping in any way, so the flags can be passed
                   through in full */
                scene.fieldFlags(i)}; // TODO test flags

        /* Otherwise grab the field in full. This will also convert offset-only
           fields to absolute. */
        } else fields[i] = scene.fieldData(i);
    }

    /* Create a new SceneData out of the unpacked index field and all others,
       unpack its data into the placeholder location */
    Trade::SceneData unpacked = combineFields(scene.mappingType(), scene.mappingBound(), fields);
    if(fieldType == Trade::SceneFieldType::UnsignedInt)
        Math::castInto(scene.field<UnsignedInt>(fieldId),
                       unpacked.mutableField<UnsignedInt>(fieldId));
    else if(fieldType == Trade::SceneFieldType::UnsignedShort)
        Math::castInto(scene.field<UnsignedShort>(fieldId),
                       unpacked.mutableField<UnsignedInt>(fieldId));
    else if(fieldType == Trade::SceneFieldType::UnsignedByte)
        Math::castInto(scene.field<UnsignedByte>(fieldId),
                       unpacked.mutableField<UnsignedInt>(fieldId));
    else if(fieldType == Trade::SceneFieldType::Int)
        Math::castInto(scene.field<Int>(fieldId),
                       unpacked.mutableField<Int>(fieldId));
    else if(fieldType == Trade::SceneFieldType::Short)
        Math::castInto(scene.field<Short>(fieldId),
                       unpacked.mutableField<Int>(fieldId));
    else if(fieldType == Trade::SceneFieldType::Byte)
        Math::castInto(scene.field<Byte>(fieldId),
                       unpacked.mutableField<Int>(fieldId));
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    mapIndexFieldInPlace(unpacked, fieldId, mapping);
    return unpacked;
}

Trade::SceneData mapIndexField(const Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::mapIndexField(): field" << field << "not found",
        (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    return mapIndexField(scene, *fieldId, mapping);
}

Trade::SceneData mapIndexField(Trade::SceneData&& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::mapIndexField(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields",
        (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    /* Perform the operation in-place, if we can transfewr the ownership and
       have the field in the target format already. There's currently no way to
       create a SceneData that's Owned but not Mutable so check for Owned is
       enough. */
    if(scene.dataFlags() & Trade::DataFlag::Owned &&
       (scene.fieldType(fieldId) == Trade::SceneFieldType::UnsignedInt ||
        scene.fieldType(fieldId) == Trade::SceneFieldType::Int))
    {
        mapIndexFieldInPlace(scene, fieldId, mapping);
        return Utility::move(scene);
    }

    /* Otherwise delegate to the function that does all the copying and format
       expansion */
    return mapIndexField(scene, fieldId, mapping);
}

Trade::SceneData mapIndexField(Trade::SceneData&& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::mapIndexField(): field" << field << "not found",
        (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    // TODO test that it gets actually moved
    return mapIndexField(Utility::move(scene), *fieldId, mapping);
}

namespace {

template<class T> void mapImplementation(const Containers::StridedArrayView1D<T>& field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    for(T& i: field) {
        CORRADE_ASSERT(i < mapping.size(),
            "SceneTools::mapIndexFieldInPlace(): index" << i << "out of range for" << mapping.size() << "mapping values", );
        CORRADE_ASSERT(mapping[i] < (1ull << sizeof(T)*8),
            "SceneTools::mapIndexFieldInPlace(): mapping value" << mapping[i] << "not representable in" << Trade::Implementation::SceneFieldTypeFor<T>::type(), );
        i = mapping[i];
    }
}

template<class T> void mapSignedImplementation(const Containers::StridedArrayView1D<T>& field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    for(T& i: field) {
        if(i == T(-1))
            continue;

        CORRADE_ASSERT(UnsignedInt(i) < mapping.size(),
            "SceneTools::mapIndexFieldInPlace(): index" << i << "out of range for" << mapping.size() << "mapping values", );
        CORRADE_ASSERT(mapping[i] >= 0 && mapping[i] < (1u << (sizeof(T)*8 - 1)),
            "SceneTools::mapIndexFieldInPlace(): mapping value" << mapping[i] << "not representable in" << Trade::Implementation::SceneFieldTypeFor<T>::type(), );
        i = T(mapping[i]);
    }
}

}

void mapIndexFieldInPlace(Trade::SceneData& scene, const UnsignedInt fieldId, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    CORRADE_ASSERT(fieldId < scene.fieldCount(),
        "SceneTools::mapIndexFieldInPlace(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", );
    CORRADE_ASSERT(scene.dataFlags() & Trade::DataFlag::Mutable,
        "SceneTools::mapIndexFieldInPlace(): data not mutable", );
    CORRADE_ASSERT(scene.fieldArraySize(fieldId) == 0,
        "SceneTools::mapIndexFieldInPlace(): array field mapping isn't supported", );

    const Trade::SceneFieldType fieldType = scene.fieldType(fieldId);
    if(fieldType == Trade::SceneFieldType::UnsignedInt)
        mapImplementation(scene.mutableField<UnsignedInt>(fieldId), mapping);
    else if(fieldType == Trade::SceneFieldType::UnsignedShort)
        mapImplementation(scene.mutableField<UnsignedShort>(fieldId), mapping);
    else if(fieldType == Trade::SceneFieldType::UnsignedByte)
        mapImplementation(scene.mutableField<UnsignedByte>(fieldId), mapping);
    else if(fieldType == Trade::SceneFieldType::Int)
        mapSignedImplementation(scene.mutableField<Int>(fieldId), mapping);
    else if(fieldType == Trade::SceneFieldType::Short)
        mapSignedImplementation(scene.mutableField<Short>(fieldId), mapping);
    else if(fieldType == Trade::SceneFieldType::Byte)
        mapSignedImplementation(scene.mutableField<Byte>(fieldId), mapping);
    else CORRADE_ASSERT_UNREACHABLE("SceneTools::mapIndexFieldInPlace(): unsupported field type" << fieldType, );
}

void mapIndexFieldInPlace(Trade::SceneData& scene, const Trade::SceneField field, const Containers::StridedArrayView1D<const UnsignedInt>& mapping) {
    const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
    CORRADE_ASSERT(fieldId,
        "SceneTools::mapIndexFieldInPlace(): field" << field << "not found", );

    return mapIndexFieldInPlace(scene, *fieldId, mapping);
}

}}
