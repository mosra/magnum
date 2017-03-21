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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Math/Color.h"

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

namespace Test {

struct ColorTest: Corrade::TestSuite::Tester {
    explicit ColorTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
    void constructConversion();
    void constructPacking();
    void constructCopy();
    void convert();

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
    void srgbMonotonic();
    void srgbLiterals();

    void xyz();
    void fromXyzDefaultAlpha();
    void xyY();

    void swizzleType();
    void debug();
    void debugUb();
    void configuration();
};

typedef Math::Vector3<Float> Vector3;
typedef Math::Color3<Float> Color3;
typedef Math::Color3<UnsignedByte> Color3ub;

typedef Math::Vector4<Float> Vector4;
typedef Math::Color4<Float> Color4;
typedef Math::Color4<UnsignedByte> Color4ub;

typedef Math::Deg<Float> Deg;

ColorTest::ColorTest() {
    addTests({&ColorTest::construct,
              &ColorTest::constructDefault,
              &ColorTest::constructNoInit,
              &ColorTest::constructOneValue,
              &ColorTest::constructParts,
              &ColorTest::constructConversion,
              &ColorTest::constructPacking,
              &ColorTest::constructCopy,
              &ColorTest::convert,

              &ColorTest::data,

              &ColorTest::literals,

              &ColorTest::colors,

              &ColorTest::hue,
              &ColorTest::saturation,
              &ColorTest::value,
              &ColorTest::hsv,
              &ColorTest::fromHsvHueOverflow,
              &ColorTest::fromHsvDefaultAlpha,

              &ColorTest::srgb});

    /* Comparing with the previous one, so not 65536 */
    addRepeatedTests({&ColorTest::srgbMonotonic}, 65535);

    addTests({&ColorTest::fromSrgbDefaultAlpha,
              &ColorTest::srgbLiterals,

              &ColorTest::xyz,
              &ColorTest::fromXyzDefaultAlpha,
              &ColorTest::xyY,

              &ColorTest::swizzleType,
              &ColorTest::debug,
              &ColorTest::debugUb,
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

    CORRADE_VERIFY((std::is_nothrow_constructible<Color3, Float, Float, Float>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, Float, Float, Float, Float>::value));
}

void ColorTest::constructDefault() {
    constexpr Color3 a1;
    constexpr Color3 a2{Math::ZeroInit};
    CORRADE_COMPARE(a1, Color3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(a2, Color3(0.0f, 0.0f, 0.0f));

    constexpr Color4 b1;
    constexpr Color4 b2{Math::ZeroInit};
    CORRADE_COMPARE(b1, Color4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b2, Color4(0.0f, 0.0f, 0.0f, 0.0f));

    constexpr Color4ub c;
    CORRADE_COMPARE(c, Color4ub(0, 0, 0, 0));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Color3>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Color4>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Color3, ZeroInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, ZeroInitT>::value));
}

void ColorTest::constructNoInit() {
    Color3 a{1.0f, 0.5f, 0.75f};
    Color4 b{1.0f, 0.5f, 0.75f, 0.5f};
    new(&a) Color3{Math::NoInit};
    new(&b) Color4{Math::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Color3{1.0f, 0.5f, 0.75f}));
        CORRADE_COMPARE(b, (Color4{1.0f, 0.5f, 0.75f, 0.5f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Color3, NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Color3>::value));
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, Color4>::value));
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

    CORRADE_VERIFY((std::is_nothrow_constructible<Color3, Float>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, Float, Float>::value));
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

    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, Color3, Float>::value));
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

    CORRADE_VERIFY((std::is_nothrow_constructible<Color3ub, Color3>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Color4, Color4ub>::value));
}

