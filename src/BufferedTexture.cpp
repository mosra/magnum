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

#include "BufferedTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"

namespace Magnum {

BufferedTexture::SetBufferImplementation BufferedTexture::setBufferImplementation = &BufferedTexture::setBufferImplementationDefault;

void BufferedTexture::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "BufferedTexture: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        setBufferImplementation = &BufferedTexture::setBufferImplementationDSA;
    }
}

void BufferedTexture::setBufferImplementationDefault(BufferedTexture::InternalFormat internalFormat, Buffer* buffer) {
    bindInternal();
    glTexBuffer(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id());
}

void BufferedTexture::setBufferImplementationDSA(BufferedTexture::InternalFormat internalFormat, Buffer* buffer) {
    glTextureBufferEXT(id(), GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id());
}

}
#endif
