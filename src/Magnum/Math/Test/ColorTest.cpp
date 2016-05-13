/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Color.h"

namespace Magnum { namespace Math { namespace Test {

struct ColorTest: Corrade::TestSuite::Tester {
    explicit ColorTest();

    void construct();
    void constructDefault();
    void constructZero();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
    void constructConversion();
    void constructNormalization();
    void constructCopy();

    void literals();

    void colors();

    void fromHue();
    void fromSaturation();
    void fromValue();

    void hue();
    void saturation();
    void value();

    void hsv();
    void hsvOverflow();
    void hsvAlpha();

    void swizzleType();
    void debug();
    void configuration();
};

typedef Math::Vector3<Float> Vector3;
typedef Math::Color3<Float> Color3;
typedef Math::Color3<UnsignedByte> Color3ub;

typedef Math::Vector4<Float> Vector4;
typedef Math::Color4<Float> Color4;
typedef Math::Color4<UnsignedByte> Color4ub;

typedef Math::Deg<Float> Deg;

using namespace Literals;

ColorTest::ColorTest() {
    addTests({&ColorTest::construct,
              &ColorTest::constructDefault,
              &ColorTest::constructZero,
              &ColorTest::constructNoInit,
              &ColorTest::constructOneValue,
              &ColorTest::constructParts,
              &ColorTest::constructConversion,
              &ColorTest::constructNormalization,
              &ColorTest::constructCopy,

              &ColorTest::literals,

              &ColorTest::colors,

              &ColorTest::fromHue,
              &ColorTest::fromSaturation,
              &ColorTest::fromValue,

              &ColorTest::hue,
              &ColorTest::saturation,
              &ColorTest::value,

              &ColorTest::hsv,
              &ColorTest::hsvOverflow,
              &ColorTest::hsvAlpha,

              &ColorTest::swizzleType,
              &ColorTest::debug,
              &ColorTest::configuration});
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
    CORRADE_COMPARE(d, Math::Vector4<UnsignedByte>(10, 25, 176, 255));
}

void ColorTest::constructDefault() {
    constexpr Color3 a1;
    constexpr Color3 a2{Math::ZeroInit};
    CORRADE_COMPARE(a1, Color3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(a2, Color3(0.0f, 0.0f, 0.0f));

    constexpr Color4 b;
    constexpr Color4ub c;
    CORRADE_COMPARE(b, Color4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(c, Color4ub(0, 0, 0, 0));
}

void ColorTest::constructZero() {
    constexpr Color3 a{Math::ZeroInit};
    constexpr Color4 b{Math::ZeroInit};
    CORRADE_COMPARE(a, Color3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b, Color4(0.0f, 0.0f, 0.0f, 0.0f));
}

void ColorTest::constructNoInit() {
    Color3 a{1.0f, 0.5f, 0.75f};
    Color4 b{1.0f, 0.5f, 0.75f, 0.5f};
    new(&a) Color3{Math::NoInit};
    new(&b) Color4{Math::NoInit};
    CORRADE_COMPARE(a, (Color3{1.0f, 0.5f, 0.75f}));
    CORRADE_COMPARE(b, (Color4{1.0f, 0.5f, 0.75f, 0.5f}));
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
    CORRADE_VERIFY(!(std::is_convertible<Float, Color3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Float, Color4>::value));
}

void ColorTest::constructParts() {
    constexpr Color3 a(1.0f, 0.5f, 0.75f);

    constexpr Color4 b = {a, 0.25f};
    CORRADE_COMPARE(b, Color4(1.0f, 0.5f, 0.75f, 0.25f));

    /* Default alpha */
    constexpr Color3ub c(10, 25, 176);
    constexpr Color4 d = a;
    constexpr Color4ub e = c;
    CORRADE_COMPARE(d, Color4(1.0f, 0.5f, 0.75f, 1.0f));
    CORRADE_COMPARE(e, Color4ub(10, 25, 176, 255));
}

void ColorTest::constructConversion() {
    constexpr Color3 a(10.1f, 12.5f, 0.75f);
    constexpr Color3ub b(a);
    CORRADE_COMPARE(b, Color3ub(10, 12, 0));

    constexpr Color4 c(10.1f, 12.5f, 0.75f, 5.25f);
    constexpr Color4ub d(c);
    CORRADE_COMPARE(d, Color4ub(10, 12, 0, 5));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Color3, Color3ub>::value));
    CORRADE_VERIFY(!(std::is_convertible<Color4, Color4ub>::value));
}

void ColorTest::constructNormalization() {
    constexpr Color3 a(1.0f, 0.5f, 0.75f);
    auto b = Math::denormalize<Color3ub>(a);
    CORRADE_COMPARE(b, Color3ub(255, 127, 191));

    constexpr Color4 c(1.0f, 0.5f, 0.75f, 0.25f);
    auto d = Math::denormalize<Color4ub>(c);
    CORRADE_COMPARE(d, Color4ub(255, 127, 191, 63));
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
}

void ColorTest::literals() {
    using namespace Literals;

    constexpr Color3ub a = 0x33b27f_rgb;
    CORRADE_COMPARE(a, (Color3ub{0x33, 0xb2, 0x7f}));

    constexpr Color4ub b = 0x33b27fcc_rgba;
    CORRADE_COMPARE(b, (Color4ub{0x33, 0xb2, 0x7f, 0xcc}));

    /* Not constexpr yet */
    CORRADE_COMPARE(0x33b27f_rgbf, (Color3{0.2f, 0.698039f, 0.498039f}));
    CORRADE_COMPARE(0x33b27fcc_rgbaf, (Color4{0.2f, 0.698039f, 0.498039f, 0.8f}));
}

void ColorTest::colors() {
    CORRADE_COMPARE(Color3ub::red(75), Color3ub(75, 0, 0));
    CORRADE_COMPARE(Color3ub::green(75), Color3ub(0, 75, 0));
    CORRADE_COMPARE(Color3ub::blue(75), Color3ub(0, 0, 75));

    CORRADE_COMPARE(Color3ub::cyan(75), Color3ub(75, 255, 255));
    CORRADE_COMPARE(Color3ub::magenta(75), Color3ub(255, 75, 255));
    CORRADE_COMPARE(Color3ub::yellow(75), Color3ub(255, 255, 75));

    CORRADE_COMPARE(Color4ub::red(75, 138), Color4ub(75, 0, 0, 138));
    CORRADE_COMPARE(Color4ub::green(75, 138), Color4ub(0, 75, 0, 138));
    CORRADE_COMPARE(Color4ub::blue(75, 138), Color4ub(0, 0, 75, 138));

    CORRADE_COMPARE(Color4ub::cyan(75, 138), Color4ub(75, 255, 255, 138));
    CORRADE_COMPARE(Color4ub::magenta(75, 138), Color4ub(255, 75, 255, 138));
    CORRADE_COMPARE(Color4ub::yellow(75, 138), Color4ub(255, 255, 75, 138));

    /* Default values */
    CORRADE_COMPARE(Color3ub::red(), Color3ub(255, 0, 0));
    CORRADE_COMPARE(Color3ub::green(), Color3ub(0, 255, 0));
    CORRADE_COMPARE(Color3ub::blue(), Color3ub(0, 0, 255));

    CORRADE_COMPARE(Color4ub::red(), Color4ub(255, 0, 0, 255));
    CORRADE_COMPARE(Color4ub::green(), Color4ub(0, 255, 0, 255));
    CORRADE_COMPARE(Color4ub::blue(), Color4ub(0, 0, 255, 255));

    CORRADE_COMPARE(Color4ub::cyan(), Color4ub(0, 255, 255, 255));
    CORRADE_COMPARE(Color4ub::magenta(), Color4ub(255, 0, 255, 255));
    CORRADE_COMPARE(Color4ub::yellow(), Color4ub(255, 255, 0, 255));
}

void ColorTest::fromHue() {
    CORRADE_COMPARE(Color3ub::fromHSV(27.0_degf, 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(86.0_degf, 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(134.0_degf, 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(191.0_degf, 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(269.0_degf, 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(317.0_degf, 1.0f, 1.0f), Color3ub(255, 0, 182));
}

void ColorTest::hue() {
    CORRADE_COMPARE(Color3ub(255, 115, 0).hue(), 27.058824_degf);
    CORRADE_COMPARE(Color3ub(145, 255, 0).hue(), 85.882353_degf);
    CORRADE_COMPARE(Color3ub(0, 255, 60).hue(), 134.11765_degf);
    CORRADE_COMPARE(Color3ub(0, 208, 255).hue(), 191.05882_degf);
    CORRADE_COMPARE(Color3ub(123, 0, 255).hue(), 268.94117_degf);
    CORRADE_COMPARE(Color3ub(255, 0, 183).hue(), 316.94117_degf);
}

void ColorTest::fromSaturation() {
    CORRADE_COMPARE(Color3ub::fromHSV(0.0_degf, 0.702f, 1.0f), Color3ub(255, 75, 75));
}

void ColorTest::saturation() {
    CORRADE_COMPARE(Color3ub(255, 76, 76).saturation(), 0.701961f);
    CORRADE_COMPARE(Color3ub().saturation(), 0.0f);
}

void ColorTest::fromValue() {
    CORRADE_COMPARE(Color3ub::fromHSV(0.0_degf, 1.0f, 0.522f), Color3ub(133, 0, 0));
}

void ColorTest::value() {
    CORRADE_COMPARE(Color3ub(133, 0, 0).value(), 0.521569f);
}

void ColorTest::hsv() {
    CORRADE_COMPARE(Color3ub::fromHSV(230.0_degf, 0.749f, 0.427f), Color3ub(27, 40, 108));

    Deg hue;
    Float saturation, value;
    std::tie(hue, saturation, value) = Color3ub(27, 41, 109).toHSV();
    CORRADE_COMPARE(hue, 229.756106_degf);
    CORRADE_COMPARE(saturation, 0.752294f);
    CORRADE_COMPARE(value, 0.427451f);
}

void ColorTest::hsvOverflow() {
    CORRADE_COMPARE(Color3ub::fromHSV(27.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(86.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(134.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(191.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(269.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(317.0_degf - 360.0_degf, 1.0f, 1.0f), Color3ub(255, 0, 182));

    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 27.0_degf, 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 86.0_degf, 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 134.0_degf, 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 191.0_degf, 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 269.0_degf, 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(360.0_degf + 317.0_degf, 1.0f, 1.0f), Color3ub(255, 0, 182));
}

void ColorTest::hsvAlpha() {
    CORRADE_COMPARE(Color4ub::fromHSV(std::make_tuple(230.0_degf, 0.749f, 0.427f), 23), Color4ub(27, 40, 108, 23));
    CORRADE_COMPARE(Color4ub::fromHSV(230.0_degf, 0.749f, 0.427f, 23), Color4ub(27, 40, 108, 23));

    /* Default alpha */
    CORRADE_COMPARE(Color4ub::fromHSV(std::make_tuple(230.0_degf, 0.749f, 0.427f)), Color4ub(27, 40, 108, 255));
    CORRADE_COMPARE(Color4ub::fromHSV(230.0_degf, 0.749f, 0.427f), Color4ub(27, 40, 108, 255));
}

void ColorTest::swizzleType() {
    constexpr Color3 origColor3;
    constexpr Color4ub origColor4;

    constexpr auto a = Math::swizzle<'y', 'z', 'r'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(a), const Color3>::value));

    constexpr auto b = Math::swizzle<'y', 'z', 'a'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(b), const Color3ub>::value));

    constexpr auto c = Math::swizzle<'y', 'z', 'y', 'x'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(c), const Color4>::value));

    constexpr auto d = Math::swizzle<'y', 'a', 'y', 'x'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(d), const Color4ub>::value));
}

void ColorTest::debug() {
    std::ostringstream o;
    Debug(&o) << Color3(0.5f, 0.75f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 1)\n");

    o.str({});
    Debug(&o) << Color4(0.5f, 0.75f, 0.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 0, 1)\n");
}

void ColorTest::configuration() {
    Corrade::Utility::Configuration c;

    Color3 color3(0.5f, 0.75f, 1.0f);
    std::string value3("0.5 0.75 1");

    c.setValue("color3", color3);
    CORRADE_COMPARE(c.value("color3"), value3);
    CORRADE_COMPARE(c.value<Color3>("color3"), color3);

    Color4 color4(0.5f, 0.75f, 0.0f, 1.0f);
    std::string value4("0.5 0.75 0 1");

    c.setValue("color4", color4);
    CORRADE_COMPARE(c.value("color4"), value4);
    CORRADE_COMPARE(c.value<Color4>("color4"), color4);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ColorTest)