void ColorTest::constructPacking() {
    constexpr Color3 a(1.0f, 0.5f, 0.75f);
    auto b = Math::pack<Color3ub>(a);
    CORRADE_COMPARE(b, Color3ub(255, 127, 191));

    constexpr Color4 c(1.0f, 0.5f, 0.75f, 0.25f);
    auto d = Math::pack<Color4ub>(c);
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

    constexpr Vec3 d3(b3);
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

    constexpr Vec4 d4(b4);
    CORRADE_COMPARE(d4.x, a4.x);
    CORRADE_COMPARE(d4.y, a4.y);
    CORRADE_COMPARE(d4.z, a4.z);
    CORRADE_COMPARE(d4.w, a4.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec3, Color3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Vec4, Color4>::value));
    CORRADE_VERIFY(!(std::is_convertible<Color3, Vec3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Color4, Vec4>::value));
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

    CORRADE_VERIFY((std::is_same<decltype(c.xyz()), Color3&>::value));
    CORRADE_VERIFY((std::is_same<decltype(cc.xyz()), const Color3>::value));
    CORRADE_VERIFY((std::is_same<decltype(c.rgb()), Color3&>::value));
    CORRADE_VERIFY((std::is_same<decltype(cc.rgb()), const Color3>::value));
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
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::red(), (Math::Color3<UnsignedShort>{65535, 0, 0}));
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::green(), (Math::Color3<UnsignedShort>{0, 65535, 0}));
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::blue(), (Math::Color3<UnsignedShort>{0, 0, 65535}));

    CORRADE_COMPARE(Math::Color4<UnsignedShort>::red(), (Math::Color4<UnsignedShort>{65535, 0, 0, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::green(), (Math::Color4<UnsignedShort>{0, 65535, 0, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::blue(), (Math::Color4<UnsignedShort>{0, 0, 65535, 65535}));

    CORRADE_COMPARE(Math::Color4<UnsignedShort>::cyan(), (Math::Color4<UnsignedShort>{0, 65535, 65535, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::magenta(), (Math::Color4<UnsignedShort>{65535, 0, 65535, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::yellow(), (Math::Color4<UnsignedShort>{65535, 65535, 0, 65535}));
}

void ColorTest::hue() {
    using namespace Literals;

    CORRADE_COMPARE(Color3::fromHsv( 27.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv( 86.0_degf, 1.0f, 1.0f), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv(134.0_degf, 1.0f, 1.0f), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv(191.0_degf, 1.0f, 1.0f), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(269.0_degf, 1.0f, 1.0f), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(317.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.0f, 0.716667f}));

    CORRADE_COMPARE((Color3{1.0f, 0.45f, 0.0f}).hue(),      27.0_degf);
    CORRADE_COMPARE((Color3{0.566667f, 1.0f, 0.0f}).hue(),  86.0_degf);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.233333f}).hue(), 134.0_degf);
    CORRADE_COMPARE((Color3{0.0f, 0.816667f, 1.0f}).hue(), 191.0_degf);
    CORRADE_COMPARE((Color3{0.483333f, 0.0f, 1.0f}).hue(), 269.0_degf);
    CORRADE_COMPARE((Color3{1.0f, 0.0f, 0.716667f}).hue(), 317.0_degf);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv(27.0_degf, 1.0f, 1.0f, 0.175f), (Color4{1.0f, 0.45f, 0.0f, 0.175f}));
    CORRADE_COMPARE((Color4{1.0f, 0.45f, 0.0f, 0.175f}).hue(), 27.0_degf);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromHsv(27.0_degf, 1.0f, 1.0f),
        (Math::Color3<UnsignedShort>{65535, 29490, 0}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(27.0_degf, 1.0f, 1.0f, 15239),
        (Math::Color4<UnsignedShort>{65535, 29490, 0, 15239}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{65535, 29490, 0}).hue(), 26.9993_degf);
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{65535, 29490, 0, 15239}.hue()), 26.9993_degf);
}

void ColorTest::saturation() {
    using namespace Literals;

    CORRADE_COMPARE(Color3::fromHsv(0.0_degf, 0.702f, 1.0f), (Color3{1.0f, 0.298f, 0.298f}));
    CORRADE_COMPARE((Color3{1.0f, 0.298f, 0.298f}).saturation(), 0.702f);

    /* Extremes */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).saturation(), 0.0f);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.0f}).saturation(), 1.0f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv(0.0_degf, 0.702f, 1.0f, 0.175f), (Color4{1.0f, 0.298f, 0.298f, 0.175f}));
    CORRADE_COMPARE((Color4{1.0f, 0.298f, 0.298f, 0.175f}).saturation(), 0.702f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromHsv(0.0_degf, 0.702f, 1.0f),
        (Math::Color3<UnsignedShort>{65535, 19529, 19529}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(0.0_degf, 0.702f, 1.0f, 15239),
        (Math::Color4<UnsignedShort>{65535, 19529, 19529, 15239}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{65535, 19529, 19529}.saturation()), 0.702007f);
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{65535, 19529, 19529, 15239}.saturation()), 0.702007f);
}

