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

#include "PbrClearCoatMaterialData.h"

namespace Magnum { namespace Trade {

bool PbrClearCoatMaterialData::hasLayerFactorRoughnessTexture() const {
    return hasAttribute(MaterialAttribute::LayerFactorTexture) &&
        hasAttribute(MaterialAttribute::RoughnessTexture) &&
        attribute<UnsignedInt>(MaterialAttribute::LayerFactorTexture) == attribute<UnsignedInt>(MaterialAttribute::RoughnessTexture) &&
        layerFactorTextureSwizzle() == MaterialTextureSwizzle::R &&
        roughnessTextureSwizzle() == MaterialTextureSwizzle::G &&
        layerFactorTextureMatrix() == roughnessTextureMatrix() &&
        layerFactorTextureCoordinates() == roughnessTextureCoordinates() &&
        layerFactorTextureLayer() == roughnessTextureLayer();
}

bool PbrClearCoatMaterialData::hasTextureTransformation() const {
    return hasAttribute(MaterialAttribute::LayerFactorTextureMatrix) ||
        hasAttribute(MaterialAttribute::RoughnessTextureMatrix) ||
        hasAttribute(MaterialAttribute::NormalTextureMatrix) ||
        hasAttribute(MaterialAttribute::TextureMatrix) ||
        hasAttribute(0, MaterialAttribute::TextureMatrix);
}

bool PbrClearCoatMaterialData::hasCommonTextureTransformation() const {
    auto check = [](Containers::Optional<Matrix3>& transformation, Matrix3 current) {
        if(!transformation) {
            transformation = current;
            return true;
        }
        return transformation == current;
    };

    Containers::Optional<Matrix3> transformation;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::LayerFactorTexture) && !check(transformation, layerFactorTextureMatrix()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasAttribute(MaterialAttribute::RoughnessTexture) && !check(transformation, roughnessTextureMatrix()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(transformation, normalTextureMatrix()))
        return false;

    return true;
}

bool PbrClearCoatMaterialData::hasTextureCoordinates() const {
    return attributeOr(MaterialAttribute::LayerFactorTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::RoughnessTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::NormalTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::TextureCoordinates, 0u) ||
        attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

bool PbrClearCoatMaterialData::hasCommonTextureCoordinates() const {
    auto check = [](Containers::Optional<UnsignedInt>& coordinates, UnsignedInt current) {
        if(!coordinates) {
            coordinates = current;
            return true;
        }
        return coordinates == current;
    };

    Containers::Optional<UnsignedInt> coordinates;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::LayerFactorTexture) && !check(coordinates, layerFactorTextureCoordinates()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasAttribute(MaterialAttribute::RoughnessTexture) && !check(coordinates, roughnessTextureCoordinates()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(coordinates, normalTextureCoordinates()))
        return false;

    return true;
}

bool PbrClearCoatMaterialData::hasTextureLayer() const {
    return attributeOr(MaterialAttribute::LayerFactorTextureLayer, 0u) ||
        attributeOr(MaterialAttribute::RoughnessTextureLayer, 0u) ||
        attributeOr(MaterialAttribute::NormalTextureLayer, 0u) ||
        attributeOr(MaterialAttribute::TextureLayer, 0u) ||
        attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

bool PbrClearCoatMaterialData::hasCommonTextureLayer() const {
    auto check = [](Containers::Optional<UnsignedInt>& layer, UnsignedInt current) {
        if(!layer) {
            layer = current;
            return true;
        }
        return layer == current;
    };

    Containers::Optional<UnsignedInt> layer;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::LayerFactorTexture) && !check(layer, layerFactorTextureLayer()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasAttribute(MaterialAttribute::RoughnessTexture) && !check(layer, roughnessTextureLayer()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(layer, normalTextureLayer()))
        return false;

    return true;
}

Float PbrClearCoatMaterialData::roughness() const {
    return attributeOr(MaterialAttribute::Roughness, 0.0f);
}

UnsignedInt PbrClearCoatMaterialData::roughnessTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::RoughnessTexture);
}

MaterialTextureSwizzle PbrClearCoatMaterialData::roughnessTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::RoughnessTexture),
        "Trade::PbrClearCoatMaterialData::roughnessTextureSwizzle(): the layer doesn't have a roughness texture", {});
    return attributeOr(MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrClearCoatMaterialData::roughnessTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::RoughnessTexture),
        "Trade::PbrClearCoatMaterialData::roughnessTextureMatrix(): the layer doesn't have a roughness texture", {});
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(MaterialAttribute::RoughnessTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrClearCoatMaterialData::roughnessTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::RoughnessTexture),
        "Trade::PbrClearCoatMaterialData::roughnessTextureCoordinates(): the layer doesn't have a roughness texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::RoughnessTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrClearCoatMaterialData::roughnessTextureLayer() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::RoughnessTexture),
        "Trade::PbrClearCoatMaterialData::roughnessTextureLayer(): the layer doesn't have a roughness texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::RoughnessTextureLayer))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureLayer))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

