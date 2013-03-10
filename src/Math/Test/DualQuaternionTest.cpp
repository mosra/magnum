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

#include "Math/DualQuaternion.h"

namespace Magnum { namespace Math { namespace Test {

class DualQuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualQuaternionTest();

        void construct();
        void constructDefault();
        void constructFromVector();

        void constExpressions();

        void lengthSquared();
        void length();
        void normalized();

        void quaternionConjugated();
        void dualConjugated();
        void conjugated();
        void inverted();
        void invertedNormalized();

        void rotation();
        void translation();
        void combinedTransformParts();
        void matrix();
        void transformPoint();
        void transformPointNormalized();

        void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Dual<Float> Dual;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::DualQuaternion<Float> DualQuaternion;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::Vector3<Float> Vector3;

DualQuaternionTest::DualQuaternionTest() {
    addTests({&DualQuaternionTest::construct,
              &DualQuaternionTest::constructDefault,
              &DualQuaternionTest::constructFromVector,

              &DualQuaternionTest::constExpressions,

              &DualQuaternionTest::lengthSquared,
              &DualQuaternionTest::length,
              &DualQuaternionTest::normalized,

              &DualQuaternionTest::quaternionConjugated,
              &DualQuaternionTest::dualConjugated,
              &DualQuaternionTest::conjugated,
              &DualQuaternionTest::inverted,
              &DualQuaternionTest::invertedNormalized,

              &DualQuaternionTest::rotation,
              &DualQuaternionTest::translation,
              &DualQuaternionTest::combinedTransformParts,
              &DualQuaternionTest::matrix,
              &DualQuaternionTest::transformPoint,
              &DualQuaternionTest::transformPointNormalized,

              &DualQuaternionTest::debug});
}

void DualQuaternionTest::construct() {
    DualQuaternion q({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(q.real(), Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_COMPARE(q.dual(), Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));
}

void DualQuaternionTest::constructDefault() {
    CORRADE_COMPARE(DualQuaternion(), DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
    CORRADE_COMPARE(DualQuaternion().length(), 1.0f);
}

void DualQuaternionTest::constructFromVector() {
    CORRADE_COMPARE(DualQuaternion(Vector3(1.0f, 2.0f, 3.0f)), DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{1.0f, 2.0f, 3.0f}, 0.0f}));
}

void DualQuaternionTest::constExpressions() {
    /* Default constructor */
    constexpr DualQuaternion a;
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));

    /* Value constructor */
    constexpr DualQuaternion b({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f});
    CORRADE_COMPARE(b, DualQuaternion({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f}));

    /* Vector constructor */
    constexpr DualQuaternion c(Vector3(1.5f, -5.0f, 3.0f));
    CORRADE_COMPARE(c, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{1.5f, -5.0f, 3.0f}, 0.0f}));

    /* Copy constructor */
    constexpr DualQuaternion d(b);
    CORRADE_COMPARE(d, DualQuaternion({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f}));
}

void DualQuaternionTest::lengthSquared() {
    DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.0f, 3.0f}, 2.0f});
    CORRADE_COMPARE(a.lengthSquared(), Dual(30.0f, -9.0f));
}

void DualQuaternionTest::length() {
    DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.0f, 3.0f}, 2.0f});
    CORRADE_COMPARE(a.length(), Dual(5.477226f, -0.821584f));
}

void DualQuaternionTest::normalized() {
    DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.0f, 3.0f}, 2.0f});
    DualQuaternion b({{0.182574f, 0.365148f, 0.547723f}, -0.730297f}, {{0.118673f, -0.49295f, 0.629881f}, 0.255604f});
    CORRADE_COMPARE(a.normalized().length(), 1.0f);
    CORRADE_COMPARE(a.normalized(), b);
}

void DualQuaternionTest::quaternionConjugated() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f}, 2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{-0.5f,  3.1f, -3.3f}, 2.0f});

    CORRADE_COMPARE(a.quaternionConjugated(), b);
}

void DualQuaternionTest::dualConjugated() {
    DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f},  2.0f});
    DualQuaternion b({{1.0f, 2.0f, 3.0f}, -4.0f}, {{-0.5f,  3.1f, -3.3f}, -2.0f});

    CORRADE_COMPARE(a.dualConjugated(), b);
}

void DualQuaternionTest::conjugated() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f},  2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f}, -2.0f});

    CORRADE_COMPARE(a.conjugated(), b);
}

void DualQuaternionTest::inverted() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 2.5f, -3.1f,  3.3f}, 2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{-2.5f,  3.1f, -3.3f}, 2.0f});

    CORRADE_COMPARE(a*a.inverted(), DualQuaternion());
    CORRADE_COMPARE(a.inverted(), b/Dual(30.0f, -3.6f));
}

