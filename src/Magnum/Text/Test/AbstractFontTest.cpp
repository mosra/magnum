/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct AbstractFontTest: TestSuite::Tester {
    explicit AbstractFontTest();

    void openSingleData();

    void openFileAsData();
    void openFileAsDataNotFound();

    void openFileNotImplemented();
    void openDataNotSupported();
    void openDataNotImplemented();

    void glyphId();
    void glyphIdNoFont();

    void glyphAdvance();
    void glyphAdvanceNoFont();

    void layout();
    void layoutNoFont();

    void fillGlyphCache();
    void fillGlyphCacheNotSupported();
    void fillGlyphCacheNotImplemented();
    void fillGlyphCacheNoFont();

    void createGlyphCache();
    void createGlyphCacheNotSupported();
    void createGlyphCacheNotImplemented();
    void createGlyphCacheNoFont();
};

AbstractFontTest::AbstractFontTest() {
    addTests({&AbstractFontTest::openSingleData,

              &AbstractFontTest::openFileAsData,
              &AbstractFontTest::openFileAsDataNotFound,

              &AbstractFontTest::openFileNotImplemented,
              &AbstractFontTest::openDataNotSupported,
              &AbstractFontTest::openDataNotImplemented,

              &AbstractFontTest::glyphId,
              &AbstractFontTest::glyphIdNoFont,

              &AbstractFontTest::glyphAdvance,
              &AbstractFontTest::glyphAdvanceNoFont,

              &AbstractFontTest::layout,
              &AbstractFontTest::layoutNoFont,

              &AbstractFontTest::fillGlyphCache,
              &AbstractFontTest::fillGlyphCacheNotSupported,
              &AbstractFontTest::fillGlyphCacheNotImplemented,
              &AbstractFontTest::fillGlyphCacheNoFont,

              &AbstractFontTest::createGlyphCache,
              &AbstractFontTest::createGlyphCacheNotSupported,
              &AbstractFontTest::createGlyphCacheNotImplemented,
              &AbstractFontTest::createGlyphCacheNoFont});
}

class SingleDataFont: public Text::AbstractFont {
    public:
        explicit SingleDataFont(): opened(false) {}

        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return opened; }
        void doClose() override {}

        Metrics doOpenSingleData(const Containers::ArrayView<const char> data, Float) override {
            opened = (data.size() == 1 && data[0] == '\xa5');
            return {};
        }

        UnsignedInt doGlyphId(char32_t) override { return 0; }

        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }

        bool opened;
};

void AbstractFontTest::openSingleData() {
    /* doOpenData() should call doOpenSingleData() */
    SingleDataFont font;
    const char data[] = {'\xa5'};
    CORRADE_VERIFY(!font.isOpened());
    font.openData({{{}, data}}, 3.0f);
    CORRADE_VERIFY(font.isOpened());
}

void AbstractFontTest::openFileAsData() {
    /* doOpenFile() should call doOpenSingleData() */
    SingleDataFont font;
    CORRADE_VERIFY(!font.isOpened());
    font.openFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"), 3.0f);
    CORRADE_VERIFY(font.isOpened());
}

void AbstractFontTest::openFileAsDataNotFound() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font.openFile("nonexistent.foo", 12.0f));
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::openFile(): cannot open file nonexistent.foo\n");
}

void AbstractFontTest::openFileNotImplemented() {
    struct MyFont: AbstractFont {
        /* Supports neither file nor data opening */
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.openFile("file.foo", 34.0f);
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::openFile(): not implemented\n");
}

void AbstractFontTest::openDataNotSupported() {
    struct MyFont: AbstractFont {
        /* Supports neither file nor data opening */
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    /** @todo replace this with nullptr once multi-file support is done
        properly via callbacks */
    font.openData({}, 34.0f);
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::openData(): feature not supported\n");
}

void AbstractFontTest::openDataNotImplemented() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    /** @todo replace this with nullptr and openSingleData() once multi-file
        support is done properly via callbacks */
    font.openData({{}}, 34.0f);
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::openSingleData(): feature advertised but not implemented\n");
}

void AbstractFontTest::glyphId() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t a) override { return a*10; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    CORRADE_COMPARE(font.glyphId(U'a'), 970);
}

void AbstractFontTest::glyphIdNoFont() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.glyphId('a');
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::glyphId(): no font opened\n");
}

void AbstractFontTest::glyphAdvance() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt a) override { return {a*10.0f, -Float(a)/10.0f}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    CORRADE_COMPARE(font.glyphAdvance(97), (Vector2{970.0f, -9.7f}));
}

void AbstractFontTest::glyphAdvanceNoFont() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override {
            return nullptr;
        }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.glyphAdvance(97);
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::glyphAdvance(): no font opened\n");
}

