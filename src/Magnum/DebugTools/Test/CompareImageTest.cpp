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
#include <numeric>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/AbstractImageConverter.h"
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
    void calculateDeltaSpecials();
    void calculateDeltaSpecials3();

    void deltaImage();
    void deltaImageScaling();
    void deltaImageColors();
    void deltaImageSpecials();

    void pixelDelta();
    void pixelDeltaEmpty();
    void pixelDeltaOverflow();
    void pixelDeltaSpecials();

    void compareDifferentSize();
    void compareDifferentFormat();
    void compareSameZeroThreshold();
    void compareAboveThresholds();
    void compareAboveMaxThreshold();
    void compareAboveMeanThreshold();
    void compareNonZeroThreshold();
    void compareSpecials();
    void compareSpecialsMeanOnly();
    void compareSpecialsDisallowedThreshold();

    void setupExternalPluginManager();
    void teardownExternalPluginManager();

    void imageZeroDelta();
    void imageNonZeroDelta();
    void imageNonZeroDeltaNoPixels();
    void imageError();
    void imageFileZeroDelta();
    void imageFileNonZeroDelta();
    void imageFileError();
    void imageFilePluginLoadFailed();
    void imageFileActualLoadFailed();
    void imageFileExpectedLoadFailed();
    void imageFileActualIsCompressed();
    void imageFileExpectedIsCompressed();
    void imageToFileZeroDelta();
    void imageToFileNonZeroDelta();
    void imageToFileError();
    void imageToFilePluginLoadFailed();
    void imageToFileExpectedLoadFailed();
    void imageToFileExpectedIsCompressed();
    void fileToImageZeroDelta();
    void fileToImageNonZeroDelta();
    void fileToImageError();
    void fileToImagePluginLoadFailed();
    void fileToImageActualLoadFailed();
    void fileToImageActualIsCompressed();

    void pixelsToImageZeroDelta();
    void pixelsToImageNonZeroDelta();
    void pixelsToImageError();
    void pixelsToFileZeroDelta();
    void pixelsToFileNonZeroDelta();
    void pixelsToFileError();

    private:
        Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> _importerManager;
        Containers::Optional<PluginManager::Manager<Trade::AbstractImageConverter>> _converterManager;
};

CompareImageTest::CompareImageTest() {
    addTests({&CompareImageTest::formatUnknown,
              &CompareImageTest::formatHalf,
              &CompareImageTest::formatImplementationSpecific,

              &CompareImageTest::calculateDelta,
              &CompareImageTest::calculateDeltaStorage,
              &CompareImageTest::calculateDeltaSpecials,
              &CompareImageTest::calculateDeltaSpecials3,

              &CompareImageTest::deltaImage,
              &CompareImageTest::deltaImageScaling,
              &CompareImageTest::deltaImageColors,
              &CompareImageTest::deltaImageSpecials,

              &CompareImageTest::pixelDelta,
              &CompareImageTest::pixelDeltaEmpty,
              &CompareImageTest::pixelDeltaOverflow,
              &CompareImageTest::pixelDeltaSpecials,

              &CompareImageTest::compareDifferentSize,
              &CompareImageTest::compareDifferentFormat,
              &CompareImageTest::compareSameZeroThreshold,
              &CompareImageTest::compareAboveThresholds,
              &CompareImageTest::compareAboveMaxThreshold,
              &CompareImageTest::compareAboveMeanThreshold,
              &CompareImageTest::compareNonZeroThreshold,
              &CompareImageTest::compareSpecials,
              &CompareImageTest::compareSpecialsMeanOnly,
              &CompareImageTest::compareSpecialsDisallowedThreshold,

              &CompareImageTest::imageZeroDelta,
              &CompareImageTest::imageNonZeroDelta,
              &CompareImageTest::imageNonZeroDeltaNoPixels,
              &CompareImageTest::imageError});

    addTests({&CompareImageTest::imageFileZeroDelta,
              &CompareImageTest::imageFileNonZeroDelta,
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

    addTests({&CompareImageTest::imageToFileZeroDelta,
              &CompareImageTest::imageToFileNonZeroDelta,
              &CompareImageTest::imageToFileError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageToFilePluginLoadFailed});

    addTests({&CompareImageTest::imageToFileExpectedLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::imageToFileExpectedIsCompressed});

    addTests({&CompareImageTest::fileToImageZeroDelta,
              &CompareImageTest::fileToImageNonZeroDelta,
              &CompareImageTest::fileToImageError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::fileToImagePluginLoadFailed});

    addTests({&CompareImageTest::fileToImageActualLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::fileToImageActualIsCompressed});

    addTests({&CompareImageTest::pixelsToImageZeroDelta,
              &CompareImageTest::pixelsToImageNonZeroDelta,
              &CompareImageTest::pixelsToImageError});

    addTests({&CompareImageTest::pixelsToFileZeroDelta,
              &CompareImageTest::pixelsToFileNonZeroDelta,
              &CompareImageTest::pixelsToFileError},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    /* Plugin manager setup is not done here, but in the
       setupExternalPluginManager() function */
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
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, PixelFormat(0xdead), 0, 0, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: unknown format PixelFormat(0xdead)\n");
}

void CompareImageTest::formatHalf() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelFormat::RG16F, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: half-float formats are not supported yet\n");
}

void CompareImageTest::formatImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, pixelFormatWrap(0xdead), 0, 0, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out.str(), "DebugTools::CompareImage: can't compare implementation-specific pixel formats\n");
}

