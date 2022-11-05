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

#include "PhongToPbrMetallicRoughness.h"

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

using namespace Containers::Literals;

Containers::Optional<Trade::MaterialData> phongToPbrMetallicRoughness(const Trade::MaterialData& material, const PhongToPbrMetallicRoughnessFlags flags) {
    /* Output attributes, reserve assuming some input attributes will get
       replaced with different */
    Containers::Array<Trade::MaterialAttributeData> attributes;
    arrayReserve(attributes, material.attributeData().size());

    /* Attributes to skip in the base layer */
    Containers::BitArray attributesToSkip{ValueInit, material.attributeCount(0)};

    /* Decide about unconvertable attributes */
    /** @todo conversion of these:
        https://github.com/CesiumGS/obj2gltf/blob/9b018ff6968edf76c33d2a68eb51a3605b873d12/lib/loadMtl.js#L962-L989
            - "textures not converted lol"
        https://computergraphics.stackexchange.com/a/1517
            - for shininess, expose flags for picking different approaches */
    for(const Trade::MaterialAttribute attribute: {
        Trade::MaterialAttribute::AmbientColor,
        Trade::MaterialAttribute::SpecularColor,
        Trade::MaterialAttribute::Shininess
    }) {
        const Containers::Optional<UnsignedInt> id = material.findAttributeId(attribute);
        if(!id) continue;

        if(flags >= PhongToPbrMetallicRoughnessFlag::FailOnUnconvertableAttributes) {
            Error{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertable" << attribute << "attribute";
            return {};
        }

        Warning{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertable" << attribute << "attribute, skipping";
        if(flags >= PhongToPbrMetallicRoughnessFlag::DropUnconvertableAttributes)
            attributesToSkip.set(*id);
    }
    for(const Trade::MaterialAttribute attribute: {
        Trade::MaterialAttribute::AmbientTexture,
        Trade::MaterialAttribute::SpecularTexture
    }) {
        const Containers::Optional<UnsignedInt> id = material.findAttributeId(attribute);
        if(!id) continue;

        if(flags >= PhongToPbrMetallicRoughnessFlag::FailOnUnconvertableAttributes) {
            Error{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertable" << attribute << "attribute";
            return {};
        }

        const Containers::Optional<UnsignedInt> matrixId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Matrix"_s);
        const Containers::Optional<UnsignedInt> coordinatesId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Coordinates"_s);
        const Containers::Optional<UnsignedInt> layerId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Layer"_s);

        Warning{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertable" << attribute << "attribute, skipping";
        if(flags >= PhongToPbrMetallicRoughnessFlag::DropUnconvertableAttributes) {
            attributesToSkip.set(*id);
            if(matrixId)
                attributesToSkip.set(*matrixId);
            if(coordinatesId)
                attributesToSkip.set(*coordinatesId);
            if(layerId)
                attributesToSkip.set(*layerId);
        }
    }

    /* Diffuse color */
    if(const Containers::Optional<UnsignedInt> id = material.findAttributeId(Trade::MaterialAttribute::DiffuseColor)) {
        /* Convert only if the target attribute isn't there already */
        if(!material.hasAttribute(Trade::MaterialAttribute::BaseColor))
            arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColor, material.attribute<Vector4>(*id));

        /* Skip unless we're told to keep the original attributes */
        if(!(flags >= PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes))
            attributesToSkip.set(*id);
    }

    /* Diffuse texture and related attributes */
    if(const Containers::Optional<UnsignedInt> id = material.findAttributeId(Trade::MaterialAttribute::DiffuseTexture)) {
        const Containers::Optional<UnsignedInt> matrixId = material.findAttributeId(Trade::MaterialAttribute::DiffuseTextureMatrix);
        const Containers::Optional<UnsignedInt> coordinatesId = material.findAttributeId(Trade::MaterialAttribute::DiffuseTextureCoordinates);
        const Containers::Optional<UnsignedInt> layerId = material.findAttributeId(Trade::MaterialAttribute::DiffuseTextureLayer);

        /* Convert only if the target attribute isn't there already */
        if(!material.hasAttribute(Trade::MaterialAttribute::BaseColorTexture)) {
            arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColorTexture, material.attribute<UnsignedInt>(*id));
            if(matrixId)
                arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColorTextureMatrix, material.attribute<Matrix3>(*matrixId));
            if(coordinatesId)
                arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColorTextureCoordinates, material.attribute<UnsignedInt>(*coordinatesId));
            if(layerId)
                arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColorTextureLayer, material.attribute<UnsignedInt>(*layerId));
        }

        /* Skip unless we're told to keep the original attributes */
        if(!(flags >= PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes)) {
            attributesToSkip.set(*id);
            if(matrixId)
                attributesToSkip.set(*matrixId);
            if(coordinatesId)
                attributesToSkip.set(*coordinatesId);
            if(layerId)
                attributesToSkip.set(*layerId);
        }
    }

    /* New layer offsets. If there's no layer data in the original, the whole
       attribute array is the base layer */
    Containers::Array<UnsignedInt> layers;
    if(material.layerData()) {
        /* Calculate the difference in base layer size */
        Int baseLayerSizeDifference = attributes.size();
        /** @todo have popcount() on BitArray */
        for(std::size_t i = 0; i != attributesToSkip.size(); ++i)
            if(attributesToSkip[i]) --baseLayerSizeDifference;

        /* Fill the new layer offset array */
        layers = Containers::Array<UnsignedInt>{NoInit, material.layerData().size()};
        for(std::size_t i = 0; i != layers.size(); ++i)
            layers[i] = material.layerData()[i] + baseLayerSizeDifference;
    }

    /* Add the remaining attribute data including the extra layers, except ones
       that are meant to be skipped in the base layer */
    for(std::size_t i = 0; i != material.attributeData().size(); ++i)
        if(i >= attributesToSkip.size() || !attributesToSkip[i])
            arrayAppend(attributes, material.attributeData()[i]);

    /* Replace Phong with PbrMetallicRoughness in the output */
    return Trade::MaterialData{(material.types() & ~Trade::MaterialType::Phong)|Trade::MaterialType::PbrMetallicRoughness, std::move(attributes), std::move(layers)};
}

}}
