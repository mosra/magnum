#ifndef Magnum_SceneTools_Implementation_combine_h
#define Magnum_SceneTools_Implementation_combine_h
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

#include <map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/SceneData.h"
#include "Magnum/Trade/Implementation/checkSharedSceneFieldMapping.h"

namespace Magnum { namespace SceneTools { namespace Implementation {

/* The combineFields() is currently transitively used also by Trade for
   (deprecated) backwards compatibility in SceneData, in particular by
   convertToSingleFunctionObjects(). Making Trade depend on SceneTools in a
   deprecated build would be a nasty complication, so the functions are inlined
   in a header that gets included in both */
/** @todo move everything to Combine.cpp and anonymous namespace once that
    compatibility is dropped */

union CombineItemView {
    explicit CombineItemView() {}

    Containers::StridedArrayView2D<char> types;
    Containers::MutableStridedBitArrayView2D bits;
    Containers::MutableStringView strings;
};

template<class T> void combineCopyMappings(const Containers::ArrayView<const Trade::SceneFieldData> fields, const Containers::ArrayView<const CombineItemView> itemViews, const Containers::ArrayView<const Containers::Triple<UnsignedInt, UnsignedInt, UnsignedInt>> itemViewMappings) {
    std::size_t latestMapping = 0;
    for(std::size_t i = 0; i != fields.size(); ++i) {
        /* If there are no shared object mappings, itemViewMappings should be
           monotonically increasing. If it's not, it means the mapping is
           shared with something earlier which got already copied -- skip. */
        const std::size_t mapping = itemViewMappings[i].first();
        if(i && mapping <= latestMapping) continue;
        latestMapping = mapping;

        /* If the field has null object data, no need to copy anything. This
           covers reserved fields but also fields of zero size. */
        if(!fields[i].mappingData()) continue;

        /* The additional cast to 2D has to be there in order to ensure the
           second dimension is contiguous which Math::castInto() requires */
        /** @todo this is an error-prone mess, fix better */
        const Containers::StridedArrayView1D<const void> src = fields[i].mappingData();
        const Containers::StridedArrayView2D<T> dst = Containers::arrayCast<2, T>(Containers::arrayCast<1, T>(itemViews[mapping].types));
        if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedByte)
            Math::castInto(Containers::arrayCast<2, const UnsignedByte>(Containers::arrayCast<const UnsignedByte>(src)), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedShort)
            Math::castInto(Containers::arrayCast<2, const UnsignedShort>(Containers::arrayCast<const UnsignedShort>(src)), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedInt)
            Math::castInto(Containers::arrayCast<2, const UnsignedInt>(Containers::arrayCast<const UnsignedInt>(src)), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedLong)
            Math::castInto(Containers::arrayCast<2, const UnsignedLong>(Containers::arrayCast<const UnsignedLong>(src)), dst);
        else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

/* Offsets have the total string size as the last item. If it's null-terminated
   the size is included in the offset, so no special handling needed. */
template<class T> std::size_t stringOffsetFieldSize(const Containers::StridedArrayView1D<const void>& field) {
    return Containers::arrayCast<const T>(field).back();
}
/* Ranges have the total string size as the max "end" of all offset+size
   pairs. Again, the null terminator is included in the size so no special
   handling needed. */
template<class T> std::size_t stringRangeFieldSize(const Containers::StridedArrayView1D<const void>& field) {
    std::size_t max = 0;
    for(const Containers::Pair<T, T> i: Containers::arrayCast<const Containers::Pair<T, T>>(field))
        max = Math::max(std::size_t(i.first() + i.second()), max);
    return max;
}
/* Null-terminated ranges have the size implicitly calculated using strlen,
   returning + 1 as it needs to include the last null terminator as well. */
template<class T> std::size_t stringRangeNullTerminatedFieldSize(const char* string, const Containers::StridedArrayView1D<const void>& field) {
    std::size_t max = 0;
    for(const T i: Containers::arrayCast<const T>(field))
        max = i + Math::max(std::strlen(string + i), max);
    return max + 1;
}

inline Trade::SceneData combineFields(const Trade::SceneMappingType mappingType, const UnsignedLong mappingBound, const Containers::ArrayView<const Trade::SceneFieldData> fields) {
    #ifndef CORRADE_NO_ASSERT
    /* Offset-only fields are not allowed as there's no data to refer them to.
       This has to be checked before shared scene field mapping, otherwise it'd
       assert there first, leading to confusion. */
    for(std::size_t i = 0; i != fields.size(); ++i) {
        CORRADE_ASSERT(!(fields[i].flags() & Trade::SceneFieldFlag::OffsetOnly),
            "SceneTools::combineFields(): field" << i << "is offset-only", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));
    }
    #endif

    /* Find fields that have to share the mapping views */
    const Trade::Implementation::SharedSceneFieldIds sharedSceneFieldIds = Trade::Implementation::findSharedSceneFields(fields);

    /* Check that they actually share the same object mapping, i.e. the same
       begin, size and stride. As offset-only fields are disallowed, the data
       pointer can be whatever, just needs to be large enough. */
    #ifndef CORRADE_NO_ASSERT
    if(!checkSharedSceneFieldMapping("SceneTools::combineFields():", sharedSceneFieldIds, {nullptr, ~std::size_t{}}, fields))
        return Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}};
    #endif

