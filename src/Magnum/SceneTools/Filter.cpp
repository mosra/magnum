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

#include "Filter.h"

#include <map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/BitAlgorithms.h>

#include "Magnum/SceneTools/Combine.h"
#include "Magnum/SceneTools/Copy.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools {

Trade::SceneData filterFields(Trade::SceneData&& scene, const Containers::BitArrayView fieldsToKeep) {
    CORRADE_ASSERT(fieldsToKeep.size() == scene.fieldCount(),
        "SceneTools::filterFields(): expected" << scene.fieldCount() << "bits but got" << fieldsToKeep.size(), (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    /* Copy fields that aren't filtered away. Not using NoInit in order to use
       the default deleter and have this usable from plugins. */
    Containers::Array<Trade::SceneFieldData> filtered{ValueInit, fieldsToKeep.count()};
    Utility::copyMasked(scene.fieldData(), fieldsToKeep, filtered);

    if(scene.dataFlags() >= Trade::DataFlag::Owned)
        return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
            scene.releaseData(), Utility::move(filtered)};
    else
        return Trade::SceneData{scene.mappingType(), scene.mappingBound(),
            {}, scene.data(), Utility::move(filtered)};
}

Trade::SceneData filterFields(const Trade::SceneData& scene, const Containers::BitArrayView fieldsToKeep) {
    return filterFields(reference(scene), fieldsToKeep);
}

Trade::SceneData filterOnlyFields(Trade::SceneData&& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    Containers::BitArray fieldsToKeep{DirectInit, scene.fieldCount(), false};
    for(const Trade::SceneField field: fields) {
        if(const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field))
            fieldsToKeep.set(*fieldId);
    }
    return filterFields(Utility::move(scene), fieldsToKeep);
}

Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    return filterOnlyFields(reference(scene), fields);
}

Trade::SceneData filterOnlyFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterOnlyFields(scene, Containers::arrayView(fields));
}

Trade::SceneData filterOnlyFields(Trade::SceneData&& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterOnlyFields(Utility::move(scene), Containers::arrayView(fields));
}

Trade::SceneData filterExceptFields(Trade::SceneData&& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    Containers::BitArray fieldsToKeep{DirectInit, scene.fieldCount(), true};
    for(const Trade::SceneField field: fields) {
        if(const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(field))
            fieldsToKeep.reset(*fieldId);
    }
    return filterFields(Utility::move(scene), fieldsToKeep);
}

Trade::SceneData filterExceptFields(const Trade::SceneData& scene, const Containers::ArrayView<const Trade::SceneField> fields) {
    return filterExceptFields(reference(scene), fields);
}

Trade::SceneData filterExceptFields(const Trade::SceneData& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterExceptFields(scene, Containers::arrayView(fields));
}

