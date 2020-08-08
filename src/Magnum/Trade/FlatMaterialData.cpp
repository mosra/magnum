/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "FlatMaterialData.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace Magnum { namespace Trade {

using namespace Math::Literals;

bool FlatMaterialData::hasTexture() const {
    return hasAttribute(MaterialAttribute::BaseColorTexture) ||
        hasAttribute(MaterialAttribute::DiffuseTexture);
}

bool FlatMaterialData::hasTextureTransformation() const {
    return (hasAttribute(MaterialAttribute::BaseColorTexture) && hasAttribute(MaterialAttribute::BaseColorTextureMatrix)) ||
        (hasAttribute(MaterialAttribute::DiffuseTexture) && hasAttribute(MaterialAttribute::DiffuseTextureMatrix)) ||
        hasAttribute(MaterialAttribute::TextureMatrix);
}

bool FlatMaterialData::hasTextureCoordinates() const {
    return (hasAttribute(MaterialAttribute::BaseColorTexture) && hasAttribute(MaterialAttribute::BaseColorTextureCoordinates)) ||
        (hasAttribute(MaterialAttribute::DiffuseTexture) && hasAttribute(MaterialAttribute::DiffuseTextureCoordinates)) ||
        hasAttribute(MaterialAttribute::TextureCoordinates);
}

Color4 FlatMaterialData::color() const {
    /* If the material has a texture, return the color that matches it */
    if(hasAttribute(MaterialAttribute::BaseColorTexture))
        return attributeOr(MaterialAttribute::BaseColor, 0xffffffff_srgbaf);
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        return attributeOr(MaterialAttribute::DiffuseColor, 0xffffffff_srgbaf);

    /* If there's no texture, return whatever is present */
    if(Containers::Optional<Color4> value = tryAttribute<Color4>(MaterialAttribute::BaseColor))
        return *value;
    return attributeOr(MaterialAttribute::DiffuseColor, 0xffffffff_srgbaf);
}

UnsignedInt FlatMaterialData::texture() const {
    /* Explicit assertion because printing that DiffuseTexture isn't found
       would be misleading as it can be also BaseColorTexture */
    CORRADE_ASSERT(hasTexture(),
        "Trade::FlatMaterialData::texture(): the material doesn't have a texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::BaseColorTexture))
        return *value;
    return attribute<UnsignedInt>(MaterialAttribute::DiffuseTexture);
}

Matrix3 FlatMaterialData::textureMatrix() const {
    if(hasAttribute(MaterialAttribute::BaseColorTexture)) {
        if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::BaseColorTextureMatrix))
            return *value;
        return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
    }

    if(hasAttribute(MaterialAttribute::DiffuseTexture)) {
        if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::DiffuseTextureMatrix))
            return *value;
        return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::FlatMaterialData::textureMatrix(): the material doesn't have a texture", {});
}

UnsignedInt FlatMaterialData::textureCoordinates() const {
    if(hasAttribute(MaterialAttribute::BaseColorTexture)) {
        if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::BaseColorTextureCoordinates))
            return *value;
        return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
    }

    if(hasAttribute(MaterialAttribute::DiffuseTexture)) {
        if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates))
            return *value;
        return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::FlatMaterialData::textureCoordinates(): the material doesn't have a texture", {});
}

}}
