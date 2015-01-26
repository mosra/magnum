/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Shader.h"
#include "Magnum/Math/RectangularMatrix.h"

#include "Implementation/DebugState.h"
#include "Implementation/ShaderProgramState.h"
#include "Implementation/State.h"

#if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID) || defined(__MINGW32__)
#include <sstream>
#endif

namespace Magnum {

Int AbstractShaderProgram::maxVertexAttributes() {
    GLint& value = Context::current()->state().shaderProgram->maxVertexAttributes;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);

    return value;
}

#ifndef MAGNUM_TARGET_GLES2
Int AbstractShaderProgram::maxAtomicCounterBufferSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxAtomicCounterBufferSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, &value);

    return value;
}

Int AbstractShaderProgram::maxComputeSharedMemorySize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::compute_shader>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxComputeSharedMemorySize;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &value);

    return value;
}

Int AbstractShaderProgram::maxComputeWorkGroupInvocations() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::compute_shader>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxComputeWorkGroupInvocations;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &value);

    return value;
}

Int AbstractShaderProgram::maxImageUnits() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxImageUnits;

    if(value == 0)
        glGetIntegerv(GL_MAX_IMAGE_UNITS, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Int AbstractShaderProgram::maxImageSamples() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxImageSamples;

    if(value == 0)
        glGetIntegerv(GL_MAX_IMAGE_SAMPLES, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Int AbstractShaderProgram::maxCombinedShaderOutputResources() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>() || !Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxCombinedShaderOutputResources;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &value);

    return value;
}

Long AbstractShaderProgram::maxShaderStorageBlockSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint64& value = Context::current()->state().shaderProgram->maxShaderStorageBlockSize;

    if(value == 0)
        glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Int AbstractShaderProgram::maxUniformBlockSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current()->state().shaderProgram->maxUniformBlockSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Int AbstractShaderProgram::maxUniformLocations() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::explicit_uniform_location>())
    #else
    if(!Context::current()->isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current()->state().shaderProgram->maxUniformLocations;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Int AbstractShaderProgram::minTexelOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        return 0;
    #endif

    GLint& value = Context::current()->state().shaderProgram->minTexelOffset;

    if(value == 0)
        glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &value);

    return value;
}

Int AbstractShaderProgram::maxTexelOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        return 0;
    #endif

    GLint& value = Context::current()->state().shaderProgram->maxTexelOffset;

    if(value == 0)
        glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &value);

    return value;
}
#endif