void CompareImageTest::calculateDelta() {
    Containers::Array<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRed.format(), ActualRed.pixels(), ExpectedRed);

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
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRgb.format(), ActualRgb.pixels(), ExpectedRgb);

    CORRADE_COMPARE_AS(delta, (Containers::Array<Float>{Containers::InPlaceInit, {
        1.0f/3.0f, (55.0f + 1.0f)/3.0f,
        48.0f/3.0f, 117.0f/3.0f
    }}), TestSuite::Compare::Container);
    CORRADE_COMPARE(max, 117.0f/3.0f);
    CORRADE_COMPARE(mean, 18.5f);
}

/* Variants:
    -   expected number, got inf (and inverse)
    -   expected number, got nan (and inverse)
    -   got inf in both (and again, but different sign)
    -   got nan in both
    -   got a number in both (twice, to ensure it's calculated correctly) */
const Float ActualDataSpecials[]{
    Constants::inf(), 0.3f,
    Constants::nan(), 0.3f,
    -Constants::inf(), -Constants::inf(),
    Constants::nan(),
    0.3f, 3.0f
};
const Float ExpectedDataSpecials[]{
    1.0f, -Constants::inf(),
    0.3f, Constants::nan(),
    -Constants::inf(), Constants::inf(),
    Constants::nan(),
    0.65f, -0.1f
};
const Float DeltaSpecials[]{
    Constants::inf(), Constants::inf(),
    Constants::nan(), Constants::nan(),
    0.0f, Constants::inf(),
    0.0f,
    0.35f, 3.1f
};

const ImageView2D ActualSpecials{PixelFormat::R32F, {9, 1}, ActualDataSpecials};
const ImageView2D ExpectedSpecials{PixelFormat::R32F, {9, 1}, ExpectedDataSpecials};

void CompareImageTest::calculateDeltaSpecials() {
    Containers::Array<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualSpecials.format(), ActualSpecials.pixels(), ExpectedSpecials);
    CORRADE_COMPARE_AS(Containers::arrayView(delta),
        Containers::arrayView(DeltaSpecials),
        TestSuite::Compare::Container);
    /* Max should be calculated *without* the specials because otherwise every
       other potential difference will be zero compared to infinity. OTOH mean
       needs to get "poisoned" by those in order to have *something* to fail
       the test with. */
    CORRADE_COMPARE(max, 3.1f);
    CORRADE_COMPARE(mean, -Constants::nan());
}

