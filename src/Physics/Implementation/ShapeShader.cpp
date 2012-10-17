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

#include "ShapeShader.h"

#include <Utility/Resource.h>

#include "Shader.h"

namespace Magnum { namespace Physics { namespace Implementation {

namespace {
    template<std::uint8_t dimensions> struct ShaderName {};

    template<> struct ShaderName<2> {
        constexpr static const char* Vertex = "ShapeShader2D.vert";
        constexpr static const char* Fragment = "ShapeShader2D.frag";
    };

    template<> struct ShaderName<3> {
        constexpr static const char* Vertex = "ShapeShader3D.vert";
        constexpr static const char* Fragment = "ShapeShader3D.frag";
    };
}

template<std::uint8_t dimensions> ShapeShader<dimensions>::ShapeShader() {
    Corrade::Utility::Resource resource("MagnumPhysics");
    attachShader(Shader::fromData(Version::GL330, Shader::Type::Vertex, resource.get(ShaderName<dimensions>::Vertex)));
    attachShader(Shader::fromData(Version::GL330, Shader::Type::Fragment, resource.get(ShaderName<dimensions>::Fragment)));

    link();

    transformationProjectionUniform = uniformLocation("transformationProjection");
    colorUniform = uniformLocation("color");
}

template class ShapeShader<2>;
template class ShapeShader<3>;

}}}
