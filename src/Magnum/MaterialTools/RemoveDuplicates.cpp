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

#include "RemoveDuplicates.h"

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/MaterialTools/Implementation/attributesEqual.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

namespace {

bool materialEqual(const Trade::MaterialData& a, const Trade::MaterialData& b) {
    /* Check if types match */
    if(a.types() != b.types())
        return false;

    /* If one has layer data implicit and the other has just one layer, they're
       equivalent */
    if(!(a.layerData().isEmpty() && b.layerData().size() == 1 && a.attributeData().size() == b.layerData()[0]) &&
       !(b.layerData().isEmpty() && a.layerData().size() == 1 && b.attributeData().size() == a.layerData()[0]))
    {
        /* Otherwise, check if layer count matches */
        if(a.layerData().size() != b.layerData().size())
            return false;

        /* And if layer data match */
        if(const Containers::ArrayView<const UnsignedInt> layerData = a.layerData()) {
            for(UnsignedInt layer = 0; layer != layerData.size(); ++layer) {
                if(b.layerData()[layer] != layerData[layer])
                    return false;
            }
        }
    }

    /* Check if attribute count matches */
    if(a.attributeData().size() != b.attributeData().size())
        return false;

    /* Check if attribute data match */
    for(UnsignedInt attribute = 0; attribute != a.attributeData().size(); ++attribute) {
        if(a.attributeData()[attribute].name() != b.attributeData()[attribute].name() ||
           a.attributeData()[attribute].type() != b.attributeData()[attribute].type() ||
          !Implementation::attributesEqual(a.attributeData()[attribute], b.attributeData()[attribute]))
            return false;
    }

    return true;
}

}

std::size_t removeDuplicatesInPlaceInto(const Containers::Iterable<Trade::MaterialData>& materials, const Containers::StridedArrayView1D<UnsignedInt>& mapping) {
    CORRADE_ASSERT(mapping.size() == materials.size(),
        "MaterialTools::removeDuplicatesInPlaceInto(): bad output size, expected" << materials.size() << "but got" << mapping.size(), {});

    /* O(n^2). As there's a lot of early returns, should be fine for a moderate
       count of materials that differ in a significant way. Won't work well for
       materials that are all the same except one attribute value. */
    std::size_t uniqueCount = 0;
    for(std::size_t i = 0; i != materials.size(); ++i) {
        /* Find a material that's already in the unique set */
        Containers::Optional<UnsignedInt> found;
        for(std::size_t j = 0; j != uniqueCount; ++j) {
            if(materialEqual(materials[i], materials[j])) {
                found = j;
                break;
            }
        }

        /* Material found, reference its ID */
        if(found) {
            mapping[i] = *found;

        /* Move the material into its new location, unless it's the same
           index, and increase the number of unique materials */
        } else {
            if(uniqueCount != i)
                materials[uniqueCount] = Utility::move(materials[i]);
            mapping[i] = uniqueCount++;
        }
    }

    return uniqueCount;
}

Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicatesInPlace(const Containers::Iterable<Trade::MaterialData>& materials) {
    Containers::Array<UnsignedInt> out{NoInit, materials.size()};
    const std::size_t uniqueCount = removeDuplicatesInPlaceInto(materials, out);
    return {Utility::move(out), uniqueCount};
}

std::size_t removeDuplicatesInto(const Containers::Iterable<const Trade::MaterialData>& materials, const Containers::StridedArrayView1D<UnsignedInt>& mapping) {
    CORRADE_ASSERT(mapping.size() == materials.size(),
        "MaterialTools::removeDuplicatesInto(): bad output size, expected" << materials.size() << "but got" << mapping.size(), {});

    /* O(n^2). Like removeDuplicatesInPlaceInto(), but as the input material
       list is immutable, it has to go through the already-processed prefix
       and compare only against materials that are unique, which may add some
       extra overhead. Another option would be to allocate a temporary array
       with (contiguous) references to the material data, but so far I think
       the prefix iteration is efficient enough to not need that. */
    std::size_t uniqueCount = 0;
    for(std::size_t i = 0; i != materials.size(); ++i) {
        /* Find a material that's already in the unique set by going through
           the already-processed prefix and comparing only against materials
           that are unique, i.e. for which the output index is the same as the
           input index. */
        Containers::Optional<UnsignedInt> found;
        for(std::size_t j = 0; j != i; ++j) {
            if(mapping[j] == j && materialEqual(materials[i], materials[j])) {
                found = j;
                break;
            }
        }

        /* Material found, reference its ID */
        if(found) {
            mapping[i] = *found;

        /* Otherwise the output index the same as the input index. Also
           increase the number of unique materials which isn't used for
           anything here except the return value. */
        } else {
            mapping[i] = i;
            uniqueCount++;
        }
    }

    return uniqueCount;
}

Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> removeDuplicates(const Containers::Iterable<const Trade::MaterialData>& materials) {
    Containers::Array<UnsignedInt> out{NoInit, materials.size()};
    const std::size_t uniqueCount = removeDuplicatesInto(materials, out);
    return {Utility::move(out), uniqueCount};
}

}}
