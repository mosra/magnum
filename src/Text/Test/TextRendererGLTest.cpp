/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Test/AbstractOpenGLTester.h"
#include "Text/AbstractFont.h"
#include "Text/TextRenderer.h"

namespace Magnum { namespace Text { namespace Test {

class TextRendererGLTest: public Magnum::Test::AbstractOpenGLTester {
    public:
        explicit TextRendererGLTest();

        void renderData();
        void renderMesh();
        void mutableText();
};

TextRendererGLTest::TextRendererGLTest() {
    addTests({&TextRendererGLTest::renderData,
              &TextRendererGLTest::renderMesh,
              &TextRendererGLTest::mutableText});
}

namespace {

class TestLayouter: public Text::AbstractLayouter {
    public:
        explicit TestLayouter(Float size, std::size_t glyphCount): _size(size) {
            _glyphCount = glyphCount;
        }

        std::tuple<Rectangle, Rectangle, Vector2> renderGlyph(UnsignedInt i) override {
            return std::make_tuple(
                Rectangle({}, Vector2(3.0f, 2.0f)*((i+1)*_size)),
                Rectangle::fromSize({i*6.0f, 0.0f}, {6.0f, 10.0f}),
                (Vector2::xAxis((i+1)*3.0f)+Vector2(1.0f, -1.0f))*_size
            );
        }

    private:
        Float _size;
};

class TestFont: public Text::AbstractFont {
    public:
        Features doFeatures() const override { return Feature::OpenData; }

        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return 0; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        AbstractLayouter* doLayout(const GlyphCache&, Float size, const std::string& text) override {
            return new TestLayouter(size, text.size());
        }
};

}

void TextRendererGLTest::renderData() {
    TestFont font;
    std::vector<Vector2> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<UnsignedInt> indices;
    Rectangle bounds;
    std::tie(positions, textureCoordinates, indices, bounds) = Text::AbstractTextRenderer::render(font, *static_cast<GlyphCache*>(nullptr), 0.25f, "abc");

    /* Three glyphs, three quads -> 12 vertices, 18 indices */
    CORRADE_COMPARE(positions.size(), 12);
    CORRADE_COMPARE(textureCoordinates.size(), 12);
    CORRADE_COMPARE(indices.size(), 18);

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
        {0.0f,  0.5f},
        {0.0f,  0.0f},
        {0.75f, 0.5f},
        {0.75f, 0.0f},

        {1.0f,  0.75f},
        {1.0f, -0.25f},
        {2.5f,  0.75f},
        {2.5f, -0.25f},

        {2.75f,  1.0f},
        {2.75f, -0.5f},
        {5.0f,   1.0f},
        {5.0f,  -0.5f}
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

    /* Bounds */
    CORRADE_COMPARE(bounds, Rectangle({0.0f, -0.5f}, {5.0f, 1.0f}));
}

void TextRendererGLTest::renderMesh() {
    TestFont font;
    Mesh mesh;
    Buffer vertexBuffer, indexBuffer;
    Rectangle bounds;
    std::tie(mesh, bounds) = Text::TextRenderer3D::render(font, *static_cast<GlyphCache*>(nullptr), 0.25f, "abc", vertexBuffer, indexBuffer, Buffer::Usage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    /* Vertex buffer contents */
    Containers::Array<Float> vertices = vertexBuffer.data<Float>();
    CORRADE_COMPARE(std::vector<Float>(vertices.begin(), vertices.end()), (std::vector<Float>{
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
    }));

    Containers::Array<UnsignedByte> indices = indexBuffer.data<UnsignedByte>();
    CORRADE_COMPARE(std::vector<UnsignedByte>(indices.begin(), indices.end()), (std::vector<UnsignedByte>{
        0,  1,  2,  1,  3,  2,
        4,  5,  6,  5,  7,  6,
        8,  9, 10,  9, 11, 10
    }));
    #endif

    /* Bounds */
    CORRADE_COMPARE(bounds, Rectangle({0.0f, -0.5f}, {5.0f, 1.0f}));
}

void TextRendererGLTest::mutableText() {
    TestFont font;
    Text::TextRenderer2D renderer(font, *static_cast<GlyphCache*>(nullptr), 0.25f);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 0);
    CORRADE_COMPARE(renderer.rectangle(), Rectangle());

    /* Reserve some capacity */
    renderer.reserve(4, Buffer::Usage::StaticDraw, Buffer::Usage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(renderer.capacity(), 4);
    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<UnsignedByte> indices = renderer.indexBuffer().data<UnsignedByte>();
    CORRADE_COMPARE(std::vector<UnsignedByte>(indices.begin(), indices.end()), (std::vector<UnsignedByte>{
         0,  1,  2,  1,  3,  2,
         4,  5,  6,  5,  7,  6,
         8,  9, 10,  9, 11, 10,
        12, 13, 14, 13, 15, 14
    }));
    #endif

    /* Render text */
    renderer.render("abc");
    MAGNUM_VERIFY_NO_ERROR();
    /** @todo How to verify this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<Float> vertices = renderer.vertexBuffer().subData<Float>(0, 48);
    CORRADE_COMPARE(std::vector<Float>(vertices.begin(), vertices.end()), (std::vector<Float>{
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
    }));
    #endif

    /* Updated bounds */
    CORRADE_COMPARE(renderer.rectangle(), Rectangle({0.0f, -0.5f}, {5.0f, 1.0f}));
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::TextRendererGLTest)
