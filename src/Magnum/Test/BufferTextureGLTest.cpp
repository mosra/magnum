/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Buffer.h"
#include "Magnum/BufferTexture.h"
#include "Magnum/BufferTextureFormat.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/ImageFormat.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct BufferTextureGLTest: OpenGLTester {
    explicit BufferTextureGLTest();

    void construct();
    void wrap();

    void bind();
    void bindImage();

    void setBuffer();
    void setBufferOffset();
};

BufferTextureGLTest::BufferTextureGLTest() {
    addTests({&BufferTextureGLTest::construct,
              &BufferTextureGLTest::wrap,

              &BufferTextureGLTest::bind,
              &BufferTextureGLTest::bindImage,

              &BufferTextureGLTest::setBuffer,
              &BufferTextureGLTest::setBufferOffset});
}

void BufferTextureGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    {
        BufferTexture texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void BufferTextureGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = BufferTexture::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    BufferTexture::wrap(id);
    glDeleteTextures(1, &id);
}

void BufferTextureGLTest::bind() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void BufferTextureGLTest::bindImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 32}, BufferUsage::StaticDraw);

    BufferTexture texture;
    texture.setBuffer(BufferTextureFormat::RGBA8, buffer)
        .bindImage(2, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void BufferTextureGLTest::setBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    Buffer buffer;
    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    buffer.setData(data, BufferUsage::StaticDraw);
    texture.setBuffer(BufferTextureFormat::R8UI, buffer);

    MAGNUM_VERIFY_NO_ERROR();
}

void BufferTextureGLTest::setBufferOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_buffer_range>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_buffer_range::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    /* Check that we have correct offset alignment */
    CORRADE_INTERNAL_ASSERT(256 % BufferTexture::offsetAlignment() == 0);

    BufferTexture texture;
    Buffer buffer;
    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    buffer.setData({nullptr, 1024}, BufferUsage::StaticDraw);
    buffer.setSubData(256 - 16, data);
    texture.setBuffer(BufferTextureFormat::R8UI, buffer, 256, 8);

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::BufferTextureGLTest)
