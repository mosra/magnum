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
#include <numeric>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct CompareImageTest: TestSuite::Tester {
    explicit CompareImageTest();

    void formatUnknown();
    void formatHalf();
    void formatImplementationSpecific();

    void calculateDelta();
    void calculateDeltaStorage();

    void deltaImage();
    void deltaImageScaling();
    void deltaImageColors();

    void pixelDelta();
    void pixelDeltaOverflow();

    void compareDifferentSize();
    void compareDifferentFormat();
    void compareSameZeroThreshold();
    void compareAboveThresholds();
    void compareAboveMaxThreshold();
    void compareAboveMeanThreshold();

    void setupExternalPluginManager();
    void teardownExternalPluginManager();

    void image();
    void imageError();
    void imageFile();
    void imageFileError();
    void imageFilePluginLoadFailed();
    void imageFileActualLoadFailed();
    void imageFileExpectedLoadFailed();
    void imageFileActualIsCompressed();
    void imageFileExpectedIsCompressed();
    void imageToFile();
    void imageToFileError();
    void imageToFilePluginLoadFailed();
    void imageToFileExpectedLoadFailed();
    void imageToFileExpectedIsCompressed();
    void fileToImage();
    void fileToImageError();
    void fileToImagePluginLoadFailed();
    void fileToImageActualLoadFailed();
    void fileToImageActualIsCompressed();

    private:
        Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> _manager;
};

CompareImageTest::CompareImageTest() {
    addTests({&CompareImageTest::formatUnknown,
              &CompareImageTest::formatHalf,
              &CompareImageTest::formatImplementationSpecific,

              &CompareImageTest::calculateDelta,
              &CompareImageTest::calculateDeltaStorage,

              &CompareImageTest::deltaImage,
              &CompareImageTest::deltaImageScaling,
              &CompareImageTest::deltaImageColors,

              &CompareImageTest::pixelDelta,
              &CompareImageTest::pixelDeltaOverflow,

              &CompareImageTest::compareDifferentSize,
              &CompareImageTest::compareDifferentFormat,
              &CompareImageTest::compareSameZeroThreshold,
              &CompareImageTest::compareAboveThresholds,
              &CompareImageTest::compareAboveMaxThreshold,
              &CompareImageTest::compareAboveMeanThreshold,

              &CompareImageTest::image,
              &CompareImageTest::imageError});

    addTests({&CompareImageTest::imageFile,
              &CompareImageTest::imageFileError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageFilePluginLoadFailed});

    addTests({&CompareImageTest::imageFileActualLoadFailed,
              &CompareImageTest::imageFileExpectedLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageFileActualIsCompressed,
              &CompareImageTest::imageFileExpectedIsCompressed});

    addTests({&CompareImageTest::imageToFile,
              &CompareImageTest::imageToFileError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageToFilePluginLoadFailed});

    addTests({&CompareImageTest::imageToFileExpectedLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageToFileExpectedIsCompressed});

    addTests({&CompareImageTest::fileToImage,
              &CompareImageTest::fileToImageError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::fileToImagePluginLoadFailed});

    addTests({&CompareImageTest::fileToImageActualLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::fileToImageActualIsCompressed});
}

const Float ActualRedData[] = {
        0.3f, 1.0f, 0.9f,
        0.9f, 0.6f, 0.2f,
    -0.1f, 1.0f, 0.0f
};

const Float ExpectedRedData[] = {
    0.65f, 1.0f, 0.6f,
    0.91f, 0.6f, 0.1f,
    0.02f, 0.0f, 0.0f
};

const Float DeltaRed[]{
    0.35f, 0.0f, 0.3f,
    0.01f, 0.0f, 0.1f,
    0.12f, 1.0f, 0.0f
};

const ImageView2D ActualRed{PixelFormat::R32F, {3, 3}, ActualRedData};
const ImageView2D ExpectedRed{PixelFormat::R32F, {3, 3}, ExpectedRedData};

