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
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once AbstractFont is <string>-free */
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/FileCallback.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/AbstractShaper.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Text { namespace Test { namespace {

struct MagnumFontTest: TestSuite::Tester {
    explicit MagnumFontTest();

    void nonexistent();
    void properties();

    void shape();
    void shapeEmpty();
    void shaperReuse();

    void fileCallbackImage();
    void fileCallbackImageNotFound();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
    PluginManager::Manager<AbstractFont> _fontManager{"nonexistent"};
};

const struct {
    const char* name;
    const char* string;
    UnsignedInt eGlyphId;
    UnsignedInt begin, end;
} ShapeData[]{
    {"", "Wave", 1, 0, ~UnsignedInt{}},
    {"substring", "haWavefefe", 1, 2, 6},
    {"UTF-8", "Wavě", 3, 0, ~UnsignedInt{}},
    {"UTF-8 substring", "haWavěfefe", 3, 2, 7},
};

MagnumFontTest::MagnumFontTest() {
    addTests({&MagnumFontTest::nonexistent,
              &MagnumFontTest::properties});

    addInstancedTests({&MagnumFontTest::shape},
        Containers::arraySize(ShapeData));

    addTests({&MagnumFontTest::shapeEmpty,
              &MagnumFontTest::shaperReuse,

              &MagnumFontTest::fileCallbackImage,
              &MagnumFontTest::fileCallbackImageNotFound});

    /* Load the plugins directly from the build tree. Otherwise they're static
       and already loaded. */
    _fontManager.registerExternalManager(_importerManager);
    #if defined(TGAIMPORTER_PLUGIN_FILENAME) && defined(MAGNUMFONT_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT_OUTPUT(_fontManager.load(MAGNUMFONT_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void MagnumFontTest::nonexistent() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!font->openFile("nonexistent.conf", 0.0f));
    /* There's an error message from Path::read() before */
    CORRADE_COMPARE_AS(out.str(),
        "\nText::AbstractFont::openFile(): cannot open file nonexistent.conf\n",
        TestSuite::Compare::StringHasSuffix);
}

void MagnumFontTest::properties() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");

    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));
    CORRADE_COMPARE(font->size(), 16.0f);
    CORRADE_COMPARE(font->ascent(), 25.0f);
    CORRADE_COMPARE(font->descent(), -10.0f);
    CORRADE_COMPARE(font->lineHeight(), 39.7333f);
    CORRADE_COMPARE(font->glyphCount(), 4);
    CORRADE_COMPARE(font->glyphId(U'W'), 2);
    CORRADE_COMPARE(font->glyphId(U'e'), 1);
    UnsignedInt eId = font->glyphId(
        /* MSVC (but not clang-cl) doesn't support UTF-8 in char32_t literals
           but it does it regular strings. Still a problem in MSVC 2022, what a
           trash fire, can't you just give up on those codepage insanities
           already, ffs?! */
        #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG)
        U'\u011B'
        #else
        U'ě'
        #endif
    );
    CORRADE_COMPARE(eId, 3);
    CORRADE_COMPARE(font->glyphSize(font->glyphId(U'W')), (Vector2{8.0f, 44.0f}));
    CORRADE_COMPARE(font->glyphAdvance(font->glyphId(U'W')), (Vector2{23.0f, 0.0f}));
}

void MagnumFontTest::shape() {
    auto&& data = ShapeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");
    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));

    Containers::Pointer<AbstractShaper> shaper = font->createShaper();

    CORRADE_COMPARE(shaper->shape(data.string, data.begin, data.end), 4);

    UnsignedInt ids[4];
    Vector2 offsets[4];
    Vector2 advances[4];
    shaper->glyphIdsInto(ids);
    shaper->glyphOffsetsAdvancesInto(offsets, advances);
    CORRADE_COMPARE_AS(Containers::arrayView(ids), Containers::arrayView({
        2u,             /* 'W' */
        0u,             /* 'a' (not found) */
        0u,             /* 'v' (not found) */
        data.eGlyphId   /* 'e' or 'ě' */
    }), TestSuite::Compare::Container);
    /* There are no glyph-specific offsets here */
    CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2>({
        {}, {}, {}, {}
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(advances), Containers::arrayView<Vector2>({
        {23.0f, 0.0f},
        {8.0f, 0.0f},
        {8.0f, 0.0f},
        {12.f, 0.0f}
    }), TestSuite::Compare::Container);
}

void MagnumFontTest::shapeEmpty() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");
    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));

    Containers::Pointer<AbstractShaper> shaper = font->createShaper();

    /* Shouldn't crash or do anything rogue */
    CORRADE_COMPARE(shaper->shape("Wave", 2, 2), 0);
}

