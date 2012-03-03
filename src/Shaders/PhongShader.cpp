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

#include "Utility/Resource.h"

using namespace Corrade::Utility;

namespace Magnum { namespace Shaders {

PhongShader::PhongShader() {
    Resource rs("shaders");
    Shader* vertexShader = Shader::fromData(Shader::Vertex, rs.get("PhongShader.vert"));
    Shader* fragmentShader = Shader::fromData(Shader::Fragment, rs.get("PhongShader.frag"));
    attachShader(vertexShader);
    attachShader(fragmentShader);

    bindAttribute(Vertex::Location, "vertex");
    bindAttribute(Normal::Location, "normal");

    link();
    delete vertexShader;
    delete fragmentShader;

    ambientColorUniform = uniformLocation("ambientColor");
    diffuseColorUniform = uniformLocation("diffuseColor");
    specularColorUniform = uniformLocation("specularColor");
    shininessUniform = uniformLocation("shininess");
    transformationMatrixUniform = uniformLocation("transformationMatrix");
    projectionMatrixUniform = uniformLocation("projectionMatrix");
    lightUniform = uniformLocation("light");
    lightAmbientColorUniform = uniformLocation("lightAmbientColor");
    lightDiffuseColorUniform = uniformLocation("lightDiffuseColor");
    lightSpecularColorUniform = uniformLocation("lightSpecularColor");
}

}}
