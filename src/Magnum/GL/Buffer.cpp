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

#include "Buffer.h"

#include <tuple>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/BufferState.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/MeshState.h"

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
#include "Magnum/GL/Implementation/TextureState.h"
#endif

namespace Magnum { namespace GL {

#ifndef MAGNUM_TARGET_GLES
Int Buffer::minMapAlignment() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::map_buffer_alignment>())
        return 1;

    GLint& value = Context::current().state().buffer->minMapAlignment;

    if(value == 0)
        glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
#ifndef MAGNUM_TARGET_WEBGL
Int Buffer::maxAtomicCounterBindings() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().buffer->maxAtomicCounterBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &value);

    return value;
}

Int Buffer::maxShaderStorageBindings() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_storage_buffer_object>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().buffer->maxShaderStorageBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &value);

    return value;
}
#endif

Int Buffer::uniformOffsetAlignment() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        return 1;
    #endif

    GLint& value = Context::current().state().buffer->uniformOffsetAlignment;

    if(value == 0)
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value);

    return value;
}

#ifndef MAGNUM_TARGET_WEBGL
Int Buffer::shaderStorageOffsetAlignment() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_storage_buffer_object>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 1;

    GLint& value = Context::current().state().buffer->shaderStorageOffsetAlignment;

    if(value == 0)
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &value);

    return value;
}
#endif

Int Buffer::maxUniformBindings() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current().state().buffer->maxUniformBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);

    return value;
}

void Buffer::unbind(const Target target, const UnsignedInt index) {
    glBindBufferBase(GLenum(target), index, 0);
}

void Buffer::unbind(const Target target, const UnsignedInt firstIndex, const std::size_t count) {
    Context::current().state().buffer->bindBasesImplementation(target, firstIndex, {nullptr, count});
}

/** @todoc const std::initializer_list makes Doxygen grumpy */
void Buffer::bind(const Target target, const UnsignedInt firstIndex, std::initializer_list<std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    Context::current().state().buffer->bindRangesImplementation(target, firstIndex, {buffers.begin(), buffers.size()});
}

/** @todoc const std::initializer_list makes Doxygen grumpy */
void Buffer::bind(const Target target, const UnsignedInt firstIndex, std::initializer_list<Buffer*> buffers) {
    Context::current().state().buffer->bindBasesImplementation(target, firstIndex, {buffers.begin(), buffers.size()});
}

void Buffer::copy(Buffer& read, Buffer& write, const GLintptr readOffset, const GLintptr writeOffset, const GLsizeiptr size) {
    Context::current().state().buffer->copyImplementation(read, write, readOffset, writeOffset, size);
}
#endif

Buffer::Buffer(const TargetHint targetHint): _flags{ObjectFlag::DeleteOnDestruction} {
    Implementation::BufferState& state = *Context::current().state().buffer;
    (this->*state.createImplementation)();
    (this->*state.setTargetHintImplementation)(targetHint);
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

Buffer::Buffer(GLuint id, TargetHint targetHint, ObjectFlags flags) noexcept: _id{id}, _flags{flags} {
    (this->*Context::current().state().buffer->setTargetHintImplementation)(targetHint);
}

void Buffer::createImplementationDefault() {
    glGenBuffers(1, &_id);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::createImplementationDSA() {
    glCreateBuffers(1, &_id);
    _flags |= ObjectFlag::Created;
}
#endif

Buffer::~Buffer() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    GLuint* bindings = Context::current().state().buffer->bindings;

    /* Remove all current bindings from the state */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) bindings[i] = 0;

    glDeleteBuffers(1, &_id);
}

Buffer& Buffer::setTargetHint(TargetHint hint) {
    (this->*Context::current().state().buffer->setTargetHintImplementation)(hint);
    return *this;
}

void Buffer::setTargetHintImplementationDefault(const TargetHint hint) {
    _targetHint = hint;
}

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
void Buffer::setTargetHintImplementationSwiftShader(const TargetHint hint) {
    /* See the "swiftshader-broken-xfb-buffer-binding-target" workaround for
       details */
    _targetHint = hint == TargetHint::TransformFeedback ? TargetHint::Array : hint;
}
#endif

