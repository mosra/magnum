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

#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#endif
#include "Magnum/GL/TextureFormat.h"

#ifdef MAGNUM_TARGET_GLES
#include "Magnum/GL/Framebuffer.h"
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct PixelStorageGLTest: OpenGLTester {
    explicit PixelStorageGLTest();

    void alignmentUnpack2D();
    void alignmentPack2D();
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void alignmentRowLengthSkipXYUnpack2D();
    void alignmentRowLengthSkipXYPack2D();
    #ifndef MAGNUM_TARGET_GLES2
    void alignmentImageHeightRowLengthSkipXYZUnpack3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void alignmentImageHeightRowLengthSkipXYZPack3D();
    #endif
    #endif
    void defaultsAfterStateReset();

    #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
    void rowLengthNotSupported();
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    void imageHeightNotSupported();
    #elif defined(MAGNUM_TARGET_GLES)
    void imageHeightSkipZPackNotSupported();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void compressedUnpack2D();
    void compressedPack2D();
    void compressedUnpack3D();
    void compressedPack3D();
    #endif
    void compressedResetParameters();
    void compressedDefaultsAfterStateReset();

    #ifdef MAGNUM_TARGET_GLES
    void compressedNotSupported();
    #endif
};

PixelStorageGLTest::PixelStorageGLTest() {
    addTests({&PixelStorageGLTest::alignmentUnpack2D,
              &PixelStorageGLTest::alignmentPack2D,
              #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
              &PixelStorageGLTest::alignmentRowLengthSkipXYUnpack2D,
              &PixelStorageGLTest::alignmentRowLengthSkipXYPack2D,
              #ifndef MAGNUM_TARGET_GLES2
              &PixelStorageGLTest::alignmentImageHeightRowLengthSkipXYZUnpack3D,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &PixelStorageGLTest::alignmentImageHeightRowLengthSkipXYZPack3D,
              #endif
              #endif
              &PixelStorageGLTest::defaultsAfterStateReset,

              #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
              &PixelStorageGLTest::rowLengthNotSupported,
              #endif
              #ifdef MAGNUM_TARGET_GLES2
              &PixelStorageGLTest::imageHeightNotSupported,
              #elif defined(MAGNUM_TARGET_GLES)
              &PixelStorageGLTest::imageHeightSkipZPackNotSupported,
              #endif

              #ifndef MAGNUM_TARGET_GLES
              &PixelStorageGLTest::compressedUnpack2D,
              &PixelStorageGLTest::compressedPack2D,
              &PixelStorageGLTest::compressedUnpack3D,
              &PixelStorageGLTest::compressedPack3D,
              #endif
              &PixelStorageGLTest::compressedResetParameters,
              &PixelStorageGLTest::compressedDefaultsAfterStateReset,

              #ifdef MAGNUM_TARGET_GLES
              &PixelStorageGLTest::compressedNotSupported,
              #endif
              });
}

constexpr char AlignmentData2D[]{
    /* Data -----------------------------------------------------------. */ /* Alignment */
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x00',
    '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x12', '\x00',
};

