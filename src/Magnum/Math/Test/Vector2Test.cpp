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

struct Vector2Test: Corrade::TestSuite::Tester {
    explicit Vector2Test();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructConversion();
    void constructCopy();
    void convert();

    void access();
    void cross();
    void axes();
    void scales();
    void perpendicular();
    void aspectRatio();

    void strictWeakOrdering();

    void swizzleType();
    void debug();
};

typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<Int> Vector2i;

Vector2Test::Vector2Test() {
    addTests({&Vector2Test::construct,
              &Vector2Test::constructDefault,
              &Vector2Test::constructNoInit,
              &Vector2Test::constructOneValue,
              &Vector2Test::constructConversion,
              &Vector2Test::constructCopy,
              &Vector2Test::convert,

              &Vector2Test::access,
              &Vector2Test::cross,
              &Vector2Test::axes,
              &Vector2Test::scales,
              &Vector2Test::perpendicular,
              &Vector2Test::aspectRatio,

              &Vector2Test::strictWeakOrdering,

              &Vector2Test::swizzleType,
              &Vector2Test::debug});
}

void Vector2Test::construct() {
    constexpr Vector2 a = {1.5f, 2.5f};
    CORRADE_COMPARE(a, (Vector<2, Float>(1.5f, 2.5f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector2, Float, Float>::value));
}

void Vector2Test::constructDefault() {
    constexpr Vector2 a;
    constexpr Vector2 b{ZeroInit};
    CORRADE_COMPARE(a, Vector2(0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector2(0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector2>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Vector2, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Vector2>::value));
}

void Vector2Test::constructNoInit() {
    Vector2 a{1.5f, 2.5f};
    new(&a) Vector2{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector2{1.5f, 2.5f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector2, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Vector2>::value));
}

void Vector2Test::constructOneValue() {
    constexpr Vector2 a(3.0f);
    CORRADE_COMPARE(a, Vector2(3.0f, 3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector2>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector2, Float>::value));
}

void Vector2Test::constructConversion() {
    constexpr Vector2 a(1.5f, 2.5f);
    constexpr Vector2i b(a);
    CORRADE_COMPARE(b, Vector2i(1, 2));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector2, Vector2i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Vector2, Vector2i>::value));
}

void Vector2Test::constructCopy() {
    constexpr Vector<2, Float> a(1.5f, 2.5f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Vector2 b(a);
    CORRADE_COMPARE(b, Vector2(1.5f, 2.5f));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector2>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector2>::value);
}

void Vector2Test::convert() {
    constexpr Vec2 a{1.5f, 2.0f};
    constexpr Vector2 b(1.5f, 2.0f);

    constexpr Vector2 c(a);
    CORRADE_COMPARE(c, b);

    constexpr Vec2 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec2, Vector2>::value));
    CORRADE_VERIFY(!(std::is_convertible<Vector2, Vec2>::value));
}

void Vector2Test::access() {
    Vector2 vec(1.0f, -2.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);

    constexpr Vector2 cvec(1.0f, -2.0f);
    constexpr Float x = cvec.x();
    constexpr Float y = cvec.y();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
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
    CORRADE_VERIFY((std::is_same<decltype(a), const Vector2i>::value));
}

void Vector2Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector2(0.5f, 15.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector2Test)
