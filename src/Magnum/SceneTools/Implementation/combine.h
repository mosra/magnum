#ifndef Magnum_SceneTools_Implementation_combine_h
#define Magnum_SceneTools_Implementation_combine_h
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

#include <map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Implementation {

/* The combineFields() is currently transitively used also by Trade for
   (deprecated) backwards compatibility in SceneData, in particular by
   convertToSingleFunctionObjects(). Making Trade depend on SceneTools in a
   deprecated build would be a nasty complication, so the functions are inlined
   in a header that gets included in both */
/** @todo move everything to Combine.cpp and anonymous namespace once that
    compatibility is dropped */

template<class T> void combineCopyMappings(const Containers::ArrayView<const Trade::SceneFieldData> fields, const Containers::ArrayView<const Containers::StridedArrayView2D<char>> itemViews, const Containers::ArrayView<const Containers::Pair<UnsignedInt, UnsignedInt>> itemViewMappings) {
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
        const Containers::StridedArrayView2D<T> dst = Containers::arrayCast<2, T>(Containers::arrayCast<1, T>(itemViews[mapping]));
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

inline Trade::SceneData combineFields(const Trade::SceneMappingType mappingType, const UnsignedLong mappingBound, const Containers::ArrayView<const Trade::SceneFieldData> fields) {
    const std::size_t mappingTypeSize = sceneMappingTypeSize(mappingType);
    const std::size_t mappingTypeAlignment = sceneMappingTypeAlignment(mappingType);

    /* Track unique mapping views (pointer, size, stride) so fields that shared
       a mapping before stay shared after as well. A map<tuple> is used because
       it has conveniently implemented ordering, an unordered_map couldn't be
       used without manually implementing a std::tuple hash because STL DOES
       NOT HAVE IT, UGH. */
    std::map<std::tuple<const void*, std::size_t, std::ptrdiff_t>, UnsignedInt> uniqueMappings;
    Containers::Array<Containers::ArrayTuple::Item> items;
    Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> itemViewMappings{NoInit, fields.size()};

    /* The item views are referenced from ArrayTuple::Item, not using a
       growable array in order to avoid an accidental reallocation */
    /** @todo once never-reallocating allocators are present, use them instead
        of the manual offset */
    Containers::Array<Containers::StridedArrayView2D<char>> itemViews{fields.size()*2};
    std::size_t itemViewOffset = 0;

    /* Go through all fields and collect ArrayTuple allocations for these */
    for(std::size_t i = 0; i != fields.size(); ++i) {
        const Trade::SceneFieldData& field = fields[i];
        CORRADE_ASSERT(!(field.flags() & Trade::SceneFieldFlag::OffsetOnly),
            "SceneTools::combineFields(): field" << i << "is offset-only", (Trade::SceneData{Trade::SceneMappingType::UnsignedInt, 0, nullptr, {}}));

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

        /* If it's not shared or it's a placeholder, allocate a new mapping
           view of given size by adding a new item to the list of views to
           allocate by an ArrayTuple. */
        } else {
            itemViewMappings[i].first() = itemViewOffset;
            arrayAppend(items, InPlaceInit, NoInit, std::size_t(field.size()), mappingTypeSize, mappingTypeAlignment, itemViews[itemViewOffset]);
            ++itemViewOffset;
        }

        /* Field data, just allocate space for it. No extra logic needed -- no
           aliasing here right now, no sharing between mapping and field data
           either. */
        /** @todo field aliasing might be useful at some point */
        itemViewMappings[i].second() = itemViewOffset;
        arrayAppend(items, InPlaceInit, NoInit, std::size_t(field.size()), sceneFieldTypeSize(field.fieldType())*(field.fieldArraySize() ? field.fieldArraySize() : 1), sceneFieldTypeAlignment(field.fieldType()), itemViews[itemViewOffset]);
        ++itemViewOffset;
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
        /* If the field has null field data, no need to copy anything. This
           covers reserved fields but also fields of zero size. */
        if(!fields[i].fieldData()) continue;

        /** @todo isn't there some less awful way to create a 2D view, sigh */
        Utility::copy(Containers::arrayCast<2, const char>(fields[i].fieldData(), sceneFieldTypeSize(fields[i].fieldType())*(fields[i].fieldArraySize() ? fields[i].fieldArraySize() : 1)), itemViews[itemViewMappings[i].second()]);
    }

    /* Map the fields to the new data */
    Containers::Array<Trade::SceneFieldData> outFields{fields.size()};
    for(std::size_t i = 0; i != fields.size(); ++i) {
        outFields[i] = Trade::SceneFieldData{fields[i].name(), itemViews[itemViewMappings[i].first()], fields[i].fieldType(), itemViews[itemViewMappings[i].second()], fields[i].fieldArraySize(), fields[i].flags()};
    }

    return Trade::SceneData{mappingType, mappingBound, std::move(outData), std::move(outFields)};
}

}}}

#endif
