/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "MagnumPlugins/TgaImporter/TgaHeader.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct TgaImageConverterTest: TestSuite::Tester {
    explicit TgaImageConverterTest();

    void wrongFormat();

    void uncompressedRgb();
    void uncompressedRgba();
    void uncompressedR();

    void rle();
    void rleRgb();
    void rleRgba();
    void rleDisabled();
    void rleFallbackIfLarger();

    void unsupportedMetadata();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _converterManager{"nonexistent"};
    PluginManager::Manager<AbstractImporter> _importerManager{"nonexistent"};
};

using namespace Math::Literals;

constexpr struct {
    const char* name;
    ImageConverterFlags flags;
    const char* message24;
    const char* message32;
} VerboseData[] {
    {"", {}, "", ""},
    {"verbose", ImageConverterFlag::Verbose,
        "Trade::TgaImageConverter::convertToData(): converting from RGB to BGR\n",
        "Trade::TgaImageConverter::convertToData(): converting from RGBA to BGRA\n"}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Containers::Array<char> data;
    Containers::Array<UnsignedByte> expected;
    Int width;
    Containers::Optional<bool> rleAcrossScanlines;
} RleData[]{
    {"single repeat run", {InPlaceInit, {
            3, 3, 3, 3, 3
        }}, {InPlaceInit, {
            0x80|4, 3
        }}, 5, {}},
    {"single sequence run", {InPlaceInit, {
            2, 7, 6, 5, 4, 37
        }}, {InPlaceInit, {
            0x00|5, 2, 7, 6, 5, 4, 37
        }}, 6, {}},
    {"1x1 pixel", {InPlaceInit, {
            2
        }}, {InPlaceInit, {
            0x00|0, 2
        }}, 1, {}},
    {"two repeats", {InPlaceInit, {
            1, 1, 1, 2, 2, 2, 2, 2
        }}, {InPlaceInit, {
            0x80|2, 1, 0x80|4, 2
        }}, 8, {}},
    {"sequence after a repeat", {InPlaceInit, {
            2, 2, 2, 3, 4, 5, 76
        }}, {InPlaceInit, {
            0x80|2, 2, 0x00|3, 3, 4, 5, 76
        }}, 7, {}},
    {"repeat after a sequence", {InPlaceInit, {
            3, 4, 5, 76, 2, 2, 2
        }}, {InPlaceInit, {
            0x00|3, 3, 4, 5, 76, 0x80|2, 2
        }}, 7, {}},
    {"repeat after a single different pixel", {InPlaceInit, {
            76, 2, 2
        }}, {InPlaceInit, {
            0x00|0, 76, 0x80|1, 2
        }}, 3, {}},
    {"single different pixel after a repeat", {InPlaceInit, {
            2, 2, 76
        }}, {InPlaceInit, {
            0x80|1, 2, 0x00|0, 76
        }}, 3, {}},
    {"repeat across a scanline", {InPlaceInit, {
            2, 4, 4,
            4, 4, 5
        }}, {InPlaceInit, {
            0x00|0, 2, 0x80|1, 4, 0x80|1, 4, 0x00|0, 5
        }}, 3, {}},
    {"repeat across a scanline, single pixel before", {InPlaceInit, {
            2, 3, 4,
            4, 4, 5
        }}, {InPlaceInit, {
            /* Whole first line encoded as a sequence */
            0x00|2, 2, 3, 4,
            0x80|1, 4, 0x00|0, 5
        }}, 3, {}},
    {"repeat across a scanline, single pixel after", {InPlaceInit, {
            2, 4, 4,
            4, 3, 5
        }}, {InPlaceInit, {
            0x00|0, 2, 0x80|1, 4,
            /* Whole second line encoded as a sequence */
            0x00|2, 4, 3, 5
        }}, 3, {}},
    {"repeat across a scanline, non-strict", {InPlaceInit, {
            2, 4, 4,
            4, 4, 5
        }}, {InPlaceInit, {
            0x00|0, 2, 0x80|3, 4, 0x00|0, 5
        }}, 3, true},
    {"sequence across a scanline", {InPlaceInit, {
            2, 2, 3, 4,
            5, 6, 7, 7
        }}, {InPlaceInit, {
            0x80|1, 2, 0x00|1, 3, 4, 0x00|1, 5, 6, 0x80|1, 7
        }}, 4, {}},
    {"sequence across a scanline, single pixel before", {InPlaceInit, {
            2, 2, 2, 4,
            5, 6, 7, 7
        }}, {InPlaceInit, {
            0x80|2, 2, 0x00|0, 4, 0x00|1, 5, 6, 0x80|1, 7
        }}, 4, {}},
    {"sequence across a scanline, single pixel after", {InPlaceInit, {
            2, 2, 3, 4,
            5, 7, 7, 7
        }}, {InPlaceInit, {
            0x80|1, 2, 0x00|1, 3, 4, 0x00|0, 5, 0x80|2, 7
        }}, 4, {}},
    {"sequence across a scanline, non-strict", {InPlaceInit, {
            2, 2, 3, 4,
            5, 6, 7, 7
        }}, {InPlaceInit, {
            0x80|1, 2, 0x00|3, 3, 4, 5, 6, 0x80|1, 7
        }}, 4, true},
    {"repeat & sequence across multiple scanlines, non-strict", {InPlaceInit, {
            2, 2, 2,
            2, 2, 2,
            2, 3, 4,
            5, 6, 7,
            8, 9, 0,
            1, 2, 3
        }}, {InPlaceInit, {
            0x80|6, 2, 0x00|10, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3
        }}, 3, true},
    {"repeat overflow", {InPlaceInit, {
         /* 1  2  3  4  5  6  7  8  9 10 11 12 13 14 16 16 */
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
            6, 6
        }}, {InPlaceInit, {
            0x80|127, 7, 0x80|30, 7, 0x80|2, 6
        }}, 128 + 31 + 3, {}},
    {"sequence overflow", {InPlaceInit, {
         /* 1  2  3  4  5  6  7  8  9 10 11 12 13 14 16 16 */
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,

            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,

            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            6, 6
        }}, {InPlaceInit, {
            0x00|127,
             /* 1  2  3  4  5  6  7  8  9 10 11 12 13 14 16 16 */
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,

                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
            0x00|30,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6,
                7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7,
            0x80|2,
                6
        }}, 128 + 31 + 3, {}},
};

