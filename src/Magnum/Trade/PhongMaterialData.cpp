/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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

#include "PhongMaterialData.h"

#include <Corrade/Containers/EnumSet.hpp>
#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/GrowableArray.h>
#endif

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace Magnum { namespace Trade {

using namespace Math::Literals;

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const UnsignedInt ambientTextureCoordinates, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const UnsignedInt diffuseTextureCoordinates, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt specularTextureCoordinates, const UnsignedInt normalTexture, const UnsignedInt normalTextureCoordinates, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: MaterialData{MaterialType::Phong, [&](){
    Containers::Array<MaterialAttributeData> data;

    if(flags & Flag::DoubleSided)
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::DoubleSided, true);
    if(alphaMode == MaterialAlphaMode::Blend)
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::AlphaBlend, true);
    /* Include a mask also if it has a non-default value to stay compatible
       with existing behavior */
    if(alphaMode == MaterialAlphaMode::Mask || alphaMask != 0.0f)
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::AlphaMask, alphaMask);

    CORRADE_ASSERT(!(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures", data);
    CORRADE_ASSERT((flags & Flag::TextureTransformation) || textureMatrix == Matrix3{},
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled", data);
    CORRADE_ASSERT((flags & Flag::TextureCoordinateSets) || (ambientTextureCoordinates == 0 && diffuseTextureCoordinates == 0 && specularTextureCoordinates == 0 && normalTextureCoordinates == 0),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinates to be enabled", data);

    arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::AmbientColor, ambientColor);
    if(flags & Flag::AmbientTexture) {
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::AmbientTexture, ambientTexture);
        if(ambientTextureCoordinates)
            arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::AmbientTextureCoordinates, ambientTextureCoordinates);
    }

    arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::DiffuseColor, diffuseColor);
    if(flags & Flag::DiffuseTexture) {
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::DiffuseTexture, diffuseTexture);
        if(diffuseTextureCoordinates)
            arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::DiffuseTextureCoordinates, diffuseTextureCoordinates);
    }

    arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::SpecularColor, specularColor);
    if(flags & Flag::SpecularTexture) {
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::SpecularTexture, specularTexture);
        if(specularTextureCoordinates)
            arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::SpecularTextureCoordinates, specularTextureCoordinates);
    }

    if(flags & Flag::NormalTexture) {
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::NormalTexture, normalTexture);
        if(normalTextureCoordinates)
            arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::NormalTextureCoordinates, normalTextureCoordinates);
    }

    if(flags & Flag::TextureTransformation)
        arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::TextureMatrix, textureMatrix);

    arrayAppend(data, Containers::InPlaceInit, MaterialAttribute::Shininess, shininess);

    /* Convert back to a non-growable Array as importers don't allow custom
       deleters in plugin implementations */
    arrayShrink(data, Containers::DefaultInit);
    return data;
}(), importerState} {
    /* The data can't be filled here because it won't be sorted correctly */
}

PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt normalTexture, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, ambientColor, ambientTexture, 0, diffuseColor, diffuseTexture, 0, specularColor, specularTexture, 0, normalTexture, 0, textureMatrix, alphaMode, alphaMask, shininess, importerState} {}

