#ifndef Magnum_Trade_Implementation_sceneTools_h
#define Magnum_Trade_Implementation_sceneTools_h
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

#include <unordered_map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace Trade { namespace Implementation {

/* These two are needed because there (obviously) isn't any overload of
   castInto with the same input and output type */
template<class T, class U> void copyOrCastInto(const Containers::StridedArrayView1D<const T>& src, const Containers::StridedArrayView1D<U>& dst) {
    Math::castInto(Containers::arrayCast<2, const T>(src), Containers::arrayCast<2, U>(dst));
}
template<class T> void copyOrCastInto(const Containers::StridedArrayView1D<const T>& src, const Containers::StridedArrayView1D<T>& dst) {
    Utility::copy(src, dst);
}

template<class T> void sceneCombineCopyObjects(const Containers::ArrayView<const SceneFieldData> fields, const Containers::ArrayView<const Containers::StridedArrayView2D<char>> itemViews, const Containers::ArrayView<const Containers::Pair<UnsignedInt, UnsignedInt>> itemViewMappings) {
    std::size_t latestMapping = 0;
    for(std::size_t i = 0; i != fields.size(); ++i) {
        /* If there are no aliased object mappings, itemViewMappings should be
           monotonically increasing. If it's not, it means the mapping is
           shared with something earlier and it got already copied -- skip. */
        const std::size_t mapping = itemViewMappings[i].first();
        if(i && mapping <= latestMapping) continue;
        latestMapping = mapping;

        /* If the field has null object data, no need to copy anything. This
           covers reserved fields but also fields of zero size. */
        if(!fields[i].mappingData()) continue;

        const Containers::StridedArrayView1D<const void> src = fields[i].mappingData();
        const Containers::StridedArrayView1D<T> dst = Containers::arrayCast<1, T>(itemViews[mapping]);
        if(fields[i].mappingType() == SceneMappingType::UnsignedByte)
            copyOrCastInto(Containers::arrayCast<const UnsignedByte>(src), dst);
        else if(fields[i].mappingType() == SceneMappingType::UnsignedShort)
            copyOrCastInto(Containers::arrayCast<const UnsignedShort>(src), dst);
        else if(fields[i].mappingType() == SceneMappingType::UnsignedInt)
            copyOrCastInto(Containers::arrayCast<const UnsignedInt>(src), dst);
        else if(fields[i].mappingType() == SceneMappingType::UnsignedLong)
            copyOrCastInto(Containers::arrayCast<const UnsignedLong>(src), dst);
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

/* Combine fields of varying mapping type together into a SceneData of a single
   given mappingType. The fields are expected to point to existing
   mapping/field memory, which will be then copied to the resulting scene. If
   you supply a field with null mapping or field data, the mapping or field
   data will not get copied, only a placeholder for copying the data later will
   be allocated. If you however need to have placeholder mapping data shared
   among multiple fields you have to allocate them upfront. Offset-only fields
   are not allowed.

   The resulting fields are always tightly packed (not interleaved).

   If multiple fields share the same object mapping views, those are preserved,
   however they have to have the exact same length. Sharing object mappings
   with different lengths will assert. */
/** @todo when published, add an initializer_list overload and turn all
    internal asserts into (tested!) message asserts */
inline SceneData sceneCombine(const SceneMappingType mappingType, const UnsignedLong mappingBound, const Containers::ArrayView<const SceneFieldData> fields) {
    const std::size_t mappingTypeSize = sceneMappingTypeSize(mappingType);
    const std::size_t mappingTypeAlignment = sceneMappingTypeAlignment(mappingType);

    /* Go through all fields and collect ArrayTuple allocations for these */
    std::unordered_map<const void*, UnsignedInt> objectMappings;
    Containers::Array<Containers::ArrayTuple::Item> items;
    Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> itemViewMappings{NoInit, fields.size()};

    /* The item views are referenced from ArrayTuple::Item, not using a
       growable array in order to avoid an accidental reallocation */
    /** @todo once never-reallocating allocators are present, use them instead
        of the manual offset */
    Containers::Array<Containers::StridedArrayView2D<char>> itemViews{fields.size()*2};
    std::size_t itemViewOffset = 0;

    for(std::size_t i = 0; i != fields.size(); ++i) {
        const SceneFieldData& field = fields[i];
        CORRADE_INTERNAL_ASSERT(!(field.flags() & SceneFieldFlag::OffsetOnly));

        /* Mapping data. Allocate if the view is a placeholder of if it wasn't
           used by other fields yet. */
        std::pair<std::unordered_map<const void*, UnsignedInt>::iterator, bool> inserted;
        if(field.mappingData().data())
            inserted = objectMappings.emplace(field.mappingData().data(), itemViewOffset);
        if(field.mappingData().data() && !inserted.second) {
            itemViewMappings[i].first() = inserted.first->second;
            /* Expect that fields sharing the same object mapping view have the
               exact same length (the length gets stored in the output view
               during the ArrayTuple::Item construction).

               We could just ignore the sharing in that case, but that'd only
               lead to misery down the line -- imagine a field that shares the
               first two items with a mesh and a material object mapping. If it
               would be the last, it gets duplicated and everything is great,
               however if it's the first then both mesh and the material get
               duplicated, and that then asserts inside the SceneData
               constructor, as those are always expected to share.

               One option that would solve this would be to store pointer+size
               in the objectMappings map (and then only mappings that share
               also the same size would be shared), another would be to use the
               longest used view (and then the shorter prefixes would share
               with it). The ultimate option would be to have some range map
               where it'd be possible to locate also arbitrary subranges, not
               just prefixes. A whole other topic altogether is checking for
               the same stride, which is not done at all.

               This might theoretically lead to assertions also when two
               compile-time arrays share a common prefix and get deduplicated
               by the compiler. But that's unlikely, at least for the internal
               use case we have right now. */
            CORRADE_INTERNAL_ASSERT(itemViews[inserted.first->second].size()[0] == field.size());
        } else {
            itemViewMappings[i].first() = itemViewOffset;
            arrayAppend(items, InPlaceInit, NoInit, std::size_t(field.size()), mappingTypeSize, mappingTypeAlignment, itemViews[itemViewOffset]);
            ++itemViewOffset;
        }

        /* Field data. No aliasing here right now, no sharing between object
           and field data either. */
        /** @todo field aliasing might be useful at some point */
        itemViewMappings[i].second() = itemViewOffset;
        arrayAppend(items, InPlaceInit, NoInit, std::size_t(field.size()), sceneFieldTypeSize(field.fieldType())*(field.fieldArraySize() ? field.fieldArraySize() : 1), sceneFieldTypeAlignment(field.fieldType()), itemViews[itemViewOffset]);
        ++itemViewOffset;
    }

    /* Allocate the data */
    Containers::Array<char> outData = Containers::ArrayTuple{items};
    CORRADE_INTERNAL_ASSERT(!outData.deleter());

    /* Copy the object data over and cast them as necessary */
    if(mappingType == SceneMappingType::UnsignedByte)
        sceneCombineCopyObjects<UnsignedByte>(fields, itemViews, itemViewMappings);
    else if(mappingType == SceneMappingType::UnsignedShort)
        sceneCombineCopyObjects<UnsignedShort>(fields, itemViews, itemViewMappings);
    else if(mappingType == SceneMappingType::UnsignedInt)
        sceneCombineCopyObjects<UnsignedInt>(fields, itemViews, itemViewMappings);
    else if(mappingType == SceneMappingType::UnsignedLong)
        sceneCombineCopyObjects<UnsignedLong>(fields, itemViews, itemViewMappings);

    /* Copy the field data over. No special handling needed here. */
    for(std::size_t i = 0; i != fields.size(); ++i) {
        /* If the field has null field data, no need to copy anything. This
           covers reserved fields but also fields of zero size. */
        if(!fields[i].fieldData()) continue;

        /** @todo isn't there some less awful way to create a 2D view, sigh */
        Utility::copy(Containers::arrayCast<2, const char>(fields[i].fieldData(), sceneFieldTypeSize(fields[i].fieldType())*(fields[i].fieldArraySize() ? fields[i].fieldArraySize() : 1)), itemViews[itemViewMappings[i].second()]);
    }

    /* Map the fields to the new data */
    Containers::Array<SceneFieldData> outFields{fields.size()};
    for(std::size_t i = 0; i != fields.size(); ++i) {
        outFields[i] = SceneFieldData{fields[i].name(), itemViews[itemViewMappings[i].first()], fields[i].fieldType(), itemViews[itemViewMappings[i].second()], fields[i].fieldArraySize(), fields[i].flags()};
    }

    return SceneData{mappingType, mappingBound, std::move(outData), std::move(outFields)};
}

inline Containers::Optional<std::size_t> findField(Containers::ArrayView<const SceneField> fields, SceneField field) {
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
inline SceneData sceneConvertToSingleFunctionObjects(const SceneData& scene, Containers::ArrayView<const SceneField> fieldsToConvert, Containers::ArrayView<const SceneField> fieldsToCopy, const UnsignedInt newObjectOffset) {
    /** @todo assert for really high object counts (where this cast would fail) */
    Containers::Array<UnsignedInt> objectAttachmentCount{ValueInit, std::size_t(scene.mappingBound())};
    for(const SceneField field: fieldsToConvert) {
        CORRADE_INTERNAL_ASSERT(field != SceneField::Parent);

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
        const SceneField field = fieldsToCopy[i];
        CORRADE_INTERNAL_ASSERT(field != SceneField::Parent);
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
    const UnsignedInt parentFieldId = scene.fieldId(SceneField::Parent);
    Containers::Array<SceneFieldData> fields{scene.fieldCount()};
    for(std::size_t i = 0; i != scene.fieldCount(); ++i) {
        const SceneFieldData& field = scene.fieldData(i);

        /* If this field is among the fields we want to copy, enlarge it for
           the new entries */
        if(Containers::Optional<std::size_t> fieldToCopy = findField(fieldsToCopy, field.name())) {
            /** @todo wow this placeholder construction is HIDEOUS */
            fields[i] = SceneFieldData{field.name(),
                field.mappingType(),
                Containers::ArrayView<const UnsignedInt>{nullptr, std::size_t(field.size() + fieldsToCopyAdditionCount[*fieldToCopy])},
                field.fieldType(),
                Containers::StridedArrayView1D<const void>{
                    {nullptr, ~std::size_t{}},
                    std::size_t(field.size() + fieldsToCopyAdditionCount[*fieldToCopy]),
                    std::ptrdiff_t((field.fieldArraySize() ? field.fieldArraySize() : 1)*sceneFieldTypeSize(field.fieldType()))
                },
                field.fieldArraySize(),
                field.flags() & ~SceneFieldFlag::ImplicitMapping
            };

        /* If this is a parent, enlarge it for the newly added objects, and if
           it was implicit make it ordered */
        } else if(field.name() == SceneField::Parent) {
            /** @todo some nicer constructor for placeholders once this is in
                public interest */
            fields[i] = SceneFieldData{SceneField::Parent, Containers::ArrayView<const UnsignedInt>{nullptr, std::size_t(field.size() + objectsToAdd)}, Containers::ArrayView<const Int>{nullptr, std::size_t(field.size() + objectsToAdd)},
                /* If the field is ordered, we preserve that. But if it's
                   implicit, we can't. */
                field.flags() & ~(SceneFieldFlag::ImplicitMapping & ~SceneFieldFlag::OrderedMapping)
            };

        /* All other fields are copied as-is, but lose the implicit/ordered
           flags */
        /** @todo the flags could get preserved for
            -   fields that don't share their object mapping with any fields in
                the fieldsToConvert list
            -   fields that don't actually get their object mapping touched
                during the process (and then all fields that share object
                mapping with them) */
        } else fields[i] = SceneFieldData{field.name(), field.mappingType(), field.mappingData(), field.fieldType(), field.fieldData(), field.fieldArraySize(), field.flags() & ~SceneFieldFlag::ImplicitMapping};
    }

    /* Combine the fields into a new SceneData */
    SceneData out = sceneCombine(SceneMappingType::UnsignedInt, Math::max(scene.mappingBound(), UnsignedLong(newObjectOffset) + objectsToAdd), fields);

    /* Copy existing parent object/field data to a prefix of the output */
    const Containers::StridedArrayView1D<UnsignedInt> outParentMapping = out.mutableMapping<UnsignedInt>(parentFieldId);
    const Containers::StridedArrayView1D<Int> outParents = out.mutableField<Int>(parentFieldId);
    CORRADE_INTERNAL_ASSERT_OUTPUT(scene.parentsInto(0, outParentMapping, outParents) == scene.fieldSize(parentFieldId));

    /* Copy existing field-to-copy data to a prefix of the output */
    for(std::size_t i = 0; i != fieldsToCopy.size(); ++i) {
        const SceneField field = fieldsToCopy[i];

        const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field);
        if(!fieldId) continue;

        const Containers::StridedArrayView1D<UnsignedInt> outMapping = out.mutableMapping<UnsignedInt>(*fieldId);
        const Containers::StridedArrayView2D<char> outField = out.mutableField(*fieldId);
        CORRADE_INTERNAL_ASSERT_OUTPUT(scene.mappingInto(*fieldId, 0, outMapping) == scene.fieldSize(*fieldId));
        Utility::copy(scene.field(*fieldId), outField.prefix(scene.fieldSize(*fieldId)));
    }

    /* List new objects at the end of the extended parent field */
    const Containers::StridedArrayView1D<UnsignedInt> newParentMapping = outParentMapping.suffix(scene.fieldSize(parentFieldId));
    const Containers::StridedArrayView1D<Int> newParents = outParents.suffix(scene.fieldSize(parentFieldId));
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
        for(const SceneField field: fieldsToConvert) {
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
                        const Containers::StridedArrayView1D<UnsignedInt> newFieldToCopyMapping = out.mutableMapping<UnsignedInt>(*fieldToCopyId).suffix(newFieldToCopyOffset);
                        const Containers::StridedArrayView2D<char> newFieldToCopy = out.mutableField(*fieldToCopyId).suffix(newFieldToCopyOffset);

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
