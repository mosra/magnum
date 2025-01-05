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

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/StridedBitArrayView.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/BitAlgorithms.h>

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

namespace {

/* - if inputLayersToKeep is nullptr, all layers are kept,
   - if inputAttributesToKeep is nullptr then only attributes present in
     enabled layers are kept,
   - if neither is nullptr then inputAttributesToKeep is patched to have zero
     bits for all attributes in filtered-out layers,
   - both can't be nullptr */
Trade::MaterialData filterAttributesLayersImplementation(const Trade::MaterialData& material, const Containers::BitArrayView inputAttributesToKeep, const Containers::BitArrayView inputLayersToKeep, const Trade::MaterialTypes typesToKeep) {
    const std::size_t totalAttributeCount = material.attributeDataOffset(material.layerCount());

    /* Generate the input attribute bit array or make a mutable copy. Using an
       Array<char> and not a BitArray so we can preserve the bit offset and
       copy without a shift. */
    /** @todo copy() for BitArray so we don't have to work around like this */
    /** @todo if the input would be mutable already, we could avoid the
        allocation, but does that matter at all? */
    Containers::Array<char> patchedInputAttributesToKeepData;
    Containers::MutableBitArrayView patchedInputAttributesToKeep;
    if(inputAttributesToKeep.data()) {
        const std::size_t sizeInBytes = (inputAttributesToKeep.offset() + inputAttributesToKeep.size() + 7)/8;
        patchedInputAttributesToKeepData = Containers::Array<char>{NoInit, sizeInBytes};
        Utility::copy({inputAttributesToKeep.data(), sizeInBytes}, patchedInputAttributesToKeepData);
        patchedInputAttributesToKeep = {patchedInputAttributesToKeepData.data(), inputAttributesToKeep.offset(), inputAttributesToKeep.size()};
    } else {
        patchedInputAttributesToKeepData = Containers::Array<char>{NoInit, (totalAttributeCount + 7)/8};
        patchedInputAttributesToKeep = {patchedInputAttributesToKeepData.data(), 0, totalAttributeCount};
        patchedInputAttributesToKeep.setAll();
    }

    /* Patch the attribute array to have zeros for all attributes in
       filtered-out layers */
    if(inputLayersToKeep.data()) for(UnsignedInt i = 0; i != material.layerCount(); ++i) {
        if(!inputLayersToKeep[i]) patchedInputAttributesToKeep
            .slice(material.attributeDataOffset(i),
                   material.attributeDataOffset(i + 1))
            .resetAll();
    }

    /* Count of layers to keep. If the base layer is to be removed, it just
       gets empty -- otherwise, say, a ClearColor layer would become a base
       layer and that's generally unwanted */
    std::size_t layerCount;
    if(inputLayersToKeep.data()) {
        layerCount = inputLayersToKeep.count();
        if(!inputLayersToKeep[0]) ++layerCount;
    } else layerCount = material.layerCount();

    /* Fill in the layer offsets based on count of attributes in each, skipping
       layers that are filtered away */
    Containers::Array<UnsignedInt> layers{NoInit, layerCount};
    /** @todo some "iterate set bits" utility, or "next set bit" */
    UnsignedInt layerOffset = 0;
    for(UnsignedInt i = 0; i != material.layerCount(); ++i) {
        if(inputLayersToKeep.data() && !inputLayersToKeep[i]) {
            /* The base layer stays, just gets empty */
            if(i == 0) layers[layerOffset++] = 0;
            continue;
        }
        layers[layerOffset++] = patchedInputAttributesToKeep.prefix(material.attributeDataOffset(i + 1)).count();
    }
    CORRADE_INTERNAL_ASSERT(layerOffset == layers.size());

    /* Copy attributes that aren't filtered away. Not using NoInit in order to
       use the default deleter and have this usable from plugins. */
    Containers::Array<Trade::MaterialAttributeData> attributes{patchedInputAttributesToKeep.count()};
    Utility::copyMasked(material.attributeData(), patchedInputAttributesToKeep, attributes);

    return Trade::MaterialData{material.types() & typesToKeep, Utility::move(attributes), Utility::move(layers)};
}

}

Trade::MaterialData filterAttributes(const Trade::MaterialData& material, const Containers::BitArrayView attributesToKeep, const Trade::MaterialTypes typesToKeep) {
    CORRADE_ASSERT(attributesToKeep.size() == material.attributeData().size(),
        "MaterialTools::filterAttributes(): expected" << material.attributeData().size() << "bits but got" << attributesToKeep.size(), (Trade::MaterialData{{}, {}}));

    return filterAttributesLayersImplementation(material, attributesToKeep, nullptr, typesToKeep);
}

Trade::MaterialData filterLayers(const Trade::MaterialData& material, const Containers::BitArrayView layersToKeep, const Trade::MaterialTypes typesToKeep) {
    CORRADE_ASSERT(layersToKeep.size() == material.layerCount(),
        "MaterialTools::filterLayers(): expected" << material.layerCount() << "bits but got" << layersToKeep.size(), (Trade::MaterialData{{}, {}}));

    return filterAttributesLayersImplementation(material, nullptr, layersToKeep, typesToKeep);
}

Trade::MaterialData filterAttributesLayers(const Trade::MaterialData& material, const Containers::BitArrayView attributesToKeep, const Containers::BitArrayView layersToKeep, const Trade::MaterialTypes typesToKeep) {
    CORRADE_ASSERT(attributesToKeep.size() == material.attributeData().size(),
        "MaterialTools::filterAttributesLayers(): expected" << material.attributeData().size() << "attribute bits but got" << attributesToKeep.size(), (Trade::MaterialData{{}, {}}));
    CORRADE_ASSERT(layersToKeep.size() == material.layerCount(),
        "MaterialTools::filterAttributesLayers(): expected" << material.layerCount() << "layer bits but got" << layersToKeep.size(), (Trade::MaterialData{{}, {}}));

    return filterAttributesLayersImplementation(material, attributesToKeep, layersToKeep, typesToKeep);
}

}}