void CompareImageTest::formatUnknown() {
    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, PixelFormat(0xdead), 0, 0, {}};
    Implementation::calculateImageDelta(image, image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: unknown format PixelFormat(0xdead)\n");
}

void CompareImageTest::formatHalf() {
    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelFormat::RG16F, {}};
    Implementation::calculateImageDelta(image, image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: half-float formats are not supported yet\n");
}

void CompareImageTest::formatImplementationSpecific() {
    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, pixelFormatWrap(0xdead), 0, 0, {}};
    Implementation::calculateImageDelta(image, image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: can't compare implementation-specific pixel formats\n");
}

void CompareImageTest::calculateDelta() {
    Containers::Array<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRed, ExpectedRed);

    CORRADE_COMPARE_AS(delta, Containers::arrayView(DeltaRed), TestSuite::Compare::Container);
    CORRADE_COMPARE(max, 1.0f);
    CORRADE_COMPARE(mean, 0.208889f);
}

/* Different storage for each */
const UnsignedByte ActualRgbData[] = {
       0,    0,    0,    0,    0,    0,    0,    0,
    0x56, 0xf8, 0x3a, 0x56, 0x47, 0xec,    0,    0,
    0x23, 0x57, 0x10, 0xab, 0xcd, 0x85,    0,    0
};

const UnsignedByte ExpectedRgbData[] = {
    0, 0, 0, 0x55, 0xf8, 0x3a, 0x56, 0x10, 0xed, 0, 0, 0,
    0, 0, 0, 0x23, 0x27, 0x10, 0xab, 0xcd, 0xfa, 0, 0, 0
};

const ImageView2D ActualRgb{PixelStorage{}.setSkip({0, 1, 0}),
    PixelFormat::RGB8Unorm, {2, 2}, ActualRgbData};
const ImageView2D ExpectedRgb{
    PixelStorage{}.setSkip({1, 0, 0}).setRowLength(3),
    PixelFormat::RGB8Unorm, {2, 2}, ExpectedRgbData};

void CompareImageTest::calculateDeltaStorage() {
    Containers::Array<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRgb, ExpectedRgb);

    CORRADE_COMPARE_AS(delta, (Containers::Array<Float>{Containers::InPlaceInit, {
        1.0f/3.0f, (55.0f + 1.0f)/3.0f,
        48.0f/3.0f, 117.0f/3.0f
    }}), TestSuite::Compare::Container);
    CORRADE_COMPARE(max, 117.0f/3.0f);
    CORRADE_COMPARE(mean, 18.5f);
}

void CompareImageTest::deltaImage() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};

    Containers::Array<Float> delta{32*32};

    for(std::int_fast32_t x = 0; x != 32; ++x)
        for(std::int_fast32_t y = 0; y != 32; ++y)
            delta[y*32 + x] = Vector2{Float(x), Float(y)}.length()/Vector2{32.0f}.length();

    Implementation::printDeltaImage(d, delta, {32, 32}, 1.0f, 0.0f, 0.0f);
    CORRADE_COMPARE(out.str(),
        "          |$$$$$$$$$$0000000888888DDDDNNNNM|\n"
        "          |ZZZZZZZ$$$$$$$$0000008888DDDDNNN|\n"
        "          |ZZZZZZZZZZZZZ$$$$$$00008888DDDDN|\n"
        "          |IIIIIIIIIIZZZZZZZ$$$$00008888DDD|\n"
        "          |7777777IIIIIIIZZZZZ$$$$00008888D|\n"
        "          |???777777777IIIIIZZZZ$$$$0000888|\n"
        "          |??????????77777IIIIZZZZ$$$$00088|\n"
        "          |+++++++??????7777IIIIZZZZ$$$0008|\n"
        "          |=====++++++????7777IIIIZZZ$$$000|\n"
        "          |=========++++????7777IIIZZZ$$$00|\n"
        "          |~~~~~~~====++++????777IIIZZZ$$$0|\n"
        "          |:::::~~~~====++++???777IIIZZZ$$$|\n"
        "          |,::::::~~~~===+++????77IIIZZZ$$$|\n"
        "          |,,,,,::::~~~===+++???777IIIZZZ$$|\n"
        "          |...,,,,:::~~~===+++??777IIIZZZ$$|\n"
        "          | ....,,:::~~~===+++???777IIZZZ$$|\n");
}

