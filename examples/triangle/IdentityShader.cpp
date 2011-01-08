/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "IdentityShader.h"

namespace Magnum { namespace Examples {

IdentityShader::IdentityShader() {
    Shader* vertexShader = Shader::fromFile(Shader::Vertex, "IdentityShader.vert");
    Shader* fragmentShader = Shader::fromFile(Shader::Fragment, "IdentityShader.frag");

    attachShader(vertexShader);
    attachShader(fragmentShader);

    bindAttribute(Vertex, "vertex");
    bindAttribute(Color, "color");

    link();

    delete vertexShader;
    delete fragmentShader;
}

}}
