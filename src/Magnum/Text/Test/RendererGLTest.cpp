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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/GlyphCache.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct RendererGLTest: GL::OpenGLTester {
    explicit RendererGLTest();

    void renderData();
    void renderMesh();
    void renderMeshIndexType();
    void mutableText();

    void multiline();
};

const struct {
    const char* name;
    Alignment alignment;
    Vector2 offset;
} RenderDataData[]{
    /* Not testing all combinations, just making sure that each horizontal,
       vertical and integer variant is covered */
    {"line left", Alignment::LineLeft,
        {}},
    /** @todo for all these, the initial glyph offset is first subtracted and
        only then the shift by either half or full size is performed, does that
        make sense? why is not done for the LineLeft case, then? */
    {"top center, integral", Alignment::TopCenterIntegral,
        /* The Y shift is rounded to whole units */
        {-5.5f - 2.5f, -7.5f - 3.5f}},
    {"top left", Alignment::TopLeft,
        {0.0f, -7.0f - 3.5f}},
    {"top right", Alignment::TopRight,
        {-10.0f - 2.5f, -7.0f - 3.5f}},
    {"middle center", Alignment::MiddleCenter,
        /* Half size of the bounds quad */
        {-5.0f - 2.5f, -3.5f - 3.625f}},
    {"middle center, integral", Alignment::MiddleCenterIntegral,
        /* The X shift is rounded to whole units */
        {-5.5f - 2.5f, -3.5f - 3.5f}},
};

RendererGLTest::RendererGLTest() {
    addInstancedTests({&RendererGLTest::renderData},
        Containers::arraySize(RenderDataData));

    addTests({&RendererGLTest::renderMesh,
              &RendererGLTest::renderMeshIndexType,
              &RendererGLTest::mutableText,

              &RendererGLTest::multiline});
}

struct TestLayouter: AbstractLayouter {
    explicit TestLayouter(const AbstractGlyphCache& cache, UnsignedInt fontId, Float scale, UnsignedInt glyphCount): AbstractLayouter{glyphCount}, _cache(cache), _fontId{fontId}, _scale{scale} {}

    Containers::Triple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt i) override {
        /* It just rotates between the three glyphs */
        UnsignedInt glyphId;
        if(i % 3 == 0)
            glyphId = 3;
        else if(i % 3 == 1)
            glyphId = 7;
        else
            glyphId = 9;

        Containers::Triple<Vector2i, Int, Range2Di> glyph = _cache.glyph(_fontId, glyphId);
        CORRADE_VERIFY(glyph.second() == 0);

        /* Offset Y and advance X is getting larger with every glyph */
        return {
            Range2D::fromSize(Vector2{glyph.first() + Vector2i::yAxis(i + 1)}*_scale, Vector2{glyph.third().size()}*_scale),
            Range2D{glyph.third()}.scaled(1.0f/Vector2{_cache.size().xy()}),
            Vector2{Float(i + 1), i % 2 ? -0.5f : +0.5f}*_scale
        };
    }

    const AbstractGlyphCache& _cache;
    UnsignedInt _fontId;
    Float _scale;
};

struct TestFont: AbstractFont {
    FontFeatures doFeatures() const override { return {}; }

    bool doIsOpened() const override { return _opened; }
    void doClose() override { _opened = false; }

    Properties doOpenFile(Containers::StringView, Float size) override {
        _opened = true;
        return {size, 0.45f, -0.25f, 0.75f, 10};
    }

    UnsignedInt doGlyphId(char32_t) override { return 0; }
    Vector2 doGlyphSize(UnsignedInt) override { return {}; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

    Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache& cache, Float size, Containers::StringView text) override {
        /* The final rendered size should be a ratio of the font and layout
           size */
        return Containers::pointer<TestLayouter>(cache, *cache.findFont(this), size/this->size(), UnsignedInt(text.size()));
    }

    bool _opened = false;
};