void MagnumFontTest::shaperReuse() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");
    CORRADE_VERIFY(font->openFile(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"), 0.0f));

    Containers::Pointer<AbstractShaper> shaper = font->createShaper();

    /* Empty text */
    {
        CORRADE_COMPARE(shaper->shape("Wave", 2, 2), 0);

    /* Short text. Empty shape shouldn't have caused any broken state. */
    } {
        CORRADE_COMPARE(shaper->shape("We"), 2);
        UnsignedInt ids[2];
        Vector2 offsets[2];
        Vector2 advances[2];
        shaper->glyphIdsInto(ids);
        shaper->glyphOffsetsAdvancesInto(offsets, advances);
        CORRADE_COMPARE_AS(Containers::arrayView(ids), Containers::arrayView({
            2u, /* 'W' */
            1u  /* 'e' */
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2>({
            {}, {},
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(advances), Containers::arrayView<Vector2>({
            {23.0f, 0.0f},
            {12.f, 0.0f}
        }), TestSuite::Compare::Container);

    /* Long text, same as in shape(), should enlarge the array for it */
    } {
        CORRADE_COMPARE(shaper->shape("Wave"), 4);
        UnsignedInt ids[4];
        Vector2 offsets[4];
        Vector2 advances[4];
        shaper->glyphIdsInto(ids);
        shaper->glyphOffsetsAdvancesInto(offsets, advances);
        CORRADE_COMPARE_AS(Containers::arrayView(ids), Containers::arrayView({
            2u, /* 'W' */
            0u, /* 'a' (not found) */
            0u, /* 'v' (not found) */
            1u  /* 'e' */
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2>({
            {}, {}, {}, {}
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(advances), Containers::arrayView<Vector2>({
            {23.0f, 0.0f},
            {8.0f, 0.0f},
            {8.0f, 0.0f},
            {12.f, 0.0f}
        }), TestSuite::Compare::Container);

    /* Short text again, should not leave the extra glyphs there */
    } {
        CORRADE_COMPARE(shaper->shape("a"), 1);
        UnsignedInt ids[1];
        Vector2 offsets[1];
        Vector2 advances[1];
        shaper->glyphIdsInto(ids);
        shaper->glyphOffsetsAdvancesInto(offsets, advances);
        CORRADE_COMPARE_AS(Containers::arrayView(ids), Containers::arrayView({
            0u,
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(offsets), Containers::arrayView<Vector2>({
            {},
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(Containers::arrayView(advances), Containers::arrayView<Vector2>({
            {8.0f, 0.0f}
        }), TestSuite::Compare::Container);
    }
}

void MagnumFontTest::fileCallbackImage() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");
    CORRADE_VERIFY(font->features() & FontFeature::FileCallback);

    std::unordered_map<std::string, Containers::Array<char>> files;
    Containers::Optional<Containers::Array<char>> conf = Utility::Path::read(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"));
    Containers::Optional<Containers::Array<char>> tga =
    Utility::Path::read(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.tga"));
    CORRADE_VERIFY(conf);
    CORRADE_VERIFY(tga);
    files["not/a/path/font.conf"] = *Utility::move(conf);
    files["not/a/path/font.tga"] = *Utility::move(tga);
    font->setFileCallback([](const std::string& filename, InputFileCallbackPolicy policy,
        std::unordered_map<std::string, Containers::Array<char>>& files) {
            Debug{} << "Loading" << filename << "with" << policy;
            return Containers::optional(Containers::ArrayView<const char>(files.at(filename)));
        }, files);

    CORRADE_VERIFY(font->openFile("not/a/path/font.conf", 13.0f));
    CORRADE_COMPARE(font->size(), 16.0f);
    CORRADE_COMPARE(font->ascent(), 25.0f);
    CORRADE_COMPARE(font->descent(), -10.0f);
    CORRADE_COMPARE(font->lineHeight(), 39.7333f);
    CORRADE_COMPARE(font->glyphAdvance(font->glyphId(U'W')), Vector2(23.0f, 0.0f));
}

void MagnumFontTest::fileCallbackImageNotFound() {
    Containers::Pointer<AbstractFont> font = _fontManager.instantiate("MagnumFont");
    CORRADE_VERIFY(font->features() & FontFeature::FileCallback);

    font->setFileCallback([](const std::string&, InputFileCallbackPolicy, void*) {
            return Containers::Optional<Containers::ArrayView<const char>>{};
        });

    std::ostringstream out;
    Error redirectError{&out};
    Containers::Optional<Containers::Array<char>> conf = Utility::Path::read(Utility::Path::join(MAGNUMFONT_TEST_DIR, "font.conf"));
    CORRADE_VERIFY(conf);
    CORRADE_VERIFY(!font->openData(*conf, 13.0f));
    CORRADE_COMPARE(out.str(), "Trade::AbstractImporter::openFile(): cannot open file font.tga\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontTest)