void Buffer::createIfNotAlready() {
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glInvalidateBufferData() or glObjectLabel()) operate
       with IDs directly and they require the object to be created. Binding the
       buffer finally creates it. Also all EXT DSA functions implicitly create
       it. */
    bindSomewhereInternal(_targetHint);
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

#ifndef MAGNUM_TARGET_WEBGL
std::string Buffer::label() {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug->getLabelImplementation(GL_BUFFER, _id);
    #else
    return Context::current().state().debug->getLabelImplementation(GL_BUFFER_KHR, _id);
    #endif
}

Buffer& Buffer::setLabelInternal(const Containers::ArrayView<const char> label) {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug->labelImplementation(GL_BUFFER, _id, label);
    #else
    Context::current().state().debug->labelImplementation(GL_BUFFER_KHR, _id, label);
    #endif
    return *this;
}
#endif

void Buffer::bindInternal(const TargetHint target, Buffer* const buffer) {
    const GLuint id = buffer ? buffer->_id : 0;
    GLuint& bound = Context::current().state().buffer->bindings[Implementation::BufferState::indexForTarget(target)];

    /* Already bound, nothing to do */
    if(bound == id) return;

    /* Bind the buffer otherwise, which will also finally create it */
    bound = id;
    if(buffer) buffer->_flags |= ObjectFlag::Created;
    glBindBuffer(GLenum(target), id);
}

auto Buffer::bindSomewhereInternal(const TargetHint hint) -> TargetHint {
    GLuint* bindings = Context::current().state().buffer->bindings;
    GLuint& hintBinding = bindings[Implementation::BufferState::indexForTarget(hint)];

    /* Shortcut - if already bound to hint, return */
    if(hintBinding == _id) return hint;

    /* Return first target in which the buffer is bound */
    /** @todo wtf there is one more? */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) return Implementation::BufferState::targetForIndex[i-1];

    /* Sorry, this is ugly because GL is also ugly. Blame GL, not me.

       If the buffer target hint is ElementArray and some VAO is bound (or our
       state tracker is not sure), we have to unbind the VAO first in order to
       prevent accidental modification of that VAO. See
       Test::MeshGLTest::unbindVAOwhenSettingIndexBufferData() for details. */
    if(hint == TargetHint::ElementArray) {
        auto& currentVAO = Context::current().state().mesh->currentVAO;
        /* It can be also State::DisengagedBinding, in which case we unbind as
           well to be sure */
        if(currentVAO != 0)
            Context::current().state().mesh->bindVAOImplementation(0);
    }

    /* Bind the buffer to hint target otherwise */
    hintBinding = _id;
    _flags |= ObjectFlag::Created;
    glBindBuffer(GLenum(hint), _id);
    return hint;
}

#ifndef MAGNUM_TARGET_GLES2
Buffer& Buffer::bind(const Target target, const UnsignedInt index, const GLintptr offset, const GLsizeiptr size) {
    glBindBufferRange(GLenum(target), index, _id, offset, size);
    return *this;
}

