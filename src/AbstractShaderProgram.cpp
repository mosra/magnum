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

#include "AbstractShaderProgram.h"

#include <fstream>

#define LINKER_MESSAGE_MAX_LENGTH 1024

using namespace std;

namespace Magnum {

bool AbstractShaderProgram::use() {
    if(state != Linked) return false;

    glUseProgram(program);
    return true;
}

bool AbstractShaderProgram::attachShader(Shader& shader) {
    GLuint _shader = shader.compile();
    if(!_shader) return false;

    glAttachShader(program, _shader);
    return true;
}

void AbstractShaderProgram::bindAttributeLocation(GLuint location, const string& name) {
    CORRADE_ASSERT(state == Initialized, "AbstractShaderProgram: attribute cannot be bound after linking.", );

    glBindAttribLocation(program, location, name.c_str());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::bindFragmentDataLocation(GLuint location, const std::string& name) {
    CORRADE_ASSERT(state == Initialized, "AbstractShaderProgram: fragment data location cannot be bound after linking.", );

    glBindFragDataLocation(program, location, name.c_str());
}
#endif

void AbstractShaderProgram::link() {
    /* Already compiled or failed, exit */
    if(state != Initialized) return;

    /* Link shader program */
    glLinkProgram(program);

    /* Check link status */
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    /* Display errors or warnings */
    char message[LINKER_MESSAGE_MAX_LENGTH];
    glGetProgramInfoLog(program, LINKER_MESSAGE_MAX_LENGTH, nullptr, message);

    /* Show error log and delete shader */
    if(status == GL_FALSE) {
        Error() << "AbstractShaderProgram: linking failed with the following message:\n"
                << message;

    /* Or just warnings, if there are any */
    } else if(message[0] != 0) {
        Debug() << "AbstractShaderProgram: linking succeeded with the following message:\n"
                << message;
    }

    state = status == GL_FALSE ? Failed : Linked;
}

GLint AbstractShaderProgram::uniformLocation(const std::string& name) {
    /** @todo What if linking just failed (not programmer error?) */
    CORRADE_ASSERT(state == Linked, "AbstractShaderProgram: uniform location cannot be retrieved before linking.", -1);

    GLint location = glGetUniformLocation(program, name.c_str());
    if(location == -1)
        Warning() << "AbstractShaderProgram: location of uniform \'" + name + "\' cannot be retrieved!";
    return location;
}

}