const struct {
    const char* name;
    char data[2];
    Containers::Array<UnsignedByte> expected;
    Containers::Optional<bool> rle;
    Containers::Optional<bool> rleFallbackIfLarger;
    ImageConverterFlags flags;
    const char* message;
} RleFallbackIfLargerData[]{
    {"RLE smaller, verbose", {7, 7}, {InPlaceInit, {
            /* well, not smaller but not larger either, so we pick what's less
               work (which is to not discard all the already-done RLE work) */
            0x80|1, 7
        }}, {}, {}, ImageConverterFlag::Verbose, ""},
    {"RLE smaller, RLE disabled, verbose", {7, 7}, {InPlaceInit, {
            7, 7
        }}, false, {}, ImageConverterFlag::Verbose, ""},
    {"uncompressed smaller", {7, 13}, {InPlaceInit, {
            7, 13
        }}, {}, {}, {}, ""},
    {"uncompressed smaller, verbose", {7, 13}, {InPlaceInit, {
            7, 13
        }}, {}, {}, ImageConverterFlag::Verbose, "Trade::TgaImageConverter::convertToData(): RLE output 1 bytes larger than uncompressed, falling back to uncompressed\n"},
    {"uncompressed smaller, fallback disabled, verbose", {7, 13}, {InPlaceInit, {
            0x00|1, 7, 13
        }}, {}, false, ImageConverterFlag::Verbose, ""},
    {"uncompressed smaller, RLE disabled, verbose", {7, 13}, {InPlaceInit, {
            7, 13
        }}, false, false, ImageConverterFlag::Verbose, ""},
};

const struct {
    const char* name;
    ImageFlags2D imageFlags;
    ImageConverterFlags converterFlags;
    const char* message;
} UnsupportedMetadataData[]{
    {"1D array", ImageFlag2D::Array, {},
        "1D array images are unrepresentable in TGA, saving as a regular 2D image"},
    {"1D array, quiet", ImageFlag2D::Array, ImageConverterFlag::Quiet,
        nullptr}
};

TgaImageConverterTest::TgaImageConverterTest() {
    addTests({&TgaImageConverterTest::wrongFormat});

    addInstancedTests({
        &TgaImageConverterTest::uncompressedRgb,
        &TgaImageConverterTest::uncompressedRgba},
        Containers::arraySize(VerboseData));

    addTests({&TgaImageConverterTest::uncompressedR});

    addInstancedTests({&TgaImageConverterTest::rle},
        Containers::arraySize(RleData));

    addTests({&TgaImageConverterTest::rleRgb,
              &TgaImageConverterTest::rleRgba,
              &TgaImageConverterTest::rleDisabled});

    addInstancedTests({&TgaImageConverterTest::rleFallbackIfLarger},
        Containers::arraySize(RleFallbackIfLargerData));

    addInstancedTests({&TgaImageConverterTest::unsupportedMetadata},
        Containers::arraySize(UnsupportedMetadataData));

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
    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");

    const char data[4]{};
    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!converter->convertToData(ImageView2D{PixelFormat::RG8Unorm, {1, 1}, data}));
    CORRADE_COMPARE(out, "Trade::TgaImageConverter::convertToData(): unsupported pixel format PixelFormat::RG8Unorm\n");
}

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

