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
    void constructNormalization();
    void constructCopy();
    void convert();

    void literals();

    void colors();

    void hue();
    void saturation();
    void value();
    void hsv();
    void fromHsvHueOverflow();
    void fromHsvDefaultAlpha();

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

using namespace Literals;

ColorTest::ColorTest() {
    addTests({&ColorTest::construct,
              &ColorTest::constructDefault,
              &ColorTest::constructNoInit,
              &ColorTest::constructOneValue,
              &ColorTest::constructParts,
              &ColorTest::constructConversion,
              &ColorTest::constructNormalization,
              &ColorTest::constructCopy,
              &ColorTest::convert,

              &ColorTest::literals,

              &ColorTest::colors,

              &ColorTest::hue,
              &ColorTest::saturation,
              &ColorTest::value,
              &ColorTest::hsv,
              &ColorTest::fromHsvHueOverflow,
              &ColorTest::fromHsvDefaultAlpha,

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

void ColorTest::hue() {
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
