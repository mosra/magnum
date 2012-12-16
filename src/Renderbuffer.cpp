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

#include "Renderbuffer.h"

#include "Context.h"

#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

Renderbuffer::~Renderbuffer() {
    /* If bound, remove itself from state */
    GLuint& binding = Context::current()->state()->framebuffer->renderbufferBinding;
    if(binding == _id) binding = 0;

    glDeleteRenderbuffers(1, &_id);
}

void Renderbuffer::setStorage(Renderbuffer::InternalFormat internalFormat, const Vector2i& size) {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat), size.x(), size.y());
}

void Renderbuffer::bind() {
    GLuint& binding = Context::current()->state()->framebuffer->renderbufferBinding;

    if(binding == _id) return;

    binding = _id;
    glBindRenderbuffer(GL_RENDERBUFFER, _id);
}

}
