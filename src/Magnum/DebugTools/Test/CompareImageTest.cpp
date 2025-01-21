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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/File.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* replaceFirst() */

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#include "configure.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct CompareImageTest: TestSuite::Tester {
    explicit CompareImageTest();

    void formatUnknown();
    void formatPackedDepthStencil();
    void formatImplementationSpecific();

    void calculateDelta();
    void calculateDeltaInteger();
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
    void pixelDeltaHalf();
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

    void mutableImageViewImageImageData();
    void mutableImageViewImageImageDataToFile();

    template<unsigned dimensions> void pixelFormatFor();
    void pixelFormatForColor();

    template<class T> void pixelsToImageZeroDelta();
    void pixelsToImageNonZeroDelta();
    void pixelsToImageDifferentFormat();
    void pixelsToImageError();
    template<class T> void pixelsToFileZeroDelta();
    void pixelsToFileNonZeroDelta();
    void pixelsToFileDifferentFormat();
    void pixelsToFileError();
    void pixelsToFileExpectedLoadFailed();

    private:
        Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> _importerManager;
        Containers::Optional<PluginManager::Manager<Trade::AbstractImageConverter>> _converterManager;
};

const struct {
    const char* name;
    bool srgb;
} PixelsToImageData[]{
    {"", false},
    {"sRGB", true}
};

CompareImageTest::CompareImageTest() {
    addTests({&CompareImageTest::formatUnknown,
              &CompareImageTest::formatPackedDepthStencil,
              &CompareImageTest::formatImplementationSpecific,

              &CompareImageTest::calculateDelta,
              &CompareImageTest::calculateDeltaInteger,
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
              &CompareImageTest::pixelDeltaHalf,
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

    addTests({&CompareImageTest::fileToImageActualIsCompressed,

              &CompareImageTest::mutableImageViewImageImageData});

    addTests({&CompareImageTest::mutableImageViewImageImageDataToFile},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::pixelFormatFor<1>,
              &CompareImageTest::pixelFormatFor<2>,
              &CompareImageTest::pixelFormatFor<3>,
              &CompareImageTest::pixelFormatFor<4>,
              &CompareImageTest::pixelFormatForColor});

    addInstancedTests<CompareImageTest>({
        &CompareImageTest::pixelsToImageZeroDelta<Color3ub>,
        &CompareImageTest::pixelsToImageZeroDelta<Vector3ub>},
        Containers::arraySize(PixelsToImageData));

    addTests({&CompareImageTest::pixelsToImageNonZeroDelta,
              &CompareImageTest::pixelsToImageDifferentFormat,
              &CompareImageTest::pixelsToImageError});

    addInstancedTests<CompareImageTest>({
        &CompareImageTest::pixelsToFileZeroDelta<Color3ub>,
        &CompareImageTest::pixelsToFileZeroDelta<Vector3ub>},
        Containers::arraySize(PixelsToImageData),
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    addTests({&CompareImageTest::pixelsToFileNonZeroDelta,
              &CompareImageTest::pixelsToFileDifferentFormat,
              &CompareImageTest::pixelsToFileError,
              &CompareImageTest::pixelsToFileExpectedLoadFailed},
        &CompareImageTest::setupExternalPluginManager,
        &CompareImageTest::teardownExternalPluginManager);

    /* Plugin manager setup is not done here, but in the
       setupExternalPluginManager() function */
}

using namespace Math::Literals;

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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, PixelFormat(0xdead), 0, 1, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out, "DebugTools::CompareImage: unknown format PixelFormat(0xdead)\n");
}

void CompareImageTest::formatPackedDepthStencil() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    ImageView2D image{PixelFormat::Depth24UnormStencil8UI, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out, "DebugTools::CompareImage: packed depth/stencil formats are not supported yet\n");
}

void CompareImageTest::formatImplementationSpecific() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    ImageView2D image{PixelStorage{}, pixelFormatWrap(0xdead), 0, 1, {}};
    Implementation::calculateImageDelta(image.format(), image.pixels(), image);

    CORRADE_COMPARE(out, "DebugTools::CompareImage: can't compare implementation-specific pixel formats\n");
}

