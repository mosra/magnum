/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Math/Color.h"
#include "Magnum/TextureTools/Sample.h"

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct SampleTest: TestSuite::Tester {
    explicit SampleTest();

    void sample1DSingleElementLinearRgb();
    void sample1DSingleElementLinearRgba();
    void sample1DSingleElementSrgb();
    void sample1DSingleElementSrgbAlpha();

    void sample1DLinearRgb();
    void sample1DLinearRgba();
    void sample1DSrgb();
    void sample1DSrgbAlpha();

    void sample1DInvalid();
};

using namespace Math::Literals;

SampleTest::SampleTest() {
    addTests({&SampleTest::sample1DSingleElementLinearRgb,
              &SampleTest::sample1DSingleElementLinearRgba,
              &SampleTest::sample1DSingleElementSrgb,
              &SampleTest::sample1DSingleElementSrgbAlpha,

              &SampleTest::sample1DLinearRgb,
              &SampleTest::sample1DLinearRgba,
              &SampleTest::sample1DSrgb,
              &SampleTest::sample1DSrgbAlpha,

              &SampleTest::sample1DInvalid});
}

void SampleTest::sample1DSingleElementLinearRgb() {
    /* Should work also with just the Vector base, not just Color3 */
    Vector3ub texture[]{
        0xff3366_rgb
    };

    /* All factors return the same value */
    CORRADE_COMPARE(sampleLinear(texture, 0.0f), 0xff3366_rgbf);
    CORRADE_COMPARE(sampleLinear(texture, 0.6375f), 0xff3366_rgbf);
    CORRADE_COMPARE(sampleLinear(texture, 1.0f), 0xff3366_rgbf);
}

void SampleTest::sample1DSingleElementLinearRgba() {
    /* Should work also with just the Vector base, not just Color3 */
    Vector4ub texture[]{
        0xff336699_rgba
    };

    /* All factors return the same value */
    CORRADE_COMPARE(sampleLinear(texture, 0.0f), 0xff336699_rgbaf);
    CORRADE_COMPARE(sampleLinear(texture, 0.6375f), 0xff336699_rgbaf);
    CORRADE_COMPARE(sampleLinear(texture, 1.0f), 0xff336699_rgbaf);
}

void SampleTest::sample1DSingleElementSrgb() {
    /* Should work also with just the Vector base, not just Color3 */
    Vector3ub texture[]{
        0xff3366_srgb
    };

    /* All factors return the same value, converted from sRGB */
    CORRADE_COMPARE(sampleSrgb(texture, 0.0f), 0xff3366_srgbf);
    CORRADE_COMPARE(sampleSrgb(texture, 0.6375f), 0xff3366_srgbf);
    CORRADE_COMPARE(sampleSrgb(texture, 1.0f), 0xff3366_srgbf);
}

void SampleTest::sample1DSingleElementSrgbAlpha() {
    /* Should work also with just the Vector base, not just Color3 */
    Vector4ub texture[]{
        0xff336699_srgba
    };

    /* All factors return the same value, converted from sRGB */
    CORRADE_COMPARE(sampleSrgbAlpha(texture, 0.0f), 0xff336699_srgbaf);
    CORRADE_COMPARE(sampleSrgbAlpha(texture, 0.6375f), 0xff336699_srgbaf);
    CORRADE_COMPARE(sampleSrgbAlpha(texture, 1.0f), 0xff336699_srgbaf);
}

/* Not const because slice() wouldn't work in that case due to the const
   variant being constexpr and thus not returning a reinterpreted reference
   (lol ffs) */
Color4ub Texture[]{
    0xff336699_rgba, /* 0.0 */
    0xdeadbeef_rgba, /* 0.25 */
    0x2200eeff_rgba, /* 0.5 */
    0xaaccaa33_rgba, /* 0.75 */
    0x996633ff_rgba, /* 1.0 */
};

void SampleTest::sample1DLinearRgb() {
    Containers::StridedArrayView1D<const Color3ub> texture = Containers::stridedArrayView(Texture).slice(&Color4ub::rgb);

    /* These should return exact values */
    CORRADE_COMPARE(sampleLinear(texture, 0.0f), 0xff3366_rgbf);
    CORRADE_COMPARE(sampleLinear(texture, 0.25f), 0xdeadbe_rgbf);
    /* This one should not attempt to lerp with the sentinel value */
    /** @todo once a variant with float input exists, put NaNs in a sentinel */
    CORRADE_COMPARE(sampleLinear(texture, 1.0f), 0x996633_rgbf);

    /* This is an exact 25% / 75% interpolation between element 2 and 3 */
    CORRADE_COMPARE(sampleLinear(texture, 0.5f + 0.0625f), 0x4433dd_rgbf);
    CORRADE_COMPARE(sampleLinear(texture, 0.75f - 0.0625f), 0x8899bb_rgbf);
}

