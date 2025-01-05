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

#include "Copy.h"

#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

Trade::MaterialData copy(const Trade::MaterialData& material) {
    return copy(Trade::MaterialData{material.types(),
        {}, material.attributeData(),
        {}, material.layerData(),
        material.importerState()});
}

Trade::MaterialData copy(Trade::MaterialData&& material) {
    /* Transfer attributes if they're owned & mutable, allocate a copy
       otherwise */
    Containers::Array<Trade::MaterialAttributeData> attributes;
    if(material.attributeDataFlags() >= (Trade::DataFlag::Mutable|Trade::DataFlag::Owned))
        attributes = material.releaseAttributeData();
    else {
        /* DefaultInit so we don't use a non-default deleter which could cause
           problems in plugins */
        attributes = Containers::Array<Trade::MaterialAttributeData>{DefaultInit, material.attributeData().size()};
        Utility::copy(material.attributeData(), attributes);
    }

    /* Same for layers, here it can be NoInit; if the original has implicit
       single layer (and empty data), this makes no allocation either */
    Containers::Array<UnsignedInt> layers;
    if(material.attributeDataFlags() >= (Trade::DataFlag::Mutable|Trade::DataFlag::Owned))
        layers = material.releaseLayerData();
    else {
        layers = Containers::Array<UnsignedInt>{NoInit, material.layerData().size()};
        Utility::copy(material.layerData(), layers);
    }

    return Trade::MaterialData{material.types(), Utility::move(attributes), Utility::move(layers), material.importerState()};
}

}}
