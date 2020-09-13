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

#include "PbrSpecularGlossinessMaterialData.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace Magnum { namespace Trade {

using namespace Math::Literals;

bool PbrSpecularGlossinessMaterialData::hasSpecularTexture() const {
    return hasAttribute(MaterialAttribute::SpecularTexture) ||
           hasAttribute(MaterialAttribute::SpecularGlossinessTexture);
}

bool PbrSpecularGlossinessMaterialData::hasGlossinessTexture() const {
    return hasAttribute(MaterialAttribute::GlossinessTexture) ||
           hasAttribute(MaterialAttribute::SpecularGlossinessTexture);
}

bool PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture() const {
    return (hasAttribute(MaterialAttribute::SpecularGlossinessTexture) ||
       (hasAttribute(MaterialAttribute::SpecularTexture) &&
        hasAttribute(MaterialAttribute::GlossinessTexture) &&
        attribute<UnsignedInt>(MaterialAttribute::SpecularTexture) == attribute<UnsignedInt>(MaterialAttribute::GlossinessTexture) &&
        specularTextureSwizzle() == MaterialTextureSwizzle::RGB &&
        glossinessTextureSwizzle() == MaterialTextureSwizzle::A)) &&
       specularTextureMatrix() == glossinessTextureMatrix() &&
       specularTextureCoordinates() == glossinessTextureCoordinates();
}

bool PbrSpecularGlossinessMaterialData::hasTextureTransformation() const {
    return hasAttribute(MaterialAttribute::TextureMatrix) ||
        hasAttribute(MaterialAttribute::DiffuseTextureMatrix) ||
        hasAttribute(MaterialAttribute::SpecularTextureMatrix) ||
        hasAttribute(MaterialAttribute::GlossinessTextureMatrix) ||
        hasAttribute(MaterialAttribute::NormalTextureMatrix) ||
        hasAttribute(MaterialAttribute::OcclusionTextureMatrix) ||
        hasAttribute(MaterialAttribute::EmissiveTextureMatrix);
}

bool PbrSpecularGlossinessMaterialData::hasCommonTextureTransformation() const {
    auto check = [](Containers::Optional<Matrix3>& transformation, Matrix3 current) {
        if(!transformation) {
            transformation = current;
            return true;
        }
        return transformation == current;
    };

    Containers::Optional<Matrix3> transformation;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::DiffuseTexture) && !check(transformation, diffuseTextureMatrix()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasSpecularTexture() && !check(transformation, specularTextureMatrix()))
        return false;
    if(hasGlossinessTexture() && !check(transformation, glossinessTextureMatrix()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(transformation, normalTextureMatrix()))
        return false;
    if(hasAttribute(MaterialAttribute::OcclusionTexture) && !check(transformation, occlusionTextureMatrix()))
        return false;
    if(hasAttribute(MaterialAttribute::EmissiveTexture) && !check(transformation, emissiveTextureMatrix()))
        return false;

    return true;
}

bool PbrSpecularGlossinessMaterialData::hasTextureCoordinates() const {
    return hasAttribute(MaterialAttribute::TextureCoordinates) ||
        hasAttribute(MaterialAttribute::DiffuseTextureCoordinates) ||
        hasAttribute(MaterialAttribute::SpecularTextureCoordinates) ||
        hasAttribute(MaterialAttribute::GlossinessTextureCoordinates) ||
        hasAttribute(MaterialAttribute::NormalTextureCoordinates) ||
        hasAttribute(MaterialAttribute::OcclusionTextureCoordinates) ||
        hasAttribute(MaterialAttribute::EmissiveTextureCoordinates);
}

bool PbrSpecularGlossinessMaterialData::hasCommonTextureCoordinates() const {
    auto check = [](Containers::Optional<UnsignedInt>& coordinates, UnsignedInt current) {
        if(!coordinates) {
            coordinates = current;
            return true;
        }
        return coordinates == current;
    };

    Containers::Optional<UnsignedInt> coordinates;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::DiffuseTexture) && !check(coordinates, diffuseTextureCoordinates()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasSpecularTexture() && !check(coordinates, specularTextureCoordinates()))
        return false;
    if(hasGlossinessTexture() && !check(coordinates, glossinessTextureCoordinates()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(coordinates, normalTextureCoordinates()))
        return false;
    if(hasAttribute(MaterialAttribute::OcclusionTexture) && !check(coordinates, occlusionTextureCoordinates()))
        return false;
    if(hasAttribute(MaterialAttribute::EmissiveTexture) && !check(coordinates, emissiveTextureCoordinates()))
        return false;

    return true;
}

Color4 PbrSpecularGlossinessMaterialData::diffuseColor() const {
    return attributeOr(MaterialAttribute::DiffuseColor, 0xffffffff_srgbaf);
}

UnsignedInt PbrSpecularGlossinessMaterialData::diffuseTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::DiffuseTexture);
}

Matrix3 PbrSpecularGlossinessMaterialData::diffuseTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::DiffuseTexture),
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::DiffuseTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::DiffuseTexture),
        "Trade::PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Color4 PbrSpecularGlossinessMaterialData::specularColor() const {
    return attributeOr(MaterialAttribute::SpecularColor, 0xffffff00_srgbaf);
}

