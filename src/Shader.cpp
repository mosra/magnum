/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Shader.h"

#include <fstream>
#include <Utility/Assert.h>

#define COMPILER_MESSAGE_MAX_LENGTH 1024

/* libgles-omap3-dev_4.03.00.02-r15.6 on BeagleBoard/Ångström linux 2011.3 doesn't have GLchar */
#ifdef MAGNUM_TARGET_GLES
typedef char GLchar;
#endif

namespace Magnum {

Shader::Shader(Version version, Type type): _type(type), _state(State::Initialized), shader(0) {
    shader = glCreateShader(static_cast<GLenum>(_type));

    switch(version) {
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: addSource("#version 120\n"); return;
        case Version::GL300: addSource("#version 130\n"); return;
        case Version::GL310: addSource("#version 140\n"); return;
        case Version::GL320: addSource("#version 150\n"); return;
        case Version::GL330: addSource("#version 330\n"); return;
        case Version::GL400: addSource("#version 400\n"); return;
        case Version::GL410: addSource("#version 410\n"); return;
        case Version::GL420: addSource("#version 420\n"); return;
        case Version::GL430: addSource("#version 430\n"); return;
        #else
        case Version::GLES200: addSource("#version 100\n"); return;
        case Version::GLES300: addSource("#version 300\n"); return;
        #endif

        case Version::None:
            CORRADE_ASSERT(false, "Shader::Shader(): unsupported version" << version, );
    }

    CORRADE_ASSERT_UNREACHABLE();
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

Shader& Shader::addSource(const std::string& source) {
    if(_state == State::Initialized)
        /* Fix line numbers, so line 41 of third added file is marked as 3(41).
           Source 0 is the #version string added in constructor. */
        sources.push_back("#line 1 " + std::to_string(sources.size()) + '\n' + source);
    return *this;
}

Shader& Shader::addFile(const std::string& filename) {
    /* Open file */
    std::ifstream file(filename.c_str());
    CORRADE_ASSERT(file.good(), "Shader file " << '\'' + filename + '\'' << " cannot be opened.", *this);

    /* Get size of shader and initialize buffer */
    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    char* source = new char[size+1];
    source[size] = '\0';

    /* Read data, close */
    file.seekg(0, std::ios::beg);
    file.read(source, size);
    file.close();

    /* Add to sources and free the buffer */
    addSource(source);
    delete[] source;

    return *this;
}

GLuint Shader::compile() {
    /* Already compiled, return */
    if(_state != State::Initialized) return shader;

    /* Array of sources */
    const GLchar** _sources = new const GLchar*[sources.size()];
    for(std::size_t i = 0; i != sources.size(); ++i)
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

        err.setFlag(Debug::NewLineAtTheEnd, false);
        err.setFlag(Debug::SpaceAfterEachValue, false);

        /* Show error log and delete shader */
        if(status == GL_FALSE) {
            err << " shader failed to compile with the following message:\n"
                << message;

        /* Or just warnings, if there are any */
        } else if(message[0] != 0) {
            err << " shader was successfully compiled with the following message:\n"
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
