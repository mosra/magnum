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

#include "PbrMetallicRoughnessMaterialData.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace Magnum { namespace Trade {

using namespace Math::Literals;

bool PbrMetallicRoughnessMaterialData::hasMetalnessTexture() const {
    return hasAttribute(MaterialAttribute::MetalnessTexture) ||
           hasAttribute(MaterialAttribute::NoneRoughnessMetallicTexture);
}

bool PbrMetallicRoughnessMaterialData::hasRoughnessTexture() const {
    return hasAttribute(MaterialAttribute::RoughnessTexture) ||
           hasAttribute(MaterialAttribute::NoneRoughnessMetallicTexture);
}

bool PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture() const {
    return (hasAttribute(MaterialAttribute::NoneRoughnessMetallicTexture) ||
       (hasAttribute(MaterialAttribute::MetalnessTexture) &&
        hasAttribute(MaterialAttribute::RoughnessTexture) &&
        roughnessTextureSwizzle() == MaterialTextureSwizzle::G &&
        metalnessTextureSwizzle() == MaterialTextureSwizzle::B)) &&
       roughnessTextureMatrix() == metalnessTextureMatrix() &&
       roughnessTextureCoordinates() == metalnessTextureCoordinates();
}

bool PbrMetallicRoughnessMaterialData::hasRoughnessMetallicOcclusionTexture() const {
    if(!(hasAttribute(MaterialAttribute::RoughnessTexture) &&
         hasAttribute(MaterialAttribute::MetalnessTexture) &&
         hasAttribute(MaterialAttribute::OcclusionTexture) &&
       roughnessTextureSwizzle() == MaterialTextureSwizzle::R &&
       metalnessTextureSwizzle() == MaterialTextureSwizzle::G &&
       occlusionTextureSwizzle() == MaterialTextureSwizzle::B))
        return false;

    const Matrix3 roughnessTextureMatrix = this->roughnessTextureMatrix();
    const UnsignedInt roughnessTextureCoordinates = this->roughnessTextureCoordinates();
    return metalnessTextureMatrix() == roughnessTextureMatrix &&
        occlusionTextureMatrix() == roughnessTextureMatrix &&
        metalnessTextureCoordinates() == roughnessTextureCoordinates &&
        occlusionTextureCoordinates() == roughnessTextureCoordinates;
}

bool PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture() const {
    if(!(hasAttribute(MaterialAttribute::OcclusionTexture) &&
         hasAttribute(MaterialAttribute::RoughnessTexture) &&
         hasAttribute(MaterialAttribute::MetalnessTexture) &&
       occlusionTextureSwizzle() == MaterialTextureSwizzle::R &&
       roughnessTextureSwizzle() == MaterialTextureSwizzle::G &&
       metalnessTextureSwizzle() == MaterialTextureSwizzle::B))
        return false;

    const Matrix3 occlusionTextureMatrix = this->occlusionTextureMatrix();
    const UnsignedInt occlusionTextureCoordinates = this->occlusionTextureCoordinates();
    return roughnessTextureMatrix() == occlusionTextureMatrix &&
        metalnessTextureMatrix() == occlusionTextureMatrix &&
        roughnessTextureCoordinates() == occlusionTextureCoordinates &&
        metalnessTextureCoordinates() == occlusionTextureCoordinates;
}

bool PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture() const {
    if(!(hasAttribute(MaterialAttribute::NormalTexture) &&
         hasAttribute(MaterialAttribute::RoughnessTexture) &&
         hasAttribute(MaterialAttribute::MetalnessTexture) &&
       normalTextureSwizzle() == MaterialTextureSwizzle::RG &&
       roughnessTextureSwizzle() == MaterialTextureSwizzle::B &&
       metalnessTextureSwizzle() == MaterialTextureSwizzle::A))
        return false;

    const Matrix3 normalTextureMatrix = this->normalTextureMatrix();
    const UnsignedInt normalTextureCoordinates = this->normalTextureCoordinates();
    return roughnessTextureMatrix() == normalTextureMatrix &&
        metalnessTextureMatrix() == normalTextureMatrix &&
        roughnessTextureCoordinates() == normalTextureCoordinates &&
        metalnessTextureCoordinates() == normalTextureCoordinates;
}

bool PbrMetallicRoughnessMaterialData::hasTextureTransformation() const {
    return hasAttribute(MaterialAttribute::TextureMatrix) ||
        hasAttribute(MaterialAttribute::BaseColorTextureMatrix) ||
        hasAttribute(MaterialAttribute::MetalnessTextureMatrix) ||
        hasAttribute(MaterialAttribute::RoughnessTextureMatrix) ||
        hasAttribute(MaterialAttribute::NormalTextureMatrix) ||
        hasAttribute(MaterialAttribute::OcclusionTextureMatrix) ||
        hasAttribute(MaterialAttribute::EmissiveTextureMatrix);;
}

bool PbrMetallicRoughnessMaterialData::hasTextureCoordinates() const {
    return hasAttribute(MaterialAttribute::TextureCoordinates) ||
        hasAttribute(MaterialAttribute::BaseColorTextureCoordinates) ||
        hasAttribute(MaterialAttribute::MetalnessTextureCoordinates) ||
        hasAttribute(MaterialAttribute::RoughnessTextureCoordinates) ||
        hasAttribute(MaterialAttribute::NormalTextureCoordinates) ||
        hasAttribute(MaterialAttribute::OcclusionTextureCoordinates) ||
        hasAttribute(MaterialAttribute::EmissiveTextureCoordinates);
}