PhongMaterialData::PhongMaterialData(const Flags flags, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, 0x000000ff_rgbaf, {}, 0xffffffff_rgbaf, {}, 0xffffffff_rgbaf, {}, {}, {}, alphaMode, alphaMask, shininess, importerState} {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
PhongMaterialData::Flags PhongMaterialData::flags() const {
    /* "Append" to flags recognized by the base class */
    Flags flags{Flag(UnsignedInt(MaterialData::flags()))};

    if(hasAttribute(MaterialAttribute::AmbientTexture))
        flags |= Flag::AmbientTexture;
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        flags |= Flag::DiffuseTexture;
    if(hasSpecularTexture())
        flags |= Flag::SpecularTexture;
    if(hasAttribute(MaterialAttribute::NormalTexture))
        flags |= Flag::NormalTexture;
    if(hasTextureTransformation())
        flags |= Flag::TextureTransformation;
    if(hasTextureCoordinates())
        flags |= Flag::TextureCoordinates;

    return flags;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

bool PhongMaterialData::hasSpecularTexture() const {
    return hasAttribute(MaterialAttribute::SpecularTexture) ||
        hasAttribute(MaterialAttribute::SpecularGlossinessTexture);
}

bool PhongMaterialData::hasTextureTransformation() const {
    return hasAttribute(MaterialAttribute::AmbientTextureMatrix) ||
        hasAttribute(MaterialAttribute::DiffuseTextureMatrix) ||
        hasAttribute(MaterialAttribute::SpecularTextureMatrix) ||
        hasAttribute(MaterialAttribute::NormalTextureMatrix) ||
        hasAttribute(MaterialAttribute::TextureMatrix);
}

bool PhongMaterialData::hasCommonTextureTransformation() const {
    auto check = [](Containers::Optional<Matrix3>& transformation, Matrix3 current) {
        if(!transformation) {
            transformation = current;
            return true;
        }
        return transformation == current;
    };

    Containers::Optional<Matrix3> transformation;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::AmbientTexture) && !check(transformation, ambientTextureMatrix()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasAttribute(MaterialAttribute::DiffuseTexture) && !check(transformation, diffuseTextureMatrix()))
        return false;
    if(hasSpecularTexture() && !check(transformation, specularTextureMatrix()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(transformation, normalTextureMatrix()))
        return false;

    return true;
}

bool PhongMaterialData::hasTextureCoordinates() const {
    return attributeOr(MaterialAttribute::AmbientTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::DiffuseTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::SpecularTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::NormalTextureCoordinates, 0u) ||
        attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

bool PhongMaterialData::hasCommonTextureCoordinates() const {
    auto check = [](Containers::Optional<UnsignedInt>& coordinates, UnsignedInt current) {
        if(!coordinates) {
            coordinates = current;
            return true;
        }
        return coordinates == current;
    };

    Containers::Optional<UnsignedInt> coordinates;
    /* First one can't fail */
    if(hasAttribute(MaterialAttribute::AmbientTexture) && !check(coordinates, ambientTextureCoordinates()))
        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    if(hasAttribute(MaterialAttribute::DiffuseTexture) && !check(coordinates, diffuseTextureCoordinates()))
        return false;
    if(hasSpecularTexture() && !check(coordinates, specularTextureCoordinates()))
        return false;
    if(hasAttribute(MaterialAttribute::NormalTexture) && !check(coordinates, normalTextureCoordinates()))
        return false;

    return true;
}

Color4 PhongMaterialData::ambientColor() const {
    return attributeOr(MaterialAttribute::AmbientColor,
        hasAttribute(MaterialAttribute::AmbientTexture) ? 0xffffffff_rgbaf : 0x000000ff_rgbaf);
}

UnsignedInt PhongMaterialData::ambientTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::AmbientTexture);
}

Matrix3 PhongMaterialData::ambientTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::AmbientTexture),
        "Trade::PhongMaterialData::ambientTextureMatrix(): the material doesn't have an ambient texture", {});
    if(Containers::Optional<Matrix3> set = tryAttribute<Matrix3>(MaterialAttribute::AmbientTextureMatrix))
        return *set;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PhongMaterialData::ambientTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::AmbientTexture),
        "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture", {});
    if(Containers::Optional<UnsignedInt> set = tryAttribute<UnsignedInt>(MaterialAttribute::AmbientTextureCoordinates))
        return *set;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Color4 PhongMaterialData::diffuseColor() const {
    return attributeOr(MaterialAttribute::DiffuseColor, 0xffffffff_rgbaf);
}

UnsignedInt PhongMaterialData::diffuseTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::DiffuseTexture);
}

Matrix3 PhongMaterialData::diffuseTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::DiffuseTexture),
        "Trade::PhongMaterialData::diffuseTextureMatrix(): the material doesn't have a diffuse texture", {});
    if(Containers::Optional<Matrix3> set = tryAttribute<Matrix3>(MaterialAttribute::DiffuseTextureMatrix))
        return *set;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PhongMaterialData::diffuseTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::DiffuseTexture),
        "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture", {});
    if(Containers::Optional<UnsignedInt> set = tryAttribute<UnsignedInt>(MaterialAttribute::DiffuseTextureCoordinates))
        return *set;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Color4 PhongMaterialData::specularColor() const {
    return attributeOr(MaterialAttribute::SpecularColor, 0xffffff00_rgbaf);
}

UnsignedInt PhongMaterialData::specularTexture() const {
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularGlossinessTexture))
        return *value;
    if(Containers::Optional<UnsignedInt> value = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularTexture))
        return *value;

    /* Explicit assertion because printing that SpecularTexture isn't found
       would be misleading as it can be also SpecularGlossinessTexture */
    CORRADE_ASSERT_UNREACHABLE("Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture", {});
}

