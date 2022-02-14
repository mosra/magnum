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

#include <unordered_map>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace SceneTools { namespace Implementation {

/* These two are needed because there (obviously) isn't any overload of
   castInto with the same input and output type */
template<class T, class U> void copyOrCastInto(const Containers::StridedArrayView1D<const T>& src, const Containers::StridedArrayView1D<U>& dst) {
    Math::castInto(Containers::arrayCast<2, const T>(src), Containers::arrayCast<2, U>(dst));
}
template<class T> void copyOrCastInto(const Containers::StridedArrayView1D<const T>& src, const Containers::StridedArrayView1D<T>& dst) {
    Utility::copy(src, dst);
}

template<class T> void combineCopyObjects(const Containers::ArrayView<const Trade::SceneFieldData> fields, const Containers::ArrayView<const Containers::StridedArrayView2D<char>> itemViews, const Containers::ArrayView<const Containers::Pair<UnsignedInt, UnsignedInt>> itemViewMappings) {
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
        if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedByte)
            copyOrCastInto(Containers::arrayCast<const UnsignedByte>(src), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedShort)
            copyOrCastInto(Containers::arrayCast<const UnsignedShort>(src), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedInt)
            copyOrCastInto(Containers::arrayCast<const UnsignedInt>(src), dst);
        else if(fields[i].mappingType() == Trade::SceneMappingType::UnsignedLong)
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
inline Trade::SceneData combine(const Trade::SceneMappingType mappingType, const UnsignedLong mappingBound, const Containers::ArrayView<const Trade::SceneFieldData> fields) {
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
        const Trade::SceneFieldData& field = fields[i];
        CORRADE_INTERNAL_ASSERT(!(field.flags() & Trade::SceneFieldFlag::OffsetOnly));

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
    if(mappingType == Trade::SceneMappingType::UnsignedByte)
        combineCopyObjects<UnsignedByte>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedShort)
        combineCopyObjects<UnsignedShort>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedInt)
        combineCopyObjects<UnsignedInt>(fields, itemViews, itemViewMappings);
    else if(mappingType == Trade::SceneMappingType::UnsignedLong)
        combineCopyObjects<UnsignedLong>(fields, itemViews, itemViewMappings);

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