Color4 PbrMetallicRoughnessMaterialData::baseColor() const {
    return attributeOr(MaterialAttribute::BaseColor, 0xffffffff_rgbaf);
}

UnsignedInt PbrMetallicRoughnessMaterialData::baseColorTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::BaseColorTexture);
}

Matrix3 PbrMetallicRoughnessMaterialData::baseColorTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::BaseColorTexture),
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureMatrix(): the material doesn't have a base color texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::BaseColorTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::BaseColorTexture),
        "Trade::PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates(): the material doesn't have a base color texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::BaseColorTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Float PbrMetallicRoughnessMaterialData::metalness() const {
    return attributeOr(MaterialAttribute::Metalness, 1.0f);
}

UnsignedInt PbrMetallicRoughnessMaterialData::metalnessTexture() const {
    /* Explicit assertion because printing  that MetalnessTexture isn't found
       would be misleading as it can be also MetallicRoughnessTexture */
    CORRADE_ASSERT(hasMetalnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTexture(): the material doesn't have a metalness texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::NoneRoughnessMetallicTexture))
        return *value;
    return attribute<UnsignedInt>(MaterialAttribute::MetalnessTexture);
}

MaterialTextureSwizzle PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle() const {
    CORRADE_ASSERT(hasMetalnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle(): the material doesn't have a metalness texture", {});
    if(hasAttribute(MaterialAttribute::NoneRoughnessMetallicTexture))
        return MaterialTextureSwizzle::B;
    return attributeOr(MaterialAttribute::MetalnessTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrMetallicRoughnessMaterialData::metalnessTextureMatrix() const {
    CORRADE_ASSERT(hasMetalnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureMatrix(): the material doesn't have a metalness texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::MetalnessTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates() const {
    CORRADE_ASSERT(hasMetalnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates(): the material doesn't have a metalness texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::MetalnessTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Float PbrMetallicRoughnessMaterialData::roughness() const {
    return attributeOr(MaterialAttribute::Roughness, 1.0f);
}

UnsignedInt PbrMetallicRoughnessMaterialData::roughnessTexture() const {
    /* Explicit assertion because printing that RoughnessTexture isn't found
       would be misleading as it can be also MetallicRoughnessTexture */
    CORRADE_ASSERT(hasRoughnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTexture(): the material doesn't have a roughness texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::NoneRoughnessMetallicTexture))
        return *value;
    return attribute<UnsignedInt>(MaterialAttribute::RoughnessTexture);
}

MaterialTextureSwizzle PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle() const {
    CORRADE_ASSERT(hasRoughnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle(): the material doesn't have a roughness texture", {});
    if(hasAttribute(MaterialAttribute::NoneRoughnessMetallicTexture))
        return MaterialTextureSwizzle::G;
    return attributeOr(MaterialAttribute::RoughnessTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrMetallicRoughnessMaterialData::roughnessTextureMatrix() const {
    CORRADE_ASSERT(hasRoughnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureMatrix(): the material doesn't have a roughness texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::RoughnessTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates() const {
    CORRADE_ASSERT(hasRoughnessTexture(),
        "Trade::PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates(): the material doesn't have a roughness texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::RoughnessTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrMetallicRoughnessMaterialData::normalTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::NormalTexture);
}

Float PbrMetallicRoughnessMaterialData::normalTextureScale() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureScale(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureScale, 1.0f);
}

MaterialTextureSwizzle PbrMetallicRoughnessMaterialData::normalTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PbrMetallicRoughnessMaterialData::normalTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureMatrix(): the material doesn't have a normal texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::NormalTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::normalTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PbrMetallicRoughnessMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::NormalTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PbrMetallicRoughnessMaterialData::occlusionTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::OcclusionTexture);
}

Float PbrMetallicRoughnessMaterialData::occlusionTextureStrength() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureStrength(): the material doesn't have an occlusion texture", {});
    return attributeOr(MaterialAttribute::OcclusionTextureStrength, 1.0f);
}

MaterialTextureSwizzle PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle(): the material doesn't have an occlusion texture", {});
    return attributeOr(MaterialAttribute::OcclusionTextureSwizzle, MaterialTextureSwizzle::R);
}

Matrix3 PbrMetallicRoughnessMaterialData::occlusionTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureMatrix(): the material doesn't have an occlusion texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::OcclusionTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::OcclusionTexture),
        "Trade::PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(): the material doesn't have an occlusion texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::OcclusionTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Color3 PbrMetallicRoughnessMaterialData::emissiveColor() const {
    return attributeOr(MaterialAttribute::EmissiveColor, 0x000000_srgbf);
}

UnsignedInt PbrMetallicRoughnessMaterialData::emissiveTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::EmissiveTexture);
}

Matrix3 PbrMetallicRoughnessMaterialData::emissiveTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::EmissiveTexture),
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureMatrix(): the material doesn't have an emissive texture", {});
    if(Containers::Optional<Matrix3> value = tryAttribute<Matrix3>(MaterialAttribute::EmissiveTextureMatrix))
        return *value;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::EmissiveTexture),
        "Trade::PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(): the material doesn't have an emissive texture", {});
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::EmissiveTextureCoordinates))
        return *value;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Matrix3 PbrMetallicRoughnessMaterialData::textureMatrix() const {
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PbrMetallicRoughnessMaterialData::textureCoordinates() const {
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

}}