void CompareImageTest::calculateDelta() {
    Containers::Triple<Containers::Array<Float>, Float, Float> deltaMaxMean = Implementation::calculateImageDelta(ActualRed.format(), ActualRed.pixels(), ExpectedRed);
    CORRADE_COMPARE_AS(deltaMaxMean.first(),
        Containers::arrayView(DeltaRed),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(deltaMaxMean.second(), 1.0f);
    CORRADE_COMPARE(deltaMaxMean.third(), 0.208889f);
}

void CompareImageTest::calculateDeltaInteger() {
    /* Like ActualRedData, ExpectedRedData and DeltaRed, just multiplied 100
       times and saved into a Short */
    const Short actualRedData[]{
        30, 100, 90,
        90, 60, 20,
        -10, 100, 0
    };

    const Short expectedRedData[]{
        65, 100, 60,
        91, 60, 10,
        2, 0, 0
    };

    const Float deltaRed[]{
        35.0f, 0.0f, 30.0f,
        1.0f, 0.0f, 10.0f,
        12.0f, 100.0f, 0.0f
    };

    const ImageView2D actualRed{PixelStorage{}.setAlignment(2), PixelFormat::R16I, {3, 3}, actualRedData};
    const ImageView2D expectedRed{PixelStorage{}.setAlignment(2), PixelFormat::R16I, {3, 3}, expectedRedData};

    Containers::Triple<Containers::Array<Float>, Float, Float> deltaMaxMean = Implementation::calculateImageDelta(actualRed.format(), actualRed.pixels(), expectedRed);
    CORRADE_COMPARE_AS(deltaMaxMean.first(),
        Containers::arrayView(deltaRed),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(deltaMaxMean.second(), 100.0f);
    CORRADE_COMPARE(deltaMaxMean.third(), 20.8889f);
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
    Containers::Triple<Containers::Array<Float>, Float, Float> deltaMaxMean = Implementation::calculateImageDelta(ActualRgb.format(), ActualRgb.pixels(), ExpectedRgb);
    CORRADE_COMPARE_AS(deltaMaxMean.first(), Containers::arrayView<Float>({
        1.0f/3.0f, (55.0f + 1.0f)/3.0f,
        48.0f/3.0f, 117.0f/3.0f
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE(deltaMaxMean.second(), 117.0f/3.0f);
    CORRADE_COMPARE(deltaMaxMean.third(), 18.5f);
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
    Containers::Triple<Containers::Array<Float>, Float, Float> deltaMaxMean = Implementation::calculateImageDelta(ActualSpecials.format(), ActualSpecials.pixels(), ExpectedSpecials);
    CORRADE_COMPARE_AS(Containers::arrayView(deltaMaxMean.first()),
        Containers::arrayView(DeltaSpecials),
        TestSuite::Compare::Container);
    /* Max should be calculated *without* the specials because otherwise every
       other potential difference will be zero compared to infinity. OTOH mean
       needs to get "poisoned" by those in order to have *something* to fail
       the test with. */
    CORRADE_COMPARE(deltaMaxMean.second(), 3.1f);
    CORRADE_COMPARE(deltaMaxMean.third(), -Constants::nan());
}

void CompareImageTest::calculateDeltaSpecials3() {
    /* Same as calculateDeltaSpecials(), but reinterpreting the data as
       a three-component vector in order to test per-component handling of
       specials */
    const ImageView2D actualSpecials3{PixelFormat::RGB32F, {3, 1}, ActualDataSpecials};
    const ImageView2D expectedSpecials3{PixelFormat::RGB32F, {3, 1}, ExpectedDataSpecials};

    Containers::Triple<Containers::Array<Float>, Float, Float> deltaMaxMean = Implementation::calculateImageDelta(actualSpecials3.format(), actualSpecials3.pixels(), expectedSpecials3);
    CORRADE_COMPARE_AS(deltaMaxMean.first(), Containers::arrayView<Float>({
        Constants::nan(), Constants::nan(), 1.15f
    }), TestSuite::Compare::Container);
    /* Max and mean should be calculated *without* the specials because
       otherwise every other potential difference will be zero compared to
       infinity */
    CORRADE_COMPARE(deltaMaxMean.second(), 1.15f);
    CORRADE_COMPARE(deltaMaxMean.third(), -Constants::nan());
}

void CompareImageTest::deltaImage() {
    Containers::Array<Float> delta{32*32};

    for(std::int_fast32_t x = 0; x != 32; ++x)
        for(std::int_fast32_t y = 0; y != 32; ++y)
            delta[y*32 + x] = Vector2{Float(x), Float(y)}.length()/Vector2{32.0f}.length();

    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printDeltaImage(d, delta, {32, 32}, 1.0f, 0.0f, 0.0f);
    }
    CORRADE_COMPARE(out,
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
    Containers::Array<Float> delta{65*40};
    for(std::int_fast32_t x = 0; x != 65; ++x)
        for(std::int_fast32_t y = 0; y != 40; ++y)
            delta[y*65 + x] = Vector2{Float(x), Float(y)}.length()/Vector2{65.0f, 40.0f}.length();

    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printDeltaImage(d, delta, {65, 40}, 1.0f, 0.0f, 0.0f);
    }
    CORRADE_COMPARE(out,
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

    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug dc{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printDeltaImage(dc, DeltaRed, {3, 3}, 2.0f, 0.5f, 0.2f);
    }
    /* Yes, there is half of the rows (2 instead of 3) in order to roughly
       preserve image ratio */
    CORRADE_COMPARE(out,
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

    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug dc{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printDeltaImage(dc, delta, {3, 4}, 3.0f, 0.0f, 0.0f);
    }
    /* Should show the max value for NaN and infs and the usual things
       otherwise */
    CORRADE_COMPARE(out,
        "          |MM |\n"
        "          |~M8|");
}

void CompareImageTest::pixelDelta() {
    {
        Debug out;
        out << "Visual verification -- some lines should be yellow, some red:";
        Implementation::printPixelDeltas(out, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 10);
    }

    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 10);
    }
    CORRADE_COMPARE(out, "\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)\n"
        "          [0,2] Vector(-0.1), expected Vector(0.02) (Δ = 0.12)");
}

void CompareImageTest::pixelDeltaEmpty() {
    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 1.0f, 1.0f, 10);
    }
    CORRADE_COMPARE(out, "");
}

void CompareImageTest::pixelDeltaOverflow() {
    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printPixelDeltas(d, DeltaRed, ActualRed.format(), ActualRed.pixels(), ExpectedRed.pixels(), 0.5f, 0.1f, 3);
    }
    CORRADE_COMPARE(out, "\n"
        "        Top 3 out of 4 pixels above max/mean threshold:\n"
        "          [1,2] Vector(1), expected Vector(0) (Δ = 1)\n"
        "          [0,0] Vector(0.3), expected Vector(0.65) (Δ = 0.35)\n"
        "          [2,0] Vector(0.9), expected Vector(0.6) (Δ = 0.3)");
}

const Vector2h ActualHalfData[]{
    {0.3_h, 1.0_h}, { 0.9_h, 0.9_h},
    {0.6_h, 0.2_h}, {-0.1_h, 1.0_h},
};

const Vector2h ExpectedHalfData[]{
    {0.65_h, 1.0_h}, {0.6_h, 0.91_h},
    {0.6_h, 0.1_h}, {0.02_h, 0.0_h}
};

const Float DeltaHalf[] {
    0.35f/2.0f, 0.31f/2.0f,
    0.01f/2.0f, 0.22f/2.0f
};

const ImageView2D ActualHalf{PixelFormat::RG16F, {2, 2}, ActualHalfData};
const ImageView2D ExpectedHalf{PixelFormat::RG16F, {2, 2}, ExpectedHalfData};

void CompareImageTest::pixelDeltaHalf() {
    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printPixelDeltas(d, DeltaHalf, ActualHalf.format(), ActualHalf.pixels(), ExpectedHalf.pixels(), 0.5f, 0.1f, 10);
    }
    CORRADE_COMPARE(out, "\n"
        "        Pixels above max/mean threshold:\n"
        "          [0,0] Vector(0.3, 1), expected Vector(0.6499, 1) (Δ = 0.175)\n"
        "          [1,0] Vector(0.8999, 0.8999), expected Vector(0.6001, 0.9102) (Δ = 0.155)\n"
        "          [1,1] Vector(-0.09998, 1), expected Vector(0.02, 0) (Δ = 0.11)");
}