UnsignedInt PbrClearCoatMaterialData::normalTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::NormalTexture);
}

Float PbrClearCoatMaterialData::normalTextureScale() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrClearCoatMaterialData::normalTextureScale(): the layer doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureScale, 1.0f);
}

MaterialTextureSwizzle PbrClearCoatMaterialData::normalTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrClearCoatMaterialData::normalTextureSwizzle(): the layer doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PbrClearCoatMaterialData::normalTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrClearCoatMaterialData::normalTextureMatrix(): the layer doesn't have a normal texture", {});
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(MaterialAttribute::NormalTextureMatrix))
        return *value;
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrClearCoatMaterialData::normalTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrClearCoatMaterialData::normalTextureCoordinates(): the layer doesn't have a normal texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::NormalTextureCoordinates))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrClearCoatMaterialData::normalTextureLayer() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrClearCoatMaterialData::normalTextureLayer(): the layer doesn't have a normal texture", {});
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::NormalTextureLayer))
        return *value;
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureLayer))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

Matrix3 PbrClearCoatMaterialData::commonTextureMatrix() const {
    CORRADE_ASSERT(hasCommonTextureTransformation(),
        "Trade::PbrClearCoatMaterialData::commonTextureMatrix(): the layer doesn't have a common texture coordinate transformation", {});
    if(hasAttribute(MaterialAttribute::LayerFactorTexture))
        return layerFactorTextureMatrix();
    if(hasAttribute(MaterialAttribute::RoughnessTexture))
        return roughnessTextureMatrix();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureMatrix();
    if(Containers::Optional<Matrix3> value = findAttribute<Matrix3>(MaterialAttribute::TextureMatrix))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrClearCoatMaterialData::commonTextureCoordinates() const {
    CORRADE_ASSERT(hasCommonTextureCoordinates(),
        "Trade::PbrClearCoatMaterialData::commonTextureCoordinates(): the layer doesn't have a common texture coordinate set", {});
    if(hasAttribute(MaterialAttribute::LayerFactorTexture))
        return layerFactorTextureCoordinates();
    if(hasAttribute(MaterialAttribute::RoughnessTexture))
        return roughnessTextureCoordinates();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureCoordinates();
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureCoordinates))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrClearCoatMaterialData::commonTextureLayer() const {
    CORRADE_ASSERT(hasCommonTextureLayer(),
        "Trade::PbrClearCoatMaterialData::commonTextureLayer(): the layer doesn't have a common array texture layer", {});
    if(hasAttribute(MaterialAttribute::LayerFactorTexture))
        return layerFactorTextureLayer();
    if(hasAttribute(MaterialAttribute::RoughnessTexture))
        return roughnessTextureLayer();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureLayer();
    if(Containers::Optional<UnsignedInt> value = findAttribute<UnsignedInt>(MaterialAttribute::TextureLayer))
        return *value;
    return attributeOr(0, MaterialAttribute::TextureLayer, 0u);
}

}}
