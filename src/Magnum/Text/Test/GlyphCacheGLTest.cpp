/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/StringView.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Text/GlyphCache.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct GlyphCacheGLTest: GL::OpenGLTester {
    explicit GlyphCacheGLTest();

    void initialize();
    void initializeExplicitFormat();

    void setImage();
};

GlyphCacheGLTest::GlyphCacheGLTest() {
    addTests({&GlyphCacheGLTest::initialize,
              &GlyphCacheGLTest::initializeExplicitFormat,

              &GlyphCacheGLTest::setImage});
}

void GlyphCacheGLTest::initialize() {
    GlyphCache cache{{1024, 2048}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.textureSize(), (Vector2i{1024, 2048}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{1024, 2048}));
    #endif
}

void GlyphCacheGLTest::initializeExplicitFormat() {
    GlyphCache cache{
        #ifndef MAGNUM_TARGET_GLES2
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        {256, 512}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.textureSize(), (Vector2i{256, 512}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{256, 512}));
    #endif
}

void GlyphCacheGLTest::setImage() {
    GlyphCache cache{{16, 8}};

    /* Clear the texture first, as it'd have random garbage otherwise */
    UnsignedByte zeros[16*8]{};
    cache.setImage({}, ImageView2D{PixelFormat::R8Unorm, {16, 8}, zeros});
    MAGNUM_VERIFY_NO_GL_ERROR();

    UnsignedByte data[]{
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
    cache.setImage({8, 4}, ImageView2D{PixelFormat::R8Unorm, {8, 4}, data});
    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    Image2D actual = cache.image();
    MAGNUM_VERIFY_NO_GL_ERROR();

    UnsignedByte expected[]{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
    CORRADE_COMPARE_AS(actual,
        (ImageView2D{PixelFormat::R8Unorm, {16, 8}, expected}),
        DebugTools::CompareImage);
    #else
    CORRADE_SKIP("Skipping image download test as it's not available on GLES.");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::GlyphCacheGLTest)