AbstractShaderProgram::AbstractShaderProgram(): _id(glCreateProgram()) {
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

AbstractShaderProgram::AbstractShaderProgram(AbstractShaderProgram&& other) noexcept: _id(other._id) {
    other._id = 0;
}

AbstractShaderProgram::~AbstractShaderProgram() {
    /* Remove current usage from the state */
    GLuint& current = Context::current()->state().shaderProgram->current;
    if(current == _id) current = 0;

    if(_id) glDeleteProgram(_id);
}

AbstractShaderProgram& AbstractShaderProgram::operator=(AbstractShaderProgram&& other) noexcept {
    std::swap(_id, other._id);
    return *this;
}

std::string AbstractShaderProgram::label() const {
    #ifndef MAGNUM_TARGET_GLES
    return Context::current()->state().debug->getLabelImplementation(GL_PROGRAM, _id);
    #else
    return Context::current()->state().debug->getLabelImplementation(GL_PROGRAM_KHR, _id);
    #endif
}

AbstractShaderProgram& AbstractShaderProgram::setLabelInternal(const Containers::ArrayReference<const char> label) {
    #ifndef MAGNUM_TARGET_GLES
    Context::current()->state().debug->labelImplementation(GL_PROGRAM, _id, label);
    #else
    Context::current()->state().debug->labelImplementation(GL_PROGRAM_KHR, _id, label);
    #endif
    return *this;
}

std::pair<bool, std::string> AbstractShaderProgram::validate() {
    glValidateProgram(_id);

    /* Check validation status */
    GLint success, logLength;
    glGetProgramiv(_id, GL_VALIDATE_STATUS, &success);
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The string is returned null-terminated, scrap
       the \0 at the end afterwards */
    std::string message(logLength, '\n');
    if(message.size() > 1)
        glGetProgramInfoLog(_id, message.size(), nullptr, &message[0]);
    message.resize(std::max(logLength, 1)-1);

    return {success, std::move(message)};
}

void AbstractShaderProgram::use() {
    /* Use only if the program isn't already in use */
    GLuint& current = Context::current()->state().shaderProgram->current;
    if(current != _id) glUseProgram(current = _id);
}

void AbstractShaderProgram::attachShader(Shader& shader) {
    glAttachShader(_id, shader.id());
}

void AbstractShaderProgram::attachShaders(std::initializer_list<std::reference_wrapper<Shader>> shaders) {
    for(Shader& s: shaders) attachShader(s);
}

void AbstractShaderProgram::bindAttributeLocationInternal(const UnsignedInt location, const Containers::ArrayReference<const char> name) {
    glBindAttribLocation(_id, location, name);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::bindFragmentDataLocationInternal(const UnsignedInt location, const Containers::ArrayReference<const char> name) {
    glBindFragDataLocation(_id, location, name);
}
void AbstractShaderProgram::bindFragmentDataLocationIndexedInternal(const UnsignedInt location, UnsignedInt index, const Containers::ArrayReference<const char> name) {
    glBindFragDataLocationIndexed(_id, location, index, name);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setTransformFeedbackOutputs(const std::initializer_list<std::string> outputs, const TransformFeedbackBufferMode bufferMode) {
    Containers::Array<const char*> names{outputs.size()};

    Int i = 0;
    for(const std::string& output: outputs) names[i++] = output.data();

    glTransformFeedbackVaryings(_id, outputs.size(), names, GLenum(bufferMode));
}
#endif

bool AbstractShaderProgram::link(std::initializer_list<std::reference_wrapper<AbstractShaderProgram>> shaders) {
    bool allSuccess = true;

    /* Invoke (possibly parallel) linking on all shaders */
    for(AbstractShaderProgram& shader: shaders) glLinkProgram(shader._id);

    /* After linking phase, check status of all shaders */
    Int i = 1;
    for(AbstractShaderProgram& shader: shaders) {
        GLint success, logLength;
        glGetProgramiv(shader._id, GL_LINK_STATUS, &success);
        glGetProgramiv(shader._id, GL_INFO_LOG_LENGTH, &logLength);

        /* Error or warning message. The string is returned null-terminated,
           scrap the \0 at the end afterwards */
        std::string message(logLength, '\n');
        if(message.size() > 1)
            glGetProgramInfoLog(shader._id, message.size(), nullptr, &message[0]);
        message.resize(std::max(logLength, 1)-1);

        /** @todo Remove when this is fixed everywhere (also the include above) */
        #if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID) || defined(__MINGW32__)
        std::ostringstream converter;
        converter << i;
        #endif

        /* Show error log */
        if(!success) {
            Error out;
            out.setFlag(Debug::NewLineAtTheEnd, false);
            out.setFlag(Debug::SpaceAfterEachValue, false);
            out << "AbstractShaderProgram::link(): linking";
            if(shaders.size() != 1) {
                #if !defined(CORRADE_TARGET_NACL_NEWLIB) && !defined(CORRADE_TARGET_ANDROID) && !defined(__MINGW32__)
                out << " of shader " << std::to_string(i);
                #else
                out << " of shader " << converter.str();
                #endif
            }
            out << " failed with the following message:\n"
                << message;

        /* Or just warnings, if any */
        } else if(!message.empty()) {
            Warning out;
            out.setFlag(Debug::NewLineAtTheEnd, false);
            out.setFlag(Debug::SpaceAfterEachValue, false);
            out << "AbstractShaderProgram::link(): linking";
            if(shaders.size() != 1) {
                #if !defined(CORRADE_TARGET_NACL_NEWLIB) && !defined(CORRADE_TARGET_ANDROID) && !defined(__MINGW32__)
                out << " of shader " << std::to_string(i);
                #else
                out << " of shader " << converter.str();
                #endif
            }
            out << " succeeded with the following message:\n"
                << message;
        }

        /* Success of all depends on each of them */
        allSuccess = allSuccess && success;
        ++i;
    }

    return allSuccess;
}

Int AbstractShaderProgram::uniformLocationInternal(const Containers::ArrayReference<const char> name) {
    GLint location = glGetUniformLocation(_id, name);
    if(location == -1) {
		std::string namestr = std::string{name, name.size()};
		while (!namestr.empty() && namestr.back() == '\0') namestr.pop_back();
        Warning() << "AbstractShaderProgram: location of uniform \'" + namestr + "\' cannot be retrieved!";
	}
		
    return location;
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Float* const values) {
    (this->*Context::current()->state().shaderProgram->uniform1fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLfloat* const values) {
    use();
    glUniform1fv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const GLfloat* const values) {
    glProgramUniform1fv(_id, location, count, values);
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const GLfloat* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform1fvEXT(_id, location, count, values);
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<2, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform2fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLfloat>* const values) {
    use();
    glUniform2fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<2, GLfloat>* const values) {
    glProgramUniform2fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<2, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform2fvEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<3, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform3fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLfloat>* const values) {
    use();
    glUniform3fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<3, GLfloat>* const values) {
    glProgramUniform3fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<3, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform3fvEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<4, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform4fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLfloat>* const values) {
    use();
    glUniform4fv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<4, GLfloat>* const values) {
    glProgramUniform4fv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<4, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform4fvEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Int* const values) {
    (this->*Context::current()->state().shaderProgram->uniform1ivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLint* const values) {
    use();
    glUniform1iv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const GLint* const values) {
    glProgramUniform1iv(_id, location, count, values);
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const GLint* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform1ivEXT(_id, location, count, values);
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<2, Int>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform2ivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLint>* const values) {
    use();
    glUniform2iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<2, GLint>* const values) {
    glProgramUniform2iv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<2, GLint>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform2ivEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<3, Int>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform3ivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLint>* const values) {
    use();
    glUniform3iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<3, GLint>* const values) {
    glProgramUniform3iv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<3, GLint>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform3ivEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<4, Int>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform4ivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLint>* const values) {
    use();
    glUniform4iv(location, count, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<4, GLint>* const values) {
    glProgramUniform4iv(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<4, GLint>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniform4ivEXT(_id, location, count, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const UnsignedInt* const values) {
    (this->*Context::current()->state().shaderProgram->uniform1uivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLuint* const values) {
    use();
    glUniform1uiv(location, count, values);
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const GLuint* const values) {
    glProgramUniform1uiv(_id, location, count, values);
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const GLuint* const values) {
    glProgramUniform1uivEXT(_id, location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<2, UnsignedInt>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform2uivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLuint>* const values) {
    use();
    glUniform2uiv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<2, GLuint>* const values) {
    glProgramUniform2uiv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<2, GLuint>* const values) {
    glProgramUniform2uivEXT(_id, location, count, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<3, UnsignedInt>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform3uivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLuint>* const values) {
    use();
    glUniform3uiv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<3, GLuint>* const values) {
    glProgramUniform3uiv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<3, GLuint>* const values) {
    glProgramUniform3uivEXT(_id, location, count, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<4, UnsignedInt>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform4uivImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLuint>* const values) {
    use();
    glUniform4uiv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<4, GLuint>* const values) {
    glProgramUniform4uiv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::Vector<4, GLuint>* const values) {
    glProgramUniform4uivEXT(_id, location, count, values[0].data());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Double* const values) {
    (this->*Context::current()->state().shaderProgram->uniform1dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const GLdouble* const values) {
    use();
    glUniform1dv(location, count, values);
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const GLdouble* const values) {
    glProgramUniform1dv(_id, location, count, values);
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const GLdouble* const values) {
    glProgramUniform1dvEXT(_id, location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<2, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform2dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<2, GLdouble>* const values) {
    use();
    glUniform2dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<2, GLdouble>* const values) {
    glProgramUniform2dv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::Vector<2, GLdouble>* const values) {
    glProgramUniform2dvEXT(_id, location, count, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<3, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform3dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<3, GLdouble>* const values) {
    use();
    glUniform3dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<3, GLdouble>* const values) {
    glProgramUniform3dv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::Vector<3, GLdouble>* const values) {
    glProgramUniform3dvEXT(_id, location, count, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::Vector<4, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniform4dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::Vector<4, GLdouble>* const values) {
    use();
    glUniform4dv(location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::Vector<4, GLdouble>* const values) {
    glProgramUniform4dv(_id, location, count, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::Vector<4, GLdouble>* const values) {
    glProgramUniform4dvEXT(_id, location, count, values[0].data());
}
#endif

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 2, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* const values) {
    use();
    glUniformMatrix2fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* const values) {
    glProgramUniformMatrix2fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniformMatrix2fvEXT(_id, location, count, GL_FALSE, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 3, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* const values) {
    use();
    glUniformMatrix3fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* const values) {
    glProgramUniformMatrix3fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniformMatrix3fvEXT(_id, location, count, GL_FALSE, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 4, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* const values) {
    use();
    glUniformMatrix4fv(location, count, GL_FALSE, values[0].data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* const values) {
    glProgramUniformMatrix4fv(_id, location, count, GL_FALSE, values[0].data());
}
#endif

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLfloat>* const values) {
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glProgramUniformMatrix4fvEXT(_id, location, count, GL_FALSE, values[0].data());
    #else
    static_cast<void>(location);
    static_cast<void>(count);
    static_cast<void>(values);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 3, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2x3fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* const values) {
    use();
    glUniformMatrix2x3fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* const values) {
    glProgramUniformMatrix2x3fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLfloat>* const values) {
    glProgramUniformMatrix2x3fvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 2, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3x2fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* const values) {
    use();
    glUniformMatrix3x2fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* const values) {
    glProgramUniformMatrix3x2fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLfloat>* const values) {
    glProgramUniformMatrix3x2fvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 4, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2x4fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* const values) {
    use();
    glUniformMatrix2x4fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* const values) {
    glProgramUniformMatrix2x4fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLfloat>* const values) {
    glProgramUniformMatrix2x4fvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 2, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4x2fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* const values) {
    use();
    glUniformMatrix4x2fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* const values) {
    glProgramUniformMatrix4x2fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLfloat>* const values) {
    glProgramUniformMatrix4x2fvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 4, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3x4fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* const values) {
    use();
    glUniformMatrix3x4fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* const values) {
    glProgramUniformMatrix3x4fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLfloat>* const values) {
    glProgramUniformMatrix3x4fvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 3, Float>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4x3fvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* const values) {
    use();
    glUniformMatrix4x3fv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* const values) {
    glProgramUniformMatrix4x3fv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT_SSOEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLfloat>* const values) {
    glProgramUniformMatrix4x3fvEXT(_id, location, count, GL_FALSE, values[0].data());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 2, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* const values) {
    use();
    glUniformMatrix2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* const values) {
    glProgramUniformMatrix2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 2, GLdouble>* const values) {
    glProgramUniformMatrix2dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 3, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* const values) {
    use();
    glUniformMatrix3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* const values) {
    glProgramUniformMatrix3dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 3, GLdouble>* const values) {
    glProgramUniformMatrix3dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 4, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* const values) {
    use();
    glUniformMatrix4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* const values) {
    glProgramUniformMatrix4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 4, GLdouble>* const values) {
    glProgramUniformMatrix4dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 3, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2x3dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* const values) {
    use();
    glUniformMatrix2x3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* const values) {
    glProgramUniformMatrix2x3dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 3, GLdouble>* const values) {
    glProgramUniformMatrix2x3dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 2, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3x2dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* const values) {
    use();
    glUniformMatrix3x2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* const values) {
    glProgramUniformMatrix3x2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 2, GLdouble>* const values) {
    glProgramUniformMatrix3x2dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<2, 4, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix2x4dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* const values) {
    use();
    glUniformMatrix2x4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* const values) {
    glProgramUniformMatrix2x4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<2, 4, GLdouble>* const values) {
    glProgramUniformMatrix2x4dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 2, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4x2dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* const values) {
    use();
    glUniformMatrix4x2dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* const values) {
    glProgramUniformMatrix4x2dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 2, GLdouble>* const values) {
    glProgramUniformMatrix4x2dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<3, 4, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix3x4dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* const values) {
    use();
    glUniformMatrix3x4dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* const values) {
    glProgramUniformMatrix3x4dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<3, 4, GLdouble>* const values) {
    glProgramUniformMatrix3x4dvEXT(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::setUniform(const Int location, const UnsignedInt count, const Math::RectangularMatrix<4, 3, Double>* const values) {
    (this->*Context::current()->state().shaderProgram->uniformMatrix4x3dvImplementation)(location, count, values);
}

void AbstractShaderProgram::uniformImplementationDefault(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* const values) {
    use();
    glUniformMatrix4x3dv(location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationSSO(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* const values) {
    glProgramUniformMatrix4x3dv(_id, location, count, GL_FALSE, values[0].data());
}

void AbstractShaderProgram::uniformImplementationDSAEXT(const GLint location, const GLsizei count, const Math::RectangularMatrix<4, 3, GLdouble>* const values) {
    glProgramUniformMatrix4x3dvEXT(_id, location, count, GL_FALSE, values[0].data());
}
#endif

}
