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

#include "PhongShader.h"

#include <Utility/Resource.h>

#include "Extensions.h"
#include "Shader.h"

namespace Magnum { namespace Shaders {

PhongShader::PhongShader() {
    Corrade::Utility::Resource rs("MagnumShaders");
    Version v = Context::current()->isVersionSupported(Version::GL320) ? Version::GL320 : Version::GL210;
    attachShader(Shader::fromData(v, Shader::Type::Vertex, rs.get("PhongShader.vert")));
    attachShader(Shader::fromData(v, Shader::Type::Fragment, rs.get("PhongShader.frag")));

    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>()) {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Normal::Location, "normal");
    }

    link();

    ambientColorUniform = uniformLocation("ambientColor");
    diffuseColorUniform = uniformLocation("diffuseColor");
    specularColorUniform = uniformLocation("specularColor");
    shininessUniform = uniformLocation("shininess");
    transformationMatrixUniform = uniformLocation("transformationMatrix");
    projectionMatrixUniform = uniformLocation("projectionMatrix");
    lightUniform = uniformLocation("light");
    lightColorUniform = uniformLocation("lightColor");
}

}}
