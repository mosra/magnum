/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/TextureImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Range.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/DebugTools/BufferData.h"
#include "Magnum/GL/BufferImage.h"
#endif

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct TextureImageGLTest: GL::OpenGLTester {
    explicit TextureImageGLTest();

    /* To avoid complex ifdefing on Esmcripten, assumes that tests are built
       on 2.0.17+. On older versions the linker will fail due to missing
       glGetBufferSubData(). */

    void subImage2D();
    void subImage2DNotReadable();
    #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
    void subImage2DBuffer();
    void subImage2DBufferNotReadable();
    #endif
    void subImage2DGeneric();

    void subImageCube();
    void subImageCubeNotReadable();
    #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
    void subImageCubeBuffer();
    void subImageCubeBufferNotReadable();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DUInt();
    void subImage2DFloat();
    void subImage2DFloatGeneric();
    void subImage2DHalf();
    void subImage2DHalfGeneric();
    #endif
};

TextureImageGLTest::TextureImageGLTest() {
    addTests({&TextureImageGLTest::subImage2D,
              &TextureImageGLTest::subImage2DNotReadable,
              #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
              &TextureImageGLTest::subImage2DBuffer,
              &TextureImageGLTest::subImage2DBufferNotReadable,
              #endif
              &TextureImageGLTest::subImage2DGeneric,

              &TextureImageGLTest::subImageCube,
              &TextureImageGLTest::subImageCubeNotReadable,
              #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
              &TextureImageGLTest::subImageCubeBuffer,
              &TextureImageGLTest::subImageCubeBufferNotReadable,
              #endif

              #ifndef MAGNUM_TARGET_GLES2
              &TextureImageGLTest::subImage2DUInt,
              &TextureImageGLTest::subImage2DFloat,
              &TextureImageGLTest::subImage2DFloatGeneric,
              &TextureImageGLTest::subImage2DHalf,
              &TextureImageGLTest::subImage2DHalfGeneric,
              #endif
              });
}

using namespace Math::Literals;

constexpr UnsignedByte Data2D[] = { 0x00, 0x01, 0x02, 0x03,
                                    0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0a, 0x0b,
                                    0x0c, 0x0d, 0x0e, 0x0f };

void TextureImageGLTest::subImage2D() {
    GL::Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RGBA));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureImageGLTest::subImage2DNotReadable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_shared_exponent>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_shared_exponent::string() << "not supported, can't test");
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(GL::Extensions::ARB::get_texture_sub_image::string() << "supported, can't test");
    #endif

    GL::Texture2D texture;
    #ifdef MAGNUM_TARGET_GLES2
    texture.setImage(0, GL::TextureFormat::Luminance, ImageView2D{GL::PixelFormat::Luminance, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D});
    #else
    texture.setImage(0, GL::TextureFormat::RGB9E5, ImageView2D{GL::PixelFormat::RGB, GL::PixelType::UnsignedInt5999Rev, Vector2i{2}, Data2D});
    #endif

    Containers::String out;
    Error redirectError{&out};
    /* The read type doesn't have to match, it doesn't get that far */
    textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::Unsupported\n");
    #else
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::IncompleteAttachment\n");
    #endif
}

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
void TextureImageGLTest::subImage2DBuffer() {
    GL::Texture2D texture;
    texture.setImage(0, GL::TextureFormat::RGBA8, ImageView2D{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D});

    CORRADE_IGNORE_DEPRECATED_PUSH
    GL::BufferImage2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    CORRADE_IGNORE_DEPRECATED_POP
    Containers::Array<char> data = bufferData(image.buffer());
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(image.type(), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<UnsignedByte>(data),
        Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}

void TextureImageGLTest::subImage2DBufferNotReadable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_shared_exponent>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_shared_exponent::string() << "not supported, can't test");
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(GL::Extensions::ARB::get_texture_sub_image::string() << "supported, can't test");
    #endif

    GL::Texture2D texture;
    texture.setImage(0, GL::TextureFormat::RGB9E5, ImageView2D{GL::PixelFormat::RGB, GL::PixelType::UnsignedInt5999Rev, Vector2i{2}, Data2D});

    Containers::String out;
    Error redirectError{&out};
    /* The read type doesn't have to match, it doesn't get that far */
    CORRADE_IGNORE_DEPRECATED_PUSH
    textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::Unsupported\n");
    #else
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::IncompleteAttachment\n");
    #endif
}
#endif

