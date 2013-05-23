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

#include "AbstractShaderProgram.h"

#include "Math/RectangularMatrix.h"
#include "Extensions.h"
#include "Shader.h"
#include "Implementation/ShaderProgramState.h"
#include "Implementation/State.h"

namespace Magnum {

AbstractShaderProgram::Uniform1fvImplementation AbstractShaderProgram::uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2fvImplementation AbstractShaderProgram::uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3fvImplementation AbstractShaderProgram::uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4fvImplementation AbstractShaderProgram::uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform1ivImplementation AbstractShaderProgram::uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2ivImplementation AbstractShaderProgram::uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3ivImplementation AbstractShaderProgram::uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4ivImplementation AbstractShaderProgram::uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#ifndef MAGNUM_TARGET_GLES2
AbstractShaderProgram::Uniform1uivImplementation AbstractShaderProgram::uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2uivImplementation AbstractShaderProgram::uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3uivImplementation AbstractShaderProgram::uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4uivImplementation AbstractShaderProgram::uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif
#ifndef MAGNUM_TARGET_GLES
AbstractShaderProgram::Uniform1dvImplementation AbstractShaderProgram::uniform1dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform2dvImplementation AbstractShaderProgram::uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform3dvImplementation AbstractShaderProgram::uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::Uniform4dvImplementation AbstractShaderProgram::uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif

AbstractShaderProgram::UniformMatrix2fvImplementation AbstractShaderProgram::uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3fvImplementation AbstractShaderProgram::uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4fvImplementation AbstractShaderProgram::uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#ifndef MAGNUM_TARGET_GLES2
AbstractShaderProgram::UniformMatrix2x3fvImplementation AbstractShaderProgram::uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x2fvImplementation AbstractShaderProgram::uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x4fvImplementation AbstractShaderProgram::uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x2fvImplementation AbstractShaderProgram::uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x4fvImplementation AbstractShaderProgram::uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x3fvImplementation AbstractShaderProgram::uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif
#ifndef MAGNUM_TARGET_GLES
AbstractShaderProgram::UniformMatrix2dvImplementation AbstractShaderProgram::uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3dvImplementation AbstractShaderProgram::uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4dvImplementation AbstractShaderProgram::uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x3dvImplementation AbstractShaderProgram::uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x2dvImplementation AbstractShaderProgram::uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix2x4dvImplementation AbstractShaderProgram::uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x2dvImplementation AbstractShaderProgram::uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix3x4dvImplementation AbstractShaderProgram::uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
AbstractShaderProgram::UniformMatrix4x3dvImplementation AbstractShaderProgram::uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDefault;
#endif

Int AbstractShaderProgram::maxSupportedVertexAttributeCount() {
    GLint& value = Context::current()->state()->shaderProgram->maxSupportedVertexAttributeCount;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);

    return value;
}

AbstractShaderProgram::AbstractShaderProgram(): _id(glCreateProgram()) {}

AbstractShaderProgram::~AbstractShaderProgram() {
    /* Remove current usage from the state */
    GLuint& current = Context::current()->state()->shaderProgram->current;
    if(current == _id) current = 0;

    if(_id) glDeleteProgram(_id);
}

void AbstractShaderProgram::use() {
    /* Use only if the program isn't already in use */
    GLuint& current = Context::current()->state()->shaderProgram->current;
    if(current != _id) glUseProgram(current = _id);
}

void AbstractShaderProgram::attachShader(Shader& shader) {
    glAttachShader(_id, shader.id());
}

