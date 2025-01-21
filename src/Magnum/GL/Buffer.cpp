/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>
    Copyright @ 2022 Hugo Amiard <hugo.amiard@wonderlandengine.com>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Triple.h>
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif
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

    GLint& value = Context::current().state().buffer.minMapAlignment;

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

    GLint& value = Context::current().state().buffer.maxAtomicCounterBindings;

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

    GLint& value = Context::current().state().buffer.maxShaderStorageBindings;

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

    GLint& value = Context::current().state().buffer.uniformOffsetAlignment;

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

    GLint& value = Context::current().state().buffer.shaderStorageOffsetAlignment;

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

    GLint& value = Context::current().state().buffer.maxUniformBindings;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);

    return value;
}

void Buffer::unbind(const Target target, const UnsignedInt index) {
    /* Inverse of what's in bind(Target, UnsignedInt, GLintptr, GLsizeiptr)
       below -- unbinding a buffer via glBindBufferBase() / glBindBufferRange()
       also unbinds it from the "regular" binding target as a side effect. */
    Context::current().state().buffer.bindings[Implementation::BufferState::indexForTarget(
        /* The Target enum is a subset of TargetHint and the values match, so
           no expensive translation is necessary */
        TargetHint(UnsignedInt(target)))] = 0;

    glBindBufferBase(GLenum(target), index, 0);
}

void Buffer::unbind(const Target target, const UnsignedInt firstIndex, const std::size_t count) {
    Context::current().state().buffer.bindBasesImplementation(target, firstIndex, {nullptr, count});
}

void Buffer::bind(const Target target, const UnsignedInt firstIndex, Containers::ArrayView<const Containers::Triple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    Context::current().state().buffer.bindRangesImplementation(target, firstIndex, {buffers.begin(), buffers.size()});
}

void Buffer::bind(const Target target, const UnsignedInt firstIndex, std::initializer_list<Containers::Triple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    Context::current().state().buffer.bindRangesImplementation(target, firstIndex, {buffers.begin(), buffers.size()});
}

void Buffer::bind(const Target target, const UnsignedInt firstIndex, Containers::ArrayView<Buffer* const> buffers) {
    Context::current().state().buffer.bindBasesImplementation(target, firstIndex, {buffers.begin(), buffers.size()});
}

void Buffer::copy(Buffer& read, Buffer& write, const GLintptr readOffset, const GLintptr writeOffset, const GLsizeiptr size) {
    Context::current().state().buffer.copyImplementation(read, write, readOffset, writeOffset, size);
}
#endif