void PixelStorageGLTest::alignmentUnpack2D() {
    ImageView2D image{
        PixelStorage{}
            .setAlignment(2),
        Magnum::PixelFormat::RGB8Unorm, {3, 2}, AlignmentData2D};

    Texture2D texture;
    texture.setImage(0, textureFormat(image.format()), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Read into a format that's guaranteed to be supported even on WebGL 1,
       i.e. a four-component one. With a three-component format both Chrome and
       Firefox produce a stupid error about "buffer not large enough", while
       the problem is the format not being supported. WebGL 2 works with RGB
       completely fine, tho.

       Strangely enough, reading to RGB (with a two-pixel alignment, tho) in
       alignmentPack2D() below works all fine. I smell some shitty ANGLE
       bug. */
    Image2D actual{PixelFormat::RGBA, PixelType::UnsignedByte, {},
        Containers::Array<char>{ValueInit, 24}};

    #ifndef MAGNUM_TARGET_GLES
    texture.image(0, actual);
    #else
    Framebuffer framebuffer{{{}, {3, 2}}};
    framebuffer
        .attachTexture(Framebuffer::ColorAttachment{0}, texture, 0)
        .read(framebuffer.viewport(), actual);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(actual.data(), Containers::arrayView({
        '\x00', '\x01', '\x02', '\xff',
        '\x03', '\x04', '\x05', '\xff',
        '\x06', '\x07', '\x08', '\xff',

        '\x0a', '\x0b', '\x0c', '\xff',
        '\x0d', '\x0e', '\x0f', '\xff',
        '\x10', '\x11', '\x12', '\xff',
    }), TestSuite::Compare::Container);
}

void PixelStorageGLTest::alignmentPack2D() {
    const char data[]{
        '\x00', '\x01', '\x02',
        '\x03', '\x04', '\x05',
        '\x06', '\x07', '\x08', '\x00', '\x00', '\x00',

        '\x0a', '\x0b', '\x0c',
        '\x0d', '\x0e', '\x0f',
        '\x10', '\x11', '\x12', '\x00', '\x00', '\x00',
    };
    ImageView2D actual{Magnum::PixelFormat::RGB8Unorm, {3, 2}, data};

    Texture2D texture;
    texture.setImage(0, textureFormat(actual.format()), actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Pre-allocate and zero out the data array so we can conveniently compare */
    Image2D image{PixelStorage{}
        .setAlignment(2),
        PixelFormat::RGB, PixelType::UnsignedByte, {}, Containers::Array<char>{ValueInit, sizeof(AlignmentData2D)}};

    #ifndef MAGNUM_TARGET_GLES
    texture.image(0, image);
    #else
    Framebuffer framebuffer{{{}, {3, 2}}};
    framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);

    /* We *need* to read as RGB in this case because otherwise the alignment
       cannot be properly tested, as it'll be always a multiple of four */
    CORRADE_EXPECT_FAIL_IF(framebuffer.implementationColorReadFormat() != PixelFormat::RGB,
        "Implementation-defined framebuffer read format is not RGB, reading will fail.");

    framebuffer.read(framebuffer.viewport(), image);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    /* SwiftShader (on Android, at least) seems to write even to the padding
       bytes, yay. Clear those before comparison. */
    if(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader) {
        CORRADE_COMPARE(image.data().size(), Containers::arraySize(AlignmentData2D));
        for(std::size_t i: {9, 19}) {
            CORRADE_ITERATION(i);
            if(image.data()[i] != '\0') {
                CORRADE_WARN("Padding byte at offset 9 isn't zero but" << image.data()[i]);
                image.data()[i] = '\0';
            }
        }
    }
    #endif

    CORRADE_COMPARE_AS(image.data(), Containers::arrayView(AlignmentData2D),
        TestSuite::Compare::Container);
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
constexpr char AlignmentRowLengthSkipXYData2D[]{
    /* Row length ------------------------------------------------------ */ /* Alignment */
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    /* ------------ Skip */ /* Data ------------------------------------ */ /* Alignment */
    '\x00', '\x00', '\x00', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x00',
    '\x00', '\x00', '\x00', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x00',
    '\x00', '\x00', '\x00', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x00',
};

constexpr const char ActualData[] = {
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x00', '\x00',
    '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x00', '\x00',
    '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x00', '\x00'
};

void PixelStorageGLTest::alignmentRowLengthSkipXYUnpack2D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #endif

    ImageView2D image{PixelStorage{}
        .setAlignment(2)
        .setRowLength(3)
        .setSkip({1, 3, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte, {2, 3}, AlignmentRowLengthSkipXYData2D};

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGB8, {2, 3})
        .setSubImage(0, {}, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Read into zero-initialized array to avoid comparing random memory in
       padding bytes (confirmed on NVidia 355.11, AMD 15.300.1025.0) */
    Image2D actual{PixelFormat::RGB, PixelType::UnsignedByte, {},
        Containers::Array<char>{ValueInit, sizeof(ActualData)}};

    #ifndef MAGNUM_TARGET_GLES
    texture.image(0, actual);
    #else
    Framebuffer fb{{{}, {2, 3}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_EXPECT_FAIL_IF(fb.implementationColorReadFormat() != PixelFormat::RGB, "Implementation-defined framebuffer read format is not RGB, reading will fail.");

    fb.read(fb.viewport(), actual);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(actual.data(), Containers::arrayView(ActualData),
        TestSuite::Compare::Container);
}

void PixelStorageGLTest::alignmentRowLengthSkipXYPack2D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::NV::pack_subimage>())
        CORRADE_SKIP(Extensions::NV::pack_subimage::string() << "is not supported.");
    #endif

    ImageView2D actual{PixelFormat::RGB, PixelType::UnsignedByte, {2, 3}, ActualData};

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGB8, {2, 3})
        .setSubImage(0, {}, actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Pre-allocate and zero out the data array so we can conveniently compare */
    Image2D image{PixelStorage{}
        .setAlignment(2)
        .setRowLength(3)
        .setSkip({1, 3, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte, {}, Containers::Array<char>{ValueInit, sizeof(AlignmentRowLengthSkipXYData2D)}};

    #ifndef MAGNUM_TARGET_GLES
    texture.image(0, image);
    #else
    Framebuffer fb{{{}, {2, 3}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_EXPECT_FAIL_IF(fb.implementationColorReadFormat() != PixelFormat::RGB, "Implementation-defined framebuffer read format is not RGB, reading will fail.");

    fb.read(fb.viewport(), image);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(image.data(), Containers::arrayView(AlignmentRowLengthSkipXYData2D),
        TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr const char Data3D[] = {
    /* Row length ------------------------------------------------------ */ /* Alignment */
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',

    /* Row length ------------------------------------------------------ */ /* Alignment */
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    /* ------------ Skip */ /* Data ------------------------------------ */ /* Alignment */
    '\x00', '\x00', '\x00', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x00',
    '\x00', '\x00', '\x00', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x00',
    '\x00', '\x00', '\x00', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x00',

    /* Filling to image height not needed */
};

void PixelStorageGLTest::alignmentImageHeightRowLengthSkipXYZUnpack3D() {
    PixelStorage storage;
    storage.setAlignment(2)
        .setRowLength(3)
        .setImageHeight(5)
        .setSkip({1, 2, 1});

    ImageView3D image{storage, PixelFormat::RGB, PixelType::UnsignedByte, {2, 3, 1}, Data3D};

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGB8, {2, 3, 1})
        .setSubImage(0, {}, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Testing mainly image height here, which is not available as pack
       parameter in ES */
    #ifndef MAGNUM_TARGET_GLES
    /* Read into zero-initialized array to avoid comparing random memory in
       padding bytes (confirmed on AMD 15.300.1025.0) */
    Image3D actual{PixelFormat::RGB, PixelType::UnsignedByte, {},
        Containers::Array<char>{ValueInit, sizeof(ActualData)}};

    texture.image(0, actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Clear padding in the last row (the driver might leave them untouched,
       confirmed on NVidia 358.16) */
    CORRADE_VERIFY(actual.data().size() == Containers::arrayView(ActualData).size());
    *(actual.data().end() - 1) = *(actual.data().end() - 2) = 0;

    CORRADE_COMPARE_AS(actual.data(), Containers::arrayView(ActualData),
        TestSuite::Compare::Container);
    #endif
}
#endif

/* Testing mainly image height here, which is not available as pack parameter
   in ES */
#ifndef MAGNUM_TARGET_GLES
void PixelStorageGLTest::alignmentImageHeightRowLengthSkipXYZPack3D() {
    ImageView3D actual{PixelFormat::RGB, PixelType::UnsignedByte, {2, 3, 1}, ActualData};

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGB8, {2, 3, 1})
        .setSubImage(0, {}, actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image{PixelStorage{}
        .setAlignment(2)
        .setRowLength(3)
        .setImageHeight(5)
        .setSkip({1, 2, 1}),
        PixelFormat::RGB, PixelType::UnsignedByte, {}, Containers::Array<char>{ValueInit, sizeof(Data3D)}};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(image.data(), Containers::arrayView(Data3D),
        TestSuite::Compare::Container);
}
#endif

void PixelStorageGLTest::defaultsAfterStateReset() {
    /* Calling Context::resetState() should trigger re-setting of all pixel
       storage parameters because they're assumed to be in an unknown state.
       It should however not attempt to set pixel storage parameters for
       features that aren't supported, such as when the EXT_unpack_subimage
       extension isn't available.

       In other words, there should be no GL error caused by the resetState()
       call. */

    Context::current().resetState(Context::State::PixelStorage);

    const char data[]{
        '\x00', '\x01', '\x02', '\xff',
        '\x03', '\x04', '\x05', '\xff',
        '\x06', '\x07', '\x08', '\xff',

        '\x0a', '\x0b', '\x0c', '\xff',
        '\x0d', '\x0e', '\x0f', '\xff',
        '\x10', '\x11', '\x12', '\xff',
    };
    Texture2D texture;
    texture.setImage(0, textureFormat(Magnum::PixelFormat::RGBA8Unorm), ImageView2D{Magnum::PixelFormat::RGBA8Unorm, {3, 2}, data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    Image2D actual = texture.image(0, Magnum::PixelFormat::RGBA8Unorm);
    #else
    Framebuffer framebuffer{{{}, {3, 2}}};
    framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);
    Image2D actual = framebuffer.read(framebuffer.viewport(), Magnum::PixelFormat::RGBA8Unorm);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(actual.data(), Containers::arrayView(data), TestSuite::Compare::Container);
}

#if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
void PixelStorageGLTest::rowLengthNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[4*4*4]{};
    MutableImageView2D image{
        PixelStorage{}
            .setRowLength(4),
        Magnum::PixelFormat::RGBA8Unorm,
        {4, 4},
        data};

    Texture2D texture;
    /* Just to reset all pixel storage parameters potentially set by any of the
       above tests to default. The (graceful) assertions would do an early
       return somewhere in the middle, leading to some params being left at
       their earlier state. */
    texture.setImage(0,
        textureFormat(image.format()),
        ImageView2D{image.format(), image.size(), image.data()});
    Framebuffer framebuffer{{}};
    framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);
    /* ... and similarly to reset unpack parameters */
    framebuffer.read({{}, image.size()},
        MutableImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::String out;
    Error redirectError{&out};
    texture.setImage(0, textureFormat(image.format()), image);
    framebuffer.read({{}, image.size()}, image);
    CORRADE_COMPARE_AS(out,
        "GL: non-default PixelStorage::rowLength() is not supported in WebGL 1.0\n"
        "GL: non-default PixelStorage::rowLength() is not supported in WebGL 1.0\n",
        TestSuite::Compare::String);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* And again to reset these for any tests that might run after */
    texture.setImage(0,
        textureFormat(image.format()),
        ImageView2D{image.format(), image.size(), image.data()});
    framebuffer.read({{}, image.size()},
        MutableImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifdef MAGNUM_TARGET_GLES2
void PixelStorageGLTest::imageHeightNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[4*4*4]{};
    MutableImageView2D image{
        PixelStorage{}
            .setImageHeight(4),
        Magnum::PixelFormat::RGBA8Unorm,
        {4, 4},
        data};

    Texture2D texture;
    /* Just to reset all pixel storage parameters potentially set by any of the
       above tests to default. The (graceful) assertions would do an early
       return somewhere in the middle, leading to some params being left at
       their earlier state. */
    texture.setImage(0,
        textureFormat(image.format()),
        ImageView2D{image.format(), image.size(), data});

    Framebuffer framebuffer{{}};
    framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);
    /* ... and similarly to reset unpack parameters */
    framebuffer.read({{}, image.size()},
        MutableImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::String out;
    Error redirectError{&out};
    texture.setImage(0, textureFormat(image.format()), image);
    framebuffer.read({{}, image.size()}, image);
    CORRADE_COMPARE_AS(out,
        "GL: non-default PixelStorage::imageHeight() is not supported in OpenGL ES 2\n"
        "GL: non-default PixelStorage::imageHeight() is not supported in OpenGL ES 2\n",
        TestSuite::Compare::String);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* And again to reset these for any tests that might run after */
    texture.setImage(0,
        textureFormat(image.format()),
        ImageView2D{image.format(), image.size(), data});
    framebuffer.read({{}, image.size()},
        MutableImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#elif defined(MAGNUM_TARGET_GLES)
void PixelStorageGLTest::imageHeightSkipZPackNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Context::current().resetState(GL::Context::State::PixelStorage);

    char data[4*4*4*2]{}; /* twice for skip Z */
    Texture2D texture;
    /* Just so the texure can be correctly read from */
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D{Magnum::PixelFormat::RGBA8Unorm, {4, 4}, data});

    Framebuffer framebuffer{{}};
    framebuffer.attachTexture(Framebuffer::ColorAttachment{0}, texture, 0);
    /* Just to reset all pixel storage parameters potentially set by any of the
       above tests to default. The (graceful) assertions would do an early
       return somewhere in the middle, leading to some params being left at
       their earlier state. */
    framebuffer.read({{}, {4, 4}},
        MutableImageView2D{Magnum::PixelFormat::RGBA8Unorm, {4, 4}, data});

    MutableImageView2D imageImageHeight{
        PixelStorage{}
            .setImageHeight(4),
        Magnum::PixelFormat::RGBA8Unorm,
        {4, 4},
        data};
    MutableImageView2D imageSkipZ{
        PixelStorage{}
            .setSkip({0, 0, 1}),
        Magnum::PixelFormat::RGBA8Unorm,
        {4, 4},
        data};

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::String out;
    Error redirectError{&out};
    framebuffer.read({{}, imageImageHeight.size()}, imageImageHeight);
    framebuffer.read({{}, imageSkipZ.size()}, imageSkipZ);
    CORRADE_COMPARE_AS(out,
        "GL: non-default PixelStorage::imageHeight() for pack is not supported in OpenGL ES\n"
        "GL: non-default PixelStorage::skip().z() for pack is not supported in OpenGL ES\n",
        TestSuite::Compare::String);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* And again to reset these for any tests that might run after */
    framebuffer.read({{}, {4, 4}},
        MutableImageView2D{Magnum::PixelFormat::RGBA8Unorm, {4, 4}, data});

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
constexpr const UnsignedByte CompressedData2D[]{
    /* Skip rows (5 blocks) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Image data row (2 blocks skipped, 2 data, 1 more for 5 total) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Image data row (2 blocks skipped, 2 data, 1 more for 5 total) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Image data row (2 blocks skipped, 2 data, 1 more for 5 total) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#endif

/* Just two different 16-byte RGBA DXT3 blocks mixed to form 6 blocks. Used by
   the non-GLES compressedUnpack*D() / compressedPack*D() tests as well as the
   ES-only compressedResetParameters() test.  */
constexpr UnsignedByte ActualCompressedData2D[]{
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
};

#ifndef MAGNUM_TARGET_GLES
void PixelStorageGLTest::compressedUnpack2D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CompressedPixelStorage storage;
    storage
        .setRowLength(20)
        .setSkip({8, 4, 0});

    CompressedImageView2D image{storage, CompressedPixelFormat::RGBAS3tcDxt3, {8, 12}, CompressedData2D};

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {8, 12})
        .setCompressedSubImage(0, {}, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage2D actual = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(actual.data()),
        Containers::arrayView(ActualCompressedData2D),
        TestSuite::Compare::Container);
}

void PixelStorageGLTest::compressedPack2D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CompressedImageView2D actual{CompressedPixelFormat::RGBAS3tcDxt3, {8, 12}, ActualCompressedData2D};

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {8, 12})
        .setCompressedSubImage(0, {}, actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Pre-allocate and zero out the data array so we can conveniently compare */
    CompressedImage2D image{CompressedPixelStorage{}
        .setRowLength(20)
        .setSkip({8, 4, 0}),
        CompressedPixelFormat::RGBAS3tcDxt3, {}, Containers::Array<char>{ValueInit, sizeof(CompressedData2D)}};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedData2D),
        TestSuite::Compare::Container);
}

constexpr const UnsignedByte CompressedData3D[] = {
    /* Skip first image (5x6 blocks) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Skip second image (5x6 blocks) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* First image data slice (1 row skipped, 3 data, 2 more for 6 total),
       each data row then 2 blocks skipped, 2 data, 1 more for 5 total */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Data */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Two more rows */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Second image data slice (1 row skipped, 3 data, 2 more for 6 total),
       each data row then 2 blocks skipped, 2 data, 1 more for 5 total */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    /* Two more rows */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* ActualCompressedData2D but repeated two times */
constexpr UnsignedByte ActualCompressedData3D[]{
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,

    /* Second slice with the two different blocks swapped */
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    68, 84, 85, 101, 102, 118, 119, 119, 239, 123, 8, 66, 213, 255, 170, 2,
    0, 17, 17, 34, 34, 51, 51, 67, 232, 57, 0, 0, 213, 255, 170, 2,
};

void PixelStorageGLTest::compressedUnpack3D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CompressedPixelStorage storage;
    storage
        .setRowLength(20)
        .setImageHeight(24)
        .setSkip({8, 4, 2});

    CompressedImageView3D image{storage, CompressedPixelFormat::RGBAS3tcDxt3, {8, 12, 2}, CompressedData3D};

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {8, 12, 2})
        .setCompressedSubImage(0, {}, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D actual = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(actual.data()),
        Containers::arrayView(ActualCompressedData3D),
        TestSuite::Compare::Container);
}

void PixelStorageGLTest::compressedPack3D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    CompressedImageView3D actual{CompressedPixelFormat::RGBAS3tcDxt3, {8, 12, 2}, ActualCompressedData3D};

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {8, 12, 2})
        .setCompressedSubImage(0, {}, actual);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Pre-allocate and zero out the data array so we can conveniently compare */
    CompressedImage3D image{CompressedPixelStorage{}
        .setRowLength(20)
        .setImageHeight(24)
        .setSkip({8, 4, 2}),
        CompressedPixelFormat::RGBAS3tcDxt3, {}, Containers::Array<char>{ValueInit, sizeof(CompressedData3D)}};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedData3D),
        TestSuite::Compare::Container);
}
#endif

void PixelStorageGLTest::compressedResetParameters() {
    #if !defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #endif

    /* This checks that uploading a compressed image doesn't use pixel storage
       parameters from the previous uncompressed upload. While the ES spec
       seems to say that these are all ignored when uploading a compressed
       image (and so resetting them shouldn't be needed), with a WebGL 2 build
       Chrome is complaining that the pixel unpack parameters are invalid if
       they're not explicitly reset to zero before the compressed upload.
       Firefox doesn't mind. On WebGL 1 row length / skip isn't supported so
       they don't get set and thus Chrome doesn't complain.

       Testing on desktop GL as well, even though there it resets just because
       the implicitly used storage is all defaults. */

    /* Pick a supported 128-bit 4x4 format if available. The data uploaded are
       always BC2 / DXT3 but since they're not rendered from it should be fine
       even for ETC, */
    Magnum::CompressedPixelFormat format;
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>() ||
       Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
    #endif
    {
        format = Magnum::CompressedPixelFormat::Bc2RGBAUnorm;
    } else
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::ES3_compatibility>())
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_etc>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::ANGLE::compressed_texture_etc>())
    #else
    /* On ES3 ETC textures are available always */
    #endif
    {
        format = Magnum::CompressedPixelFormat::Etc2RGBA8Unorm;
    }
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_TARGET_GLES2)
    else {
        #ifndef MAGNUM_TARGET_GLES
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_compression_s3tc::string() << "nor" << Extensions::ARB::ES3_compatibility::string() << "is supported, can't test");
        #elif defined(MAGNUM_TARGET_WEBGL)
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "not supported, can't test");
        #else
        CORRADE_SKIP("None of" << Extensions::EXT::texture_compression_s3tc::string() << Debug::nospace << "," << Extensions::ANGLE::texture_compression_dxt3::string() << "or" << Extensions::ANGLE::compressed_texture_etc::string() << "extensions are supported, can't test");
        #endif
    }
    #endif

    char data[20*4];
    Texture2D uncompressed;
    uncompressed.setImage(0, textureFormat(Magnum::PixelFormat::RGB8Unorm),
        ImageView2D{
            PixelStorage{}
                .setAlignment(2)
                /* Assume these are supported on ES2 */
                #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
                .setRowLength(6)
                .setSkip({1, 2, 0})
                #endif
            , Magnum::PixelFormat::RGB8Unorm, {3, 2}, data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Texture2D compressed;
    compressed.setCompressedImage(0,
        CompressedImageView2D{format, {8, 12}, ActualCompressedData2D});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Verify that the skip etc arguments indeed weren't used, just in case.
       They're all not whole multiples of compressed blocks so they should also
       cause a GL error if used by accident. */
    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = compressed.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(ActualCompressedData2D),
        TestSuite::Compare::Container);
    #endif
}

