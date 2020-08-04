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

namespace Magnum { namespace Trade {

using namespace Math::Literals;

PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const UnsignedInt ambientTextureCoordinates, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const UnsignedInt diffuseTextureCoordinates, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt specularTextureCoordinates, const UnsignedInt normalTexture, const UnsignedInt normalTextureCoordinates, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: AbstractMaterialData{MaterialType::Phong, AbstractMaterialData::Flag(UnsignedShort(flags)), alphaMode, alphaMask, importerState}, _ambientColor{ambientColor}, _diffuseColor{diffuseColor}, _specularColor{specularColor}, _shininess{shininess} {
    CORRADE_ASSERT(!(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures", );
    CORRADE_ASSERT((flags & Flag::TextureTransformation) || textureMatrix == Matrix3{},
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled", );
    CORRADE_ASSERT((flags & Flag::TextureCoordinates) || (ambientTextureCoordinates == 0 && diffuseTextureCoordinates == 0 && specularTextureCoordinates == 0 && normalTextureCoordinates == 0),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinates to be enabled", );

    if(flags & Flag::AmbientTexture) {
        _ambientTexture = ambientTexture;
        _ambientTextureCoordinates = ambientTextureCoordinates;
    }
    if(flags & Flag::DiffuseTexture) {
        _diffuseTexture = diffuseTexture;
        _diffuseTextureCoordinates = diffuseTextureCoordinates;
    }
    if(flags & Flag::SpecularTexture) {
        _specularTexture = specularTexture;
        _specularTextureCoordinates = specularTextureCoordinates;
    }
    if(flags & Flag::NormalTexture) {
        _normalTexture = normalTexture;
        _normalTextureCoordinates = normalTextureCoordinates;
    }
    if(flags & Flag::TextureTransformation)
        _textureMatrix = textureMatrix;
}

PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt normalTexture, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, ambientColor, ambientTexture, 0, diffuseColor, diffuseTexture, 0, specularColor, specularTexture, 0, normalTexture, 0, textureMatrix, alphaMode, alphaMask, shininess, importerState} {}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongMaterialData::PhongMaterialData(const Flags flags, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, 0x000000ff_rgbaf, {}, 0xffffffff_rgbaf, {}, 0xffffffff_rgbaf, {}, {}, {}, alphaMode, alphaMask, shininess, importerState} {}
#endif

PhongMaterialData::PhongMaterialData(PhongMaterialData&& other) noexcept = default;

PhongMaterialData& PhongMaterialData::operator=(PhongMaterialData&& other) noexcept = default;

UnsignedInt PhongMaterialData::ambientTexture() const {
    CORRADE_ASSERT(flags() & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientTexture(): the material doesn't have an ambient texture", {});
    return _ambientTexture;
}


UnsignedInt PhongMaterialData::ambientTextureCoordinates() const {
    CORRADE_ASSERT(flags() & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientTextureCoordinates(): the material doesn't have an ambient texture", {});
    return _ambientTextureCoordinates;
}

UnsignedInt PhongMaterialData::diffuseTexture() const {
    CORRADE_ASSERT(flags() & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have a diffuse texture", {});
    return _diffuseTexture;
}


UnsignedInt PhongMaterialData::diffuseTextureCoordinates() const {
    CORRADE_ASSERT(flags() & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseTextureCoordinates(): the material doesn't have a diffuse texture", {});
    return _diffuseTextureCoordinates;
}

UnsignedInt PhongMaterialData::specularTexture() const {
    CORRADE_ASSERT(flags() & Flag::SpecularTexture, "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture", {});
    return _specularTexture;
}


UnsignedInt PhongMaterialData::specularTextureCoordinates() const {
    CORRADE_ASSERT(flags() & Flag::SpecularTexture, "Trade::PhongMaterialData::specularTextureCoordinates(): the material doesn't have a specular texture", {});
    return _specularTextureCoordinates;
}

UnsignedInt PhongMaterialData::normalTexture() const {
    CORRADE_ASSERT(flags() & Flag::NormalTexture, "Trade::PhongMaterialData::normalTexture(): the material doesn't have a normal texture", {});
    return _normalTexture;
}

UnsignedInt PhongMaterialData::normalTextureCoordinates() const {
    CORRADE_ASSERT(flags() & Flag::NormalTexture, "Trade::PhongMaterialData::normalTextureCoordinates(): the material doesn't have a normal texture", {});
    return _normalTextureCoordinates;
}

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

}}