GlyphCache testGlyphCache(AbstractFont& font) {
    GlyphCache cache{{20, 20}};

    /* Add one more font to verify the right one gets picked */
    cache.addFont(96);
    UnsignedInt fontId = cache.addFont(font.glyphCount(), &font);

    /* Three glyphs, covering bottom, top left and top right of the cache */
    cache.addGlyph(fontId, 3, {5, 10}, {{}, {20, 10}});
    cache.addGlyph(fontId, 7, {10, 5}, {{0, 10}, {10, 20}});
    cache.addGlyph(fontId, 9, {5, 5}, {{10, 10}, {20, 20}});

    return cache;
}

void RendererGLTest::renderData() {
    auto&& data = RenderDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    TestFont font;
    font.openFile({}, 0.5f);
    GlyphCache cache = testGlyphCache(font);

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

    /* Bounds */
    CORRADE_COMPARE(bounds, (Range2D{{2.5f, 3.75f}, {12.5f, 10.5f}}.translated(data.offset)));

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

void RendererGLTest::renderMesh() {
    /* Like render(middle center), but with a mesh output instead of data */

    TestFont font;
    font.openFile({}, 0.5f);
    GlyphCache cache = testGlyphCache(font);

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    GL::Mesh mesh{NoCreate};
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array},
        indexBuffer{GL::Buffer::TargetHint::ElementArray};
    Range2D bounds;
    std::tie(mesh, bounds) = Renderer3D::render(font, cache,
        0.25f, "abc", vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw, Alignment::MiddleCenter);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Alignment offset */
    /** @todo same as in render(), figure out if the initial offset makes
        sense or not */
    const Vector2 offset{-5.0f - 2.5f, -3.5f - 3.625f};

    /* Bounds */
    CORRADE_COMPARE(bounds, (Range2D{{2.5f, 3.75f}, {12.5f, 10.5f}}.translated(offset)));

    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    /* Vertex buffer contents */
    Containers::Array<char> vertices = vertexBuffer.data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(vertices), Containers::arrayView<Vector2>({
        Vector2{ 2.5f, 10.5f} + offset, {0.0f, 0.5f},
        Vector2{ 2.5f,  5.5f} + offset, {0.0f, 0.0f},
        Vector2{12.5f, 10.5f} + offset, {1.0f, 0.5f},
        Vector2{12.5f,  5.5f} + offset, {1.0f, 0.0f},

        Vector2{ 5.5f, 8.75f} + offset, {0.0f, 1.0f},
        Vector2{ 5.5f, 3.75f} + offset, {0.0f, 0.5f},
        Vector2{10.5f, 8.75f} + offset, {0.5f, 1.0f},
        Vector2{10.5f, 3.75f} + offset, {0.5f, 0.5f},

        Vector2{ 4.0f,  9.0f} + offset, {0.5f, 1.0f},
        Vector2{ 4.0f,  4.0f} + offset, {0.5f, 0.5f},
        Vector2{ 9.0f,  9.0f} + offset, {1.0f, 1.0f},
        Vector2{ 9.0f,  4.0f} + offset, {1.0f, 0.5f},
    }), TestSuite::Compare::Container);

    Containers::Array<char> indices = indexBuffer.data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indices),
        Containers::arrayView<UnsignedByte>({
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }), TestSuite::Compare::Container);
    #endif
}

void RendererGLTest::renderMeshIndexType() {
    #ifndef MAGNUM_TARGET_GLES
    TestFont font;
    font.openFile({}, 0.5f);
    GlyphCache cache = testGlyphCache(font);

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    GL::Mesh mesh{NoCreate};
    GL::Buffer vertexBuffer, indexBuffer;

    /* Sizes: four vertices per glyph, each vertex has 2D position and 2D
       texture coordinates, each float is four bytes; six indices per glyph. */

    /* 8-bit indices (exactly 256 vertices) */
    std::tie(mesh, std::ignore) = Renderer3D::render(font, cache,
        1.0f, std::string(64, 'a'), vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    Containers::Array<char> indicesByte = indexBuffer.data();
    CORRADE_COMPARE(vertexBuffer.size(), 256*(2 + 2)*4);
    CORRADE_COMPARE(indicesByte.size(), 64*6);
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indicesByte).prefix(18),
        Containers::arrayView<UnsignedByte>({
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }), TestSuite::Compare::Container);

    /* 16-bit indices (260 vertices) */
    std::tie(mesh, std::ignore) = Renderer3D::render(font, cache,
        1.0f, std::string(65, 'a'), vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    Containers::Array<char> indicesShort = indexBuffer.data();
    CORRADE_COMPARE(vertexBuffer.size(), 260*(2 + 2)*4);
    CORRADE_COMPARE(indicesShort.size(), 65*6*2);
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(indicesShort).prefix(18),
        Containers::arrayView<UnsignedShort>({
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }), TestSuite::Compare::Container);
    #else
    CORRADE_SKIP("Can't verify buffer contents on OpenGL ES.");
    #endif
}

