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

#include "Shader.h"

#include <fstream>

#define COMPILER_MESSAGE_MAX_LENGTH 1024

using namespace std;
using namespace Corrade::Utility;

namespace Magnum {

bool Shader::addFile(const std::string& filename) {
    /* Open file */
    ifstream file(filename.c_str());
    if(!file.good()) {
        file.close();

        Error() << "Shader file " << '\'' + filename + '\'' << " cannot be opened.";
        return false;
    }

    /* Get size of shader and initialize buffer */
    file.seekg(0, ios::end);
    size_t size = file.tellg();
    char* source = new char[size+1];
    source[size] = '\0';

    /* Read data, close */
    file.seekg(0, ios::beg);
    file.read(source, size);
    file.close();

    /* Add to sources and free the buffer */
    addSource(source);
    delete source;

    return true;
}

GLuint Shader::compile() {
    /* Already compiled, return */
    if(_state != Initialized) return shader;

    /* Array of sources */
    const GLchar** _sources = new const GLchar*[sources.size()];
    for(size_t i = 0; i != sources.size(); ++i)
        _sources[i] = static_cast<const GLchar*>(sources[i].c_str());

    /* Create shader and set its source */
    shader = glCreateShader(_type);
    glShaderSource(shader, 1, _sources, 0);

    /* Compile shader */
    glCompileShader(shader);
    delete _sources;

    /* Check compilation status */
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    /* Display errors or warnings */
    char message[COMPILER_MESSAGE_MAX_LENGTH];
    glGetShaderInfoLog(shader, COMPILER_MESSAGE_MAX_LENGTH, 0, message);

    if(status == GL_FALSE || message[0] != 0) {
        Error err;
        err << "Shader:";

        switch(_type) {
            case Vertex:    err << "vertex";        break;
            case Geometry:  err << "geometry";      break;
            case Fragment:  err << "fragment";      break;
        }

        /* Show error log and delete shader */
        if(status == GL_FALSE) {
            err << "shader failed to compile with the following message:\n"
                << message;

        /* Or just warnings, if there are any */
        } else if(message[0] != 0) {
            err << "shader was successfully compiled with the following message:\n"
                << message;
        }
    }

    if(status == GL_FALSE) {
        glDeleteShader(shader);
        shader = 0;
        _state = Failed;
    } else _state = Compiled;

    return shader;
}

}