Trade::SceneData filterExceptFields(Trade::SceneData&& scene, std::initializer_list<Trade::SceneField> fields) {
    return filterExceptFields(Utility::move(scene), Containers::arrayView(fields));
}

Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, const Containers::ArrayView<const Containers::Pair<UnsignedInt, Containers::BitArrayView>> entriesToKeep) {
    /* Track unique mapping views (pointer, size, stride) so fields that shared
       a mapping before stay shared after as well -- if they're filtered, they
       will have the mapping allocated in SharedMapping::filteredMapping()
       instead of just a null placeholder when passing the filtered fields to
       combineFields(), which will ensure they stay shared. If they're not
       filtered, the original field view will get passed through, which ensures
       the same. This also conveniently handles all cases of enforced mapping
       such as for TRS fields so we don't need to special-case that here again. */
    struct SharedMapping {
        /* How many times given mapping is shared */
        UnsignedInt count = 1;
        /* How many times given mapping is filtered. Should be either 0 or same
           as `count`. */
        UnsignedInt filteredCount = 0;
        #ifndef CORRADE_NO_ASSERT
        /* Index in `entriesToKeep` that contains the filtering mask. All other
           entries should use the same view (same pointer, offset and size). */
        UnsignedInt maskIndex = ~UnsignedInt{};
        #endif
        /* Data array allocated for this mapping, in order to have
           combineFields() preserve their sharing in the output. Doesn't
           contain any actual data, it's used just to have a unique
           (pointer, size, stride) combination. */
        /** @todo any idea how to do this without the throwaway allocations? */
        Containers::Array<char> filteredMapping;
    };
    /* A map<tuple> is used because it has conveniently implemented ordering,
       an unordered_map couldn't be used without manually implementing a
       std::tuple hash because STL DOES NOT HAVE IT, UGH. */
    std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, SharedMapping> uniqueMappings;
    for(UnsignedInt i = 0; i != scene.fieldCount(); ++i) {
        /* Skip empty fields as those make no sense to include for sharing */
        if(!scene.fieldSize(i))
            continue;

        const Containers::StridedArrayView2D<const char> mapping = scene.mapping(i);
        const std::pair<std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, SharedMapping>::iterator, bool> inserted = uniqueMappings.emplace(std::make_tuple(mapping.data(), mapping.size()[0], mapping.stride()[0]), SharedMapping{});
        if(!inserted.second)
            ++inserted.first->second.count;
    }

    /* Copy all field metadata. By default, if the field isn't referenced, it's
       kept in full. Can't use Utility::copy() on the whole fieldData() array
       as those can be offset-only. */
    Containers::Array<Trade::SceneFieldData> fields{ValueInit, scene.fieldCount()};
    for(std::size_t i = 0; i != scene.fieldCount(); ++i)
        fields[i] = scene.fieldData(i);

    const std::size_t mappingTypeSize = Trade::sceneMappingTypeSize(scene.mappingType());

    /* For fields that are being filtered update the field size and turn it
       into a placeholder */
    #ifndef CORRADE_NO_ASSERT
    Containers::BitArray usedFields{ValueInit, scene.fieldCount()};
    #endif
    for(std::size_t i = 0; i != entriesToKeep.size(); ++i) {
        const UnsignedInt fieldId = entriesToKeep[i].first();
        const Containers::BitArrayView mask = entriesToKeep[i].second();

        CORRADE_ASSERT(fieldId < scene.fieldCount(),
            "SceneTools::filterFieldEntries(): index" << fieldId << "out of range for" << scene.fieldCount() << "fields", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
        CORRADE_ASSERT(!usedFields[fieldId],
            "SceneTools::filterFieldEntries(): field" << scene.fieldName(fieldId) << "listed more than once", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
        #ifndef CORRADE_NO_ASSERT
        usedFields.set(fieldId);
        #endif
        CORRADE_ASSERT(scene.fieldSize(fieldId) == mask.size(),
            "SceneTools::filterFieldEntries(): expected" << scene.fieldSize(fieldId) << "bits for" << scene.fieldName(fieldId) << "but got" << mask.size(),
            (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

        const Trade::SceneFieldType fieldType = scene.fieldType(fieldId);
        CORRADE_ASSERT(!Trade::Implementation::isSceneFieldTypeString(fieldType),
            "SceneTools::filterFieldEntries(): filtering string fields is not implemented yet, sorry", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
        CORRADE_ASSERT(fieldType != Trade::SceneFieldType::Bit,
            "SceneTools::filterFieldEntries(): filtering bit fields is not implemented yet, sorry", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

        /* Skip empty fields as there's nothing to do for them and they don't
           even have an entry in the uniqueMappings map. But do that only after
           doing all checks for them for consistent behavior. */
        if(!scene.fieldSize(fieldId))
            continue;

        const Containers::StridedArrayView2D<const char> mapping = scene.mapping(fieldId);
        SharedMapping& sharedMapping = uniqueMappings.at(std::make_tuple(mapping.data(), mapping.size()[0], mapping.stride()[0]));

        /* If the mapping is shared, pass a pre-allocated array with the final
           contents to combineFields() to keep the sharing */
        const std::size_t filteredFieldSize = mask.count();
        Containers::StridedArrayView1D<const void> filteredMapping;
        if(sharedMapping.count > 1) {
            /* This is the first mask that filters a shared mapping, allocate
               the output for it and copy the filtered mapping there */
            if(!sharedMapping.filteredCount) {
                sharedMapping.filteredMapping = Containers::Array<char>{NoInit, mappingTypeSize*filteredFieldSize};
                Utility::copyMasked(scene.mapping(fieldId), mask, Containers::StridedArrayView2D<char>{sharedMapping.filteredMapping, {filteredFieldSize, mappingTypeSize}});
                #ifndef CORRADE_NO_ASSERT
                sharedMapping.maskIndex = i;
                #endif
            }
            #ifndef CORRADE_NO_ASSERT
            /* Otherwise check that all shared fields use the same filter
               view */
            else {
                #ifndef CORRADE_STANDARD_ASSERT
                const UnsignedInt originalFieldId = entriesToKeep[sharedMapping.maskIndex].first();
                #endif
                const Containers::BitArrayView originalMask = entriesToKeep[sharedMapping.maskIndex].second();
                CORRADE_ASSERT(
                    originalMask.data() == mask.data() &&
                    originalMask.offset() == mask.offset() &&
                    originalMask.size() == mask.size(),
                    "SceneTools::filterFieldEntries(): field" << scene.fieldName(fieldId) << "shares mapping with" << scene.fieldName(originalFieldId) << "but was passed a different mask view",
                    (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
            }
            #endif

            filteredMapping = {sharedMapping.filteredMapping, filteredFieldSize, std::ptrdiff_t(mappingTypeSize)};
        } else {
            CORRADE_INTERNAL_ASSERT(sharedMapping.count == 1);
            filteredMapping = {{nullptr, mappingTypeSize*filteredFieldSize}, filteredFieldSize, std::ptrdiff_t(mappingTypeSize)};
        }

        /* Preserve flags, but if the field was marked as having implicit
           mapping before, item removal causes it to be only ordered now */
        Trade::SceneFieldFlags fieldFlags = scene.fieldFlags(fieldId);
        if(fieldFlags >= Trade::SceneFieldFlag::ImplicitMapping)
            fieldFlags &= (~Trade::SceneFieldFlag::ImplicitMapping)|Trade::SceneFieldFlag::OrderedMapping;

        const std::size_t fieldTypeSize = Trade::sceneFieldTypeSize(fieldType);
        fields[fieldId] = Trade::SceneFieldData{scene.fieldName(fieldId),
            scene.mappingType(), filteredMapping,
            fieldType, Containers::StridedArrayView1D<const void>{{nullptr, fieldTypeSize*filteredFieldSize}, filteredFieldSize, std::ptrdiff_t(fieldTypeSize)}, scene.fieldArraySize(fieldId), fieldFlags};

        ++sharedMapping.filteredCount;
    }

    #ifndef CORRADE_NO_ASSERT
    for(const std::pair<const std::tuple<const void*, std::size_t, std::ptrdiff_t>, SharedMapping>& i: uniqueMappings) {
        CORRADE_ASSERT(!i.second.filteredCount || i.second.count == i.second.filteredCount,
            "SceneTools::filterFieldEntries(): field" << scene.fieldName(entriesToKeep[i.second.maskIndex].first()) << "shares mapping with" << i.second.count << "fields but only" << i.second.filteredCount << "are filtered",
            (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
    }
    #endif

    Trade::SceneData out = combineFields(scene.mappingType(), scene.mappingBound(), fields);

    for(const Containers::Pair<UnsignedInt, Containers::BitArrayView>& i: entriesToKeep) {
        /* Skip empty fields as there's nothing to do for them and they don't
           even have an entry in the uniqueMappings map */
        if(!scene.fieldSize(i.first()))
            continue;

        /* Copy the mapping only if it isn't shared among more fields -- in
           that case it got already copied above */
        const Containers::StridedArrayView2D<const char> mapping = scene.mapping(i.first());
        if(uniqueMappings.at(std::make_tuple(mapping.data(), mapping.size()[0], mapping.stride()[0])).count == 1)
            Utility::copyMasked(mapping, i.second(), out.mutableMapping(i.first()));

        Utility::copyMasked(scene.field(i.first()), i.second(), out.mutableField(i.first()));
    }

    return out;
}

Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, const std::initializer_list<Containers::Pair<UnsignedInt, Containers::BitArrayView>> entriesToKeep) {
    return filterFieldEntries(scene, Containers::arrayView(entriesToKeep));
}

Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, const Containers::ArrayView<const Containers::Pair<Trade::SceneField, Containers::BitArrayView>> entriesToKeep) {
    Containers::Array<Containers::Pair<UnsignedInt, Containers::BitArrayView>> out{NoInit, entriesToKeep.size()};
    for(std::size_t i = 0; i != entriesToKeep.size(); ++i) {
        const Containers::Optional<UnsignedInt> fieldId = scene.findFieldId(entriesToKeep[i].first());
        CORRADE_ASSERT(fieldId,
            "SceneTools::filterFieldEntries(): field" << entriesToKeep[i].first() << "not found", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
        out[i] = {*fieldId, entriesToKeep[i].second()};
    }

    return filterFieldEntries(scene, out);
}

Trade::SceneData filterFieldEntries(const Trade::SceneData& scene, const std::initializer_list<Containers::Pair<Trade::SceneField, Containers::BitArrayView>> entriesToKeep) {
    return filterFieldEntries(scene, Containers::arrayView(entriesToKeep));
}

namespace {

template<class T> std::size_t filterObjectsImplementation(const Trade::SceneData& scene, const Containers::ArrayView<Containers::Pair<UnsignedInt, Containers::BitArrayView>> fieldStorage, const Containers::MutableBitArrayView maskStorage, const Containers::BitArrayView objects, std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, Containers::Optional<UnsignedInt>>& uniqueMappings) {
    std::size_t fieldOffset = 0;
    std::size_t maskOffset = 0;
    for(UnsignedInt fieldId = 0; fieldId != scene.fieldCount(); ++fieldId) {
        /* Skip empty fields as there's nothing to do for them and they don't
           even have an entry in the uniqueMappings map */
        if(!scene.fieldSize(fieldId))
            continue;

        const Containers::StridedArrayView1D<const T> mapping = scene.mapping<T>(fieldId);

        /* Shared mappings need to stay shared, thus filterFieldEntries() needs
           to get the exact same mask for such fields -- for implementation
           simplicity not just the bit values but the actual view */
        Containers::Optional<UnsignedInt>& sharedMapping = uniqueMappings.at(std::make_tuple(mapping.data(), mapping.size(), mapping.stride()));

        /* If a mask was already calculated for this mapping, reuse the view */
        if(sharedMapping) {
            /* If the field wasn't filtered in any way, it wasn't added to the
               list, which is indicated by ~UnsignedInt{}. Do nothing in that
               case. */
            if(*sharedMapping != ~UnsignedInt{})
                fieldStorage[fieldOffset++] = {fieldId, fieldStorage[*sharedMapping].second()};

        /* If not, calculate the mask and remember it for potential other
           fields that share the same mapping view */
        } else {
            const Containers::MutableBitArrayView mask = maskStorage.sliceSize(maskOffset, mapping.size());

            bool anyFiltered = false;
            for(std::size_t i = 0; i != mapping.size(); ++i) {
                /** @todo ugh! mask.set(i, objects[mapping[i]]) and then .all()
                    once it's implemented (needs BMI variants similarly to
                    count()) */
                if(objects[mapping[i]])
                    mask.set(i);
                else {
                    anyFiltered = true;
                    mask.reset(i);
                }
            }

            /* Only add the field to the list if it's not all 1s */
            if(anyFiltered) {
                sharedMapping = fieldOffset;
                fieldStorage[fieldOffset++] = {fieldId, mask};
                /* Not bothering with rounding this to whole bytes as
                   Utility::copyMasked() has to special-case the begin/end
                   anyway */
                maskOffset += mask.size();
            } else {
                sharedMapping = ~UnsignedInt{};
            }
        }
    }

    CORRADE_INTERNAL_ASSERT(fieldOffset <= fieldStorage.size());
    CORRADE_INTERNAL_ASSERT(maskOffset <= maskStorage.size());

    return fieldOffset;
}

}

Trade::SceneData filterObjects(const Trade::SceneData& scene, const Containers::BitArrayView objects) {
    CORRADE_ASSERT(objects.size() == scene.mappingBound(),
        "SceneTools::filterObjects(): expected" << scene.mappingBound() << "bits but got" << objects.size(), (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

    /** @todo while a BitArrayView is certainly faster for lookup than an
        unordered list of IDs, it might become rather problematic in cases
        where the mapping bound is sparse and *really huge* (i.e., storing
        pointers) -- then there either needs to be an overload that takes an
        `ArrayView<const UnsignedLong>` and does some less ideal lookup, or a
        `packObjects()` tool that makes the object numbering contiguous for
        this API to be usable, storing also mapping back to the original ID in
        the scene, and an `unpackObjects()` that restores the original IDs */

    /* Count the total count of bits possibly needed */
    std::size_t bitCount = 0;
    for(UnsignedInt i = 0; i != scene.fieldCount(); ++i)
        bitCount += scene.fieldSize(i);

    /* Allocate scratch memory for all the bits and field references */
    Containers::ArrayView<Containers::Pair<UnsignedInt, Containers::BitArrayView>> fieldStorage;
    Containers::MutableBitArrayView maskStorage;
    Containers::ArrayTuple storage{
        {NoInit, scene.fieldCount(), fieldStorage},
        {NoInit, bitCount, maskStorage}
    };

    /* Collect a map of unique mappings. The value is a placeholder where
       filterObjectsImplementation() will subsequently record a reference to a
       BitArrayView that should be used for all fields. */
    std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, Containers::Optional<UnsignedInt>> uniqueMappings;
    for(UnsignedInt i = 0; i != scene.fieldCount(); ++i) {
        /* Skip empty fields as those make no sense to include for sharing */
        if(!scene.fieldSize(i))
            continue;

        const Containers::StridedArrayView2D<const char> mapping = scene.mapping(i);
        uniqueMappings.emplace(std::make_tuple(mapping.data(), mapping.size()[0], mapping.stride()[0]), Containers::NullOpt);
    }

    /* Delegate to a concrete filtering implementation based on used mapping
       type. Returns the prefix of fieldStorage that got filled, with fields
       that didn't need to be changed omitted. */
    std::size_t fieldCount = ~std::size_t{};
    switch(scene.mappingType()) {
        case Trade::SceneMappingType::UnsignedByte:
            fieldCount = filterObjectsImplementation<UnsignedByte>(scene, fieldStorage, maskStorage, objects, uniqueMappings);
            break;
        case Trade::SceneMappingType::UnsignedShort:
            fieldCount = filterObjectsImplementation<UnsignedShort>(scene, fieldStorage, maskStorage, objects, uniqueMappings);
            break;
        case Trade::SceneMappingType::UnsignedInt:
            fieldCount = filterObjectsImplementation<UnsignedInt>(scene, fieldStorage, maskStorage, objects, uniqueMappings);
            break;
        case Trade::SceneMappingType::UnsignedLong:
            fieldCount = filterObjectsImplementation<UnsignedLong>(scene, fieldStorage, maskStorage, objects, uniqueMappings);
            break;
    }
    CORRADE_INTERNAL_ASSERT(fieldCount != ~std::size_t{});

    /* Delegate the rest to the low-level field entry filtering API */
    return filterFieldEntries(scene, fieldStorage.prefix(fieldCount));
}

}}
