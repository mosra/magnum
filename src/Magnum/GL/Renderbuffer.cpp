/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Renderbuffer.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/FramebufferState.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL {

Int Renderbuffer::maxSize() {
    GLint& value = Context::current().state().framebuffer.maxRenderbufferSize;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &value);

    return value;
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
Int Renderbuffer::maxSamples() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::framebuffer_multisample>() && !Context::current().isExtensionSupported<Extensions::NV::framebuffer_multisample>())
        return 0;
    #endif

    GLint& value = Context::current().state().framebuffer.maxSamples;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_SAMPLES, &value);
        #else
        glGetIntegerv(GL_MAX_SAMPLES_NV, &value);
        #endif
    }

    return value;
}
#endif

Renderbuffer::Renderbuffer(): _flags{ObjectFlag::DeleteOnDestruction} {
    Context::current().state().framebuffer.createRenderbufferImplementation(*this);
}

void Renderbuffer::createImplementationDefault(Renderbuffer& self) {
    glGenRenderbuffers(1, &self._id);
}

#ifndef MAGNUM_TARGET_GLES
void Renderbuffer::createImplementationDSA(Renderbuffer& self) {
    glCreateRenderbuffers(1, &self._id);
    self._flags |= ObjectFlag::Created;
}
#endif

Renderbuffer::~Renderbuffer() {
    /* Moved out, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    /* If bound, remove itself from state */
    GLuint& binding = Context::current().state().framebuffer.renderbufferBinding;
    if(binding == _id) binding = 0;

    glDeleteRenderbuffers(1, &_id);
}

inline void Renderbuffer::createIfNotAlready() {
    if(_flags & ObjectFlag::Created) return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glObjectLabel()) operate with IDs directly and they
       require the object to be created. Binding the renderbuffer finally
       creates it. Also all EXT DSA functions implicitly create it. */
    bind();
    CORRADE_INTERNAL_ASSERT(_flags & ObjectFlag::Created);
}

#ifndef MAGNUM_TARGET_WEBGL
Containers::String Renderbuffer::label() {
    createIfNotAlready();
    return Context::current().state().debug.getLabelImplementation(GL_RENDERBUFFER, _id);
}

Renderbuffer& Renderbuffer::setLabel(const Containers::StringView label) {
    createIfNotAlready();
    Context::current().state().debug.labelImplementation(GL_RENDERBUFFER, _id, label);
    return *this;
}
#endif

void Renderbuffer::setStorage(const RenderbufferFormat internalFormat, const Vector2i& size) {
    Context::current().state().framebuffer.renderbufferStorageImplementation(*this, internalFormat, size);
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void Renderbuffer::setStorageMultisample(const Int samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    Context::current().state().framebuffer.renderbufferStorageMultisampleImplementation(*this, samples, internalFormat, size);
}
#endif

void Renderbuffer::bind() {
    GLuint& binding = Context::current().state().framebuffer.renderbufferBinding;

    if(binding == _id) return;

    /* Binding the renderbuffer finally creates it */
    binding = _id;
    _flags |= ObjectFlag::Created;
    glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

void Renderbuffer::storageImplementationDefault(Renderbuffer& self, RenderbufferFormat internalFormat, const Vector2i& size) {
    self.bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat), size.x(), size.y());
}

#ifndef MAGNUM_TARGET_GLES
void Renderbuffer::storageImplementationDSA(Renderbuffer& self, const RenderbufferFormat internalFormat, const Vector2i& size) {
    glNamedRenderbufferStorage(self._id, GLenum(internalFormat), size.x(), size.y());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void Renderbuffer::storageMultisampleImplementationDefault(Renderbuffer& self, const GLsizei samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    self.bind();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GLenum(internalFormat), size.x(), size.y());
}
#elif !defined(MAGNUM_TARGET_WEBGL)
void Renderbuffer::storageMultisampleImplementationANGLE(Renderbuffer& self, const GLsizei samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    self.bind();
    glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples, GLenum(internalFormat), size.x(), size.y());
}

void Renderbuffer::storageMultisampleImplementationNV(Renderbuffer& self, const GLsizei samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    self.bind();
    glRenderbufferStorageMultisampleNV(GL_RENDERBUFFER, samples, GLenum(internalFormat), size.x(), size.y());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Renderbuffer::storageMultisampleImplementationDSA(Renderbuffer& self, const GLsizei samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    glNamedRenderbufferStorageMultisample(self._id, samples, GLenum(internalFormat), size.x(), size.y());
}
#endif

}}
