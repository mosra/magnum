/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
    void constructVectorScalar();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructFromVector();
    void constructConversion();
    void constructCopy();
    void convert();

    void isNormalized();
    template<class T> void isNormalizedEpsilonRotation();
    template<class T> void isNormalizedEpsilonTranslation();

    void lengthSquared();
    void length();
    void normalized();
    template<class T> void normalizedIterative();

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

    void sclerp();

    void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Dual<Float> Dual;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::DualQuaternion<Float> DualQuaternion;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::Vector3<Float> Vector3;

using namespace Literals;

DualQuaternionTest::DualQuaternionTest() {
    addTests({&DualQuaternionTest::construct,
              &DualQuaternionTest::constructVectorScalar,
              &DualQuaternionTest::constructIdentity,
              &DualQuaternionTest::constructZero,
              &DualQuaternionTest::constructNoInit,
              &DualQuaternionTest::constructFromVector,
              &DualQuaternionTest::constructConversion,
              &DualQuaternionTest::constructCopy,
              &DualQuaternionTest::convert,

              &DualQuaternionTest::isNormalized,
              &DualQuaternionTest::isNormalizedEpsilonRotation<Float>,
              &DualQuaternionTest::isNormalizedEpsilonRotation<Double>,
              &DualQuaternionTest::isNormalizedEpsilonTranslation<Float>,
              &DualQuaternionTest::isNormalizedEpsilonTranslation<Double>,

              &DualQuaternionTest::lengthSquared,
              &DualQuaternionTest::length,
              &DualQuaternionTest::normalized});

    addRepeatedTests<DualQuaternionTest>({
        &DualQuaternionTest::normalizedIterative<Float>,
        &DualQuaternionTest::normalizedIterative<Double>}, 1000);

    addTests({&DualQuaternionTest::quaternionConjugated,
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

              &DualQuaternionTest::sclerp,

              &DualQuaternionTest::debug});
}

void DualQuaternionTest::construct() {
    constexpr DualQuaternion a = {{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};
    CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));

    constexpr Quaternion b = a.real();
    CORRADE_COMPARE(b, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));

    constexpr Quaternion c = a.dual();
    CORRADE_COMPARE(c, Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));

    constexpr DualQuaternion d({{1.0f, 2.0f, 3.0f}, -4.0f});
    CORRADE_COMPARE(d, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, Quaternion, Quaternion>::value));
}

void DualQuaternionTest::constructVectorScalar() {
    constexpr DualQuaternion a = {{{1.0f, 2.0f, 3.0f}, {0.5f, -3.1f, 3.3f}}, {-4.0f, 2.0f}};
    CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));

    constexpr Quaternion b = a.real();
    CORRADE_COMPARE(b, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));

    constexpr Quaternion c = a.dual();
    CORRADE_COMPARE(c, Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, Math::Dual<Vector3>, Math::Dual<Float>>::value));
}

void DualQuaternionTest::constructIdentity() {
    constexpr DualQuaternion a;
    constexpr DualQuaternion b{IdentityInit};
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
    CORRADE_COMPARE(b, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(b.length(), 1.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<DualQuaternion>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, IdentityInitT>::value));
}

void DualQuaternionTest::constructZero() {
    constexpr DualQuaternion a{ZeroInit};
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 0.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, ZeroInitT>::value));
}

void DualQuaternionTest::constructNoInit() {
    DualQuaternion a{{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};
    new(&a) DualQuaternion{NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<NoInitT, DualQuaternion>::value));
}

void DualQuaternionTest::constructFromVector() {
    constexpr DualQuaternion a(Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{1.0f, 2.0f, 3.0f}, 0.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, DualQuaternion>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, Vector3>::value));
}

