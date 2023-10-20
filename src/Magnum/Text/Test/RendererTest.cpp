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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h> /** @todo drop once Debug is stream-free */

#include "Magnum/PixelFormat.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    void renderData();

    void multiline();

    #ifdef MAGNUM_TARGET_GL
    void arrayGlyphCache();
    void fontNotFoundInCache();
    #endif
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Alignment alignment;
    Vector2 offset;
} RenderDataData[]{
    /* Not testing all combinations, just making sure that each horizontal,
       vertical, glyph bounds and integer variant is covered */
    {"line left", Alignment::LineLeft,
        /* This is the default (0) value, thus should result in no shift */
        {}},
    {"line left, glyph bounds", Alignment::LineLeftGlyphBounds,
        /* The first glyph has X offset of 2.5, which is subtracted */
        {-2.5f, 0.0f}},
    {"top left", Alignment::TopLeft,
        /* Ascent is 4.5, scaled by 0.5 */
        {0.0f, -2.25f}},
    {"top left, glyph bounds", Alignment::TopLeftGlyphBounds,
        /* Largest Y value is 10.5f */
        {-2.5f, -10.5f}},
    {"top right", Alignment::TopRight,
        /* Advances were 1, 2, 3, so 6 in total, ascent is 4.5; scaled by
           0.5 */
        {-3.0f, -2.25f}},
    {"top right, glyph bounds", Alignment::TopRightGlyphBounds,
        /* Basically subtracting the largest vertex value */
        {-12.5f, -10.5f}},
    {"top center", Alignment::TopCenter,
        /* Advances were 1, 2, 3, so 6 in total, center is 3, scaled by 0.5 */
        {-1.5f, -2.25f}},
    {"top center, integral", Alignment::TopCenterIntegral,
        /* The Y shift isn't whole units but only X is rounded here */
        {-2.0f, -2.25f}},
    {"top center, glyph bounds", Alignment::TopCenterGlyphBounds,
        {-7.5f, -10.5f}},
    {"top center, glyph bounds, integral", Alignment::TopCenterGlyphBoundsIntegral,
        /* The Y shift isn't whole units but only X is rounded here */
        {-8.0f, -10.5f}},
    {"middle left, glyph bounds", Alignment::MiddleLeftGlyphBounds,
        {-2.5f, -7.125f}},
    {"middle left, glyph bounds, integral", Alignment::MiddleLeftGlyphBoundsIntegral,
        /* The X shift isn't whole units but only Y is rounded here */
        {-2.5f, -7.0f}},
    {"middle center", Alignment::MiddleCenter,
        {-1.5f, -0.5f}},
    {"middle center, integral", Alignment::MiddleCenterIntegral,
        /* Rounding happens on both X and Y in this case */
        {-2.0f, -1.0f}},
    {"middle center, glyph bounds", Alignment::MiddleCenterGlyphBounds,
        /* Half size of the bounds quad */
        {-7.5f, -7.125f}},
    {"middle center, glyph bounds, integral", Alignment::MiddleCenterGlyphBoundsIntegral,
        {-8.0f, -7.0f}},
    {"bottom left", Alignment::BottomLeft,
        /* Descent is -2.5; scaled by 0.5 */
        {0.0f, 1.25f}},
    {"bottom right", Alignment::BottomRight,
        {-3.0f, 1.25f}},
    {"bottom right, glyph bounds", Alignment::BottomRightGlyphBounds,
        {-12.5f, -3.75f}},
};