Buffer& Buffer::bind(const Target target, const UnsignedInt index) {
    glBindBufferBase(GLenum(target), index, _id);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Buffer& Buffer::setStorage(const Containers::ArrayView<const void> data, const StorageFlags flags) {
    (this->*Context::current().state().buffer->storageImplementation)(data, flags);
    return *this;
}
#endif

Int Buffer::size() {
    /**
     * @todo there is something like glGetBufferParameteri64v in 3.2 (I
     *      couldn't find any matching extension, though)
     */
    GLint size;
    (this->*Context::current().state().buffer->getParameterImplementation)(GL_BUFFER_SIZE, &size);
    return size;
}

#ifndef MAGNUM_TARGET_GLES
Containers::Array<char> Buffer::data() {
    return subData(0, size());
}
#endif

Buffer& Buffer::setData(const Containers::ArrayView<const void> data, const BufferUsage usage) {
    (this->*Context::current().state().buffer->dataImplementation)(data.size(), data, usage);
    return *this;
}

Buffer& Buffer::setSubData(const GLintptr offset, const Containers::ArrayView<const void> data) {
    (this->*Context::current().state().buffer->subDataImplementation)(offset, data.size(), data);
    return *this;
}

Buffer& Buffer::invalidateData() {
    (this->*Context::current().state().buffer->invalidateImplementation)();
    return *this;
}

Buffer& Buffer::invalidateSubData(const GLintptr offset, const GLsizeiptr length) {
    (this->*Context::current().state().buffer->invalidateSubImplementation)(offset, length);
    return *this;
}

#ifndef MAGNUM_TARGET_WEBGL
char* Buffer::map(const MapAccess access) {
    return static_cast<char*>((this->*Context::current().state().buffer->mapImplementation)(access));
}

Containers::ArrayView<char> Buffer::map(const GLintptr offset, const GLsizeiptr length, const MapFlags flags) {
    return {static_cast<char*>((this->*Context::current().state().buffer->mapRangeImplementation)(offset, length, flags)), std::size_t(length)};
}

Buffer& Buffer::flushMappedRange(const GLintptr offset, const GLsizeiptr length) {
    (this->*Context::current().state().buffer->flushMappedRangeImplementation)(offset, length);
    return *this;
}

bool Buffer::unmap() { return (this->*Context::current().state().buffer->unmapImplementation)(); }
#endif

#ifndef MAGNUM_TARGET_GLES
Containers::Array<char> Buffer::subData(const GLintptr offset, const GLsizeiptr size) {
    Containers::Array<char> data(size);
    if(size) (this->*Context::current().state().buffer->getSubDataImplementation)(offset, size, data);
    return data;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Buffer::bindImplementationFallback(const Target target, const GLuint firstIndex, Containers::ArrayView<Buffer* const> buffers) {
    for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(buffers && buffers[i]) buffers[i]->bind(target, firstIndex + i);
        else unbind(target, firstIndex + i);
    }
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::bindImplementationMulti(const Target target, const GLuint firstIndex, Containers::ArrayView<Buffer* const> buffers) {
    /** @todo C++1z: VLAs? */
    Containers::Array<GLuint> ids{buffers ? buffers.size() : 0};
    if(buffers) for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(buffers[i]) {
            buffers[i]->createIfNotAlready();
            ids[i] = buffers[i]->_id;
        } else {
            ids[i] = 0;
        }
    }

    glBindBuffersBase(GLenum(target), firstIndex, buffers.size(), ids);
}
#endif

/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void Buffer::bindImplementationFallback(const Target target, const GLuint firstIndex, Containers::ArrayView<const std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(buffers && std::get<0>(buffers[i]))
            std::get<0>(buffers[i])->bind(target, firstIndex + i, std::get<1>(buffers[i]), std::get<2>(buffers[i]));
        else unbind(target, firstIndex + i);
    }
}

#ifndef MAGNUM_TARGET_GLES
/** @todoc const Containers::ArrayView makes Doxygen grumpy */
void Buffer::bindImplementationMulti(const Target target, const GLuint firstIndex, Containers::ArrayView<const std::tuple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    /** @todo use ArrayTuple */
    Containers::Array<GLuint> ids{buffers ? buffers.size() : 0};
    Containers::Array<GLintptr> offsetsSizes{buffers ? buffers.size()*2 : 0};
    if(buffers) for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(std::get<0>(buffers[i])) {
            std::get<0>(buffers[i])->createIfNotAlready();
            ids[i] = std::get<0>(buffers[i])->_id;
            std::tie(std::ignore, offsetsSizes[i], offsetsSizes[buffers.size() + i]) = buffers[i];
        } else {
            ids[i] = 0;
            offsetsSizes[i] = 0;
            /** @todo fix workaround when NVidia 343.13 accepts zero sizes */
            offsetsSizes[buffers.size() + i] = 1;
        }
    }

    glBindBuffersRange(GLenum(target), firstIndex, buffers.size(), ids, offsetsSizes, offsetsSizes + buffers.size());
}
#endif

