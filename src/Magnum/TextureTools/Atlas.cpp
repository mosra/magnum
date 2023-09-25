/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace TextureTools {

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

    /** @todo actual magic implementation, not this joke */

    atlas.reserve(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        atlas.push_back(Range2Di::fromSize(Vector2i(i%gridSize.x(), i/gridSize.x())*paddedSize+padding, sizes[i]));

    return atlas;
}

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

}}
