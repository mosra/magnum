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

#include <tuple>

#include "Magnum/OpenGLTester.h"
#include "Magnum/Text/GlyphCache.h"

namespace Magnum { namespace Text { namespace Test {

struct GlyphCacheGLTest: OpenGLTester {
    explicit GlyphCacheGLTest();

    void initialize();
    void access();
    void reserve();
};

GlyphCacheGLTest::GlyphCacheGLTest() {
    addTests({&GlyphCacheGLTest::initialize,
              &GlyphCacheGLTest::access,
              &GlyphCacheGLTest::reserve});
}

void GlyphCacheGLTest::initialize() {
    Text::GlyphCache cache({1024, 2048});
    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(cache.texture().imageSize(0), Vector2i(1024, 2048));
    #endif
}

void GlyphCacheGLTest::access() {
    Text::GlyphCache cache(Vector2i(236));
    Vector2i position;
    Range2Di rectangle;

    /* Default "Not Found" glyph */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    std::tie(position, rectangle) = cache[0];
    CORRADE_COMPARE(position, Vector2i(0, 0));
    CORRADE_COMPARE(rectangle, Range2Di({0, 0}, {0, 0}));

    /* Overwrite "Not Found" glyph */
    cache.insert(0, {3, 5}, {{10, 10}, {23, 45}});
    CORRADE_COMPARE(cache.glyphCount(), 1);
    std::tie(position, rectangle) = cache[0];
    CORRADE_COMPARE(position, Vector2i(3, 5));
    CORRADE_COMPARE(rectangle, Range2Di({10, 10}, {23, 45}));

    /* Querying available glyph */
    cache.insert(25, {3, 4}, {{15, 30}, {45, 35}});
    CORRADE_COMPARE(cache.glyphCount(), 2);
    std::tie(position, rectangle) = cache[25];
    CORRADE_COMPARE(position, Vector2i(3, 4));
    CORRADE_COMPARE(rectangle, Range2Di({15, 30}, {45, 35}));

    /* Querying not available glyph falls back to "Not Found" */
    std::tie(position, rectangle) = cache[42];
    CORRADE_COMPARE(position, Vector2i(3, 5));
    CORRADE_COMPARE(rectangle, Range2Di({10, 10}, {23, 45}));
}

void GlyphCacheGLTest::reserve() {
    Text::GlyphCache cache(Vector2i(236));

    /* Verify that this works for "empty" cache */
    CORRADE_VERIFY(!cache.reserve({{5, 3}}).empty());
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::GlyphCacheGLTest)