void TextureImageGLTest::subImage2DGeneric() {
    GL::Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        ImageView2D{PixelFormat::RGBA8Unorm, Vector2i{2}, Data2D});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::RGBA8Unorm});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image.formatExtra(), 0);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureImageGLTest::subImageCube() {
    ImageView2D view{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D};

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    constexpr GL::TextureFormat format = GL::TextureFormat::RGBA8;
    #else
    constexpr GL::TextureFormat format = GL::TextureFormat::RGBA;
    #endif

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, format, view);

    Image2D image = textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RGBA));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(Data2D), TestSuite::Compare::Container);
}

void TextureImageGLTest::subImageCubeNotReadable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_shared_exponent>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_shared_exponent::string() << "not supported, can't test");
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(GL::Extensions::ARB::get_texture_sub_image::string() << "supported, can't test");
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    GL::TextureFormat format = GL::TextureFormat::Luminance;
    ImageView2D view{GL::PixelFormat::Luminance, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D};
    #else
    GL::TextureFormat format = GL::TextureFormat::RGB9E5;
    ImageView2D view{GL::PixelFormat::RGB, GL::PixelType::UnsignedInt5999Rev, Vector2i{2}, Data2D};
    #endif

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, format, view);

    Containers::String out;
    Error redirectError{&out};
    /* The read type doesn't have to match, it doesn't get that far */
    textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte});
    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::Unsupported\n");
    #else
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::IncompleteAttachment\n");
    #endif
}

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
void TextureImageGLTest::subImageCubeBuffer() {
    ImageView2D view{GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte, Vector2i{2}, Data2D};

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, GL::TextureFormat::RGBA8, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, GL::TextureFormat::RGBA8, view);

    CORRADE_IGNORE_DEPRECATED_PUSH
    GL::BufferImage2D image = textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    CORRADE_IGNORE_DEPRECATED_POP
    Containers::Array<char> data = bufferData(image.buffer());
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), GL::PixelFormat::RGBA);
    CORRADE_COMPARE(image.type(), GL::PixelType::UnsignedByte);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<UnsignedByte>(data),
        Containers::arrayView(Data2D),
        TestSuite::Compare::Container);
}

void TextureImageGLTest::subImageCubeBufferNotReadable() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_shared_exponent>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_shared_exponent::string() << "not supported, can't test");
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(GL::Extensions::ARB::get_texture_sub_image::string() << "supported, can't test");
    #endif

    GL::TextureFormat format = GL::TextureFormat::RGB9E5;
    ImageView2D view{GL::PixelFormat::RGB, GL::PixelType::UnsignedInt5999Rev, Vector2i{2}, Data2D};

    GL::CubeMapTexture texture;
    texture.setImage(GL::CubeMapCoordinate::PositiveX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeX, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeY, 0, format, view)
           .setImage(GL::CubeMapCoordinate::PositiveZ, 0, format, view)
           .setImage(GL::CubeMapCoordinate::NegativeZ, 0, format, view);

    Containers::String out;
    Error redirectError{&out};
    /* The read type doesn't have to match, it doesn't get that far */
    CORRADE_IGNORE_DEPRECATED_PUSH
    textureSubImage(texture, GL::CubeMapCoordinate::PositiveX, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RGBA, GL::PixelType::UnsignedByte}, GL::BufferUsage::StaticRead);
    CORRADE_IGNORE_DEPRECATED_POP
    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::Unsupported\n");
    #else
    CORRADE_COMPARE(out, "DebugTools::textureSubImage(): texture format not framebuffer-readable: GL::Framebuffer::Status::IncompleteAttachment\n");
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr UnsignedInt Data2DUInt[] = { 0xcafebabe,
                                       0xdeadbeef,
                                       0xbadf00d,
                                       0xdeadbabe };