void TgaImageConverterTest::uncompressedRgb() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    CORRADE_COMPARE(converter->extension(), "tga");
    CORRADE_COMPARE(converter->mimeType(), "image/x-tga");

    converter->setFlags(data.flags);

    /* Disable RLE, that's tested in rle*() instead */
    converter->configuration().setValue("rle", false);

    Containers::String out;
    Containers::Optional<Containers::Array<char>> array;
    {
        Debug redirectOutput{&out};
        array = converter->convertToData(OriginalRGB);
    }
    CORRADE_VERIFY(array);
    CORRADE_COMPARE(out, data.message24);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 1);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedDataRGB),
        TestSuite::Compare::Container);
}

/* Padding / skip tested in uncompressedRgb() */
constexpr char OriginalDataRGBA[] = {
    1, 2, 3, 4, 2, 3, 4, 5,
    3, 4, 5, 6, 4, 5, 6, 7,
    5, 6, 7, 8, 6, 7, 8, 9
};
const ImageView2D OriginalRGBA{PixelFormat::RGBA8Unorm, {2, 3}, OriginalDataRGBA};

void TgaImageConverterTest::uncompressedRgba() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->setFlags(data.flags);

    /* Disable RLE, that's tested in rle*() instead */
    converter->configuration().setValue("rle", false);

    Containers::String out;
    Containers::Optional<Containers::Array<char>> array;
    {
        Debug redirectOutput{&out};
        array = converter->convertToData(OriginalRGBA);
    }
    CORRADE_VERIFY(array);
    CORRADE_COMPARE(out, data.message32);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 4);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(OriginalDataRGBA),
        TestSuite::Compare::Container);
}

/* Padding / skip tested in uncompressedRgb() */
constexpr char OriginalDataR[] = {
    1, 2,
    3, 4,
    5, 6,
};
const ImageView2D OriginalR{PixelStorage{}.setAlignment(1), PixelFormat::R8Unorm, {2, 3}, OriginalDataR};

void TgaImageConverterTest::uncompressedR() {
    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");

    /* Disable RLE, that's tested in rle*() instead */
    converter->configuration().setValue("rle", false);

    Containers::Optional<Containers::Array<char>> array = converter->convertToData(OriginalR);
    CORRADE_VERIFY(array);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->storage().alignment(), 1);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(OriginalDataR),
        TestSuite::Compare::Container);
}

void TgaImageConverterTest::rle() {
    auto&& data = RleData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_COMPARE_AS(data.data.size(), data.width,
        TestSuite::Compare::Divisible);
    Vector2i size{data.width, Int(data.data.size()/data.width)};
    /* Skip/alignment handling tested in rleRgb() */
    ImageView2D image{PixelStorage{}.setAlignment(1), PixelFormat::R8Unorm, size, data.data};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    if(data.rleAcrossScanlines)
        converter->configuration().setValue("rleAcrossScanlines", *data.rleAcrossScanlines);
    /* Force RLE to be used even if larger than uncompressed. This behavior is
       tested in rleFallbackIfLarger() instead. */
    converter->configuration().setValue("rleFallbackIfLarger", false);

    Containers::Optional<Containers::Array<char>> array = converter->convertToData(image);
    CORRADE_VERIFY(array);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<const UnsignedByte>(*array)
            .exceptPrefix(sizeof(Implementation::TgaHeader)),
        data.expected,
        TestSuite::Compare::Container);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), size);
    CORRADE_COMPARE(converted->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(converted->data(),
        data.data,
        TestSuite::Compare::Container);
}

void TgaImageConverterTest::rleRgb() {
    Color3ub data[]{
        {}, {}, {}, {},
        /* Three different pixels, differing always in only one component */
        0x0000ff_rgb, 0x0000ef_rgb, 0x0100ef_rgb, {},
        /* One different and two same pixels */
        0x0100ef_rgb, 0xaabbcc_rgb, 0xaabbcc_rgb, {},
    };

    ImageView2D image{PixelStorage{}.setRowLength(4).setSkip({0, 1, 0}), PixelFormat::RGB8Unorm, {3, 2}, data};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    Containers::Optional<Containers::Array<char>> array = converter->convertToData(image);
    CORRADE_VERIFY(array);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<const UnsignedByte>(*array)
            .exceptPrefix(sizeof(Implementation::TgaHeader)),
        Containers::arrayView<UnsignedByte>({
            /* Swizzled to BGR */
            0x00|2, 0xff, 0x00, 0x00,
                    0xef, 0x00, 0x00,
                    0xef, 0x00, 0x01,
            /* No runs across rows by default */
            0x00|0, 0xef, 0x00, 0x01,
            0x80|1, 0xcc, 0xbb, 0xaa,
        }),
        TestSuite::Compare::Container);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), (Vector2i{3, 2}));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayCast<const char>(Containers::arrayView({
        0x0000ff_rgb, 0x0000ef_rgb, 0x0100ef_rgb,
        0x0100ef_rgb, 0xaabbcc_rgb, 0xaabbcc_rgb,
    })), TestSuite::Compare::Container);
}

