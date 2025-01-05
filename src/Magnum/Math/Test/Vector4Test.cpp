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

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"

struct Vec4 {
    float x, y, z, w;
};

namespace Magnum { namespace Math {

namespace Implementation {

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

struct Vector4Test: TestSuite::Tester {
    explicit Vector4Test();

    void construct();
    void constructPad();
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
    void threeComponent();
    void twoComponent();

    void multiplyDivideIntegral();

    void planeEquationThreePoints();
    void planeEquationNormalPoint();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

using Magnum::Vector4;
using Magnum::Vector3;
using Magnum::Vector2;

Vector4Test::Vector4Test() {
    addTests({&Vector4Test::construct,
              &Vector4Test::constructPad,
              &Vector4Test::constructDefault,
              &Vector4Test::constructNoInit,
              &Vector4Test::constructOneValue,
              &Vector4Test::constructParts,
              &Vector4Test::constructArray,
              &Vector4Test::constructArrayRvalue,
              &Vector4Test::constructConversion,
              &Vector4Test::constructBit,
              &Vector4Test::constructCopy,
              &Vector4Test::convert,

              &Vector4Test::access,
              &Vector4Test::threeComponent,
              &Vector4Test::twoComponent,

              &Vector4Test::multiplyDivideIntegral,

              &Vector4Test::planeEquationThreePoints,
              &Vector4Test::planeEquationNormalPoint,

              &Vector4Test::strictWeakOrdering,

              &Vector4Test::swizzleType,
              &Vector4Test::debug});
}

void Vector4Test::construct() {
    constexpr Vector4 a = {1.0f, -2.5f, 3.0f, 4.1f};
    CORRADE_COMPARE(a, (Vector<4, Float>(1.0f, -2.5f, 3.0f, 4.1f)));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Float, Float, Float, Float>::value);
}

void Vector4Test::constructPad() {
    constexpr Vector<2, Float> a{3.0f, -1.0f};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector4 b = Vector4::pad(a);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector4 c = Vector4::pad(a, 5.0f);
    constexpr Vector4 d = Vector4::pad(a, 5.0f, 1.0f);
    CORRADE_COMPARE(b, Vector4(3.0f, -1.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(c, Vector4(3.0f, -1.0f, 5.0f, 5.0f));
    CORRADE_COMPARE(d, Vector4(3.0f, -1.0f, 5.0f, 1.0f));
}

void Vector4Test::constructDefault() {
    constexpr Vector4 a;
    constexpr Vector4 b{ZeroInit};
    CORRADE_COMPARE(a, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Vector4>::value);
}

void Vector4Test::constructNoInit() {
    Vector4 a{1.0f, -2.5f, 3.0f, 4.1f};
    new(&a) Vector4{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector4{1.0f, -2.5f, 3.0f, 4.1f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Vector4>::value);
}

void Vector4Test::constructOneValue() {
    constexpr Vector4 a(4.3f);
    CORRADE_COMPARE(a, Vector4(4.3f, 4.3f, 4.3f, 4.3f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Vector4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Float>::value);
}

void Vector4Test::constructParts() {
    constexpr Vector3 a(1.0f, 2.0f, 3.0f);
    constexpr Vector4 b = {a, 4.0f};
    CORRADE_COMPARE(b, Vector4(1.0f, 2.0f, 3.0f, 4.0f));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Vector3, Float>::value);
}

void Vector4Test::constructArray() {
    float data[]{1.3f, 2.7f, -15.0f, 7.0f};
    Vector4 a{data};
    CORRADE_COMPARE(a, (Vector4{1.3f, 2.7f, -15.0f, 7.0f}));

    constexpr float cdata[]{1.3f, 2.7f, -15.0f, 7.0f};
    constexpr Vector4 ca{cdata};
    CORRADE_COMPARE(ca, (Vector4{1.3f, 2.7f, -15.0f, 7.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[4], Vector4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, float[4]>::value);

    /* See VectorTest::constructArray() for details */
    #if 0
    float data1[]{1.3f};
    float data5[]{1.3f, 2.7f, -15.0f, 7.0f, 22.6f};
    Vector4 b{data1};
    Vector4 c{data5};
    #endif
}

void Vector4Test::constructArrayRvalue() {
    /* Silly but why not. Could theoretically help with some fancier types
       that'd otherwise require explicit typing with the variadic
       constructor. */
    Vector4 a{{1.3f, 2.7f, -15.0f, 7.0f}};
    CORRADE_COMPARE(a, (Vector4{1.3f, 2.7f, -15.0f, 7.0f}));

    constexpr Vector4 ca{{1.3f, 2.7f, -15.0f, 7.0f}};
    CORRADE_COMPARE(ca, (Vector4{1.3f, 2.7f, -15.0f, 7.0f}));

    /* See VectorTest::constructArrayRvalue() for details */
    #if 0
    Vector4 c{{1.3f, 2.7f, -15.0f, 7.0f, 22.6f}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a Vector from a smaller array isn't an error on GCC 4.8.");
    Vector4 b{{1.3f, 2.7f}};
    #endif
}

void Vector4Test::constructConversion() {
    constexpr Vector4 a(1.0f, -2.5f, 3.0f, 4.1f);
    constexpr Vector4i b(a);
    CORRADE_COMPARE(b, Vector4i(1, -2, 3, 4));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vector4, Vector4i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Vector4i>::value);
}

void Vector4Test::constructBit() {
    BitVector4 a{'\xa'}; /* 0b1010 */
    CORRADE_COMPARE(Vector4{a}, (Vector4{0.0f, 1.0f, 0.0f, 1.0f}));

    constexpr BitVector4 ca{'\xa'}; /* 0b1010 */
    constexpr Vector4 cb{ca};
    CORRADE_COMPARE(cb, (Vector4{0.0f, 1.0f, 0.0f, 1.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector4, Vector4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, BitVector4>::value);
}

void Vector4Test::constructCopy() {
    constexpr Vector<4, Float> a(1.0f, -2.5f, 3.0f, 4.1f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector4 b(a);
    CORRADE_COMPARE(b, Vector4(1.0f, -2.5f, 3.0f, 4.1f));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Vector4>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector4>::value);
}

void Vector4Test::convert() {
    constexpr Vec4 a{1.5f, 2.0f, -3.5f, -0.5f};
    constexpr Vector4 b(1.5f, 2.0f, -3.5f, -0.5f);

    constexpr Vector4 c(a);
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Vec4(b);
    #else
    constexpr Vec4 d(b);
    #endif
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);
    CORRADE_COMPARE(d.w, a.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vec4, Vector4>::value);
    CORRADE_VERIFY(!std::is_convertible<Vector4, Vec4>::value);
}

void Vector4Test::access() {
    Vector4 vec(1.0f, -2.0f, 5.0f, 0.5f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.r(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.g(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);
    CORRADE_COMPARE(vec.b(), 5.0f);
    CORRADE_COMPARE(vec.w(), 0.5f);
    CORRADE_COMPARE(vec.a(), 0.5f);

    constexpr Vector4 cvec(1.0f, -2.0f, 5.0f, 0.5f);
    constexpr Float x = cvec.x();
    constexpr Float r = cvec.r();
    constexpr Float y = cvec.y();
    constexpr Float g = cvec.g();
    constexpr Float z = cvec.z();
    constexpr Float b = cvec.b();
    constexpr Float w = cvec.w();
    constexpr Float a = cvec.a();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(r, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(g, -2.0f);
    CORRADE_COMPARE(z, 5.0f);
    CORRADE_COMPARE(b, 5.0f);
    CORRADE_COMPARE(w, 0.5f);
    CORRADE_COMPARE(a, 0.5f);
}

void Vector4Test::threeComponent() {
    Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    CORRADE_COMPARE(a.xyz(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a.rgb(), Vector3(1.0f, 2.0f, 3.0f));

    constexpr Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector3 c1 = b.xyz();
    constexpr Vector3 c2 = b.rgb();
    constexpr Float d1 = b.xyz().y();
    constexpr Float d2 = b.rgb().g();
    CORRADE_COMPARE(c1, Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(c2, Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(d1, 2.0f);
    CORRADE_COMPARE(d2, 2.0f);
}

void Vector4Test::twoComponent() {
    Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    CORRADE_COMPARE(a.xy(), Vector2(1.0f, 2.0f));

    constexpr Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector2 c = b.xy();
    constexpr Float d = b.xy().x();
    CORRADE_COMPARE(c, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d, 1.0f);
}

void Vector4Test::multiplyDivideIntegral() {
    const Vector4i vector{32, 10, -6, 2};
    const Vector4i multiplied{-48, -15, 9, -3};

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    CORRADE_COMPARE(-1.5f*vector, multiplied);

    constexpr Vector4i cvector{32, 10, -6, 2};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector4i ca1 = cvector*-1.5f;
    /* On MSVC 2015 this picks an int*Vector2i overload, leading to a wrong
       result, unless MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() is used */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
    constexpr
    #endif
    Vector4i ca2 = -1.5f*cvector;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);
}

void Vector4Test::planeEquationThreePoints() {
    const Vector3 a{1.0f, 0.5f, 3.0f};
    const Vector3 b{1.5f, 1.5f, 2.5f};
    const Vector3 c{2.0f, 1.5f, 1.0f};
    const Vector4 eq = Math::planeEquation(a, b, c);

    CORRADE_COMPARE(Math::dot(a, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(Math::dot(b, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(Math::dot(c, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(eq, (Vector4{-0.9045340f, 0.3015113f, -0.3015113f, 1.658312f}));

    /* Different winding order should only give a negated normal */
    CORRADE_COMPARE(Math::planeEquation(a, c, b), -eq);
}

void Vector4Test::planeEquationNormalPoint() {
    const Vector3 a{1.0f, 0.5f, 3.0f};
    const Vector3 normal{-0.9045340f, 0.3015113f, -0.3015113f};
    const Vector4 eq = Math::planeEquation(normal, a);

    const Vector3 b{1.5f, 1.5f, 2.5f};
    const Vector3 c{2.0f, 1.5f, 1.0f};
    CORRADE_COMPARE(Math::dot(a, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(Math::dot(b, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(Math::dot(c, eq.xyz()) + eq.w(), 0.0f);
    CORRADE_COMPARE(eq, (Vector4{-0.9045340f, 0.3015113f, -0.3015113f, 1.658312f}));
}

void Vector4Test::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Vector4 v4a{1.0f, 2.0f, 3.0f, 4.0f};
    const Vector4 v4b{2.0f, 3.0f, 4.0f, 5.0f};
    const Vector4 v4c{1.0f, 2.0f, 3.0f, 5.0f};

    CORRADE_VERIFY( o(v4a, v4b));
    CORRADE_VERIFY(!o(v4b, v4a));
    CORRADE_VERIFY( o(v4a, v4c));
    CORRADE_VERIFY(!o(v4c, v4a));
    CORRADE_VERIFY( o(v4c, v4b));
    CORRADE_VERIFY(!o(v4b, v4c));

    CORRADE_VERIFY(!o(v4a, v4a));
}

void Vector4Test::swizzleType() {
    constexpr Vector4i orig;
    constexpr auto c = gather<'y', 'a', 'y', 'x'>(orig);
    CORRADE_VERIFY(std::is_same<decltype(c), const Vector4i>::value);
}

void Vector4Test::debug() {
    Containers::String out;
    Debug{&out} << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 15, 1, 1)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector4Test)
