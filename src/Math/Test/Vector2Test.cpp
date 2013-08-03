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

#include "Math/Vector3.h"

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

namespace Test {

class Vector2Test: public Corrade::TestSuite::Tester {
    public:
        Vector2Test();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructConversion();
        void constructCopy();

        void convert();

        void access();
        void cross();
        void axes();
        void scales();
        void perpendicular();

        void debug();
        void configuration();
};

typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector2<Int> Vector2i;

Vector2Test::Vector2Test() {
    addTests({&Vector2Test::construct,
              &Vector2Test::constructDefault,
              &Vector2Test::constructOneValue,
              &Vector2Test::constructConversion,
              &Vector2Test::constructCopy,

              &Vector2Test::convert,

              &Vector2Test::access,
              &Vector2Test::cross,
              &Vector2Test::axes,
              &Vector2Test::scales,
              &Vector2Test::perpendicular,

              &Vector2Test::debug,
              &Vector2Test::configuration});
}

void Vector2Test::construct() {
    constexpr Vector2 a = {1.5f, 2.5f};
    CORRADE_COMPARE(a, (Vector<2, Float>(1.5f, 2.5f)));
}

void Vector2Test::constructDefault() {
    constexpr Vector2 a;
    CORRADE_COMPARE(a, Vector2(0.0f, 0.0f));
}

void Vector2Test::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector2 a(3.0f);
    #else
    Vector2 a(3.0f); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(a, Vector2(3.0f, 3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector2>::value));
}

void Vector2Test::constructConversion() {
    constexpr Vector2 a(1.5f, 2.5f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector2i b(a);
    #else
    Vector2i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Vector2i(1, 2));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector2, Vector2i>::value));
}

void Vector2Test::constructCopy() {
    constexpr Vector<2, Float> a(1.5f, 2.5f);
    constexpr Vector2 b(a);
    CORRADE_COMPARE(b, Vector2(1.5f, 2.5f));
}

void Vector2Test::convert() {
    constexpr Vec2 a{1.5f, 2.0f};
    constexpr Vector2 b(1.5f, 2.0f);

    constexpr Vector2 c(a);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vec2 d(b);
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

    CORRADE_COMPARE(Vector2i::cross(a, b), 7);
    CORRADE_COMPARE(Vector3i::cross({a, 0}, {b, 0}), Vector3i(0, 0, Vector2i::cross(a, b)));
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
    CORRADE_COMPARE(Vector2::dot(a.perpendicular(), a), 0.0f);
    CORRADE_COMPARE(Vector2::xAxis().perpendicular(), Vector2::yAxis());
}

void Vector2Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector2(0.5f, 15.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15)\n");
}

void Vector2Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector2 vec(3.125f, 9.0f);
    std::string value("3.125 9");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector2>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector2Test)