void CompareImageTest::calculateDeltaSpecials3() {
    /* Same as calculateDeltaSpecials(), but reinterpreting the data as
       a three-component vector in order to test per-component handling of
       specials */
    const ImageView2D actualSpecials3{PixelFormat::RGB32F, {3, 1}, ActualDataSpecials};
    const ImageView2D expectedSpecials3{PixelFormat::RGB32F, {3, 1}, ExpectedDataSpecials};

    Containers::Array<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(actualSpecials3.format(), actualSpecials3.pixels(), expectedSpecials3);
    CORRADE_COMPARE_AS(delta, (Containers::Array<Float>{Containers::InPlaceInit, {
        Constants::nan(), Constants::nan(), 1.15f
    }}), TestSuite::Compare::Container);
    /* Max and mean should be calculated *without* the specials because
       otherwise every other potential difference will be zero compared to
       infinity */
    CORRADE_COMPARE(max, 1.15f);
    CORRADE_COMPARE(mean, -Constants::nan());
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
        "          | ....,,:::~~~===+++???777IIZZZ$$|");
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
        "          |...,,,::~~~==++???77IIIZZ$$000888|");
}

void CompareImageTest::deltaImageColors() {
    /* Print for visual color verification */
    {
        Debug out;
        out << "Visual verification -- some letters should be yellow, some red, some white:"
            << Debug::newline;
        Implementation::printDeltaImage(out, DeltaRed, {3, 3}, 2.0f, 0.5f, 0.2f);
    }

    std::ostringstream out;
    Debug dc{&out, Debug::Flag::DisableColors};
    Implementation::printDeltaImage(dc, DeltaRed, {3, 3}, 2.0f, 0.5f, 0.2f);
    /* Yes, there is half of the rows (2 instead of 3) in order to roughly
       preserve image ratio */
    CORRADE_COMPARE(out.str(),
        "          |.7 |\n"
        "          |: ,|");
}

void CompareImageTest::deltaImageSpecials() {
    const Float inf = Constants::inf(), nan = Constants::nan();
    /* Duplicate the rows as the delta image visualizer merges each two to
       preserve ratio */
    const Float delta[]{0.7f,  inf, 2.5f,
                        0.7f,  nan, 2.5f,
                         nan,  inf, 0.0f,
                         nan,  inf, 0.0f};

    std::ostringstream out;
    Debug dc{&out, Debug::Flag::DisableColors};
    Implementation::printDeltaImage(dc, delta, {3, 4}, 3.0f, 0.0f, 0.0f);
    /* Should show the max value for NaN and infs and the usual things
       otherwise */
    CORRADE_COMPARE(out.str(),
        "          |MM |\n"
        "          |~M8|");
}

void CompareImageTest::pixelDelta() {
    {
        Debug out;
        out << "Visual verification -- some lines should be yellow, some red:";
        Implementation::printPixelDeltas(out, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 10);
    }

    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 10);

    CORRADE_COMPARE(out.str(), "\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)\n"
        "          [0,2] Vector(-0.1), expected Vector(0.02) (Δ = 0.12)");
}

void CompareImageTest::pixelDeltaEmpty() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 1.0f, 1.0f, 10);

    CORRADE_COMPARE(out.str(), "");
}

void CompareImageTest::pixelDeltaOverflow() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 3);

    CORRADE_COMPARE(out.str(), "\n"
        "        Top 3 out of 4 pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)");
}

void CompareImageTest::pixelDeltaSpecials() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};
    Implementation::printPixelDeltas(d, DeltaSpecials, ActualSpecials.format(), ActualSpecials.pixels(), ExpectedSpecials.pixels(), 1.5f, 0.5f, 10);

    /* MSVC prints -nan(ind) instead of ±nan. But only sometimes. */
    #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG_CL)
    CORRADE_COMPARE(out.str(), "\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = -nan(ind))\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = -nan(ind))\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)");
    #else
    CORRADE_COMPARE(out.str(), "\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)");
    #endif
}

