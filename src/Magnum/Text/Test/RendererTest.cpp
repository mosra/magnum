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
#include <Corrade/Containers/StringStl.h> /** @todo drop once Debug is stream-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h> /** @todo drop once Debug is stream-free */

#include "Magnum/PixelFormat.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/Direction.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    void lineGlyphPositions();
    void lineGlyphPositionsAliasedViews();
    void lineGlyphPositionsInvalidViewSizes();
    void lineGlyphPositionsInvalidDirection();
    void lineGlyphPositionsNoFontOpened();

    void glyphQuads();
    void glyphQuadsAliasedViews();
    void glyphQuadsInvalidViewSizes();
    void glyphQuadsNoFontOpened();
    void glyphQuadsFontNotFoundInCache();

    void glyphQuads2D();
    void glyphQuads2DArrayGlyphCache();

    void alignLine();
    void alignLineInvalidDirection();

    void alignBlock();
    void alignBlockInvalidDirection();

    template<class T> void glyphQuadIndices();
    void glyphQuadIndicesTypeTooSmall();

    void renderData();

    void multiline();

    #ifdef MAGNUM_TARGET_GL
    void arrayGlyphCache();
    void fontNotFoundInCache();
    #endif
};

const struct {
    const char* name;
    bool globalIds;
} GlyphQuadsData[]{
    {"font-specific glyph IDs", false},
    {"cache-global glyph IDs", true}
};

const struct {
    const char* name;
    Alignment alignment;
    Float offset;
} AlignLineData[]{
    /* The vertical alignment and GlyphBounds has no effect here */
    /* Left is the default (0) value, thus should result in no shift */
    {"left", Alignment::BottomLeft, -10.0f},
    {"right", Alignment::LineRightGlyphBounds, -13.5f},
    /* Integral should be handled only for Center */
    {"right, integral", Alignment::MiddleRightGlyphBoundsIntegral, -13.5f},
    {"center", Alignment::TopCenter, -11.75f},
    {"center, integral", Alignment::TopCenterIntegral, -12.0f},
};

const struct {
    const char* name;
    Alignment alignment;
    Float offset;
} AlignBlockData[]{
    /* The horizontal alignment and GlyphBounds has no effect here */
    /* Line is the default (0) value, thus should result in no shift */
    {"line", Alignment::LineCenterGlyphBounds, 0.0f},
    {"bottom", Alignment::BottomRight, -9.5f},
    {"top", Alignment::TopLeftGlyphBounds, -19.5f},
    /* Integral should be handled only for Middle */
    {"top, integral", Alignment::TopCenterGlyphBoundsIntegral, -19.5f},
    {"middle", Alignment::MiddleLeft, -14.5f},
    {"middle, integral", Alignment::MiddleLeftIntegral, -15.0f}
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
    addTests({&RendererTest::lineGlyphPositions,
              &RendererTest::lineGlyphPositionsAliasedViews,
              &RendererTest::lineGlyphPositionsInvalidViewSizes,
              &RendererTest::lineGlyphPositionsInvalidDirection,
              &RendererTest::lineGlyphPositionsNoFontOpened});

    addInstancedTests({&RendererTest::glyphQuads,
                       &RendererTest::glyphQuadsAliasedViews},
        Containers::arraySize(GlyphQuadsData));

    addTests({&RendererTest::glyphQuadsInvalidViewSizes,
              &RendererTest::glyphQuadsNoFontOpened,
              &RendererTest::glyphQuadsFontNotFoundInCache});

    addInstancedTests({&RendererTest::glyphQuads2D},
        Containers::arraySize(GlyphQuadsData));

    addTests({&RendererTest::glyphQuads2DArrayGlyphCache});

    addInstancedTests({&RendererTest::alignLine},
        Containers::arraySize(AlignLineData));

    addTests({&RendererTest::alignLineInvalidDirection});

    addInstancedTests({&RendererTest::alignBlock},
        Containers::arraySize(AlignBlockData));

    addTests({&RendererTest::alignBlockInvalidDirection,

              &RendererTest::glyphQuadIndices<UnsignedInt>,
              &RendererTest::glyphQuadIndices<UnsignedShort>,
              &RendererTest::glyphQuadIndices<UnsignedByte>,
              &RendererTest::glyphQuadIndicesTypeTooSmall});

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

    void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
        for(UnsignedInt i = 0; i != ids.size(); ++i) {
            /* It just rotates between the three glyphs */
            if(i % 3 == 0)
                ids[i] = 3;
            else if(i % 3 == 1)
                ids[i] = 7;
            else
                ids[i] = 9;
        }
    }
    void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
        for(UnsignedInt i = 0; i != offsets.size(); ++i) {
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

    void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
        for(UnsignedInt& i: glyphs)
            i = 0;
    }
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
    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}, {}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top right and top left of the cache.
       Adding them in a shuffled order to verify non-trivial font-specific to
       cache-global glyph mapping in glyphQuads() below. */
    cache.addGlyph(fontId, 3, {5, 10}, {{}, {20, 10}});
    cache.addGlyph(fontId, 9, {5, 5}, {{10, 10}, {20, 20}});
    cache.addGlyph(fontId, 7, {10, 5}, {{0, 10}, {10, 20}});

    return cache;
}

