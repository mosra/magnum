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

#include <iostream>
#include <fstream>

#define LINKER_MESSAGE_MAX_LENGTH 1024

using namespace std;

namespace Magnum {

AbstractShaderProgram::LogLevel AbstractShaderProgram::_logLevel = AbstractShaderProgram::Errors;

AbstractShaderProgram::AbstractShaderProgram(): state(Initialized) {
    program = glCreateProgram();
}

AbstractShaderProgram::~AbstractShaderProgram() {
    if(program) glDeleteProgram(program);
}

bool AbstractShaderProgram::use() {
    if(state != Linked) return false;

    glUseProgram(program);
    return true;
}

bool AbstractShaderProgram::attachShader(Shader* shader) {
    if(!shader) return false;

    GLuint _shader = shader->compile();
    if(!_shader) return false;

    glAttachShader(program, _shader);
    return true;
}

bool AbstractShaderProgram::bindAttribute(GLuint location, const string& name) {
    /* Check whether given id already exists */
    if(attributes.find(location) != attributes.end()) return false;

    /* Check whether given name already exists */
    for(map<GLuint, string>::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
        if(it->second == name) return false;

    attributes.insert(pair<GLuint, string>(location, name));
    return true;
}

void AbstractShaderProgram::link() {
    /* Already compiled or failed, exit */
    if(state != Initialized) return;

    /* Set state to failed if anything goes wrong */
    state = Failed;

    /* Bind attributes to specified locations */
    for(map<GLuint, string>::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
        glBindAttribLocation(program, it->first, it->second.c_str());

    /* Link shader program */
    glLinkProgram(program);

    /* Check link status */
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    /* Display errors or warnings */
    if((status == GL_FALSE && _logLevel != None) || _logLevel == Warnings) {
        /* Get message */
        char message[LINKER_MESSAGE_MAX_LENGTH];
        glGetProgramInfoLog(program, LINKER_MESSAGE_MAX_LENGTH, 0, message);

        /* Show error log and delete shader */
        if(status == GL_FALSE) {
            cerr << "Shader program failed to link. Error message:" << endl
                 << message << endl;

        /* Or just warnings, if there are any */
        } else if(message[0] != 0) {
            cerr << "Shader program was linked with the following warnings:" << endl
                 << message << endl;
        }
    }

    if(status == GL_FALSE) {
        glDeleteProgram(program);
        program = 0;
        state = Failed;
    } else state = Linked;
}

GLint AbstractShaderProgram::uniformLocation(const std::string& name) {
    if(state != Linked) return -1;
    return glGetUniformLocation(program, name.c_str());
}

}
