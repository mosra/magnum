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

#include "Math/Vector.h"

struct Vec3 {
    float x, y, z;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<3, float, Vec3> {
    inline constexpr static Vector<3, Float> from(const Vec3& other) {
        return {other.x, other.y, other.z};
    }

    inline constexpr static Vec3 to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

}

namespace Test {

class VectorTest: public Corrade::TestSuite::Tester {
    public:
        VectorTest();

        void construct();
        void constructFromData();
        void constructDefault();
        void constructOneValue();
        void constructOneComponent();
        void constructConversion();
        void constructCopy();

        void isNormalized();

        void convert();
        void data();

        void negative();
        void addSubtract();
        void multiplyDivide();
        void multiplyDivideComponentWise();

        void compare();
        void compareComponentWise();

        void dot();
        void dotSelf();
        void length();
        void normalized();

        void sum();
        void product();
        void min();
        void minAbs();
        void max();
        void maxAbs();

        void projected();
        void projectedOntoNormalized();
        void angle();

        void debug();
        void configuration();
};

typedef Math::Rad<Float> Rad;
typedef Vector<3, Float> Vector3;
typedef Vector<4, Float> Vector4;
typedef Vector<4, Int> Vector4i;

VectorTest::VectorTest() {
    addTests({&VectorTest::construct,
              &VectorTest::constructFromData,
              &VectorTest::constructDefault,
              &VectorTest::constructOneValue,
              &VectorTest::constructOneComponent,
              &VectorTest::constructConversion,
              &VectorTest::constructCopy,

              &VectorTest::isNormalized,

              &VectorTest::convert,
              &VectorTest::data,

              &VectorTest::negative,
              &VectorTest::addSubtract,
              &VectorTest::multiplyDivide,
              &VectorTest::multiplyDivideComponentWise,

              &VectorTest::compare,
              &VectorTest::compareComponentWise,

              &VectorTest::dot,
              &VectorTest::dotSelf,
              &VectorTest::length,
              &VectorTest::normalized,

              &VectorTest::sum,
              &VectorTest::product,
              &VectorTest::min,
              &VectorTest::minAbs,
              &VectorTest::max,
              &VectorTest::maxAbs,

              &VectorTest::projected,
              &VectorTest::projectedOntoNormalized,
              &VectorTest::angle,

              &VectorTest::debug,
              &VectorTest::configuration});
}

void VectorTest::construct() {
    constexpr Vector4 a(1.0f, 2.0f, -3.0f, 4.5f);
    CORRADE_COMPARE(a, Vector4(1.0f, 2.0f, -3.0f, 4.5f));
}

void VectorTest::constructDefault() {
    constexpr Vector4 a;
    CORRADE_COMPARE(a, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void VectorTest::constructFromData() {
    Float data[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    CORRADE_COMPARE(Vector4::from(data), Vector4(1.0f, 2.0f, 3.0f, 4.0f));
}

void VectorTest::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector4 a(7.25f);
    #else
    Vector4 a(7.25f); /* Not constexpr under GCC < 4.7 */
    #endif

    CORRADE_COMPARE(a, Vector4(7.25f, 7.25f, 7.25f, 7.25f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector4>::value));
}

void VectorTest::constructOneComponent() {
    typedef Vector<1, Float> Vector1;

    /* Implicit constructor must work */
    constexpr Vector1 vec = 1.0f;
    CORRADE_COMPARE(vec, Vector1(1));
}

void VectorTest::constructConversion() {
    constexpr Vector4 a(1.3f, 2.7f, -15.0f, 7.0f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Vector4i b(a);
    #else
    Vector4i b(a); /* Not constexpr under GCC < 4.7 */
    #endif

    CORRADE_COMPARE(b, Vector4i(1, 2, -15, 7));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector4, Vector4i>::value));
}

void VectorTest::constructCopy() {
    constexpr Vector4 a(1.0f, 3.5f, 4.0f, -2.7f);
    constexpr Vector4 b(a);
    CORRADE_COMPARE(b, Vector4(1.0f, 3.5f, 4.0f, -2.7f));
}

void VectorTest::isNormalized() {
    CORRADE_VERIFY(!Vector3(1.0f, 2.0f, -1.0f).isNormalized());
    CORRADE_VERIFY(Vector3(0.0f, 1.0f, 0.0f).isNormalized());
}

void VectorTest::convert() {
    constexpr Vec3 a{1.5f, 2.0f, -3.5f};
    constexpr Vector3 b(1.5f, 2.0f, -3.5f);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vector3 c(b);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vec3 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);
}

void VectorTest::data() {
    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);
    vector[2] = 1.0f;
    vector[3] = 1.5f;