    Containers::Array<Containers::ArrayTuple::Item> items;
    Containers::Array<Containers::Triple<UnsignedInt, UnsignedInt, UnsignedInt>> itemViewMappings{NoInit, fields.size()};

    /* The item views are referenced from ArrayTuple::Item. It's either of the
       three views in the union --- from the group of (up to) 3 views per
       field, first is for the mapping (unless shared with another view) and is
       always `types`, second for the data (either `types` or `bits`) and third
       for the string data (`strings`, if the field is a string). In most cases
       they array won't be fully used but we need to avoid accidental
       reallocation so the array is made with an upper bound on size. */
    /** @todo once never-reallocating allocators are present, use them instead
        of the manual offset */
    Containers::Array<CombineItemView> itemViews{fields.size()*3};
    std::size_t itemViewOffset = 0;

    const std::size_t mappingTypeSize = sceneMappingTypeSize(mappingType);
    const std::size_t mappingTypeAlignment = sceneMappingTypeAlignment(mappingType);

    /* If any share group has a placeholder view (which thanks to the above
       check implies that all present fields in that group), add a mapping view
       for it -- it'll get picked up below */
    Containers::Optional<UnsignedInt> sharedTrsMapping;
    if(sharedSceneFieldIds.trs[0] != ~UnsignedInt{} && !fields[sharedSceneFieldIds.trs[0]].mappingData().data()) {
        sharedTrsMapping = itemViewOffset;
        arrayAppend(items, InPlaceInit,
            NoInit,
            fields[sharedSceneFieldIds.trs[0]].size(),
            mappingTypeSize,
            mappingTypeAlignment,
            itemViews[itemViewOffset].types);
        ++itemViewOffset;
    }
    Containers::Optional<UnsignedInt> sharedMeshMaterialMapping;
    if(sharedSceneFieldIds.meshMaterial[0] != ~UnsignedInt{} && !fields[sharedSceneFieldIds.meshMaterial[0]].mappingData().data()) {
        sharedMeshMaterialMapping = itemViewOffset;
        arrayAppend(items, InPlaceInit,
            NoInit,
            fields[sharedSceneFieldIds.meshMaterial[0]].size(),
            mappingTypeSize,
            mappingTypeAlignment,
            itemViews[itemViewOffset].types);
        ++itemViewOffset;
    }

    /* Track unique mapping views (pointer, size, stride) so fields that shared
       a mapping before stay shared after as well. A map<tuple> is used because
       it has conveniently implemented ordering, an unordered_map couldn't be
       used without manually implementing a std::tuple hash because STL DOES
       NOT HAVE IT, UGH. */
    std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, UnsignedInt> uniqueMappings;

