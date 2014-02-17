/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "BufferTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

Int BufferTexture::offsetAlignment() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_buffer_range>())
        return 0;

    GLint& value = Context::current()->state().texture->bufferOffsetAlignment;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &value);

    return value;
}

BufferTexture& BufferTexture::setBuffer(const BufferTextureFormat internalFormat, Buffer& buffer) {
    (this->*Context::current()->state().texture->setBufferImplementation)(internalFormat, buffer);
    return *this;
}

BufferTexture& BufferTexture::setBuffer(const BufferTextureFormat internalFormat, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    (this->*Context::current()->state().texture->setBufferRangeImplementation)(internalFormat, buffer, offset, size);
    return *this;
}

void BufferTexture::setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer) {
    bindInternal();
    glTexBuffer(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id());
}

void BufferTexture::setBufferImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer) {
    glTextureBufferEXT(id(), GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id());
}

void BufferTexture::setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size) {
    bindInternal();
    glTexBufferRange(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id(), offset, size);
}

void BufferTexture::setBufferRangeImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size) {
    glTextureBufferRangeEXT(id(), GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id(), offset, size);
}

}
#endif
