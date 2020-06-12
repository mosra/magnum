/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/DebugStl.h>

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

struct Vector4Test: Corrade::TestSuite::Tester {
    explicit Vector4Test();

    void construct();
    void constructPad();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
    void constructConversion();
    void constructCopy();
    void convert();

    void access();
    void threeComponent();
    void twoComponent();

    void planeEquationThreePoints();
    void planeEquationNormalPoint();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

typedef Math::Vector4<Float> Vector4;
typedef Math::Vector4<Int> Vector4i;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector2<Float> Vector2;

Vector4Test::Vector4Test() {
    addTests({&Vector4Test::construct,
              &Vector4Test::constructPad,
              &Vector4Test::constructDefault,
              &Vector4Test::constructNoInit,
              &Vector4Test::constructOneValue,
              &Vector4Test::constructParts,
              &Vector4Test::constructConversion,
              &Vector4Test::constructCopy,
              &Vector4Test::convert,

              &Vector4Test::access,
              &Vector4Test::threeComponent,
              &Vector4Test::twoComponent,

              &Vector4Test::planeEquationThreePoints,
              &Vector4Test::planeEquationNormalPoint,

              &Vector4Test::strictWeakOrdering,

              &Vector4Test::swizzleType,
              &Vector4Test::debug});
}

void Vector4Test::construct() {
    constexpr Vector4 a = {1.0f, -2.5f, 3.0f, 4.1f};
    CORRADE_COMPARE(a, (Vector<4, Float>(1.0f, -2.5f, 3.0f, 4.1f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, Float, Float, Float, Float>::value));
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
    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Vector4>::value));
}

void Vector4Test::constructNoInit() {
    Vector4 a{1.0f, -2.5f, 3.0f, 4.1f};
    new(&a) Vector4{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector4{1.0f, -2.5f, 3.0f, 4.1f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Vector4>::value));
}

void Vector4Test::constructOneValue() {
    constexpr Vector4 a(4.3f);
    CORRADE_COMPARE(a, Vector4(4.3f, 4.3f, 4.3f, 4.3f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector4>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, Float>::value));
}

void Vector4Test::constructParts() {
    constexpr Vector3 a(1.0f, 2.0f, 3.0f);
    constexpr Vector4 b = {a, 4.0f};
    CORRADE_COMPARE(b, Vector4(1.0f, 2.0f, 3.0f, 4.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, Vector3, Float>::value));
}

void Vector4Test::constructConversion() {
    constexpr Vector4 a(1.0f, -2.5f, 3.0f, 4.1f);
    constexpr Vector4i b(a);
    CORRADE_COMPARE(b, Vector4i(1, -2, 3, 4));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector4, Vector4i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector4, Vector4i>::value));
}

void Vector4Test::constructCopy() {
    constexpr Vector<4, Float> a(1.0f, -2.5f, 3.0f, 4.1f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector4 b(a);
    CORRADE_COMPARE(b, Vector4(1.0f, -2.5f, 3.0f, 4.1f));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector4>::value);
}

void Vector4Test::convert() {
    constexpr Vec4 a{1.5f, 2.0f, -3.5f, -0.5f};
    constexpr Vector4 b(1.5f, 2.0f, -3.5f, -0.5f);

    constexpr Vector4 c(a);
    CORRADE_COMPARE(c, b);

    constexpr Vec4 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);
    CORRADE_COMPARE(d.w, a.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec4, Vector4>::value));
    CORRADE_VERIFY(!(std::is_convertible<Vector4, Vec4>::value));
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
    constexpr Vector3 c = b.xyz();
    constexpr Float d = b.xyz().y();
    CORRADE_COMPARE(c, Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(d, 2.0f);
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
    CORRADE_VERIFY((std::is_same<decltype(c), const Vector4i>::value));
}

void Vector4Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector4Test)