    /* Go through all fields and collect ArrayTuple allocations for these */
    for(std::size_t i = 0; i != fields.size(); ++i) {
        const Trade::SceneFieldData& field = fields[i];

        /* Mapping data. If the view isn't a placeholder, check if it is
           shared with an existing view already, and insert it if not. */
        std::pair<std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, UnsignedInt>::iterator, bool> inserted;
        if(field.mappingData().data())
            inserted = uniqueMappings.emplace(std::make_tuple(field.mappingData().data(), field.mappingData().size(), field.mappingData().stride()), itemViewOffset);

        /* If it's shared (inserting failed), remember the field ID it's shared
           with. We don't need the original size or stride for anything after
           -- it was just used to find matching views, and if a match was
           found, it already has a correct size, and the stride is implicit. */
        if(field.mappingData().data() && !inserted.second) {
            itemViewMappings[i].first() = inserted.first->second;

        /* If it's a placeholder in one of the required-to-be-shared groups,
           add a view that was preallocated above */
        } else if(!field.mappingData().data() &&
            (field.name() == Trade::SceneField::Translation ||
             field.name() == Trade::SceneField::Rotation ||
             field.name() == Trade::SceneField::Scaling)) {
            itemViewMappings[i].first() = *sharedTrsMapping;
        } else if(!field.mappingData().data() &&
            (field.name() == Trade::SceneField::Mesh ||
             field.name() == Trade::SceneField::MeshMaterial)) {
            itemViewMappings[i].first() = *sharedMeshMaterialMapping;

        /* If it's not shared or it's a placeholder, allocate a new mapping
           view of given size by adding a new item to the list of views to
           allocate by an ArrayTuple. */
        } else {
            itemViewMappings[i].first() = itemViewOffset;
            arrayAppend(items, InPlaceInit,
                NoInit,
                field.size(),
                mappingTypeSize,
                mappingTypeAlignment,
                itemViews[itemViewOffset].types);
            ++itemViewOffset;
        }

        /* Field data, just allocate space for it. No extra logic needed -- no
           aliasing here right now, no sharing between mapping and field data
           either. */
        /** @todo field aliasing might be useful at some point */
        itemViewMappings[i].second() = itemViewOffset;
        const Trade::SceneFieldType fieldType = field.fieldType();
        if(fieldType == Trade::SceneFieldType::Bit) {
            arrayAppend(items, InPlaceInit,
                NoInit,
                Containers::Size2D{field.size(), field.fieldArraySize() ? field.fieldArraySize() : 1},
                itemViews[itemViewOffset].bits);
            ++itemViewOffset;
        } else {
            arrayAppend(items, InPlaceInit,
                NoInit,
                field.size(), sceneFieldTypeSize(fieldType)*(field.fieldArraySize() ? field.fieldArraySize() : 1),
                sceneFieldTypeAlignment(fieldType),
                itemViews[itemViewOffset].types);
            ++itemViewOffset;

            /* For string fields we need to allocate also for the actual string
               data. For space reasons the SceneFieldData stores only the data
               pointer, size is implicit, so need to calculate it as the max of
               end pointers of all strings */
            if(Trade::Implementation::isSceneFieldTypeString(fieldType)) {
                const Containers::StridedArrayView1D<const void> fieldData = field.fieldData();
                CORRADE_ASSERT(!field.size() || fieldData.data(),
                    "SceneTools::combineFields(): string field" << i << "has a placeholder data", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

                const char* const stringData = field.stringData();
                CORRADE_ASSERT(!field.size() || stringData,
                    "SceneTools::combineFields(): string field" << i << "has a placeholder string data", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

                std::size_t size;
                if(field.size() == 0)
                    size = 0;
                else if(fieldType == Trade::SceneFieldType::StringOffset8)
                    size = stringOffsetFieldSize<UnsignedByte>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringOffset16)
                    size = stringOffsetFieldSize<UnsignedShort>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringOffset32)
                    size = stringOffsetFieldSize<UnsignedInt>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringOffset64)
                    size = stringOffsetFieldSize<UnsignedLong>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRange8)
                    size = stringRangeFieldSize<UnsignedByte>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRange16)
                    size = stringRangeFieldSize<UnsignedShort>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRange32)
                    size = stringRangeFieldSize<UnsignedInt>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRange64)
                    size = stringRangeFieldSize<UnsignedLong>(fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRangeNullTerminated8)
                    size = stringRangeNullTerminatedFieldSize<UnsignedByte>(stringData, fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRangeNullTerminated16)
                    size = stringRangeNullTerminatedFieldSize<UnsignedShort>(stringData, fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRangeNullTerminated32)
                    size = stringRangeNullTerminatedFieldSize<UnsignedInt>(stringData, fieldData);
                else if(fieldType == Trade::SceneFieldType::StringRangeNullTerminated64)
                    size = stringRangeNullTerminatedFieldSize<UnsignedLong>(stringData, fieldData);
                else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

                itemViewMappings[i].third() = itemViewOffset;
                arrayAppend(items, InPlaceInit,
                    NoInit,
                    size,
                    itemViews[itemViewOffset].strings);
                ++itemViewOffset;
            }
        }
    }

    CORRADE_INTERNAL_ASSERT(itemViewOffset <= itemViews.size());

    /* Allocate the data */
    Containers::Array<char> outData = Containers::ArrayTuple{items};
    CORRADE_INTERNAL_ASSERT(!outData.deleter());

    /* Copy the mapping data over and cast them as necessary */
    if(mappingType == Trade::SceneMappingType::UnsignedByte)
        combineCopyMappings<UnsignedByte>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedShort)
        combineCopyMappings<UnsignedShort>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedInt)
        combineCopyMappings<UnsignedInt>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedLong)
        combineCopyMappings<UnsignedLong>(fields, itemViews, itemViewMappings);
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* Copy the field data over. No special handling needed here. */
    for(std::size_t i = 0; i != fields.size(); ++i) {
        const Trade::SceneFieldData& field = fields[i];
        const Trade::SceneFieldType fieldType = field.fieldType();
        if(fieldType == Trade::SceneFieldType::Bit) {
            const Containers::StridedBitArrayView2D src = field.fieldBitData();

            /* If the field has null field data, no need to copy anything. This
               covers reserved fields but also fields of zero size. */
            if(!src.data()) continue;

            /** @todo this needs Utility::copy() for bits, which is HARD */
            const Containers::MutableStridedBitArrayView2D dst = itemViews[itemViewMappings[i].second()].bits;
            const std::size_t arraySize = field.fieldArraySize() ? field.fieldArraySize() : 1;
            for(std::size_t j = 0, iMax = field.size(); j != iMax; ++j) {
                const Containers::StridedBitArrayView1D srcI = src[j];
                const Containers::MutableStridedBitArrayView1D dstI = dst[j];
                for(std::size_t k = 0; k != arraySize; ++k)
                    dstI.set(k, srcI[k]);
            }
        } else {
            const Containers::StridedArrayView1D<const void> src = field.fieldData();

            /* If the field has null field data, no need to copy anything. This
               covers reserved fields but also fields of zero size. */
            if(!src.data()) continue;

            /** @todo isn't there some less awful way to create a 2D view, sigh */
            Utility::copy(Containers::arrayCast<2, const char>(src, sceneFieldTypeSize(fieldType)*(field.fieldArraySize() ? field.fieldArraySize() : 1)), itemViews[itemViewMappings[i].second()].types);

            /* If the field is a string, copy also the actual string data. The
               size was calculated above and is recorded into the output
               view. */
            if(Trade::Implementation::isSceneFieldTypeString(fieldType)) {
                const Containers::MutableStringView dst = itemViews[itemViewMappings[i].third()].strings;
                Utility::copy(Containers::arrayView(field.stringData(), dst.size()), dst);
            }
        }
    }

    /* Map the fields to the new data */
    Containers::Array<Trade::SceneFieldData> outFields{fields.size()};
    for(std::size_t i = 0; i != fields.size(); ++i) {
        const Trade::SceneFieldData& field = fields[i];
        const Trade::SceneFieldType fieldType = field.fieldType();
        if(fieldType == Trade::SceneFieldType::Bit) {
            /* Pass arrays as 2D views, non-arrays as 1D views */
            if(field.fieldArraySize())
                outFields[i] = Trade::SceneFieldData{field.name(),
                    itemViews[itemViewMappings[i].first()].types,
                    itemViews[itemViewMappings[i].second()].bits,
                    field.flags()};
            else
                outFields[i] = Trade::SceneFieldData{field.name(),
                    itemViews[itemViewMappings[i].first()].types,
                    /** @todo creating a 1D view isn't really easy either, huh? */
                    itemViews[itemViewMappings[i].second()].bits.transposed<0, 1>()[0],
                    field.flags()};
        } else if(Trade::Implementation::isSceneFieldTypeString(fieldType)) {
            outFields[i] = Trade::SceneFieldData{field.name(),
                itemViews[itemViewMappings[i].first()].types,
                itemViews[itemViewMappings[i].third()].strings.data(),
                fieldType, itemViews[itemViewMappings[i].second()].types,
                field.flags()};
        } else {
            outFields[i] = Trade::SceneFieldData{field.name(),
                itemViews[itemViewMappings[i].first()].types,
                fieldType, itemViews[itemViewMappings[i].second()].types,
                field.fieldArraySize(), field.flags()};
        }
    }

    return Trade::SceneData{mappingType, mappingBound, Utility::move(outData), Utility::move(outFields)};
}

}}}

#endif
