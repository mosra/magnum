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

namespace Magnum {

DefaultFramebuffer defaultFramebuffer;

DefaultFramebuffer::DefaultFramebuffer() { _id = 0; }

#ifndef MAGNUM_TARGET_GLES2
void DefaultFramebuffer::mapForDraw(std::initializer_list<DrawAttachment> attachments) {
    GLenum* _attachments = new GLenum[attachments.size()];
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it-attachments.begin()] = static_cast<GLenum>(*it);

    bind(drawTarget);
    glDrawBuffers(attachments.size(), _attachments);
    delete[] _attachments;
}
#endif

}