DummyGlyphCache testGlyphCacheArray(AbstractFont& font) {
    /* Default padding is 1 to avoid artifacts, set that to 0 to simplify */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20, 3}, {}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top right and top left of the cache.
       Adding them in a shuffled order to verify non-trivial font-specific to
       cache-global glyph mapping in glyphQuads() below. */
    cache.addGlyph(fontId, 3, {5, 10}, 2, {{}, {20, 10}});
    cache.addGlyph(fontId, 9, {5, 5}, 1, {{10, 10}, {20, 20}});
    cache.addGlyph(fontId, 7, {10, 5}, 0, {{0, 10}, {10, 20}});

    return cache;
}

void RendererTest::lineGlyphPositions() {
    TestFont font;
    font.openFile({}, 2.5f);

    Vector2 glyphOffsets[]{
        {0.2f, -0.4f},
        {0.4f, 0.8f},
        {-0.2f, 0.4f},
    };
    Vector2 glyphAdvances[]{
        {1.0f, 0.0f},
        {2.0f, 0.2f},
        {3.0f, -0.2f}
    };
    Vector2 cursor{100.0f, 200.0f};

    /* The font is opened at 2.5, rendering at 1.25, so everything will be
       scaled by 0.5 */
    Vector2 glyphPositions[3];
    Range2D rectangle = renderLineGlyphPositionsInto(font, 1.25f, LayoutDirection::HorizontalTopToBottom, glyphOffsets, glyphAdvances, cursor, glyphPositions);
    /* The rectangle contains the cursor range and descent to ascent */
    CORRADE_COMPARE(rectangle, (Range2D{{100.0f, 198.75f}, {103.0f, 202.25}}));
    CORRADE_COMPARE(cursor, (Vector2{103.0f, 200.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(glyphPositions), Containers::arrayView<Vector2>({
        {100.1f, 199.8f},
        {100.7f, 200.4f},
        {101.4f, 200.3f}
    }), TestSuite::Compare::Container);
}

void RendererTest::lineGlyphPositionsAliasedViews() {
    /* Like lineGlyphPositions(), but with the input data stored in the output
       array. The internals should be written in a way that doesn't overwrite
       the input before it's read. */
    TestFont font;
    font.openFile({}, 2.5f);

    Vector2 glyphOffsetsPositions[]{
        {0.2f, -0.4f},
        {0.4f, 0.8f},
        {-0.2f, 0.4f},
    };
    Vector2 glyphAdvances[]{
        {1.0f, 0.0f},
        {2.0f, 0.2f},
        {3.0f, -0.2f}
    };
    Vector2 cursor{100.0f, 200.0f};

    Range2D rectangle = renderLineGlyphPositionsInto(font, 1.25f, LayoutDirection::HorizontalTopToBottom, glyphOffsetsPositions, glyphAdvances, cursor, glyphOffsetsPositions);
    CORRADE_COMPARE(rectangle, (Range2D{{100.0f, 198.75f}, {103.0f, 202.25}}));
    CORRADE_COMPARE(cursor, (Vector2{103.0f, 200.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(glyphOffsetsPositions), Containers::arrayView<Vector2>({
        {100.1f, 199.8f},
        {100.7f, 200.4f},
        {101.4f, 200.3f}
    }), TestSuite::Compare::Container);
}

void RendererTest::lineGlyphPositionsInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 data[5];
    Vector2 dataInvalid[4];
    Vector2 cursor;

    std::ostringstream out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, data, data, cursor, dataInvalid);
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, data, dataInvalid, cursor, data);
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, dataInvalid, data, cursor, data);
    CORRADE_COMPARE(out.str(),
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 5, 5 and 4\n"
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 5, 4 and 5\n"
        "Text::renderLineGlyphPositionsInto(): expected glyphOffsets, glyphAdvances and output views to have the same size, got 4, 5 and 5\n");
}

