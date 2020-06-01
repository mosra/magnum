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
#include <tuple>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Text/AbstractGlyphCache.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractGlyphCacheTest: TestSuite::Tester {
    explicit AbstractGlyphCacheTest();

    void initialize();
    void access();
    void reserve();

    void setImage();
    void setImageOutOfBounds();

    void image();
    void imageNotSupported();
    void imageNotImplemented();
};

AbstractGlyphCacheTest::AbstractGlyphCacheTest() {
    addTests({&AbstractGlyphCacheTest::initialize,
              &AbstractGlyphCacheTest::access,
              &AbstractGlyphCacheTest::reserve,

              &AbstractGlyphCacheTest::setImage,
              &AbstractGlyphCacheTest::setImageOutOfBounds,

              &AbstractGlyphCacheTest::image,
              &AbstractGlyphCacheTest::imageNotSupported,
              &AbstractGlyphCacheTest::imageNotImplemented});
}

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

void AbstractGlyphCacheTest::initialize() {
    DummyGlyphCache cache({1024, 2048});

    CORRADE_COMPARE(cache.textureSize(), (Vector2i{1024, 2048}));
}

void AbstractGlyphCacheTest::access() {
    DummyGlyphCache cache(Vector2i(236));
    Vector2i position;
    Range2Di rectangle;

    /* Default "Not Found" glyph */
    CORRADE_COMPARE(cache.glyphCount(), 1);
    std::tie(position, rectangle) = cache[0];
    CORRADE_COMPARE(position, Vector2i(0, 0));
    CORRADE_COMPARE(rectangle, Range2Di({0, 0}, {0, 0}));

    /* Overwrite the "Not Found" glyph */
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

void AbstractGlyphCacheTest::reserve() {
    DummyGlyphCache cache(Vector2i(236));

    /* Verify that this works for "empty" cache */
    CORRADE_VERIFY(!cache.reserve({{5, 3}}).empty());
}

void AbstractGlyphCacheTest::setImage() {
    struct MyGlyphCache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i& offset, const ImageView2D& image) override {
            imageOffset = offset;
            imageSize = image.size();
        }

        Vector2i imageOffset, imageSize;
    } cache{{100, 200}};

    cache.setImage({80, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});

    CORRADE_COMPARE(cache.imageOffset, (Vector2i{80, 175}));
    CORRADE_COMPARE(cache.imageSize, (Vector2i{20, 25}));
}

void AbstractGlyphCacheTest::setImageOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    DummyGlyphCache cache{{100, 200}};

    std::ostringstream out;
    Error redirectError{&out};
    cache.setImage({80, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setImage({81, 175}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});
    cache.setImage({80, -1}, ImageView2D{PixelFormat::R8Unorm, {20, 25}});

    CORRADE_COMPARE(out.str(),
        "Text::AbstractGlyphCache::setImage(): Range({81, 175}, {101, 200}) out of bounds for texture size Vector(100, 200)\n"
        "Text::AbstractGlyphCache::setImage(): Range({80, -1}, {100, 24}) out of bounds for texture size Vector(100, 200)\n");
}

void AbstractGlyphCacheTest::image() {
    struct MyGlyphCache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ImageDownload; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}

        Image2D doImage() override { return Image2D{PixelFormat::RG8Unorm}; }
    } cache{{200, 300}};

    Image2D image = cache.image();
    CORRADE_COMPARE(image.format(), PixelFormat::RG8Unorm);
}

void AbstractGlyphCacheTest::imageNotSupported() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct MyGlyphCache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{{200, 300}};

    std::ostringstream out;
    Error redirectError{&out};
    cache.image();
    CORRADE_COMPARE(out.str(), "Text::AbstractGlyphCache::image(): feature not supported\n");
}

void AbstractGlyphCacheTest::imageNotImplemented() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    struct MyGlyphCache: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return GlyphCacheFeature::ImageDownload; }
        void doSetImage(const Vector2i&, const ImageView2D&) override {}
    } cache{{200, 300}};

    std::ostringstream out;
    Error redirectError{&out};
    cache.image();
    CORRADE_COMPARE(out.str(), "Text::AbstractGlyphCache::image(): feature advertised but not implemented\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractGlyphCacheTest)