void CompareImageTest::pixelDeltaSpecials() {
    /* The string gets fully written only on destruction or with a newline at
       the end */
    Containers::String out;
    {
        Debug d{&out, Debug::Flag::DisableColors|Debug::Flag::NoNewlineAtTheEnd};
        Implementation::printPixelDeltas(d, DeltaSpecials, ActualSpecials.format(), ActualSpecials.pixels(), ExpectedSpecials.pixels(), 1.5f, 0.5f, 10);
    }
    /* MSVC before version 2019 16.10(11?) prints -nan(ind) instead of ±nan.
       But only sometimes. */
    #if defined(CORRADE_TARGET_MSVC) && _MSC_VER < 1929 && !defined(CORRADE_TARGET_CLANG_CL)
    CORRADE_COMPARE(out, "\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(-nan(ind)) (Δ = -nan(ind))\n"
        "          [2,0] Vector(-nan(ind)), expected Vector(0.3) (Δ = -nan(ind))\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)");
    #else
    CORRADE_COMPARE(out, "\n"
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
    Containers::String out;

    char data[8*5];
    ImageView2D a{PixelFormat::RG8UI, {3, 4}, data};
    ImageView2D b{PixelFormat::RG8UI, {3, 5}, data};

    {
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        TestSuite::ComparisonStatusFlags flags = compare(a, b);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Images a and b have different size, actual Vector(3, 4) but Vector(3, 5) expected.\n");
}

void CompareImageTest::compareDifferentFormat() {
    Containers::String out;

    char data[16*12];
    ImageView2D a{PixelFormat::RGBA32F, {3, 4}, data};
    ImageView2D b{PixelFormat::RGB32F, {3, 4}, data};

    {
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        TestSuite::ComparisonStatusFlags flags = compare(a, b);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Images a and b have different format, actual PixelFormat::RGBA32F but PixelFormat::RGB32F expected.\n");
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
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out,
        "Images a and b have both max and mean delta above threshold, actual 39/18.5 but at most 20/10 expected. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
        "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n"
        "          [0,1] #235710, expected #232710 (Δ = 16)\n");
}

void CompareImageTest::compareAboveMaxThreshold() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{30.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out,
        "Images a and b have max delta above threshold, actual 39 but at most 30 expected. Mean delta 18.5 is within threshold 20. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n");
}

void CompareImageTest::compareAboveMeanThreshold() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{50.0f, 18.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out,
        "Images a and b have mean delta above threshold, actual 18.5 but at most 18 expected. Max delta 39 is within threshold 50. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n"
        "          [1,0] #5647ec, expected #5610ed (Δ = 18.6667)\n");
}

