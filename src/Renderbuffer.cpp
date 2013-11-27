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

#include "Renderbuffer.h"

#include "Context.h"
#include "Extensions.h"

#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

Renderbuffer::StorageImplementation Renderbuffer::storageImplementation = &Renderbuffer::storageImplementationDefault;
Renderbuffer::StorageMultisampleImplementation Renderbuffer::storageMultisampleImplementation =
    #ifndef MAGNUM_TARGET_GLES2
    &Renderbuffer::storageMultisampleImplementationDefault;
    #else
    nullptr;
    #endif

Int Renderbuffer::maxSize() {
    GLint& value = Context::current()->state().framebuffer->maxRenderbufferSize;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &value);

    return value;
}

Int Renderbuffer::maxSamples() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>() && !Context::current()->isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>())
        return 0;
    #endif

    GLint& value = Context::current()->state().framebuffer->maxSamples;

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

Renderbuffer::~Renderbuffer() {
    /* If bound, remove itself from state */
    GLuint& binding = Context::current()->state().framebuffer->renderbufferBinding;
    if(binding == _id) binding = 0;

    glDeleteRenderbuffers(1, &_id);
}

void Renderbuffer::bind() {
    GLuint& binding = Context::current()->state().framebuffer->renderbufferBinding;

    if(binding == _id) return;

    binding = _id;
    glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

void Renderbuffer::initializeContextBasedFunctionality(Context& context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Renderbuffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        storageImplementation = &Renderbuffer::storageImplementationDSA;
        storageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDSA;
    }
    #elif !defined(MAGNUM_TARGET_GLES3)
    if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>()) {
        Debug() << "Renderbuffer: using" << Extensions::GL::ANGLE::framebuffer_multisample::string() << "features";

        storageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationANGLE;
    } else if (context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>()) {
        Debug() << "Renderbuffer: using" << Extensions::GL::NV::framebuffer_multisample::string() << "features";

        storageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationNV;
    }
    #else
    static_cast<void>(context);
    #endif
}

void Renderbuffer::storageImplementationDefault(RenderbufferFormat internalFormat, const Vector2i& size) {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat), size.x(), size.y());
}

#ifndef MAGNUM_TARGET_GLES
void Renderbuffer::storageImplementationDSA(RenderbufferFormat internalFormat, const Vector2i& size) {
    glNamedRenderbufferStorageEXT(_id, GLenum(internalFormat), size.x(), size.y());
}
#endif

/** @todo Enable when extension wrangler for ES is done */

#ifndef MAGNUM_TARGET_GLES2
void Renderbuffer::storageMultisampleImplementationDefault(const GLsizei samples, const RenderbufferFormat internalFormat, const Vector2i& size) {
    bind();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GLenum(internalFormat), size.x(), size.y());
}
#else
void Renderbuffer::storageMultisampleImplementationANGLE(GLsizei, RenderbufferFormat, const Vector2i&) {
    CORRADE_INTERNAL_ASSERT(false);
    //glRenderbufferStorageMultisampleANGLE(GL_RENDERBUFFER, samples, internalFormat, size.x(), size.y());
}

void Renderbuffer::storageMultisampleImplementationNV(GLsizei, RenderbufferFormat, const Vector2i&) {
    CORRADE_INTERNAL_ASSERT(false);
    //glRenderbufferStorageMultisampleNV(GL_RENDERBUFFER, samples, internalFormat, size.x(), size.y());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Renderbuffer::storageMultisampleImplementationDSA(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size) {
    glNamedRenderbufferStorageMultisampleEXT(_id, samples, GLenum(internalFormat), size.x(), size.y());
}
#endif

}