void CompareImageTest::compareDifferentSize() {
    std::stringstream out;

    char data[8*5];
    ImageView2D a{PixelFormat::RG8UI, {3, 4}, data};
    ImageView2D b{PixelFormat::RG8UI, {3, 5}, data};

    {
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        TestSuite::ComparisonStatusFlags flags = compare(a, b);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Error e(&out);
        compare.printMessage(flags, e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different size, actual Vector(3, 4) but Vector(3, 5) expected.\n");
}

void CompareImageTest::compareDifferentFormat() {
    std::stringstream out;

    char data[16*12];
    ImageView2D a{PixelFormat::RGBA32F, {3, 4}, data};
    ImageView2D b{PixelFormat::RGB32F, {3, 4}, data};

    {
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        TestSuite::ComparisonStatusFlags flags = compare(a, b);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Error e(&out);
        compare.printMessage(flags, e, "a", "b");
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
    CORRADE_COMPARE((TestSuite::Comparator<CompareImage>{0.0f, 0.0f}(image, image)), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::compareAboveThresholds() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
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
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have max delta above threshold, actual 39 but at most 30 expected. Mean delta 18.5 is within threshold 20. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n");
}

void CompareImageTest::compareAboveMeanThreshold() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{50.0f, 18.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual 18.5 but at most 18 expected. Max delta 39 is within threshold 50. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
        "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n");
}

void CompareImageTest::compareNonZeroThreshold() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(),
        "Images a and b have deltas 39/18.5 below threshold 40/20. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n");
}

void CompareImageTest::compareSpecials() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{1.5f, 0.5f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualSpecials, ExpectedSpecials);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* Apple platforms, Android and MinGW32 don't print signed NaNs. This is
       *not* a libc++ thing, tho -- libc++ on Linux prints signed NaNs. It used
       to be with Emscripten too, but since 1.38.44 works the same as Linux. */
    #if defined(CORRADE_TARGET_APPLE) || defined(CORRADE_TARGET_ANDROID) || defined(CORRADE_TARGET_MINGW)
    CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 3.1/nan but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* clang-cl prints -nan(ind) instead of ±nan, but differently than MSVC */
    #elif defined(CORRADE_TARGET_CLANG_CL)
    CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 3.1/-nan(ind) but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* MSVC prints -nan(ind) instead of ±nan. But only sometimes, and
       differently on 32/64bit builds. */
    #elif defined(CORRADE_TARGET_MSVC)
    #ifdef _M_X64
    CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 3.1/-nan(ind) but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = nan)\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 3.1/nan but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = nan)\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif

    /* Linux, Emscripten. Somehow, a Release build sometimes gives a positive
       NaN, so test for both. */
    #else
    constexpr const char* expectedPositive =
        "Images a and b have both max and mean delta above threshold, actual 3.1/nan but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n";
    if(out.str() == expectedPositive)
        CORRADE_COMPARE(out.str(), expectedPositive);
    else CORRADE_COMPARE(out.str(),
        "Images a and b have both max and mean delta above threshold, actual 3.1/-nan but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif
}