void Buffer::copyImplementationDefault(Buffer& read, Buffer& write, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    glCopyBufferSubData(GLenum(read.bindSomewhereInternal(TargetHint::CopyRead)), GLenum(write.bindSomewhereInternal(TargetHint::CopyWrite)), readOffset, writeOffset, size);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::copyImplementationDSA(Buffer& read, Buffer& write, const GLintptr readOffset, const GLintptr writeOffset, const GLsizeiptr size) {
    glCopyNamedBufferSubData(read._id, write._id, readOffset, writeOffset, size);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::storageImplementationDefault(Containers::ArrayView<const void> data, StorageFlags flags) {
    glBufferStorage(GLenum(bindSomewhereInternal(_targetHint)), data.size(), data.data(), GLbitfield(flags));
}

void Buffer::storageImplementationDSA(Containers::ArrayView<const void> data, const StorageFlags flags) {
    glNamedBufferStorage(_id, data.size(), data.data(), GLbitfield(flags));
}
#endif

void Buffer::getParameterImplementationDefault(const GLenum value, GLint* const data) {
    glGetBufferParameteriv(GLenum(bindSomewhereInternal(_targetHint)), value, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::getParameterImplementationDSA(const GLenum value, GLint* const data) {
    glGetNamedBufferParameteriv(_id, value, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::getSubDataImplementationDefault(const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetBufferSubData(GLenum(bindSomewhereInternal(_targetHint)), offset, size, data);
}

void Buffer::getSubDataImplementationDSA(const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetNamedBufferSubData(_id, offset, size, data);
}
#endif

void Buffer::dataImplementationDefault(GLsizeiptr size, const GLvoid* data, BufferUsage usage) {
    glBufferData(GLenum(bindSomewhereInternal(_targetHint)), size, data, GLenum(usage));
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::dataImplementationDSA(const GLsizeiptr size, const GLvoid* const data, const BufferUsage usage) {
    glNamedBufferData(_id, size, data, GLenum(usage));
}
#endif

void Buffer::subDataImplementationDefault(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(GLenum(bindSomewhereInternal(_targetHint)), offset, size, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::subDataImplementationDSA(const GLintptr offset, const GLsizeiptr size, const GLvoid* const data) {
    glNamedBufferSubData(_id, offset, size, data);
}
#endif

void Buffer::invalidateImplementationNoOp() {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateImplementationARB() {
    createIfNotAlready();
    glInvalidateBufferData(_id);
}
#endif

void Buffer::invalidateSubImplementationNoOp(GLintptr, GLsizeiptr) {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateSubImplementationARB(GLintptr offset, GLsizeiptr length) {
    createIfNotAlready();
    glInvalidateBufferSubData(_id, offset, length);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
void* Buffer::mapImplementationDefault(MapAccess access) {
    #ifndef MAGNUM_TARGET_GLES
    return glMapBuffer(GLenum(bindSomewhereInternal(_targetHint)), GLenum(access));
    #else
    return glMapBufferOES(GLenum(bindSomewhereInternal(_targetHint)), GLenum(access));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapImplementationDSA(const MapAccess access) {
    return glMapNamedBuffer(_id, GLenum(access));
}
#endif

void* Buffer::mapRangeImplementationDefault(GLintptr offset, GLsizeiptr length, MapFlags access) {
    #ifndef MAGNUM_TARGET_GLES2
    return glMapBufferRange(GLenum(bindSomewhereInternal(_targetHint)), offset, length, GLenum(access));
    #else
    return glMapBufferRangeEXT(GLenum(bindSomewhereInternal(_targetHint)), offset, length, GLenum(access));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapRangeImplementationDSA(const GLintptr offset, const GLsizeiptr length, const MapFlags access) {
    return glMapNamedBufferRange(_id, offset, length, GLenum(access));
}
#endif

void Buffer::flushMappedRangeImplementationDefault(GLintptr offset, GLsizeiptr length) {
    #ifndef MAGNUM_TARGET_GLES2
    glFlushMappedBufferRange(GLenum(bindSomewhereInternal(_targetHint)), offset, length);
    #else
    glFlushMappedBufferRangeEXT(GLenum(bindSomewhereInternal(_targetHint)), offset, length);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::flushMappedRangeImplementationDSA(const GLintptr offset, const GLsizeiptr length) {
    glFlushMappedNamedBufferRange(_id, offset, length);
}
#endif

bool Buffer::unmapImplementationDefault() {
    #ifndef MAGNUM_TARGET_GLES2
    return glUnmapBuffer(GLenum(bindSomewhereInternal(_targetHint)));
    #else
    return glUnmapBufferOES(GLenum(bindSomewhereInternal(_targetHint)));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool Buffer::unmapImplementationDSA() {
    return glUnmapNamedBuffer(_id);
}
#endif
#endif

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
/* See apple-buffer-texture-detach-on-data-modify for the gory details. */
void Buffer::textureWorkaroundAppleBefore() {
    /* My Mac Mini reports 80 texture units, which means this thing can have a
       pretty significant overhead. Skipping the whole thing if no buffer
       texture is known to be bound. */
    Implementation::TextureState& textureState = *Context::current().state().texture;
    if(textureState.bufferTextureBound.none()) return;
    for(GLint textureUnit = 0; textureUnit != GLint(textureState.bindings.size()); ++textureUnit) {
        /* Checking just
            textureState.bindings[textureUnit].first != GL_TEXTURE_BUFFER
           isn't enough, as for GL allows to bind different texture types under
           the same texture unit. Magnum's state tracker ignores that (as it
           would mean having to maintain a state cache of 128 units times 12
           targets) and so this state is tracked separately. */
        if(!textureState.bufferTextureBound[textureUnit]) continue;

        /* Activate given texture unit if not already active, update state
           tracker */
        if(textureState.currentTextureUnit != textureUnit)
            glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = textureUnit));

        /* Unbind the texture, reset state tracker */
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        /* libstdc++ since GCC 6.3 can't handle just = {} (ambiguous overload
           of operator=) */
        textureState.bindings[textureUnit] = std::pair<GLenum, GLuint>{};
        textureState.bufferTextureBound.set(textureUnit, false);
    }
}

void Buffer::dataImplementationApple(const GLsizeiptr size, const GLvoid* const data, const BufferUsage usage) {
    textureWorkaroundAppleBefore();
    dataImplementationDefault(size, data, usage);
}

void Buffer::subDataImplementationApple(const GLintptr offset, const GLsizeiptr size, const GLvoid* const data) {
    textureWorkaroundAppleBefore();
    subDataImplementationDefault(offset, size, data);
}

void* Buffer::mapImplementationApple(const MapAccess access) {
    textureWorkaroundAppleBefore();
    return mapImplementationDefault(access);
}

void* Buffer::mapRangeImplementationApple(const GLintptr offset, const GLsizeiptr length, const MapFlags access) {
    textureWorkaroundAppleBefore();
    return mapRangeImplementationDefault(offset, length, access);
}

bool Buffer::unmapImplementationApple() {
    textureWorkaroundAppleBefore();
    return unmapImplementationDefault();
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Buffer::TargetHint value) {
    debug << "GL::Buffer::TargetHint" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Buffer::TargetHint::value: return debug << "::" #value;
        _c(Array)
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        _c(AtomicCounter)
        #endif
        _c(CopyRead)
        _c(CopyWrite)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(DispatchIndirect)
        _c(DrawIndirect)
        #endif
        #endif
        _c(ElementArray)
        #ifndef MAGNUM_TARGET_GLES2
        _c(PixelPack)
        _c(PixelUnpack)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ShaderStorage)
        #endif
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(Texture)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(TransformFeedback)
        _c(Uniform)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}

#ifndef MAGNUM_TARGET_GLES2
Debug& operator<<(Debug& debug, const Buffer::Target value) {
    debug << "GL::Buffer::Target" << Debug::nospace;

    switch(value) {
        #define _c(value) case Buffer::Target::value: return debug << "::" #value;
        #ifndef MAGNUM_TARGET_WEBGL
        _c(AtomicCounter)
        _c(ShaderStorage)
        #endif
        _c(Uniform)
        #undef _c
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif
#endif

}}