void PixelStorageGLTest::compressedDefaultsAfterStateReset() {
    /* Like defaultsAfterStateReset(), but with a compressed format */

    /* Pick a supported 128-bit 4x4 format if available. The data uploaded are
       always BC2 / DXT3 but since they're not rendered from it should be fine
       even for ETC, */
    Magnum::CompressedPixelFormat format;
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>() ||
       Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
    #endif
    {
        format = Magnum::CompressedPixelFormat::Bc2RGBAUnorm;
    } else
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::ES3_compatibility>())
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_etc>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::ANGLE::compressed_texture_etc>())
    #else
    /* On ES3 ETC textures are available always */
    #endif
    {
        format = Magnum::CompressedPixelFormat::Etc2RGBA8Unorm;
    }
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_TARGET_GLES2)
    else {
        #ifndef MAGNUM_TARGET_GLES
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_compression_s3tc::string() << "nor" << Extensions::ARB::ES3_compatibility::string() << "is supported, can't test");
        #elif defined(MAGNUM_TARGET_WEBGL)
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "not supported, can't test");
        #else
        CORRADE_SKIP("None of" << Extensions::EXT::texture_compression_s3tc::string() << Debug::nospace << "," << Extensions::ANGLE::texture_compression_dxt3::string() << "or" << Extensions::ANGLE::compressed_texture_etc::string() << "extensions are supported, can't test");
        #endif
    }
    #endif

    Context::current().resetState(Context::State::PixelStorage);

    Texture2D texture;
    texture.setCompressedImage(0,
        CompressedImageView2D{format, {8, 12}, ActualCompressedData2D});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* There's no way to test the compressed contents on ES */
    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(ActualCompressedData2D),
        TestSuite::Compare::Container);
    #endif
}