void CompareImageTest::compareSpecialsMeanOnly() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{15.0f, 0.5f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualSpecials, ExpectedSpecials);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* Apple platforms, Android and MinGW32 don't print signed NaNs. This is
       *not* a libc++ thing, tho -- libc++ on Linux prints signed NaNs. It used
       to be with Emscripten too, but since 1.38.44 works the same as Linux. */
    #if defined(CORRADE_TARGET_APPLE) || defined(CORRADE_TARGET_ANDROID) || defined(__MINGW32__)
    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual nan but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* clang-cl prints -nan(ind) instead of ±nan, but differently than MSVC */
    #elif defined(CORRADE_TARGET_CLANG_CL)
    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual -nan(ind) but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* MSVC prints -nan(ind) instead of ±nan. But only sometimes, and
       differently on 32/64bit builds. */
    #elif defined(CORRADE_TARGET_MSVC)
    #ifdef _M_X64
    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual -nan(ind) but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = nan)\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual nan but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = nan)\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif

    /* Linux, Emscripten. Somehow, a Release build sometimes gives a positive
       NaN, so test for both. */
    #else
    constexpr const char* expectedPositive =
        "Images a and b have mean delta above threshold, actual nan but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n";
    if(out.str() == expectedPositive)
        CORRADE_COMPARE(out.str(), expectedPositive);
    else CORRADE_COMPARE(out.str(),
        "Images a and b have mean delta above threshold, actual -nan but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif
}

void CompareImageTest::compareSpecialsDisallowedThreshold() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;

    {
        Error redirectError{&out};
        TestSuite::Comparator<CompareImage> a{Constants::inf(), 0.3f};
        TestSuite::Comparator<CompareImage> b{0.3f, Constants::nan()};
    }

    CORRADE_COMPARE(out.str(),
        "DebugTools::CompareImage: thresholds can't be NaN or infinity\n"
        "DebugTools::CompareImage: thresholds can't be NaN or infinity\n");
}

void CompareImageTest::setupExternalPluginManager() {
    _importerManager.emplace("nonexistent");
    _converterManager.emplace("nonexistent");
    /* Load the plugin directly from the build tree. Otherwise it's either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager->load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager->load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager->load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager->load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void CompareImageTest::teardownExternalPluginManager() {
    _importerManager = Containers::NullOpt;
    _converterManager = Containers::NullOpt;
}

constexpr const char* ImageCompareVerbose =
    "Images a and b have deltas 39/18.5 below threshold 40/20. Delta image:\n"
    "          |?M|\n"
    "        Pixels above max/mean threshold:\n"
    "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n";

constexpr const char* ImageCompareError =
    "Images a and b have both max and mean delta above threshold, actual 39/18.5 but at most 20/10 expected. Delta image:\n"
    "          |?M|\n"
    "        Pixels above max/mean threshold:\n"
    "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
    "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n"
    "          [0,1] #235710, expected #232710 (Δ = 16)\n";

void CompareImageTest::imageZeroDelta() {
    CORRADE_COMPARE_WITH(ExpectedRgb, ExpectedRgb, (CompareImage{40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
    CORRADE_COMPARE(compare(ExpectedRgb, ExpectedRgb), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::imageNonZeroDelta() {
    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(ActualRgb, ExpectedRgb, (CompareImage{40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::imageNonZeroDeltaNoPixels() {
    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(ActualRgb, ExpectedRgb, (CompareImage{40.0f, 40.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 40.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* No pixel list written as there are no outliers. Testing this just once
       since all other combinations (image/file/pixels) use the same codepath. */
    CORRADE_COMPARE(out.str(),
        "Images a and b have deltas 39/18.5 below threshold 40/40. Delta image:\n"
        "          |?M|\n");
}

void CompareImageTest::imageError() {
    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::imageFileZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")),
        TestSuite::ComparisonStatusFlag{});
}

