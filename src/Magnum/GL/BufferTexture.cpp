/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/BufferTextureFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"

namespace Magnum { namespace GL {

Int BufferTexture::maxSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        return 0;
    #endif

    GLint& value = Context::current().state().texture->maxBufferSize;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &value);

    return value;
}

Int BufferTexture::offsetAlignment() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_range>())
        return 1;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        return 0;
    #endif

    GLint& value = Context::current().state().texture->bufferOffsetAlignment;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &value);

    return value;
}

Int BufferTexture::size() {
    /* Can't use DataHelper<1>::imageSize(*this, 0)[0] because for 1D textures
       it's not defined on ES */
    Int size;
    (this->*Context::current().state().texture->getLevelParameterivImplementation)(0, GL_TEXTURE_WIDTH, &size);
    return size;
}

BufferTexture& BufferTexture::setBuffer(const BufferTextureFormat internalFormat, Buffer& buffer) {
    buffer.createIfNotAlready();
    (this->*Context::current().state().texture->setBufferImplementation)(internalFormat, &buffer);
    return *this;
}

BufferTexture& BufferTexture::setBuffer(const BufferTextureFormat internalFormat, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    buffer.createIfNotAlready();
    (this->*Context::current().state().texture->setBufferRangeImplementation)(internalFormat, buffer, offset, size);
    return *this;
}

void BufferTexture::setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer* buffer) {
    bindInternal();
    glTexBuffer(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer ? buffer->id() : 0);
}

#ifdef MAGNUM_TARGET_GLES
void BufferTexture::setBufferImplementationEXT(BufferTextureFormat internalFormat, Buffer* buffer) {
    bindInternal();
    glTexBufferEXT(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer ? buffer->id() : 0);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void BufferTexture::setBufferImplementationDSA(const BufferTextureFormat internalFormat, Buffer* buffer) {
    glTextureBuffer(id(), GLenum(internalFormat), buffer ? buffer->id() : 0);
}
#endif

void BufferTexture::setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size) {
    bindInternal();
    glTexBufferRange(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id(), offset, size);
}

#ifdef MAGNUM_TARGET_GLES
void BufferTexture::setBufferRangeImplementationEXT(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size) {
    bindInternal();
    glTexBufferRangeEXT(GL_TEXTURE_BUFFER, GLenum(internalFormat), buffer.id(), offset, size);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void BufferTexture::setBufferRangeImplementationDSA(const BufferTextureFormat internalFormat, Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    glTextureBufferRange(id(), GLenum(internalFormat), buffer.id(), offset, size);
}
#endif

BufferTexture& BufferTexture::resetBuffer() {
    /* R8 is the default state according to ARB_texture_buffer_object, so use
       that */
    (this->*Context::current().state().texture->setBufferImplementation)(BufferTextureFormat::R8, nullptr);
    return *this;
}

}}
#endif
