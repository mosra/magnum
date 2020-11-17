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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/BufferTexture.h"
#include "Magnum/GL/BufferTextureFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/ImageFormat.h"
#include "Magnum/GL/OpenGLTester.h"

#if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct BufferTextureGLTest: OpenGLTester {
    explicit BufferTextureGLTest();

    void construct();
    void constructMove();
    void wrap();

    void bind();
    void bindImage();

    void setBuffer();
    void setBufferEmptyFirst();
    void setBufferOffset();

    void resetBuffer();

    #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
    void appleSetBufferSubData();
    void appleSetUnrelatedBufferData();
    void appleSetBufferQueryData();
    void appleSetBufferMap();
    void appleSetBufferMapRange();
    void appleBindUnrelatedTextureInBetween();
    #endif
};

BufferTextureGLTest::BufferTextureGLTest() {
    addTests({&BufferTextureGLTest::construct,
              &BufferTextureGLTest::constructMove,
              &BufferTextureGLTest::wrap,

              &BufferTextureGLTest::bind,
              &BufferTextureGLTest::bindImage,

              &BufferTextureGLTest::setBuffer,
              &BufferTextureGLTest::setBufferEmptyFirst,
              &BufferTextureGLTest::setBufferOffset,

              &BufferTextureGLTest::resetBuffer,

              #if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
              &BufferTextureGLTest::appleSetBufferSubData,
              &BufferTextureGLTest::appleSetUnrelatedBufferData,
              &BufferTextureGLTest::appleSetBufferQueryData,
              &BufferTextureGLTest::appleSetBufferMap,
              &BufferTextureGLTest::appleSetBufferMapRange,
              &BufferTextureGLTest::appleBindUnrelatedTextureInBetween
              #endif
              });
}

void BufferTextureGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    {
        BufferTexture texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::constructMove() {
    /* Move constructor tested in AbstractTexture, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(BufferTexture), sizeof(AbstractTexture));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<BufferTexture>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<BufferTexture>::value);
}

void BufferTextureGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
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
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::bindImage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 32}, BufferUsage::StaticDraw);

    BufferTexture texture;
    texture.setBuffer(BufferTextureFormat::RGBA8, buffer)
        .bindImage(2, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

void BufferTextureGLTest::setBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    Buffer buffer;
    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    buffer.setData(data, BufferUsage::StaticDraw);
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::setBufferEmptyFirst() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    Buffer buffer;
    texture.setBuffer(BufferTextureFormat::RGBA8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(Context::current().isVersionSupported(Version::GLES310))
        CORRADE_COMPARE(texture.size(), 0);

    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    buffer.setData(data, BufferUsage::StaticDraw);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.size(), 4);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::setBufferOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_range>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_range::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
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
    texture.setBuffer(BufferTextureFormat::RGBA8UI, buffer, 256, 8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.size(), 2);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::resetBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_buffer>())
        CORRADE_SKIP(Extensions::EXT::texture_buffer::string() + std::string(" is not supported."));
    #endif

    BufferTexture texture;
    Buffer buffer;
    buffer.setData({nullptr, 16});
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isVersionSupported(Version::GLES310))
    #endif
    {
        CORRADE_COMPARE(texture.size(), 8);
    }

    texture.resetBuffer();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.size(), 0);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS)
void BufferTextureGLTest::appleSetBufferSubData() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This also crashes unless worked around. Ugh. */
    buffer.setSubData<UnsignedByte>(2, {0xf3, 0xab, 0x01, 0x57});

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::appleSetUnrelatedBufferData() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This crashes even though there is no relation to the texture. */
    Buffer another;
    another.setData<UnsignedByte>({0xf3, 0xab, 0x01, 0x57});

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::appleSetBufferQueryData() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This shouldn't suffer from the same problem as setData() and so isn't
       worked around in any way */
    buffer.data();

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::appleSetBufferMap() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This also crashes unless worked around. Ugh. */
    const char* ptr = buffer.mapRead();
    CORRADE_VERIFY(ptr);

    /* This too */
    buffer.unmap();

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::appleSetBufferMapRange() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* This also crashes unless worked around. Ugh. */
    char* ptr = buffer.map(0, 16, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit);
    CORRADE_VERIFY(ptr);

    ptr[12] = 0x35;

    /* This doesn't, it seems (yay!) */
    buffer.flushMappedRange(8, 8);

    /* This would crash again unless worked around */
    buffer.unmap();

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void BufferTextureGLTest::appleBindUnrelatedTextureInBetween() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_buffer_object>())
        CORRADE_SKIP(Extensions::ARB::texture_buffer_object::string() + std::string(" is not supported."));

    BufferTexture texture;
    Buffer buffer{Buffer::TargetHint::Texture};
    buffer.setData<UnsignedByte>({
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    });
    texture.setBuffer(BufferTextureFormat::RG8UI, buffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Bind a texture of different type to the same slot, which makes Magnum's
       state tracker think there's no buffer texture bound */
    Texture2D whatever;
    whatever.setStorage(1, GL::TextureFormat::RGBA32F, Vector2i{16, 16});

    /* This then crashes, unless we remember there was a buffer texture bound
       before and account for that. */
    buffer.setSubData<UnsignedByte>(2, {0xf3, 0xab, 0x01, 0x57});

    CORRADE_COMPARE(texture.size(), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::BufferTextureGLTest)
