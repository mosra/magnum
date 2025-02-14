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

#include "Atlas.h"

#include <algorithm>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif

namespace Magnum { namespace TextureTools {

Debug& operator<<(Debug& debug, const AtlasLandfillFlag value) {
    debug << "TextureTools::AtlasLandfillFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case AtlasLandfillFlag::v: return debug << "::" #v;
        _c(RotatePortrait)
        _c(RotateLandscape)
        _c(WidestFirst)
        _c(NarrowestFirst)
        _c(ReverseDirectionAlways)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AtlasLandfillFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "TextureTools::AtlasLandfillFlags{}", {
        AtlasLandfillFlag::RotatePortrait,
        AtlasLandfillFlag::RotateLandscape,
        AtlasLandfillFlag::WidestFirst,
        AtlasLandfillFlag::NarrowestFirst,
        AtlasLandfillFlag::ReverseDirectionAlways,
    });
}

namespace Implementation {

struct AtlasLandfillState {
    struct Slice {
        Int direction = +1; /* +1 left-to-right, -1 right-to-left */
        /* If direction is left-to-right, it's offset from the left, otherwise
           from the right */
        Int xOffset = 0;
    };
    Containers::Array<Slice> slices;
    /* One entry for every size.x() */
    Containers::Array<UnsignedShort> yOffsets;
    /* X = MAX and z = 1 is for 2D unbounded, z = MAX is for 3D unbounded */
    Vector3i size;
    AtlasLandfillFlags flags = AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::WidestFirst;
    Vector2i padding;
};

}

namespace {

Containers::Optional<Range3Di> atlasLandfillAddSortedFlipped(Implementation::AtlasLandfillState& state, const Int slice, const Containers::StridedArrayView1D<const Containers::Pair<Vector2i, UnsignedInt>> sortedFlippedSizes, const Containers::StridedArrayView1D<Vector2i> offsets, const Containers::StridedArrayView1D<Int> zOffsets, const Containers::BitArrayView rotations) {
    /* Add a new slice if not there yet, extend the yOffsets array */
    if(UnsignedInt(slice) >= state.slices.size()) {
        CORRADE_INTERNAL_ASSERT(UnsignedInt(slice) == state.slices.size());
        CORRADE_INTERNAL_ASSERT(state.yOffsets.size() == state.slices.size()*state.size.x());
        arrayAppend(state.slices, InPlaceInit);
        /** @todo have an option to always start at the last tile so it doesn't
            use a ton of memory when not filling incrementally and doesn't take
            ages when incrementally filling a deep array */
        /** @todo Utility::fill() */
        for(UnsignedShort& i: arrayAppend(state.yOffsets, NoInit, state.size.x()))
            i = 0;
    }

    Implementation::AtlasLandfillState::Slice& sliceState = state.slices[slice];

    /* View on the Y offsets in current slice and in current fill direction */
    Containers::StridedArrayView1D<UnsignedShort> sliceYOffsets = state.yOffsets.sliceSize(slice*state.size.x(), state.size.x());
    if(sliceState.direction == -1)
        sliceYOffsets = sliceYOffsets.flipped<0>();

    Range3Di range;
    std::size_t i;
    for(i = 0; i != sortedFlippedSizes.size(); ++i) {
        const Vector2i size = sortedFlippedSizes[i].first();

        /* If the width cannnot fit into current offset, start a new row */
        if(sliceState.xOffset + size.x() > state.size.x()) {
            /* Flip the direction and start from the same position if we're
               either forced to or we ended up not higher than on the other
               side, otherwise start from the other side in the same direction
               in an attempt to level it up */
            if((state.flags & AtlasLandfillFlag::ReverseDirectionAlways) || sliceYOffsets.front() >= sliceYOffsets[sliceState.xOffset - 1]) {
                sliceState.direction *= -1;
                sliceYOffsets = sliceYOffsets.flipped<0>();
            }

            sliceState.xOffset = 0;
        }

        /* Find the lowest Y offset where the width can be placed. If the
           height cannot fit in there, bail. */
        const Containers::StridedArrayView1D<UnsignedShort> placementYOffsets = sliceYOffsets.sliceSize(sliceState.xOffset, size.x());
        const Int placementYOffset = Math::max(placementYOffsets);
        /** @todo skip it until some smaller fits, and then continue with the
            skipped rest to the next slice */
        if(placementYOffset + size.y() > state.size.y())
            break;

        /** @todo Utility::fill() */
        const UnsignedShort newYOffset = placementYOffset + size.y();
        for(UnsignedShort& yOffset: placementYOffsets)
            yOffset = newYOffset;

        /* Index of this item in the original array */
        const UnsignedInt index = sortedFlippedSizes[i].second();

        /* Figure out padding of this item. If the size was rotated, rotate it
           as well. If the rotations aren't even present, no rotations were
           done. */
        const Vector2i padding = !rotations.isEmpty() && rotations[index] ?
            state.padding.flipped() : state.padding;

        /* Save the position (X-flip it in case we're in reverse direction),
           add the (appropriately rotated) padding to it so it points to the
           original unpadded size */
        const Vector2i offset{
            sliceState.direction > 0 ? sliceState.xOffset :
                state.size.x() - sliceState.xOffset - size.x(),
            placementYOffset};
        offsets[index] = padding + offset;

        /* Add this item to the range spanning all added items, including the
           (potentially rotated) padding */
        range = join(range, Range3Di::fromSize({offset, slice}, {size, 1}));

        /* Advance to the next X offset */
        sliceState.xOffset += size.x();
    }

    /* If the Z offset array is present, fill it with current slice index for
       all items that fit */
    if(zOffsets) for(std::size_t j = 0; j != i; ++j)
        zOffsets[sortedFlippedSizes[j].second()] = slice;

    /* If there are items that didn't fit, recurse to the next slice. This
       should only happen if the Y size is bounded. */
    if(i < sortedFlippedSizes.size()) {
        /* If there are no more slices, fail */
        if(slice + 1 == state.size.z())
            return {};
        /* If the recursion succeeded, return the two ranges joined */
        if(const Containers::Optional<Range3Di> out = atlasLandfillAddSortedFlipped(state, slice + 1,  sortedFlippedSizes.exceptPrefix(i), offsets, zOffsets, rotations))
            return Range3Di{join(range, *out)};
        /* If it didn't, fail */
        return {};
    }

    /* Everything fit, success */
    return range;
}

}

AtlasLandfill::AtlasLandfill(const Vector3i& size):_state{InPlaceInit} {
    CORRADE_ASSERT(size.x(), "TextureTools::AtlasLandfill: expected non-zero width, got" << Debug::packed << size, );
    CORRADE_ASSERT(size.y() || size.z() == 1, "TextureTools::AtlasLandfill: expected a single array slice for unbounded height, got" << Debug::packed << size, );
    CORRADE_ASSERT(size.y() <= 65536, "TextureTools::AtlasLandfill: expected height to fit into 16 bits, got" << Debug::packed << size, );

    /* Change y / z = 0 to y / z = MAX so the algorithm doesn't need to branch
       on that internally */
    _state->size = {size.x(),
                    size.y() ? size.y() : 0x7fffffff,
                    size.z() ? size.z() : 0x7fffffff};
}

AtlasLandfill::AtlasLandfill(const Vector2i& size): AtlasLandfill{{size, 1}} {}

AtlasLandfill::AtlasLandfill(AtlasLandfill&&) noexcept = default;

AtlasLandfill::~AtlasLandfill() = default;

AtlasLandfill& AtlasLandfill::operator=(AtlasLandfill&&) noexcept = default;

Vector3i AtlasLandfill::size() const {
    /* Change y / z = MAX (that's there so the algorithm doesn't need to branch
       on that internally) back to y / z = 0 */
    return {_state->size.x(),
            _state->size.y() == 0x7fffffff ? 0 : _state->size.y(),
            _state->size.z() == 0x7fffffff ? 0 : _state->size.z()};
}

Vector3i AtlasLandfill::filledSize() const {
    if(_state->size.z() == 1)
        return {_state->size.x(), Math::max(_state->yOffsets), 1};

    CORRADE_INTERNAL_ASSERT(_state->size.y());
    return {_state->size.xy(), Int(_state->slices.size())};
}

Vector2i AtlasLandfill::padding() const {
    return _state->padding;
}

AtlasLandfill& AtlasLandfill::setPadding(const Vector2i& padding) {
    _state->padding = padding;
    return *this;
}

AtlasLandfillFlags AtlasLandfill::flags() const {
    return _state->flags;
}

AtlasLandfill& AtlasLandfill::setFlags(AtlasLandfillFlags flags) {
    CORRADE_ASSERT(!(flags & AtlasLandfillFlag::RotatePortrait) ||
                   !(flags & AtlasLandfillFlag::RotateLandscape),
        "TextureTools::AtlasLandfill::setFlags(): only one of RotatePortrait and RotateLandscape can be set", *this);
    CORRADE_ASSERT(!(flags & AtlasLandfillFlag::WidestFirst) ||
                   !(flags & AtlasLandfillFlag::NarrowestFirst),
        "TextureTools::AtlasLandfill::setFlags(): only one of WidestFirst and NarrowestFirst can be set", *this);
    _state->flags = flags;
    return *this;
}

namespace {

Containers::Optional<Range3Di> atlasLandfillAdd(Implementation::AtlasLandfillState& state, const Containers::StridedArrayView1D<const Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i> offsets, const Containers::StridedArrayView1D<Int> zOffsets, const Containers::MutableBitArrayView rotations) {
    CORRADE_ASSERT(offsets.size() == sizes.size(),
        "TextureTools::AtlasLandfill::add(): expected sizes and offsets views to have the same size, got" << sizes.size() << "and" << offsets.size(), {});
    CORRADE_ASSERT((!(state.flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)) && rotations.isEmpty()) || rotations.size() == sizes.size(),
        "TextureTools::AtlasLandfill::add(): expected sizes and rotations views to have the same size, got" << sizes.size() << "and" << rotations.size(), {});
    /* These are sliced internally from a Vector3i input, so should match */
    CORRADE_INTERNAL_ASSERT(!zOffsets || zOffsets.size() == sizes.size());

