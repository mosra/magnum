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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct TgaImporterTest: TestSuite::Tester {
    explicit TgaImporterTest();

    void invalidEmpty();
    void invalidShort();
    void invalid();
    void invalidBits();

    void color24();
    void color24Rle();
    void color32();
    void color32Rle();
    void grayscale8();
    void grayscale8Rle();

    void tga2();
    void fileTooLong();

    void openMemory();
    void openTwice();
    void importTwice();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr const char Grayscale8[]{
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 8, 0,
    1, 2,
    3, 4,
    5, 6,
};

constexpr const char Grayscale8Rle[]{
    0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 8, 0,
    /* 2 pixels as-is */
    '\x01', 1, 2,
    /* 1 pixel 2x repeated */
    '\x81', 3,
    /* 1 pixel as-is */
    '\x00', 5,
    /* 1 pixel 1x repeated */
    '\x00', 6
};

constexpr const char Color24[] = {
    0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
    1, 2, 3, 2, 3, 4,
    3, 4, 5, 4, 5, 6,
    5, 6, 7, 6, 7, 8
};

constexpr const char Color24Rle[] = {
    0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
    /* 3 pixels as-is */
    '\x02', 1, 2, 3,
            2, 3, 4,
            3, 4, 5,
    /* 1 pixel 3x repeated */
    '\x82', 4, 5, 6
};

/* Separate from ErrorData so we can just slice existing arrays instead of
   creating new ones from scratch */
const struct {
    const char* name;
    Containers::ArrayView<const char> data;
    const char* message;
} InvalidShortData[] {
    {"short header", Containers::arrayView(Color24).prefix(17),
        "file too short, expected at least 18 bytes but got 17"},
    {"short data", Containers::arrayView(Color24).exceptSuffix(1),
        "file too short, expected 36 bytes but got 35"},
    {"short RLE data", Containers::arrayView(Color24Rle).exceptSuffix(1),
        "RLE file too short at pixel 3"},
    {"short RLE raw data", Containers::arrayView(Color24Rle).exceptSuffix(5),
        "RLE file too short at pixel 0"}
};

const struct {
    const char* name;
    char imageType;
    char bpp;
    const char* message;
} InvalidBitsData[] {
    {"color 16", 2, 16, "unsupported color bits-per-pixel: 16"},
    {"grayscale 16", 3, 16, "unsupported grayscale bits-per-pixel: 16"},
    {"RLE color 16", 10, 16, "unsupported color bits-per-pixel: 16"},
    {"RLE grayscale 16", 11, 16, "unsupported grayscale bits-per-pixel: 16"}
};

const struct {
    const char* name;
    Containers::Array<char> data;
    const char* message;
} InvalidData[]{
    {"paletted", {InPlaceInit, {
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }}, "paletted files are not supported"},
    {"invalid image type", {InPlaceInit, {
        0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }}, "unsupported image type: 9"},
    {"TGA 2 file too short", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, /* One byte for the sizes missing here */
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 file too short, expected at least 44 bytes but got 43"},
    {"TGA 2 extension offset overlaps with file header", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        17, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 extension offset 17 overlaps with file header"},
    {"TGA 2 extension offset overlaps with file footer", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        19, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 extension offset 19 out of range for 44 bytes and a 26-byte file footer"},
    {"TGA 2 developer area offset overlaps with file header", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        0, 0, 0, 0, 17, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 developer area offset 17 overlaps with file header"},
    {"TGA 2 developer area offset overlaps with file footer", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        0, 0, 0, 0, 19, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 developer area offset 19 out of range for 44 bytes and a 26-byte file footer"},
    {"TGA 2 developer area offset overlaps with extension area", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        '\xdd', '\xee', '\xee',
        19, 0, 0, 0, 18, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, "TGA 2 developer area offset 18 overlaps with extensions at 19 bytes"},
    {"RLE too large", {InPlaceInit, {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        /* 3 pixels as-is */
        '\x02', 1, 2, 3,
                2, 3, 4,
                3, 4, 5,
        /* 1 pixel 4x repeated (one more than it should be) */
        '\x83', 4, 5, 6
    }}, "RLE data at byte 28 contains 4 pixels but only 3 left to decode"},
};

const struct {
    const char* name;
    ImporterFlags flags;
    const char* message24;
    const char* message32;
} VerboseData[] {
    {"", {}, "", ""},
    {"verbose", ImporterFlag::Verbose,
        "Trade::TgaImporter::image2D(): converting from BGR to RGB\n",
        "Trade::TgaImporter::image2D(): converting from BGRA to RGBA\n"}
};

/* Tga2Data footer offsets rely on this */
static_assert(sizeof(Grayscale8Rle) == 27, "size of grayscale data not 27 bytes");
const struct {
    const char* name;
    Containers::Array<char> footer;
} Tga2Data[]{
    {"just the footer", {InPlaceInit, {
        0, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"extension", {InPlaceInit, {
        '\xee', '\xee',
        27, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"developer area", {InPlaceInit, {
        '\xdd', '\xdd',
        0, 0, 0, 0, 27, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"both extension and developer area", {InPlaceInit, {
        '\xee', '\xee', '\xee', '\xdd', '\xdd', '\xdd', '\xdd', '\xdd',
        27, 0, 0, 0, 30, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"empty extension area", {InPlaceInit, {
        27, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"empty developer area", {InPlaceInit, {
        0, 0, 0, 0, 27, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
    {"empty extension and developer area", {InPlaceInit, {
        27, 0, 0, 0, 27, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}},
};

const struct {
    const char* name;
    Containers::Array<char> extra;
    ImporterFlags flags;
    bool quiet;
} FileTooLongData[]{
    {"", {InPlaceInit, {
        'e', 'x', 't', 'r', 'a'
    }}, {}, false},
    {"TGA 2", {InPlaceInit, {
        'e', 'x', 't', 'r', 'a',
        0, 0, 0, 0, 0, 0, 0, 0,
        'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
    }}, ImporterFlag::Quiet, true},
};

/* Shared among all plugins that implement data copying optimizations */
const struct {
    const char* name;
    bool(*open)(AbstractImporter&, Containers::ArrayView<const void>);
} OpenMemoryData[]{
    {"data", [](AbstractImporter& importer, Containers::ArrayView<const void> data) {
        /* Copy to ensure the original memory isn't referenced */
        Containers::Array<char> copy{NoInit, data.size()};
        Utility::copy(Containers::arrayCast<const char>(data), copy);
        return importer.openData(copy);
    }},
    {"memory", [](AbstractImporter& importer, Containers::ArrayView<const void> data) {
        return importer.openMemory(data);
    }},
};

TgaImporterTest::TgaImporterTest() {
    addTests({&TgaImporterTest::invalidEmpty});

    addInstancedTests({&TgaImporterTest::invalidShort},
        Containers::arraySize(InvalidShortData));

    addInstancedTests({&TgaImporterTest::invalid},
        Containers::arraySize(InvalidData));

    addInstancedTests({&TgaImporterTest::invalidBits},
        Containers::arraySize(InvalidBitsData));

    addInstancedTests({
        &TgaImporterTest::color24,
        &TgaImporterTest::color24Rle,
        &TgaImporterTest::color32,
        &TgaImporterTest::color32Rle},
        Containers::arraySize(VerboseData));

    addTests({&TgaImporterTest::grayscale8,
              &TgaImporterTest::grayscale8Rle});

    addInstancedTests({&TgaImporterTest::tga2},
        Containers::arraySize(Tga2Data));

    addInstancedTests({&TgaImporterTest::fileTooLong},
        Containers::arraySize(FileTooLongData));

    addInstancedTests({&TgaImporterTest::openMemory},
        Containers::arraySize(OpenMemoryData));

    addTests({&TgaImporterTest::openTwice,
              &TgaImporterTest::importTwice});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void TgaImporterTest::invalidEmpty() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    Containers::String out;
    Error redirectError{&out};
    char a{};
    /* Explicitly checking non-null but empty view */
    CORRADE_VERIFY(!importer->openData({&a, 0}));
    CORRADE_COMPARE(out, "Trade::TgaImporter::openData(): the file is empty\n");
}

void TgaImporterTest::invalidShort() {
    auto&& data = InvalidShortData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    CORRADE_VERIFY(importer->openData(data.data));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out, Utility::format("Trade::TgaImporter::image2D(): {}\n", data.message));
}

void TgaImporterTest::invalid() {
    auto&& data = InvalidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    CORRADE_VERIFY(importer->openData(data.data));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out, Utility::format("Trade::TgaImporter::image2D(): {}\n", data.message));
}

void TgaImporterTest::invalidBits() {
    auto&& data = InvalidBitsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char imageData[]{
        0, 0, data.imageType, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, data.bpp, 0
    };
    CORRADE_VERIFY(importer->openData(imageData));

    Containers::String out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out, Utility::format("Trade::TgaImporter::image2D(): {}\n", data.message));
}

void TgaImporterTest::color24() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    CORRADE_VERIFY(importer->openData(Color24));

    Containers::String out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        7, 6, 5, 8, 7, 6
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out, data.message24);
}

void TgaImporterTest::color24Rle() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    CORRADE_VERIFY(importer->openData(Color24Rle));

    Containers::String out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        6, 5, 4, 6, 5, 4
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out, data.message24);
}

void TgaImporterTest::color32() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    const char input[] = {
        0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        1, 2, 3, 4, 2, 3, 4, 5,
        3, 4, 5, 6, 4, 5, 6, 7,
        5, 6, 7, 8, 6, 7, 8, 9
    };
    CORRADE_VERIFY(importer->openData(input));

    Containers::String out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        3, 2, 1, 4, 4, 3, 2, 5,
        5, 4, 3, 6, 6, 5, 4, 7,
        7, 6, 5, 8, 8, 7, 6, 9
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out, data.message32);
}

void TgaImporterTest::color32Rle() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    const char input[] = {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 32, 0,
        /* 2 pixels repeated */
        '\x81', 1, 2, 3, 4,
        /* 4 pixels as-is */
        '\x03', 3, 4, 5, 6,
                4, 5, 6, 7,
                5, 6, 7, 8,
                6, 7, 8, 9
    };
    CORRADE_VERIFY(importer->openData(input));

    Containers::String out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        3, 2, 1, 4, 3, 2, 1, 4,
        5, 4, 3, 6, 6, 5, 4, 7,
        7, 6, 5, 8, 8, 7, 6, 9
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(out, data.message32);
}

void TgaImporterTest::grayscale8() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(Grayscale8));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        1, 2,
        3, 4,
        5, 6
    }), TestSuite::Compare::Container);
}

void TgaImporterTest::grayscale8Rle() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openData(Grayscale8Rle));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        1, 2,
        3, 3,
        5, 6
    }), TestSuite::Compare::Container);
}

