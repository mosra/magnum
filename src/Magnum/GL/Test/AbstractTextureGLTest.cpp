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

#include "Magnum/ImageView.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct AbstractTextureGLTest: OpenGLTester {
    explicit AbstractTextureGLTest();

    void construct();
    void constructMove();

    /* label() tested in subclasses because these all have to provide overloads
       to return correct type for method chaining and these overloads have to
       be deinlined to avoid including a StringView */

    #ifndef MAGNUM_TARGET_GLES
    void imageQueryViewNullptr();
    void imageQueryViewBadSize();
    void subImageQueryViewNullptr();
    void subImageQueryViewBadSize();

    void compressedImageQueryViewNullptr();
    void compressedImageQueryViewBadSize();
    void compressedImageQueryViewBadDataSize();
    void compressedImageQueryViewBadFormat();
    void compressedSubImageQueryViewNullptr();
    void compressedSubImageQueryViewBadSize();
    void compressedSubImageQueryViewBadDataSize();
    void compressedSubImageQueryViewBadFormat();
    #endif
};

AbstractTextureGLTest::AbstractTextureGLTest() {
    addTests({&AbstractTextureGLTest::construct,
              &AbstractTextureGLTest::constructMove,

              #ifndef MAGNUM_TARGET_GLES
              &AbstractTextureGLTest::imageQueryViewNullptr,
              &AbstractTextureGLTest::imageQueryViewBadSize,
              &AbstractTextureGLTest::subImageQueryViewNullptr,
              &AbstractTextureGLTest::subImageQueryViewBadSize,

              &AbstractTextureGLTest::compressedImageQueryViewNullptr,
              &AbstractTextureGLTest::compressedImageQueryViewBadSize,
              &AbstractTextureGLTest::compressedImageQueryViewBadDataSize,
              &AbstractTextureGLTest::compressedImageQueryViewBadFormat,
              &AbstractTextureGLTest::compressedSubImageQueryViewNullptr,
              &AbstractTextureGLTest::compressedSubImageQueryViewBadSize,
              &AbstractTextureGLTest::compressedSubImageQueryViewBadDataSize,
              &AbstractTextureGLTest::compressedSubImageQueryViewBadFormat,
              #endif
              });
}

void AbstractTextureGLTest::construct() {
    {
        const Texture2D texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
        CORRADE_COMPARE(texture.target(), GL_TEXTURE_2D);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractTextureGLTest::constructMove() {
    Texture2D a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    Texture2D b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Texture2D c;
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTextureGLTest::imageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, {nullptr, 2*2*4}};

    Containers::String out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::image(): image view is nullptr\n");
}

void AbstractTextureGLTest::imageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4];
    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2, 1}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.image(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::image(): expected image view size Vector(2, 2) but got Vector(2, 1)\n");
}

void AbstractTextureGLTest::subImageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, {nullptr, 2*2*4}};

    Containers::String out;
    Error redirectError{&out};
    texture.subImage(0, {{}, Vector2i{2}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::subImage(): image view is nullptr\n");
}

void AbstractTextureGLTest::subImageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*4];
    MutableImageView2D image{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2, 1}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.subImage(0, {{}, Vector2i{2}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::subImage(): expected image view size Vector(2, 2) but got Vector(2, 1)\n");
}

void AbstractTextureGLTest::compressedImageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, {nullptr, 16}};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedImage(): image view is nullptr\n");
}

void AbstractTextureGLTest::compressedImageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4, 8}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedImage(): expected image view size Vector(4, 4) but got Vector(4, 8)\n");
}

void AbstractTextureGLTest::compressedImageQueryViewBadDataSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16 - 1];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedImage(): expected image view data size 16 bytes but got 15\n");
}

void AbstractTextureGLTest::compressedImageQueryViewBadFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector2i{4}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedImage(0, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}

void AbstractTextureGLTest::compressedSubImageQueryViewNullptr() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, {nullptr, 16}};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, {{}, Vector2i{4}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedSubImage(): image view is nullptr\n");
}

void AbstractTextureGLTest::compressedSubImageQueryViewBadSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[2*16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4, 8}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, {{}, Vector2i{4}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedSubImage(): expected image view size Vector(4, 4) but got Vector(4, 8)\n");
}

void AbstractTextureGLTest::compressedSubImageQueryViewBadDataSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16 - 1];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, {{}, Vector2i{4}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedSubImage(): expected image view data size 16 bytes but got 15\n");
}

void AbstractTextureGLTest::compressedSubImageQueryViewBadFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::internalformat_query2>())
        CORRADE_SKIP(Extensions::ARB::internalformat_query2::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, Vector2i{4});

    MAGNUM_VERIFY_NO_GL_ERROR();

    char data[16];
    MutableCompressedImageView2D image{CompressedPixelFormat::RGBAS3tcDxt1, Vector2i{4}, data};

    Containers::String out;
    Error redirectError{&out};
    texture.compressedSubImage(0, {{}, Vector2i{4}}, image);
    CORRADE_COMPARE(out, "GL::AbstractTexture::compressedSubImage(): expected image view format GL::CompressedPixelFormat::RGBAS3tcDxt3 but got GL::CompressedPixelFormat::RGBAS3tcDxt1\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AbstractTextureGLTest)
