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
Buffer::SetDataImplementation Buffer::setDataImplementation = &Buffer::setDataImplementationDefault;
Buffer::SetSubDataImplementation Buffer::setSubDataImplementation = &Buffer::setSubDataImplementationDefault;
Buffer::MapImplementation Buffer::mapImplementation = &Buffer::mapImplementationDefault;
Buffer::MapRangeImplementation Buffer::mapRangeImplementation = &Buffer::mapRangeImplementationDefault;
Buffer::FlushMappedRangeImplementation Buffer::flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDefault;
Buffer::UnmapImplementation Buffer::unmapImplementation = &Buffer::unmapImplementationDefault;

void Buffer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Buffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        copyImplementation = &Buffer::copyImplementationDSA;
        setDataImplementation = &Buffer::setDataImplementationDSA;
        setSubDataImplementation = &Buffer::setSubDataImplementationDSA;
        mapImplementation = &Buffer::mapImplementationDSA;
        mapRangeImplementation = &Buffer::mapRangeImplementationDSA;
        flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDSA;
        unmapImplementation = &Buffer::unmapImplementationDSA;
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
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) return Implementation::BufferState::targetForIndex[i];

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

void Buffer::setDataImplementationDefault(GLsizeiptr size, const GLvoid* data, Buffer::Usage usage) {
    glBufferData(static_cast<GLenum>(bindInternal(_targetHint)), size, data, static_cast<GLenum>(usage));
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::setDataImplementationDSA(GLsizeiptr size, const GLvoid* data, Buffer::Usage usage) {
    glNamedBufferDataEXT(_id, size, data, static_cast<GLenum>(usage));
}
#endif

void Buffer::setSubDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(static_cast<GLenum>(bindInternal(_targetHint)), offset, size, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::setSubDataImplementationDSA(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glNamedBufferSubDataEXT(_id, offset, size, data);
}
#endif

void* Buffer::mapImplementationDefault(MapAccess access) {
    return glMapBuffer(static_cast<GLenum>(bindInternal(_targetHint)), GLenum(access));
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapImplementationDSA(MapAccess access) {
    return glMapNamedBufferEXT(_id, GLenum(access));
}
#endif

void* Buffer::mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access) {
    return glMapBufferRange(static_cast<GLenum>(bindInternal(_targetHint)), offset, length, GLenum(access));
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapRangeImplementationDSA(GLintptr offset, GLsizeiptr length, MapFlags access) {
    return glMapNamedBufferRangeEXT(_id, offset, length, GLenum(access));
}
#endif

void Buffer::flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length) {
    glFlushMappedBufferRange(static_cast<GLenum>(bindInternal(_targetHint)), offset, length);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::flushMappedRangeImplementationDSA(GLintptr offset, GLsizeiptr length) {
    glFlushMappedNamedBufferRangeEXT(_id, offset, length);
}
#endif

bool Buffer::unmapImplementationDefault() {
    return glUnmapBuffer(static_cast<GLenum>(bindInternal(_targetHint)));
}

#ifndef MAGNUM_TARGET_GLES
bool Buffer::unmapImplementationDSA() {
    return glUnmapNamedBufferEXT(_id);
}
#endif

}