void ColorTest::value() {
    using namespace Literals;

    CORRADE_COMPARE(Color3::fromHsv(0.0_degf, 1.0f, 0.522f), (Color3{0.522f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color3{0.522f, 0.0f, 0.0f}).value(), 0.522f);

    /* Extremes */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).value(), 0.0f);
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.0f}).value(), 1.0f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv(0.0_degf, 1.0f, 0.522f, 0.175f), (Color4{0.522f, 0.0f, 0.0f, 0.175f}));
    CORRADE_COMPARE((Color4{0.522f, 0.0f, 0.0f, 0.175f}).value(), 0.522f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromHsv(0.0_degf, 1.0f, 0.522f),
        (Math::Color3<UnsignedShort>{34209, 0, 0}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(0.0_degf, 1.0f, 0.522f, 15239),
        (Math::Color4<UnsignedShort>{34209, 0, 0, 15239}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{34209, 0, 0}.value()), 0.521996f);
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{34209, 0, 0, 15239}.value()), 0.521996f);
}

void ColorTest::hsv() {
    using namespace Literals;

    CORRADE_COMPARE(Color3::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f)),
        (Color3{0.107177f, 0.160481f, 0.427f}));
    CORRADE_COMPARE(Color3::fromHsv(230.0_degf, 0.749f, 0.427f),
        (Color3{0.107177f, 0.160481f, 0.427f}));

    Deg hue;
    Float saturation, value;
    std::tie(hue, saturation, value) = Color3{0.107177f, 0.160481f, 0.427f}.toHsv();
    CORRADE_COMPARE(hue, 230.0_degf);
    CORRADE_COMPARE(saturation, 0.749f);
    CORRADE_COMPARE(value, 0.427f);

    /* RGBA */
    CORRADE_COMPARE(Color4::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f), 0.175f),
        (Color4{0.107177f, 0.160481f, 0.427f, 0.175f}));
    CORRADE_COMPARE(Color4::fromHsv(230.0_degf, 0.749f, 0.427f, 0.175f),
        (Color4{0.107177f, 0.160481f, 0.427f, 0.175f}));

    std::tie(hue, saturation, value) = Color4{0.107177f, 0.160481f, 0.427f, 0.175f}.toHsv();
    CORRADE_COMPARE(hue, 230.0_degf);
    CORRADE_COMPARE(saturation, 0.749f);
    CORRADE_COMPARE(value, 0.427f);

    /* Integral -- little precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f)),
        (Math::Color3<UnsignedShort>{7023, 10517, 27983}));
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromHsv(230.0_degf, 0.749f, 0.427f),
        (Math::Color3<UnsignedShort>{7023, 10517, 27983}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f), 15239),
        (Math::Color4<UnsignedShort>{7023, 10517, 27983, 15239}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(230.0_degf, 0.749f, 0.427f, 15239),
        (Math::Color4<UnsignedShort>{7023, 10517, 27983, 15239}));

    std::tie(hue, saturation, value) = Math::Color3<UnsignedShort>{7023, 10517, 27983}.toHsv();
    CORRADE_COMPARE(hue, 230.0_degf);
    CORRADE_COMPARE(saturation, 0.749026f);
    CORRADE_COMPARE(value, 0.426993f);

    std::tie(hue, saturation, value) = Math::Color4<UnsignedShort>{7023, 10517, 27983, 15239}.toHsv();
    CORRADE_COMPARE(hue, 230.0_degf);
    CORRADE_COMPARE(saturation, 0.749026f);
    CORRADE_COMPARE(value, 0.426993f);

    /* Round-trip */
    CORRADE_COMPARE(Color3::fromHsv(230.0_degf, 0.749f, 0.427f).toHsv(),
                    std::make_tuple(230.0_degf, 0.749f, 0.427f));
    CORRADE_COMPARE(Color4::fromHsv(230.0_degf, 0.749f, 0.427f, 0.175f).toHsv(),
                    std::make_tuple(230.0_degf, 0.749f, 0.427f));
}

