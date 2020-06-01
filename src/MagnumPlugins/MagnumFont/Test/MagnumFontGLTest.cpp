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

#include <sstream>
#include <Corrade/Utility/Directory.h>

#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/GlyphCache.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct MagnumFontGLTest: GL::OpenGLTester {
    explicit MagnumFontGLTest();

    void createGlyphCache();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
    PluginManager::Manager<AbstractFont> _fontManager{"nonexistent"};
};

MagnumFontGLTest::MagnumFontGLTest() {
    addTests({&MagnumFontGLTest::createGlyphCache});

    /* Load the plugins directly from the build tree. Otherwise they're static
       and already loaded. */
    _fontManager.registerExternalManager(_importerManager);
    #if defined(TGAIMPORTER_PLUGIN_FILENAME) && defined(MAGNUMFONT_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT_OUTPUT(_fontManager.load(MAGNUMFONT_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void MagnumFontGLTest::createGlyphCache() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    CORRADE_VERIFY(font->openFile(Utility::Directory::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));

    /* Just testing that nothing crashes, asserts or errors */
    Containers::Pointer<AbstractGlyphCache> cache = font->createGlyphCache();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cache);
    CORRADE_COMPARE(cache->glyphCount(), 3);

    /** @todo properly test contents */
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontGLTest)
