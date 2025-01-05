#ifndef Magnum_SceneTools_Implementation_convertToSingleFunctionObjects_h
#define Magnum_SceneTools_Implementation_convertToSingleFunctionObjects_h
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

#include <unordered_map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/SceneTools/Implementation/combine.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Implementation {

inline Containers::Optional<std::size_t> findField(Containers::ArrayView<const Trade::SceneField> fields, Trade::SceneField field) {
    for(std::size_t i = 0; i != fields.size(); ++i)
        if(fields[i] == field) return i;
    return {};
}

/* Creates a SceneData copy where each object has at most one of the fields
   listed in the passed `fieldsToConvert` array. This is done by enlarging the
   parents array and moving extraneous features to new objects that are marked
   as a child of the original. Fields that are connected together (such as
   meshes and materials) are assumed to share the same object mapping with only
   one of them passed in the fieldsToConvert array, which will result for all
   fields from the same set being reassociated to the new object.

   Fields listed in `fieldsToCopy` are copied from the original object. This
   is useful for e.g. skins, to preserve them for the separated meshes.

   Requies a SceneField::Parent to be present -- otherwise it wouldn't be
   possible to know where to attach the new objects. */
/** @todo when published, (again) add an initializer_list overload and turn all
    internal asserts into (tested!) message asserts */