void RendererTest::lineGlyphPositionsInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 cursor;

    std::ostringstream out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::VerticalLeftToRight, {}, {}, cursor, {});
    CORRADE_COMPARE(out.str(), "Text::renderLineGlyphPositionsInto(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalLeftToRight\n");
}

void RendererTest::lineGlyphPositionsNoFontOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    Vector2 cursor;

    std::ostringstream out;
    Error redirectError{&out};
    renderLineGlyphPositionsInto(font, 10.0f, LayoutDirection::HorizontalTopToBottom, {}, {}, cursor, {});
    CORRADE_COMPARE(out.str(), "Text::renderLineGlyphPositionsInto(): no font opened\n");
}

void RendererTest::glyphQuads() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCacheArray(font);

    Vector2 glyphPositions[]{
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    };
    UnsignedInt fontGlyphIds[]{
        3, 7, 9
    };
    UnsignedInt glyphIds[]{
        /* Glyph 0 is the cache-global invalid glyph */
        1, 3, 2
    };

    Vector2 positions[3*4];
    Vector3 textureCoordinates[3*4];
    /* The font is opened at 2.5, rendering at 1.25, so everything will be
       scaled by 0.5 */
    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, fontGlyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    /* 2---3
       |   |
       0---1 */
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f}, /* Offset {5, 10}, size {20, 10}, scaled by 0.5 */
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f}, /* Offset {10, 5}, size {10, 10}, scaled by 0.5 */
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f}, /* Offset {5, 5}, size {10, 10}, scaled by 0.5 */
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    /* First glyph is bottom, second top left, third top right; layer is
       different for each.

       +-+-+
       |b|c|
       2---3
       | a |
       0---1 */
    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 2.0f},
        {1.0f, 0.0f, 2.0f},
        {0.0f, 0.5f, 2.0f},
        {1.0f, 0.5f, 2.0f},

        {0.0f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.5f, 1.0f, 0.0f},

        {0.5f, 0.5f, 1.0f},
        {1.0f, 0.5f, 1.0f},
        {0.5f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadsAliasedViews() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like lineGlyphPositions(), but with the input data stored in the output
       array. The internals should be written in a way that doesn't overwrite
       the input before it's read. */

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCacheArray(font);

    Vector2 positions[3*4];
    Vector3 textureCoordinates[3*4];

    Containers::StridedArrayView1D<Vector2> glyphPositions = Containers::stridedArrayView(positions).every(4);
    Utility::copy({
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    }, glyphPositions);

    Containers::StridedArrayView1D<UnsignedInt> glyphIds = Containers::arrayCast<UnsignedInt>(Containers::stridedArrayView(textureCoordinates).every(4));
    data.globalIds ?
        Utility::copy({1, 3, 2}, glyphIds) :
        Utility::copy({3, 7, 9}, glyphIds);

    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, glyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f},
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f},
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f},
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 2.0f},
        {1.0f, 0.0f, 2.0f},
        {0.0f, 0.5f, 2.0f},
        {1.0f, 0.5f, 2.0f},

        {0.0f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.5f, 1.0f, 0.0f},

        {0.5f, 0.5f, 1.0f},
        {1.0f, 0.5f, 1.0f},
        {0.5f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadsInvalidViewSizes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 5.0f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    cache.addFont(96, &font);
    Vector2 glyphPositions[4];
    Vector2 glyphPositionsInvalid[5];
    UnsignedInt glyphIds[4]{};
    UnsignedInt glyphIdsInvalid[3];
    Vector2 positions[16];
    Vector2 positionsInvalid[15];
    Vector3 textureCoordinates[16];
    Vector3 textureCoordinatesInvalid[17];

    std::ostringstream out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIdsInvalid, positions, textureCoordinates);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositions, glyphIdsInvalid, positions, textureCoordinates);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositionsInvalid, glyphIds, positions, textureCoordinates);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositionsInvalid, glyphIds, positions, textureCoordinates);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIds, positions, textureCoordinatesInvalid);
    renderGlyphQuadsInto(cache, 2.0f, glyphPositions, glyphIds, positions, textureCoordinatesInvalid);
    renderGlyphQuadsInto(font, 10.0f, cache, glyphPositions, glyphIds, positionsInvalid, textureCoordinates);
    renderGlyphQuadsInto(cache, 10.0f, glyphPositions, glyphIds, positionsInvalid, textureCoordinates);
    CORRADE_COMPARE_AS(out.str(),
        "Text::renderGlyphQuadsInto(): expected fontGlyphIds and glyphPositions views to have the same size, got 3 and 4\n"
        "Text::renderGlyphQuadsInto(): expected glyphIds and glyphPositions views to have the same size, got 3 and 4\n"
        "Text::renderGlyphQuadsInto(): expected fontGlyphIds and glyphPositions views to have the same size, got 4 and 5\n"
        "Text::renderGlyphQuadsInto(): expected glyphIds and glyphPositions views to have the same size, got 4 and 5\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 16 and 17\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 16 and 17\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 15 and 16\n"
        "Text::renderGlyphQuadsInto(): expected vertexPositions and vertexTextureCoordinates views to have 16 elements, got 15 and 16\n",
        TestSuite::Compare::String);
}