    /* Nothing is flipped by default */
    rotations.resetAll();

    /* Copy all input sizes to a mutable array, flip them if not portrait,
       and remember their original order for sorting */
    Containers::Array<Containers::Pair<Vector2i, UnsignedInt>> sortedFlippedSizes{NoInit, sizes.size()};
    for(std::size_t i = 0; i != sizes.size(); ++i) {
        Vector2i size = sizes[i];
        #ifndef CORRADE_NO_ASSERT
        Vector2i padding = state.padding;
        #endif
        Vector2i sizePadded = size + 2*state.padding;
        if((state.flags & AtlasLandfillFlag::RotateLandscape && sizePadded.x() < sizePadded.y()) ||
           (state.flags & AtlasLandfillFlag::RotatePortrait && sizePadded.x() > sizePadded.y()))
        {
            #ifndef CORRADE_NO_ASSERT
            size = size.flipped();
            padding = padding.flipped();
            #endif
            sizePadded = sizePadded.flipped();
            rotations.set(i);
        }

        /* Zero-size items are allowed, as they don't really contribute to the
           layout in any way if padding is zero without needing to special-case
           anything:

            - If the item width is zero, it still gets sorted according to its
              height relative to thers and gets placed according to
              placementYOffsets, but no actual placementYOffsets update happens
              because the range to update is empty.
            - If the item height is zero and it's not rotated to a portrait
              becoming the above case, it's placed as the last item of all and
              if everything before fit, it fits always too. The
              placementYOffsets update *does* happen, but as there are no items
              after it only affects incremental filling.

           On the other hand, if padding is non-zero, the items are expected to
           not overlap each other by the caller (for example in order to
           perform a blur or distance field calculation). In that case they're
           treated as any other non-empty item. */

        #ifndef CORRADE_NO_ASSERT
        if(state.padding.isZero())
            CORRADE_ASSERT((sizePadded <= state.size.xy()).all(),
                "TextureTools::AtlasLandfill::add(): expected size" << i << "to be not larger than" << Debug::packed << state.size.xy() << "but got" << Debug::packed << size, {});
        else
            CORRADE_ASSERT((sizePadded <= state.size.xy()).all(),
                "TextureTools::AtlasLandfill::add(): expected size" << i << "to be not larger than" << Debug::packed << state.size.xy() << "but got" << Debug::packed << size << "and padding" << Debug::packed << padding, {});
        #endif

        sortedFlippedSizes[i] = {sizePadded, UnsignedInt(i)};
    }