    CORRADE_COMPARE(vector[2], 1.0f);
    CORRADE_COMPARE(vector[3], 1.5f);
    CORRADE_COMPARE(vector, Vector4(4.0f, 5.0f, 1.0f, 1.5f));

    /* Pointer chasings, i.e. *(b.data()[3]), are not possible */
    constexpr Vector4 a(1.0f, 2.0f, -3.0f, 4.5f);
    constexpr Float f = a[3];
    constexpr Float g = *a.data();
    CORRADE_COMPARE(f, 4.5f);
    CORRADE_COMPARE(g, 1.0f);
}

void VectorTest::compare() {
    CORRADE_VERIFY(Vector4(1.0f, -3.5f, 5.0f, -10.0f) == Vector4(1.0f + TypeTraits<Float>::epsilon()/2, -3.5f, 5.0f, -10.0f));
    CORRADE_VERIFY(Vector4(1.0f, -1.0f, 5.0f, -10.0f) != Vector4(1.0f, -1.0f + TypeTraits<Float>::epsilon()*2, 5.0f, -10.0f));

    CORRADE_VERIFY(Vector4i(1, -3, 5, -10) == Vector4i(1, -3, 5, -10));
    CORRADE_VERIFY(Vector4i(1, -3, 5, -10) != Vector4i(1, -2, 5, -10));
}

void VectorTest::compareComponentWise() {
    typedef BoolVector<3> BoolVector3;
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) < Vector3(1.1f, -1.0f, 3.0f), BoolVector3(0x1));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) <= Vector3(1.1f, -1.0f, 3.0f), BoolVector3(0x3));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) >= Vector3(1.1f, -1.0f, 3.0f), BoolVector3(0x6));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) > Vector3(1.1f, -1.0f, 3.0f), BoolVector3(0x4));
}

void VectorTest::negative() {
    CORRADE_COMPARE(-Vector4(1.0f, -3.0f, 5.0f, -10.0f), Vector4(-1.0f, 3.0f, -5.0f, 10.0f));
}

void VectorTest::addSubtract() {
    Vector4 a(1.0f, -3.0f, 5.0f, -10.0f);
    Vector4 b(7.5f, 33.0f, -15.0f, 0.0f);
    Vector4 c(8.5f, 30.0f, -10.0f, -10.0f);

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void VectorTest::multiplyDivide() {
    Vector4 vector(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 multiplied(-1.5f, -3.0f, -4.5f, -6.0f);

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*vector, multiplied);
    CORRADE_COMPARE(multiplied/-1.5f, vector);

    Math::Vector<1, Byte> vectorChar(32);
    Math::Vector<1, Byte> multipliedChar(-48);
    CORRADE_COMPARE(vectorChar*-1.5f, multipliedChar);
    CORRADE_COMPARE(multipliedChar/-1.5f, vectorChar);
    CORRADE_COMPARE(-1.5f*vectorChar, multipliedChar);

    /* Divide vector with number and inverse */
    Vector4 divisor(1.0f, 2.0f, -4.0f, 8.0f);
    Vector4 result(1.0f, 0.5f, -0.25f, 0.125f);
    CORRADE_COMPARE(1.0f/divisor, result);
    CORRADE_COMPARE(-1550.0f/multipliedChar, vectorChar);
}

void VectorTest::multiplyDivideComponentWise() {
    Vector4 vec(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 multiplier(7.0f, -4.0f, -1.5f, 1.0f);
    Vector4 multiplied(7.0f, -8.0f, -4.5f, 4.0f);

    CORRADE_COMPARE(vec*multiplier, multiplied);
    CORRADE_COMPARE(multiplied/multiplier, vec);
}

void VectorTest::dot() {
    CORRADE_COMPARE(Vector4::dot({1.0f, 0.5f, 0.75f, 1.5f}, {2.0f, 4.0f, 1.0f, 7.0f}), 15.25f);
}

void VectorTest::dotSelf() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).dot(), 30.0f);
}