void RendererTest::glyphQuadsNoFontOpened() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};

    std::ostringstream out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector3>{});
    CORRADE_COMPARE(out.str(), "Text::renderGlyphQuadsInto(): no font opened\n");
}

void RendererTest::glyphQuadsFontNotFoundInCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    font.openFile({}, 0.5f);
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}};
    cache.addFont(56);
    cache.addFont(13);

    std::ostringstream out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector3>{});
    CORRADE_COMPARE(out.str(), "Text::renderGlyphQuadsInto(): font not found among 2 fonts in passed glyph cache\n");
}

void RendererTest::glyphQuads2D() {
    auto&& data = GlyphQuadsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Like lineGlyphPositions(), but with just a 2D glyph cache and using the
       three-component overload. */

    TestFont font;
    font.openFile({}, 2.5f);
    DummyGlyphCache cache = testGlyphCache(font);

    Vector2 glyphPositions[]{
        {100.0f, 200.0f},
        {103.0f, 202.0f},
        {107.0f, 196.0f}
    };
    UnsignedInt fontGlyphIds[]{
        3, 7, 9
    };
    UnsignedInt glyphIds[]{
        1, 3, 2
    };

    Vector2 positions[3*4];
    Vector2 textureCoordinates[3*4];
    Range2D rectangle = data.globalIds ?
        renderGlyphQuadsInto(cache, 1.25f/2.5f, glyphPositions, glyphIds, positions, textureCoordinates) :
        renderGlyphQuadsInto(font, 1.25f, cache, glyphPositions, fontGlyphIds, positions, textureCoordinates);
    CORRADE_COMPARE(rectangle, (Range2D{{102.5f, 198.5f}, {114.5f, 210.0f}}));

    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {102.5f, 205.0f},
        {112.5f, 205.0f},
        {102.5f, 210.0f},
        {112.5f, 210.0f},

        {108.0f, 204.5f},
        {113.0f, 204.5f},
        {108.0f, 209.5f},
        {113.0f, 209.5f},

        {109.5f, 198.5f},
        {114.5f, 198.5f},
        {109.5f, 203.5f},
        {114.5f, 203.5f},
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(Containers::arrayView(textureCoordinates), Containers::arrayView<Vector2>({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.5f},
        {1.0f, 0.5f},

        {0.0f, 0.5f},
        {0.5f, 0.5f},
        {0.0f, 1.0f},
        {0.5f, 1.0f},

        {0.5f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {1.0f, 1.0f},
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuads2DArrayGlyphCache() {
    CORRADE_SKIP_IF_NO_ASSERT();

    TestFont font;
    struct: AbstractGlyphCache {
        using AbstractGlyphCache::AbstractGlyphCache;

        GlyphCacheFeatures doFeatures() const override { return {}; }
    } cache{PixelFormat::R8Unorm, {20, 20, 2}};

    std::ostringstream out;
    Error redirectError{&out};
    renderGlyphQuadsInto(font, 10.0f, cache, nullptr, nullptr, nullptr, Containers::StridedArrayView1D<Vector2>{});
    CORRADE_COMPARE(out.str(), "Text::renderGlyphQuadsInto(): can't use this overload with an array glyph cache\n");
}

void RendererTest::alignLine() {
    auto&& data = AlignLineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Range2D rectangle{{10.0f, 200.0f}, {13.5f, -960.0f}};

    /* The positions aren't taken into account, so they can be arbitrary */
    Vector2 positions[]{
        {100.0f, 200.0f},
        {300.0f, -60.0f},
        {-10.0f, 100.0f},
    };
    Range2D alignedRectangle = alignRenderedLine(rectangle, LayoutDirection::HorizontalTopToBottom, data.alignment, positions);
    CORRADE_COMPARE(alignedRectangle, rectangle.translated({data.offset, 0.0f}));
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {100.0f + data.offset, 200.0f},
        {300.0f + data.offset, -60.0f},
        {-10.0f + data.offset, 100.0f}
    }), TestSuite::Compare::Container);
}

void RendererTest::alignLineInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    alignRenderedLine({}, LayoutDirection::VerticalRightToLeft, Alignment::LineLeft, nullptr);
    CORRADE_COMPARE(out.str(), "Text::alignRenderedLine(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalRightToLeft\n");
}

