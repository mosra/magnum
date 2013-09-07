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

#include <Utility/Directory.h>
#include <TestSuite/Compare/File.h>
#include <Extensions.h>
#include <ImageFormat.h>
#include <TextureFormat.h>
#include <Test/AbstractOpenGLTester.h>
#include <Text/GlyphCache.h>
#include <Trade/ImageData.h>

#include "FreeTypeFont/FreeTypeFont.h"
#include "MagnumFontConverter/MagnumFontConverter.h"
#include "TgaImporter/TgaImporter.h"

#include "freeTypeFontTestConfigure.h"
#include "magnumFontTestConfigure.h"
#include "magnumFontConverterTestConfigure.h"

namespace Magnum { namespace Text { namespace Test {

class MagnumFontConverterTest: public Magnum::Test::AbstractOpenGLTester {
    public:
        explicit MagnumFontConverterTest();

        ~MagnumFontConverterTest();

        void exportFont();
};

MagnumFontConverterTest::MagnumFontConverterTest() {
    addTests({&MagnumFontConverterTest::exportFont});

    FreeTypeFont::initialize();
}

MagnumFontConverterTest::~MagnumFontConverterTest() {
    FreeTypeFont::finalize();
}

void MagnumFontConverterTest::exportFont() {
    /* Remove previously created files */
    Utility::Directory::rm(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf"));
    Utility::Directory::rm(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga"));

    /* Open font */
    FreeTypeFont font;
    CORRADE_VERIFY(font.openFile(Utility::Directory::join(FREETYPEFONT_TEST_DIR, "Oxygen.ttf"), 16.0f));

    /* Create fake cache */
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::texture_rg);
    GlyphCache cache(TextureFormat::R8, Vector2i(1536), Vector2i(256), Vector2i(24));
    cache.insert(font.glyphId(U'W'), {25, 34}, {{0, 8}, {16, 128}});
    cache.insert(font.glyphId(U'e'), {25, 12}, {{16, 4}, {64, 32}});

    /* Convert the file */
    MagnumFontConverter converter;
    converter.exportFontToFile(font, cache, Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font"), "Wave");

    /* Verify font parameters */
    /** @todo This might behave differently elsewhere due to unspecified order of glyphs in cache */
    CORRADE_COMPARE_AS(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.conf"),
                       Utility::Directory::join(MAGNUMFONT_TEST_DIR, "font.conf"),
                       TestSuite::Compare::File);

    /* Verify font image, no need to test image contents, as the image is garbage anyway */
    Trade::TgaImporter importer;
    CORRADE_VERIFY(importer.openFile(Utility::Directory::join(MAGNUMFONTCONVERTER_TEST_WRITE_DIR, "font.tga")));
    std::optional<Trade::ImageData2D> image = importer.image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->size(), Vector2i(256));
    CORRADE_COMPARE(image->format(), ImageFormat::Red);
    CORRADE_COMPARE(image->type(), ImageType::UnsignedByte);
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::MagnumFontConverterTest)