void CompareImageTest::imageFileNonZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageFile{*_importerManager, 40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::imageFileError() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    {
        out.str({});
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out.str(), Utility::formatString("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* Can't load a plugin, so we can't open the file, so we can't save
           it either and thus no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageFileActualLoadFailed() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare("nonexistent.tga",
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* We can't open the file, so we can't save it either and thus no
           diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Actual image a (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::imageFileExpectedLoadFailed() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        "nonexistent.tga");
    /* Actual file *could* be loaded, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Expected image b (nonexistent.tga) could not be loaded.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "nonexistent.tga");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    {
        out.str({});
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out.str(), Utility::formatString("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageFileActualIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"),
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* We most probably couldn't save the file because it's in a different
           format, so no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."), "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::imageFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"));
    /* Actual file is not, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageCompressed.dds");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* This will attempt to save a DDS and then fails, because we have no DDS
       exporter. */
    Debug d;
    compare.saveDiagnostic(flags, d, COMPAREIMAGETEST_SAVE_DIR);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
}

void CompareImageTest::imageToFileZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(ExpectedRgb,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(ExpectedRgb, Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")),
        TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::imageToFileNonZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(ActualRgb,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::imageToFileError() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    {
        out.str({});
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out.str(), Utility::formatString("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageToFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&manager, nullptr, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb,
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* Can't load a plugin, so we can't open the file, so we can't save
           it either and thus no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageToFileExpectedLoadFailed() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, "nonexistent.tga");
    /* Actual file *could* be loaded, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Expected image b (nonexistent.tga) could not be loaded.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "nonexistent.tga");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    {
        out.str({});
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out.str(), Utility::formatString("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageToFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager;
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
            CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageToFile> compare{&manager, nullptr, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"));
    /* Actual file is not, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."),
        "Expected image b (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageCompressed.dds");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    /* This will attempt to save a DDS and then fails, because we have no DDS
       exporter. */
    Debug d;
    compare.saveDiagnostic(flags, d, COMPAREIMAGETEST_SAVE_DIR);
    CORRADE_VERIFY(!Utility::Directory::exists(filename));
}

void CompareImageTest::fileToImageZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        ExpectedRgb,
        (CompareFileToImage{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        ExpectedRgb), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::fileToImageNonZeroDelta() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        ExpectedRgb,
        (CompareFileToImage{*_importerManager, 40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
        ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::fileToImageError() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
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
        TestSuite::ComparisonStatusFlags flags = compare(
            Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"),
            ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::fileToImageActualLoadFailed() {
    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare("nonexistent.tga", ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
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
        TestSuite::ComparisonStatusFlags flags = compare(Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds"), ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out.str(), DEBUGTOOLS_TEST_DIR, "..."),
        "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::pixelsToImageZeroDelta() {
    /* Same as image(), but taking pixels instead */

    CORRADE_COMPARE_WITH(ExpectedRgb.pixels<Color3ub>(),
        ExpectedRgb, (CompareImage{40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
    CORRADE_COMPARE(compare(ExpectedRgb.pixels<Color3ub>(), ExpectedRgb), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::pixelsToImageNonZeroDelta() {
    /* Same as image(), but taking pixels instead */

    CORRADE_COMPARE_WITH(ActualRgb.pixels<Color3ub>(),
        ExpectedRgb, (CompareImage{40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(), ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::pixelsToImageError() {
    /* Same as imageError(), but taking pixels instead */

    std::stringstream out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags =
            compare(ActualRgb.pixels<Color3ub>(), ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);
}

void CompareImageTest::pixelsToFileZeroDelta() {
    /* Same as imageToFile(), but taking pixels instead */

    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(ExpectedRgb.pixels<Color3ub>(),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(ExpectedRgb.pixels<Color3ub>(), Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga")),
        TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::pixelsToFileNonZeroDelta() {
    /* Same as imageToFile(), but taking pixels instead */

    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(ActualRgb.pixels<Color3ub>(),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"),
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    std::ostringstream out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(), Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareVerbose);
}

void CompareImageTest::pixelsToFileError() {
    /* Same as imageToFileError(), but taking pixels instead */

    if(_importerManager->loadState("AnyImageImporter") == PluginManager::LoadState::NotFound ||
       _importerManager->loadState("TgaImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter or TgaImporter plugins not found.");

    std::stringstream out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga"));
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out.str(), ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Directory::mkpath(COMPAREIMAGETEST_SAVE_DIR));
    std::string filename = Utility::Directory::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Directory::exists(filename))
        CORRADE_VERIFY(Utility::Directory::rm(filename));

    {
        out.str({});
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out.str(), Utility::formatString("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CompareImageTest)