void VectorTest::length() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).length(), 5.4772256f);
}

void VectorTest::normalized() {
    CORRADE_COMPARE(Vector4(1.0f, 1.0f, 1.0f, 1.0f).normalized(), Vector4(0.5f, 0.5f, 0.5f, 0.5f));
}

void VectorTest::sum() {
    CORRADE_COMPARE(Vector3(1.0f, 2.0f, 4.0f).sum(), 7.0f);
}

void VectorTest::product() {
    CORRADE_COMPARE(Vector3(1.0f, 2.0f, 3.0f).product(), 6.0f);
}

void VectorTest::min() {
    /* Check also that initial value isn't initialized to 0 */
    CORRADE_COMPARE(Vector3(1.0f, -2.0f, 3.0f).min(), -2.0f);
}

void VectorTest::minAbs() {
    /* Check that initial value is absolute and also all others */
    CORRADE_COMPARE(Vector3(-2.0f, 1.0f, 3.0f).minAbs(), 1.0f);
    CORRADE_COMPARE(Vector3(1.0f, -2.0f, 3.0f).minAbs(), 1.0f);
}

void VectorTest::max() {
    /* Check also that initial value isn't initialized to 0 */
    CORRADE_COMPARE(Vector3(-1.0f, -2.0f, -3.0f).max(), -1.0f);
}

void VectorTest::maxAbs() {
    /* Check that initial value is absolute and also all others */
    CORRADE_COMPARE(Vector3(-5.0f, 1.0f, 3.0f).maxAbs(), 5.0f);
    CORRADE_COMPARE(Vector3(1.0f, -5.0f, 3.0f).maxAbs(), 5.0f);
}

void VectorTest::projected() {
    Vector3 line(1.0f, -1.0f, 0.5f);
    Vector3 projected = Vector3(1.0f, 2.0f, 3.0f).projected(line);

    CORRADE_COMPARE(projected, Vector3(0.222222f, -0.222222f, 0.111111f));
    CORRADE_COMPARE(projected.normalized(), line.normalized());
}

void VectorTest::projectedOntoNormalized() {
    std::ostringstream o;
    Error::setOutput(&o);

    Vector3 vector(1.0f, 2.0f, 3.0f);
    Vector3 line(1.0f, -1.0f, 0.5f);
    Vector3 projected = vector.projectedOntoNormalized(line);
    CORRADE_VERIFY(projected != projected);
    CORRADE_COMPARE(o.str(), "Math::Vector::projectedOntoNormalized(): line must be normalized\n");

    projected = vector.projectedOntoNormalized(line.normalized());
    CORRADE_COMPARE(projected, Vector3(0.222222f, -0.222222f, 0.111111f));
    CORRADE_COMPARE(projected.normalized(), line.normalized());
    CORRADE_COMPARE(projected, vector.projected(line));
}

void VectorTest::angle() {
    std::ostringstream o;
    Error::setOutput(&o);
    auto angle = Vector3::angle(Vector3(2.0f, 3.0f, 4.0f).normalized(), {1.0f, -2.0f, 3.0f});
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Vector::angle(): vectors must be normalized\n");

    o.str({});
    angle = Vector3::angle({2.0f, 3.0f, 4.0f}, Vector3(1.0f, -2.0f, 3.0f).normalized());
    CORRADE_VERIFY(angle != angle);
    CORRADE_COMPARE(o.str(), "Math::Vector::angle(): vectors must be normalized\n");

    CORRADE_COMPARE(Vector3::angle(Vector3(2.0f,  3.0f, 4.0f).normalized(),
                                   Vector3(1.0f, -2.0f, 3.0f).normalized()),
                    Rad(1.162514f));
}

void VectorTest::debug() {
    std::ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");

    o.str({});
    Debug(&o) << "a" << Vector4() << "b" << Vector4();
    CORRADE_COMPARE(o.str(), "a Vector(0, 0, 0, 0) b Vector(0, 0, 0, 0)\n");
}

void VectorTest::configuration() {
    Corrade::Utility::Configuration c;

    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    std::string value("3 3.125 9 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector4>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::VectorTest)
