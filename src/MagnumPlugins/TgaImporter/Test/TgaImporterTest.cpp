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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct TgaImporterTest: TestSuite::Tester {
    explicit TgaImporterTest();

    void openEmpty();
    void openShort();

    void paletted();
    void invalid();
    void unsupportedBits();

    void color24();
    void color24Rle();
    void color32();
    void color32Rle();
    void grayscale8();
    void grayscale8Rle();

    void rleTooLarge();

    void openTwice();
    void importTwice();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImporter> _manager{"nonexistent"};
};

constexpr struct {
    const char* name;
    char imageType;
    char bpp;
    const char* message;
} UnsupportedBitsData[] {
    {"color 16", 2, 16, "unsupported color bits-per-pixel: 16"},
    {"grayscale 16", 3, 16, "unsupported grayscale bits-per-pixel: 16"},
    {"RLE color 16", 10, 16, "unsupported color bits-per-pixel: 16"},
    {"RLE grayscale 16", 11, 16, "unsupported grayscale bits-per-pixel: 16"}
};

constexpr struct {
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

/* MSVC 2015 crashes when seeing constexpr here. Not doing that, then. */
const struct {
    const char* name;
    Containers::ArrayView<const char> data;
    const char* message;
} ShortData[] {
    {"short header", Containers::arrayView(Color24).prefix(17),
        "file too short, expected at least 18 bytes but got 17"},
    {"short data", Containers::arrayView(Color24).except(1),
        "file too short, expected 36 bytes but got 35"},
    {"short RLE data", Containers::arrayView(Color24Rle).except(1),
        "RLE file too short at pixel 3"},
    {"short RLE raw data", Containers::arrayView(Color24Rle).except(5),
        "RLE file too short at pixel 0"}
};

TgaImporterTest::TgaImporterTest() {
    addTests({&TgaImporterTest::openEmpty});

    addInstancedTests({&TgaImporterTest::openShort},
        Containers::arraySize(ShortData));

    addTests({&TgaImporterTest::paletted,
              &TgaImporterTest::invalid});

    addInstancedTests({
        &TgaImporterTest::unsupportedBits},
        Containers::arraySize(UnsupportedBitsData));

    addInstancedTests({
        &TgaImporterTest::color24,
        &TgaImporterTest::color24Rle,
        &TgaImporterTest::color32,
        &TgaImporterTest::color32Rle},
        Containers::arraySize(VerboseData));

    addTests({&TgaImporterTest::grayscale8,
              &TgaImporterTest::grayscale8Rle,

              &TgaImporterTest::rleTooLarge});

    addTests({&TgaImporterTest::openTwice,
              &TgaImporterTest::importTwice});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void TgaImporterTest::openEmpty() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    std::ostringstream out;
    Error redirectError{&out};
    char a{};
    /* Explicitly checking non-null but empty view */
    CORRADE_VERIFY(!importer->openData({&a, 0}));
    CORRADE_COMPARE(out.str(), "Trade::TgaImporter::openData(): the file is empty\n");
}

void TgaImporterTest::openShort() {
    auto&& data = ShortData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    CORRADE_VERIFY(importer->openData(data.data));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out.str(), Utility::formatString("Trade::TgaImporter::image2D(): {}\n", data.message));
}

void TgaImporterTest::paletted() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char data[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CORRADE_VERIFY(importer->openData(data));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out.str(), "Trade::TgaImporter::image2D(): paletted files are not supported\n");
}

void TgaImporterTest::invalid() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char data[] = { 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CORRADE_VERIFY(importer->openData(data));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out.str(), "Trade::TgaImporter::image2D(): unsupported image type: 9\n");
}

void TgaImporterTest::unsupportedBits() {
    auto&& data = UnsupportedBitsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char imageData[] = {
        0, 0, data.imageType, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, data.bpp, 0
    };
    CORRADE_VERIFY(importer->openData(imageData));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out.str(), Utility::formatString("Trade::TgaImporter::image2D(): {}\n", data.message));
}

void TgaImporterTest::color24() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    const char pixels[] = {
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        7, 6, 5, 8, 7, 6
    };
    CORRADE_VERIFY(importer->openData(Color24));

    std::ostringstream out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message24);
}

void TgaImporterTest::color24Rle() {
    auto&& data = VerboseData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    importer->setFlags(data.flags);
    const char pixels[] = {
        3, 2, 1, 4, 3, 2,
        5, 4, 3, 6, 5, 4,
        6, 5, 4, 6, 5, 4
    };
    CORRADE_VERIFY(importer->openData(Color24Rle));

    std::ostringstream out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message24);
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
    const char pixels[] = {
        3, 2, 1, 4, 4, 3, 2, 5,
        5, 4, 3, 6, 6, 5, 4, 7,
        7, 6, 5, 8, 8, 7, 6, 9
    };
    CORRADE_VERIFY(importer->openData(input));

    std::ostringstream out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message32);
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
    const char pixels[] = {
        3, 2, 1, 4, 3, 2, 1, 4,
        5, 4, 3, 6, 6, 5, 4, 7,
        7, 6, 5, 8, 8, 7, 6, 9
    };
    CORRADE_VERIFY(importer->openData(input));

    std::ostringstream out;
    Containers::Optional<Trade::ImageData2D> image;
    {
        Debug redirectOutput{&out};
        image = importer->image2D(0);
    }
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 4);
    CORRADE_COMPARE(image->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(out.str(), data.message32);
}

void TgaImporterTest::grayscale8() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char data[] = {
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 8, 0,
        1, 2,
        3, 4,
        5, 6
    };
    const char pixels[] {
        1, 2,
        3, 4,
        5, 6
    };
    CORRADE_VERIFY(importer->openData(data));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
}

void TgaImporterTest::grayscale8Rle() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char data[] = {
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
    const char pixels[] {
        1, 2,
        3, 3,
        5, 6
    };
    CORRADE_VERIFY(importer->openData(data));

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->storage().alignment(), 1);
    CORRADE_COMPARE(image->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE(image->size(), Vector2i(2, 3));
    CORRADE_COMPARE_AS(image->data(), Containers::arrayView(pixels),
        TestSuite::Compare::Container);
}

void TgaImporterTest::rleTooLarge() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    const char data[] = {
        0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 24, 0,
        /* 3 pixels as-is */
        '\x02', 1, 2, 3,
                2, 3, 4,
                3, 4, 5,
        /* 1 pixel 4x repeated (one more than it should be) */
        '\x83', 4, 5, 6
    };
    CORRADE_VERIFY(importer->openData(data));

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!importer->image2D(0));
    CORRADE_COMPARE(out.str(), "Trade::TgaImporter::image2D(): RLE data larger than advertised Vector(2, 3) pixels at byte 28\n");
}

void TgaImporterTest::openTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(TGAIMPORTER_TEST_DIR, "file.tga")));
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(TGAIMPORTER_TEST_DIR, "file.tga")));

    /* Shouldn't crash, leak or anything */
}

void TgaImporterTest::importTwice() {
    Containers::Pointer<AbstractImporter> importer = _manager.instantiate("TgaImporter");
    CORRADE_VERIFY(importer->openFile(Utility::Directory::join(TGAIMPORTER_TEST_DIR, "file.tga")));

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