/** @todo it also needs a better name ("make fields unique"?) */
inline Trade::SceneData convertToSingleFunctionObjects(const Trade::SceneData& scene, Containers::ArrayView<const Trade::SceneField> fieldsToConvert, Containers::ArrayView<const Trade::SceneField> fieldsToCopy, const UnsignedInt newObjectOffset) {
    /** @todo assert for really high object counts (where this cast would fail) */
    Containers::Array<UnsignedInt> objectAttachmentCount{ValueInit, std::size_t(scene.mappingBound())};
    for(const Trade::SceneField field: fieldsToConvert) {
        CORRADE_INTERNAL_ASSERT(field != Trade::SceneField::Parent);

        /* Skip fields that are not present -- is it's not present, then it
           definitely won't be responsible for multi-function objects */
        const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
        if(!fieldId) continue;

        /** @todo use a statically-allocated array & Into() in a loop instead
            once this is more than a private backwards-compatibility utility
            where PERF WHATEVER WHO CARES */
        for(const UnsignedInt object: scene.mappingAsArray(*fieldId)) {
            CORRADE_INTERNAL_ASSERT(object < objectAttachmentCount.size());
            ++objectAttachmentCount[object];
        }
    }

    /* entriesToAddToFieldsToCopy[i] specifies how many fields to add for the
       fieldsToCopy[i] field */
    Containers::Array<UnsignedInt> fieldsToCopyAdditionCount{ValueInit, fieldsToCopy.size()};
    for(std::size_t i = 0; i != fieldsToCopy.size(); ++i) {
        const Trade::SceneField field = fieldsToCopy[i];
        CORRADE_INTERNAL_ASSERT(field != Trade::SceneField::Parent);
        CORRADE_INTERNAL_ASSERT(!findField(fieldsToConvert, field));

        /* Skip fields that are not present */
        const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
        if(!fieldId) continue;

        /** @todo use a statically-allocated array & Into() in a loop instead
            once this is more than a private backwards-compatibility utility
            where PERF WHATEVER WHO CARES */
        for(const UnsignedInt object: scene.mappingAsArray(*fieldId)) {
            CORRADE_INTERNAL_ASSERT(object < objectAttachmentCount.size());
            if(objectAttachmentCount[object])
                fieldsToCopyAdditionCount[i] += objectAttachmentCount[object] - 1;
        }
    }

    UnsignedInt objectsToAdd = 0;
    for(const UnsignedInt count: objectAttachmentCount)
        if(count > 1) objectsToAdd += count - 1;

    /* Ensure we don't overflow the 32-bit object count with the objects to
       add. This should also cover the case when the parent field would not be
       representable in 32 bits. */
    CORRADE_INTERNAL_ASSERT(newObjectOffset + objectsToAdd >= newObjectOffset);

    /* Copy the fields over, enlarging them as necessary */
    const UnsignedInt parentFieldId = scene.fieldId(Trade::SceneField::Parent);
    Containers::Array<Trade::SceneFieldData> fields{scene.fieldCount()};
    for(std::size_t i = 0; i != scene.fieldCount(); ++i) {
        const Trade::SceneFieldData& field = scene.fieldData(i);

        /* If this field is among the fields we want to copy, enlarge it for
           the new entries */
        if(Containers::Optional<std::size_t> fieldToCopy = findField(fieldsToCopy, field.name())) {
            /** @todo wow this placeholder construction is HIDEOUS */
            fields[i] = Trade::SceneFieldData{field.name(),
                field.mappingType(),
                Containers::ArrayView<const UnsignedInt>{nullptr, field.size() + fieldsToCopyAdditionCount[*fieldToCopy]},
                field.fieldType(),
                Containers::StridedArrayView1D<const void>{
                    {nullptr, ~std::size_t{}},
                    field.size() + fieldsToCopyAdditionCount[*fieldToCopy],
                    std::ptrdiff_t((field.fieldArraySize() ? field.fieldArraySize() : 1)*sceneFieldTypeSize(field.fieldType()))
                },
                field.fieldArraySize(),
                field.flags() & ~Trade::SceneFieldFlag::ImplicitMapping
            };

        /* If this is a parent, enlarge it for the newly added objects, and if
           it was implicit make it ordered */
        } else if(field.name() == Trade::SceneField::Parent) {
            /** @todo some nicer constructor for placeholders once this is in
                public interest */
            fields[i] = Trade::SceneFieldData{Trade::SceneField::Parent, Containers::ArrayView<const UnsignedInt>{nullptr, field.size() + objectsToAdd}, Containers::ArrayView<const Int>{nullptr, field.size() + objectsToAdd},
                /* If the field is ordered, we preserve that. But if it's
                   implicit, we can't. */
                field.flags() & ~(Trade::SceneFieldFlag::ImplicitMapping & ~Trade::SceneFieldFlag::OrderedMapping)
            };

        /* All other fields are copied as-is, but lose the implicit/ordered
           flags */
        /** @todo the flags could get preserved for
            -   fields that don't share their object mapping with any fields in
                the fieldsToConvert list
            -   fields that don't actually get their object mapping touched
                during the process (and then all fields that share object
                mapping with them) */
        } else fields[i] = Trade::SceneFieldData{field.name(), field.mappingType(), field.mappingData(), field.fieldType(), field.fieldData(), field.fieldArraySize(), field.flags() & ~Trade::SceneFieldFlag::ImplicitMapping};
    }

    /* Combine the fields into a new SceneData */
    Trade::SceneData out = combineFields(Trade::SceneMappingType::UnsignedInt, Math::max(scene.mappingBound(), UnsignedLong(newObjectOffset) + objectsToAdd), fields);

    /* Copy existing parent object/field data to a prefix of the output */
    const Containers::StridedArrayView1D<UnsignedInt> outParentMapping = out.mutableMapping<UnsignedInt>(parentFieldId);
    const Containers::StridedArrayView1D<Int> outParents = out.mutableField<Int>(parentFieldId);
    CORRADE_INTERNAL_ASSERT_OUTPUT(scene.parentsInto(0, outParentMapping, outParents) == scene.fieldSize(parentFieldId));

    /* Copy existing field-to-copy data to a prefix of the output */
    for(std::size_t i = 0; i != fieldsToCopy.size(); ++i) {
        const Trade::SceneField field = fieldsToCopy[i];

        const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
        if(!fieldId) continue;

        const Containers::StridedArrayView1D<UnsignedInt> outMapping = out.mutableMapping<UnsignedInt>(*fieldId);
        const Containers::StridedArrayView2D<char> outField = out.mutableField(*fieldId);
        CORRADE_INTERNAL_ASSERT_OUTPUT(scene.mappingInto(*fieldId, 0, outMapping) == scene.fieldSize(*fieldId));
        Utility::copy(scene.field(*fieldId), outField.prefix(scene.fieldSize(*fieldId)));
    }

    /* List new objects at the end of the extended parent field */
    const Containers::StridedArrayView1D<UnsignedInt> newParentMapping = outParentMapping.exceptPrefix(scene.fieldSize(parentFieldId));
    const Containers::StridedArrayView1D<Int> newParents = outParents.exceptPrefix(scene.fieldSize(parentFieldId));
    for(std::size_t i = 0; i != newParentMapping.size(); ++i) {
        newParentMapping[i] = newObjectOffset + i;
        newParents[i] = -1;
    }

    /* Clear the objectAttachmentCount array to reuse it below */
    /** @todo use a BitArray instead once it exists? */
    constexpr UnsignedInt zero[1]{};
    Utility::copy(Containers::stridedArrayView(zero).broadcasted<0>(scene.mappingBound()), objectAttachmentCount);

    /* Clear the fieldsToCopyAdditionCount array to reuse it below */
    Utility::copy(Containers::stridedArrayView(zero).broadcasted<0>(fieldsToCopy.size()), fieldsToCopyAdditionCount);

    /* For objects with multiple fields move the extra fields to newly added
       children */
    {
        std::size_t newParentIndex = 0;
        for(const Trade::SceneField field: fieldsToConvert) {
            const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
            if(!fieldId) continue;

            for(UnsignedInt& fieldObject: out.mutableMapping<UnsignedInt>(*fieldId)) {
                /* If the object is not new (could happen when an object
                   mapping array is shared among multiple fields, in which case
                   it *might* have been updated already to an ID larger than
                   the mapping array size) and it already has something
                   attached, then attach the field to a new object and make
                   that new object a child of the previous one. */
                if(fieldObject < objectAttachmentCount.size() && objectAttachmentCount[fieldObject]) {
                    /* Go through all fields to copy and copy each entry that
                       was assigned to the original object */
                    for(std::size_t i = 0; i != fieldsToCopy.size(); ++i) {
                        const Containers::Optional<UnsignedInt> fieldToCopyId = scene.findFieldId(fieldsToCopy[i]);
                        if(!fieldToCopyId) continue;

                        /* View to copy the data from */
                        const Containers::StridedArrayView2D<const char> fieldToCopyDataSrc = scene.field(*fieldToCopyId);

                        /* Views to put the mapping to and copy the data to */
                        const std::size_t newFieldToCopyOffset = scene.fieldSize(*fieldToCopyId);
                        const Containers::StridedArrayView1D<UnsignedInt> newFieldToCopyMapping = out.mutableMapping<UnsignedInt>(*fieldToCopyId).exceptPrefix(newFieldToCopyOffset);
                        const Containers::StridedArrayView2D<char> newFieldToCopy = out.mutableField(*fieldToCopyId).exceptPrefix(newFieldToCopyOffset);

                        /* As long as there are entries attached to the
                           original objects, copy them */
                        std::size_t offset = 0;
                        while(Containers::Optional<std::size_t> found = scene.findFieldObjectOffset(*fieldToCopyId, fieldObject, offset)) {
                            /* Assgn a new field entry to the new object */
                            newFieldToCopyMapping[fieldsToCopyAdditionCount[i]] = newParentMapping[newParentIndex];

                            /* Copy the data from the old entry to it */
                            Utility::copy(fieldToCopyDataSrc[*found], newFieldToCopy[fieldsToCopyAdditionCount[i]]);

                            ++fieldsToCopyAdditionCount[i];
                            offset = *found + 1;
                        }
                    }

                    /* Use the old object as a parent of the new object */
                    newParents[newParentIndex] = fieldObject;
                    /* Assign the field to the new object */
                    fieldObject = newParentMapping[newParentIndex];
                    /* Move to the next reserved object */
                    ++newParentIndex;

                    /** @todo mark this field as touched here and the restore
                        the original flags for all fields that didn't have
                        their object mapping touched */

                } else ++objectAttachmentCount[fieldObject];
            }
        }

        CORRADE_INTERNAL_ASSERT(newParentIndex == objectsToAdd);
    }

    return out;
}

}}}

#endif
