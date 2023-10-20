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
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h> /** @todo drop once Debug is stream-free */

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Text/GlyphCache.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct RendererGLTest: GL::OpenGLTester {
    explicit RendererGLTest();

    void renderMesh();
    void renderMeshIndexType();
    void mutableText();
};

RendererGLTest::RendererGLTest() {
    addTests({&RendererGLTest::renderMesh,
              &RendererGLTest::renderMeshIndexType,
              &RendererGLTest::mutableText});
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
    const Vector2 offset{-1.5f, -0.5f};

    /* Bounds */
    CORRADE_COMPARE(bounds, (Range2D{{0.0f, -1.25f}, {3.0f, 2.25f}}.translated(offset)));

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
    const Vector2 offset{-1.5f, -0.5f};

    /* Updated bounds and mesh vertex count */
    CORRADE_COMPARE(renderer.rectangle(), (Range2D{{0.0f, -1.25f}, {3.0f, 2.25f}}.translated(offset)));
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

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::RendererGLTest)