const Color4ub RleRgbaData[]{
    /* Four different pixels, differing always in only one component */
    0x0000ffff_rgba, 0x0000efff_rgba, 0x0100efff_rgba, 0x0100effe_rgba,
    /* One different and three same pixels */
    0x0100effe_rgba, 0xaabbccdd_rgba, 0xaabbccdd_rgba, 0xaabbccdd_rgba
};

void TgaImageConverterTest::rleRgba() {
    ImageView2D image{PixelFormat::RGBA8Unorm, {4, 2}, RleRgbaData};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    Containers::Optional<Containers::Array<char>> array = converter->convertToData(image);
    CORRADE_VERIFY(array);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<const UnsignedByte>(*array)
            .exceptPrefix(sizeof(Implementation::TgaHeader)),
        Containers::arrayView<UnsignedByte>({
            /* Swizzled to BGRA */
            0x00|3, 0xff, 0x00, 0x00, 0xff,
                    0xef, 0x00, 0x00, 0xff,
                    0xef, 0x00, 0x01, 0xff,
                    0xef, 0x00, 0x01, 0xfe,
            /* No runs across rows by default */
            0x00|0, 0xef, 0x00, 0x01, 0xfe,
            0x80|2, 0xcc, 0xbb, 0xaa, 0xdd
        }),
        TestSuite::Compare::Container);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), (Vector2i{4, 2}));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(converted->data(),
        Containers::arrayCast<const char>(RleRgbaData),
        TestSuite::Compare::Container);
}

void TgaImageConverterTest::rleDisabled() {
    ImageView2D image{PixelFormat::RGBA8Unorm, {4, 2}, RleRgbaData};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->configuration().setValue("rle", false);

    const Containers::Optional<Containers::Array<char>> array = converter->convertToData(image);
    CORRADE_VERIFY(array);
    CORRADE_COMPARE_AS(array->exceptPrefix(sizeof(Implementation::TgaHeader)),Containers::arrayCast<const char>(Containers::arrayView({
        /* Swizzled to BGRA */
        0xff0000ff_rgba, 0xef0000ff_rgba, 0xef0001ff_rgba, 0xef0001fe_rgba,
        0xef0001fe_rgba, 0xccbbaadd_rgba, 0xccbbaadd_rgba, 0xccbbaadd_rgba
    })), TestSuite::Compare::Container);

    /* No need to verify a roundtrip, that's tested enough in uncompressed*() */
}

void TgaImageConverterTest::rleFallbackIfLarger() {
    auto&& data = RleFallbackIfLargerData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Skip/alignment handling tested in rleRgb() */
    ImageView2D image{PixelStorage{}.setAlignment(1), PixelFormat::R8Unorm, {2, 1}, data.data};

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->setFlags(data.flags);
    if(data.rle)
        converter->configuration().setValue("rle", *data.rle);
    if(data.rleFallbackIfLarger)
        converter->configuration().setValue("rleFallbackIfLarger", *data.rleFallbackIfLarger);

    Containers::String out;
    Containers::Optional<Containers::Array<char>> array;
    {
        Debug redirectOutput{&out};
        array = converter->convertToData(image);
    }
    CORRADE_VERIFY(array);
    CORRADE_COMPARE_AS(
        Containers::arrayCast<const UnsignedByte>(*array)
            .exceptPrefix(sizeof(Implementation::TgaHeader)),
        data.expected,
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out, data.message);

    if(!(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("TgaImporter plugin not enabled, can't test the result");

    Containers::Pointer<AbstractImporter> importer = _importerManager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(*array));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), (Vector2i{2, 1}));
    CORRADE_COMPARE(converted->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(converted->data(),
        Containers::arrayView(data.data),
        TestSuite::Compare::Container);
}

void TgaImageConverterTest::unsupportedMetadata() {
    auto&& data = UnsupportedMetadataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImageConverter> converter = _converterManager.instantiate("TgaImageConverter");
    converter->setFlags(data.converterFlags);

    const char imageData[4]{};
    ImageView2D image{PixelFormat::RGBA8Unorm, {1, 1}, imageData, data.imageFlags};

    Containers::String out;
    Warning redirectWarning{&out};
    CORRADE_VERIFY(converter->convertToData(image));
    if(!data.message)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, Utility::format("Trade::TgaImageConverter::convertToData(): {}\n", data.message));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImageConverterTest)