#ifdef MAGNUM_TARGET_GLES
void PixelStorageGLTest::compressedNotSupported() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Pick a 64-bit 4x4 format. Zero data are uploaded so it doesn't matter
       which it is. */
    Magnum::CompressedPixelFormat format;
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>() ||
       Context::current().isExtensionSupported<Extensions::EXT::texture_compression_dxt1>() ||
       Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt1>())
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
    #endif
    {
        format = Magnum::CompressedPixelFormat::Bc1RGBUnorm;
    } else
    #ifdef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_etc>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::ANGLE::compressed_texture_etc>())
    #else
    /* On ES3 ETC textures are available always */
    #endif
    {
        format = Magnum::CompressedPixelFormat::Etc2RGB8Unorm;
    }
    #if defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_TARGET_GLES2)
    else {
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "not supported, can't test");
        #else
        CORRADE_SKIP("None of" << Extensions::EXT::texture_compression_s3tc::string() << Debug::nospace << "," << Extensions::EXT::texture_compression_dxt1::string() << Debug::nospace << "," << Extensions::ANGLE::texture_compression_dxt1::string() << "or" << Extensions::ANGLE::compressed_texture_etc::string() << "extensions are supported, can't test");
        #endif
    }
    #endif

    char data[8]{};
    MutableCompressedImageView2D image{
        /* Just set any random property to make it different from the
           default-constructed instance to trigger the assert */
        CompressedPixelStorage{}
            .setRowLength(4),
        format,
        {4, 4},
        data};

    Texture2D texture;
    /* Just to reset all pixel storage parameters potentially set by any of the
       above tests to default. The (graceful) assertions would do an early
       return somewhere in the middle, leading to some params being left at
       their earlier state. */
    texture.setCompressedImage(0,
        CompressedImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::String out;
    Error redirectError{&out};
    texture.setCompressedImage(0, image);
    /* There isn't any way to use CompressedPixelStorage for pixel pack on
       GLES */
    CORRADE_COMPARE_AS(out,
        "GL: non-default CompressedPixelStorage parameters are not supported in OpenGL ES or WebGL\n",
        TestSuite::Compare::String);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* And again to reset these for any tests that might run after */
    texture.setCompressedImage(0,
        CompressedImageView2D{image.format(), image.size(), data});

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PixelStorageGLTest)