void CompareImageTest::deltaImageScaling() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};

    Containers::Array<Float> delta{65*40};
    for(std::int_fast32_t x = 0; x != 65; ++x)
        for(std::int_fast32_t y = 0; y != 40; ++y)
            delta[y*65 + x] = Vector2{Float(x), Float(y)}.length()/Vector2{65.0f, 40.0f}.length();

    Implementation::printDeltaImage(d, delta, {65, 40}, 1.0f, 0.0f, 0.0f);
    CORRADE_COMPARE(out.str(),
        "          |777777IIIIIIZZZZ$$$0000888DDDNNMM|\n"
        "          |????777777IIIIZZZZ$$$000888DDDNNN|\n"
        "          |?????????7777IIIIZZZ$$$00888DDDNN|\n"
        "          |++++++++????777IIIZZZ$$$00088DDDN|\n"
        "          |======++++????777IIIZZ$$$00088DDD|\n"
        "          |~~~~~====+++???777IIIZZ$$$00888DD|\n"
        "          |::::~~~~===+++??777IIZZZ$$00088DD|\n"
        "          |,,::::~~~===++???777IIZZ$$$00888D|\n"
        "          |.,,,,:::~~===++???77IIZZZ$$000888|\n"
        "          |...,,,::~~~==++???77IIIZZ$$000888|\n");
}

void CompareImageTest::deltaImageColors() {
    /* Print for visual color verification */
    {
        Debug() << "Visual verification -- some letters should be yellow, some red, some white:";
        Debug d{Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printDeltaImage(d, DeltaRed, {3, 3}, 2.0f, 0.5f, 0.2f);
    }

    std::ostringstream out;
    Debug dc{&out, Debug::Flag::DisableColors};
    Implementation::printDeltaImage(dc, DeltaRed, {3, 3}, 2.0f, 0.5f, 0.2f);
    /* Yes, there is half of the rows (2 instead of 3) in order to roughly
       preserve image ratio */
    CORRADE_COMPARE(out.str(),
        "          |.7 |\n"
        "          |: ,|\n");
}

void CompareImageTest::pixelDelta() {
    {
        Debug() << "Visual verification -- some lines should be yellow, some red:";
        Debug d;
        Implementation::printPixelDeltas(d, DeltaRed, ActualRed, ExpectedRed, 0.5f, 0.1f, 10);
    }

    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaRed, ActualRed, ExpectedRed, 0.5f, 0.1f, 10);

    CORRADE_COMPARE(out.str(),
        "        Pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)\n"
        "          [0,2] Vector(-0.1), expected Vector(0.02) (Δ = 0.12)");
}

void CompareImageTest::pixelDeltaOverflow() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaRed, ActualRed, ExpectedRed, 0.5f, 0.1f, 3);

    CORRADE_COMPARE(out.str(),
        "        Top 3 out of 4 pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)");
}

