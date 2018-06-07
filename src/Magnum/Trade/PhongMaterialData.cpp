/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

PhongMaterialData::PhongMaterialData(PhongMaterialData&& other) noexcept: AbstractMaterialData{std::move(other)}, _flags{other._flags}, _shininess{other._shininess} {
    if(_flags & Flag::AmbientTexture)
        _ambient.texture = other._ambient.texture;
    else
        _ambient.color = other._ambient.color;

    if(_flags & Flag::DiffuseTexture)
        _diffuse.texture = other._diffuse.texture;
    else
        _diffuse.color = other._diffuse.color;

    if(_flags & Flag::SpecularTexture)
        _specular.texture = other._specular.texture;
    else
        _specular.color = other._specular.color;
}

PhongMaterialData& PhongMaterialData::operator=(PhongMaterialData&& other) noexcept {
    AbstractMaterialData::operator=(std::move(other));

    _flags = other._flags;
    _shininess = other._shininess;

    if(_flags & Flag::AmbientTexture)
        _ambient.texture = other._ambient.texture;
    else
        _ambient.color = other._ambient.color;

    if(_flags & Flag::DiffuseTexture)
        _diffuse.texture = other._diffuse.texture;
    else
        _diffuse.color = other._diffuse.color;

    if(_flags & Flag::SpecularTexture)
        _specular.texture = other._specular.texture;
    else
        _specular.color = other._specular.color;

    return *this;
}

Color4& PhongMaterialData::ambientColor() {
    CORRADE_ASSERT(!(_flags & Flag::AmbientTexture), "Trade::PhongMaterialData::ambientColor(): the material has ambient texture", _ambient.color);
    return _ambient.color;
}

UnsignedInt& PhongMaterialData::ambientTexture() {
    CORRADE_ASSERT(_flags & Flag::AmbientTexture, "Trade::PhongMaterialData::ambientTexture(): the material doesn't have ambient texture", _ambient.texture);
    return _ambient.texture;
}

Color4& PhongMaterialData::diffuseColor() {
    CORRADE_ASSERT(!(_flags & Flag::DiffuseTexture), "Trade::PhongMaterialData::diffuseColor(): the material has diffuse texture", _diffuse.color);
    return _diffuse.color;
}

UnsignedInt& PhongMaterialData::diffuseTexture() {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture, "Trade::PhongMaterialData::diffuseTexture(): the material doesn't have diffuse texture", _diffuse.texture);
    return _diffuse.texture;
}

Color4& PhongMaterialData::specularColor() {
    CORRADE_ASSERT(!(_flags & Flag::SpecularTexture), "Trade::PhongMaterialData::specularColor(): the material has specular texture", _specular.color);
    return _specular.color;
}

UnsignedInt& PhongMaterialData::specularTexture() {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture, "Trade::PhongMaterialData::specularTexture(): the material doesn't have specular texture", _specular.texture);
    return _specular.texture;
}

Debug& operator<<(Debug& debug, const PhongMaterialData::Flag value) {
    switch(value) {
        #define _c(v) case PhongMaterialData::Flag::v: return debug << "Trade::PhongMaterialData::Flag::" #v;
        _c(AmbientTexture)
        _c(DiffuseTexture)
        _c(SpecularTexture)
        #undef _c
    }

    return debug << "Trade::PhongMaterialData::Flag(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PhongMaterialData::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Trade::PhongMaterialData::Flags{}", {
        PhongMaterialData::Flag::AmbientTexture,
        PhongMaterialData::Flag::DiffuseTexture,
        PhongMaterialData::Flag::SpecularTexture});
}

}}
