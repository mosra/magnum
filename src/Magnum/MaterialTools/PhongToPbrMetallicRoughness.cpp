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

#include "PhongToPbrMetallicRoughness.h"

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/MaterialTools/Filter.h"
#include "Magnum/MaterialTools/Merge.h"
#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace MaterialTools {

using namespace Containers::Literals;

Containers::Optional<Trade::MaterialData> phongToPbrMetallicRoughness(const Trade::MaterialData& material, const PhongToPbrMetallicRoughnessFlags flags) {
    /* Attributes to merge into the base layer. We'll need at most 5 -- color,
       texture and texture layer/coordinates/matrix. */
    Containers::Array<Trade::MaterialAttributeData> attributes;
    arrayReserve(attributes, 5);

    /* Attributes to keep */
    Containers::BitArray attributesToKeep{DirectInit, material.attributeDataOffset(material.layerCount()), true};

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

        if(flags >= PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes) {
            Error{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertible" << attribute << "attribute";
            return {};
        }

        Warning{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertible" << attribute << "attribute, skipping";
        if(flags >= PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes)
            attributesToKeep.reset(*id);
    }
    for(const Trade::MaterialAttribute attribute: {
        Trade::MaterialAttribute::AmbientTexture,
        Trade::MaterialAttribute::SpecularTexture
    }) {
        const Containers::Optional<UnsignedInt> id = material.findAttributeId(attribute);
        if(!id) continue;

        if(flags >= PhongToPbrMetallicRoughnessFlag::FailOnUnconvertibleAttributes) {
            Error{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertible" << attribute << "attribute";
            return {};
        }

        const Containers::Optional<UnsignedInt> matrixId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Matrix"_s);
        const Containers::Optional<UnsignedInt> coordinatesId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Coordinates"_s);
        const Containers::Optional<UnsignedInt> layerId = material.findAttributeId(Trade::materialAttributeName(attribute) + "Layer"_s);

        Warning{} << "MaterialTools::phongToPbrMetallicRoughness(): unconvertible" << attribute << "attribute, skipping";
        if(flags >= PhongToPbrMetallicRoughnessFlag::DropUnconvertibleAttributes) {
            attributesToKeep.reset(*id);
            if(matrixId)
                attributesToKeep.reset(*matrixId);
            if(coordinatesId)
                attributesToKeep.reset(*coordinatesId);
            if(layerId)
                attributesToKeep.reset(*layerId);
        }
    }

    /* Diffuse color */
    if(const Containers::Optional<UnsignedInt> id = material.findAttributeId(Trade::MaterialAttribute::DiffuseColor)) {
        /* Convert only if the target attribute isn't there already */
        if(!material.hasAttribute(Trade::MaterialAttribute::BaseColor))
            arrayAppend(attributes, InPlaceInit, Trade::MaterialAttribute::BaseColor, material.attribute<Vector4>(*id));

        /* Skip unless we're told to keep the original attributes */
        if(!(flags >= PhongToPbrMetallicRoughnessFlag::KeepOriginalAttributes))
            attributesToKeep.reset(*id);
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
            attributesToKeep.reset(*id);
            if(matrixId)
                attributesToKeep.reset(*matrixId);
            if(coordinatesId)
                attributesToKeep.reset(*coordinatesId);
            if(layerId)
                attributesToKeep.reset(*layerId);
        }
    }

    /* Filter & merge the attributes -- they have to be moved into the
       constructor in order to get sorted. Remove the Phong type from the
       output as well. There should be no conflicts if we did everything above
       correctly, so just unpack the Optional directly. */
    return *CORRADE_INTERNAL_ASSERT_EXPRESSION(merge(
        filterAttributes(material, attributesToKeep, ~Trade::MaterialType::Phong),
        Trade::MaterialData{Trade::MaterialType::PbrMetallicRoughness, Utility::move(attributes)}));
}

}}
