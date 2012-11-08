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
#include <Utility/Debug.h>

#define COMPILER_MESSAGE_MAX_LENGTH 1024

/* libgles-omap3-dev_4.03.00.02-r15.6 on BeagleBoard/Ångström linux 2011.3 doesn't have GLchar */
#ifdef MAGNUM_TARGET_GLES
typedef char GLchar;
#endif

using namespace std;

namespace Magnum {

Shader::Shader(Version version, Type type): _type(type), _state(State::Initialized), shader(0) {
    shader = glCreateShader(static_cast<GLenum>(_type));

    switch(version) {
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: addSource("#version 120\n"); break;
        case Version::GL300: addSource("#version 130\n"); break;
        case Version::GL310: addSource("#version 140\n"); break;
        case Version::GL320: addSource("#version 150\n"); break;
        case Version::GL330: addSource("#version 330\n"); break;
        case Version::GL400: addSource("#version 400\n"); break;
        case Version::GL410: addSource("#version 410\n"); break;
        case Version::GL420: addSource("#version 420\n"); break;
        case Version::GL430: addSource("#version 430\n"); break;
        #else
        case Version::GLES200: addSource("#version 100\n"); break;
        case Version::GLES300: addSource("#version 300\n"); break;
        #endif

        default:
            CORRADE_ASSERT(false, "Shader::Shader(): unsupported version" << GLint(version), );
    }
}

Shader::Shader(Shader&& other): _type(other._type), _state(other._state), sources(other.sources), shader(other.shader) {
    other.shader = 0;
}

Shader& Shader::operator=(Shader&& other) {
    glDeleteShader(shader);

    _type = other._type;
    _state = other._state;
    sources = other.sources;
    shader = other.shader;

    other.shader = 0;

    return *this;
}

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
    delete[] source;

    return true;
}

GLuint Shader::compile() {
    /* Already compiled, return */
    if(_state != State::Initialized) return shader;

    /* Array of sources */
    const GLchar** _sources = new const GLchar*[sources.size()];
    for(size_t i = 0; i != sources.size(); ++i)
        _sources[i] = static_cast<const GLchar*>(sources[i].c_str());

    /* Create shader and set its source */
    glShaderSource(shader, sources.size(), _sources, nullptr);

    /* Compile shader */
    glCompileShader(shader);
    delete _sources;

    /* Check compilation status */
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    /* Display errors or warnings */
    char message[COMPILER_MESSAGE_MAX_LENGTH];
    glGetShaderInfoLog(shader, COMPILER_MESSAGE_MAX_LENGTH, nullptr, message);

    if(status == GL_FALSE || message[0] != 0) {
        Error err;
        err << "Shader:";

        switch(_type) {
            case Type::Vertex:      err << "vertex";        break;
            #ifndef MAGNUM_TARGET_GLES
            case Type::Geometry:    err << "geometry";      break;
            case Type::TessellationControl: err << "tessellation control"; break;
            case Type::TessellationEvaluation: err << "tessellation evaluation"; break;
            case Type::Compute:     err << "compute";       break;
            #endif
            case Type::Fragment:    err << "fragment";      break;
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
        _state = State::Failed;
        return 0;
    } else {
        _state = State::Compiled;
        return shader;
    }
}

}
