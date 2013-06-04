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

        void access();

        void fromHue();
        void fromSaturation();
        void fromValue();

        void hue();
        void saturation();
        void value();

        void hsv();
        void hsvOverflow();
        void hsvAlpha();

        void debug();
        void configuration();
};

typedef Magnum::Color3<UnsignedByte> Color3;
typedef Magnum::Color4<UnsignedByte> Color4;
typedef Magnum::Color3<Float> Color3f;
typedef Magnum::Color4<Float> Color4f;

ColorTest::ColorTest() {
    addTests({&ColorTest::access,

              &ColorTest::fromHue,
              &ColorTest::fromSaturation,
              &ColorTest::fromValue,

              &ColorTest::hue,
              &ColorTest::saturation,
              &ColorTest::value,

              &ColorTest::hsv,
              &ColorTest::hsvOverflow,
              &ColorTest::hsvAlpha,

              &ColorTest::debug,
              &ColorTest::configuration});
}

void ColorTest::access() {
    Color3f c3(15, 255, 10);
    const Color3f cc3(15, 255, 10);

    CORRADE_COMPARE(c3.r(), 15);
    CORRADE_COMPARE(c3.g(), 255);
    CORRADE_COMPARE(c3.b(), 10);
    CORRADE_COMPARE(cc3.r(), 15);
    CORRADE_COMPARE(cc3.g(), 255);
    CORRADE_COMPARE(cc3.b(), 10);

    Color4 c4(125, 98, 51, 22);
    const Color4f cc4(125, 98, 51, 22);

    CORRADE_COMPARE(c4.r(), 125);
    CORRADE_COMPARE(c4.g(), 98);
    CORRADE_COMPARE(c4.b(), 51);
    CORRADE_COMPARE(c4.a(), 22);
    CORRADE_COMPARE(cc4.r(), 125);
    CORRADE_COMPARE(cc4.g(), 98);
    CORRADE_COMPARE(cc4.b(), 51);
    CORRADE_COMPARE(cc4.a(), 22);
}

void ColorTest::fromHue() {
    CORRADE_COMPARE(Color3::fromHSV(Deg(27.0f), 1.0f, 1.0f), Color3(255, 114, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(86.0f), 1.0f, 1.0f), Color3(144, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(134.0f), 1.0f, 1.0f), Color3(0, 255, 59));
    CORRADE_COMPARE(Color3::fromHSV(Deg(191.0f), 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(269.0f), 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(317.0f), 1.0f, 1.0f), Color3(255, 0, 182));
}

void ColorTest::hue() {
    CORRADE_COMPARE(Color3(255, 115, 0).hue(), Deg(27.058824f));
    CORRADE_COMPARE(Color3(145, 255, 0).hue(), Deg(85.882353f));
    CORRADE_COMPARE(Color3(0, 255, 60).hue(), Deg(134.11765f));
    CORRADE_COMPARE(Color3(0, 208, 255).hue(), Deg(191.05882f));
    CORRADE_COMPARE(Color3(123, 0, 255).hue(), Deg(268.94117f));
    CORRADE_COMPARE(Color3(255, 0, 183).hue(), Deg(316.94117f));
}

void ColorTest::fromSaturation() {
    CORRADE_COMPARE(Color3::fromHSV(Deg(0.0f), 0.702f, 1.0f), Color3(255, 75, 75));
}

void ColorTest::saturation() {
    CORRADE_COMPARE(Color3(255, 76, 76).saturation(), 0.701961f);
    CORRADE_COMPARE(Color3().saturation(), 0.0f);
}

void ColorTest::fromValue() {
    CORRADE_COMPARE(Color3::fromHSV(Deg(0.0f), 1.0f, 0.522f), Color3(133, 0, 0));
}

void ColorTest::value() {
    CORRADE_COMPARE(Color3(133, 0, 0).value(), 0.521569f);
}

void ColorTest::hsv() {
    CORRADE_COMPARE(Color3::fromHSV(Deg(230.0f), 0.749f, 0.427f), Color3(27, 40, 108));

    Deg hue;
    Float saturation, value;
    std::tie(hue, saturation, value) = Color3(27, 41, 109).toHSV();
    CORRADE_COMPARE(hue, Deg(229.756106f));
    CORRADE_COMPARE(saturation, 0.752294f);
    CORRADE_COMPARE(value, 0.427451f);
}

void ColorTest::hsvOverflow() {
    CORRADE_COMPARE(Color3::fromHSV(Deg(27.0f-360.0f), 1.0f, 1.0f), Color3(255, 114, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(86.0f-360.0f), 1.0f, 1.0f), Color3(144, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(134.0f-360.0f), 1.0f, 1.0f), Color3(0, 255, 59));
    CORRADE_COMPARE(Color3::fromHSV(Deg(191.0f-360.0f), 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(269.0f-360.0f), 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(317.0f-360.0f), 1.0f, 1.0f), Color3(255, 0, 182));

    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+27.0f), 1.0f, 1.0f), Color3(255, 114, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+86.0f), 1.0f, 1.0f), Color3(144, 255, 0));
    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+134.0f), 1.0f, 1.0f), Color3(0, 255, 59));
    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+191.0f), 1.0f, 1.0f), Color3(0, 208, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+269.0f), 1.0f, 1.0f), Color3(123, 0, 255));
    CORRADE_COMPARE(Color3::fromHSV(Deg(360.0f+317.0f), 1.0f, 1.0f), Color3(255, 0, 182));
}

void ColorTest::hsvAlpha() {
    CORRADE_COMPARE(Color4::fromHSV(std::make_tuple(Deg(230.0f), 0.749f, 0.427f), 23), Color4(27, 40, 108, 23));
    CORRADE_COMPARE(Color4::fromHSV(Deg(230.0f), 0.749f, 0.427f, 23), Color4(27, 40, 108, 23));
}

void ColorTest::debug() {
    std::ostringstream o;
    Debug(&o) << Color3f(0.5f, 0.75f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 1)\n");

    o.str({});
    Debug(&o) << Color4f(0.5f, 0.75f, 0.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 0.75, 0, 1)\n");
}

void ColorTest::configuration() {
    Utility::Configuration c;

    Color3f color3(0.5f, 0.75f, 1.0f);
    std::string value3("0.5 0.75 1");

    c.setValue("color3", color3);
    CORRADE_COMPARE(c.value("color3"), value3);
    CORRADE_COMPARE(c.value<Color3f>("color3"), color3);

    Color4f color4(0.5f, 0.75f, 0.0f, 1.0f);
    std::string value4("0.5 0.75 0 1");

    c.setValue("color4", color4);
    CORRADE_COMPARE(c.value("color4"), value4);
    CORRADE_COMPARE(c.value<Color4f>("color4"), color4);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ColorTest)