const struct {
    const char* name;
    Alignment alignment;
    /* The Y offset value could be calculated, but this is easier to grasp and
       makes it possible to test overrideable line height later, for example */
    Vector2 offset0, offset1, offset2;
} MultilineData[]{
    {"line left", Alignment::LineLeft,
        {0.0f, -0.0f},
        {0.0f, -4.0f},
        {0.0f, -12.0f}},
    {"line left, glyph bounds", Alignment::LineLeftGlyphBounds,
        {0.0f, 0.0f},
        {0.0f, -4.0f},
        {0.0f, -12.0f}},
    {"middle left", Alignment::MiddleLeft,
        {0.0f, 6.0f},
        {0.0f, 2.0f},
        {0.0f, -6.0f}},
    {"middle left, glyph bounds", Alignment::MiddleLeftGlyphBounds,
        {0.0f, 5.5f},
        {0.0f, 1.5f},
        {0.0f, -6.5f}},
    {"middle left, glyph bounds, integral", Alignment::MiddleLeftGlyphBoundsIntegral,
        {0.0f, 6.0f},
        {0.0f, 2.0f},
        {0.0f, -6.0f}},
    {"middle center", Alignment::MiddleCenter,
        /* The advance for the rightmost glyph is one unit larger than the
           actual bounds which makes it different */
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"middle center, integral", Alignment::MiddleCenterIntegral,
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"middle center, glyph bounds", Alignment::MiddleCenterGlyphBounds,
        {-3.5f, 5.5f},
        {-1.5f, 1.5f},
        {-2.5f, -6.5f}},
    {"middle center, glyph bounds, integral", Alignment::MiddleCenterGlyphBoundsIntegral,
        {-4.0f, 6.0f},
        {-2.0f, 2.0f},
        {-3.0f, -6.0f}},
    {"top right", Alignment::TopRight,
        {-8.0f, -0.5f},
        {-4.0f, -4.5f},
        {-6.0f, -12.5f}},
    {"top right, glyph bounds", Alignment::TopRightGlyphBounds,
        {-7.0f, -1.0f},
        {-3.0f, -5.0f},
        {-5.0f, -13.0f}},
    {"top center", Alignment::TopCenter,
        /* The advance for the rightmost glyph is one unit larger than the
           actual bounds which makes it different */
        {-4.0f, -0.5f},
        {-2.0f, -4.5f},
        {-3.0f, -12.5f}},
    {"top center, integral", Alignment::TopCenterIntegral,
        /* The Y shift isn't whole units but only X (which is already whole
           units) would be rounded here */
        {-4.0f, -0.5f},
        {-2.0f, -4.5f},
        {-3.0f, -12.5f}},
    {"top center, glyph bounds", Alignment::TopCenterGlyphBounds,
        {-3.5f, -1.0f},
        {-1.5f, -5.0f},
        {-2.5f, -13.0f}},
    {"top center, integral", Alignment::TopCenterGlyphBoundsIntegral,
        {-4.0f, -1.0f},
        {-2.0f, -5.0f},
        {-3.0f, -13.0f}},
};

RendererTest::RendererTest() {
    addInstancedTests({&RendererTest::renderData},
        Containers::arraySize(RenderDataData));

    addInstancedTests({&RendererTest::multiline},
        Containers::arraySize(MultilineData));

    #ifdef MAGNUM_TARGET_GL
    addTests({&RendererTest::arrayGlyphCache,
              &RendererTest::fontNotFoundInCache});
    #endif
}

struct TestShaper: AbstractShaper {
    using AbstractShaper::AbstractShaper;

    UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
        return text.size();
    }

    void doGlyphsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids, const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
        for(UnsignedInt i = 0; i != ids.size(); ++i) {
            /* It just rotates between the three glyphs */
            if(i % 3 == 0)
                ids[i] = 3;
            else if(i % 3 == 1)
                ids[i] = 7;
            else
                ids[i] = 9;

            /* Offset Y and advance X is getting larger with every glyph,
               advance Y is flipping its sign with every glyph */
            offsets[i] = Vector2::yAxis(i + 1);
            advances[i] = {Float(i + 1), i % 2 ? -0.5f : +0.5f};
        }
    }
};

struct TestFont: AbstractFont {
    FontFeatures doFeatures() const override { return {}; }

    bool doIsOpened() const override { return _opened; }
    void doClose() override { _opened = false; }

    Properties doOpenFile(Containers::StringView, Float size) override {
        _opened = true;
        /* Line height isn't used for anything here so can be arbitrary */
        return {size, 4.5f, -2.5f, 10000.0f, 10};
    }

