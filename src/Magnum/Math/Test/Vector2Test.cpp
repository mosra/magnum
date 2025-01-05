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
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Vector3.h" /* Vector3 used in Vector2Test::cross() */
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"

struct Vec2 {
    float x, y;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<2, float, Vec2> {
    constexpr static Vector<2, float> from(const Vec2& other) {
        return {other.x, other.y};
    }

    constexpr static Vec2 to(const Vector<2, float>& other) {
        return {other[0], other[1]};
    }
};

}

namespace Test { namespace {

struct Vector2Test: TestSuite::Tester {
    explicit Vector2Test();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructArray();
    void constructArrayRvalue();
    void constructConversion();
    void constructBit();
    void constructCopy();
    void convert();

    void access();
    void cross();
    void axes();
    void scales();
    void perpendicular();
    void aspectRatio();

    void multiplyDivideIntegral();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

using Magnum::Vector2;

Vector2Test::Vector2Test() {
    addTests({&Vector2Test::construct,
              &Vector2Test::constructDefault,
              &Vector2Test::constructNoInit,
              &Vector2Test::constructOneValue,
              &Vector2Test::constructArray,
              &Vector2Test::constructArrayRvalue,
              &Vector2Test::constructConversion,
              &Vector2Test::constructBit,
              &Vector2Test::constructCopy,
              &Vector2Test::convert,

              &Vector2Test::access,
              &Vector2Test::cross,
              &Vector2Test::axes,
              &Vector2Test::scales,
              &Vector2Test::perpendicular,
              &Vector2Test::aspectRatio,

              &Vector2Test::multiplyDivideIntegral,

              &Vector2Test::strictWeakOrdering,

              &Vector2Test::swizzleType,
              &Vector2Test::debug});
}

void Vector2Test::construct() {
    constexpr Vector2 a = {1.5f, 2.5f};
    CORRADE_COMPARE(a, (Vector<2, Float>(1.5f, 2.5f)));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, Float, Float>::value);
}

void Vector2Test::constructDefault() {
    constexpr Vector2 a;
    constexpr Vector2 b{ZeroInit};
    CORRADE_COMPARE(a, Vector2(0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector2(0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector2>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Vector2>::value);
}

void Vector2Test::constructNoInit() {
    Vector2 a{1.5f, 2.5f};
    new(&a) Vector2{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector2{1.5f, 2.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Vector2>::value);
}

void Vector2Test::constructOneValue() {
    constexpr Vector2 a(3.0f);
    CORRADE_COMPARE(a, Vector2(3.0f, 3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Vector2>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, Float>::value);
}

void Vector2Test::constructArray() {
    float data[]{1.3f, 2.7f};
    Vector2 a{data};
    CORRADE_COMPARE(a, (Vector2{1.3f, 2.7f}));

    constexpr float cdata[]{1.3f, 2.7f};
    constexpr Vector2 ca{cdata};
    CORRADE_COMPARE(ca, (Vector2{1.3f, 2.7f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[2], Vector2>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, float[2]>::value);

    /* See VectorTest::constructArray() for details */
    #if 0
    float data1[]{1.3f};
    float data4[]{1.3f, 2.7f, -15.0f, 7.0f};
    Vector2 b{data1};
    Vector2 c{data4};
    #endif
}

void Vector2Test::constructArrayRvalue() {
    /* Silly but why not. Could theoretically help with some fancier types
       that'd otherwise require explicit typing with the variadic
       constructor. */
    Vector2 a{{1.3f, 2.7f}};
    CORRADE_COMPARE(a, (Vector2{1.3f, 2.7f}));

    constexpr Vector2 ca{{1.3f, 2.7f}};
    CORRADE_COMPARE(ca, (Vector2{1.3f, 2.7f}));

    /* See VectorTest::constructArrayRvalue() for details. In case of a
       two-component vector, doing Vector2{{1.3f}} isn't an error, as it picks
       the one-value overload, and possibly only warns about "braces around
       scalar initializer". */
    #if 0
    Vector2 c{{1.3f, 2.7f, -15.0f, 7.0f}};
    #endif
}

void Vector2Test::constructConversion() {
    constexpr Vector2 a(1.5f, 2.5f);
    constexpr Vector2i b(a);
    CORRADE_COMPARE(b, Vector2i(1, 2));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vector2, Vector2i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, Vector2i>::value);
}

void Vector2Test::constructBit() {
    BitVector2 a{'\x1'}; /* 0b01 */
    CORRADE_COMPARE(Vector2{a}, (Vector2{1.0f, 0.0f}));

    constexpr BitVector2 ca{'\x1'}; /* 0b01 */
    constexpr Vector2 cb{ca};
    CORRADE_COMPARE(cb, (Vector2{1.0f, 0.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector2, Vector2>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector2, BitVector2>::value);
}

void Vector2Test::constructCopy() {
    constexpr Vector<2, Float> a(1.5f, 2.5f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector2 b(a);
    CORRADE_COMPARE(b, Vector2(1.5f, 2.5f));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Vector2>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Vector2>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector2>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector2>::value);
}

void Vector2Test::convert() {
    constexpr Vec2 a{1.5f, 2.0f};
    constexpr Vector2 b(1.5f, 2.0f);

    constexpr Vector2 c(a);
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Vec2(b);
    #else
    constexpr Vec2 d(b);
    #endif
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vec2, Vector2>::value);
    CORRADE_VERIFY(!std::is_convertible<Vector2, Vec2>::value);
}

void Vector2Test::access() {
    Vector2 vec(1.0f, -2.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.r(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.g(), -2.0f);

    constexpr Vector2 cvec(1.0f, -2.0f);
    constexpr Float x = cvec.x();
    constexpr Float r = cvec.r();
    constexpr Float y = cvec.y();
    constexpr Float g = cvec.g();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(r, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(g, -2.0f);
}

void Vector2Test::cross() {
    Vector2i a(1, -1);
    Vector2i b(4, 3);

    CORRADE_COMPARE(Math::cross(a, b), 7);
    CORRADE_COMPARE(Math::cross<Int>({a, 0}, {b, 0}), Vector3i(0, 0, Math::cross(a, b)));
}

void Vector2Test::axes() {
    constexpr Vector2 x = Vector2::xAxis(5.0f);
    constexpr Vector2 y = Vector2::yAxis(6.0f);
    CORRADE_COMPARE(x, Vector2(5.0f, 0.0f));
    CORRADE_COMPARE(y, Vector2(0.0f, 6.0f));
}

void Vector2Test::scales() {
    constexpr Vector2 x = Vector2::xScale(-5.0f);
    constexpr Vector2 y = Vector2::yScale(-0.2f);
    CORRADE_COMPARE(x, Vector2(-5.0f, 1.0f));
    CORRADE_COMPARE(y, Vector2(1.0f, -0.2f));
}

void Vector2Test::perpendicular() {
    const Vector2 a(0.5f, -15.0f);
    CORRADE_COMPARE(a.perpendicular(), Vector2(15.0f, 0.5f));
    CORRADE_COMPARE(dot(a.perpendicular(), a), 0.0f);
    CORRADE_COMPARE(Vector2::xAxis().perpendicular(), Vector2::yAxis());
}

void Vector2Test::aspectRatio() {
    CORRADE_COMPARE(Vector2(3.0f, 4.0f).aspectRatio(), 0.75f);
}

void Vector2Test::multiplyDivideIntegral() {
    const Vector2i vector{32, -6};
    const Vector2i multiplied{-48, 9};

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    CORRADE_COMPARE(-1.5f*vector, multiplied);

    constexpr Vector2i cvector{32, -6};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector2i ca1 = cvector*-1.5f;
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector2i ca2 = -1.5f*cvector;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);
}

void Vector2Test::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Vector2 v2a{1.0f, 2.0f};
    const Vector2 v2b{2.0f, 3.0f};
    const Vector2 v2c{1.0f, 3.0f};

    CORRADE_VERIFY( o(v2a, v2b));
    CORRADE_VERIFY(!o(v2b, v2a));
    CORRADE_VERIFY( o(v2a, v2c));
    CORRADE_VERIFY(!o(v2c, v2a));
    CORRADE_VERIFY( o(v2c, v2b));
    CORRADE_VERIFY(!o(v2b, v2c));

    CORRADE_VERIFY(!o(v2a, v2a));
}

void Vector2Test::swizzleType() {
    constexpr Vector<4, Int> orig;
    constexpr auto a = gather<'y', 'a'>(orig);
    CORRADE_VERIFY(std::is_same<decltype(a), const Vector2i>::value);
}

void Vector2Test::debug() {
    Containers::String out;
    Debug{&out} << Vector2(0.5f, 15.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 15)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector2Test)