void RendererGLTest::mutableText() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::ARB::map_buffer_range::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::OES::mapbuffer>())
        CORRADE_SKIP("No required extension is supported");
    #endif

    /* Like render(middle center) and renderMesh(), but modifying an instance
       instead of rendering once */

    TestFont font;
    font.openFile({}, 0.5f);
    GlyphCache cache = testGlyphCache(font);
    Renderer2D renderer(font, cache, 0.25f, Alignment::MiddleCenter);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 0);
    CORRADE_COMPARE(renderer.fontSize(), 0.25f);
    CORRADE_COMPARE(renderer.rectangle(), Range2D());

    /* Reserve some capacity */
    renderer.reserve(4, GL::BufferUsage::DynamicDraw, GL::BufferUsage::DynamicDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 4);
    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> indices = renderer.indexBuffer().data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indices).prefix(24),
        Containers::arrayView<UnsignedByte>({
             0,  1,  2,  1,  3,  2,
             4,  5,  6,  5,  7,  6,
             8,  9, 10,  9, 11, 10,
            12, 13, 14, 13, 15, 14
        }), TestSuite::Compare::Container);
    #endif

    /* Render text */
    renderer.render("abc");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Alignment offset */
    /** @todo same as in render(), figure out if the initial offset makes
        sense or not */
    const Vector2 offset{-5.0f - 2.5f, -3.5f - 3.625f};

    /* Updated bounds and mesh vertex count */
    CORRADE_COMPARE(renderer.rectangle(), (Range2D{{2.5f, 3.75f}, {12.5f, 10.5f} }.translated(offset)));
    CORRADE_COMPARE(renderer.mesh().count(), 3*6);

    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> vertices = renderer.vertexBuffer().data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const Vector2>(vertices).prefix(2*4*3), Containers::arrayView<Vector2>({
        Vector2{ 2.5f, 10.5f} + offset, {0.0f, 0.5f},
        Vector2{ 2.5f,  5.5f} + offset, {0.0f, 0.0f},
        Vector2{12.5f, 10.5f} + offset, {1.0f, 0.5f},
        Vector2{12.5f,  5.5f} + offset, {1.0f, 0.0f},

        Vector2{ 5.5f, 8.75f} + offset, {0.0f, 1.0f},
        Vector2{ 5.5f, 3.75f} + offset, {0.0f, 0.5f},
        Vector2{10.5f, 8.75f} + offset, {0.5f, 1.0f},
        Vector2{10.5f, 3.75f} + offset, {0.5f, 0.5f},

        Vector2{ 4.0f,  9.0f} + offset, {0.5f, 1.0f},
        Vector2{ 4.0f,  4.0f} + offset, {0.5f, 0.5f},
        Vector2{ 9.0f,  9.0f} + offset, {1.0f, 1.0f},
        Vector2{ 9.0f,  4.0f} + offset, {1.0f, 0.5f},
    }), TestSuite::Compare::Container);
    #endif
}