    UnsignedInt doGlyphId(char32_t) override { return 0; }
    Vector2 doGlyphSize(UnsignedInt) override { return {}; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

    Containers::Pointer<AbstractShaper> doCreateShaper() override {
        return Containers::pointer<TestShaper>(*this);
    }

    bool _opened = false;
};

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

DummyGlyphCache testGlyphCache(AbstractFont& font) {
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top left and top right of the cache */
    cache.addGlyph(fontId, 3, {5, 10}, {{}, {20, 10}});
    cache.addGlyph(fontId, 7, {10, 5}, {{0, 10}, {10, 20}});
    cache.addGlyph(fontId, 9, {5, 5}, {{10, 10}, {20, 20}});

    return cache;
}

void RendererTest::renderData() {
    auto&& data = RenderDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache = testGlyphCache(font);

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    std::vector<Vector2> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<UnsignedInt> indices;
    Range2D bounds;
    std::tie(positions, textureCoordinates, indices, bounds) = AbstractRenderer::render(font, cache, 0.25f, "abc", data.alignment);

    /* Three glyphs, three quads -> 12 vertices, 18 indices */
    CORRADE_COMPARE(positions.size(), 12);
    CORRADE_COMPARE(textureCoordinates.size(), 12);
    CORRADE_COMPARE(indices.size(), 18);

    /* Vertex positions. Rectangles coming from the cache and offsets +
       advances from the layouter are scaled by 0.5. First glyph is moved by
       (scaled) 1 up and has advance of (scaled) {1, ±0.5}, every next glyph is
       moved up and further distanced by (scaled) {1, ±0.5}. First glyph is
       wide, the other two are square.

                   +-+
              +-+  |c|
        0---2 |b|  +-+
        | a | +-+
        1---3          */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        /* Cursor is {0, 0}. Offset from the cache is {5, 10}, offset from the
           renderer is {0, 1}, size is {20, 10}; all scaled by 0.5 */
        Vector2{ 2.5f, 10.5f} + data.offset,
        Vector2{ 2.5f,  5.5f} + data.offset,
        Vector2{12.5f, 10.5f} + data.offset,
        Vector2{12.5f,  5.5f} + data.offset,

        /* Advance was {1, 0.5}, cursor is {1, 0.5}. Offset from the cache is
           {10, 5}, offset from the renderer is {0, 2}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 5.5f, 8.75f} + data.offset,
        Vector2{ 5.5f, 3.75f} + data.offset,
        Vector2{10.5f, 8.75f} + data.offset,
        Vector2{10.5f, 3.75f} + data.offset,

        /* Advance was {2, -0.5}, cursor is {3, 0}. Offset from the cache is
           {5, 5}, offset from the renderer is {0, 3}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 4.0f,  9.0f} + data.offset,
        Vector2{ 4.0f,  4.0f} + data.offset,
        Vector2{ 9.0f,  9.0f} + data.offset,
        Vector2{ 9.0f,  4.0f} + data.offset
    }), TestSuite::Compare::Container);

    /* Bounds. Different depending on whether or not GlyphBounds alignment is
       used. */
    if(UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds)
        CORRADE_COMPARE(bounds, (Range2D{{2.5f, 3.75f}, {12.5f, 10.5f}}.translated(data.offset)));
    else
        CORRADE_COMPARE(bounds, (Range2D{{0.0f, -1.25f}, {3.0f, 2.25f}}.translated(data.offset)));

    /* Texture coordinates. First glyph is bottom, second top left, third top
       right.
       +-+-+
       |b|c|
       0---2
       | a |
       1---3 */
    CORRADE_COMPARE_AS(textureCoordinates, (std::vector<Vector2>{
        {0.0f, 0.5f},
        {0.0f, 0.0f},
        {1.0f, 0.5f},
        {1.0f, 0.0f},

        {0.0f, 1.0f},
        {0.0f, 0.5f},
        {0.5f, 1.0f},
        {0.5f, 0.5f},

        {0.5f, 1.0f},
        {0.5f, 0.5f},
        {1.0f, 1.0f},
        {1.0f, 0.5f}
    }), TestSuite::Compare::Container);

    /* Indices
       0---2 0---2 5
       |   | |  / /|
       |   | | / / |
       |   | |/ /  |
       1---3 1 3---4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
        0,  1,  2,  1,  3,  2,
        4,  5,  6,  5,  7,  6,
        8,  9, 10,  9, 11, 10
    }), TestSuite::Compare::Container);
}

void RendererTest::multiline() {
    auto&& data = MultilineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Shaper: AbstractShaper {
        using AbstractShaper::AbstractShaper;

        UnsignedInt doShape(Containers::StringView text, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>) override {
            return text.size();
        }

        void doGlyphsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids, const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i) {
                ids[i] = 0;
                offsets[i] = {};
                advances[i] = Vector2::xAxis(4.0f);
            }
        }
    };

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }
        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* Compared to the glyph bounds, which are from 0 to 2, this is
               shifted by one unit, thus by 0.5 in the output */
            return {size, 1.0f, -1.0f, 8.0f, 10};
        }

        UnsignedInt doGlyphId(char32_t) override { return 0; }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractShaper> doCreateShaper() override {
            return Containers::pointer<Shaper>(*this);
        }

