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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/DebugTools/TextureImage.h"
#endif
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/GlyphCache.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct GlyphCacheGLTest: GL::OpenGLTester {
    explicit GlyphCacheGLTest();

    void construct();
    void constructCustomFormat();

    void constructCopy();
    void constructMove();

    void setImage();
    void setImageCustomFormat();
};

GlyphCacheGLTest::GlyphCacheGLTest() {
    addTests({&GlyphCacheGLTest::construct,
              &GlyphCacheGLTest::constructCustomFormat,

              &GlyphCacheGLTest::constructCopy,
              &GlyphCacheGLTest::constructMove,

              &GlyphCacheGLTest::setImage,
              &GlyphCacheGLTest::setImageCustomFormat});
}

void GlyphCacheGLTest::construct() {
    GlyphCache cache{{1024, 2048}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{1024, 2048, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{1024, 2048}));
    #endif
}

void GlyphCacheGLTest::constructCustomFormat() {
    GlyphCache cache{
        #ifndef MAGNUM_TARGET_GLES2
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        {256, 512}};
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(cache.size(), (Vector3i{256, 512, 1}));
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), (Vector2i{256, 512}));
    #endif
}

void GlyphCacheGLTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<GlyphCache>{});
    CORRADE_VERIFY(!std::is_copy_assignable<GlyphCache>{});
}

void GlyphCacheGLTest::constructMove() {
    GlyphCache a{{1024, 512}};

    GlyphCache b = Utility::move(a);
    CORRADE_COMPARE(b.size(), (Vector3i{1024, 512, 1}));

    GlyphCache c{{2, 3}};
    c = Utility::move(b);
    CORRADE_COMPARE(c.size(), (Vector3i{1024, 512, 1}));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<GlyphCache>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<GlyphCache>::value);
}

const UnsignedByte InputData[]{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
};

const UnsignedByte ExpectedData[]{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0, 0, 0, 0, 0, 0, 0, 0, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
};

void GlyphCacheGLTest::setImage() {
    GlyphCache cache{{16, 8}};

    Utility::copy(
        Containers::StridedArrayView2D<const UnsignedByte>{InputData, {4, 8}},
        cache.image().pixels<UnsignedByte>()[0].sliceSize({4, 8}, {4, 8}));
    cache.flushImage(Range2Di::fromSize({8, 4}, {8, 4}));
    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView3D actual3 = cache.image();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImageView2D expected{PixelFormat::R8Unorm, {16, 8}, ExpectedData};
    CORRADE_COMPARE_AS(actual,
        expected,
        DebugTools::CompareImage);

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Luminance format used on GLES2 isn't usable for framebuffer reading, can't verify texture contents.");
    #else
    /* Verify the actual texture. It should be the same as above. On GLES we
       cannot really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = cache.texture().image(0, {PixelFormat::R8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(cache.texture(), 0, {{}, {16, 8}}, {PixelFormat::R8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(image,
        expected,
        DebugTools::CompareImage);
    #endif
}

void GlyphCacheGLTest::setImageCustomFormat() {
    /* Same as setImage(), but with a four-channel format (so quarter of
       width). Needed to be able to read the texture on ES2 to verify the
       upload works, as there's a special case for when the EXT_unpack_subimage
       extension isn't present. */

    GlyphCache cache{
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        GL::TextureFormat::RGBA8,
        #else
        GL::TextureFormat::RGBA,
        #endif
        {4, 8}};

    Utility::copy(
        Containers::StridedArrayView2D<const Color4ub>{Containers::arrayCast<const Color4ub>(Containers::arrayView(InputData)), {4, 2}},
        cache.image().pixels<Color4ub>()[0].sliceSize({4, 2}, {4, 2}));
    cache.flushImage(Range2Di::fromSize({2, 4}, {2, 4}));
    MAGNUM_VERIFY_NO_GL_ERROR();

    MutableImageView3D actual3 = cache.image();
    /** @todo ugh have slicing on images directly already */
    MutableImageView2D actual{actual3.format(), actual3.size().xy(), actual3.data()};
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImageView2D expected{PixelFormat::RGBA8Unorm, {4, 8}, ExpectedData};
    CORRADE_COMPARE_AS(actual,
        expected,
        DebugTools::CompareImage);

    /* Verify the actual texture. It should be the same as above. On GLES we
       cannot really verify that the size matches, but at least something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = cache.texture().image(0, {PixelFormat::RGBA8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(cache.texture(), 0, {{}, {4, 8}}, {PixelFormat::RGBA8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(image,
        expected,
        DebugTools::CompareImage);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::GlyphCacheGLTest)