void RendererTest::alignBlock() {
    auto&& data = AlignBlockData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Range2D rectangle{{100.0f, 9.5f}, {-70.0f, 19.5f}};

    /* The positions aren't taken into account, so they can be arbitrary */
    Vector2 positions[]{
        {100.0f, 200.0f},
        {-10.0f, 100.0f},
        {300.0f, -60.0f},
    };
    Range2D alignedRectangle = alignRenderedBlock(rectangle, LayoutDirection::HorizontalTopToBottom, data.alignment, positions);
    CORRADE_COMPARE(alignedRectangle, rectangle.translated({0.0f, data.offset}));
    CORRADE_COMPARE_AS(Containers::arrayView(positions), Containers::arrayView<Vector2>({
        {100.0f, 200.0f + data.offset},
        {-10.0f, 100.0f + data.offset},
        {300.0f, -60.0f + data.offset},
    }), TestSuite::Compare::Container);
}

void RendererTest::alignBlockInvalidDirection() {
    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};
    alignRenderedBlock({}, LayoutDirection::VerticalRightToLeft, Alignment::LineLeft, nullptr);
    CORRADE_COMPARE(out.str(), "Text::alignRenderedBlock(): only Text::LayoutDirection::HorizontalTopToBottom is supported right now, got Text::LayoutDirection::VerticalRightToLeft\n");
}

template<class T> void RendererTest::glyphQuadIndices() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* 2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    T indices[3*6];
    renderGlyphQuadIndicesInto(60, indices);
    CORRADE_COMPARE_AS(Containers::arrayView(indices), Containers::arrayView<T>({
        240, 241, 242, 242, 241, 243,
        244, 245, 246, 246, 245, 247,
        248, 249, 250, 250, 249, 251
    }), TestSuite::Compare::Container);
}

