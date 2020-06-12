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

struct Vector3Test: Corrade::TestSuite::Tester {
    explicit Vector3Test();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructParts();
    void constructConversion();
    void constructCopy();
    void convert();

    void access();
    void cross();
    void axes();
    void scales();
    void twoComponent();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector2<Float> Vector2;

Vector3Test::Vector3Test() {
    addTests({&Vector3Test::construct,
              &Vector3Test::constructDefault,
              &Vector3Test::constructNoInit,
              &Vector3Test::constructOneValue,
              &Vector3Test::constructParts,
              &Vector3Test::constructConversion,
              &Vector3Test::constructCopy,
              &Vector3Test::convert,

              &Vector3Test::access,
              &Vector3Test::cross,
              &Vector3Test::axes,
              &Vector3Test::scales,
              &Vector3Test::twoComponent,

              &Vector3Test::strictWeakOrdering,

              &Vector3Test::swizzleType,
              &Vector3Test::debug});
}

void Vector3Test::construct() {
    constexpr Vector3 a = {1.0f, 2.5f, -3.0f};
    CORRADE_COMPARE(a, (Vector<3, Float>(1.0f, 2.5f, -3.0f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, Float, Float, Float>::value));
}

void Vector3Test::constructDefault() {
    constexpr Vector3 a;
    constexpr Vector3 b{ZeroInit};
    CORRADE_COMPARE(a, Vector3(0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector3(0.0f, 0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector3>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Vector3>::value));
}

void Vector3Test::constructNoInit() {
    Vector3 a{1.0f, 2.5f, -3.0f};
    new(&a) Vector3{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector3{1.0f, 2.5f, -3.0f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Vector3>::value));
}

void Vector3Test::constructOneValue() {
    constexpr Vector3 a(-3.0f);
    CORRADE_COMPARE(a, Vector3(-3.0f, -3.0f, -3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector3>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, Float>::value));
}

void Vector3Test::constructParts() {
    constexpr Vector2 a(1.0f, 2.0f);
    constexpr Vector3 b = {a, 3.0f};
    CORRADE_COMPARE(b, Vector3(1.0f, 2.0f, 3.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, Vector2, Float>::value));
}

void Vector3Test::constructConversion() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    constexpr Vector3i b(a);
    CORRADE_COMPARE(b, Vector3i(1, 2, -3));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, Vector3i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector3, Vector3i>::value));
}

void Vector3Test::constructCopy() {
    constexpr Vector<3, Float> a(1.0f, 2.5f, -3.0f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector3 b(a);
    CORRADE_COMPARE(b, Vector3(1.0f, 2.5f, -3.0f));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector3>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector3>::value);
}

void Vector3Test::convert() {
    constexpr Vec3 a{1.5f, 2.0f, -3.5f};
    constexpr Vector3 b(1.5f, 2.0f, -3.5f);

    constexpr Vector3 c(a);
    CORRADE_COMPARE(c, b);

    constexpr Vec3 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec3, Vector3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Vector3, Vec3>::value));
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

    constexpr Vector3 b(1.0f, 2.0f, 3.0f);
    constexpr Vector2 c = b.xy();
    constexpr Float d = b.xy().y();
    CORRADE_COMPARE(c, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d, 2.0f);
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
    CORRADE_VERIFY((std::is_same<decltype(b), const Vector3i>::value));
}

void Vector3Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector3(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector3Test)
