/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "PhongMaterialData.h"

#include <Corrade/Containers/EnumSet.hpp>

namespace Magnum { namespace Trade {

using namespace Math::Literals;

PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const UnsignedInt ambientCoordinateSet, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const UnsignedInt diffuseCoordinateSet, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt specularCoordinateSet, const UnsignedInt normalTexture, const UnsignedInt normalCoordinateSet, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: AbstractMaterialData{MaterialType::Phong, AbstractMaterialData::Flag(UnsignedShort(flags)), alphaMode, alphaMask, importerState}, _ambientColor{ambientColor}, _diffuseColor{diffuseColor}, _specularColor{specularColor}, _shininess{shininess} {
    CORRADE_ASSERT(!(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Trade::PhongMaterialData: texture transformation enabled but the material has no textures", );
    CORRADE_ASSERT((flags & Flag::TextureTransformation) || textureMatrix == Matrix3{},
        "PhongMaterialData::PhongMaterialData: non-default texture matrix requires Flag::TextureTransformation to be enabled", );
    CORRADE_ASSERT((flags & Flag::TextureCoordinateSets) || (ambientCoordinateSet == 0 && diffuseCoordinateSet == 0 && specularCoordinateSet == 0 && normalCoordinateSet == 0),
        "PhongMaterialData::PhongMaterialData: non-zero texture coordinate sets require Flag::TextureCoordinateSets to be enabled", );

    if(flags & Flag::AmbientTexture) {
        _ambientTexture = ambientTexture;
        _ambientCoordinateSet = ambientCoordinateSet;
    }
    if(flags & Flag::DiffuseTexture) {
        _diffuseTexture = diffuseTexture;
        _diffuseCoordinateSet = diffuseCoordinateSet;
    }
    if(flags & Flag::SpecularTexture) {
        _specularTexture = specularTexture;
        _specularCoordinateSet = specularCoordinateSet;
    }
    if(flags & Flag::NormalTexture) {
        _normalTexture = normalTexture;
        _normalCoordinateSet = normalCoordinateSet;
    }
    if(flags & Flag::TextureTransformation)
        _textureMatrix = textureMatrix;
}

PhongMaterialData::PhongMaterialData(const Flags flags, const Color4& ambientColor, const UnsignedInt ambientTexture, const Color4& diffuseColor, const UnsignedInt diffuseTexture, const Color4& specularColor, const UnsignedInt specularTexture, const UnsignedInt normalTexture, const Matrix3& textureMatrix, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, ambientColor, ambientTexture, 0, diffuseColor, diffuseTexture, 0, specularColor, specularTexture, 0, normalTexture, 0, textureMatrix, alphaMode, alphaMask, shininess, importerState} {}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongMaterialData::PhongMaterialData(const Flags flags, const MaterialAlphaMode alphaMode, const Float alphaMask, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, 0x000000ff_rgbaf, {}, 0xffffffff_rgbaf, {}, 0xffffffff_rgbaf, {}, {}, {}, alphaMode, alphaMask, shininess, importerState} {}

PhongMaterialData::PhongMaterialData(const Flags flags, const Float shininess, const void* const importerState) noexcept: PhongMaterialData{flags, 0x000000ff_rgbaf, {}, 0xffffffff_rgbaf, {}, 0xffffffff_rgbaf, {}, {}, {}, MaterialAlphaMode::Opaque, 0.5f, shininess, importerState} {}
#endif

PhongMaterialData::PhongMaterialData(PhongMaterialData&& other) noexcept = default;

PhongMaterialData& PhongMaterialData::operator=(PhongMaterialData&& other) noexcept = default;

UnsignedInt PhongMaterialData::ambientTexture() const {
    CORRADE_ASSERT(flags() & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientTexture(): the material doesn't have an ambient texture", {});
    return _ambientTexture;
}

#ifdef MAGNUM_BUILD_DEPRECATED
UnsignedInt& PhongMaterialData::ambientTexture() {
    CORRADE_ASSERT(flags() & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientTexture(): the material doesn't have an ambient texture", _ambientTexture);
    return _ambientTexture;
}
#endif

UnsignedInt PhongMaterialData::ambientCoordinateSet() const {
    CORRADE_ASSERT(flags() & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientCoordinateSet(): the material doesn't have an ambient texture", {});
    return _ambientCoordinateSet;
}

UnsignedInt PhongMaterialData::diffuseTexture() const {
    CORRADE_ASSERT(flags() & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have a diffuse texture", {});
    return _diffuseTexture;
}

#ifdef MAGNUM_BUILD_DEPRECATED
UnsignedInt& PhongMaterialData::diffuseTexture() {
    CORRADE_ASSERT(flags() & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have a diffuse texture", _diffuseTexture);
    return _diffuseTexture;
}
#endif

UnsignedInt PhongMaterialData::diffuseCoordinateSet() const {
    CORRADE_ASSERT(flags() & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseCoordinateSet(): the material doesn't have a diffuse texture", {});
    return _diffuseCoordinateSet;
}

UnsignedInt PhongMaterialData::specularTexture() const {
    CORRADE_ASSERT(flags() & Flag::SpecularTexture, "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture", {});
    return _specularTexture;
}

#ifdef MAGNUM_BUILD_DEPRECATED
UnsignedInt& PhongMaterialData::specularTexture() {
    CORRADE_ASSERT(flags() & Flag::SpecularTexture, "Trade::PhongMaterialData::specularTexture(): the material doesn't have a specular texture", _specularTexture);
    return _specularTexture;
}
#endif

UnsignedInt PhongMaterialData::specularCoordinateSet() const {
    CORRADE_ASSERT(flags() & Flag::SpecularTexture, "Trade::PhongMaterialData::specularCoordinateSet(): the material doesn't have a specular texture", {});
    return _specularCoordinateSet;
}

UnsignedInt PhongMaterialData::normalTexture() const {
    CORRADE_ASSERT(flags() & Flag::NormalTexture, "Trade::PhongMaterialData::normalTexture(): the material doesn't have a normal texture", {});
    return _normalTexture;
}

UnsignedInt PhongMaterialData::normalCoordinateSet() const {
    CORRADE_ASSERT(flags() & Flag::NormalTexture, "Trade::PhongMaterialData::normalCoordinateSet(): the material doesn't have a normal texture", {});
    return _normalCoordinateSet;
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
        _c(TextureCoordinateSets)
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
        PhongMaterialData::Flag::TextureCoordinateSets});
}

}}
