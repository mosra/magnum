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
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct TgaImageConverterTest: TestSuite::Tester {
    explicit TgaImageConverterTest();

    void wrongFormat();

    void rgb();
    void rgba();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _converterManager{"nonexistent"};
    PluginManager::Manager<AbstractImporter> _importerManager{"nonexistent"};
};

constexpr struct {
    const char* name;
    ImageConverterFlags flags;
    const char* message24;
    const char* message32;
} VerboseData[] {
    {"", {}, "", ""},
    {"verbose", ImageConverterFlag::Verbose,
        "Trade::TgaImageConverter::exportToData(): converting from RGB to BGR\n",
        "Trade::TgaImageConverter::exportToData(): converting from RGBA to BGRA\n"}
};

/* Padded to four byte alignment (the resulting file is *not* padded) */
constexpr char OriginalDataRGB[] = {
    /* Skip */
    0, 0, 0, 0, 0, 0, 0, 0,

    1, 2, 3, 2, 3, 4, 0, 0,
    3, 4, 5, 4, 5, 6, 0, 0,
    5, 6, 7, 6, 7, 8, 0, 0
};
constexpr char ConvertedDataRGB[] = {
    1, 2, 3, 2, 3, 4,
    3, 4, 5, 4, 5, 6,
    5, 6, 7, 6, 7, 8
};

const ImageView2D OriginalRGB{PixelStorage{}.setSkip({0, 1, 0}),
    PixelFormat::RGB8Unorm, {2, 3}, OriginalDataRGB};

constexpr char OriginalDataRGBA[] = {
    1, 2, 3, 4, 2, 3, 4, 5,
    3, 4, 5, 6, 4, 5, 6, 7,
    5, 6, 7, 8, 6, 7, 8, 9
};
const ImageView2D OriginalRGBA{PixelFormat::RGBA8Unorm, {2, 3}, OriginalDataRGBA};

TgaImageConverterTest::TgaImageConverterTest() {
    addTests({&TgaImageConverterTest::wrongFormat});

    addInstancedTests({
        &TgaImageConverterTest::rgb,
        &TgaImageConverterTest::rgba},
        Containers::arraySize(VerboseData));

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* Optional plugins that don't have to be here */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void TgaImageConverterTest::wrongFormat() {
    ImageView2D image{PixelFormat::RG8Unorm, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    const auto data = converter->exportToData(image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::exportToData(): unsupported pixel format PixelFormat::RG8Unorm\n");
}

void TgaImageConverterTest::rgb() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->setFlags(data.flags);

    std::ostringstream out;
    Containers::Array<char> array;
    {
        Debug redirectOutput{&out};
        array = converter->exportToData(OriginalRGB);
    }
    CORRADE_VERIFY(out);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 1);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedDataRGB),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message24);
}

void TgaImageConverterTest::rgba() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->setFlags(data.flags);

    std::ostringstream out;
    Containers::Array<char> array;
    {
        Debug redirectOutput{&out};
        array = converter->exportToData(OriginalRGBA);
    }
    CORRADE_VERIFY(out);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 4);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(OriginalDataRGBA),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message32);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImageConverterTest)
