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

#include "Atlas.h"

#include <algorithm>
#include <vector>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Pair.h>

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

Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, const Containers::StridedArrayView1D<const Vector2i> sizes) {
    CORRADE_ASSERT(layerSize.product() && layerSize.x() == layerSize.y() && (layerSize & (layerSize - Vector2i{1})).isZero(),
        "TextureTools::atlasArrayPowerOfTwo(): expected layer size to be a non-zero power-of-two square, got" << Debug::packed << layerSize, {});

    if(sizes.isEmpty())
        return {};

    Containers::Array<Vector3i> output{NoInit, sizes.size()};

    /* Copy the input to a sorted array, together with a mapping to the
       original order stored in Z. Can't really reuse the output allocation
       as it would be overwritten in random order. */
    Containers::Array<Vector3i> sortedSizes{NoInit, sizes.size()};
    for(std::size_t i = 0; i != sizes.size(); ++i) {
        const Vector2i size = sizes[i];
        CORRADE_ASSERT(size.product() && size.x() == size.y() && (size & (size - Vector2i{1})).isZero(),
            "TextureTools::atlasArrayPowerOfTwo(): expected size" << i << "to be a non-zero power-of-two square, got" << Debug::packed << size, {});

        sortedSizes[i].xy() = size;
        sortedSizes[i].z() = i;
    }

    /* Sort to have the biggest size first. Assuming the items are square,
       which is checked below in the loop. It's highly likely there are many
       textures of the same size, thus use a stable sort to have output
       consistent across platforms */
    /** @todo stable_sort allocates, would be great if i could make it reuse
        the memory allocated for output */
    std::stable_sort(sortedSizes.begin(), sortedSizes.end(), [](const Vector3i& a, const Vector3i& b) {
        return a.x() > b.x();
    });

    /* Start with the whole first layer free */
    Int layer = 0;
    UnsignedInt free = 1;
    Vector2i previousSize = layerSize;
    for(const Vector3i& size: sortedSizes) {
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
        free *= (previousSize/size.xy()).product();

        /* Slot index as if the whole layer was consisting just of slots of
           this size. */
        const UnsignedInt sideSlotCount = layerSize.x()/size.x();
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
        output[size.z()] = {coordinates, layer};
        previousSize = size.xy();
        --free;
    }

    return {layer + 1, std::move(output)};
}

Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, const std::initializer_list<Vector2i> sizes) {
    return atlasArrayPowerOfTwo(layerSize, Containers::stridedArrayView(sizes));
}

}}
