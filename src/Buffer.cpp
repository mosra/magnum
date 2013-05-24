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

#include "Buffer.h"

#include <Utility/Debug.h>

#include "Context.h"
#include "Extensions.h"
#include "Implementation/State.h"
#include "Implementation/BufferState.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
Buffer::CopyImplementation Buffer::copyImplementation = &Buffer::copyImplementationDefault;
#endif
Buffer::DataImplementation Buffer::dataImplementation = &Buffer::dataImplementationDefault;
Buffer::SubDataImplementation Buffer::subDataImplementation = &Buffer::subDataImplementationDefault;
Buffer::InvalidateImplementation Buffer::invalidateImplementation = &Buffer::invalidateImplementationNoOp;
Buffer::InvalidateSubImplementation Buffer::invalidateSubImplementation = &Buffer::invalidateSubImplementationNoOp;
#ifndef MAGNUM_TARGET_GLES3
Buffer::MapImplementation Buffer::mapImplementation = &Buffer::mapImplementationDefault;
#endif
Buffer::MapRangeImplementation Buffer::mapRangeImplementation = &Buffer::mapRangeImplementationDefault;
Buffer::FlushMappedRangeImplementation Buffer::flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDefault;
Buffer::UnmapImplementation Buffer::unmapImplementation = &Buffer::unmapImplementationDefault;

void Buffer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Buffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        copyImplementation = &Buffer::copyImplementationDSA;
        dataImplementation = &Buffer::dataImplementationDSA;
        subDataImplementation = &Buffer::subDataImplementationDSA;
        mapImplementation = &Buffer::mapImplementationDSA;
        mapRangeImplementation = &Buffer::mapRangeImplementationDSA;
        flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDSA;
        unmapImplementation = &Buffer::unmapImplementationDSA;
    }

    if(context->isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        Debug() << "Buffer: using" << Extensions::GL::ARB::invalidate_subdata::string() << "features";

        invalidateImplementation = &Buffer::invalidateImplementationARB;
        invalidateSubImplementation = &Buffer::invalidateSubImplementationARB;
    }
    #else
    static_cast<void>(context);
    #endif
}

Buffer::~Buffer() {
    GLuint* bindings = Context::current()->state()->buffer->bindings;

    /* Remove all current bindings from the state */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) bindings[i] = 0;

    glDeleteBuffers(1, &_id);
}

void Buffer::bind(Target target, GLuint id) {
    GLuint& bound = Context::current()->state()->buffer->bindings[Implementation::BufferState::indexForTarget(target)];

    /* Already bound, nothing to do */
    if(bound == id) return;

    /* Bind the buffer otherwise */
    bound = id;
    glBindBuffer(static_cast<GLenum>(target), id);
}

Buffer::Target Buffer::bindInternal(Target hint) {
    GLuint* bindings = Context::current()->state()->buffer->bindings;
    GLuint& hintBinding = bindings[Implementation::BufferState::indexForTarget(hint)];

    /* Shortcut - if already bound to hint, return */
    if(hintBinding == _id) return hint;

    /* Return first target in which the buffer is bound */
    /** @todo wtf there is one more? */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) return Implementation::BufferState::targetForIndex[i-1];

    /* Bind the buffer to hint target otherwise */
    hintBinding = _id;
    glBindBuffer(static_cast<GLenum>(hint), _id);
    return hint;
}

#ifndef MAGNUM_TARGET_GLES2
void Buffer::copyImplementationDefault(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glCopyBufferSubData(static_cast<GLenum>(read->bindInternal(Target::CopyRead)), static_cast<GLenum>(write->bindInternal(Target::CopyWrite)), readOffset, writeOffset, size);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::copyImplementationDSA(Buffer* read, Buffer* write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glNamedCopyBufferSubDataEXT(read->_id, write->_id, readOffset, writeOffset, size);
}
#endif
#endif

void Buffer::dataImplementationDefault(GLsizeiptr size, const GLvoid* data, Buffer::Usage usage) {
    glBufferData(static_cast<GLenum>(bindInternal(_targetHint)), size, data, static_cast<GLenum>(usage));
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::dataImplementationDSA(GLsizeiptr size, const GLvoid* data, Buffer::Usage usage) {
    glNamedBufferDataEXT(_id, size, data, static_cast<GLenum>(usage));
}
#endif

void Buffer::subDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(static_cast<GLenum>(bindInternal(_targetHint)), offset, size, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::subDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glNamedBufferSubDataEXT(_id, offset, size, data);
}
#endif

void Buffer::invalidateImplementationNoOp() {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateImplementationARB() {
    glInvalidateBufferData(_id);
}
#endif

void Buffer::invalidateSubImplementationNoOp(GLintptr, GLsizeiptr) {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateSubImplementationARB(GLintptr offset, GLsizeiptr length) {
    glInvalidateBufferSubData(_id, offset, length);
}
#endif

#ifndef MAGNUM_TARGET_GLES3
void* Buffer::mapImplementationDefault(MapAccess access) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    return glMapBuffer(static_cast<GLenum>(bindInternal(_targetHint)), GLenum(access));
    #else
    static_cast<void>(access);
    return nullptr;
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapImplementationDSA(MapAccess access) {
    return glMapNamedBufferEXT(_id, GLenum(access));
}
#endif
#endif

void* Buffer::mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    return glMapBufferRange(static_cast<GLenum>(bindInternal(_targetHint)), offset, length, GLenum(access));
    #else
    static_cast<void>(offset);
    static_cast<void>(length);
    static_cast<void>(access);
    return nullptr;
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapRangeImplementationDSA(GLintptr offset, GLsizeiptr length, MapFlags access) {
    return glMapNamedBufferRangeEXT(_id, offset, length, GLenum(access));
}
#endif

void Buffer::flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length) {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    glFlushMappedBufferRange(static_cast<GLenum>(bindInternal(_targetHint)), offset, length);
    #else
    static_cast<void>(offset);
    static_cast<void>(length);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::flushMappedRangeImplementationDSA(GLintptr offset, GLsizeiptr length) {
    glFlushMappedNamedBufferRangeEXT(_id, offset, length);
}
#endif

bool Buffer::unmapImplementationDefault() {
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES2
    return glUnmapBuffer(static_cast<GLenum>(bindInternal(_targetHint)));
    #else
    return false;
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool Buffer::unmapImplementationDSA() {
    return glUnmapNamedBufferEXT(_id);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, Buffer::Target value) {
    switch(value) {
        #define _c(value) case Buffer::Target::value: return debug << "Buffer::Target::" #value;
        _c(Array)
        #ifndef MAGNUM_TARGET_GLES
        _c(AtomicCounter)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(CopyRead)
        _c(CopyWrite)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(DispatchIndirect)
        _c(DrawIndirect)
        #endif
        _c(ElementArray)
        #ifndef MAGNUM_TARGET_GLES2
        _c(PixelPack)
        _c(PixelUnpack)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(ShaderStorage)
        _c(Texture)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(TransformFeedback)
        _c(Uniform)
        #endif
        #undef _c
    }

    return debug << "Buffer::Target::(invalid)";
}
#endif

}
