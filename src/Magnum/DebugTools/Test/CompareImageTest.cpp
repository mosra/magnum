/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace DebugTools { namespace Test {

struct CompareImageTest: TestSuite::Tester {
    explicit CompareImageTest();

    void calculateDelta();
    void calculateDeltaStorage();

    void deltaImage();
    void deltaImageScaling();
    void deltaImageColors();

    void pixelDelta();
    void pixelDeltaOverflow();

    void compareDifferentSize();
    void compareDifferentFormat();
    void compareDifferentType();
    void compareSameZeroThreshold();
    void compareAboveThresholds();
    void compareAboveMaxThreshold();
    void compareAboveMeanThreshold();
};

CompareImageTest::CompareImageTest() {
    addTests({&CompareImageTest::calculateDelta,
              &CompareImageTest::calculateDeltaStorage,

              &CompareImageTest::deltaImage,
              &CompareImageTest::deltaImageScaling,
              &CompareImageTest::deltaImageColors,

              &CompareImageTest::pixelDelta,
              &CompareImageTest::pixelDeltaOverflow,

              &CompareImageTest::compareDifferentSize,
              &CompareImageTest::compareDifferentFormat,
              &CompareImageTest::compareDifferentType,
              &CompareImageTest::compareSameZeroThreshold,
              &CompareImageTest::compareAboveThresholds,
              &CompareImageTest::compareAboveMaxThreshold,
              &CompareImageTest::compareAboveMeanThreshold});
}

namespace {
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

    const std::vector<Float> DeltaRed{
        0.35f, 0.0f, 0.3f,
        0.01f, 0.0f, 0.1f,
        0.12f, 1.0f, 0.0f};

    const ImageView2D ActualRed{
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        PixelFormat::Red
        #else
        PixelFormat::Luminance
        #endif
        , PixelType::Float, {3, 3}, ActualRedData};
    const ImageView2D ExpectedRed{
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        PixelFormat::Red
        #else
        PixelFormat::Luminance
        #endif
        , PixelType::Float, {3, 3}, ExpectedRedData};
}

void CompareImageTest::calculateDelta() {
    std::vector<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRed, ExpectedRed);

    CORRADE_COMPARE_AS(delta, DeltaRed, TestSuite::Compare::Container);
    CORRADE_COMPARE(max, 1.0f);
    CORRADE_COMPARE(mean, 0.208889f);
}

namespace {
    /* Different storage for each */
    const UnsignedByte ActualRgbData[] = {
           0,    0,    0,    0,    0,    0,    0,    0,
        0x56, 0xf8, 0x3a, 0x56, 0x47, 0xec,    0,    0,
        0x23, 0x57, 0x10, 0xab, 0xcd, 0x85,    0,    0
    };

    const UnsignedByte ExpectedRgbData[] = {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        0, 0, 0, 0x55, 0xf8, 0x3a, 0x56, 0x10, 0xed, 0, 0, 0,
        0, 0, 0, 0x23, 0x27, 0x10, 0xab, 0xcd, 0xfa, 0, 0, 0
        #else
        0x55, 0xf8, 0x3a, 0x56, 0x10, 0xed, 0, 0,
        0x23, 0x27, 0x10, 0xab, 0xcd, 0xfa, 0, 0,
        #endif
    };

    const ImageView2D ActualRgb{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::RGB, PixelType::UnsignedByte, {2, 2}, ActualRgbData};
    const ImageView2D ExpectedRgb{
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        PixelStorage{}.setSkip({1, 0, 0}).setRowLength(3),
        #endif
        PixelFormat::RGB, PixelType::UnsignedByte, {2, 2}, ExpectedRgbData};
}

void CompareImageTest::calculateDeltaStorage() {
    std::vector<Float> delta;
    Float max, mean;
    std::tie(delta, max, mean) = Implementation::calculateImageDelta(ActualRgb, ExpectedRgb);

    CORRADE_COMPARE_AS(delta, (std::vector<Float>{
        1.0f/3.0f, (55.0f + 1.0f)/3.0f,
        48.0f/3.0f, 117.0f/3.0f
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE(max, 117.0f/3.0f);
    CORRADE_COMPARE(mean, 18.5f);
}

void CompareImageTest::deltaImage() {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::DisableColors};

    std::vector<Float> delta(32*32);

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

    std::vector<Float> delta(65*40);
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

    ImageView2D a{
        #ifndef MAGNUM_TARGET_GLES2
        PixelFormat::RGInteger,
        #else
        PixelFormat::LuminanceAlpha,
        #endif
        PixelType::UnsignedByte, {3, 4}, nullptr};
    ImageView2D b{
        #ifndef MAGNUM_TARGET_GLES2
        PixelFormat::RGInteger,
        #else
        PixelFormat::LuminanceAlpha,
        #endif
        PixelType::UnsignedByte, {3, 5}, nullptr};

    {
        Error e(&out);
        TestSuite::Comparator<CompareImage> compare;
        CORRADE_VERIFY(!compare(a, b));
        compare.printErrorMessage(e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different size, actual Vector(3, 4) but Vector(3, 5) expected.\n");
}

void CompareImageTest::compareDifferentFormat() {
    std::stringstream out;

    ImageView2D a{PixelFormat::RGBA, PixelType::Float, {3, 4}, nullptr};
    ImageView2D b{PixelFormat::RGB, PixelType::Float, {3, 4}, nullptr};

    {
        Error e(&out);
        TestSuite::Comparator<CompareImage> compare;
        CORRADE_VERIFY(!compare(a, b));
        compare.printErrorMessage(e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different format, actual PixelFormat::RGBA/PixelType::Float but PixelFormat::RGB/PixelType::Float expected.\n");
}

void CompareImageTest::compareDifferentType() {
    std::stringstream out;

    ImageView2D a{PixelFormat::RGB, PixelType::UnsignedByte, {3, 4}, nullptr};
    ImageView2D b{PixelFormat::RGB, PixelType::UnsignedShort, {3, 4}, nullptr};

    {
        Error e(&out);
        TestSuite::Comparator<CompareImage> compare;
        CORRADE_VERIFY(!compare(a, b));
        compare.printErrorMessage(e, "a", "b");
    }

    CORRADE_COMPARE(out.str(), "Images a and b have different format, actual PixelFormat::RGB/PixelType::UnsignedByte but PixelFormat::RGB/PixelType::UnsignedShort expected.\n");
}

void CompareImageTest::compareSameZeroThreshold() {
    using namespace Math::Literals;

    const Color3 data[] = {
        0xcafeba_rgbf, 0xdeadbe_rgbf,
        0xbadc0d_rgbf, 0xbeefe0_rgbf
    };

    const ImageView2D image{PixelFormat::RGB, PixelType::Float, {2, 2}, data};
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

}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CompareImageTest)