void ColorTest::fromHsvHueOverflow() {
    using namespace Literals;

    CORRADE_COMPARE(Color3::fromHsv( 27.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv( 86.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv(134.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv(191.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(269.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(317.0_degf - 360.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.0f, 0.716667f}));

    CORRADE_COMPARE(Color3::fromHsv( 27.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.45f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv( 86.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{0.566667f, 1.0f, 0.0f}));
    CORRADE_COMPARE(Color3::fromHsv(134.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{0.0f, 1.0f, 0.233333f}));
    CORRADE_COMPARE(Color3::fromHsv(191.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{0.0f, 0.816667f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(269.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{0.483333f, 0.0f, 1.0f}));
    CORRADE_COMPARE(Color3::fromHsv(317.0_degf + 360.0_degf, 1.0f, 1.0f), (Color3{1.0f, 0.0f, 0.716667f}));
}

void ColorTest::fromHsvDefaultAlpha() {
    using namespace Literals;

    CORRADE_COMPARE(Color4::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f)),
        (Color4{0.107177f, 0.160481f, 0.427f, 1.0f}));
    CORRADE_COMPARE(Color4::fromHsv(230.0_degf, 0.749f, 0.427f),
        (Color4{0.107177f, 0.160481f, 0.427f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(std::make_tuple(230.0_degf, 0.749f, 0.427f)),
        (Math::Color4<UnsignedShort>{7023, 10517, 27983, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromHsv(230.0_degf, 0.749f, 0.427f),
        (Math::Color4<UnsignedShort>{7023, 10517, 27983, 65535}));
}

void ColorTest::srgb() {
    /* Linear start */
    CORRADE_COMPARE(Color3::fromSrgb({0.01292f, 0.01938f, 0.0437875f}), (Color3{0.001f, 0.0015f, 0.0034f}));
    CORRADE_COMPARE(Color3::fromSrgb({0.02584f, 0.03876f, 0.0768655f}), (Color3{0.002f, 0.0030f, 0.0068f}));
    CORRADE_COMPARE((Color3{0.001f, 0.0015f, 0.0034f}).toSrgb(), (Vector3{0.01292f, 0.01938f, 0.0437875f}));
    CORRADE_COMPARE((Color3{0.002f, 0.0030f, 0.0068f}).toSrgb(), (Vector3{0.02584f, 0.03876f, 0.0768655f}));

    /* Power series */
    CORRADE_COMPARE((Color3{0.0005f, 0.135f, 0.278f}).toSrgb(), (Vector3{0.00646f, 0.403027f, 0.563877f}));
    CORRADE_COMPARE((Color3{0.0020f, 0.352f, 0.895f}).toSrgb(), (Vector3{0.02584f, 0.627829f, 0.952346f}));

    /* Extremes */
    CORRADE_COMPARE((Color3{0.0f, 1.0f, 0.5f}).toSrgb(), (Vector3{0.0f, 1.0f, 0.735357f}));

    /* RGBA */
    CORRADE_COMPARE(Color4::fromSrgbAlpha({0.625398f, 0.02584f, 0.823257f, 0.175f}),
        (Color4{0.349f, 0.0020f, 0.644f, 0.175f}));
    CORRADE_COMPARE(Color4::fromSrgb({0.625398f, 0.02584f, 0.823257f}, 0.175f),
        (Color4{0.349f, 0.0020f, 0.644f, 0.175f}));
    CORRADE_COMPARE((Color4{0.349f, 0.0020f, 0.644f, 0.175f}).toSrgbAlpha(),
        (Vector4{0.625398f, 0.02584f, 0.823257f, 0.175f}));

    /* Integral RGB */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromSrgb({0.1523f, 0.00125f, 0.9853f}),
        (Math::Color3<UnsignedShort>{1319, 6, 63364}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgbAlpha({0.1523f, 0.00125f, 0.9853f, 0.175f}),
        (Math::Color4<UnsignedShort>{1319, 6, 63364, 11468}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgb({0.1523f, 0.00125f, 0.9853f}, 15299),
        (Math::Color4<UnsignedShort>{1319, 6, 63364, 15299}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{1319, 6, 63364}).toSrgb(),
        (Vector3{0.152248f, 0.00118288f, 0.985295f}));
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{1319, 6, 63364, 11468}).toSrgbAlpha(),
        (Vector4{0.152248f, 0.00118288f, 0.985295f, 0.175f}));

    /* Integral 8bit sRGB -- slight precision loss */
    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}),
        (Color3{0.896269f, 0.0231534f, 0.215861f}));
    CORRADE_COMPARE(Color4::fromSrgbAlpha<UnsignedByte>({0xf3, 0x2a, 0x80, 0x23}),
        (Color4{0.896269f, 0.0231534f, 0.215861f, 0.137255f}));
    CORRADE_COMPARE(Color4::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}, 0.175f),
        (Color4{0.896269f, 0.0231534f, 0.215861f, 0.175f}));
    CORRADE_COMPARE((Color3{0.896269f, 0.0231534f, 0.215861f}).toSrgb<UnsignedByte>(),
        (Math::Vector3<UnsignedByte>{0xf2, 0x2a, 0x80}));
    CORRADE_COMPARE((Color4{0.896269f, 0.0231534f, 0.215861f, 0.137255f}).toSrgbAlpha<UnsignedByte>(),
        (Math::Vector4<UnsignedByte>{0xf2, 0x2a, 0x80, 0x23}));

    /* Integral both -- larger precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}),
        (Math::Color3<UnsignedShort>{58737, 1517, 14146}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgbAlpha<UnsignedByte>({0xf3, 0x2a, 0x80, 0x23}),
        (Math::Color4<UnsignedShort>{58737, 1517, 14146, 8995}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}, 15299),
        (Math::Color4<UnsignedShort>{58737, 1517, 14146, 15299}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{58737, 1517, 14146}).toSrgb<UnsignedByte>(),
        (Math::Vector3<UnsignedByte>{0xf2, 0x29, 0x7f}));
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{58737, 1517, 14146, 8995}).toSrgbAlpha<UnsignedByte>(),
        (Math::Vector4<UnsignedByte>{0xf2, 0x29, 0x7f, 0x23}));

    /* Round-trip */
    CORRADE_COMPARE(Color3::fromSrgb({0.00646f, 0.403027f, 0.563877f}).toSrgb(),
        (Vector3{0.00646f, 0.403027f, 0.563877f}));
    CORRADE_COMPARE(Color4::fromSrgbAlpha({0.00646f, 0.403027f, 0.563877f, 0.175f}).toSrgbAlpha(),
        (Vector4{0.00646f, 0.403027f, 0.563877f, 0.175f}));
}

void ColorTest::fromSrgbDefaultAlpha() {
    CORRADE_COMPARE(Color4::fromSrgb({0.625398f, 0.02584f, 0.823257f}),
        (Color4{0.349f, 0.0020f, 0.644f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgb({0.1523f, 0.00125f, 0.9853f}),
        (Math::Color4<UnsignedShort>{1319, 6, 63364, 65535}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromSrgb<UnsignedByte>({0xf3, 0x2a, 0x80}),
        (Math::Color4<UnsignedShort>{58737, 1517, 14146, 65535}));
}

void ColorTest::srgbMonotonic() {
    Color3 rgbPrevious = Color3::fromSrgb(Math::Vector3<UnsignedShort>(testCaseRepeatId()));
    Color3 rgb = Color3::fromSrgb(Math::Vector3<UnsignedShort>(testCaseRepeatId() + 1));
    CORRADE_COMPARE_AS(rgb, rgbPrevious, Corrade::TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(rgb, Color3(0.0f), Corrade::TestSuite::Compare::GreaterOrEqual);
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(testCaseRepeatId() == 65534 && !!(rgb > Color3(1.0f)),
            "Some minor rounding error possibly due to some Esmcripten "
            "optimizations. Happens only on larger optimization levels, not "
            "on -O1.");
        #endif
        CORRADE_COMPARE_AS(rgb, Color3(1.0f), Corrade::TestSuite::Compare::LessOrEqual);
    }
}

void ColorTest::srgbLiterals() {
    using namespace Literals;

    constexpr Math::Vector3<UnsignedByte> a = 0x33b27f_srgb;
    CORRADE_COMPARE(a, (Math::Vector3<UnsignedByte>{0x33, 0xb2, 0x7f}));

    constexpr Math::Vector4<UnsignedByte> b = 0x33b27fcc_srgba;
    CORRADE_COMPARE(b, (Math::Vector4<UnsignedByte>{0x33, 0xb2, 0x7f, 0xcc}));

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
    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>({232, 157, 16}).toXyz(),
        (Vector3{0.454279f, 0.413092f, 0.0607124f}));
    CORRADE_COMPARE(Color3::fromXyz({0.454279f, 0.413092f, 0.0607124f}).toSrgb<UnsignedByte>(),
        (Math::Vector3<UnsignedByte>{231, 156, 16}));
    CORRADE_COMPARE(Color3::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Color3{0.806952f, 0.337163f, 0.0051861f}));

    CORRADE_COMPARE(Color3::fromSrgb<UnsignedByte>({96, 43, 193}).toXyz(),
        (Vector3{0.153122f, 0.0806478f, 0.512037f}));
    CORRADE_COMPARE(Color3::fromXyz({0.153122f, 0.0806478f, 0.512037f}).toSrgb<UnsignedByte>(),
        (Math::Vector3<UnsignedByte>{95, 43, 192}));
    CORRADE_COMPARE(Color3::fromXyz({0.153122f, 0.0806478f, 0.512037f}),
        (Color3{0.11697f, 0.0241579f, 0.533276f}));

    /* Extremes -- for black it should be zeros, for white roughly X = 0.95,
       Y = 1, Z = 1.09 corresponding to white point in D65 */
    CORRADE_COMPARE((Color3{0.0f, 0.0f, 0.0f}).toXyz(),
        (Vector3{0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE((Color3{1.0f, 1.0f, 1.0f}).toXyz(),
        (Vector3{0.950456f, 1.0f, 1.08906f}));

    /* RGBA */
    CORRADE_COMPARE(Color4::fromXyz({0.454279f, 0.413092f, 0.0607124f}, 0.175f),
        (Color4{0.806952f, 0.337163f, 0.0051861f, 0.175f}));
    CORRADE_COMPARE(Color4::fromSrgb<UnsignedByte>({232, 157, 16}, 0.175f).toXyz(),
        (Vector3{0.454279f, 0.413092f, 0.0607124f}));

    /* Integral -- slight precision loss */
    CORRADE_COMPARE(Math::Color3<UnsignedShort>::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Math::Color3<UnsignedShort>{52883, 22095, 339}));
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromXyz({0.454279f, 0.413092f, 0.0607124f}, 15299),
        (Math::Color4<UnsignedShort>{52883, 22095, 339, 15299}));
    CORRADE_COMPARE((Math::Color3<UnsignedShort>{52883, 22095, 339}).toXyz(),
        (Vector3{0.454268f, 0.413079f, 0.0607021f}));
    CORRADE_COMPARE((Math::Color4<UnsignedShort>{52883, 22095, 339, 15299}).toXyz(),
        (Vector3{0.454268f, 0.413079f, 0.0607021f}));

    /* Round-trip */
    CORRADE_COMPARE(Color3::fromXyz({0.454279f, 0.413092f, 0.0607124f}).toXyz(),
        (Vector3{0.454279f, 0.413092f, 0.0607124f}));
    CORRADE_COMPARE(Color3::fromXyz({0.153122f, 0.0806478f, 0.512037f}).toXyz(),
        (Vector3{0.153122f, 0.0806478f, 0.512037f}));
    CORRADE_COMPARE(Color4::fromXyz({0.454279f, 0.413092f, 0.0607124f}, 0.175f).toXyz(),
        (Vector3{0.454279f, 0.413092f, 0.0607124f}));
}

void ColorTest::fromXyzDefaultAlpha() {
    CORRADE_COMPARE(Color4::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Color4{0.806952f, 0.337163f, 0.0051861f, 1.0f}));

    /* Integral */
    CORRADE_COMPARE(Math::Color4<UnsignedShort>::fromXyz({0.454279f, 0.413092f, 0.0607124f}),
        (Math::Color4<UnsignedShort>{52883, 22095, 339, 65535}));
}

void ColorTest::xyY() {
    /* Verified using http://www.easyrgb.com/index.php?X=CALC */
    Vector3 xyz{0.454279f, 0.413092f, 0.0607124f};
    Vector3 xyY{0.489481f, 0.445102f, 0.413092f};

    CORRADE_COMPARE(xyzToXyY(xyz), xyY);
    CORRADE_COMPARE(xyYToXyz(xyY), xyz);
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

void ColorTest::debugUb() {
    using namespace Magnum::Math::Literals;

    std::ostringstream o;
    Debug(&o) << 0x123456_rgb << 0x789abc_rgb;
    CORRADE_COMPARE(o.str(), "#123456 #789abc\n");

    o.str({});
    Debug(&o) << 0x12345678_rgba << 0x90abcdef_rgba;
    CORRADE_COMPARE(o.str(), "#12345678 #90abcdef\n");
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