    /* Sort according to the preference specified in flags, but always to have
       the highest first. It's highly likely there are many textures of the
       same size, thus use a stable sort to have output consistent across
       platforms. */
    /** @todo stable_sort allocates, would be great if i could make it reuse
        the memory allocated for output */
    if(state.flags & AtlasLandfillFlag::NarrowestFirst)
        std::stable_sort(sortedFlippedSizes.begin(), sortedFlippedSizes.end(), [](const Containers::Pair<Vector2i, UnsignedInt>& a, const Containers::Pair<Vector2i, UnsignedInt>& b) {
            return a.first().y() == b.first().y() ?
                a.first().x() < b.first().x() :
                a.first().y() > b.first().y();
        });
    else if(state.flags & AtlasLandfillFlag::WidestFirst)
        std::stable_sort(sortedFlippedSizes.begin(), sortedFlippedSizes.end(), [](const Containers::Pair<Vector2i, UnsignedInt>& a, const Containers::Pair<Vector2i, UnsignedInt>& b) {
            return a.first().y() == b.first().y() ?
                a.first().x() > b.first().x() :
                a.first().y() > b.first().y();
        });
    else
        std::stable_sort(sortedFlippedSizes.begin(), sortedFlippedSizes.end(), [](const Containers::Pair<Vector2i, UnsignedInt>& a, const Containers::Pair<Vector2i, UnsignedInt>& b) {
            return a.first().y() > b.first().y();
        });