void CompareImageTest::compareNonZeroThreshold() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out,
        "Images a and b have deltas 39/18.5 below threshold 40/20. Delta image:\n"
        "          |?M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [1,1] #abcd85, expected #abcdfa (Δ = 39)\n");
}

void CompareImageTest::compareSpecials() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{1.5f, 0.5f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualSpecials, ExpectedSpecials);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* clang-cl prints -nan(ind) instead of ±nan, but differently than MSVC */
    #ifdef CORRADE_TARGET_CLANG_CL
    CORRADE_COMPARE(out,
        "Images a and b have both max and mean delta above threshold, actual 3.1/-nan(ind) but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* MSVC prints -nan(ind) instead of ±nan. But only sometimes, differently
       on 32/64bit builds and differently since version 2019 16.10(11?). FFS I
       need my own float printer already, this is utter madness. */
    #elif defined(CORRADE_TARGET_MSVC)
    #if _MSC_VER < 1929
    #ifdef _M_X64
    CORRADE_COMPARE(out,
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
    CORRADE_COMPARE(out,
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
    #else
    CORRADE_COMPARE(out,
        "Images a and b have both max and mean delta above threshold, actual 3.1/-nan(ind) but at most 1.5/0.5 expected. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif

    /* Linux, Emscripten, Android. Somehow, a Release build sometimes gives a
       positive NaN, so test for both; Android randomly differs between the
       two. Apple platforms and MinGW32 don't print signed NaNs, but it doesn't
       make sense to have yet another branch for those so they're handled here
       as well. */
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
    if(out == expectedPositive)
        CORRADE_COMPARE(out, expectedPositive);
    else CORRADE_COMPARE(out,
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
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{15.0f, 0.5f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualSpecials, ExpectedSpecials);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* clang-cl prints -nan(ind) instead of ±nan, but differently than MSVC */
    #ifdef CORRADE_TARGET_CLANG_CL
    CORRADE_COMPARE(out,
        "Images a and b have mean delta above threshold, actual -nan(ind) but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");

    /* MSVC prints -nan(ind) instead of ±nan. But only sometimes, differently
       on 32/64bit builds and differently since version 2019 16.10(11?). FFS I
       need my own float printer already, this is utter madness. */
    #elif defined(CORRADE_TARGET_MSVC)
    #if _MSC_VER < 1929
    #ifdef _M_X64
    CORRADE_COMPARE(out,
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
    CORRADE_COMPARE(out,
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
    #else
    CORRADE_COMPARE(out,
        "Images a and b have mean delta above threshold, actual -nan(ind) but at most 0.5 expected. Max delta 3.1 is within threshold 15. Delta image:\n"
        "          |MMMM M ,M|\n"
        "        Pixels above max/mean threshold:\n"
        "          [5,0] Vector(-inf), expected Vector(inf) (Δ = inf)\n"
        "          [3,0] Vector(0.3), expected Vector(nan) (Δ = nan)\n"
        "          [2,0] Vector(nan), expected Vector(0.3) (Δ = nan)\n"
        "          [1,0] Vector(0.3), expected Vector(-inf) (Δ = inf)\n"
        "          [0,0] Vector(inf), expected Vector(1) (Δ = inf)\n"
        "          [8,0] Vector(3), expected Vector(-0.1) (Δ = 3.1)\n");
    #endif

    /* Linux, Emscripten, Android. Somehow, a Release build sometimes gives a
       positive NaN, so test for both; Android randomly differs between the
       two. Apple platforms and MinGW32 don't print signed NaNs, but it doesn't
       make sense to have yet another branch for those so they're handled here
       as well. */
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
    if(out == expectedPositive)
        CORRADE_COMPARE(out, expectedPositive);
    else CORRADE_COMPARE(out,
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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;

    {
        Error redirectError{&out};
        TestSuite::Comparator<CompareImage> a{Constants::inf(), 0.3f};
        TestSuite::Comparator<CompareImage> b{0.3f, Constants::nan()};
    }

    CORRADE_COMPARE(out,
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

    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::imageNonZeroDeltaNoPixels() {
    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(ActualRgb, ExpectedRgb, (CompareImage{40.0f, 40.0f}));

    Containers::String out;

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
    CORRADE_COMPARE(out,
        "Images a and b have deltas 39/18.5 below threshold 40/40. Delta image:\n"
        "          |?M|\n");
}

void CompareImageTest::imageError() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);
}

void CompareImageTest::imageFileZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
    CORRADE_COMPARE_WITH(expectedFilename, expectedFilename,
        (CompareImageFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(expectedFilename, expectedFilename),
        TestSuite::ComparisonStatusFlag{});
}

void CompareImageTest::imageFileNonZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(actualFilename, expectedFilename,
        (CompareImageFile{*_importerManager, 40.0f, 20.0f}));

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, expectedFilename);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::imageFileError() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
    TestSuite::ComparisonStatusFlags flags = compare(actualFilename, expectedFilename);
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
        Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, expectedFilename);
        /* Can't load a plugin, so we can't open the file, so we can't save
           it either and thus no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageFileActualLoadFailed() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, nullptr, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
        TestSuite::ComparisonStatusFlags flags = compare("nonexistent.tga", expectedFilename);
        /* We can't open the file, so we can't save it either and thus no
           diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Actual image a (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::imageFileExpectedLoadFailed() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    TestSuite::Comparator<CompareImageFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
    TestSuite::ComparisonStatusFlags flags = compare(actualFilename, "nonexistent.tga");
    /* Actual file *could* be loaded, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Expected image b (nonexistent.tga) could not be loaded.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "nonexistent.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageFileActualIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds");
        Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, expectedFilename);
        /* We most probably couldn't save the file because it's in a different
           format, so no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out, DEBUGTOOLS_TEST_DIR, "..."), "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::imageFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
        CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    Containers::String out;

    TestSuite::Comparator<CompareImageFile> compare{&manager, nullptr, 20.0f, 10.0f};
    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds");
    TestSuite::ComparisonStatusFlags flags = compare(actualFilename, expectedFilename);
    /* Actual file is not, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageCompressed.dds");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* This will attempt to save a DDS and then fails, because we have no DDS
       exporter. */
    Debug d;
    compare.saveDiagnostic(flags, d, COMPAREIMAGETEST_SAVE_DIR);
    CORRADE_VERIFY(!Utility::Path::exists(filename));
}

void CompareImageTest::imageToFileZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    CORRADE_COMPARE_WITH(ExpectedRgb, expectedFilename,
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(ExpectedRgb, expectedFilename),
        TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::imageToFileNonZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(ActualRgb, expectedFilename,
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, expectedFilename);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::imageToFileError() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, expectedFilename);
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageToFilePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&manager, nullptr, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, expectedFilename);
        /* Can't load a plugin, so we can't open the file, so we can't save
           it either and thus no diagnostic */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::imageToFileExpectedLoadFailed() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, "nonexistent.tga");
    /* Actual file *could* be loaded, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Expected image b (nonexistent.tga) could not be loaded.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "nonexistent.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::imageToFileExpectedIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
            CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    Containers::String out;

    TestSuite::Comparator<CompareImageToFile> compare{&manager, nullptr, 20.0f, 10.0f};
    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds");
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb, expectedFilename);
    /* Actual file is not, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out, DEBUGTOOLS_TEST_DIR, "..."),
        "Expected image b (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageCompressed.dds");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    /* This will attempt to save a DDS and then fails, because we have no DDS
       exporter. */
    Debug d;
    compare.saveDiagnostic(flags, d, COMPAREIMAGETEST_SAVE_DIR);
    CORRADE_VERIFY(!Utility::Path::exists(filename));
}

void CompareImageTest::fileToImageZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    CORRADE_COMPARE_WITH(expectedFilename, ExpectedRgb,
        (CompareFileToImage{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(expectedFilename, ExpectedRgb), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::fileToImageNonZeroDelta() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");

    /* This will produce output if --verbose is specified */
    CORRADE_COMPARE_WITH(actualFilename, ExpectedRgb,
        (CompareFileToImage{*_importerManager, 40.0f, 20.0f}));

    Containers::String out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::fileToImageError() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);
}

void CompareImageTest::fileToImagePluginLoadFailed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{"nonexistent"};
    if(manager.loadState("AnyImageImporter") != PluginManager::LoadState::NotFound)
        CORRADE_SKIP("AnyImageImporter plugin found, can't test.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&manager, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga");
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "AnyImageImporter plugin could not be loaded.\n");
}