void RendererGLTest::multiline() {
    struct Layouter: AbstractLayouter {
        explicit Layouter(const AbstractGlyphCache& cache, UnsignedInt fontId, Float scale, UnsignedInt glyphCount): AbstractLayouter{glyphCount}, _cache(cache), _fontId{fontId}, _scale{scale} {}

        Containers::Triple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override {
            Containers::Triple<Vector2i, Int, Range2Di> glyph = _cache.glyph(_fontId, 0);
            CORRADE_VERIFY(glyph.second() == 0);

            return {
                Range2D::fromSize(Vector2{glyph.first()}*_scale,
                                    Vector2{glyph.third().size()}*_scale),
                Range2D{glyph.third()}.scaled(1.0f/Vector2{_cache.size().xy()}),
                Vector2::xAxis(4.0f)*_scale
            };
        }

        const AbstractGlyphCache& _cache;
        UnsignedInt _fontId;
        Float _scale;
    };

    struct: AbstractFont {
        FontFeatures doFeatures() const override { return {}; }

        bool doIsOpened() const override { return _opened; }
        void doClose() override { _opened = false; }

        Properties doOpenFile(Containers::StringView, Float size) override {
            _opened = true;
            /* The ascent and descent values shouldn't be used for anything
               here and so can be completely arbitrary */
            return {size, -10000.0f, 1000.0f, 6.0f, 10};
        }

        UnsignedInt doGlyphId(char32_t) override { return 0; }
        Vector2 doGlyphSize(UnsignedInt) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache& cache, Float size, Containers::StringView text) override {
            /* The final rendered size should be a ratio of the font and layout
               size */
            return Containers::pointer<Layouter>(cache, *cache.findFont(this), size/this->size(), UnsignedInt(text.size()));
        }

        bool _opened = false;
    } font;
    font.openFile({}, 0.5f);

    /* Just a single glyph that scales to {1, 1} in the end */
    GlyphCache cache{{20, 20}};
    UnsignedInt fontId = cache.addFont(1, &font);
    cache.addGlyph(fontId, 0, {}, {{}, {2, 2}});

    /* Capture the correct function name */
    CORRADE_VERIFY(true);

    Range2D rectangle;
    std::vector<UnsignedInt> indices;
    std::vector<Vector2> positions, textureCoordinates;
    std::tie(positions, textureCoordinates, indices, rectangle) = Renderer2D::render(font,
        cache, 0.25f, "abcd\nef\n\nghi", Alignment::MiddleCenter);

    /* We're rendering text at 0.25f size and the font is scaled to 0.5f, so
       the line advance should be 6.0f*0.25f/0.5f = 3.0f */
    CORRADE_COMPARE(font.size(), 0.5f);
    CORRADE_COMPARE(font.lineHeight(), 6.0f);

    /* Bounds */
    CORRADE_COMPARE(rectangle, Range2D({-3.5f, -5.0f}, {3.5f, 5.0f}));

    /* Vertices
       [a] [b] [c] [d]
           [e] [f]

         [g] [h] [i]   */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        {-3.5f,  5.0f}, {-3.5f,  4.0f}, /* a */
        {-2.5f,  5.0f}, {-2.5f,  4.0f},

        {-1.5f,  5.0f}, {-1.5f,  4.0f}, /* b */
        {-0.5f,  5.0f}, {-0.5f,  4.0f},

        { 0.5f,  5.0f}, { 0.5f,  4.0f}, /* c */
        { 1.5f,  5.0f}, { 1.5f,  4.0f},

        { 2.5f,  5.0f}, { 2.5f,  4.0f}, /* d */
        { 3.5f,  5.0f}, { 3.5f,  4.0f},

        {-1.5f,  2.0f}, {-1.5f,  1.0f}, /* e */
        {-0.5f,  2.0f}, {-0.5f,  1.0f},

        { 0.5f,  2.0f}, { 0.5f,  1.0f}, /* f */
        { 1.5f,  2.0f}, { 1.5f,  1.0f},

        /* Two linebreaks here */

        {-2.5f, -4.0f}, {-2.5f, -5.0f}, /* g */
        {-1.5f, -4.0f}, {-1.5f, -5.0f},

        {-0.5f, -4.0f}, {-0.5f, -5.0f}, /* h */
        { 0.5f, -4.0f}, { 0.5f, -5.0f},

        { 1.5f, -4.0f}, { 1.5f, -5.0f}, /* i */
        { 2.5f, -4.0f}, { 2.5f, -5.0f},
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

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::RendererGLTest)