    return atlasLandfillAddSortedFlipped(state, 0, sortedFlippedSizes, offsets, zOffsets, rotations);
}

}

Containers::Optional<Range3Di> AtlasLandfill::add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView flips) {
    return atlasLandfillAdd(*_state, sizes, offsets.slice(&Vector3i::xy), offsets.slice(&Vector3i::z), flips);
}

Containers::Optional<Range3Di> AtlasLandfill::add(const std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView flips) {
    return add(Containers::stridedArrayView(sizes), offsets, flips);
}

Containers::Optional<Range3Di> AtlasLandfill::add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets) {
    CORRADE_ASSERT(!(_state->flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)),
        "TextureTools::AtlasLandfill::add():" << (_state->flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)) << "set, expected a rotations view", {});
    return add(sizes, offsets, nullptr);
}

Containers::Optional<Range3Di> AtlasLandfill::add(const std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets) {
    return add(Containers::stridedArrayView(sizes), offsets);
}

Containers::Optional<Range2Di> AtlasLandfill::add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView flips) {
    CORRADE_ASSERT(_state->size.z() == 1,
        "TextureTools::AtlasLandfill::add(): use the three-component overload for an array atlas", {});
    const Containers::Optional<Range3Di> out = atlasLandfillAdd(*_state, sizes, offsets, nullptr, flips);
    return out ? Containers::optional(out->xy()) : Containers::NullOpt;
}

Containers::Optional<Range2Di> AtlasLandfill::add(const std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView flips) {
    return add(Containers::stridedArrayView(sizes), offsets, flips);
}

Containers::Optional<Range2Di> AtlasLandfill::add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets) {
    CORRADE_ASSERT(!(_state->flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)),
        "TextureTools::AtlasLandfill::add():" << (_state->flags & (AtlasLandfillFlag::RotatePortrait|AtlasLandfillFlag::RotateLandscape)) << "set, expected a rotations view", {});
    return add(sizes, offsets, nullptr);
}

