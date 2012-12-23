/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "TextShader.h"

#include <Utility/Resource.h>

#include "Context.h"
#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<std::uint8_t dimensions> struct ShaderName {};

    template<> struct ShaderName<2> {
        constexpr static const char* vertex() { return "TextShader2D.vert"; }
        constexpr static const char* fragment() { return "TextShader2D.frag"; }
    };

    template<> struct ShaderName<3> {
        constexpr static const char* vertex() { return "TextShader3D.vert"; }
        constexpr static const char* fragment() { return "TextShader3D.frag"; }
    };
}

template<std::uint8_t dimensions> TextShader<dimensions>::TextShader() {
    Corrade::Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    Version v = Context::current()->supportedVersion({Version::GL320, Version::GL210});
    #else
    Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vertexShader(v, Shader::Type::Vertex);
    vertexShader.addSource(rs.get("compatibility.glsl"));
    vertexShader.addSource(rs.get(ShaderName<dimensions>::vertex()));
    AbstractTextShader<dimensions>::attachShader(vertexShader);

    Shader fragmentShader(v, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get(ShaderName<dimensions>::fragment()));
    AbstractTextShader<dimensions>::attachShader(fragmentShader);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>()) {
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300)) {
    #endif
        AbstractTextShader<dimensions>::bindAttributeLocation(AbstractTextShader<dimensions>::Position::Location, "position");
        AbstractTextShader<dimensions>::bindAttributeLocation(AbstractTextShader<dimensions>::TextureCoordinates::Location, "textureCoordinates");
    }

    AbstractTextShader<dimensions>::link();

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>()) {
    #else
    {
    #endif
        transformationProjectionMatrixUniform = AbstractTextShader<dimensions>::uniformLocation("transformationProjectionMatrix");
        colorUniform = AbstractTextShader<dimensions>::uniformLocation("color");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>())
        AbstractTextShader<dimensions>::setUniform(AbstractTextShader<dimensions>::uniformLocation("fontTexture"), AbstractTextShader<dimensions>::FontTextureLayer);
    #endif
}

template class TextShader<2>;
template class TextShader<3>;

}}
