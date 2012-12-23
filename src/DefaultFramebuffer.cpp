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
void DefaultFramebuffer::mapForDraw(std::initializer_list<std::pair<GLuint, DrawAttachment>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(const auto& attachment: attachments)
        if(attachment.first > max) max = attachment.first;

    /* Create linear array from associative */
    GLenum* _attachments = new GLenum[max+1];
    std::fill_n(_attachments, max, GL_NONE);
    for(const auto& attachment: attachments)
        _attachments[attachment.first] = static_cast<GLenum>(attachment.second);

    (this->*drawBuffersImplementation)(max+1, _attachments);
    delete[] _attachments;
}
#endif

void DefaultFramebuffer::initializeContextBasedFunctionality(Context* context) {
    Implementation::FramebufferState* state = context->state()->framebuffer;

    /* Initial framebuffer size */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    defaultFramebuffer._viewport = state->viewport = Rectanglei::fromSize({viewport[0], viewport[1]}, {viewport[2], viewport[3]});
}

}