void SampleTest::sample1DLinearRgba() {
    /* These should return exact values */
    CORRADE_COMPARE(sampleLinear(Texture, 0.0f), 0xff336699_rgbaf);
    CORRADE_COMPARE(sampleLinear(Texture, 0.25f), 0xdeadbeef_rgbaf);
    CORRADE_COMPARE(sampleLinear(Texture, 1.0f), 0x996633ff_rgbaf);

    /* This is an exact 25% / 75% interpolation between element 2 and 3 */
    CORRADE_COMPARE(sampleLinear(Texture, 0.5f + 0.0625f), 0x4433ddcc_rgbaf);
    CORRADE_COMPARE(sampleLinear(Texture, 0.75f - 0.0625f), 0x8899bb66_rgbaf);
}

void SampleTest::sample1DSrgb() {
    Containers::StridedArrayView1D<const Color3ub> texture = Containers::stridedArrayView(Texture).slice(&Color4ub::rgb);

    /* These should return exact values, converted from sRGB */
    CORRADE_COMPARE(sampleSrgb(texture, 0.0f), 0xff3366_srgbf);
    CORRADE_COMPARE(sampleSrgb(texture, 0.25f), 0xdeadbe_srgbf);
    CORRADE_COMPARE(sampleSrgb(texture, 1.0f), 0x996633_srgbf);

    /* This is an exact 25% / 75% interpolation between element 2 and 3, but
       with sRGB conversion happening first */
    CORRADE_COMPARE(sampleSrgb(texture, 0.5f + 0.0625f), Math::lerp(0x2200ee_srgbf, 0xaaccaa_srgbf, 0.25f));
    CORRADE_COMPARE(sampleSrgb(texture, 0.75f - 0.0625f), Math::lerp(0x2200ee_srgbf, 0xaaccaa_srgbf, 0.75f));
}

void SampleTest::sample1DSrgbAlpha() {
    /* These should return exact values */
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.0f), 0xff336699_srgbaf);
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.25f), 0xdeadbeef_srgbaf);
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 1.0f), 0x996633ff_srgbaf);

    /* This is an exact 25% / 75% interpolation between element 2 and 3, but
       with sRGB conversion for the RGB channels happening first */
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.5f + 0.0625f), Math::lerp(0x2200eeff_srgbaf, 0xaaccaa33_srgbaf, 0.25f));
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.75f - 0.0625f), Math::lerp(0x2200eeff_srgbaf, 0xaaccaa33_srgbaf, 0.75f));
    /* The literals should handle alpha as linear but verifying it also
       separately just in case -- the channel should have the same value as in
       the sample1DLinearRgba() test */
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.5f + 0.0625f).a(),
        (Math::unpack<Float, UnsignedByte>(0xcc)));
    CORRADE_COMPARE(sampleSrgbAlpha(Texture, 0.75f - 0.0625f).a(),
        (Math::unpack<Float, UnsignedByte>(0x66)));
}

void SampleTest::sample1DInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Color3ub rgb[1];
    Color4ub rgba[1];

    Containers::String out;
    Error redirectError{&out};
    sampleLinear(Containers::ArrayView<const Color3ub>{}, 0.0f);
    sampleLinear(Containers::ArrayView<const Color4ub>{}, 0.0f);
    sampleSrgb(Containers::ArrayView<const Color3ub>{}, 0.0f);
    sampleSrgbAlpha(Containers::ArrayView<const Color4ub>{}, 0.0f);

    sampleLinear(rgb, -0.125f);
    sampleLinear(rgba, 1.125f);
    sampleSrgb(rgb, -Constants::inf());
    sampleSrgbAlpha(rgba, Constants::nan());

    /* MSVC (w/o clang-cl) before 2019 shows -nan(ind) */
    #if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG_CL) && _MSC_VER < 1920
    const char* nan = "-nan(ind)";
    #else
    const char* nan = "nan";
    #endif
    CORRADE_COMPARE_AS(out, Utility::format(
        "TextureTools::sampleLinear(): expected texture to have at least one element\n"
        "TextureTools::sampleLinear(): expected texture to have at least one element\n"
        "TextureTools::sampleSrgb(): expected texture to have at least one element\n"
        "TextureTools::sampleSrgbAlpha(): expected texture to have at least one element\n"

        "TextureTools::sampleLinear(): expected factor to be within the [0, 1] range but got -0.125\n"
        "TextureTools::sampleLinear(): expected factor to be within the [0, 1] range but got 1.125\n"
        "TextureTools::sampleSrgb(): expected factor to be within the [0, 1] range but got -inf\n"
        "TextureTools::sampleSrgbAlpha(): expected factor to be within the [0, 1] range but got {}\n", nan),
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::SampleTest)