Containers::Optional<Range2Di> AtlasLandfill::add(const std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets) {
    return add(Containers::stridedArrayView(sizes), offsets);
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::vector<Range2Di> atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes, const Vector2i& padding) {
    if(sizes.empty()) return {};

    /* Size of largest texture */
    Vector2i maxSize;
    for(const Vector2i& size: sizes)
        maxSize = Math::max(maxSize, size);

    std::vector<Range2Di> atlas;

    /* Columns and rows */
    const Vector2i paddedSize = maxSize+2*padding;
    const Vector2i gridSize = atlasSize/paddedSize;
    if(std::size_t(gridSize.product()) < sizes.size()) {
        Error() << "TextureTools::atlas(): requested atlas size" << atlasSize
                << "is too small to fit" << sizes.size() << paddedSize
                << "textures. Generated atlas will be empty.";
        return atlas;
    }

    /* I could also just delegate to the AtlasLandfill class, but that'd be a
       waste of time as the interface of this API is extremely bad anyway. */

    atlas.reserve(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        atlas.push_back(Range2Di::fromSize(Vector2i(i%gridSize.x(), i/gridSize.x())*paddedSize+padding, sizes[i]));

    return atlas;
}
#endif

Int atlasArrayPowerOfTwo(const Vector2i& layerSize, const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets) {
    CORRADE_ASSERT(offsets.size() == sizes.size(),
        "TextureTools::atlasArrayPowerOfTwo(): expected sizes and offsets views to have the same size, got" << sizes.size() << "and" << offsets.size(), {});
    CORRADE_ASSERT(layerSize.product() && layerSize.x() == layerSize.y() && (layerSize & (layerSize - Vector2i{1})).isZero(),
        "TextureTools::atlasArrayPowerOfTwo(): expected layer size to be a non-zero power-of-two square, got" << Debug::packed << layerSize, {});

    if(sizes.isEmpty())
        return {};

    /* Copy the input to a sorted array, together with a mapping to the
       original order stored in Z. Can't really reuse the output allocation
       as it would be overwritten in random order. */
    Containers::Array<Containers::Pair<Vector2i, UnsignedInt>> sortedSizes{NoInit, sizes.size()};
    for(std::size_t i = 0; i != sizes.size(); ++i) {
        const Vector2i size = sizes[i];
        CORRADE_ASSERT(size.product() && size.x() == size.y() && (size & (size - Vector2i{1})).isZero() && size <= layerSize,
            "TextureTools::atlasArrayPowerOfTwo(): expected size" << i << "to be a non-zero power-of-two square not larger than" << Debug::packed << layerSize << "but got" << Debug::packed << size, {});

        sortedSizes[i] = {size, UnsignedInt(i)};
    }

    /* Sort to have the biggest size first. Assuming the items are square,
       which is checked above in the loop. It's highly likely there are many
       textures of the same size, thus use a stable sort to have output
       consistent across platforms. */
    /** @todo stable_sort allocates, would be great if i could make it reuse
        the memory allocated for output */
    std::stable_sort(sortedSizes.begin(), sortedSizes.end(), [](const Containers::Pair<Vector2i, UnsignedInt>& a, const Containers::Pair<Vector2i, UnsignedInt>& b) {
        return a.first().x() > b.first().x();
    });

    /* Start with the whole first layer free */
    Int layer = 0;
    UnsignedInt free = 1;
    Vector2i previousSize = layerSize;
    for(const Containers::Pair<Vector2i, UnsignedInt>& size: sortedSizes) {
        /* No free slots left, go to the next layer. Then, what's free, is one
           whole layer. */
        if(!free) {
            ++layer;
            free = 1;
            previousSize = layerSize;
        }

        /* Multiply number of free slots based on area difference from previous
           size. If the size is the same, nothing changes. */
        /** @todo there's definitely some bit trick for dividing power-of-two
            numbers, use it */
        free *= (previousSize/size.first()).product();

        /* Slot index as if the whole layer was consisting just of slots of
           this size. */
        const UnsignedInt sideSlotCount = layerSize.x()/size.first().x();
        const UnsignedInt layerDepth = Math::log2(sideSlotCount);
        const UnsignedInt slotIndex = sideSlotCount*sideSlotCount - free;

        /* Calculate coordinates out of the slot index */
        Vector2i coordinates;
        for(UnsignedInt i = 0; i < layerDepth; ++i) {
            if(slotIndex & (1 << 2*(layerDepth - i - 1)))
                coordinates.x() += layerSize.x() >> (i + 1);
            if(slotIndex & (1 << (2*(layerDepth - i - 1) + 1)))
                coordinates.y() += layerSize.y() >> (i + 1);
        }

        /* Save to the output in the original order */
        offsets[size.second()] = {coordinates, layer};
        previousSize = size.first();
        --free;
    }

    return layer + 1;
}

Int atlasArrayPowerOfTwo(const Vector2i& layerSize, const std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets) {
    return atlasArrayPowerOfTwo(layerSize, Containers::stridedArrayView(sizes), offsets);
}

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, const Containers::StridedArrayView1D<const Vector2i>& sizes) {
    Containers::Array<Vector3i> offsets{NoInit, sizes.size()};
    Int layers = atlasArrayPowerOfTwo(layerSize, sizes, offsets);
    return {layers, Utility::move(offsets)};
}

Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, const std::initializer_list<Vector2i> sizes) {
    CORRADE_IGNORE_DEPRECATED_PUSH
    return atlasArrayPowerOfTwo(layerSize, Containers::stridedArrayView(sizes));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

Matrix3 atlasTextureCoordinateTransformation(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset) {
    CORRADE_ASSERT((offset + size <= atlasSize).all(),
        "TextureTools::atlasTextureCoordinateTransformation(): size" << Debug::packed << size << "and offset" << Debug::packed << offset << "doesn't fit into" << Debug::packed << atlasSize, {});
    const Vector2 atlasSizeF = Vector2{atlasSize};
    return Matrix3{{Float(size.x())/atlasSizeF.x(), 0.0f, 0.0f},
                   {0.0f, Float(size.y())/atlasSizeF.y(), 0.0f},
                   {Vector2{offset}/atlasSizeF, 1.0f}};
}

Matrix3 atlasTextureCoordinateTransformationRotatedCounterClockwise(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset) {
    CORRADE_ASSERT((offset + size.flipped() <= atlasSize).all(),
        "TextureTools::atlasTextureCoordinateTransformationRotatedCounterClockwise(): (rotated) size" << Debug::packed << size.flipped() << "and offset" << Debug::packed << offset << "doesn't fit into" << Debug::packed << atlasSize, {});
    const Vector2 atlasSizeF = Vector2{atlasSize};
    return Matrix3{{0.0f, Float(size.x())/atlasSizeF.y(), 0.0f},
                   {-Float(size.y())/atlasSizeF.x(), 0.0f, 0.0f},
                   {Float(offset.x() + size.y())/atlasSizeF.x(),
                    Float(offset.y())/atlasSizeF.y(), 1.0f}};
}

Matrix3 atlasTextureCoordinateTransformationRotatedClockwise(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset) {
    CORRADE_ASSERT((offset + size.flipped() <= atlasSize).all(),
        "TextureTools::atlasTextureCoordinateTransformationRotatedClockwise(): (rotated) size" << Debug::packed << size.flipped() << "and offset" << Debug::packed << offset << "doesn't fit into" << Debug::packed << atlasSize, {});
    const Vector2 atlasSizeF = Vector2{atlasSize};
    return Matrix3{{0.0f, -Float(size.x())/atlasSizeF.y(), 0.0f},
                   {Float(size.y())/atlasSizeF.x(), 0.0f, 0.0f},
                   {Float(offset.x())/atlasSizeF.x(),
                    Float(offset.y() + size.x())/atlasSizeF.y(), 1.0f}};
}

}}
