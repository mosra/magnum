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

#include <new>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Tweakable.h>
#endif

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, w;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<3, float, Vec3> {
    constexpr static Vector<3, Float> from(const Vec3& other) {
        return {other.x, other.y, other.z};
    }

    constexpr static Vec3 to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct VectorConverter<4, float, Vec4> {
    constexpr static Vector<4, Float> from(const Vec4& other) {
        return {other.x, other.y, other.z, other.w};
    }

    constexpr static Vec4 to(const Vector<4, Float>& other) {
        return {other[0], other[1], other[2], other[3]};
    }
};

}

namespace Test { namespace {

struct ColorTest: TestSuite::Tester {
    explicit ColorTest();

    void construct();
    void constructDefaultAlphaHalf();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
    void constructArray();
    void constructArrayRvalue();
    void constructConversion();
    void constructBit();
    void constructPacking();
    void constructCopy();
    void convert();

    void constructHsv();
    void constructHsvDefault();
    void constructHsvNoInit();
    void constructHsvConversion();
    void constructHsvCopy();
    void compareHsv();

    void data();

    void literals();

    void colors();

    void hue();
    void saturation();
    void value();
    void hsv();
    void fromHsvHueOverflow();
    void fromHsvDefaultAlpha();

    void srgb();
    void fromSrgbDefaultAlpha();
    void srgbToIntegral();
    /* no linearRgbToIntegral() as there's no builtin linear RGB -> integral
       APIs (it's done with pack() instead) */
    void fromIntegralSrgb();
    void fromIntegralLinearRgb();
    void integralSrgbToIntegral();
    void integralLinearRgbToIntegral();
    void srgbMonotonic();
    void srgb8bitRoundtrip();
    void srgbLiterals();

    void xyz();
    void fromXyzDefaultAlpha();
    void xyY();

    void premultiplied();
    template<class T> void premultipliedRoundtrip();
    void unpremultiplied();
    template<class T> void unpremultipliedRoundtrip();

    void multiplyDivideIntegral();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
    void debugUb();
    void debugUbColor();
    void debugUbColorColorsDisabled();
    void debugHsv();

    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    void tweakableRgb();
    void tweakableSrgb();
    void tweakableRgbf();
    void tweakableSrgbf();
    void tweakableRgba();
    void tweakableSrgba();
    void tweakableRgbaf();
    void tweakableSrgbaf();