void TgaImporterTest::tga2() {
    auto&& data = Tga2Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    /* The actual image data is always the same, only the footer differs */
    CORRADE_VERIFY(importer->openData(
        Containers::StringView{Containers::arrayView(Grayscale8Rle)} +
        Containers::StringView{data.footer}));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        1, 2,
        3, 3,
        5, 6
    }), TestSuite::Compare::Container);
}

void TgaImporterTest::fileTooLong() {
    auto&& data = FileTooLongData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);

    /* The actual image data is always the same, only the end differs */
    CORRADE_VERIFY(importer->openData(
        Containers::StringView{Containers::arrayView(Grayscale8)} +
        Containers::StringView{data.extra}));

    Containers::Optional<Trade::ImageData2D> image;
    Containers::String out;
    {
        Warning redirectWarning{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        1, 2,
        3, 4,
        5, 6
    }), TestSuite::Compare::Container);
    if(data.quiet)
        CORRADE_COMPARE(out, "");
    else
        CORRADE_COMPARE(out, "Trade::TgaImporter::image2D(): ignoring 5 extra bytes at the end of image data\n");
}

void TgaImporterTest::openMemory() {
    /* same as color24() except that it uses openData() & openMemory() to test
       data copying on import */

    auto&& data = OpenMemoryData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    /* Eh fuck off, GCC, why can't you convert the array to an ArrayView on
       your own if it's passed to a function pointer?! Clang can. */
    CORRADE_VERIFY(data.open(*importer, Containers::arrayView(Color24)));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->flags(), ImageFlags2D{});
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView<char>({
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        7, 6, 5, 8, 7, 6
    }), TestSuite::Compare::Container);
}

void TgaImporterTest::openTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(TGAIMPORTER_TEST_DIR, "file.tga")));
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(TGAIMPORTER_TEST_DIR, "file.tga")));

    /* Shouldn't crash, leak or anything */
}

void TgaImporterTest::importTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Path::join(TGAIMPORTER_TEST_DIR, "file.tga")));

    /* Verify that everything is working the same way on second use */
    {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->size(), (Vector2i{2, 3}));
    } {
        Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
        CORRADE_VERIFY(image);
        CORRADE_COMPARE(image->size(), (Vector2i{2, 3}));
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TgaImporterTest)
