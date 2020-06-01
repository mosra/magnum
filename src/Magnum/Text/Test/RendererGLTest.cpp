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

#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Text/AbstractFont.h"
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

RendererGLTest::RendererGLTest() {
    addTests({&RendererGLTest::renderData,
              &RendererGLTest::renderMesh,
              &RendererGLTest::renderMeshIndexType,
              &RendererGLTest::mutableText,

              &RendererGLTest::multiline});
}

class TestLayouter: public Text::AbstractLayouter {
    public:
        explicit TestLayouter(Float size, std::size_t glyphCount): AbstractLayouter(glyphCount), _size(size) {}

    private:
        std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt i) override {
            return std::make_tuple(
                Range2D({}, Vector2(3.0f, 2.0f)*((i+1)*_size)),
                Range2D::fromSize({i*6.0f, 0.0f}, {6.0f, 10.0f}),
                (Vector2::xAxis((i+1)*3.0f)+Vector2(1.0f, -1.0f))*_size
            );
        }

        Float _size;
};

class TestFont: public Text::AbstractFont {
    FontFeatures doFeatures() const override { return FontFeature::OpenData; }

    bool doIsOpened() const override { return true; }
    void doClose() override {}

    UnsignedInt doGlyphId(char32_t) override { return 0; }
    Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

    Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, const Float size, const std::string& text) override {
        return Containers::Pointer<AbstractLayouter>(new TestLayouter(size, text.size()));
    }
};

/* *static_cast<GlyphCache*>(nullptr) makes Clang Analyzer grumpy */
char glyphCacheData;
GlyphCache& nullGlyphCache = *reinterpret_cast<GlyphCache*>(&glyphCacheData);

void RendererGLTest::renderData() {
    TestFont font;
    std::vector<Vector2> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<UnsignedInt> indices;
    Range2D bounds;
    std::tie(positions, textureCoordinates, indices, bounds) = Text::AbstractRenderer::render(font, nullGlyphCache, 0.25f, "abc", Alignment::MiddleRightIntegral);

    /* Three glyphs, three quads -> 12 vertices, 18 indices */
    CORRADE_COMPARE(positions.size(), 12);
    CORRADE_COMPARE(textureCoordinates.size(), 12);
    CORRADE_COMPARE(indices.size(), 18);

    /* Alignment offset. Y would be -0.25f if it wasn't integral */
    const Vector2 offset{-5.0f, 0.0f};

    /* Bounds */
    CORRADE_COMPARE(bounds, Range2D({0.0f, -0.5f}, {5.0f, 1.0f}).translated(offset));

    /* Vertex positions and texture coordinates
       0---2
       |   |
       |   |
       |   |
       1---3 */

    /* Vertex positions
              +---+
          +-+ |   |
        a |b| | c |
          +-+ |   |
              +---+ */
    CORRADE_COMPARE(positions, (std::vector<Vector2>{
        Vector2{0.0f,  0.5f} + offset,
        Vector2{0.0f,  0.0f} + offset,
        Vector2{0.75f, 0.5f} + offset,
        Vector2{0.75f, 0.0f} + offset,

        Vector2{1.0f,  0.75f} + offset,
        Vector2{1.0f, -0.25f} + offset,
        Vector2{2.5f,  0.75f} + offset,
        Vector2{2.5f, -0.25f} + offset,

        Vector2{2.75f,  1.0f} + offset,
        Vector2{2.75f, -0.5f} + offset,
        Vector2{5.0f,   1.0f} + offset,
        Vector2{5.0f,  -0.5f} + offset
    }));

    /* Texture coordinates
       +-+ +-+ +-+
       |a| |b| |c|
       +-+ +-+ +-+ */
    CORRADE_COMPARE(textureCoordinates, (std::vector<Vector2>{
        {0.0f, 10.0f},
        {0.0f,  0.0f},
        {6.0f, 10.0f},
        {6.0f,  0.0f},

        { 6.0f, 10.0f},
        { 6.0f,  0.0f},
        {12.0f, 10.0f},
        {12.0f,  0.0f},

        {12.0f, 10.0f},
        {12.0f,  0.0f},
        {18.0f, 10.0f},
        {18.0f,  0.0f}
    }));

    /* Indices
       0---2 0---2 5
       |   | |  / /|
       |   | | / / |
       |   | |/ /  |
       1---3 1 3---4 */
    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{
        0,  1,  2,  1,  3,  2,
        4,  5,  6,  5,  7,  6,
        8,  9, 10,  9, 11, 10
    }));
}

