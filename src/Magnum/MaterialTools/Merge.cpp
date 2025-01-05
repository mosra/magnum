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

#include "Merge.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

Containers::Optional<Trade::MaterialData> merge(const Trade::MaterialData& first, const Trade::MaterialData& second, MergeConflicts conflicts) {
    Containers::Array<Trade::MaterialAttributeData> attributes;
    arrayReserve(attributes, first.attributeData().size() + second.attributeData().size());

    Containers::Array<UnsignedInt> layers{NoInit, Math::max(first.layerCount(), second.layerCount())};

    /* Go over all layers that are in both materials */
    std::size_t layer = 0;
    for(const std::size_t layerMin = Math::min(first.layerCount(), second.layerCount()); layer != layerMin; ++layer) {
        UnsignedInt attributeFirst = 0;
        UnsignedInt attributeSecond = 0;

        /* Take the earliest-sorted attribute from either material */
        while(attributeFirst != first.attributeCount(layer) && attributeSecond != second.attributeCount(layer)) {
            /* Attributes have the same name */
            /** @todo add StringView::compare() returning -1, 0, 1 and do the
                comparison just once instead of three times */
            if(first.attributeName(layer, attributeFirst) == second.attributeName(layer, attributeSecond)) {
                /* Fail if we are told to not merge attributes of the same
                   name */
                if(conflicts == MergeConflicts::Fail) {
                    Error{} << "MaterialTools::merge(): conflicting attribute" << first.attributeName(layer, attributeFirst) << "in layer" << layer;
                    return {};
                }

                /* Fail if we are told to not merge attributes of the same name
                   but different type */
                if(first.attributeType(layer, attributeFirst) != second.attributeType(layer, attributeSecond) && conflicts == MergeConflicts::KeepFirstIfSameType) {
                    Error{} << "MaterialTools::merge(): conflicting type" << first.attributeType(layer, attributeFirst) << "vs" << Debug::packed << second.attributeType(layer, attributeSecond) << "of attribute" << first.attributeName(layer, attributeFirst) << "in layer" << layer;
                    return {};
                }

                /* Add the first attribute, ignore the second */
                arrayAppend(attributes, first.attributeData(layer, attributeFirst));
                ++attributeFirst;
                ++attributeSecond;

            /* The attribute from first material should go first */
            } else if(first.attributeName(layer, attributeFirst) < second.attributeName(layer, attributeSecond)) {
                arrayAppend(attributes, first.attributeData(layer, attributeFirst));
                ++attributeFirst;

            /* The attribute from second material should go first */
            } else if(first.attributeName(layer, attributeFirst) > second.attributeName(layer, attributeSecond)) {
                arrayAppend(attributes, second.attributeData(layer, attributeSecond));
                ++attributeSecond;
            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }

        /* Consume remaining leftover attributes in either. Only one of these
           loops get entered. */
        while(attributeFirst < first.attributeCount(layer)) {
            arrayAppend(attributes, first.attributeData(layer, attributeFirst));
            ++attributeFirst;
        }
        while(attributeSecond < second.attributeCount(layer)) {
            arrayAppend(attributes, second.attributeData(layer, attributeSecond));
            ++attributeSecond;
        }

        layers[layer] = attributes.size();
    }

    /* Go over remaining layers which weren't in the other attribute and
       add them as a whole. Only one of these loops get entered. */
    for(; layer < first.layerCount(); ++layer) {
        arrayAppend(attributes, first.attributeData().slice(
            first.attributeDataOffset(layer),
            first.attributeDataOffset(layer + 1)));

        layers[layer] = attributes.size();
    }
    for(; layer < second.layerCount(); ++layer) {
        arrayAppend(attributes, second.attributeData().slice(
            second.attributeDataOffset(layer),
            second.attributeDataOffset(layer + 1)));

        layers[layer] = attributes.size();
    }

    CORRADE_INTERNAL_ASSERT(layer == layers.size());

    return Trade::MaterialData{first.types()|second.types(), Utility::move(attributes), Utility::move(layers)};
}

}}
