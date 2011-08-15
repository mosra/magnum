/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "TexturedIdentityShader.h"

namespace Magnum { namespace Examples {

TexturedIdentityShader::TexturedIdentityShader() {
    Shader* vertexShader = Shader::fromFile(Shader::Vertex, "TexturedIdentityShader.vert");
    Shader* fragmentShader = Shader::fromFile(Shader::Fragment, "TexturedIdentityShader.frag");

    attachShader(vertexShader);
    attachShader(fragmentShader);

    bindAttribute(Vertex, "vertex");
    bindAttribute(TextureCoordinates, "textureCoordinates");

    link();

    textureUniform = uniformLocation("textureData");

    delete vertexShader;
    delete fragmentShader;
}

}}