void CompareImageTest::fileToImageActualLoadFailed() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&*_importerManager, 20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags = compare("nonexistent.tga", ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Actual image a (nonexistent.tga) could not be loaded.\n");
}

void CompareImageTest::fileToImageActualIsCompressed() {
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_INSTALL_DIR};
    if(manager.load("AnyImageImporter") < PluginManager::LoadState::Loaded ||
       manager.load("DdsImporter") < PluginManager::LoadState::Loaded)
            CORRADE_SKIP("AnyImageImporter or DdsImporter plugins can't be loaded.");

    Containers::String out;

    {
        TestSuite::Comparator<CompareFileToImage> compare{&manager, 20.0f, 10.0f};
        /* The filenames are referenced as string views as the assumption is
           that the whole comparison and diagnostic printing gets done in a
           single expression. Thus don't pass them as temporaries to avoid
           dangling views. */
        Containers::String actualFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageCompressed.dds");
        TestSuite::ComparisonStatusFlags flags = compare(actualFilename, ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(Utility::String::replaceFirst(out, DEBUGTOOLS_TEST_DIR, "..."),
        "Actual image a (.../CompareImageCompressed.dds) is compressed, comparison not possible.\n");
}

void CompareImageTest::mutableImageViewImageImageData() {
    /* All other cases use an ImageView, so verify that the other variants all
       compile too */

    char pixels[]{
        '\x56', '\xf8', '\x3a', '\x56', '\x47', '\xec', '\0', '\0',
        '\x23', '\x57', '\x10', '\xab', '\xcd', '\x85', '\0', '\0'
    };
    MutableImageView2D view{PixelFormat::RGB8Unorm, {2, 2}, pixels};

    Image2D image{PixelFormat::RGB8Unorm, {2, 2}, Containers::array({
        '\x56', '\xf8', '\x3a', '\x56', '\x47', '\xec', '\0', '\0',
        '\x23', '\x57', '\x10', '\xab', '\xcd', '\x85', '\0', '\0'
    })};

    Trade::ImageData2D data{PixelFormat::RGB8Unorm, {2, 2}, Trade::DataFlags{}, pixels};

    CORRADE_COMPARE_AS(view, view, CompareImage);
    CORRADE_COMPARE_AS(view, image, CompareImage);
    CORRADE_COMPARE_AS(view, data, CompareImage);
    CORRADE_COMPARE_AS(image, view, CompareImage);
    CORRADE_COMPARE_AS(image, image, CompareImage);
    CORRADE_COMPARE_AS(image, data, CompareImage);
    CORRADE_COMPARE_AS(data, view, CompareImage);
    CORRADE_COMPARE_AS(data, image, CompareImage);
    CORRADE_COMPARE_AS(data, data, CompareImage);
}

void CompareImageTest::mutableImageViewImageImageDataToFile() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* All other cases use an ImageView, so verify that the other variants all
       compile too */

    char pixels[]{
        '\x55', '\xf8', '\x3a', '\x56', '\x10', '\xed', '\0', '\0',
        '\x23', '\x27', '\x10', '\xab', '\xcd', '\xfa', '\0', '\0'
    };
    MutableImageView2D view{PixelFormat::RGB8Unorm, {2, 2}, pixels};

    Image2D image{PixelFormat::RGB8Unorm, {2, 2}, Containers::array({
        '\x55', '\xf8', '\x3a', '\x56', '\x10', '\xed', '\0', '\0',
        '\x23', '\x27', '\x10', '\xab', '\xcd', '\xfa', '\0', '\0'
    })};

    Trade::ImageData2D data{PixelFormat::RGB8Unorm, {2, 2}, Trade::DataFlags{}, pixels};

    Containers::String filename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");
    CORRADE_COMPARE_WITH(filename, view,
        CompareFileToImage{*_importerManager});
    CORRADE_COMPARE_WITH(filename, image,
        CompareFileToImage{*_importerManager});
    CORRADE_COMPARE_WITH(filename, data,
        CompareFileToImage{*_importerManager});
    CORRADE_COMPARE_WITH(view, filename,
        CompareImageToFile{*_importerManager});
    CORRADE_COMPARE_WITH(image, filename,
        CompareImageToFile{*_importerManager});
    CORRADE_COMPARE_WITH(data, filename,
        CompareImageToFile{*_importerManager});
}

