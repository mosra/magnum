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

#include "FlatShader.h"

#include <Utility/Resource.h>

#include "Shader.h"

namespace Magnum { namespace Shaders {

namespace {
    template<std::uint8_t dimensions> struct ShaderName {};

    template<> struct ShaderName<2> {
        constexpr static const char* Vertex = "FlatShader2D.vert";
        constexpr static const char* Fragment = "FlatShader2D.frag";
    };

    template<> struct ShaderName<3> {
        constexpr static const char* Vertex = "FlatShader3D.vert";
        constexpr static const char* Fragment = "FlatShader3D.frag";
    };
}

template<std::uint8_t dimensions> FlatShader<dimensions>::FlatShader() {
    Corrade::Utility::Resource resource("MagnumShaders");
    attachShader(Shader::fromData(Version::GL330, Shader::Type::Vertex, resource.get(ShaderName<dimensions>::Vertex)));
    attachShader(Shader::fromData(Version::GL330, Shader::Type::Fragment, resource.get(ShaderName<dimensions>::Fragment)));

    link();

    transformationProjectionUniform = uniformLocation("transformationProjection");
    colorUniform = uniformLocation("color");
}

template class FlatShader<2>;
template class FlatShader<3>;

}}