MaterialTextureSwizzle PhongMaterialData::specularTextureSwizzle() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PhongMaterialData::specularTextureSwizzle(): the material doesn't have a specular texture", {});
    if(hasAttribute(MaterialAttribute::SpecularGlossinessTexture))
        return MaterialTextureSwizzle::RGB;
    return attributeOr(MaterialAttribute::SpecularTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PhongMaterialData::specularTextureMatrix() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PhongMaterialData::specularTextureMatrix(): the material doesn't have a specular texture", {});
    if(Containers::Optional<Matrix3> set = tryAttribute<Matrix3>(MaterialAttribute::SpecularTextureMatrix))
        return *set;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PhongMaterialData::specularTextureCoordinates() const {
    CORRADE_ASSERT(hasSpecularTexture(),
        "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture", {});
    if(Containers::Optional<UnsignedInt> set = tryAttribute<UnsignedInt>(MaterialAttribute::SpecularTextureCoordinates))
        return *set;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

UnsignedInt PhongMaterialData::normalTexture() const {
    return attribute<UnsignedInt>(MaterialAttribute::NormalTexture);
}

Float PhongMaterialData::normalTextureScale() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PhongMaterialData::normalTextureScale(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureScale, 1.0f);
}

MaterialTextureSwizzle PhongMaterialData::normalTextureSwizzle() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PhongMaterialData::normalTextureSwizzle(): the material doesn't have a normal texture", {});
    return attributeOr(MaterialAttribute::NormalTextureSwizzle, MaterialTextureSwizzle::RGB);
}

Matrix3 PhongMaterialData::normalTextureMatrix() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PhongMaterialData::normalTextureMatrix(): the material doesn't have a normal texture", {});
    if(Containers::Optional<Matrix3> set = tryAttribute<Matrix3>(MaterialAttribute::NormalTextureMatrix))
        return *set;
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

UnsignedInt PhongMaterialData::normalTextureCoordinates() const {
    CORRADE_ASSERT(hasAttribute(MaterialAttribute::NormalTexture),
        "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture", {});
    if(Containers::Optional<UnsignedInt> set = tryAttribute<UnsignedInt>(MaterialAttribute::NormalTextureCoordinates))
        return *set;
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Matrix3 PhongMaterialData::commonTextureMatrix() const {
    CORRADE_ASSERT(hasCommonTextureTransformation(),
        "Trade::PhongMaterialData::commonTextureMatrix(): the material doesn't have a common texture coordinate transformation", {});
    if(hasAttribute(MaterialAttribute::AmbientTexture))
        return ambientTextureMatrix();
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        return diffuseTextureMatrix();
    if(hasSpecularTexture())
        return specularTextureMatrix();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureMatrix();
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}

#ifdef MAGNUM_BUILD_DEPRECATED
Matrix3 PhongMaterialData::textureMatrix() const {
    if(hasCommonTextureTransformation()) return commonTextureMatrix();
    return attributeOr(MaterialAttribute::TextureMatrix, Matrix3{});
}
#endif

UnsignedInt PhongMaterialData::commonTextureCoordinates() const {
    CORRADE_ASSERT(hasCommonTextureCoordinates(),
        "Trade::PhongMaterialData::commonTextureCoordinates(): the material doesn't have a common texture coordinate set", {});
    if(hasAttribute(MaterialAttribute::AmbientTexture))
        return ambientTextureCoordinates();
    if(hasAttribute(MaterialAttribute::DiffuseTexture))
        return diffuseTextureCoordinates();
    if(hasSpecularTexture())
        return specularTextureCoordinates();
    if(hasAttribute(MaterialAttribute::NormalTexture))
        return normalTextureCoordinates();
    return attributeOr(MaterialAttribute::TextureCoordinates, 0u);
}

Float PhongMaterialData::shininess() const {
    return attributeOr(MaterialAttribute::Shininess, 80.0f);
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Debug& operator<<(Debug& debug, const PhongMaterialData::Flag value) {
    debug << "Trade::PhongMaterialData::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case PhongMaterialData::Flag::v: return debug << "::" #v;
        _c(DoubleSided)
        _c(AmbientTexture)
        _c(DiffuseTexture)
        _c(SpecularTexture)
        _c(NormalTexture)
        _c(TextureTransformation)
        _c(TextureCoordinates)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PhongMaterialData::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::PhongMaterialData::Flags{}", {
        PhongMaterialData::Flag::DoubleSided,
        PhongMaterialData::Flag::AmbientTexture,
        PhongMaterialData::Flag::DiffuseTexture,
        PhongMaterialData::Flag::SpecularTexture,
        PhongMaterialData::Flag::NormalTexture,
        PhongMaterialData::Flag::TextureTransformation,
        PhongMaterialData::Flag::TextureCoordinates});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}
