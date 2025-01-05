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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/DebugTools/TextureImage.h"
#endif
#include "Magnum/Math/Range.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/GlyphCacheGL.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct MagnumFontGLTest: GL::OpenGLTester {
    explicit MagnumFontGLTest();

    void createGlyphCache();
    void createGlyphCacheProcessedImage();
    void createGlyphCacheNoGlyphs();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
    PluginManager::Manager<AbstractFont> _fontManager{"nonexistent"};
};

MagnumFontGLTest::MagnumFontGLTest() {
    addTests({&MagnumFontGLTest::createGlyphCache,
              &MagnumFontGLTest::createGlyphCacheProcessedImage,
              &MagnumFontGLTest::createGlyphCacheNoGlyphs});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree. */
    _fontManager.registerExternalManager(_importerManager);
    #if defined(TGAIMPORTER_PLUGIN_FILENAME) && defined(MAGNUMFONT_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT_OUTPUT(_fontManager.load(MAGNUMFONT_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void MagnumFontGLTest::createGlyphCache() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));

    Containers::Pointer<AbstractGlyphCache> cache = font->createGlyphCache();
    CORRADE_VERIFY(cache);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The font should associate itself with the cache */
    CORRADE_COMPARE(cache->fontCount(), 1);
    CORRADE_COMPARE(cache->findFont(*font), 0);

    /* Verify glyph contents */
    CORRADE_COMPARE(cache->glyphCount(), 5);
    CORRADE_COMPARE(cache->fontGlyphCount(0), 4);
    CORRADE_COMPARE(cache->glyph(0), Containers::triple(
        Vector2i{-16, -8},
        0,
        Range2Di{{0, 0}, {32, 16}}));
    CORRADE_COMPARE(cache->glyph(0, 0), Containers::triple(
        Vector2i{-16, -8},
        0,
        Range2Di{{0, 0}, {32, 16}}));
    CORRADE_COMPARE(cache->glyph(0, font->glyphId(U'W')), Containers::triple(
        Vector2i{9, 26},
        0,
        Range2Di{{0, 4}, {40, 64}}));
    CORRADE_COMPARE(cache->glyph(0, font->glyphId(U'e')), Containers::triple(
        Vector2i{9, 4},
        0,
        Range2Di{{20, 0}, {128, 48}}));
    /* ě has deliberately the same glyph data as e */
    UnsignedInt eId = font->glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    );
    CORRADE_COMPARE(cache->glyph(0, eId), Containers::triple(
        Vector2i{9, 4},
        0,
        Range2Di{{20, 0}, {128, 48}}));

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Luminance format used on GLES2 isn't usable for framebuffer reading, can't verify texture contents.");
    #else
    /* Verify the actual texture. It should be the image file verbatim. On GLES
       we cannot really verify that the size matches, but at least
       something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = static_cast<GlyphCacheGL&>(*cache).texture().image(0, {PixelFormat::R8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(static_cast<GlyphCacheGL&>(*cache).texture(), 0, {{}, {128, 64}}, {PixelFormat::R8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(image,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.tga"),
        DebugTools::CompareImageToFile{_importerManager});
    #endif
}

void MagnumFontGLTest::createGlyphCacheProcessedImage() {
    /* Compared to createGlyphCache(), this tests the case where the image size
       is different from the actual size to which glyphs are positioned */

    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font-processed.conf"), 0.0f));

    Containers::Pointer<AbstractGlyphCache> cache = font->createGlyphCache();
    CORRADE_VERIFY(cache);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The font should associate itself with the cache */
    CORRADE_COMPARE(cache->fontCount(), 1);
    CORRADE_COMPARE(cache->findFont(*font), 0);

    /* Verify glyph contents */
    CORRADE_COMPARE(cache->glyphCount(), 5);
    CORRADE_COMPARE(cache->fontGlyphCount(0), 4);
    CORRADE_COMPARE(cache->glyph(0), Containers::triple(
        Vector2i{-16, -8},
        0,
        Range2Di{{0, 0}, {32, 16}}));
    CORRADE_COMPARE(cache->glyph(0, 0), Containers::triple(
        Vector2i{-16, -8},
        0,
        Range2Di{{0, 0}, {32, 16}}));
    CORRADE_COMPARE(cache->glyph(0, font->glyphId(U'W')), Containers::triple(
        Vector2i{9, 26},
        0,
        Range2Di{{0, 4}, {40, 64}}));
    CORRADE_COMPARE(cache->glyph(0, font->glyphId(U'e')), Containers::triple(
        Vector2i{9, 4},
        0,
        Range2Di{{20, 0}, {128, 48}}));
    /* ě has deliberately the same glyph data as e */
    UnsignedInt eId = font->glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    );
    CORRADE_COMPARE(cache->glyph(0, eId), Containers::triple(
        Vector2i{9, 4},
        0,
        Range2Di{{20, 0}, {128, 48}}));

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found, not testing glyph cache contents");

    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Luminance format used on GLES2 isn't usable for framebuffer reading, can't verify texture contents.");
    #else
    /* Verify the actual texture. It should be the image file verbatim. On GLES
       we cannot really verify that the size matches, but at least
       something. */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = static_cast<GlyphCacheGL&>(*cache).texture().image(0, {PixelFormat::R8Unorm});
    #else
    Image2D image = DebugTools::textureSubImage(static_cast<GlyphCacheGL&>(*cache).texture(), 0, {{}, {8, 4}}, {PixelFormat::R8Unorm});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_WITH(image,
        Utility::Path::join(MAGNUMFONT_TEST_DIR, "font-processed.tga"),
        DebugTools::CompareImageToFile{_importerManager});
    #endif
}

void MagnumFontGLTest::createGlyphCacheNoGlyphs() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font-empty.conf"), 0.0f));

    Containers::Pointer<AbstractGlyphCache> cache = font->createGlyphCache();
    CORRADE_VERIFY(cache);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The font should associate itself with the cache even in this case */
    CORRADE_COMPARE(cache->fontCount(), 1);
    CORRADE_COMPARE(cache->findFont(*font), 0);

    /* There's just the empty glyph added by the cache itself, nothing else */
    CORRADE_COMPARE(cache->glyphCount(), 1);
    CORRADE_COMPARE(cache->fontGlyphCount(0), 0);
    CORRADE_COMPARE(cache->glyph(0), Containers::triple(
        Vector2i{},
        0,
        Range2Di{}));

    /* Not testing the image as there's no special codepath taken for it if
       there are no glyphs */
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontGLTest)
