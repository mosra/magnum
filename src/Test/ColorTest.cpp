/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Color.h"

namespace Magnum { namespace Test {

class ColorTest: public TestSuite::Tester {
    public:
        ColorTest();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructParts();
        void constructConversion();
        void constructNormalization();
        void constructCopy();

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

typedef BasicColor3<UnsignedByte> Color3ub;
typedef BasicColor4<UnsignedByte> Color4ub;

ColorTest::ColorTest() {
    addTests<ColorTest>({&ColorTest::construct,
              &ColorTest::constructDefault,
              &ColorTest::constructOneValue,
              &ColorTest::constructParts,
              &ColorTest::constructConversion,
              &ColorTest::constructNormalization,
              &ColorTest::constructCopy,

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
    /* GCC 4.4 added its own variant of copy constructor and then thinks this
       is ambiguous */
    #ifndef CORRADE_GCC44_COMPATIBILITY
    constexpr Color3 a = {1.0f, 0.5f, 0.75f};
    #else
    constexpr Color3 a{1.0f, 0.5f, 0.75f};
    #endif
    CORRADE_COMPARE(a, Vector3(1.0f, 0.5f, 0.75f));

    #ifndef CORRADE_GCC44_COMPATIBILITY
    constexpr Color4 b = {1.0f, 0.5f, 0.75f, 0.5f};
    #else
    constexpr Color4 b{1.0f, 0.5f, 0.75f, 0.5f};
    #endif
    CORRADE_COMPARE(b, Vector4(1.0f, 0.5f, 0.75f, 0.5f));

    /* Default alpha */
    #ifndef CORRADE_GCC44_COMPATIBILITY
    constexpr Color4 c = {1.0f, 0.5f, 0.75f};
    constexpr Color4ub d = {10, 25, 176};
    #else
    constexpr Color4 c{1.0f, 0.5f, 0.75f};
    constexpr Color4ub d{10, 25, 176};
    #endif
    CORRADE_COMPARE(c, Vector4(1.0f, 0.5f, 0.75f, 1.0f));
    CORRADE_COMPARE(d, Math::Vector4<UnsignedByte>(10, 25, 176, 255));
}

void ColorTest::constructDefault() {
    constexpr Vector3 a;
    CORRADE_COMPARE(a, Color3(0.0f, 0.0f, 0.0f));

    constexpr Color4 b;
    constexpr Color4ub c;
    CORRADE_COMPARE(b, Color4(0.0f, 0.0f, 0.0f, 1.0f));
    CORRADE_COMPARE(c, Color4ub(0, 0, 0, 255));
}

void ColorTest::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Color3 a(0.25f);
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
    typedef BasicColor3<UnsignedByte> Color3ub;
    typedef BasicColor4<UnsignedByte> Color4ub;

    constexpr Color3 a(10.1f, 12.5f, 0.75f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Color3ub b(a);
    CORRADE_COMPARE(b, Color3ub(10, 12, 0));

    constexpr Color4 c(10.1f, 12.5f, 0.75f, 5.25f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Color4ub d(c);
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
    constexpr Color3 b(a);
    CORRADE_COMPARE(b, Color3(1.0f, 0.5f, 0.75f));

    constexpr Math::Vector<4, Float> c(1.0f, 0.5f, 0.75f, 0.25f);
    constexpr Color4 d(c);
    CORRADE_COMPARE(d, Color4(1.0f, 0.5f, 0.75f, 0.25f));
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
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(27.0f), 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(86.0f), 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(134.0f), 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(191.0f), 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(269.0f), 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(317.0f), 1.0f, 1.0f), Color3ub(255, 0, 182));
}

void ColorTest::hue() {
    CORRADE_COMPARE(Color3ub(255, 115, 0).hue(), Deg(27.058824f));
    CORRADE_COMPARE(Color3ub(145, 255, 0).hue(), Deg(85.882353f));
    CORRADE_COMPARE(Color3ub(0, 255, 60).hue(), Deg(134.11765f));
    CORRADE_COMPARE(Color3ub(0, 208, 255).hue(), Deg(191.05882f));
    CORRADE_COMPARE(Color3ub(123, 0, 255).hue(), Deg(268.94117f));
    CORRADE_COMPARE(Color3ub(255, 0, 183).hue(), Deg(316.94117f));
}

void ColorTest::fromSaturation() {
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(0.0f), 0.702f, 1.0f), Color3ub(255, 75, 75));
}

void ColorTest::saturation() {
    CORRADE_COMPARE(Color3ub(255, 76, 76).saturation(), 0.701961f);
    CORRADE_COMPARE(Color3ub().saturation(), 0.0f);
}

void ColorTest::fromValue() {
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(0.0f), 1.0f, 0.522f), Color3ub(133, 0, 0));
}

void ColorTest::value() {
    CORRADE_COMPARE(Color3ub(133, 0, 0).value(), 0.521569f);
}

void ColorTest::hsv() {
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(230.0f), 0.749f, 0.427f), Color3ub(27, 40, 108));

    Deg hue;
    Float saturation, value;
    std::tie(hue, saturation, value) = Color3ub(27, 41, 109).toHSV();
    CORRADE_COMPARE(hue, Deg(229.756106f));
    CORRADE_COMPARE(saturation, 0.752294f);
    CORRADE_COMPARE(value, 0.427451f);
}

void ColorTest::hsvOverflow() {
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(27.0f-360.0f), 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(86.0f-360.0f), 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(134.0f-360.0f), 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(191.0f-360.0f), 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(269.0f-360.0f), 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(317.0f-360.0f), 1.0f, 1.0f), Color3ub(255, 0, 182));

    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+27.0f), 1.0f, 1.0f), Color3ub(255, 114, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+86.0f), 1.0f, 1.0f), Color3ub(144, 255, 0));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+134.0f), 1.0f, 1.0f), Color3ub(0, 255, 59));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+191.0f), 1.0f, 1.0f), Color3ub(0, 208, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+269.0f), 1.0f, 1.0f), Color3ub(123, 0, 255));
    CORRADE_COMPARE(Color3ub::fromHSV(Deg(360.0f+317.0f), 1.0f, 1.0f), Color3ub(255, 0, 182));
}

void ColorTest::hsvAlpha() {
    CORRADE_COMPARE(Color4ub::fromHSV(std::make_tuple(Deg(230.0f), 0.749f, 0.427f), 23), Color4ub(27, 40, 108, 23));
    CORRADE_COMPARE(Color4ub::fromHSV(Deg(230.0f), 0.749f, 0.427f, 23), Color4ub(27, 40, 108, 23));
}

void ColorTest::swizzleType() {
    constexpr Color3 origColor3;
    constexpr BasicColor4<UnsignedByte> origColor4;

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto a = Math::swizzle<'y', 'z', 'r'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(a), const Color3>::value));

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto b = Math::swizzle<'y', 'z', 'a'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(b), const BasicColor3<UnsignedByte>>::value));

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto c = Math::swizzle<'y', 'z', 'y', 'x'>(origColor3);
    CORRADE_VERIFY((std::is_same<decltype(c), const Color4>::value));

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto d = Math::swizzle<'y', 'a', 'y', 'x'>(origColor4);
    CORRADE_VERIFY((std::is_same<decltype(d), const BasicColor4<UnsignedByte>>::value));
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
    Utility::Configuration c;

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

}}

CORRADE_TEST_MAIN(Magnum::Test::ColorTest)