void RendererGLTest::renderMesh() {
    TestFont font;
    GL::Mesh mesh{NoCreate};
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array},
        indexBuffer{GL::Buffer::TargetHint::ElementArray};
    Range2D bounds;
    std::tie(mesh, bounds) = Text::Renderer3D::render(font, nullGlyphCache,
        0.25f, "abc", vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw, Alignment::TopCenter);
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Alignment offset */
    const Vector2 offset{-2.5f, -1.0f};

    /* Bounds */
    CORRADE_COMPARE(bounds, Range2D({0.0f, -0.5f}, {5.0f, 1.0f}).translated(offset));

    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    /* Vertex buffer contents */
    Containers::Array<char> vertices = vertexBuffer.data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(vertices),
        (Containers::Array<Float>{Containers::InPlaceInit, {
            0.0f + offset.x(),  0.5f + offset.y(), 0.0f, 10.0f,
            0.0f + offset.x(),  0.0f + offset.y(), 0.0f,  0.0f,
            0.75f + offset.x(), 0.5f + offset.y(), 6.0f, 10.0f,
            0.75f + offset.x(), 0.0f + offset.y(), 6.0f,  0.0f,

            1.0f + offset.x(),  0.75f + offset.y(),  6.0f, 10.0f,
            1.0f + offset.x(), -0.25f + offset.y(),  6.0f,  0.0f,
            2.5f + offset.x(),  0.75f + offset.y(), 12.0f, 10.0f,
            2.5f + offset.x(), -0.25f + offset.y(), 12.0f,  0.0f,

            2.75f + offset.x(),  1.0f + offset.y(), 12.0f, 10.0f,
            2.75f + offset.x(), -0.5f + offset.y(), 12.0f,  0.0f,
            5.0f + offset.x(),   1.0f + offset.y(), 18.0f, 10.0f,
            5.0f + offset.x(),  -0.5f + offset.y(), 18.0f,  0.0f
        }}), TestSuite::Compare::Container);

    Containers::Array<char> indices = indexBuffer.data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indices),
        (Containers::Array<UnsignedByte>{Containers::InPlaceInit, {
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }}), TestSuite::Compare::Container);
    #endif
}

void RendererGLTest::renderMeshIndexType() {
    #ifndef MAGNUM_TARGET_GLES
    TestFont font;
    GL::Mesh mesh{NoCreate};
    GL::Buffer vertexBuffer, indexBuffer;

    /* Sizes: four vertices per glyph, each vertex has 2D position and 2D
       texture coordinates, each float is four bytes; six indices per glyph. */

    /* 8-bit indices (exactly 256 vertices) */
    std::tie(mesh, std::ignore) = Text::Renderer3D::render(font, nullGlyphCache,
        1.0f, std::string(64, 'a'), vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    Containers::Array<char> indicesByte = indexBuffer.data();
    CORRADE_COMPARE(vertexBuffer.size(), 256*(2 + 2)*4);
    CORRADE_COMPARE(indicesByte.size(), 64*6);
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indicesByte).prefix(18),
        (Containers::Array<UnsignedByte>{Containers::InPlaceInit, {
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }}), TestSuite::Compare::Container);

    /* 16-bit indices (260 vertices) */
    std::tie(mesh, std::ignore) = Text::Renderer3D::render(font, nullGlyphCache,
        1.0f, std::string(65, 'a'), vertexBuffer, indexBuffer, GL::BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    Containers::Array<char> indicesShort = indexBuffer.data();
    CORRADE_COMPARE(vertexBuffer.size(), 260*(2 + 2)*4);
    CORRADE_COMPARE(indicesShort.size(), 65*6*2);
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedShort>(indicesShort).prefix(18),
        (Containers::Array<UnsignedShort>{Containers::InPlaceInit, {
            0,  1,  2,  1,  3,  2,
            4,  5,  6,  5,  7,  6,
            8,  9, 10,  9, 11, 10
        }}), TestSuite::Compare::Container);
    #else
    CORRADE_SKIP("Can't verify buffer contents on OpenGL ES.");
    #endif
}

void RendererGLTest::mutableText() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::map_buffer_range>())
        CORRADE_SKIP(GL::Extensions::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::map_buffer_range>() &&
       !GL::Context::current().isExtensionSupported<GL::Extensions::OES::mapbuffer>())
        CORRADE_SKIP("No required extension is supported");
    #endif

    TestFont font;
    Text::Renderer2D renderer(font, nullGlyphCache, 0.25f);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 0);
    CORRADE_COMPARE(renderer.rectangle(), Range2D());

    /* Reserve some capacity */
    renderer.reserve(4, GL::BufferUsage::DynamicDraw, GL::BufferUsage::DynamicDraw);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 4);
    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> indices = renderer.indexBuffer().data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const UnsignedByte>(indices).prefix(24),
        (Containers::Array<UnsignedByte>{Containers::InPlaceInit, {
             0,  1,  2,  1,  3,  2,
             4,  5,  6,  5,  7,  6,
             8,  9, 10,  9, 11, 10,
            12, 13, 14, 13, 15, 14
        }}), TestSuite::Compare::Container);
    #endif

    /* Render text */
    renderer.render("abc");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Updated bounds */
    CORRADE_COMPARE(renderer.rectangle(), Range2D({0.0f, -0.5f}, {5.0f, 1.0f}));

    /* Aligned to line/left, no offset needed */

    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> vertices = renderer.vertexBuffer().data();
    CORRADE_COMPARE_AS(Containers::arrayCast<const Float>(vertices).prefix(48),
        (Containers::Array<Float>{Containers::InPlaceInit, {
            0.0f,  0.5f, 0.0f, 10.0f,
            0.0f,  0.0f, 0.0f,  0.0f,
            0.75f, 0.5f, 6.0f, 10.0f,
            0.75f, 0.0f, 6.0f,  0.0f,

            1.0f,  0.75f,  6.0f, 10.0f,
            1.0f, -0.25f,  6.0f,  0.0f,
            2.5f,  0.75f, 12.0f, 10.0f,
            2.5f, -0.25f, 12.0f,  0.0f,

            2.75f,  1.0f, 12.0f, 10.0f,
            2.75f, -0.5f, 12.0f,  0.0f,
            5.0f,   1.0f, 18.0f, 10.0f,
            5.0f,  -0.5f, 18.0f,  0.0f
        }}), TestSuite::Compare::Container);
    #endif
}