UnsignedInt PbrSpecularGlossinessMaterialData::specularTexture() const {
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularGlossinessTexture))
        return *value;
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularTexture))
        return *value;

    /* Explicit assertion because printing that SpecularTexture isn't found
       would be misleading as it can be also SpecularGlossinessTexture */
    CORRADE_ASSERT_UNREACHABLE("Trade::PbrSpecularGlossinessMaterialData::specularTexture(): the material doesn't have a specular texture", {});
}

MaterialTextureSwizzle PbrSpecularGlossinessMaterialData::specularTextureSwizzle() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture", {});
    if(hasAttribute(MaterialAttribute::SpecularGlossinessTexture))
        return MaterialTextureSwizzle::RGB;
    return attributeOr(MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PbrSpecularGlossinessMaterialData::specularTextureMatrix() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureMatrix(): the material doesn't have a specular texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::SpecularTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::specularTextureCoordinates() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Float PbrSpecularGlossinessMaterialData::glossiness() const {
    return attributeOr(MaterialAttribute::Glossiness, 1.0f);
}

UnsignedInt PbrSpecularGlossinessMaterialData::glossinessTexture() const {
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularGlossinessTexture))
        return *value;
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::GlossinessTexture))
        return *value;

    /* Explicit assertion because printing that GlossinessTexture isn't found
       would be misleading as it can be also SpecularGlossinessTexture */
    CORRADE_ASSERT_UNREACHABLE("Trade::PbrSpecularGlossinessMaterialData::glossinessTexture(): the material doesn't have a glossiness texture", {});
}

MaterialTextureSwizzle PbrSpecularGlossinessMaterialData::glossinessTextureSwizzle() const {
    CORRADE_ASSERT(hasGlossinessTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureSwizzle(): the material doesn't have a glossiness texture", {});
    if(hasAttribute(MaterialAttribute::SpecularGlossinessTexture))
        return MaterialTextureSwizzle::A;
    return attributeOr(MaterialAttribute::GlossinessTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrSpecularGlossinessMaterialData::glossinessTextureMatrix() const {
    CORRADE_ASSERT(hasGlossinessTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureMatrix(): the material doesn't have a glossiness texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::GlossinessTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates() const {
    CORRADE_ASSERT(hasGlossinessTexture(),
        "Trade::PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates(): the material doesn't have a glossiness texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::GlossinessTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrSpecularGlossinessMaterialData::normalTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::NormalTexture);
}

Float PbrSpecularGlossinessMaterialData::normalTextureScale() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureScale(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureScale, 1.0f);
}

MaterialTextureSwizzle PbrSpecularGlossinessMaterialData::normalTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PbrSpecularGlossinessMaterialData::normalTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::NormalTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::normalTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrSpecularGlossinessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::NormalTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrSpecularGlossinessMaterialData::occlusionTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::OcclusionTexture);
}

Float PbrSpecularGlossinessMaterialData::occlusionTextureStrength() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture", {});
    return attributeOr(MaterialAttribute::OcclusionTextureStrength, 1.0f);
}

MaterialTextureSwizzle PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture", {});
    return attributeOr(MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrSpecularGlossinessMaterialData::occlusionTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::OcclusionTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::OcclusionTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Color3 PbrSpecularGlossinessMaterialData::emissiveColor() const {
    return attributeOr(MaterialAttribute::EmissiveColor, 0x000000_srgbf);
}

UnsignedInt PbrSpecularGlossinessMaterialData::emissiveTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::EmissiveTexture);
}

Matrix3 PbrSpecularGlossinessMaterialData::emissiveTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::EmissiveTexture),
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::EmissiveTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::EmissiveTexture),
        "Trade::PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::EmissiveTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Matrix3 PbrSpecularGlossinessMaterialData::commonTextureMatrix() const {
    CORRADE_ASSERT(hasCommonTextureTransformation(),
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation", {});
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        return diffuseTextureMatrix();
    if(hasSpecularTexture())
        return specularTextureMatrix();
    if(hasGlossinessTexture())
        return glossinessTextureMatrix();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureMatrix();
    if(hasAttribute(MaterialAttribute::OcclusionTexture))
        return occlusionTextureMatrix();
    if(hasAttribute(MaterialAttribute::EmissiveTexture))
        return emissiveTextureMatrix();
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrSpecularGlossinessMaterialData::commonTextureCoordinates() const {
    CORRADE_ASSERT(hasCommonTextureCoordinates(),
        "Trade::PbrSpecularGlossinessMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set", {});
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        return diffuseTextureCoordinates();
    if(hasSpecularTexture())
        return specularTextureCoordinates();
    if(hasGlossinessTexture())
        return glossinessTextureCoordinates();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureCoordinates();
    if(hasAttribute(MaterialAttribute::OcclusionTexture))
        return occlusionTextureCoordinates();
    if(hasAttribute(MaterialAttribute::EmissiveTexture))
        return emissiveTextureCoordinates();
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

}}
