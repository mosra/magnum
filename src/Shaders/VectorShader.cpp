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

#include "VectorShader.h"

#include <Utility/Resource.h>

#include "Context.h"
#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<UnsignedInt> constexpr const char* vertexShaderName();
    template<> constexpr const char* vertexShaderName<2>() { return "AbstractVectorShader2D.vert"; }
    template<> constexpr const char* vertexShaderName<3>() { return "AbstractVectorShader3D.vert"; }
}

template<UnsignedInt dimensions> VectorShader<dimensions>::VectorShader(): transformationProjectionMatrixUniform(0), colorUniform(1) {
    Corrade::Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    Version v = Context::current()->supportedVersion({Version::GL320, Version::GL210});
    #else
    Version v = Context::current()->supportedVersion({Version::GLES300, Version::GLES200});
    #endif

    Shader vertexShader(v, Shader::Type::Vertex);
    vertexShader.addSource(rs.get("compatibility.glsl"));
    vertexShader.addSource(rs.get(vertexShaderName<dimensions>()));
    AbstractVectorShader<dimensions>::attachShader(vertexShader);

    Shader fragmentShader(v, Shader::Type::Fragment);
    fragmentShader.addSource(rs.get("compatibility.glsl"));
    fragmentShader.addSource(rs.get("VectorShader.frag"));
    AbstractVectorShader<dimensions>::attachShader(fragmentShader);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>() ||
        Context::current()->version() == Version::GL210) {
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300)) {
    #endif
        AbstractVectorShader<dimensions>::bindAttributeLocation(AbstractVectorShader<dimensions>::Position::Location, "position");
        AbstractVectorShader<dimensions>::bindAttributeLocation(AbstractVectorShader<dimensions>::TextureCoordinates::Location, "textureCoordinates");
    }

    AbstractVectorShader<dimensions>::link();

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>()) {
    #else
    {
    #endif
        transformationProjectionMatrixUniform = AbstractVectorShader<dimensions>::uniformLocation("transformationProjectionMatrix");
        colorUniform = AbstractVectorShader<dimensions>::uniformLocation("color");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>())
        AbstractVectorShader<dimensions>::setUniform(AbstractVectorShader<dimensions>::uniformLocation("vectorTexture"), AbstractVectorShader<dimensions>::VectorTextureLayer);
    #endif
}

template class VectorShader<2>;
template class VectorShader<3>;

}}
