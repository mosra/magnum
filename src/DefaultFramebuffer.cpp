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

#include "DefaultFramebuffer.h"

#include "Context.h"

#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

DefaultFramebuffer defaultFramebuffer;

DefaultFramebuffer::DefaultFramebuffer() { _id = 0; }

#ifndef MAGNUM_TARGET_GLES2
void DefaultFramebuffer::mapForDraw(std::initializer_list<DrawAttachment> attachments) {
    GLenum* _attachments = new GLenum[attachments.size()];
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it-attachments.begin()] = static_cast<GLenum>(*it);

    bindInternal(drawTarget);
    glDrawBuffers(attachments.size(), _attachments);
    delete[] _attachments;
}
#endif

void DefaultFramebuffer::mapForRead(ReadAttachment attachment) {
    bindInternal(readTarget);
    /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glReadBuffer(static_cast<GLenum>(attachment));
    #else
    static_cast<void>(attachment);
    #endif
}

void DefaultFramebuffer::initializeContextBasedFunctionality(Context* context) {
    Implementation::FramebufferState* state = context->state()->framebuffer;

    /* Initial framebuffer size */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    defaultFramebuffer._viewportPosition = state->viewportPosition = {viewport[0], viewport[1]};
    defaultFramebuffer._viewportSize = state->viewportSize = {viewport[2], viewport[3]};
}

}
