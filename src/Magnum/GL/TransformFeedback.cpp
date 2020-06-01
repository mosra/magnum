/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "TransformFeedback.h"

#ifndef MAGNUM_TARGET_GLES2
#include <tuple>
#include <Corrade/Utility/Assert.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TransformFeedbackState.h"

namespace Magnum { namespace GL {

Int TransformFeedback::maxInterleavedComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::transform_feedback>())
        return 0;
    #endif

    GLint& value = Context::current().state().transformFeedback->maxInterleavedComponents;

    if(value == 0)
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &value);

    return value;
}

Int TransformFeedback::maxSeparateAttributes() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::transform_feedback>())
        return 0;
    #endif

    GLint& value = Context::current().state().transformFeedback->maxSeparateAttributes;

    if(value == 0)
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &value);

    return value;
}

Int TransformFeedback::maxSeparateComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::transform_feedback>())
        return 0;
    #endif

    GLint& value = Context::current().state().transformFeedback->maxSeparateComponents;

    if(value == 0)
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &value);

    return value;
}

#ifndef MAGNUM_TARGET_GLES
Int TransformFeedback::maxBuffers() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback3>())
        return maxSeparateAttributes();

    GLint& value = Context::current().state().transformFeedback->maxBuffers;

    if(value == 0)
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &value);

    return value;
}

Int TransformFeedback::maxVertexStreams() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback3>())
        return 1;

    GLint& value = Context::current().state().transformFeedback->maxVertexStreams;

    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_STREAMS, &value);

    return value;
}
#endif

TransformFeedback::TransformFeedback(): _flags{ObjectFlag::DeleteOnDestruction} {
    (this->*Context::current().state().transformFeedback->createImplementation)();
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

void TransformFeedback::createImplementationDefault() {
    glGenTransformFeedbacks(1, &_id);
}

#ifndef MAGNUM_TARGET_GLES
void TransformFeedback::createImplementationDSA() {
    glCreateTransformFeedbacks(1, &_id);
    _flags |= ObjectFlag::Created;
}
#endif

TransformFeedback::~TransformFeedback() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    /* If bound, remove itself from state */
    GLuint& binding = Context::current().state().transformFeedback->binding;
    if(binding == _id) binding = 0;

    glDeleteTransformFeedbacks(1, &_id);
}

void TransformFeedback::bindInternal() {
    GLuint& bound = Context::current().state().transformFeedback->binding;

    /* Already bound, nothing to do */
    if(bound == _id) return;

    /* Bind the transform feedback otherwise, which will also finally create it */
    bound = _id;
    _flags |= ObjectFlag::Created;
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _id);
}

inline void TransformFeedback::createIfNotAlready() {
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glObjectLabel()) operate with IDs directly and they
       require the object to be created. Binding the transform feedback finally
       creates it. Also all EXT DSA functions implicitly create it. */
    bindInternal();
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

#ifndef MAGNUM_TARGET_WEBGL
std::string TransformFeedback::label() {
    createIfNotAlready();
    return Context::current().state().debug->getLabelImplementation(GL_TRANSFORM_FEEDBACK, _id);
}

TransformFeedback& TransformFeedback::setLabelInternal(const Containers::ArrayView<const char> label) {
    createIfNotAlready();
    Context::current().state().debug->labelImplementation(GL_TRANSFORM_FEEDBACK, _id, label);
    return *this;
}
#endif

TransformFeedback& TransformFeedback::attachBuffer(const UnsignedInt index, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    (this->*Context::current().state().transformFeedback->attachRangeImplementation)(index, buffer, offset, size);
    return *this;
}

TransformFeedback& TransformFeedback::attachBuffer(const UnsignedInt index, Buffer& buffer) {
    (this->*Context::current().state().transformFeedback->attachBaseImplementation)(index, buffer);
    return *this;
}

void TransformFeedback::attachImplementationFallback(const GLuint index, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    bindInternal();
    buffer.bind(Buffer::Target(GL_TRANSFORM_FEEDBACK_BUFFER), index, offset, size);
}

#ifndef MAGNUM_TARGET_GLES
void TransformFeedback::attachImplementationDSA(const GLuint index, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    glTransformFeedbackBufferRange(_id, index, buffer.id(), offset, size);
}
#endif

void TransformFeedback::attachImplementationFallback(const GLuint index, Buffer& buffer) {
    bindInternal();
    buffer.bind(Buffer::Target(GL_TRANSFORM_FEEDBACK_BUFFER), index);
}

#ifndef MAGNUM_TARGET_GLES
void TransformFeedback::attachImplementationDSA(const GLuint index, Buffer& buffer) {
    glTransformFeedbackBufferBase(_id, index, buffer.id());
}
#endif

/** @todoc const std::initializer_list makes Doxygen grumpy */
TransformFeedback& TransformFeedback::attachBuffers(const UnsignedInt firstIndex, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    (this->*Context::current().state().transformFeedback->attachRangesImplementation)(firstIndex, buffers);
    return *this;
}

/** @todoc const std::initializer_list makes Doxygen grumpy */
TransformFeedback& TransformFeedback::attachBuffers(const UnsignedInt firstIndex, std::initializer_list<Buffer*> buffers) {
    (this->*Context::current().state().transformFeedback->attachBasesImplementation)(firstIndex, buffers);
    return *this;
}

/** @todoc const std::initializer_list makes Doxygen grumpy */
void TransformFeedback::attachImplementationFallback(const GLuint firstIndex, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    bindInternal();
    Buffer::bind(Buffer::Target(GL_TRANSFORM_FEEDBACK_BUFFER), firstIndex, buffers);
}

#ifndef MAGNUM_TARGET_GLES
/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void TransformFeedback::attachImplementationDSA(const GLuint firstIndex, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    for(std::size_t i = 0; i != buffers.size(); ++i) {
        Buffer* buffer;
        GLintptr offset;
        GLsizeiptr size;
        std::tie(buffer, offset, size) = *(buffers.begin() + i);

        glTransformFeedbackBufferRange(_id, firstIndex + i, buffer ? buffer->id() : 0, offset, size);
    }
}
#endif

/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void TransformFeedback::attachImplementationFallback(const GLuint firstIndex, std::initializer_list<Buffer*> buffers) {
    bindInternal();
    Buffer::bind(Buffer::Target(GL_TRANSFORM_FEEDBACK_BUFFER), firstIndex, buffers);
}

#ifndef MAGNUM_TARGET_GLES
/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void TransformFeedback::attachImplementationDSA(const GLuint firstIndex, std::initializer_list<Buffer*> buffers) {
    for(std::size_t i = 0; i != buffers.size(); ++i)
        glTransformFeedbackBufferBase(_id, firstIndex + i, *(buffers.begin() + i) ? (*(buffers.begin() + i))->id() : 0);
}
#endif

void TransformFeedback::begin(AbstractShaderProgram& shader, const PrimitiveMode mode) {
    shader.use();
    bindInternal();
    glBeginTransformFeedback(GLenum(mode));
}

void TransformFeedback::pause() {
    bindInternal();
    glPauseTransformFeedback();
}

void TransformFeedback::resume() {
    bindInternal();
    glResumeTransformFeedback();
}

void TransformFeedback::end() {
    bindInternal();
    glEndTransformFeedback();
}

}}
#endif