struct DummyGlyphCache: AbstractGlyphCache {
    using AbstractGlyphCache::AbstractGlyphCache;

    GlyphCacheFeatures doFeatures() const override { return {}; }
    void doSetImage(const Vector2i&, const ImageView2D&) override {}
};

void AbstractFontTest::layout() {
    struct Layouter: AbstractLayouter {
        explicit Layouter(UnsignedInt count): AbstractLayouter{count} {}
        std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt) override { return {}; }
    };

    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache& cache, Float size, const std::string& str) override {
            return Containers::pointer<Layouter>(UnsignedInt(cache.textureSize().x()*str.size()*size));
        }
    } font;

    DummyGlyphCache cache{{100, 200}};
    Containers::Pointer<AbstractLayouter> layouter = font.layout(cache, 0.25f, "hello");
    CORRADE_COMPARE(layouter->glyphCount(), 100*5/4);
}

void AbstractFontTest::layoutNoFont() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    DummyGlyphCache cache{{100, 200}};
    font.layout(cache, 0.25f, "hello");
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::layout(): no font opened\n");
}

void AbstractFontTest::fillGlyphCache() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }

        void doFillGlyphCache(AbstractGlyphCache& cache, const std::u32string& characters) override {
            for(char a: characters) cache.insert(a*10, {a/2, a*2}, {});
        }
    } font;

    DummyGlyphCache cache{{100, 100}};

    CORRADE_COMPARE(cache.glyphCount(), 1);
    font.fillGlyphCache(cache, "helo");

    CORRADE_COMPARE(cache.glyphCount(), 5);
    CORRADE_COMPARE(cache['h'*10], (std::pair<Vector2i, Range2Di>{{52, 208}, {}}));
    CORRADE_COMPARE(cache['e'*10], (std::pair<Vector2i, Range2Di>{{50, 202}, {}}));
    CORRADE_COMPARE(cache['l'*10], (std::pair<Vector2i, Range2Di>{{54, 216}, {}}));
    CORRADE_COMPARE(cache['o'*10], (std::pair<Vector2i, Range2Di>{{55, 222}, {}}));
}

void AbstractFontTest::fillGlyphCacheNotSupported() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    DummyGlyphCache cache{{100, 100}};
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::fillGlyphCache(): feature not supported\n");
}

void AbstractFontTest::fillGlyphCacheNotImplemented() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    DummyGlyphCache cache{{100, 100}};
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::fillGlyphCache(): feature advertised but not implemented\n");
}

void AbstractFontTest::fillGlyphCacheNoFont() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    DummyGlyphCache cache{{100, 100}};
    font.fillGlyphCache(cache, "hello");
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::fillGlyphCache(): no font opened\n");
}

void AbstractFontTest::createGlyphCache() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }

        Containers::Pointer<AbstractGlyphCache> doCreateGlyphCache() override {
            Containers::Pointer<AbstractGlyphCache> cache{new DummyGlyphCache{{100, 100}}};
            for(char a: std::string{"helo"}) cache->insert(a*10, {a/2, a*2}, {});
            return cache;
        }
    } font;

    Containers::Pointer<AbstractGlyphCache> cache = font.createGlyphCache();

    CORRADE_COMPARE(cache->glyphCount(), 5);
    CORRADE_COMPARE((*cache)['h'*10], (std::pair<Vector2i, Range2Di>{{52, 208}, {}}));
    CORRADE_COMPARE((*cache)['e'*10], (std::pair<Vector2i, Range2Di>{{50, 202}, {}}));
    CORRADE_COMPARE((*cache)['l'*10], (std::pair<Vector2i, Range2Di>{{54, 216}, {}}));
    CORRADE_COMPARE((*cache)['o'*10], (std::pair<Vector2i, Range2Di>{{55, 222}, {}}));
}

void AbstractFontTest::createGlyphCacheNotSupported() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return {}; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::createGlyphCache(): feature not supported\n");
}

void AbstractFontTest::createGlyphCacheNotImplemented() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::PreparedGlyphCache; }
        bool doIsOpened() const override { return true; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::createGlyphCache(): feature advertised but not implemented\n");
}

void AbstractFontTest::createGlyphCacheNoFont() {
    struct MyFont: AbstractFont {
        Features doFeatures() const override { return Feature::PreparedGlyphCache; }
        bool doIsOpened() const override { return false; }
        void doClose() override {}

        UnsignedInt doGlyphId(char32_t) override { return {}; }
        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }
        Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache&, Float, const std::string&) override { return nullptr; }
    } font;

    std::ostringstream out;
    Error redirectError{&out};
    font.createGlyphCache();
    CORRADE_COMPARE(out.str(), "Text::AbstractFont::createGlyphCache(): no font opened\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontTest)
