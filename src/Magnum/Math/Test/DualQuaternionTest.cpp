/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/Math/DualQuaternion.h"

struct DualQuat {
    struct { float x, y, z, w; } re;
    struct { float x, y, z, w; } du;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct DualQuaternionConverter<Float, DualQuat> {
    constexpr static DualQuaternion<Float> from(const DualQuat& other) {
        return {{{other.re.x, other.re.y, other.re.z}, other.re.w},
                {{other.du.x, other.du.y, other.du.z}, other.du.w}};
    }

    constexpr static DualQuat to(const DualQuaternion<Float>& other) {
        return {{other.real().vector().x(), other.real().vector().y(), other.real().vector().z(), other.real().scalar()},
                {other.dual().vector().x(), other.dual().vector().y(), other.dual().vector().z(), other.dual().scalar()}};
    }
};

}

namespace Test {

struct DualQuaternionTest: Corrade::TestSuite::Tester {
    explicit DualQuaternionTest();

    void construct();
    void constructDefault();
    void constructFromVector();
    void constructCopy();
    void convert();

    void isNormalized();

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
              &DualQuaternionTest::constructCopy,
              &DualQuaternionTest::convert,

              &DualQuaternionTest::isNormalized,

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
    constexpr DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));

    constexpr Quaternion b = a.real();
    CORRADE_COMPARE(b, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));

    constexpr Quaternion c = a.dual();
    CORRADE_COMPARE(c, Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));

    constexpr DualQuaternion d({{1.0f, 2.0f, 3.0f}, -4.0f});
    CORRADE_COMPARE(d, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
}

void DualQuaternionTest::constructDefault() {
    constexpr DualQuaternion a;
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
    CORRADE_COMPARE(a.length(), 1.0f);
}

void DualQuaternionTest::constructFromVector() {
    constexpr DualQuaternion a(Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{1.0f, 2.0f, 3.0f}, 0.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, DualQuaternion>::value));
}

void DualQuaternionTest::constructCopy() {
    constexpr Math::Dual<Quaternion> a({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f});
    constexpr DualQuaternion b(a);
    CORRADE_COMPARE(b, DualQuaternion({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f}));
}

void DualQuaternionTest::convert() {
    constexpr DualQuat a{{1.5f, -3.5f, 7.0f, -0.5f}, {15.0f, 0.25f, -9.5f, 0.8f}};
    constexpr DualQuaternion b{{{1.5f, -3.5f, 7.0f}, -0.5f}, {{15.0f, 0.25f, -9.5f}, 0.8f}};

    constexpr DualQuaternion c{a};
    CORRADE_COMPARE(c, b);

    constexpr DualQuat d(b);
    CORRADE_COMPARE(d.re.x, a.re.x);
    CORRADE_COMPARE(d.re.y, a.re.y);
    CORRADE_COMPARE(d.re.z, a.re.z);
    CORRADE_COMPARE(d.re.w, a.re.w);
    CORRADE_COMPARE(d.du.x, a.du.x);
    CORRADE_COMPARE(d.du.y, a.du.y);
    CORRADE_COMPARE(d.du.z, a.du.z);
    CORRADE_COMPARE(d.du.w, a.du.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<DualQuat, DualQuaternion>::value));
    CORRADE_VERIFY(!(std::is_convertible<DualQuaternion, DualQuat>::value));
}

void DualQuaternionTest::isNormalized() {
    CORRADE_VERIFY(!DualQuaternion({{1.0f, 2.0f, 3.0f}, 4.0f}, {}).isNormalized());
    CORRADE_VERIFY((DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({3.0f, 1.0f, -0.5f})).isNormalized());
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
    CORRADE_COMPARE_AS(q.rotation().angle(), Deg(120.0f), Deg);
    CORRADE_COMPARE(q.rotation().axis(), axis);

    /* Constexpr access to rotation */
    constexpr DualQuaternion b({{-1.0f, 2.0f, 3.0f}, 4.0f}, {});
    constexpr Quaternion c = b.rotation();
    CORRADE_COMPARE(c, Quaternion({-1.0f, 2.0f, 3.0f}, 4.0f));
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

    CORRADE_COMPARE(a.rotation().axis(), Vector3::xAxis());
    CORRADE_COMPARE(b.rotation().axis(), Vector3::xAxis());
    CORRADE_COMPARE_AS(a.rotation().angle(), Deg(23.0f), Rad);
    CORRADE_COMPARE_AS(b.rotation().angle(), Deg(23.0f), Rad);

    CORRADE_COMPARE(a.translation(), translation);
    CORRADE_COMPARE(b.translation(), Quaternion::rotation(Deg(23.0f), Vector3::xAxis()).transformVector(translation));
}

void DualQuaternionTest::matrix() {
    DualQuaternion q = DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::rotationX(Deg(23.0f))*Matrix4::translation({-1.0f, 2.0f, 3.0f});

    /* Verify that negated dual quaternion gives the same transformation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);

    std::ostringstream o;
    Error::setOutput(&o);
    DualQuaternion::fromMatrix(m*2);
    CORRADE_COMPARE(o.str(), "Math::DualQuaternion::fromMatrix(): the matrix doesn't represent rigid transformation\n");

    DualQuaternion p = DualQuaternion::fromMatrix(m);
    CORRADE_COMPARE(p, q);
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
    (a*Dual(2)).transformPointNormalized(v);
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