template<UnsignedInt dimensions> void CompareImageTest::pixelFormatFor() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    /* Defaults to an integer / float format if no match */
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedByte>>(PixelFormat{})),
        pixelFormat(PixelFormat::R8UI, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Byte>>(PixelFormat{})),
        pixelFormat(PixelFormat::R8I, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedShort>>(PixelFormat{})),
        pixelFormat(PixelFormat::R16UI, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Short>>(PixelFormat{})),
        pixelFormat(PixelFormat::R16I, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedInt>>(PixelFormat{})),
        pixelFormat(PixelFormat::R32UI, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Int>>(PixelFormat{})),
        pixelFormat(PixelFormat::R32I, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Half>>(PixelFormat{})),
        pixelFormat(PixelFormat::R16F, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Float>>(PixelFormat{})),
        pixelFormat(PixelFormat::R32F, dimensions, false));

    /* Matching normalized type if the image has it */
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedByte>>(pixelFormat(PixelFormat::R8Unorm, dimensions, false))),
        pixelFormat(PixelFormat::R8Unorm, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Byte>>(pixelFormat(PixelFormat::R8Snorm, dimensions, false))),
        pixelFormat(PixelFormat::R8Snorm, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedShort>>(pixelFormat(PixelFormat::R16Unorm, dimensions, false))),
        pixelFormat(PixelFormat::R16Unorm, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Short>>(pixelFormat(PixelFormat::R16Snorm, dimensions, false))),
        pixelFormat(PixelFormat::R16Snorm, dimensions, false));

    /* Matching sRGB type if the image has it */
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedByte>>(pixelFormat(PixelFormat::R8Srgb, dimensions, true))),
        pixelFormat(PixelFormat::R8Srgb, dimensions, true));

    /* But not if it has different underlying type */
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, Short>>(pixelFormat(PixelFormat::R8Snorm, dimensions, false))),
        pixelFormat(PixelFormat::R16I, dimensions, false));
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Vector<dimensions, UnsignedShort>>(pixelFormat(PixelFormat::R8Srgb, dimensions, false))),
        pixelFormat(PixelFormat::R16UI, dimensions, false));
}