void DualQuaternionTest::constructConversion() {
    typedef Math::DualQuaternion<Int> DualQuaternioni;

    constexpr DualQuaternion a{{{1.3f, 2.7f, -15.0f}, 7.0f}, {{1.0f, -2.0f, 3.0f}, 0.0f}};
    constexpr DualQuaternioni b{a};

    CORRADE_COMPARE(b, (DualQuaternioni{{{1, 2, -15}, 7}, {{1, -2, 3}, 0}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<DualQuaternion, DualQuaternioni>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, DualQuaternioni>::value));
}

void DualQuaternionTest::constructCopy() {
    constexpr Math::Dual<Quaternion> a({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    DualQuaternion b(a);
    CORRADE_COMPARE(b, DualQuaternion({{1.0f, 2.0f, -3.0f}, -3.5f}, {{4.5f, -7.0f, 2.0f}, 1.0f}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<DualQuaternion>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<DualQuaternion>::value);
}

void DualQuaternionTest::convert() {
    constexpr DualQuat a{{1.5f, -3.5f, 7.0f, -0.5f}, {15.0f, 0.25f, -9.5f, 0.8f}};
    constexpr DualQuaternion b{{{1.5f, -3.5f, 7.0f}, -0.5f}, {{15.0f, 0.25f, -9.5f}, 0.8f}};

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
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
    CORRADE_VERIFY((DualQuaternion::rotation(Deg(23.0f), Vector3::xAxis())*DualQuaternion::translation({0.9f, -1.0f, -0.5f})).isNormalized());
}

template<class T> void DualQuaternionTest::isNormalizedEpsilonRotation() {
    setTestCaseName(std::string{"isNormalizedEpsilonRotation<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY((Math::DualQuaternion<T>{{{T(0.199367934417197) + TypeTraits<T>::epsilon()/T(2.0), T(0.0), T(0.0)}, T(0.97992470462083)}, {{T(0.440966117079373), T(-0.440120368706115), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));
    CORRADE_VERIFY(!(Math::DualQuaternion<T>{{{T(0.199367934417197), T(0.0), T(0.0)}, T(0.97992470462083) + TypeTraits<T>::epsilon()*T(2.0)}, {{T(0.440966117079373), T(-0.440120368706115), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));
}

template<class T> void DualQuaternionTest::isNormalizedEpsilonTranslation() {
    setTestCaseName(std::string{"isNormalizedEpsilonTranslation<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY((Math::DualQuaternion<T>{{{T(0.199367934417197), T(0.0), T(0.0)}, T(0.97992470462083)}, {{T(0.440966117079373), T(-0.440120368706115) + TypeTraits<T>::epsilon()*T(2.0), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));
    CORRADE_VERIFY(!(Math::DualQuaternion<T>{{{T(0.199367934417197), T(0.0), T(0.0)}, T(0.97992470462083)}, {{T(0.440966117079373) + TypeTraits<T>::epsilon()*T(4.0), T(-0.440120368706115), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));

    /* Large translation -- large epsilon */
    CORRADE_VERIFY((Math::DualQuaternion<T>{{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128)}, {{T(5056871.9114386), T(-245303.943266211) + TypeTraits<T>::epsilon()*T(10000000.0), T(-606492.066475555)}, T(-6315.26116124973)}}.isNormalized()));
    CORRADE_VERIFY(!(Math::DualQuaternion<T>{{{T(0.0106550719778129), T(0.311128101752138), T(-0.0468823167023769)}, T(0.949151106053128)}, {{T(5056871.9114386), T(-245303.943266211) + TypeTraits<T>::epsilon()*T(20000000.0), T(-606492.066475555)}, T(-6315.26116124973)}}.isNormalized()));
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

namespace {
    template<class> struct NormalizedIterativeData;
    template<> struct NormalizedIterativeData<Float> {
        static Math::Vector3<Float> translation() { return {10000.0f, -50.0f, 20000.0f}; }
    };
    template<> struct NormalizedIterativeData<Double> {
        static Math::Vector3<Double> translation() { return {10000000000000.0, -500.0, 20000000000000.0}; }
    };
}

template<class T> void DualQuaternionTest::normalizedIterative() {
    setTestCaseName(std::string{"normalizedIterative<"} + TypeTraits<T>::name() + ">");

    const auto axis = Math::Vector3<T>{T(0.5), T(7.9), T(0.1)}.normalized();
    auto a = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(36.7)}, Math::Vector3<T>{T(0.25), T(7.3), T(-1.1)}.normalized())*Math::DualQuaternion<T>::translation(NormalizedIterativeData<T>::translation());
    for(std::size_t i = 0; i != testCaseRepeatId(); ++i) {
        a = Math::DualQuaternion<T>::rotation(Math::Deg<T>{T(87.1)}, axis)*a;
        a = a.normalized();
    }

    CORRADE_VERIFY(a.isNormalized());
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
    Error redirectError{&o};
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
    Error redirectError{&o};

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
    Error redirectError{&o};
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
    Error redirectError{&o};
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

void DualQuaternionTest::sclerp() {
    const DualQuaternion from = DualQuaternion::translation(Vector3{20.0f, .0f, .0f})*DualQuaternion::rotation(180.0_degf, Vector3{.0f, 1.0f, .0f});
    const DualQuaternion to = DualQuaternion::translation(Vector3{42.0f, 42.0f, 42.0f})*DualQuaternion::rotation(75.0_degf, Vector3{1.0f, .0f, .0f});

    constexpr DualQuaternion expected1{Quaternion{{.23296291314453416f, .9238795325112867f, .0f}, .303603179340959f},
                                       Quaternion{{2.235619101917766f, 2.8169719855488395f, 10.722240915237789f}, -10.287636336847847f}};
    constexpr DualQuaternion expected2{Quaternion{{.4437679833315842f, .6845471059286887f, .0f}, .5783296955322937f},
                                       Quaternion{{5.764394870292371f, 11.161306653193549f, 9.671267015501789f}, -17.634394590712066f}};
    constexpr DualQuaternion expected3{Quaternion{{.5979785904506439f, .18738131458572468f, .0f}, .7793008714910992f},
                                       Quaternion{{13.409627907069353f, 25.452124456683414f, 5.681581047706807f}, -16.409481115504978f}};

    const DualQuaternion interp1 = Math::sclerp(from, to, 0.25f);
    const DualQuaternion interp2 = Math::sclerp(from, to, 0.52f);
    const DualQuaternion interp3 = Math::sclerp(from, to, 0.88f);

    CORRADE_COMPARE(interp1, expected1);
    CORRADE_COMPARE(interp2, expected2);
    CORRADE_COMPARE(interp3, expected3);

    /* Edge cases: */

    /* Dual quaternions with identical rotation */
    CORRADE_COMPARE(Math::sclerp(from, from, 0.42f), from);
    CORRADE_COMPARE(Math::sclerp(from, DualQuaternion(-from.real(), from.dual()), 0.42f), from);

    /* No difference in rotation, but in translation */
    const auto rotation = DualQuaternion::rotation(35.0_degf, Vector3{0.3f, 0.2f, 0.1f});
    CORRADE_COMPARE(Math::sclerp(DualQuaternion::translation(Vector3{1.0f, 2.0f, 4.0f})*rotation, DualQuaternion::translation(Vector3{5, -6, 2})*rotation, 0.25f),
                    DualQuaternion::translation(Vector3{2.0f, 0.0f, 3.5f})*rotation);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualQuaternionTest)