void AbstractShaderProgram::bindAttributeLocation(UnsignedInt location, const std::string& name) {
    glBindAttribLocation(_id, location, name.c_str());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::bindFragmentDataLocation(UnsignedInt location, const std::string& name) {
    glBindFragDataLocation(_id, location, name.c_str());
}
void AbstractShaderProgram::bindFragmentDataLocationIndexed(UnsignedInt location, UnsignedInt index, const std::string& name) {
    glBindFragDataLocationIndexed(_id, location, index, name.c_str());
}
#endif

bool AbstractShaderProgram::link() {
    /* Link shader program */
    glLinkProgram(_id);

    /* Check link status */
    GLint success, logLength;
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The string is returned null-terminated, scrap
       the \0 at the end afterwards */
    std::string message(logLength, '\n');
    if(!message.empty()) {
        glGetProgramInfoLog(_id, message.size(), nullptr, &message[0]);
        message.resize(logLength-1);
    }

    /* Show error log and delete shader */
    if(!success) {
        Error out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "AbstractShaderProgram: linking failed with the following message:\n"
            << message;

    /* Or just warnings, if there are any */
    } else if(!message.empty()) {
        Debug out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "AbstractShaderProgram: linking succeeded with the following message:\n"
            << message;
    }

    return success;
}

Int AbstractShaderProgram::uniformLocation(const std::string& name) {
    GLint location = glGetUniformLocation(_id, name.c_str());
    if(location == -1)
        Warning() << "AbstractShaderProgram: location of uniform \'" + name + "\' cannot be retrieved!";
    return location;
}

void AbstractShaderProgram::initializeContextBasedFunctionality(Context* context) {
    /** @todo OpenGL ES 2 has extension @es_extension{EXT,separate_shader_objects} for this */
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ||
       context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractShaderProgram: using" << (context->isExtensionSupported<Extensions::GL::ARB::separate_shader_objects>() ?
            Extensions::GL::ARB::separate_shader_objects::string() : Extensions::GL::EXT::direct_state_access::string()) << "features";
        uniform1fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4ivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4uivImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform1dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniform4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;

        uniformMatrix2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3fvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix2x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x2dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix3x4dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
        uniformMatrix4x3dvImplementation = &AbstractShaderProgram::uniformImplementationDSA;
    }
    #else
    static_cast<void>(context);
    #endif
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLfloat* const values) {
    use();
    glUniform1fv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const GLfloat* const values) {
    glProgramUniform1fv(_id, location, count, values);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLfloat>* const values) {
    use();
    glUniform2fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<2, GLfloat>* const values) {
    glProgramUniform2fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLfloat>* const values) {
    use();
    glUniform3fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<3, GLfloat>* const values) {
    glProgramUniform3fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLfloat>* const values) {
    use();
    glUniform4fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<4, GLfloat>* const values) {
    glProgramUniform4fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLint* const values) {
    use();
    glUniform1iv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const GLint* const values) {
    glProgramUniform1iv(_id, location, count, values);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLint>* const values) {
    use();
    glUniform2iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<2, GLint>* const values) {
    glProgramUniform2iv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLint>* const values) {
    use();
    glUniform3iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<3, GLint>* const values) {
    glProgramUniform3iv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLint>* const values) {
    use();
    glUniform4iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<4, GLint>* const values) {
    glProgramUniform4iv(_id, location, count, values[0].data());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLuint* const values) {
    use();
    glUniform1uiv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const GLuint* const values) {
    glProgramUniform1uiv(_id, location, count, values);
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLuint>* const values) {
    use();
    glUniform2uiv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<2, GLuint>* const values) {
    glProgramUniform2uiv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLuint>* const values) {
    use();
    glUniform3uiv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<3, GLuint>* const values) {
    glProgramUniform3uiv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLuint>* const values) {
    use();
    glUniform4uiv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<4, GLuint>* const values) {
    glProgramUniform4uiv(_id, location, count, values[0].data());
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLdouble* const values) {
    use();
    glUniform1dv(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const GLdouble* const values) {
    glProgramUniform1dv(_id, location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLdouble>* const values) {
    use();
    glUniform2dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<2, GLdouble>* const values) {
    glProgramUniform2dv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLdouble>* const values) {
    use();
    glUniform3dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<3, GLdouble>* const values) {
    glProgramUniform3dv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLdouble>* const values) {
    use();
    glUniform4dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::Vector<4, GLdouble>* const values) {
    glProgramUniform4dv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* const values) {
    use();
    glUniformMatrix2fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* const values) {
    glProgramUniformMatrix2fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* const values) {
    use();
    glUniformMatrix3fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* const values) {
    glProgramUniformMatrix3fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* const values) {
    use();
    glUniformMatrix4fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* const values) {
    glProgramUniformMatrix4fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* const values) {
    use();
    glUniformMatrix2x3fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* const values) {
    glProgramUniformMatrix2x3fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* const values) {
    use();
    glUniformMatrix3x2fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* const values) {
    glProgramUniformMatrix3x2fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* const values) {
    use();
    glUniformMatrix2x4fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* const values) {
    glProgramUniformMatrix2x4fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* const values) {
    use();
    glUniformMatrix4x2fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* const values) {
    glProgramUniformMatrix4x2fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* const values) {
    use();
    glUniformMatrix3x4fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* const values) {
    glProgramUniformMatrix3x4fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* const values) {
    use();
    glUniformMatrix4x3fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* const values) {
    glProgramUniformMatrix4x3fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* const values) {
    use();
    glUniformMatrix2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* const values) {
    glProgramUniformMatrix2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* const values) {
    use();
    glUniformMatrix3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* const values) {
    glProgramUniformMatrix3dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* const values) {
    use();
    glUniformMatrix4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* const values) {
    glProgramUniformMatrix4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* const values) {
    use();
    glUniformMatrix2x3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* const values) {
    glProgramUniformMatrix2x3dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* const values) {
    use();
    glUniformMatrix3x2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* const values) {
    glProgramUniformMatrix3x2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* const values) {
    use();
    glUniformMatrix2x4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* const values) {
    glProgramUniformMatrix2x4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* const values) {
    use();
    glUniformMatrix4x2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* const values) {
    glProgramUniformMatrix4x2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* const values) {
    use();
    glUniformMatrix3x4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* const values) {
    glProgramUniformMatrix3x4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* const values) {
    use();
    glUniformMatrix4x3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSA(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* const values) {
    glProgramUniformMatrix4x3dv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

namespace Implementation {

std::size_t FloatAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        case DataType::HalfFloat:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}

#ifndef MAGNUM_TARGET_GLES2
std::size_t IntAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
            return 4*components;
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}
#endif

#ifndef MAGNUM_TARGET_GLES
std::size_t DoubleAttribute::size(GLint components, DataType dataType) {
    switch(dataType) {
        case DataType::Double:
            return 8*components;
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}
#endif

std::size_t Attribute<Math::Vector<4, Float>>::size(GLint components, DataType dataType) {
    #ifndef MAGNUM_TARGET_GLES
    if(components == GL_BGRA) components = 4;
    #endif

    switch(dataType) {
        case DataType::UnsignedByte:
        case DataType::Byte:
            return components;
        case DataType::UnsignedShort:
        case DataType::Short:
        case DataType::HalfFloat:
            return 2*components;
        case DataType::UnsignedInt:
        case DataType::Int:
        case DataType::Float:
            return 4*components;
        #ifndef MAGNUM_TARGET_GLES
        case DataType::Double:
            return 8*components;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case DataType::UnsignedInt2101010Rev:
        case DataType::Int2101010Rev:
            CORRADE_INTERNAL_ASSERT(components == 4);
            return 4;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
    return 0;
}

Debug operator<<(Debug debug, SizedAttribute<1, 1>::Components value) {
    switch(value) {
        case SizedAttribute<1, 1>::Components::One:
            return debug << "AbstractShaderProgram::Attribute::Components::One";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedAttribute<1, 2>::Components value) {
    switch(value) {
        case SizedAttribute<1, 2>::Components::One:
            return debug << "AbstractShaderProgram::Attribute::Components::One";
        case SizedAttribute<1, 2>::Components::Two:
            return debug << "AbstractShaderProgram::Attribute::Components::Two";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedAttribute<1, 3>::Components value) {
    switch(value) {
        case SizedAttribute<1, 3>::Components::One:
            return debug << "AbstractShaderProgram::Attribute::Components::One";
        case SizedAttribute<1, 3>::Components::Two:
            return debug << "AbstractShaderProgram::Attribute::Components::Two";
        case SizedAttribute<1, 3>::Components::Three:
            return debug << "AbstractShaderProgram::Attribute::Components::Three";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedAttribute<1, 4>::Components value) {
    switch(value) {
        case SizedAttribute<1, 4>::Components::One:
            return debug << "AbstractShaderProgram::Attribute::Components::One";
        case SizedAttribute<1, 4>::Components::Two:
            return debug << "AbstractShaderProgram::Attribute::Components::Two";
        case SizedAttribute<1, 4>::Components::Three:
            return debug << "AbstractShaderProgram::Attribute::Components::Three";
        case SizedAttribute<1, 4>::Components::Four:
            return debug << "AbstractShaderProgram::Attribute::Components::Four";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedMatrixAttribute<2>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<2>::Components::Two:
            return debug << "AbstractShaderProgram::Attribute::Components::Two";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedMatrixAttribute<3>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<3>::Components::Three:
            return debug << "AbstractShaderProgram::Attribute::Components::Three";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, SizedMatrixAttribute<4>::Components value) {
    switch(value) {
        case SizedMatrixAttribute<4>::Components::Four:
            return debug << "AbstractShaderProgram::Attribute::Components::Four";
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, Attribute<Math::Vector<4, Float>>::Components value) {
    switch(value) {
        case Attribute<Math::Vector<4, Float>>::Components::One:
            return debug << "AbstractShaderProgram::Attribute::Components::One";
        case Attribute<Math::Vector<4, Float>>::Components::Two:
            return debug << "AbstractShaderProgram::Attribute::Components::Two";
        case Attribute<Math::Vector<4, Float>>::Components::Three:
            return debug << "AbstractShaderProgram::Attribute::Components::Three";
        case Attribute<Math::Vector<4, Float>>::Components::Four:
            return debug << "AbstractShaderProgram::Attribute::Components::Four";
        #ifndef MAGNUM_TARGET_GLES
        case Attribute<Math::Vector<4, Float>>::Components::BGRA:
            return debug << "AbstractShaderProgram::Attribute::Components::BGRA";
        #endif
    }

    return debug << "AbstractShaderProgram::Attribute::Components::(invalid)";
}

Debug operator<<(Debug debug, FloatAttribute::DataType value) {
    switch(value) {
        #define _c(value) case FloatAttribute::DataType::value: return debug << "AbstractShaderProgram::Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #undef _c
    }

    return debug << "AbstractShaderProgram::Attribute::DataType::(invalid)";
}

#ifndef MAGNUM_TARGET_GLES2
Debug operator<<(Debug debug, IntAttribute::DataType value) {
    switch(value) {
        #define _c(value) case IntAttribute::DataType::value: return debug << "AbstractShaderProgram::Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        #undef _c
    }

    return debug << "AbstractShaderProgram::Attribute::DataType::(invalid)";
}
#endif

#ifndef MAGNUM_TARGET_GLES
Debug operator<<(Debug debug, DoubleAttribute::DataType value) {
    switch(value) {
        #define _c(value) case DoubleAttribute::DataType::value: return debug << "AbstractShaderProgram::Attribute::DataType::" #value;
        _c(Double)
        #undef _c
    }

    return debug << "AbstractShaderProgram::Attribute::DataType::(invalid)";
}
#endif

Debug operator<<(Debug debug, Attribute<Math::Vector<4, Float>>::DataType value) {
    switch(value) {
        #define _c(value) case Attribute<Math::Vector<4, Float>>::DataType::value: return debug << "AbstractShaderProgram::Attribute::DataType::" #value;
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(HalfFloat)
        _c(Float)
        #ifndef MAGNUM_TARGET_GLES
        _c(Double)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UnsignedInt2101010Rev)
        _c(Int2101010Rev)
        #endif
        #undef _c
    }

    return debug << "AbstractShaderProgram::Attribute::DataType::(invalid)";
}

}

}