void CompareImageTest::pixelFormatForColor() {
    /* Defaults to a normalized format if no match */
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color3ub>(PixelFormat{})),
        PixelFormat::RGB8Unorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color4ub>(PixelFormat{})),
        PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Color3<Byte>>(PixelFormat{})),
        PixelFormat::RGB8Snorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Color4<Byte>>(PixelFormat{})),
        PixelFormat::RGBA8Snorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color3us>(PixelFormat{})),
        PixelFormat::RGB16Unorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color4us>(PixelFormat{})),
        PixelFormat::RGBA16Unorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Color3<Short>>(PixelFormat{})),
        PixelFormat::RGB16Snorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Math::Color4<Short>>(PixelFormat{})),
        PixelFormat::RGBA16Snorm);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color3h>(PixelFormat{})),
        PixelFormat::RGB16F);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color4h>(PixelFormat{})),
        PixelFormat::RGBA16F);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color3>(PixelFormat{})),
        PixelFormat::RGB32F);
    CORRADE_COMPARE(
        (Implementation::pixelFormatFor<Color4>(PixelFormat{})),
        PixelFormat::RGBA32F);

    /* Matching sRGB type if the image has it */
    CORRADE_COMPARE(
        Implementation::pixelFormatFor<Color3ub>(PixelFormat::RGB8Srgb),
        PixelFormat::RGB8Srgb);
    CORRADE_COMPARE(
        Implementation::pixelFormatFor<Color4ub>(PixelFormat::RGBA8Srgb),
        PixelFormat::RGBA8Srgb);

    /* But not if it has different underlying type or dimension count */
    CORRADE_COMPARE(
        Implementation::pixelFormatFor<Color3us>(PixelFormat::RGB8Srgb),
        PixelFormat::RGB16Unorm);
    CORRADE_COMPARE(
        Implementation::pixelFormatFor<Color4ub>(PixelFormat::RGB8Srgb),
        PixelFormat::RGBA8Unorm);
}