void TextureImageGLTest::subImage2DUInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::texture_integer>())
        CORRADE_SKIP(GL::Extensions::EXT::texture_integer::string() << "is not supported.");
    #endif

    GL::Texture2D texture;
    texture.setImage(0, GL::TextureFormat::R32UI, ImageView2D{GL::PixelFormat::RedInteger, GL::PixelType::UnsignedInt, Vector2i{2}, Data2DUInt});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::RedInteger, GL::PixelType::UnsignedInt});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::RedInteger));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::UnsignedInt);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(image.data()),
        Containers::arrayView(Data2DUInt),
        TestSuite::Compare::Container);
}

constexpr Float Data2DFloat[] = { 1.0f,
                                  3.14159f,
                                  2.71828f,
                                  1.41421f };

void TextureImageGLTest::subImage2DFloat() {
    GL::Texture2D texture;
    texture
        /* If I don't set min filter, SwiftShader will return all zeros. ARM
           Mali G71 (on Huawei P10) needs the mag filter as well. */
        .setMinificationFilter(GL::SamplerFilter::Nearest)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, GL::TextureFormat::R32F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Float, Vector2i{2}, Data2DFloat});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::Red, GL::PixelType::Float});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::Red));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::Float);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(image.data()),
        Containers::arrayView(Data2DFloat),
        TestSuite::Compare::Container);
}

void TextureImageGLTest::subImage2DFloatGeneric() {
    GL::Texture2D texture;
    texture
        /* If I don't set min filter, SwiftShader will return all zeros. ARM
           Mali G71 (on Huawei P10) needs the mag filter as well. */
        .setMinificationFilter(GL::SamplerFilter::Nearest)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setStorage(1, GL::TextureFormat::R32F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Float, Vector2i{2}, Data2DFloat});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::R32F});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), PixelFormat::R32F);
    CORRADE_COMPARE(image.formatExtra(), 0);
    CORRADE_COMPARE(image.pixelSize(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(image.data()),
        Containers::arrayView(Data2DFloat),
        TestSuite::Compare::Container);
}

const Half Data2DHalf[] = { 1.0_h,
                            3.14159_h,
                            2.71828_h,
                            1.41421_h };

void TextureImageGLTest::subImage2DHalf() {
    GL::Texture2D texture;
    texture
        .setStorage(1, GL::TextureFormat::R16F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Half, Vector2i{2}, Data2DHalf});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {GL::PixelFormat::Red, GL::PixelType::Half});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), pixelFormatWrap(GL::PixelFormat::Red));
    CORRADE_COMPARE(GL::PixelType(image.formatExtra()), GL::PixelType::Half);
    CORRADE_COMPARE(image.pixelSize(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<Half>(image.data()),
        Containers::arrayView(Data2DHalf),
        TestSuite::Compare::Container);
}

void TextureImageGLTest::subImage2DHalfGeneric() {
    GL::Texture2D texture;
    texture
        .setStorage(1, GL::TextureFormat::R16F, Vector2i{2})
        .setSubImage(0, {}, ImageView2D{GL::PixelFormat::Red, GL::PixelType::Half, Vector2i{2}, Data2DHalf});

    Image2D image = textureSubImage(texture, 0, {{}, Vector2i{2}}, {PixelFormat::R16F});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE(image.format(), PixelFormat::R16F);
    CORRADE_COMPARE(image.formatExtra(), 0);
    CORRADE_COMPARE(image.pixelSize(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<Half>(image.data()),
        Containers::arrayView(Data2DHalf),
        TestSuite::Compare::Container);
}

#endif

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::TextureImageGLTest)