        bool _opened = false;
    } font;
    font.openFile({}, 0.5f);

    /* Just a single glyph that scales to {1, 1} in the end */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    UnsignedInt fontId = cache.addFont(1, &font);
    cache.addGlyph(fontId, 0, {}, {{}, {2, 2}});

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    Range2D rectangle;
    std::vector<UnsignedInt> indices;
    std::vector<Vector2> positions, textureCoordinates;
    std::tie(positions, textureCoordinates, indices, rectangle) = Renderer2D::render(font,
        cache, 0.25f, "abcd\nef\n\nghi", data.alignment);

    /* We're rendering text at 0.25f size and the font is scaled to 0.5f, so
       the line advance should be 8.0f*0.25f/0.5f = 4.0f */
    CORRADE_COMPARE(font.size(), 0.5f);
    CORRADE_COMPARE(font.lineHeight(), 8.0f);

    /* Bounds. The advance for the rightmost glyph is one unit larger than the
       actual bounds so it's different on X between the two variants */
    if(UnsignedByte(data.alignment) & Implementation::AlignmentGlyphBounds)
        CORRADE_COMPARE(rectangle, Range2D({0.0f, -12.0f}, {7.0f, 1.0f}).translated(data.offset0));
    else
        CORRADE_COMPARE(rectangle, Range2D({0.0f, -12.5f}, {8.0f, 0.5f}).translated(data.offset0));

    /* Vertices
       [a] [b] [c] [d]
           [e] [f]

         [g] [h] [i]   */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        Vector2{0.0f, 1.0f} + data.offset0, /* a */
        Vector2{0.0f, 0.0f} + data.offset0,
        Vector2{1.0f, 1.0f} + data.offset0,
        Vector2{1.0f, 0.0f} + data.offset0,

        Vector2{2.0f, 1.0f} + data.offset0, /* b */
        Vector2{2.0f, 0.0f} + data.offset0,
        Vector2{3.0f, 1.0f} + data.offset0,
        Vector2{3.0f, 0.0f} + data.offset0,

        Vector2{4.0f, 1.0f} + data.offset0, /* c */
        Vector2{4.0f, 0.0f} + data.offset0,
        Vector2{5.0f, 1.0f} + data.offset0,
        Vector2{5.0f, 0.0f} + data.offset0,

        Vector2{6.0f, 1.0f} + data.offset0, /* d */
        Vector2{6.0f, 0.0f} + data.offset0,
        Vector2{7.0f, 1.0f} + data.offset0,
        Vector2{7.0f, 0.0f} + data.offset0,

        Vector2{0.0f, 1.0f} + data.offset1, /* e */
        Vector2{0.0f, 0.0f} + data.offset1,
        Vector2{1.0f, 1.0f} + data.offset1,
        Vector2{1.0f, 0.0f} + data.offset1,

        Vector2{2.0f, 1.0f} + data.offset1, /* f */
        Vector2{2.0f, 0.0f} + data.offset1,
        Vector2{3.0f, 1.0f} + data.offset1,
        Vector2{3.0f, 0.0f} + data.offset1,

        /* Two linebreaks here */

        Vector2{0.0f, 1.0f} + data.offset2, /* g */
        Vector2{0.0f, 0.0f} + data.offset2,
        Vector2{1.0f, 1.0f} + data.offset2,
        Vector2{1.0f, 0.0f} + data.offset2,

        Vector2{2.0f, 1.0f} + data.offset2, /* h */
        Vector2{2.0f, 0.0f} + data.offset2,
        Vector2{3.0f, 1.0f} + data.offset2,
        Vector2{3.0f, 0.0f} + data.offset2,

        Vector2{4.0f, 1.0f} + data.offset2, /* i */
        Vector2{4.0f, 0.0f} + data.offset2,
        Vector2{5.0f, 1.0f} + data.offset2,
        Vector2{5.0f, 0.0f} + data.offset2,
    }), TestSuite::Compare::Container);

    /* Indices
       0---2 0---2 5
       |   | |  / /|
       |   | | / / |
       |   | |/ /  |
       1---3 1 3---4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
         0,  1,  2,  1,  3,  2,
         4,  5,  6,  5,  7,  6,
         8,  9, 10,  9, 11, 10,
        12, 13, 14, 13, 15, 14,
        16, 17, 18, 17, 19, 18,
        20, 21, 22, 21, 23, 22,
        24, 25, 26, 25, 27, 26,
        28, 29, 30, 29, 31, 30,
        32, 33, 34, 33, 35, 34
    }), TestSuite::Compare::Container);
}

#ifdef MAGNUM_TARGET_GL
void RendererTest::arrayGlyphCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } cache{PixelFormat::R8Unorm, {100, 100, 3}};

    std::ostringstream out;
    Error redirectError{&out};
    AbstractRenderer::render(font, cache, 0.25f, "abc");
    CORRADE_COMPARE(out.str(), "Text::Renderer: array glyph caches are not supported\n");
}

void RendererTest::fontNotFoundInCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {100, 100}};

    cache.addFont(34);
    cache.addFont(25);

    std::ostringstream out;
    Error redirectError{&out};
    AbstractRenderer::render(font, cache, 0.25f, "abc");
    CORRADE_COMPARE(out.str(), "Text::Renderer: font not found among 2 fonts in passed glyph cache\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::RendererTest)