void CompareImageTest::compareDifferentSize() {
    std::stringstream out;

    ImageView2D a{PixelFormat::RG8UI, {3, 4}, nullptr};
    ImageView2D b{PixelFormat::RG8UI, {3, 5}, nullptr};

    {
        Error e(&out);
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        CORRADE_VERIFY(!compare(a, b));
        compare.printErrorMessage(e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different size, actual Vector(3, 4) but Vector(3, 5) expected.\n");
}

void CompareImageTest::compareDifferentFormat() {
    std::stringstream out;

    ImageView2D a{PixelFormat::RGBA32F, {3, 4}, nullptr};
    ImageView2D b{PixelFormat::RGB32F, {3, 4}, nullptr};

    {
        Error e(&out);
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        CORRADE_VERIFY(!compare(a, b));
        compare.printErrorMessage(e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different format, actual PixelFormat::RGBA32F but PixelFormat::RGB32F expected.\n");
}

void CompareImageTest::compareSameZeroThreshold() {
    using namespace Math::Literals;

    const Color3 data[] = {
        0xcafeba_rgbf, 0xdeadbe_rgbf,
        0xbadc0d_rgbf, 0xbeefe0_rgbf
    };

    const ImageView2D image{PixelFormat::RGB32F, {2, 2}, data};
    CORRADE_VERIFY((TestSuite::Comparator<CompareImage>{0.0f, 0.0f}(image, image)));
}

void CompareImageTest::compareAboveThresholds() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb, ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 39/18.5 but at most 20/10 expected. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
        "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n"
        "          [0,1] #235710, expected #232710 (Δ = 16)\n");
}

void CompareImageTest::compareAboveMaxThreshold() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{30.0f, 20.0f};
        CORRADE_VERIFY(!compare(ActualRgb, ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have max delta above threshold, actual 39 but at most 30 expected. Mean delta 18.5 is below threshold 20. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n");
}

void CompareImageTest::compareAboveMeanThreshold() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{50.0f, 18.0f};
        CORRADE_VERIFY(!compare(ActualRgb, ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual 18.5 but at most 18 expected. Max delta 39 is below threshold 50. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
        "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n");
}

void CompareImageTest::setupExternalPluginManager() {
    _manager.emplace("nonexistent");
    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #if defined(ANYIMAGEIMPORTER_PLUGIN_FILENAME) && defined(TGAIMPORTER_PLUGIN_FILENAME)
    CORRADE_INTERNAL_ASSERT(_manager->load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    CORRADE_INTERNAL_ASSERT(_manager->load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void CompareImageTest::teardownExternalPluginManager() {
    _manager = Containers::NullOpt;
}

constexpr const char* ImageCompareError =
    "Images a and b have both max and mean delta above threshold, actual 39/18.5 but at most 20/10 expected. Delta image:\n"
    "          |?M|\n"
    "        Pixels above max/mean threshold:\n"
    "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
    "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n"
    "          [0,1] #235710, expected #232710 (Δ = 16)\n";

void CompareImageTest::image() {
    CORRADE_COMPARE_WITH(ActualRgb, ExpectedRgb, (CompareImage{40.0f, 20.0f}));
}

void CompareImageTest::imageError() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb, ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::imageFile() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageFile{*_manager, 40.0f, 20.0f}));
}

void CompareImageTest::imageFileError() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::imageFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageFileActualLoadFailed() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare("nonexistent.tga",
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Actual image a (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::imageFileExpectedLoadFailed() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            "nonexistent.tga"));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Expected image b (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::imageFileActualIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."), "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::imageFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."),
        "Expected image b (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::imageToFile() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(ActualRgb,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageToFile{*_manager, 40.0f, 20.0f}));
}

void CompareImageTest::imageToFileError() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb,
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::imageToFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb,
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageToFileExpectedLoadFailed() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb, "nonexistent.tga"));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Expected image b (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::imageToFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
            CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(ActualRgb, Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds")));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."),
        "Expected image b (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::fileToImage() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        ExpectedRgb,
        (CompareFileToImage{*_manager, 40.0f, 20.0f}));
}

void CompareImageTest::fileToImageError() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::fileToImagePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::fileToImageActualLoadFailed() {
    if(_manager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _manager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare("nonexistent.tga", ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Actual image a (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::fileToImageActualIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
            CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&manager, 20.0f, 10.0f};
        CORRADE_VERIFY(!compare(Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"), ExpectedRgb));
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printErrorMessage(d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."),
        "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CompareImageTest)
