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

#include "BufferTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"

namespace Magnum {

BufferTexture::SetBufferImplementation BufferTexture::setBufferImplementation = &BufferTexture::setBufferImplementationDefault;
BufferTexture::SetBufferRangeImplementation BufferTexture::setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDefault;

void BufferTexture::initializeContextBasedFunctionality(Context* context) {
    if(context->isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "BufferTexture: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        setBufferImplementation = &BufferTexture::setBufferImplementationDSA;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDSA;
    }
}

void BufferTexture::setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer* buffer) {
    bindInternal();
    glTexBuffer(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id());
}

void BufferTexture::setBufferImplementationDSA(BufferTextureFormat internalFormat, Buffer* buffer) {
    glTextureBufferEXT(id(), GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id());
}

void BufferTexture::setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer* buffer, GLintptr offset, GLsizeiptr size) {
    bindInternal();
    glTexBufferRange(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id(), offset, size);
}

void BufferTexture::setBufferRangeImplementationDSA(BufferTextureFormat internalFormat, Buffer* buffer, GLintptr offset, GLsizeiptr size) {
    glTextureBufferRangeEXT(id(), GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer->id(), offset, size);
}

}
#endif
