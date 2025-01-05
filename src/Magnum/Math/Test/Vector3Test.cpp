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

#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"

struct Vec3 {
    float x, y, z;
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

}

namespace Test { namespace {

struct Vector3Test: TestSuite::Tester {
    explicit Vector3Test();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
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
    void twoComponent();

    void multiplyDivideIntegral();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

using Magnum::Vector3;
using Magnum::Vector2;

Vector3Test::Vector3Test() {
    addTests({&Vector3Test::construct,
              &Vector3Test::constructDefault,
              &Vector3Test::constructNoInit,
              &Vector3Test::constructOneValue,
              &Vector3Test::constructParts,
              &Vector3Test::constructArray,
              &Vector3Test::constructArrayRvalue,
              &Vector3Test::constructConversion,
              &Vector3Test::constructBit,
              &Vector3Test::constructCopy,
              &Vector3Test::convert,

              &Vector3Test::access,
              &Vector3Test::cross,
              &Vector3Test::axes,
              &Vector3Test::scales,
              &Vector3Test::twoComponent,

              &Vector3Test::multiplyDivideIntegral,

              &Vector3Test::strictWeakOrdering,

              &Vector3Test::swizzleType,
              &Vector3Test::debug});
}

void Vector3Test::construct() {
    constexpr Vector3 a = {1.0f, 2.5f, -3.0f};
    CORRADE_COMPARE(a, (Vector<3, Float>(1.0f, 2.5f, -3.0f)));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, Float, Float, Float>::value);
}

void Vector3Test::constructDefault() {
    constexpr Vector3 a;
    constexpr Vector3 b{ZeroInit};
    CORRADE_COMPARE(a, Vector3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector3(0.0f, 0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector3>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Vector3>::value);
}

void Vector3Test::constructNoInit() {
    Vector3 a{1.0f, 2.5f, -3.0f};
    new(&a) Vector3{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector3{1.0f, 2.5f, -3.0f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Vector3>::value);
}

void Vector3Test::constructOneValue() {
    constexpr Vector3 a(-3.0f);
    CORRADE_COMPARE(a, Vector3(-3.0f, -3.0f, -3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Vector3>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, Float>::value);
}

void Vector3Test::constructParts() {
    constexpr Vector2 a(1.0f, 2.0f);
    constexpr Vector3 b = {a, 3.0f};
    CORRADE_COMPARE(b, Vector3(1.0f, 2.0f, 3.0f));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, Vector2, Float>::value);
}

void Vector3Test::constructArray() {
    float data[]{1.3f, 2.7f, -15.0f};
    Vector3 a{data};
    CORRADE_COMPARE(a, (Vector3{1.3f, 2.7f, -15.0f}));

    constexpr float cdata[]{1.3f, 2.7f, -15.0f};
    constexpr Vector3 ca{cdata};
    CORRADE_COMPARE(ca, (Vector3{1.3f, 2.7f, -15.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[3], Vector3>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, float[3]>::value);

    /* See VectorTest::constructArray() for details */
    #if 0
    float data1[]{1.3f};
    float data4[]{1.3f, 2.7f, -15.0f, 7.0f};
    Vector3 b{data1};
    Vector3 c{data4};
    #endif
}

void Vector3Test::constructArrayRvalue() {
    /* Silly but why not. Could theoretically help with some fancier types
       that'd otherwise require explicit typing with the variadic
       constructor. */
    Vector3 a{{1.3f, 2.7f, -15.0f}};
    CORRADE_COMPARE(a, (Vector3{1.3f, 2.7f, -15.0f}));

    constexpr Vector3 ca{{1.3f, 2.7f, -15.0f}};
    CORRADE_COMPARE(ca, (Vector3{1.3f, 2.7f, -15.0f}));

    /* See VectorTest::constructArrayRvalue() for details */
    #if 0
    Vector3 c{{1.3f, 2.7f, -15.0f, 7.0f}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a Vector from a smaller array isn't an error on GCC 4.8.");
    Vector3 b{{1.3f, 2.7f}};
    #endif
}

void Vector3Test::constructConversion() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    constexpr Vector3i b(a);
    CORRADE_COMPARE(b, Vector3i(1, 2, -3));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vector3, Vector3i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, Vector3i>::value);
}

void Vector3Test::constructBit() {
    BitVector3 a{'\x5'}; /* 0b101 */
    CORRADE_COMPARE(Vector3{a}, (Vector3{1.0f, 0.0f, 1.0f}));

    constexpr BitVector3 ca{'\x5'}; /* 0b101 */
    constexpr Vector3 cb{ca};
    CORRADE_COMPARE(cb, (Vector3{1.0f, 0.0f, 1.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector3, Vector3>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, BitVector3>::value);
}

void Vector3Test::constructCopy() {
    constexpr Vector<3, Float> a(1.0f, 2.5f, -3.0f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector3 b(a);
    CORRADE_COMPARE(b, Vector3(1.0f, 2.5f, -3.0f));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Vector3>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Vector3>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector3>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector3>::value);
}

void Vector3Test::convert() {
    constexpr Vec3 a{1.5f, 2.0f, -3.5f};
    constexpr Vector3 b(1.5f, 2.0f, -3.5f);

    constexpr Vector3 c(a);
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Vec3(b);
    #else
    constexpr Vec3 d(b);
    #endif
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vec3, Vector3>::value);
    CORRADE_VERIFY(!std::is_convertible<Vector3, Vec3>::value);
}

void Vector3Test::access() {
    Vector3 vec(1.0f, -2.0f, 5.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.r(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.g(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);
    CORRADE_COMPARE(vec.b(), 5.0f);

    constexpr Vector3 cvec(1.0f, -2.0f, 5.0f);
    constexpr Float x = cvec.x();
    constexpr Float r = cvec.r();
    constexpr Float y = cvec.y();
    constexpr Float g = cvec.g();
    constexpr Float z = cvec.z();
    constexpr Float b = cvec.b();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(r, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(g, -2.0f);
    CORRADE_COMPARE(z, 5.0f);
    CORRADE_COMPARE(b, 5.0f);
}

void Vector3Test::cross() {
    Vector3i a(1, -1, 1);
    Vector3i b(4, 3, 7);

    CORRADE_COMPARE(Math::cross(a, b), Vector3i(-10, -3, 7));
}

void Vector3Test::axes() {
    constexpr Vector3 x = Vector3::xAxis(5.0f);
    constexpr Vector3 y = Vector3::yAxis(6.0f);
    constexpr Vector3 z = Vector3::zAxis(7.0f);
    CORRADE_COMPARE(x, Vector3(5.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(y, Vector3(0.0f, 6.0f, 0.0f));
    CORRADE_COMPARE(z, Vector3(0.0f, 0.0f, 7.0f));
}

void Vector3Test::scales() {
    constexpr Vector3 x = Vector3::xScale(-5.0f);
    constexpr Vector3 y = Vector3::yScale(-0.2f);
    constexpr Vector3 z = Vector3::zScale(71.0f);
    CORRADE_COMPARE(x, Vector3(-5.0f, 1.0f, 1.0f));
    CORRADE_COMPARE(y, Vector3(1.0f, -0.2f, 1.0f));
    CORRADE_COMPARE(z, Vector3(1.0f, 1.0f, 71.0f));
}

void Vector3Test::twoComponent() {
    Vector3 a(1.0f, 2.0f, 3.0f);
    CORRADE_COMPARE(a.xy(), Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(a.rg(), Vector2(1.0f, 2.0f));

    constexpr Vector3 b(1.0f, 2.0f, 3.0f);
    constexpr Vector2 c1 = b.xy();
    constexpr Vector2 c2 = b.rg();
    constexpr Float d1 = b.xy().y();
    constexpr Float d2 = b.rg().g();
    CORRADE_COMPARE(c1, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(c2, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d1, 2.0f);
    CORRADE_COMPARE(d2, 2.0f);
}

void Vector3Test::multiplyDivideIntegral() {
    const Vector3i vector{32, 10, -6};
    const Vector3i multiplied{-48, -15, 9};

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    CORRADE_COMPARE(-1.5f*vector, multiplied);

    constexpr Vector3i cvector{32, 10, -6};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector3i ca1 = cvector*-1.5f;
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector3i ca2 = -1.5f*cvector;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);
}

void Vector3Test::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Vector3 v3a{1.0f, 2.0f, 3.0f};
    const Vector3 v3b{2.0f, 3.0f, 4.0f};
    const Vector3 v3c{1.0f, 2.0f, 4.0f};

    CORRADE_VERIFY( o(v3a, v3b));
    CORRADE_VERIFY(!o(v3b, v3a));
    CORRADE_VERIFY( o(v3a, v3c));
    CORRADE_VERIFY(!o(v3c, v3a));
    CORRADE_VERIFY( o(v3c, v3b));
    CORRADE_VERIFY(!o(v3b, v3c));

    CORRADE_VERIFY(!o(v3a, v3a));
}

void Vector3Test::swizzleType() {
    constexpr Vector<4, Int> orig;
    constexpr auto b = gather<'y', 'z', 'a'>(orig);
    CORRADE_VERIFY(std::is_same<decltype(b), const Vector3i>::value);
}

void Vector3Test::debug() {
    Containers::String out;
    Debug{&out} << Vector3(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 15, 1)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector3Test)
