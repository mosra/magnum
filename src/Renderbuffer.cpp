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

Renderbuffer::~Renderbuffer() {
    /* If bound, remove itself from state */
    GLuint& binding = Context::current()->state()->framebuffer->renderbufferBinding;
    if(binding == _id) binding = 0;

    glDeleteRenderbuffers(1, &_id);
}

void Renderbuffer::bind() {
    GLuint& binding = Context::current()->state()->framebuffer->renderbufferBinding;

    if(binding == _id) return;

    binding = _id;
    glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

void Renderbuffer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "Renderbuffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        storageImplementation = &Renderbuffer::storageImplementationDSA;
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

}