void DualQuaternionTest::invertedNormalized() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 2.5f, -3.1f,  3.3f}, 2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{-2.5f,  3.1f, -3.3f}, 2.0f});

    std::ostringstream o;
    Error::setOutput(&o);
    CORRADE_COMPARE(a.invertedNormalized(), DualQuaternion());
    CORRADE_COMPARE(o.str(), "Math::DualQuaternion::invertedNormalized(): dual quaternion must be normalized\n");

    DualQuaternion normalized = a.normalized();
    DualQuaternion inverted = normalized.invertedNormalized();
    CORRADE_COMPARE(normalized*inverted, DualQuaternion());
    CORRADE_COMPARE(inverted*normalized, DualQuaternion());
    CORRADE_COMPARE(inverted, b/Math::sqrt(Dual(30.0f, -3.6f)));
}

void DualQuaternionTest::rotation() {
    std::ostringstream o;
    Error::setOutput(&o);

    Vector3 axis(1.0f/Constants<Float>::sqrt3());

    CORRADE_COMPARE(DualQuaternion::rotation(Deg(120.0f), axis*2.0f), DualQuaternion());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::rotation(): axis must be normalized\n");

    DualQuaternion q = DualQuaternion::rotation(Deg(120.0f), axis);
    CORRADE_COMPARE(q.length(), 1.0f);
    CORRADE_COMPARE(q, DualQuaternion({Vector3(0.5f, 0.5f, 0.5f), 0.5f}, {{}, 0.0f}));
    CORRADE_COMPARE_AS(q.rotationAngle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q.rotationAxis(), axis);
}

void DualQuaternionTest::translation() {
    Vector3 vec(1.0f, -3.5f, 0.5f);
    DualQuaternion q = DualQuaternion::translation(vec);
    CORRADE_COMPARE(q.length(), 1.0f);
    CORRADE_COMPARE(q, DualQuaternion({}, {{0.5f, -1.75f, 0.25f}, 0.0f}));
    CORRADE_COMPARE(q.translation(), vec);
}

void DualQuaternionTest::combinedTransformParts() {
    Vector3 translation = Vector3(-1.0f, 2.0f, 3.0f);
    DualQuaternion a = DualQuaternion::translation(translation)*DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation(translation);

    CORRADE_COMPARE(a.rotationAxis(), Vector3::xAxis());
    CORRADE_COMPARE(b.rotationAxis(), Vector3::xAxis());
    CORRADE_COMPARE_AS(a.rotationAngle(), Deg(23.0f), Rad);
    CORRADE_COMPARE_AS(b.rotationAngle(), Deg(23.0f), Rad);

    CORRADE_COMPARE(a.translation(), translation);
    CORRADE_COMPARE(b.translation(), Quaternion::rotation(Deg(23.0f), Vector3::xAxis()).transformVector(translation));
}

void DualQuaternionTest::matrix() {
    DualQuaternion q = DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::rotationX(Deg(23.0f))*Matrix4::translation({-1.0f, 2.0f, 3.0f});

    /* Verify that negated dual quaternion gives the same transformation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);
}

void DualQuaternionTest::transformPoint() {
    DualQuaternion a = DualQuaternion::translation({-1.0f, 2.0f, 3.0f})*DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::translation({-1.0f, 2.0f, 3.0f})*Matrix4::rotationX(Deg(23.0f));
    Matrix4 n = Matrix4::rotationX(Deg(23.0f))*Matrix4::translation({-1.0f, 2.0f, 3.0f});
    Vector3 v(0.0f, -3.6f, 0.7f);

    Vector3 transformedA = (a*Dual(2)).transformPoint(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector3(-1.0f, -1.58733f, 2.237721f));

    Vector3 transformedB = (b*Dual(2)).transformPoint(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector3(-1.0f, -2.918512f, 2.780698f));
}

void DualQuaternionTest::transformPointNormalized() {
    DualQuaternion a = DualQuaternion::translation({-1.0f, 2.0f, 3.0f})*DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::translation({-1.0f, 2.0f, 3.0f})*Matrix4::rotationX(Deg(23.0f));
    Matrix4 n = Matrix4::rotationX(Deg(23.0f))*Matrix4::translation({-1.0f, 2.0f, 3.0f});
    Vector3 v(0.0f, -3.6f, 0.7f);

    std::ostringstream o;
    Corrade::Utility::Error::setOutput(&o);
    Vector3 notTransformed = (a*Dual(2)).transformPointNormalized(v);
    CORRADE_VERIFY(notTransformed != notTransformed);
    CORRADE_COMPARE(o.str(), "Math::DualQuaternion::transformPointNormalized(): dual quaternion must be normalized\n");

    Vector3 transformedA = a.transformPointNormalized(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector3(-1.0f, -1.58733f, 2.237721f));

    Vector3 transformedB = b.transformPointNormalized(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector3(-1.0f, -2.918512f, 2.780698f));
}

void DualQuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(o.str(), "DualQuaternion({{1, 2, 3}, -4}, {{0.5, -3.1, 3.3}, 2})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualQuaternionTest)