    void tweakableErrorRgb();
    void tweakableErrorSrgb();
    void tweakableErrorRgba();
    void tweakableErrorSrgba();
    void tweakableErrorRgbf();
    void tweakableErrorSrgbf();
    void tweakableErrorRgbaf();
    void tweakableErrorSrgbaf();
    #endif
};

using Magnum::Vector2;
using Magnum::Vector3;
using Magnum::Color3;

using Magnum::Vector4;
using Magnum::Color4;

using Magnum::ColorHsv;

using Magnum::Deg;

using namespace Literals;

const struct {
    Int r, g, b;
} UnpremultipliedRoundtripData[]{
    {10, 8, 2}, /* same as in premultipliedRoundtrip() */
    {4, 5, 0},
    {9, 6, 7},
    /** @todo for 1 and 3 it results in less precision, what to do? */
};

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
const struct {
    const char* name;
    const char* dataRgb;
    const char* dataRgba;
    Color4 result;
    Color4 resultSrgba;
    Color4ub resultUb;
} TweakableData[] {
    {"lowercase", "0xff3366_{}", "0xff3366aa_{}", 0xff3366aa_rgbaf, 0xff3366aa_srgbaf, 0xff3366aa_rgba},
    {"uppercase", "0XFF3366_{}", "0XFF3366AA_{}", 0xff3366aa_rgbaf, 0xff3366aa_srgbaf, 0xff3366aa_rgba}
};

constexpr struct {
    const char* name;
    const char* data;
    Utility::TweakableState state;
    const char* error;
} TweakableErrorData[] {
    {"empty", "", Utility::TweakableState::Recompile,
        "Utility::TweakableParser:  is not a hexadecimal color literal\n"},
    {"char", "'a'", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: 'a' is not a hexadecimal color literal\n"},
    {"not hex", "{}_{}", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: {0}_{2}{1} is not a hexadecimal color literal\n"},
    {"garbage after", "0x{}._{}", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: unexpected characters ._{2}{1} after a color literal\n"},
    {"different suffix", "0x{}f", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: 0x{0}f has an unexpected suffix, expected _{1} or _s{1}\n"},
    {"bad size", "0x333_{1}", Utility::TweakableState::Error,
        "Utility::TweakableParser: 0x333_{2}{1} doesn't have expected number of digits\n"},
};
#endif

ColorTest::ColorTest() {
    addTests({&ColorTest::construct,
              &ColorTest::constructDefaultAlphaHalf,
              &ColorTest::constructDefault,
              &ColorTest::constructNoInit,
              &ColorTest::constructOneValue,
              &ColorTest::constructParts,
              &ColorTest::constructArray,
              &ColorTest::constructArrayRvalue,
              &ColorTest::constructConversion,
              &ColorTest::constructBit,
              &ColorTest::constructPacking,
              &ColorTest::constructCopy,
              &ColorTest::convert,

              &ColorTest::constructHsv,
              &ColorTest::constructHsvDefault,
              &ColorTest::constructHsvNoInit,
              &ColorTest::constructHsvConversion,
              &ColorTest::constructHsvCopy,
              &ColorTest::compareHsv,

              &ColorTest::data,

              &ColorTest::literals,

              &ColorTest::colors,

              &ColorTest::hue,
              &ColorTest::saturation,
              &ColorTest::value,
              &ColorTest::hsv,
              &ColorTest::fromHsvHueOverflow,
              &ColorTest::fromHsvDefaultAlpha,

              &ColorTest::srgb,
              &ColorTest::fromSrgbDefaultAlpha,
              &ColorTest::srgbToIntegral,
              &ColorTest::fromIntegralSrgb,
              &ColorTest::fromIntegralLinearRgb,
              &ColorTest::integralSrgbToIntegral,
              &ColorTest::integralLinearRgbToIntegral});

    /* Comparing with the previous one, so not 65536 */
    addRepeatedTests({&ColorTest::srgbMonotonic}, 65535);

    addRepeatedTests({&ColorTest::srgb8bitRoundtrip}, 256);

    addTests({&ColorTest::srgbLiterals,

              &ColorTest::xyz,
              &ColorTest::fromXyzDefaultAlpha,
              &ColorTest::xyY,

              &ColorTest::premultiplied,
              &ColorTest::premultipliedRoundtrip<UnsignedByte>,
              &ColorTest::premultipliedRoundtrip<UnsignedShort>,
              &ColorTest::unpremultiplied});

    addInstancedTests<ColorTest>({
        &ColorTest::unpremultipliedRoundtrip<UnsignedByte>,
        &ColorTest::unpremultipliedRoundtrip<UnsignedShort>},
        Containers::arraySize(UnpremultipliedRoundtripData));

    addTests({&ColorTest::multiplyDivideIntegral,

              &ColorTest::strictWeakOrdering,

              &ColorTest::swizzleType,
              &ColorTest::debug,
              &ColorTest::debugUb,
              &ColorTest::debugUbColor,
              &ColorTest::debugUbColorColorsDisabled,
              &ColorTest::debugHsv});

    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    addInstancedTests({&ColorTest::tweakableRgb,
                       &ColorTest::tweakableSrgb,
                       &ColorTest::tweakableRgba,
                       &ColorTest::tweakableSrgba,
                       &ColorTest::tweakableRgbf,
                       &ColorTest::tweakableSrgbf,
                       &ColorTest::tweakableRgbaf,
                       &ColorTest::tweakableSrgbaf},
        Containers::arraySize(TweakableData));

    addInstancedTests({&ColorTest::tweakableErrorRgb,
                       &ColorTest::tweakableErrorSrgb,
                       &ColorTest::tweakableErrorRgba,
                       &ColorTest::tweakableErrorSrgba,
                       &ColorTest::tweakableErrorRgbf,
                       &ColorTest::tweakableErrorSrgbf,
                       &ColorTest::tweakableErrorRgbaf,
                       &ColorTest::tweakableErrorSrgbaf},
        Containers::arraySize(TweakableErrorData));
    #endif
}

void ColorTest::construct() {
    constexpr Color3 a = {1.0f, 0.5f, 0.75f};
    CORRADE_COMPARE(a, Vector3(1.0f, 0.5f, 0.75f));

    constexpr Color4 b = {1.0f, 0.5f, 0.75f, 0.5f};
    CORRADE_COMPARE(b, Vector4(1.0f, 0.5f, 0.75f, 0.5f));

    /* Default alpha */
    constexpr Color4 c = {1.0f, 0.5f, 0.75f};
    constexpr Color4ub d = {10, 25, 176};
    CORRADE_COMPARE(c, Vector4(1.0f, 0.5f, 0.75f, 1.0f));
    CORRADE_COMPARE(d, Vector4ub(10, 25, 176, 255));

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, Float, Float, Float>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, Float, Float, Float, Float>::value);
}

void ColorTest::constructDefaultAlphaHalf() {
    /* The default for alpha should work also for the Half type */
    Color4h a{1.0_h, 0.5_h, 0.75_h};
    CORRADE_COMPARE(a, Vector4h(1.0_h, 0.5_h, 0.75_h, 1.0_h));
}

void ColorTest::constructDefault() {
    constexpr Color3 a1;
    constexpr Color3 a2{ZeroInit};
    CORRADE_COMPARE(a1, Color3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(a2, Color3(0.0f, 0.0f, 0.0f));

    constexpr Color4 b1;
    constexpr Color4 b2{ZeroInit};
    CORRADE_COMPARE(b1, Color4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b2, Color4(0.0f, 0.0f, 0.0f, 0.0f));

    constexpr Color4ub c;
    CORRADE_COMPARE(c, Color4ub(0, 0, 0, 0));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Color3>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Color4>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, ZeroInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Color4>::value);
}

void ColorTest::constructNoInit() {
    Color3 a{1.0f, 0.5f, 0.75f};
    Color4 b{1.0f, 0.5f, 0.75f, 0.5f};
    new(&a) Color3{Magnum::NoInit};
    new(&b) Color4{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Color3{1.0f, 0.5f, 0.75f}));
        CORRADE_COMPARE(b, (Color4{1.0f, 0.5f, 0.75f, 0.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, Magnum::NoInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Color4>::value);
}

void ColorTest::constructOneValue() {
    constexpr Color3 a(0.25f);
    CORRADE_COMPARE(a, Color3(0.25f, 0.25f, 0.25f));

    constexpr Color4 b(0.25f, 0.5f);
    CORRADE_COMPARE(b, Color4(0.25f, 0.25f, 0.25f, 0.5f));

    /* Default alpha */
    constexpr Color4 c(0.25f);
    constexpr Color4ub d(67);
    CORRADE_COMPARE(c, Color4(0.25f, 0.25f, 0.25f, 1.0f));
    CORRADE_COMPARE(d, Color4ub(67, 67, 67, 255));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<Float, Color4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, Float>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, Float, Float>::value);
}

void ColorTest::constructParts() {
    constexpr Vector2 a{1.0f, 0.5f};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea, who cares */
    #endif
    Color3 b = {a, 0.75f};
    CORRADE_COMPARE(b, (Color3{1.0f, 0.5f, 0.75f}));

    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea, who cares */
    #endif
    Color4 c = {b, 0.25f};
    CORRADE_COMPARE(c, (Color4{1.0f, 0.5f, 0.75f, 0.25f}));

    /* Default alpha */
    constexpr Color3ub d(10, 25, 176);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea, who cares */
    #endif
    Color4 e = b;
    constexpr Color4ub f = d;
    CORRADE_COMPARE(e, (Color4{1.0f, 0.5f, 0.75f, 1.0f}));
    CORRADE_COMPARE(f, (Color4ub{10, 25, 176, 255}));

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, Vector2, Float>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, Color3, Float>::value);
}

void ColorTest::constructArray() {
    float data3[]{1.3f, 2.7f, -15.0f};
    float data4[]{1.3f, 2.7f, -15.0f, 7.0f};
    Color3 a3{data3};
    Color4 a4{data4};
    CORRADE_COMPARE(a3, (Color3{1.3f, 2.7f, -15.0f}));
    CORRADE_COMPARE(a4, (Color4{1.3f, 2.7f, -15.0f, 7.0f}));

    constexpr float cdata3[]{1.3f, 2.7f, -15.0f};
    constexpr float cdata4[]{1.3f, 2.7f, -15.0f, 7.0f};
    constexpr Color3 ca3{cdata3};
    constexpr Color4 ca4{cdata4};
    CORRADE_COMPARE(ca3, (Color3{1.3f, 2.7f, -15.0f}));
    CORRADE_COMPARE(ca4, (Color4{1.3f, 2.7f, -15.0f, 7.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[3], Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<float[4], Color4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, float[3]>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, float[4]>::value);

    /* See VectorTest::constructArray() for details */
    #if 0
    float data1[]{1.3f};
    float data5[]{1.3f, 2.7f, -15.0f, 7.0f, 22.6f};
    Color3 b3{data1};
    Color4 b4{data1};
    Color3 c3{data5};
    Color4 c4{data5};
    #endif
}

void ColorTest::constructArrayRvalue() {
    /* Silly but why not. Could theoretically help with some fancier types
       that'd otherwise require explicit typing with the variadic
       constructor. */
    Color3 a3{{1.3f, 2.7f, -15.0f}};
    Color4 a4{{1.3f, 2.7f, -15.0f, 7.0f}};
    CORRADE_COMPARE(a3, (Color3{1.3f, 2.7f, -15.0f}));
    CORRADE_COMPARE(a4, (Color4{1.3f, 2.7f, -15.0f, 7.0f}));

    constexpr Color3 ca3{{1.3f, 2.7f, -15.0f}};
    constexpr Color4 ca4{{1.3f, 2.7f, -15.0f, 7.0f}};
    CORRADE_COMPARE(ca3, (Color3{1.3f, 2.7f, -15.0f}));
    CORRADE_COMPARE(ca4, (Color4{1.3f, 2.7f, -15.0f, 7.0f}));

    /* See VectorTest::constructArrayRvalue() for details */
    #if 0
    Color3 c3{{1.3f, 2.7f, -15.0f, 7.0f}};
    Color4 c4{{1.3f, 2.7f, -15.0f, 7.0f, 22.6f}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a Color from a smaller array isn't an error on GCC 4.8.");
    Color3 b3{{1.3f, 2.7f}};
    Color4 b4{{1.3f, 2.7f}};
    #endif
}

void ColorTest::constructConversion() {
    constexpr Color3 a(10.1f, 12.5f, 0.75f);
    constexpr Color3ub b(a);
    CORRADE_COMPARE(b, Color3ub(10, 12, 0));

    constexpr Color4 c(10.1f, 12.5f, 0.75f, 5.25f);
    constexpr Color4ub d(c);
    CORRADE_COMPARE(d, Color4ub(10, 12, 0, 5));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Color3, Color3ub>::value);
    CORRADE_VERIFY(!std::is_convertible<Color4, Color4ub>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3ub, Color3>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, Color4ub>::value);
}

void ColorTest::constructBit() {
    BitVector3 a3{'\x5'}; /* 0b101 */
    BitVector4 a4{'\xa'}; /* 0b1010 */
    CORRADE_COMPARE(Color3{a3}, (Color3{1.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color4ub{a4}, (Color4ub{0, 1, 0, 1}));

    constexpr BitVector3 ca3{'\x5'}; /* 0b101 */
    constexpr BitVector4 ca4{'\xa'}; /* 0b1010 */
    constexpr Color3 cb3{ca3};
    constexpr Color4ub cb4{ca4};
    CORRADE_COMPARE(cb3, (Color3{1.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(cb4, (Color4ub{0, 1, 0, 1}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector3, Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<BitVector4, Color4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Color3, BitVector3>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Color4, BitVector4>::value);
}

void ColorTest::constructPacking() {
    constexpr Color3 a(1.0f, 0.5f, 0.75f);
    auto b = Math::pack<Color3ub>(a);
    CORRADE_COMPARE(b, Color3ub(255, 128, 191));

    constexpr Color4 c(1.0f, 0.5f, 0.75f, 0.25f);
    auto d = Math::pack<Color4ub>(c);
    CORRADE_COMPARE(d, Color4ub(255, 128, 191, 64));
}

void ColorTest::constructCopy() {
    constexpr Math::Vector<3, Float> a(1.0f, 0.5f, 0.75f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Color3 b(a);
    CORRADE_COMPARE(b, Color3(1.0f, 0.5f, 0.75f));

    constexpr Math::Vector<4, Float> c(1.0f, 0.5f, 0.75f, 0.25f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Color4 d(c);
    CORRADE_COMPARE(d, Color4(1.0f, 0.5f, 0.75f, 0.25f));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Color3>::value);
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Color4>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Color3>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Color4>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Color3>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Color4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Color3>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Color4>::value);
}

void ColorTest::convert() {
    constexpr Vec3 a3{1.5f, 2.0f, -3.5f};
    constexpr Color3 b3{1.5f, 2.0f, -3.5f};

    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea what's wrong. For Vector3 it "just works" */
    #endif
    Color3 c3(a3);
    CORRADE_COMPARE(c3, b3);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d3 = Vec3(b3);
    #else
    constexpr Vec3 d3(b3);
    #endif
    CORRADE_COMPARE(d3.x, a3.x);
    CORRADE_COMPARE(d3.y, a3.y);
    CORRADE_COMPARE(d3.z, a3.z);

    constexpr Vec4 a4{1.5f, 2.0f, -3.5f, -0.5f};
    constexpr Color4 b4{1.5f, 2.0f, -3.5f, -0.5f};

    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea what's wrong. For Vector4 it "just works" */
    #endif
    Color4 c4(a4);
    CORRADE_COMPARE(c4, b4);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d4 = Vec4(b4);
    #else
    constexpr Vec4 d4(b4);
    #endif
    CORRADE_COMPARE(d4.x, a4.x);
    CORRADE_COMPARE(d4.y, a4.y);
    CORRADE_COMPARE(d4.z, a4.z);
    CORRADE_COMPARE(d4.w, a4.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vec3, Color3>::value);
    CORRADE_VERIFY(!std::is_convertible<Vec4, Color4>::value);
    CORRADE_VERIFY(!std::is_convertible<Color3, Vec3>::value);
    CORRADE_VERIFY(!std::is_convertible<Color4, Vec4>::value);
}

void ColorTest::constructHsv() {
    ColorHsv a{135.0_degf, 0.5f, 0.9f};
    CORRADE_COMPARE(a.hue, 135.0_degf);
    CORRADE_COMPARE(a.saturation, 0.5f);
    CORRADE_COMPARE(a.value, 0.9f);

    constexpr ColorHsv ca{135.0_degf, 0.5f, 0.9f};
    constexpr Deg hue = ca.hue;
    constexpr Float saturation = ca.saturation;
    constexpr Float value = ca.value;
    CORRADE_COMPARE(hue, 135.0_degf);
    CORRADE_COMPARE(saturation, 0.5f);
    CORRADE_COMPARE(value, 0.9f);

    CORRADE_VERIFY(std::is_nothrow_constructible<ColorHsv, Deg, Float, Float>::value);
}

void ColorTest::constructHsvDefault() {
    ColorHsv a1;
    ColorHsv a2{ZeroInit};
    CORRADE_COMPARE(a1, (ColorHsv{0.0_degf, 0.0f, 0.0f}));
    CORRADE_COMPARE(a2, (ColorHsv{0.0_degf, 0.0f, 0.0f}));

    constexpr ColorHsv ca1;
    constexpr ColorHsv ca2{ZeroInit};
    CORRADE_COMPARE(ca1, (ColorHsv{0.0_degf, 0.0f, 0.0f}));
    CORRADE_COMPARE(ca2, (ColorHsv{0.0_degf, 0.0f, 0.0f}));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<ColorHsv>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<ColorHsv, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, ColorHsv>::value);
}

void ColorTest::constructHsvNoInit() {
    ColorHsv a{135.0_degf, 0.5f, 0.9f};
    new(&a) ColorHsv{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (ColorHsv{135.0_degf, 0.5f, 0.9f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<ColorHsv, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, ColorHsv>::value);
}

void ColorTest::constructHsvConversion() {
    typedef Math::ColorHsv<Double> ColorHsvd;

    /* Converting from double to float and not the other way around because
       0.9f is 0.899999976158142 */
    ColorHsvd a{135.0_deg, 0.5, 0.9};
    ColorHsv b{a};
    CORRADE_COMPARE(b, (ColorHsv(135.0_degf, 0.5f, 0.9f)));

    constexpr ColorHsvd ca{135.0_deg, 0.5, 0.9};
    constexpr ColorHsv cb{ca};
    CORRADE_COMPARE(cb, (ColorHsv(135.0_degf, 0.5f, 0.9f)));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ColorHsvd, ColorHsv>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<ColorHsv, ColorHsvd>::value);
}

void ColorTest::constructHsvCopy() {
    ColorHsv a{135.0_degf, 0.5f, 0.9f};
    ColorHsv b{a};
    CORRADE_COMPARE(b, (ColorHsv(135.0_degf, 0.5f, 0.9f)));

    constexpr ColorHsv ca{135.0_degf, 0.5f, 0.9f};
    constexpr ColorHsv cb{ca};
    CORRADE_COMPARE(cb, (ColorHsv(135.0_degf, 0.5f, 0.9f)));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<ColorHsv>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<ColorHsv>::value);
}

void ColorTest::compareHsv() {
    CORRADE_VERIFY(ColorHsv{135.0_degf, 0.5f, 0.9f} == ColorHsv{135.0_degf + TypeTraits<Float>::epsilon()*100.0_degf, 0.5f, 0.9f});
    CORRADE_VERIFY(ColorHsv{135.0_degf, 0.5f, 0.9f} != ColorHsv{135.0_degf + TypeTraits<Float>::epsilon()*400.0_degf, 0.5f, 0.9f});

    CORRADE_VERIFY(ColorHsv{135.0_degf, 0.5f, 0.9f} == ColorHsv{135.0_degf, 0.5f, 0.9f + TypeTraits<Float>::epsilon()*0.5f});
    CORRADE_VERIFY(ColorHsv{135.0_degf, 0.5f, 0.9f} != ColorHsv{135.0_degf, 0.5f, 0.9f + TypeTraits<Float>::epsilon()*2.0f});
}

void ColorTest::data() {
    Color4 c{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr const Color4 cc{1.0f, 2.0f, 3.0f, 4.0f};

    Color3 c3a = c.rgb();
    Color3 c3b = c.xyz();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea what's wrong. For Vector4 it "just works" */
    #endif
    Color3 cc3a{cc.rgb()};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY
    constexpr /* No idea what's wrong. For Vector4 it "just works" */
    #endif
    Color3 cc3b{cc.xyz()};
    CORRADE_COMPARE(c3a, (Color3{1.0f, 2.0f, 3.0f}));
    CORRADE_COMPARE(c3b, (Color3{1.0f, 2.0f, 3.0f}));
    CORRADE_COMPARE(cc3a, (Color3{1.0f, 2.0f, 3.0f}));
    CORRADE_COMPARE(cc3b, (Color3{1.0f, 2.0f, 3.0f}));

    CORRADE_VERIFY(std::is_same<decltype(c.xyz()), Color3&>::value);
    CORRADE_VERIFY(std::is_same<decltype(cc.xyz()), const Color3>::value);
    CORRADE_VERIFY(std::is_same<decltype(c.rgb()), Color3&>::value);
    CORRADE_VERIFY(std::is_same<decltype(cc.rgb()), const Color3>::value);
}

void ColorTest::literals() {
    CORRADE_COMPARE(0x33b27f_rgb, (Color3ub{0x33, 0xb2, 0x7f}));
    CORRADE_COMPARE(0x33b27fcc_rgba, (Color4ub{0x33, 0xb2, 0x7f, 0xcc}));
    CORRADE_COMPARE(0x33b27f_rgbf, (Color3{0.2f, 0.698039f, 0.498039f}));
    CORRADE_COMPARE(0x33b27fcc_rgbaf, (Color4{0.2f, 0.698039f, 0.498039f, 0.8f}));

    /* As the implementation doesn't delegate into unpack() etc in order to be
       constexpr, test also boundary values to be sure */
    CORRADE_COMPARE(0xffffff_rgbf, Color3{1.0f});
    CORRADE_COMPARE(0x000000_rgbf, Color3{0.0f});
    CORRADE_COMPARE(0xffffffff_rgbaf, Color4{1.0f});
    CORRADE_COMPARE(0x00000000_rgbaf, (Color4{0.0f, 0.0f}));

    constexpr Color3ub ca = 0x33b27f_rgb;
    constexpr Color4ub cb = 0x33b27fcc_rgba;
    constexpr Color3 cc = 0x33b27f_rgbf;
    constexpr Color4 cd = 0x33b27fcc_rgbaf;
    CORRADE_COMPARE(ca, (Color3ub{0x33, 0xb2, 0x7f}));
    CORRADE_COMPARE(cb, (Color4ub{0x33, 0xb2, 0x7f, 0xcc}));
    CORRADE_COMPARE(cc, (Color3{0.2f, 0.698039f, 0.498039f}));
    CORRADE_COMPARE(cd, (Color4{0.2f, 0.698039f, 0.498039f, 0.8f}));
}

void ColorTest::colors() {
    CORRADE_COMPARE(Color3::red(0.289f), (Color3{0.289f, 0.0f, 0.0f}));
    CORRADE_COMPARE(Color3::green(0.289f), (Color3{0.0f, 0.289f, 0.0f}));
    CORRADE_COMPARE(Color3::blue(0.289f), (Color3{0.0f, 0.0f, 0.289f}));

    CORRADE_COMPARE(Color3::cyan(0.289f), (Color3{0.289f, 1.0f, 1.0f}));
    CORRADE_COMPARE(Color3::magenta(0.289f), (Color3{1.0f, 0.289f, 1.0f}));
    CORRADE_COMPARE(Color3::yellow(0.289f), (Color3{1.0f, 1.0f, 0.289f}));

    CORRADE_COMPARE(Color4::red(0.289f, 0.47f), (Color4{0.289f, 0.0f, 0.0f, 0.47f}));
    CORRADE_COMPARE(Color4::green(0.289f, 0.47f), (Color4{0.0f, 0.289f, 0.0f, 0.47f}));
    CORRADE_COMPARE(Color4::blue(0.289f, 0.47f), (Color4{0.0f, 0.0f, 0.289f, 0.47f}));

    CORRADE_COMPARE(Color4::cyan(0.289f, 0.47f), (Color4{0.289f, 1.0f, 1.0f, 0.47f}));
    CORRADE_COMPARE(Color4::magenta(0.289f, 0.47f), (Color4{1.0f, 0.289f, 1.0f, 0.47f}));
    CORRADE_COMPARE(Color4::yellow(0.289f, 0.47f), (Color4{1.0f, 1.0f, 0.289f, 0.47f}));

    /* Default values */
    CORRADE_COMPARE(Color3::red(), (Color3{1.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE(Color3::green(), (Color3{0.0f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::blue(), (Color3{0.0f, 0.0f, 1.0f}));

    CORRADE_COMPARE(Color4::red(), (Color4{1.0f, 0.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color4::green(), (Color4{0.0f, 1.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color4::blue(), (Color4{0.0f, 0.0f, 1.0f, 1.0f}));

    CORRADE_COMPARE(Color4::cyan(), (Color4{0.0f, 1.0f, 1.0f, 1.0f}));
    CORRADE_COMPARE(Color4::magenta(), (Color4{1.0f, 0.0f, 1.0f, 1.0f}));
    CORRADE_COMPARE(Color4::yellow(), (Color4{1.0f, 1.0f, 0.0f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Color3us::red(), (Color3us{65535, 0, 0}));
    CORRADE_COMPARE(Color3us::green(), (Color3us{0, 65535, 0}));
    CORRADE_COMPARE(Color3us::blue(), (Color3us{0, 0, 65535}));

    CORRADE_COMPARE(Color4us::red(), (Color4us{65535, 0, 0, 65535}));
    CORRADE_COMPARE(Color4us::green(), (Color4us{0, 65535, 0, 65535}));
    CORRADE_COMPARE(Color4us::blue(), (Color4us{0, 0, 65535, 65535}));

    CORRADE_COMPARE(Color4us::cyan(), (Color4us{0, 65535, 65535, 65535}));
    CORRADE_COMPARE(Color4us::magenta(), (Color4us{65535, 0, 65535, 65535}));
    CORRADE_COMPARE(Color4us::yellow(), (Color4us{65535, 65535, 0, 65535}));
}

void ColorTest::hue() {
    CORRADE_COMPARE(Color3::fromHsv({ 27.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({ 86.0_degf, 1.0f, 1.0f}), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({134.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv({191.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({269.0_degf, 1.0f, 1.0f}), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({317.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.0f, 0.716667f}));

    CORRADE_COMPARE((Color3{1.0f, 0.45f, 0.0f}).hue(),      27.0_degf);
    CORRADE_COMPARE((Color3{0.566667f, 1.0f, 0.0f}).hue(),  86.0_degf);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.233333f}).hue(), 134.0_degf);
    CORRADE_COMPARE((Color3{0.0f, 0.816667f, 1.0f}).hue(), 191.0_degf);
    CORRADE_COMPARE((Color3{0.483333f, 0.0f, 1.0f}).hue(), 269.0_degf);
    CORRADE_COMPARE((Color3{1.0f, 0.0f, 0.716667f}).hue(), 317.0_degf);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv({27.0_degf, 1.0f, 1.0f}, 0.175f), (Color4{1.0f, 0.45f, 0.0f, 0.175f}));
    CORRADE_COMPARE((Color4{1.0f, 0.45f, 0.0f, 0.175f}).hue(), 27.0_degf);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Color3us::fromHsv({27.0_degf, 1.0f, 1.0f}),
        (Color3us{65535, 29491, 0}));
    CORRADE_COMPARE(Color4us::fromHsv({27.0_degf, 1.0f, 1.0f}, 15239),
        (Color4us{65535, 29491, 0, 15239}));
    CORRADE_COMPARE((Color3us{65535, 29490, 0}).hue(), 26.9993_degf);
    CORRADE_COMPARE((Color4us{65535, 29490, 0, 15239}.hue()), 26.9993_degf);
}

void ColorTest::saturation() {
    CORRADE_COMPARE(Color3::fromHsv({0.0_degf, 0.702f, 1.0f}), (Color3{1.0f, 0.298f, 0.298f}));
    CORRADE_COMPARE((Color3{1.0f, 0.298f, 0.298f}).saturation(), 0.702f);

    /* Extremes */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).saturation(), 0.0f);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.0f}).saturation(), 1.0f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv({0.0_degf, 0.702f, 1.0f}, 0.175f), (Color4{1.0f, 0.298f, 0.298f, 0.175f}));
    CORRADE_COMPARE((Color4{1.0f, 0.298f, 0.298f, 0.175f}).saturation(), 0.702f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Color3us::fromHsv({0.0_degf, 0.702f, 1.0f}),
        (Color3us{65535, 19529, 19529}));
    CORRADE_COMPARE(Color4us::fromHsv({0.0_degf, 0.702f, 1.0f}, 15239),
        (Color4us{65535, 19529, 19529, 15239}));
    CORRADE_COMPARE((Color3us{65535, 19529, 19529}.saturation()), 0.702007f);
    CORRADE_COMPARE((Color4us{65535, 19529, 19529, 15239}.saturation()), 0.702007f);
}

void ColorTest::value() {
    CORRADE_COMPARE(Color3::fromHsv({0.0_degf, 1.0f, 0.522f}), (Color3{0.522f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color3{0.522f, 0.0f, 0.0f}).value(), 0.522f);

    /* Extremes */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).value(), 0.0f);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.0f}).value(), 1.0f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv({0.0_degf, 1.0f, 0.522f}, 0.175f), (Color4{0.522f, 0.0f, 0.0f, 0.175f}));
    CORRADE_COMPARE((Color4{0.522f, 0.0f, 0.0f, 0.175f}).value(), 0.522f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Color3us::fromHsv({0.0_degf, 1.0f, 0.522f}),
        (Color3us{34209, 0, 0}));
    CORRADE_COMPARE(Color4us::fromHsv({0.0_degf, 1.0f, 0.522f}, 15239),
        (Color4us{34209, 0, 0, 15239}));
    CORRADE_COMPARE((Color3us{34209, 0, 0}.value()), 0.521996f);
    CORRADE_COMPARE((Color4us{34209, 0, 0, 15239}.value()), 0.521996f);
}

void ColorTest::hsv() {
    CORRADE_COMPARE(Color3::fromHsv({230.0_degf, 0.749f, 0.427f}),
        (Color3{0.107177f, 0.160481f, 0.427f}));

    ColorHsv hsv = Color3{0.107177f, 0.160481f, 0.427f}.toHsv();
    CORRADE_COMPARE(hsv.hue, 230.0_degf);
    CORRADE_COMPARE(hsv.saturation, 0.749f);
    CORRADE_COMPARE(hsv.value, 0.427f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv({230.0_degf, 0.749f, 0.427f}, 0.175f),
        (Color4{0.107177f, 0.160481f, 0.427f, 0.175f}));

    hsv = Color4{0.107177f, 0.160481f, 0.427f, 0.175f}.toHsv();
    CORRADE_COMPARE(hsv.hue, 230.0_degf);
    CORRADE_COMPARE(hsv.saturation, 0.749f);
    CORRADE_COMPARE(hsv.value, 0.427f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Color3us::fromHsv({230.0_degf, 0.749f, 0.427f}),
        (Color3us{7024, 10517, 27983}));
    CORRADE_COMPARE(Color4us::fromHsv({230.0_degf, 0.749f, 0.427f}, 15239),
        (Color4us{7024, 10517, 27983, 15239}));

    hsv = Color3us{7023, 10517, 27983}.toHsv();
    CORRADE_COMPARE(hsv.hue, 230.0_degf);
    CORRADE_COMPARE(hsv.saturation, 0.749026f);
    CORRADE_COMPARE(hsv.value, 0.426993f);

    hsv = Color4us{7023, 10517, 27983, 15239}.toHsv();
    CORRADE_COMPARE(hsv.hue, 230.0_degf);
    CORRADE_COMPARE(hsv.saturation, 0.749026f);
    CORRADE_COMPARE(hsv.value, 0.426993f);

    /* Round-trip */
    CORRADE_COMPARE(Color3::fromHsv({230.0_degf, 0.749f, 0.427f}).toHsv(),
                    (ColorHsv{230.0_degf, 0.749f, 0.427f}));
    CORRADE_COMPARE(Color4::fromHsv({230.0_degf, 0.749f, 0.427f}, 0.175f).toHsv(),
                    (ColorHsv{230.0_degf, 0.749f, 0.427f}));
}

void ColorTest::fromHsvHueOverflow() {
    CORRADE_COMPARE(Color3::fromHsv({ 27.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({ 86.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({134.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv({191.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({269.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({317.0_degf - 360.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.0f, 0.716667f}));

    CORRADE_COMPARE(Color3::fromHsv({ 27.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({ 86.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv({134.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv({191.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({269.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv({317.0_degf + 360.0_degf, 1.0f, 1.0f}), (Color3{1.0f, 0.0f, 0.716667f}));
}

void ColorTest::fromHsvDefaultAlpha() {
    CORRADE_COMPARE(Color4::fromHsv({230.0_degf, 0.749f, 0.427f}),
        (Color4{0.107177f, 0.160481f, 0.427f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Color4us::fromHsv({230.0_degf, 0.749f, 0.427f}),
        (Color4us{7024, 10517, 27983, 65535}));
}

void ColorTest::srgb() {
    /* Linear start */
    {
        Vector3 srgb1{0.01292f, 0.01938f, 0.0437875f};
        Vector3 srgb2{0.02584f, 0.03876f, 0.0768655f};
        Color3 linear1{0.001f, 0.0015f, 0.0034f};
        Color3 linear2{0.002f, 0.0030f, 0.0068f};
        CORRADE_COMPARE(Color3::fromSrgb(srgb1), linear1);
        CORRADE_COMPARE(Color3::fromSrgb(srgb2), linear2);
        CORRADE_COMPARE(linear1.toSrgb(), srgb1);
        CORRADE_COMPARE(linear2.toSrgb(), srgb2);
    }

    /* Power series */
    {
        Vector3 srgb1{0.00646f, 0.403027f, 0.563877f};
        Vector3 srgb2{0.02584f, 0.627829f, 0.952346f};
        Color3 linear1{0.0005f, 0.135f, 0.278f};
        Color3 linear2{0.0020f, 0.352f, 0.895f};
        CORRADE_COMPARE(Color3::fromSrgb(srgb1), linear1);
        CORRADE_COMPARE(Color3::fromSrgb(srgb2), linear2);
        CORRADE_COMPARE(linear1.toSrgb(), srgb1);
        CORRADE_COMPARE(linear2.toSrgb(), srgb2);
    }

    /* Extremes */
    {
        Color3 linear{0.0f, 1.0f, 0.5f};
        Vector3 srgb{0.0f, 1.0f, 0.735357f};
        CORRADE_COMPARE(Color3::fromSrgb(srgb), linear);
        CORRADE_COMPARE(linear.toSrgb(), srgb);
    }

    /* RGBA */
    {
        Vector4 srgb{0.625398f, 0.02584f, 0.823257f, 0.175f};
        Color4 linear{0.349f, 0.0020f, 0.644f, 0.175f};
        CORRADE_COMPARE(Color4::fromSrgbAlpha(srgb), linear);
        CORRADE_COMPARE(Color4::fromSrgb(srgb.rgb(), 0.15f), (Color4{linear.rgb(), 0.15f}));
        CORRADE_COMPARE(linear.toSrgbAlpha(), srgb);
    }
}

void ColorTest::fromSrgbDefaultAlpha() {
    CORRADE_COMPARE(Color4::fromSrgb({0.625398f, 0.02584f, 0.823257f}),
        (Color4{0.349f, 0.0020f, 0.644f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Color4us::fromSrgb({0.1523f, 0.00125f, 0.9853f}),
        (Color4us{1320, 6, 63365, 65535}));
    CORRADE_COMPARE(Color4us::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}),
        (Color4us{58737, 1517, 14146, 65535}));
}

void ColorTest::srgbToIntegral() {
    Vector4 srgb{0.152314f, 0.00118288f, 0.985301f, 0.175f};
    Color4us linear{1320, 6, 63365, 11469};

    CORRADE_COMPARE(Color3us::fromSrgb(srgb.rgb()), linear.rgb());
    CORRADE_COMPARE(Color4us::fromSrgbAlpha(srgb), linear);
    CORRADE_COMPARE(Color4us::fromSrgb(srgb.rgb(), 15299),
        (Color4us{linear.rgb(), 15299}));
    CORRADE_COMPARE(linear.rgb().toSrgb(), srgb.rgb());
    CORRADE_COMPARE(linear.toSrgbAlpha(), srgb);
}

void ColorTest::fromIntegralSrgb() {
    Vector4ub srgb{0xf3, 0x2a, 0x80, 0x23};
    Color4 linear{0.896269f, 0.0231534f, 0.215861f, 0.137255f};

    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>(srgb.rgb()), linear.rgb());
    CORRADE_COMPARE(Color3::fromSrgbInt(0xf32a80), linear.rgb());
    CORRADE_COMPARE(Color4::fromSrgbAlpha(srgb), linear);
    CORRADE_COMPARE(Color4::fromSrgbAlphaInt(0xf32a8023), linear);
    CORRADE_COMPARE(Color4::fromSrgb(srgb.rgb(), 0.175f), (Color4{linear.rgb(), 0.175f}));
    CORRADE_COMPARE(Color4::fromSrgbInt(0xf32a80, 0.175f), (Color4{linear.rgb(), 0.175f}));

    CORRADE_COMPARE(linear.rgb().toSrgb<UnsignedByte>(), srgb.rgb());
    CORRADE_COMPARE(linear.rgb().toSrgbInt(), 0xf32a80);
    CORRADE_COMPARE(linear.toSrgbAlpha<UnsignedByte>(), srgb);
    CORRADE_COMPARE(linear.toSrgbAlphaInt(), 0xf32a8023);
}

void ColorTest::fromIntegralLinearRgb() {
    /* Like fromIntegralSrgb(), but without the sRGB conversion -- thus only
       the int-taking / int-producing APIs are tested, and compared to
       unpack() / pack() as the ground truth */

    Color4ub linear8{0xf3, 0x2a, 0x80, 0x23};
    Color4 linear{0.952941f, 0.164706f, 0.501961f, 0.137255f};

    CORRADE_COMPARE(unpack<Color3>(linear8.rgb()), linear.rgb());
    CORRADE_COMPARE(Color3::fromLinearRgbInt(0xf32a80), linear.rgb());
    CORRADE_COMPARE(unpack<Color4>(linear8), linear);
    CORRADE_COMPARE(Color4::fromLinearRgbaInt(0xf32a8023), linear);
    /* No unpack<T>(rgb, alpha) variant that would make sense to compare to */
    CORRADE_COMPARE(Color4::fromLinearRgbInt(0xf32a80, 0.175f), (Color4{linear.rgb(), 0.175f}));

    CORRADE_COMPARE(pack<Color3ub>(linear.rgb()), linear8.rgb());
    CORRADE_COMPARE(linear.rgb().toLinearRgbInt(), 0xf32a80);
    CORRADE_COMPARE(pack<Color4ub>(linear), linear8);
    CORRADE_COMPARE(linear.toLinearRgbaInt(), 0xf32a8023);
}

void ColorTest::integralSrgbToIntegral() {
    Vector4ub srgb{0xf3, 0x2a, 0x80, 0x23};
    Color4us linear{58737, 1517, 14146, 8995};

    CORRADE_COMPARE(Color3us::fromSrgb(srgb.rgb()), linear.rgb());
    CORRADE_COMPARE(Color3us::fromSrgbInt(0xf32a80), linear.rgb());
    CORRADE_COMPARE(Color4us::fromSrgbAlpha(srgb), linear);
    CORRADE_COMPARE(Color4us::fromSrgbAlphaInt(0xf32a8023), linear);
    CORRADE_COMPARE(Color4us::fromSrgb(srgb.rgb(), 15299),
        (Color4us{linear.rgb(), 15299}));
    CORRADE_COMPARE(Color4us::fromSrgbInt(0xf32a80, 15299),
        (Color4us{linear.rgb(), 15299}));
    CORRADE_COMPARE(linear.rgb().toSrgb<UnsignedByte>(), srgb.rgb());
    CORRADE_COMPARE(linear.rgb().toSrgbInt(), 0xf32a80);
    CORRADE_COMPARE(linear.toSrgbAlpha<UnsignedByte>(), srgb);
    CORRADE_COMPARE(linear.toSrgbAlphaInt(), 0xf32a8023);
}

void ColorTest::integralLinearRgbToIntegral() {
    /* Compared to integralSrgbToIntegral() this operates with linear values on
       both sides again, similarly as fromIntegralLinearRgb(), but as there's
       no direct operation to go from 8-bit to a 16-bit type and vice versa,
       it's always combination of unpack() and pack() */

    Color4ub linear8{0xf3, 0x2a, 0x80, 0x23};
    Color4us linear16{62451, 10794, 32896, 8995};

    CORRADE_COMPARE(pack<Color3us>(unpack<Color3>(linear8.rgb())), linear16.rgb());
    CORRADE_COMPARE(Color3us::fromLinearRgbInt(0xf32a80), linear16.rgb());
    CORRADE_COMPARE(pack<Color4us>(unpack<Color4>(linear8)), linear16);
    CORRADE_COMPARE(Color4us::fromLinearRgbaInt(0xf32a8023), linear16);
    /* No unpack<T>(rgb, alpha) variant that would make sense to compare to */
    CORRADE_COMPARE(Color4us::fromLinearRgbInt(0xf32a80, 15299),
        (Color4us{linear16.rgb(), 15299}));
    CORRADE_COMPARE(pack<Color3ub>(unpack<Color3>(linear16.rgb())), linear8.rgb());
    CORRADE_COMPARE(linear16.rgb().toLinearRgbInt(), 0xf32a80);
    CORRADE_COMPARE(pack<Color4ub>(unpack<Color4>(linear16)), linear8);
    CORRADE_COMPARE(linear16.toLinearRgbaInt(), 0xf32a8023);
}

void ColorTest::srgbMonotonic() {
    Color3 rgbPrevious = Color3::fromSrgb(Vector3us(testCaseRepeatId()));
    Color3 rgb = Color3::fromSrgb(Vector3us(testCaseRepeatId() + 1));
    CORRADE_COMPARE_AS(rgb, rgbPrevious, TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(rgb, Color3(0.0f), TestSuite::Compare::GreaterOrEqual);
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(testCaseRepeatId() == 65534 && !!(rgb > Color3(1.0f)),
            "Some minor rounding error possibly due to some Esmcripten "
            "optimizations. Happens only on larger optimization levels, not "
            "on -O1.");
        #endif
        CORRADE_COMPARE_AS(rgb, Color3(1.0f), TestSuite::Compare::LessOrEqual);
    }
}

void ColorTest::srgb8bitRoundtrip() {
    CORRADE_COMPARE(Color3::fromSrgbInt(testCaseRepeatId()).toSrgbInt(), testCaseRepeatId());
}

void ColorTest::srgbLiterals() {
    constexpr Vector3ub a = 0x33b27f_srgb;
    CORRADE_COMPARE(a, (Vector3ub{0x33, 0xb2, 0x7f}));

    constexpr Vector4ub b = 0x33b27fcc_srgba;
    CORRADE_COMPARE(b, (Vector4ub{0x33, 0xb2, 0x7f, 0xcc}));

    /* Not constexpr yet */
    CORRADE_COMPARE(0x33b27f_srgbf, (Color3{0.0331048f, 0.445201f, 0.212231f}));
    CORRADE_COMPARE(0x33b27fcc_srgbaf, (Color4{0.0331048f, 0.445201f, 0.212231f, 0.8f}));
}

void ColorTest::xyz() {
    /* Verified using http://colormine.org/convert/rgb-to-xyz and
       http://www.easyrgb.com/index.php?X=CALC. The results have slight
       precision differences, because most of the code out there uses just the
       rounded matrices from Wikipedia which don't round-trip perfectly. I'm
       having Y in 0-1 instead of 0-100, thus the values are 100 times smaller. */
    Vector3ub a8s{232, 157, 16};
    Vector3 aXyz{0.454279f, 0.413092f, 0.0607124f};
    Color3 aLinear{0.806952f, 0.337163f, 0.0051861f};
    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>(a8s).toXyz(), aXyz);
    CORRADE_COMPARE(Color3::fromXyz(aXyz).toSrgb<UnsignedByte>(), a8s);
    CORRADE_COMPARE(Color3::fromXyz(aXyz), aLinear);

    Vector3ub b8s{96, 43, 193};
    Vector3 bXyz{0.153122f, 0.0806478f, 0.512037f};
    Color3 bLinear{0.11697f, 0.0241579f, 0.533276f};
    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>(b8s).toXyz(), bXyz);
    CORRADE_COMPARE(Color3::fromXyz(bXyz).toSrgb<UnsignedByte>(), b8s);
    CORRADE_COMPARE(Color3::fromXyz(bXyz), bLinear);

    /* Extremes -- for black it should be zeros, for white roughly X = 0.95,
       Y = 1, Z = 1.09 corresponding to white point in D65 */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).toXyz(),
        (Vector3{0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color3{1.0f, 1.0f, 1.0f}).toXyz(),
        (Vector3{0.950456f, 1.0f, 1.08906f}));

    /* RGBA */
    CORRADE_COMPARE(Color4::fromXyz(aXyz, 0.175f), (Color4{aLinear, 0.175f}));
    CORRADE_COMPARE(Color4::fromSrgb<UnsignedByte>(a8s, 0.175f).toXyz(), aXyz);

    /* Integral */
    Math::Color3<UnsignedShort> c16{52884, 22096, 340};
    Vector3 cXyz{0.454283f, 0.413094f, 0.0607178f};
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromXyz(cXyz), c16);
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromXyz(cXyz, 15299),
        (Math::Color4<UnsignedShort>{c16, 15299}));
    CORRADE_COMPARE(c16.toXyz(), cXyz);
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{c16, 15299}).toXyz(), cXyz);

    /* Round-trip */
    CORRADE_COMPARE(Color3::fromXyz(aXyz).toXyz(), aXyz);
    CORRADE_COMPARE(Color3::fromXyz(bXyz).toXyz(), bXyz);
    CORRADE_COMPARE(Color3::fromXyz(cXyz).toXyz(), cXyz);
    CORRADE_COMPARE(Color4::fromXyz(aXyz, 0.175f).toXyz(), aXyz);
}

void ColorTest::fromXyzDefaultAlpha() {
    CORRADE_COMPARE(Color4::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Color4{0.806952f, 0.337163f, 0.0051861f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Math::Color4<UnsignedShort>{52884, 22096, 340, 65535}));
}

void ColorTest::xyY() {
    /* Verified using http://www.easyrgb.com/index.php?X=CALC */
    Vector3 xyz{0.454279f, 0.413092f, 0.0607124f};
    Vector3 xyY{0.489481f, 0.445102f, 0.413092f};

    CORRADE_COMPARE(xyzToXyY(xyz), xyY);
    CORRADE_COMPARE(xyYToXyz(xyY), xyz);
}

void ColorTest::premultiplied() {
    /* Usual scenario */
    CORRADE_COMPARE((Color4{0.6f, 0.8f, 0.4f, 0.25f}).premultiplied(), (Color4{0.15f, 0.2f, 0.1f, 0.25f}));
    /* Slight imprecision with packed types */
    CORRADE_COMPARE((Color4us{0x9999, 0xcccc, 0x6666, 0x3fff}).premultiplied(), (Color4us{0x2666, 0x3332, 0x1999, 0x3fff}));
    /* Lol it wants to treat _rgba.premultiplied() as a literal suffix as a
       whole?! Load-bearing space?! */
    CORRADE_COMPARE(0x99cc663f_rgba .premultiplied(), 0x2632193f_rgba);

    /* Zero alpha just zeroes out the rest, no special treatment */
    CORRADE_COMPARE((Color4{0.6f, 0.8f, 0.4f, 0.0f}).premultiplied(), (Color4{0.0f, 0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color4us{0, 0, 0, 0}).premultiplied(), (Color4us{0, 0, 0, 0}));
    CORRADE_COMPARE(0x00000000_rgba .premultiplied(), 0x00000000_rgba);

    /* RGB channels over 1 aren't treated in any special way */
    CORRADE_COMPARE((Color4{1.6f, 1.8f, 1.4f, 0.25f}).premultiplied(), (Color4{0.4f, 0.45f, 0.35f, 0.25f}));
    /* (no way to express this with packed types) */

    constexpr Color4 a{0.6f, 0.8f, 0.4f, 0.25f};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4 ap = a.premultiplied();
    CORRADE_COMPARE(ap, (Color4{0.15f, 0.2f, 0.1f, 0.25f}));

    constexpr Color4us b{0x9999, 0xcccc, 0x6666, 0x3fff};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4us bp = b.premultiplied();
    CORRADE_COMPARE(bp, (Color4us{0x2666, 0x3332, 0x1999, 0x3fff}));

    constexpr Color4ub c = 0x99cc663f_rgba;
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4ub cp = c.premultiplied();
    CORRADE_COMPARE(cp, 0x2632193f_rgba);
}

template<class T> void ColorTest::premultipliedRoundtrip() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    /* Unpacking, premultiplying and packing a color should give the same
       result as premultiplying a packed color directly. The implementation
       doesn't use pack() etc to be constexpr so verify the two have the same
       rounding behavior.

       This only holds for premultiplied(), with unpremultiplied() it doesn't,
       see unpremultipliedRoundtrip() below. */

    for(UnsignedInt i = 0; i != Implementation::bitMax<T>(); ++i) {
        CORRADE_ITERATION(Debug::hex << i);

        Math::Color4<T> a{
            Math::pack<T>(1.0f), /* 0xff or 0xffff */
            Math::pack<T>(0.8f), /* 0x99 or 0x9999 */
            Math::pack<T>(0.2f), /* 0x33 or 0x3333 */
            T(i)};
        CORRADE_COMPARE(a.premultiplied(), Math::pack<Math::Color4<T>>(Math::unpack<Color4>(a).premultiplied()));
    }
}

void ColorTest::unpremultiplied() {
    /* Usual scenario, inverse of the above */
    CORRADE_COMPARE((Color4{0.15f, 0.2f, 0.1f, 0.25f}).unpremultiplied(), (Color4{0.6f, 0.8f, 0.4f, 0.25f}));
    /* Slight imprecision with packed types */
    CORRADE_COMPARE((Color4us{0x2666, 0x3333, 0x1999, 0x3fff}).unpremultiplied(), (Color4us{0x999a, 0xccce, 0x6665, 0x3fff}));
    /* Lol a load-bearing space again */
    CORRADE_COMPARE(0x2633193f_rgba .unpremultiplied(), 0x9ace653f_rgba);

    /* With zero alpha the RGB channels get ignored, no matter what they are */
    CORRADE_COMPARE((Color4{0.6f, 0.8f, 0.4f, 0.0f}).unpremultiplied(), (Color4{0.0f, 0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color4us{0x6666, 0xcccc, 0xffff, 0}).unpremultiplied(), (Color4us{0, 0, 0, 0}));
    CORRADE_COMPARE(0x33ff9900_rgba .unpremultiplied(), 0x00000000_rgba);

    /* RGB channels over alpha aren't treated in any special way for floats
       (inverse of what's tested in premultiplied()) */
    CORRADE_COMPARE((Color4{0.4f, 0.45f, 0.35f, 0.25f}).unpremultiplied(), (Color4{1.6f, 1.8f, 1.4f, 0.25f}));
    /* For packed types they get individually clamped -- i.e., it's not all of
       them being set to full channel, only those that overflow */
    CORRADE_COMPARE((Color4us{0x6666, 0x2666, 0x4000, 0x3fff}).unpremultiplied(), (Color4us{0xffff, 0x999a, 0xffff, 0x3fff}));
    CORRADE_COMPARE(0x2666193f_rgba .unpremultiplied(), 0x9aff653f_rgba);

    constexpr Color4 ap{0.15f, 0.2f, 0.1f, 0.25f};
    constexpr Color4 apz{0.15f, 0.2f, 0.1f, 0.0f};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4 a = ap.unpremultiplied();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4 az = apz.unpremultiplied();
    CORRADE_COMPARE(a, (Color4{0.6f, 0.8f, 0.4f, 0.25f}));
    CORRADE_COMPARE(az, (Color4{0.0f, 0.0f, 0.0f, 0.0f}));

    /* Second channel overflows here */
    constexpr Color4us bp{0x2666, 0x6666, 0x1999, 0x3fff};
    constexpr Color4us bpz{0x2666, 0x6666, 0x1999, 0};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4us b = bp.unpremultiplied();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4us bz = bpz.unpremultiplied();
    CORRADE_COMPARE(b, (Color4us{0x999a, 0xffff, 0x6665, 0x3fff}));
    CORRADE_COMPARE(bz, (Color4us{0, 0, 0, 0}));

    /* First channel overflows here */
    constexpr Color4ub cp = 0x6633193f_rgba;
    constexpr Color4ub cpz = 0x66331900_rgba;
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4ub c = cp.unpremultiplied();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* do I care? nah */
    constexpr
    #endif
    Color4ub cz = cpz.unpremultiplied();
    CORRADE_COMPARE(c, 0xffce653f_rgba);
    CORRADE_COMPARE(cz, 0x00000000_rgba);
}

/* A variant of packed unpremultiplied() that does a calculation that has
   exactly the same rounding behavior as unpack() followed by pack(). */
template<class T> constexpr Math::Color4<T> unpremultipliedPackedExact(const Math::Color4<T>& color) {
    return color.a() == T(0) ? Math::Color4<T>{} : Math::Color4<T>{
        T(Implementation::bitMax<T>()*((typename Math::Color4<T>::FloatingPointType(min(color.r(), color.a()))/Implementation::bitMax<T>())/(typename Math::Color4<T>::FloatingPointType(color.a())/Implementation::bitMax<T>())) + typename Math::Color4<T>::FloatingPointType(0.5)),
        T(Implementation::bitMax<T>()*((typename Math::Color4<T>::FloatingPointType(min(color.g(), color.a()))/Implementation::bitMax<T>())/(typename Math::Color4<T>::FloatingPointType(color.a())/Implementation::bitMax<T>())) + typename Math::Color4<T>::FloatingPointType(0.5)),
        T(Implementation::bitMax<T>()*((typename Math::Color4<T>::FloatingPointType(min(color.b(), color.a()))/Implementation::bitMax<T>())/(typename Math::Color4<T>::FloatingPointType(color.a())/Implementation::bitMax<T>())) + typename Math::Color4<T>::FloatingPointType(0.5)),
        color.a()
    };
}

template<class T> void ColorTest::unpremultipliedRoundtrip() {
    auto&& data = UnpremultipliedRoundtripData[testCaseInstanceId()];
    setTestCaseTemplateName(TypeTraits<T>::name());
    setTestCaseDescription(Utility::format("{}/10, {}/10, {}/10", data.r, data.g, data.b));

    /* Compared to premultipliedRoundtrip(), the sequence of operations with
       pack()/unpack() causes extra rounding differences and in general is more
       complex code. The simpler sequence in unpremultiplied() doesn't lead to
       more precision always, but only in majority of cases, which is what this
       test tries to show. */

    UnsignedInt impreciseCount = 0, preciseCount = 0;
    for(UnsignedInt i = 0; i != Implementation::bitMax<T>(); ++i) {
        CORRADE_ITERATION(Debug::hex << i);

        Math::Color4<T> ap{T(i*data.r/10),
                           T(i*data.g/10),
                           T(i*data.b/10),
                           T(i)};

        /* It only matches when we replicate the exact sequence of operations */
        Math::Color4<T> ae = unpremultipliedPackedExact(ap);
        CORRADE_COMPARE(ae, Math::pack<Math::Color4<T>>(Math::unpack<Color4>(ap).unpremultiplied()));

        /* The unpremultiplied() implementation is at most off-by-one from
           that. Casting, not unpacking, to a float type so we can compare with
           a ±1 delta even the boundary values without overflow. */
        Math::Color4<T> a = ap.unpremultiplied();
        CORRADE_COMPARE_WITH(Color4{a},
            Color4{Math::pack<Math::Color4<T>>(Math::unpack<Color4>(ap).unpremultiplied())},
            TestSuite::Compare::around(Color4{1.0f, 1.0f}));

        /* If they're different, the unpremultiplied() should be always closer
           to the ideal than unpack() + pack() */
        if(ae != a) {
            const Color3 expected{
                Float(UnsignedInt(Implementation::bitMax<T>())*data.r/10)/Implementation::bitMax<T>(),
                Float(UnsignedInt(Implementation::bitMax<T>())*data.g/10)/Implementation::bitMax<T>(),
                Float(UnsignedInt(Implementation::bitMax<T>())*data.b/10)/Implementation::bitMax<T>()};
            const Float aDelta = (Math::unpack<Color3>(a.rgb()) - expected).dot();
            const Float aeDelta = (Math::unpack<Color3>(ae.rgb()) - expected).dot();

            if(aDelta > aeDelta)
                ++impreciseCount;
            else if(aDelta < aeDelta)
                ++preciseCount;
        }
    }

    if(impreciseCount > preciseCount)
        CORRADE_FAIL(impreciseCount << "values out of" << Implementation::bitMax<T>() << "were less precise than the pack()/unpack() variant," << preciseCount << "were more precise.");
    if(impreciseCount)
        CORRADE_WARN(impreciseCount << "values out of" << Implementation::bitMax<T>() << "were less precise than the pack()/unpack() variant," << preciseCount << "were more precise.");
    else if(preciseCount)
        CORRADE_INFO(preciseCount << "values out of" << Implementation::bitMax<T>() << "were more precise than the pack()/unpack() variant.");
}

void ColorTest::multiplyDivideIntegral() {
    typedef Math::Color3<Int> Color3i;
    typedef Math::Color4<Int> Color4i;

    const Color3i vector3{32, 10, -6};
    const Color4i vector4{32, 10, -6, 2};
    const Color3i multiplied3{-48, -15, 9};
    const Color4i multiplied4{-48, -15, 9, -3};

    CORRADE_COMPARE(vector3*-1.5f, multiplied3);
    CORRADE_COMPARE(vector4*-1.5f, multiplied4);
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    CORRADE_COMPARE(-1.5f*vector3, multiplied3);
    CORRADE_COMPARE(-1.5f*vector4, multiplied4);

    constexpr Color3i cvector3{32, 10, -6};
    constexpr Color4i cvector4{32, 10, -6, 2};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Color3i ca31 = cvector3*-1.5f;
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Color4i ca41 = cvector4*-1.5f;
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Color3i ca32 = -1.5f*cvector3;
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Color4i ca42 = -1.5f*cvector4;
    CORRADE_COMPARE(ca31, multiplied3);
    CORRADE_COMPARE(ca41, multiplied4);
    CORRADE_COMPARE(ca32, multiplied3);
    CORRADE_COMPARE(ca42, multiplied4);
}

void ColorTest::strictWeakOrdering() {
    StrictWeakOrdering o;

    auto r = Color4::red();
    auto g = Color4::green();
    auto b = Color4::blue();

    CORRADE_VERIFY( o(b, r));
    CORRADE_VERIFY( o(g, r));
    CORRADE_VERIFY( o(b, g));
    CORRADE_VERIFY(!o(r, r));

    auto ba = b;
    ba.a() = 0.5f;
    CORRADE_VERIFY(o(ba, b));

    auto r3 = r.rgb();
    auto g3 = g.rgb();
    auto b3 = b.rgb();

    CORRADE_VERIFY( o(b3, r3));
    CORRADE_VERIFY( o(g3, r3));
    CORRADE_VERIFY( o(b3, g3));
    CORRADE_VERIFY(!o(r3, r3));
}

void ColorTest::swizzleType() {
    constexpr Color3 origColor3;
    constexpr Color4ub origColor4;

    constexpr auto a = Math::gather<'y', 'z', 'r'>(origColor3);
    CORRADE_VERIFY(std::is_same<decltype(a), const Color3>::value);

    constexpr auto b = Math::gather<'y', 'z', 'a'>(origColor4);
    CORRADE_VERIFY(std::is_same<decltype(b), const Color3ub>::value);

    constexpr auto c = Math::gather<'y', 'z', 'y', 'x'>(origColor3);
    CORRADE_VERIFY(std::is_same<decltype(c), const Color4>::value);

    constexpr auto d = Math::gather<'y', 'a', 'y', 'x'>(origColor4);
    CORRADE_VERIFY(std::is_same<decltype(d), const Color4ub>::value);
}

void ColorTest::debug() {
    Containers::String out;
    Debug{&out} << Color3(0.5f, 0.75f, 1.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 0.75, 1)\n");

    out = {};
    Debug{&out} << Color4(0.5f, 0.75f, 0.0f, 1.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 0.75, 0, 1)\n");
}

void ColorTest::debugUb() {
    Containers::String out;
    Debug{&out} << 0x123456_rgb << 0x789abc_rgb;
    CORRADE_COMPARE(out, "#123456 #789abc\n");

    out = {};
    Debug{&out} << 0x12345678_rgba << 0x90abcdef_rgba;
    CORRADE_COMPARE(out, "#12345678 #90abcdef\n");

    /* The Hex flag shouldn't affect this at all */
    out = {};
    Debug{&out, Debug::Flag::Hex} << 0x789abc_rgb << 0x12345678_rgba;
    CORRADE_COMPARE(out, "#789abc #12345678\n");
}

void ColorTest::debugUbColor() {
    Debug{} << "The following should be the Magnum / m.css color palette:";

    Debug{Debug::Flag::Color}
        << 0xdcdcdc_rgb << 0xa5c9ea_rgb << 0x3bd267_rgb
        << 0xc7cf2f_rgb << 0xcd3431_rgb << 0x2f83cc_rgb << 0x747474_rgb;

    Debug{} << "The following should have increasing (overmultiplied) alpha:";

    Debug{Debug::Flag::Color}
        << 0x3bd26700_rgba << 0x3bd26733_rgba << 0x3bd26766_rgba
        << 0x3bd26799_rgba << 0x3bd267cc_rgba << 0x3bd267ff_rgba;

    /* It should work just for the immediately following value */
    Containers::String out;
    Debug{&out}
        << Debug::color << 0x3bd267_rgb
        << Debug::color << 0x2f83cc99_rgba
        << 0x3bd267_rgb << 0x2f83cc99_rgba;
    CORRADE_COMPARE(out,
        "\033[38;2;59;210;103m\033[48;2;59;210;103m██\033[0m "
        "\033[38;2;47;131;204m▒▒\033[0m #3bd267 #2f83cc99\n");
}

void ColorTest::debugUbColorColorsDisabled() {
    Debug{} << "The following should be the Magnum / m.css uncolored palette:";

    Debug{Debug::Flag::Color|Debug::Flag::DisableColors}
        << 0xdcdcdc_rgb << 0xa5c9ea_rgb << 0x3bd267_rgb
        << 0xc7cf2f_rgb << 0xcd3431_rgb << 0x2f83cc_rgb << 0x747474_rgb;

    Debug{} << "The following should have increasing (overmultiplied) alpha:";

    Debug{Debug::Flag::Color|Debug::Flag::DisableColors}
        << 0x3bd26700_rgba << 0x3bd26733_rgba << 0x3bd26766_rgba
        << 0x3bd26799_rgba << 0x3bd267cc_rgba << 0x3bd267ff_rgba;

    /* It should work just for the immediately following value */
    Containers::String out;
    Debug{&out, Debug::Flag::DisableColors}
        << Debug::color << 0x2f83cc_rgb
        << Debug::color << 0x2f83cc99_rgba
        << 0x2f83cc_rgb << 0x2f83cc99_rgba;
    CORRADE_COMPARE(out, "▓▓ ▒▒ #2f83cc #2f83cc99\n");
}

void ColorTest::debugHsv() {
    Containers::String out;
    Debug{&out} << ColorHsv(135.0_degf, 0.75f, 0.3f);
    CORRADE_COMPARE(out, "ColorHsv(Deg(135), 0.75, 0.3)\n");
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
void ColorTest::tweakableRgb() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color3ub> result = Utility::TweakableParser<Color3ub>::parse(Utility::format(data.dataRgb, "rgb"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultUb.rgb());
}

void ColorTest::tweakableSrgb() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color3ub> result = Utility::TweakableParser<Vector3ub>::parse(Utility::format(data.dataRgb, "srgb"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultUb.rgb());
}

void ColorTest::tweakableRgba() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color4ub> result = Utility::TweakableParser<Color4ub>::parse(Utility::format(data.dataRgba, "rgba"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultUb);
}

void ColorTest::tweakableSrgba() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color4ub> result = Utility::TweakableParser<Vector4ub>::parse(Utility::format(data.dataRgba, "srgba"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultUb);
}

void ColorTest::tweakableRgbf() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color3> result = Utility::TweakableParser<Color3>::parse(Utility::format(data.dataRgb, "rgbf"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.result.rgb());
}

void ColorTest::tweakableSrgbf() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color3> result = Utility::TweakableParser<Color3>::parse(Utility::format(data.dataRgb, "srgbf"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultSrgba.rgb());
}

void ColorTest::tweakableRgbaf() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color4> result = Utility::TweakableParser<Color4>::parse(Utility::format(data.dataRgba, "rgbaf"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.result);
}

void ColorTest::tweakableSrgbaf() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Pair<Utility::TweakableState, Color4> result = Utility::TweakableParser<Color4>::parse(Utility::format(data.dataRgba, "srgbaf"));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), data.resultSrgba);
}

void ColorTest::tweakableErrorRgb() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color3ub>::parse(Utility::format(data.data, "ff3366", "rgb")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366", "rgb", ""));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorSrgb() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Vector3ub>::parse(Utility::format(data.data, "ff3366", "srgb")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366", "rgb", "s"));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorRgba() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color4ub>::parse(Utility::format(data.data, "ff3366aa", "rgba")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366aa", "rgba", ""));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorSrgba() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Vector4ub>::parse(Utility::format(data.data, "ff3366aa", "srgba")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366aa", "rgba", "s"));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorRgbf() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color3>::parse(Utility::format(data.data, "ff3366", "rgbf")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366", "rgbf", ""));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorSrgbf() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color3>::parse(Utility::format(data.data, "ff3366", "srgbf")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366", "rgbf", "s"));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorRgbaf() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color4>::parse(Utility::format(data.data, "ff3366aa", "rgbaf")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366aa", "rgbaf", ""));
    CORRADE_COMPARE(state, data.state);
}

void ColorTest::tweakableErrorSrgbaf() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<Color4>::parse(Utility::format(data.data, "ff3366aa", "srgbaf")).first();
    CORRADE_COMPARE(out, Utility::format(data.error, "ff3366aa", "rgbaf", "s"));
    CORRADE_COMPARE(state, data.state);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ColorTest)