void RendererGLTest::multiline() {
    class Layouter: public Text::AbstractLayouter {
        public:
            explicit Layouter(UnsignedInt glyphCount): AbstractLayouter(glyphCount) {}

        private:
            std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override {
                return std::make_tuple(Range2D({}, Vector2(1.0f)), Range2D({}, Vector2(1.0f)), Vector2::xAxis(2.0f));
            }
    };

    class Font: public Text::AbstractFont {
        public:
            explicit Font(): _opened(false) {}

        private:
            FontFeatures doFeatures() const override { return {};  }

            bool doIsOpened() const override { return _opened; }
            void doClose() override { _opened = false; }

            Metrics doOpenFile(const std::string&, Float) override {
                _opened = true;
                return {0.5f, 0.45f, -0.25f, 0.75f};
            }

            UnsignedInt doGlyphId(char32_t) override { return 0; }
            Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

            Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string& text) override {
                return Containers::Pointer<AbstractLayouter>(new Layouter(text.size()));
            }

            bool _opened;
    };

    Font font;
    font.openFile({}, 0.0f);
    Range2D rectangle;
    std::vector<UnsignedInt> indices;
    std::vector<Vector2> positions, textureCoordinates;
    std::tie(positions, textureCoordinates, indices, rectangle) = Text::Renderer2D::render(font,
        nullGlyphCache, 2.0f, "abcd\nef\n\nghi", Alignment::MiddleCenter);

    /* We're rendering text at 2.0f size and the font is scaled to 0.3f, so the
       line advance should be 0.75f*2.0f/0.5f = 3.0f */
    CORRADE_COMPARE(font.size(), 0.5f);
    CORRADE_COMPARE(font.ascent(), 0.45f);
    CORRADE_COMPARE(font.descent(), -0.25f);
    CORRADE_COMPARE(font.lineHeight(), 0.75f);

    /* Bounds */
    CORRADE_COMPARE(rectangle, Range2D({-3.5f, -5.0f}, {3.5f, 5.0f}));

    /* Vertices
       [a] [b] [c] [d]
           [e] [f]

         [g] [h] [i]   */
    CORRADE_COMPARE_AS(positions, (std::vector<Vector2>{
        Vector2{-3.5f,  5.0f}, Vector2{-3.5f,  4.0f}, /* a */
        Vector2{-2.5f,  5.0f}, Vector2{-2.5f,  4.0f},

        Vector2{-1.5f,  5.0f}, Vector2{-1.5f,  4.0f}, /* b */
        Vector2{-0.5f,  5.0f}, Vector2{-0.5f,  4.0f},

        Vector2{ 0.5f,  5.0f}, Vector2{ 0.5f,  4.0f}, /* c */
        Vector2{ 1.5f,  5.0f}, Vector2{ 1.5f,  4.0f},

        Vector2{ 2.5f,  5.0f}, Vector2{ 2.5f,  4.0f}, /* d */
        Vector2{ 3.5f,  5.0f}, Vector2{ 3.5f,  4.0f},

        Vector2{-1.5f,  2.0f}, Vector2{-1.5f,  1.0f}, /* e */
        Vector2{-0.5f,  2.0f}, Vector2{-0.5f,  1.0f},

        Vector2{ 0.5f,  2.0f}, Vector2{ 0.5f,  1.0f}, /* f */
        Vector2{ 1.5f,  2.0f}, Vector2{ 1.5f,  1.0f},

        Vector2{-2.5f, -4.0f}, Vector2{-2.5f, -5.0f}, /* g */
        Vector2{-1.5f, -4.0f}, Vector2{-1.5f, -5.0f},

        Vector2{-0.5f, -4.0f}, Vector2{-0.5f, -5.0f}, /* h */
        Vector2{ 0.5f, -4.0f}, Vector2{ 0.5f, -5.0f},

        Vector2{ 1.5f, -4.0f}, Vector2{ 1.5f, -5.0f}, /* i */
        Vector2{ 2.5f, -4.0f}, Vector2{ 2.5f, -5.0f},
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