Buffer::Buffer(const TargetHint targetHint): _flags{ObjectFlag::DeleteOnDestruction} {
    const Implementation::BufferState& state = Context::current().state().buffer;
    state.createImplementation(*this);
    state.setTargetHintImplementation(*this, targetHint);
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

Buffer::Buffer(GLuint id, TargetHint targetHint, ObjectFlags flags) noexcept: _id{id}, _flags{flags} {
    Context::current().state().buffer.setTargetHintImplementation(*this, targetHint);
}

void Buffer::createImplementationDefault(Buffer& self) {
    glGenBuffers(1, &self._id);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::createImplementationDSA(Buffer& self) {
    glCreateBuffers(1, &self._id);
    self._flags |= ObjectFlag::Created;
}
#endif

Buffer::~Buffer() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    GLuint* bindings = Context::current().state().buffer.bindings;

    /* Remove all current bindings from the state */
    for(std::size_t i = 1; i != Implementation::BufferState::TargetCount; ++i)
        if(bindings[i] == _id) bindings[i] = 0;

    glDeleteBuffers(1, &_id);
}

Buffer& Buffer::setTargetHint(TargetHint hint) {
    Context::current().state().buffer.setTargetHintImplementation(*this, hint);
    return *this;
}

void Buffer::setTargetHintImplementationDefault(Buffer& self, const TargetHint hint) {
    self._targetHint = hint;
}

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
void Buffer::setTargetHintImplementationSwiftShader(Buffer& self, const TargetHint hint) {
    /* See the "swiftshader-broken-xfb-buffer-binding-target" workaround for
       details */
    self._targetHint = hint == TargetHint::TransformFeedback ? TargetHint::Array : hint;
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
Containers::String Buffer::label() {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug.getLabelImplementation(GL_BUFFER, _id);
    #else
    return Context::current().state().debug.getLabelImplementation(GL_BUFFER_KHR, _id);
    #endif
}

Buffer& Buffer::setLabel(const Containers::StringView label) {
    createIfNotAlready();
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug.labelImplementation(GL_BUFFER, _id, label);
    #else
    Context::current().state().debug.labelImplementation(GL_BUFFER_KHR, _id, label);
    #endif
    return *this;
}
#endif

void Buffer::bindInternal(const TargetHint target, Buffer* const buffer) {
    const GLuint id = buffer ? buffer->_id : 0;
    GLuint& bound = Context::current().state().buffer.bindings[Implementation::BufferState::indexForTarget(target)];

    /* Already bound, nothing to do */
    if(bound == id) return;

    /* Bind the buffer otherwise, which will also finally create it */
    bound = id;
    if(buffer) buffer->_flags |= ObjectFlag::Created;
    glBindBuffer(GLenum(target), id);
}

auto Buffer::bindSomewhereInternal(const TargetHint hint) -> TargetHint {
    GLuint* bindings = Context::current().state().buffer.bindings;
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
        auto& currentVAO = Context::current().state().mesh.currentVAO;
        /* It can be also State::DisengagedBinding, in which case we unbind as
           well to be sure */
        if(currentVAO != 0)
            Context::current().state().mesh.bindVAOImplementation(0);
    }

    /* Bind the buffer to hint target otherwise */
    hintBinding = _id;
    _flags |= ObjectFlag::Created;
    glBindBuffer(GLenum(hint), _id);
    return hint;
}

#ifndef MAGNUM_TARGET_GLES2
Buffer& Buffer::bind(const Target target, const UnsignedInt index, const GLintptr offset, const GLsizeiptr size) {
    /* glBindBufferBase() and glBindBufferRange() bind the buffer to the
       "regular" binding target as a side effect:
        https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml
       So update the state tracker to be aware of that. Apart from saving a
       needless rebind in some cases, this also prevents an inverse case where
       it would think a buffer is bound and won't call glBindBuffer() for it,
       causing for example a (DSA-less) data upload to happen to some entirely
       different buffer. In comparison, the multi-bind APIs don't have this
       side effect:
        https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffersBase.xhtml

       See BufferGLTest::bindBaseRangeUpdateRegularBinding() for a
       corresponding test case. */
    Context::current().state().buffer.bindings[Implementation::BufferState::indexForTarget(
        /* The Target enum is a subset of TargetHint and the values match, so
           no expensive translation is necessary */
        TargetHint(UnsignedInt(target)))] = _id;
    /* As the "regular" binding target is a side effect, I assume it also
       creates the object internally if not already, equivalently to
       glBindBuffer() and to what the createIfNotAlready() call does, and
       satisfying the internal assertion there which in turn expects that if a
       buffer is in the bindings state tracker array, it also has the Created
       flag set. */
    _flags |= ObjectFlag::Created;

    glBindBufferRange(GLenum(target), index, _id, offset, size);
    return *this;
}

Buffer& Buffer::bind(const Target target, const UnsignedInt index) {
    /* Same as in bind(Target, UnsignedInt, GLintptr, GLsizeiptr) above */
    Context::current().state().buffer.bindings[Implementation::BufferState::indexForTarget(
        /* The Target enum is a subset of TargetHint and the values match, so
           no expensive translation is necessary */
        TargetHint(UnsignedInt(target)))] = _id;
    /* Ditto */
    _flags |= ObjectFlag::Created;

    glBindBufferBase(GLenum(target), index, _id);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Buffer& Buffer::setStorage(const Containers::ArrayView<const void> data, const StorageFlags flags) {
    Context::current().state().buffer.storageImplementation(*this, data, flags);
    return *this;
}

Buffer& Buffer::setStorage(const std::size_t size, const StorageFlags flags) {
    return setStorage({nullptr, size}, flags);
}
#endif

Int Buffer::size() {
    /**
     * @todo there is something like glGetBufferParameteri64v in 3.2 (I
     *      couldn't find any matching extension, though)
     */
    GLint size;
    Context::current().state().buffer.getParameterImplementation(*this, GL_BUFFER_SIZE, &size);
    return size;
}

#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20017)
Containers::Array<char> Buffer::data() {
    return subData(0, size());
}
#endif

Buffer& Buffer::setData(const Containers::ArrayView<const void> data, const BufferUsage usage) {
    Context::current().state().buffer.dataImplementation(*this, data.size(), data, usage);
    return *this;
}

Buffer& Buffer::setSubData(const GLintptr offset, const Containers::ArrayView<const void> data) {
    Context::current().state().buffer.subDataImplementation(*this, offset, data.size(), data);
    return *this;
}

Buffer& Buffer::invalidateData() {
    Context::current().state().buffer.invalidateImplementation(*this);
    return *this;
}

Buffer& Buffer::invalidateSubData(const GLintptr offset, const GLsizeiptr length) {
    Context::current().state().buffer.invalidateSubImplementation(*this, offset, length);
    return *this;
}

#ifndef MAGNUM_TARGET_WEBGL
char* Buffer::map(const MapAccess access) {
    return static_cast<char*>(Context::current().state().buffer.mapImplementation(*this, access));
}

Containers::ArrayView<char> Buffer::map(const GLintptr offset, const GLsizeiptr length, const MapFlags flags) {
    return {static_cast<char*>(Context::current().state().buffer.mapRangeImplementation(*this, offset, length, flags)), std::size_t(length)};
}

Buffer& Buffer::flushMappedRange(const GLintptr offset, const GLsizeiptr length) {
    Context::current().state().buffer.flushMappedRangeImplementation(*this, offset, length);
    return *this;
}

bool Buffer::unmap() { return Context::current().state().buffer.unmapImplementation(*this); }
#endif

#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20017)
Containers::Array<char> Buffer::subData(const GLintptr offset, const GLsizeiptr size) {
    Containers::Array<char> data{NoInit, std::size_t(size)};
    if(size) Context::current().state().buffer.getSubDataImplementation(*this, offset, size, data);
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

    /* Unlike Buffer::bind(Target, UnsignedInt) this doesn't affect the regular
       binding points:
        https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffersBase.xhtml
       See the comment in that function for details. */
    glBindBuffersBase(GLenum(target), firstIndex, buffers.size(), ids);
}
#endif

void Buffer::bindImplementationFallback(const Target target, const GLuint firstIndex, Containers::ArrayView<const Containers::Triple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(buffers && buffers[i].first())
            buffers[i].first()->bind(target, firstIndex + i, buffers[i].second(), buffers[i].third());
        else unbind(target, firstIndex + i);
    }
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::bindImplementationMulti(const Target target, const GLuint firstIndex, Containers::ArrayView<const Containers::Triple<Buffer*, GLintptr, GLsizeiptr>> buffers) {
    /** @todo use ArrayTuple */
    Containers::Array<GLuint> ids{buffers ? buffers.size() : 0};
    Containers::Array<GLintptr> offsetsSizes{buffers ? buffers.size()*2 : 0};
    if(buffers) for(std::size_t i = 0; i != buffers.size(); ++i) {
        if(buffers[i].first()) {
            buffers[i].first()->createIfNotAlready();
            ids[i] = buffers[i].first()->_id;
            offsetsSizes[i] = buffers[i].second();
            offsetsSizes[buffers.size() + i] = buffers[i].third();
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void Buffer::storageImplementationDefault(Buffer& self, Containers::ArrayView<const void> data, StorageFlags flags) {
    #ifndef MAGNUM_TARGET_GLES
    glBufferStorage
    #else
    glBufferStorageEXT
    #endif
        (GLenum(self.bindSomewhereInternal(self._targetHint)), data.size(), data.data(), GLbitfield(flags));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::storageImplementationDSA(Buffer& self, Containers::ArrayView<const void> data, const StorageFlags flags) {
    glNamedBufferStorage(self._id, data.size(), data.data(), GLbitfield(flags));
}
#endif

void Buffer::getParameterImplementationDefault(Buffer& self, const GLenum value, GLint* const data) {
    glGetBufferParameteriv(GLenum(self.bindSomewhereInternal(self._targetHint)), value, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::getParameterImplementationDSA(Buffer& self, const GLenum value, GLint* const data) {
    glGetNamedBufferParameteriv(self._id, value, data);
}
#endif

#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20017)
void Buffer::getSubDataImplementationDefault(Buffer& self, const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetBufferSubData(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, size, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Buffer::getSubDataImplementationDSA(Buffer& self, const GLintptr offset, const GLsizeiptr size, GLvoid* const data) {
    glGetNamedBufferSubData(self._id, offset, size, data);
}
#endif

void Buffer::dataImplementationDefault(Buffer& self, GLsizeiptr size, const GLvoid* data, BufferUsage usage) {
    glBufferData(GLenum(self.bindSomewhereInternal(self._targetHint)), size, data, GLenum(usage));
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::dataImplementationDSA(Buffer& self, const GLsizeiptr size, const GLvoid* const data, const BufferUsage usage) {
    glNamedBufferData(self._id, size, data, GLenum(usage));
}
#endif

void Buffer::subDataImplementationDefault(Buffer& self, GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, size, data);
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::subDataImplementationDSA(Buffer& self, const GLintptr offset, const GLsizeiptr size, const GLvoid* const data) {
    glNamedBufferSubData(self._id, offset, size, data);
}
#endif

void Buffer::invalidateImplementationNoOp(Buffer&) {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateImplementationARB(Buffer& self) {
    self.createIfNotAlready();
    glInvalidateBufferData(self._id);
}
#endif

void Buffer::invalidateSubImplementationNoOp(Buffer&, GLintptr, GLsizeiptr) {}

#ifndef MAGNUM_TARGET_GLES
void Buffer::invalidateSubImplementationARB(Buffer& self, GLintptr offset, GLsizeiptr length) {
    self.createIfNotAlready();
    glInvalidateBufferSubData(self._id, offset, length);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
void* Buffer::mapImplementationDefault(Buffer& self, MapAccess access) {
    #ifndef MAGNUM_TARGET_GLES
    return glMapBuffer(GLenum(self.bindSomewhereInternal(self._targetHint)), GLenum(access));
    #else
    return glMapBufferOES(GLenum(self.bindSomewhereInternal(self._targetHint)), GLenum(access));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapImplementationDSA(Buffer& self, const MapAccess access) {
    return glMapNamedBuffer(self._id, GLenum(access));
}
#endif

void* Buffer::mapRangeImplementationDefault(Buffer& self, GLintptr offset, GLsizeiptr length, MapFlags access) {
    #ifndef MAGNUM_TARGET_GLES2
    return glMapBufferRange(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, length, GLenum(access));
    #else
    return glMapBufferRangeEXT(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, length, GLenum(access));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void* Buffer::mapRangeImplementationDSA(Buffer& self, const GLintptr offset, const GLsizeiptr length, const MapFlags access) {
    return glMapNamedBufferRange(self._id, offset, length, GLenum(access));
}
#endif

void Buffer::flushMappedRangeImplementationDefault(Buffer& self, GLintptr offset, GLsizeiptr length) {
    #ifndef MAGNUM_TARGET_GLES2
    glFlushMappedBufferRange(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, length);
    #else
    glFlushMappedBufferRangeEXT(GLenum(self.bindSomewhereInternal(self._targetHint)), offset, length);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Buffer::flushMappedRangeImplementationDSA(Buffer& self, const GLintptr offset, const GLsizeiptr length) {
    glFlushMappedNamedBufferRange(self._id, offset, length);
}
#endif

bool Buffer::unmapImplementationDefault(Buffer& self) {
    #ifndef MAGNUM_TARGET_GLES2
    return glUnmapBuffer(GLenum(self.bindSomewhereInternal(self._targetHint)));
    #else
    return glUnmapBufferOES(GLenum(self.bindSomewhereInternal(self._targetHint)));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool Buffer::unmapImplementationDSA(Buffer& self) {
    return glUnmapNamedBuffer(self._id);
}
#endif
#endif

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
/* See apple-buffer-texture-detach-on-data-modify for the gory details. */
void Buffer::textureWorkaroundAppleBefore() {
    /* My Mac Mini reports 80 texture units, which means this thing can have a
       pretty significant overhead. Skipping the whole thing if no buffer
       texture is known to be bound. */
    Implementation::TextureState& textureState = Context::current().state().texture;
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
        textureState.bindings[textureUnit] = {};
        textureState.bufferTextureBound.set(textureUnit, false);
    }
}

void Buffer::dataImplementationApple(Buffer& self, const GLsizeiptr size, const GLvoid* const data, const BufferUsage usage) {
    self.textureWorkaroundAppleBefore();
    dataImplementationDefault(self, size, data, usage);
}

void Buffer::subDataImplementationApple(Buffer& self, const GLintptr offset, const GLsizeiptr size, const GLvoid* const data) {
    self.textureWorkaroundAppleBefore();
    subDataImplementationDefault(self, offset, size, data);
}

void* Buffer::mapImplementationApple(Buffer& self, const MapAccess access) {
    self.textureWorkaroundAppleBefore();
    return mapImplementationDefault(self, access);
}

void* Buffer::mapRangeImplementationApple(Buffer& self, const GLintptr offset, const GLsizeiptr length, const MapFlags access) {
    self.textureWorkaroundAppleBefore();
    return mapRangeImplementationDefault(self, offset, length, access);
}

bool Buffer::unmapImplementationApple(Buffer& self) {
    self.textureWorkaroundAppleBefore();
    return unmapImplementationDefault(self);
}
#endif

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

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
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

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}
#endif

}}