void RendererTest::glyphQuadIndicesTypeTooSmall() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This should be fine */
    UnsignedByte indices8[18];
    UnsignedShort indices16[18];
    UnsignedInt indices32[18];
    renderGlyphQuadIndicesInto(256/4 - 3, indices8);
    renderGlyphQuadIndicesInto(65536/4 - 3, indices16);
    renderGlyphQuadIndicesInto(4294967296u/4 - 3, indices32);
    CORRADE_COMPARE(indices8[17], 255);
    CORRADE_COMPARE(indices16[17], 65535);
    CORRADE_COMPARE(indices32[17], 4294967295);

    /* Empty view also */
    renderGlyphQuadIndicesInto(256/4, Containers::ArrayView<UnsignedByte>{});
    renderGlyphQuadIndicesInto(65536/4, Containers::ArrayView<UnsignedShort>{});
    renderGlyphQuadIndicesInto(4294967296u/4, Containers::ArrayView<UnsignedInt>{});

    std::ostringstream out;
    Error redirectError{&out};
    renderGlyphQuadIndicesInto(256/4 - 3 + 1, indices8);
    renderGlyphQuadIndicesInto(65536/4 - 3 + 1, indices16);
    renderGlyphQuadIndicesInto(4294967296u/4 - 3 + 1, indices32);
    /* Should assert even if there's actually no indices to write */
    renderGlyphQuadIndicesInto(256/4 + 1, Containers::ArrayView<UnsignedByte>{});
    renderGlyphQuadIndicesInto(65536/4 + 1, Containers::ArrayView<UnsignedShort>{});
    renderGlyphQuadIndicesInto(4294967296u/4 + 1, Containers::ArrayView<UnsignedInt>{});
    CORRADE_COMPARE(out.str(),
        "Text::renderGlyphQuadIndicesInto(): max index value of 259 cannot fit into a 8-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 65539 cannot fit into a 16-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 4294967299 cannot fit into a 32-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 259 cannot fit into a 8-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 65539 cannot fit into a 16-bit type\n"
        "Text::renderGlyphQuadIndicesInto(): max index value of 4294967299 cannot fit into a 32-bit type\n");
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
        2---3 |b|  +-+
        | a | +-+
        0---1          */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        /* Cursor is {0, 0}. Offset from the cache is {5, 10}, offset from the
           renderer is {0, 1}, size is {20, 10}; all scaled by 0.5 */
        Vector2{ 2.5f,  5.5f} + data.offset,
        Vector2{12.5f,  5.5f} + data.offset,
        Vector2{ 2.5f, 10.5f} + data.offset,
        Vector2{12.5f, 10.5f} + data.offset,

        /* Advance was {1, 0.5}, cursor is {1, 0.5}. Offset from the cache is
           {10, 5}, offset from the renderer is {0, 2}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 5.5f, 3.75f} + data.offset,
        Vector2{10.5f, 3.75f} + data.offset,
        Vector2{ 5.5f, 8.75f} + data.offset,
        Vector2{10.5f, 8.75f} + data.offset,

        /* Advance was {2, -0.5}, cursor is {3, 0}. Offset from the cache is
           {5, 5}, offset from the renderer is {0, 3}, size is {10, 10}; all
           scaled by 0.5 */
        Vector2{ 4.0f,  4.0f} + data.offset,
        Vector2{ 9.0f,  4.0f} + data.offset,
        Vector2{ 4.0f,  9.0f} + data.offset,
        Vector2{ 9.0f,  9.0f} + data.offset,
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
       2---3
       | a |
       0---1 */
    CORRADE_COMPARE_AS(textureCoordinates, (std::vector<Vector2>{
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.5f},
        {1.0f, 0.5f},

        {0.0f, 0.5f},
        {0.5f, 0.5f},
        {0.0f, 1.0f},
        {0.5f, 1.0f},

        {0.5f, 0.5f},
        {1.0f, 0.5f},
        {0.5f, 1.0f},
        {1.0f, 1.0f},
    }), TestSuite::Compare::Container);

    /* Indices
       2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
        0,  1,  2,  2,  1,  3,
        4,  5,  6,  6,  5,  7,
        8,  9, 10, 10,  9, 11,
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

        void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const override {
            for(UnsignedInt i = 0; i != ids.size(); ++i) {
                ids[i] = 0;
            }
        }
        void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const override {
            for(UnsignedInt i = 0; i != offsets.size(); ++i) {
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

        void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>&, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) override {
            for(UnsignedInt& i: glyphs)
                i = 0;
        }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractShaper> doCreateShaper() override {
            return Containers::pointer<Shaper>(*this);
        }

        bool _opened = false;
    } font;
    font.openFile({}, 0.5f);

    /* Just a single glyph that scales to {1, 1} in the end. Default padding is
       1 which would prevent this, set it back to 0. */
    DummyGlyphCache cache{PixelFormat::R8Unorm, {20, 20}, {}};
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
        Vector2{0.0f, 0.0f} + data.offset0, /* a */
        Vector2{1.0f, 0.0f} + data.offset0,
        Vector2{0.0f, 1.0f} + data.offset0,
        Vector2{1.0f, 1.0f} + data.offset0,

        Vector2{2.0f, 0.0f} + data.offset0, /* b */
        Vector2{3.0f, 0.0f} + data.offset0,
        Vector2{2.0f, 1.0f} + data.offset0,
        Vector2{3.0f, 1.0f} + data.offset0,

        Vector2{4.0f, 0.0f} + data.offset0, /* c */
        Vector2{5.0f, 0.0f} + data.offset0,
        Vector2{4.0f, 1.0f} + data.offset0,
        Vector2{5.0f, 1.0f} + data.offset0,

        Vector2{6.0f, 0.0f} + data.offset0, /* d */
        Vector2{7.0f, 0.0f} + data.offset0,
        Vector2{6.0f, 1.0f} + data.offset0,
        Vector2{7.0f, 1.0f} + data.offset0,

        Vector2{0.0f, 0.0f} + data.offset1, /* e */
        Vector2{1.0f, 0.0f} + data.offset1,
        Vector2{0.0f, 1.0f} + data.offset1,
        Vector2{1.0f, 1.0f} + data.offset1,

        Vector2{2.0f, 0.0f} + data.offset1, /* f */
        Vector2{3.0f, 0.0f} + data.offset1,
        Vector2{2.0f, 1.0f} + data.offset1,
        Vector2{3.0f, 1.0f} + data.offset1,

        /* Two linebreaks here */

        Vector2{0.0f, 0.0f} + data.offset2, /* g */
        Vector2{1.0f, 0.0f} + data.offset2,
        Vector2{0.0f, 1.0f} + data.offset2,
        Vector2{1.0f, 1.0f} + data.offset2,

        Vector2{2.0f, 0.0f} + data.offset2, /* h */
        Vector2{3.0f, 0.0f} + data.offset2,
        Vector2{2.0f, 1.0f} + data.offset2,
        Vector2{3.0f, 1.0f} + data.offset2,

        Vector2{4.0f, 0.0f} + data.offset2, /* i */
        Vector2{5.0f, 0.0f} + data.offset2,
        Vector2{4.0f, 1.0f} + data.offset2,
        Vector2{5.0f, 1.0f} + data.offset2,
    }), TestSuite::Compare::Container);

    /* Indices
       2---3 2 3---5
       |   | |\ \  |
       |   | | \ \ |
       |   | |  \ \|
       0---1 0---1 4 */
    CORRADE_COMPARE_AS(indices, (std::vector<UnsignedInt>{
         0,  1,  2,  2,  1,  3,
         4,  5,  6,  6,  5,  7,
         8,  9, 10, 10,  9, 11,
        12, 13, 14, 14, 13, 15,
        16, 17, 18, 18, 17, 19,
        20, 21, 22, 22, 21, 23,
        24, 25, 26, 26, 25, 27,
        28, 29, 30, 30, 29, 31,
        32, 33, 34, 34, 33, 35,
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