template<class T> void CompareImageTest::pixelsToImageZeroDelta() {
    auto&& data = PixelsToImageData[testCaseInstanceId()];
    setTestCaseTemplateName(std::is_same<T, Color3ub>::value ? "Color3ub" : "Vector3ub");
    setTestCaseDescription(data.name);

    /* Same as image(), but taking pixels instead. For T being Color3ub, the
       autodetected PixelFormat is RGB8Unorm, for Vector3ub it's RGB8UI. It
       should get matched to either RGB8Unorm or RGB8Srgb based on the format
       in the expected image. */

    /* Same as ExpectedRGB but with pixel format being different */
    const ImageView2D expected{
        PixelStorage{}.setSkip({1, 0, 0}).setRowLength(3),
        pixelFormat(PixelFormat::RGB8Unorm, 3, data.srgb),
        {2, 2}, ExpectedRgbData};

    CORRADE_COMPARE_WITH(expected.pixels<T>(),
        expected, (CompareImage{40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
    CORRADE_COMPARE(compare(expected.pixels<T>(), expected), TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::pixelsToImageNonZeroDelta() {
    /* Same as image(), but taking pixels instead */

    CORRADE_COMPARE_WITH(ActualRgb.pixels<Color3ub>(),
        ExpectedRgb, (CompareImage{40.0f, 20.0f}));

    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(), ExpectedRgb);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::pixelsToImageDifferentFormat() {
    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{{}, {}};
        TestSuite::ComparisonStatusFlags flags =
            compare(ExpectedRgb.pixels<Vector3b>(), ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Images a and b have different format, actual PixelFormat::RGB8I but PixelFormat::RGB8Unorm expected.\n");
}

void CompareImageTest::pixelsToImageError() {
    /* Same as imageError(), but taking pixels instead */

    Containers::String out;

    {
        TestSuite::Comparator<CompareImage> compare{20.0f, 10.0f};
        TestSuite::ComparisonStatusFlags flags =
            compare(ActualRgb.pixels<Color3ub>(), ExpectedRgb);
        /* No diagnostic as we don't have any expected filename */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);
}

template<class T> void CompareImageTest::pixelsToFileZeroDelta() {
    auto&& data = PixelsToImageData[testCaseInstanceId()];
    setTestCaseTemplateName(std::is_same<T, Color3ub>::value ? "Color3ub" : "Vector3ub");
    setTestCaseDescription(data.name);

    /* Same as imageToFile(), but taking pixels instead. For T being Color3ub,
       the autodetected PixelFormat is RGB8Unorm, for Vector3ub it's RGB8UI. It
       should get matched to either RGB8Unorm or RGB8Srgb based on the format
       in the expected image. */

    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    /* Same as ExpectedRGB but with pixel format being different */
    const ImageView2D expected{
        PixelStorage{}.setSkip({1, 0, 0}).setRowLength(3),
        pixelFormat(PixelFormat::RGB8Unorm, 3, data.srgb),
        {2, 2}, ExpectedRgbData};

    CORRADE_COMPARE_WITH(expected.pixels<T>(), expectedFilename,
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    /* No diagnostic as there's no error */
    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
    CORRADE_COMPARE(compare(expected.pixels<T>(), expectedFilename),
        TestSuite::ComparisonStatusFlags{});
}

void CompareImageTest::pixelsToFileNonZeroDelta() {
    /* Same as imageToFile(), but taking pixels instead */

    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    CORRADE_COMPARE_WITH(ActualRgb.pixels<Color3ub>(),
        expectedFilename,
        (CompareImageToFile{*_importerManager, 40.0f, 20.0f}));

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, 40.0f, 20.0f};
        TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(), expectedFilename);
        /* No diagnostic as there's no error */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Verbose);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareVerbose);
}

void CompareImageTest::pixelsToFileDifferentFormat() {
    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    Containers::String out;

    {
        TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, nullptr, {}, {}};
        TestSuite::ComparisonStatusFlags flags =
            compare(ExpectedRgb.pixels<Math::Color3<Byte>>(), expectedFilename);
        /* Diagnostic but we're not checking it, here we just want to make sure
           the right format gets detected */
        CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Images a and b have different format, actual PixelFormat::RGB8Snorm but PixelFormat::RGB8Unorm expected.\n");
}

void CompareImageTest::pixelsToFileError() {
    /* Same as imageToFileError(), but taking pixels instead */

    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* The filenames are referenced as string views as the assumption is that
       the whole comparison and diagnostic printing gets done in a single
       expression. Thus don't pass them as temporaries to avoid dangling
       views. */
    Containers::String expectedFilename = Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageExpected.tga");

    Containers::String out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    /* Vector3ub gets matched to PixelFormat::R8UI initially, but once the
       expected image is loaded it gets updated to PixelFormat::R8Unorm to
       match it */
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Vector3ub>(), expectedFilename);
    /* The diagnostic flag should be slapped on the failure coming from the
       operator() comparing two ImageViews */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, ImageCompareError);

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "CompareImageExpected.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

void CompareImageTest::pixelsToFileExpectedLoadFailed() {
    /* Same as imageToFileExpectedLoadFailed(), but taking pixels instead */

    if(!(_importerManager->loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager->loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Containers::String out;

    TestSuite::Comparator<CompareImageToFile> compare{&*_importerManager, &*_converterManager, 20.0f, 10.0f};
    TestSuite::ComparisonStatusFlags flags = compare(ActualRgb.pixels<Color3ub>(), "nonexistent.tga");
    /* Actual file *could* be loaded, so save it! */
    CORRADE_COMPARE(flags, TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic);

    {
        Debug d{&out, Debug::Flag::DisableColors};
        compare.printMessage(flags, d, "a", "b");
    }

    CORRADE_COMPARE(out, "Expected image b (nonexistent.tga) could not be loaded.\n");

    /* Create the output dir if it doesn't exist, but avoid stale files making
       false positives */
    CORRADE_VERIFY(Utility::Path::make(COMPAREIMAGETEST_SAVE_DIR));
    Containers::String filename = Utility::Path::join(COMPAREIMAGETEST_SAVE_DIR, "nonexistent.tga");
    if(Utility::Path::exists(filename))
        CORRADE_VERIFY(Utility::Path::remove(filename));

    if(!(_converterManager->loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager->loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    {
        out = {};
        Debug redirectOutput(&out);
        compare.saveDiagnostic(flags, redirectOutput, COMPAREIMAGETEST_SAVE_DIR);
    }

    /* We expect the *actual* contents, but under the *expected* filename.
       Comparing file contents, expecting the converter makes exactly the same
       file. */
    CORRADE_COMPARE(out, Utility::format("-> {}\n", filename));
    CORRADE_COMPARE_AS(filename,
        Utility::Path::join(DEBUGTOOLS_TEST_DIR, "CompareImageActual.tga"), TestSuite::Compare::File);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CompareImageTest)
