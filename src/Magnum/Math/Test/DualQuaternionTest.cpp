/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/StrictWeakOrdering.h"

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

namespace Test { namespace {

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

    void data();

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
    void invertedNormalizedNotNormalized();

    void rotation();
    void rotationNotNormalized();
    void translation();
    void combinedTransformParts();

    void fromParts();
    void matrix();
    void matrixNotOrthogonal();
    void transformVector();
    void transformVectorNormalized();
    void transformVectorNormalizedNotNormalized();
    void transformPoint();
    void transformPointNormalized();
    void transformPointNormalizedNotNormalized();

    void sclerp();
    void sclerpShortestPath();

    void strictWeakOrdering();

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

              &DualQuaternionTest::data,

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
              &DualQuaternionTest::invertedNormalizedNotNormalized,

              &DualQuaternionTest::rotation,
              &DualQuaternionTest::rotationNotNormalized,
              &DualQuaternionTest::translation,
              &DualQuaternionTest::combinedTransformParts,

              &DualQuaternionTest::fromParts,
              &DualQuaternionTest::matrix,
              &DualQuaternionTest::matrixNotOrthogonal,
              &DualQuaternionTest::transformVector,
              &DualQuaternionTest::transformVectorNormalized,
              &DualQuaternionTest::transformVectorNormalizedNotNormalized,
              &DualQuaternionTest::transformPoint,
              &DualQuaternionTest::transformPointNormalized,
              &DualQuaternionTest::transformPointNormalizedNotNormalized,

              &DualQuaternionTest::sclerp,
              &DualQuaternionTest::sclerpShortestPath,

              &DualQuaternionTest::strictWeakOrdering,

              &DualQuaternionTest::debug});
}

void DualQuaternionTest::construct() {
    constexpr DualQuaternion a = {{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};
    CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));
    CORRADE_COMPARE(a.real(), Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_COMPARE(a.dual(), Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));

    constexpr DualQuaternion b({{1.0f, 2.0f, 3.0f}, -4.0f});
    CORRADE_COMPARE(b, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));

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

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, DualQuaternion>::value));
}

void DualQuaternionTest::constructZero() {
    constexpr DualQuaternion a{ZeroInit};
    CORRADE_COMPARE(a, DualQuaternion({{0.0f, 0.0f, 0.0f}, 0.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, DualQuaternion>::value));
}

void DualQuaternionTest::constructNoInit() {
    DualQuaternion a{{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};
    new(&a) DualQuaternion{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<DualQuaternion, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, DualQuaternion>::value));
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

void DualQuaternionTest::data() {
    constexpr DualQuaternion ca{{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};

    constexpr Quaternion b = ca.real();
    CORRADE_COMPARE(b, Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));

    constexpr Quaternion c = ca.dual();
    CORRADE_COMPARE(c, Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));

    DualQuaternion a{{{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f}};

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing a pointer is verboten */
    constexpr
    #endif
    Float d = *ca.data();
    Float e = a.data()[7];
    CORRADE_COMPARE(d, 1.0f);
    CORRADE_COMPARE(e, 2.0f);
}

void DualQuaternionTest::isNormalized() {
    CORRADE_VERIFY(!DualQuaternion({{1.0f, 2.0f, 3.0f}, 4.0f}, {}).isNormalized());
    CORRADE_VERIFY((DualQuaternion::rotation(23.0_degf, Vector3::xAxis())*DualQuaternion::translation({0.9f, -1.0f, -0.5f})).isNormalized());
}

template<class T> void DualQuaternionTest::isNormalizedEpsilonRotation() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY((Math::DualQuaternion<T>{{{T(0.199367934417197) + TypeTraits<T>::epsilon()/T(2.0), T(0.0), T(0.0)}, T(0.97992470462083)}, {{T(0.440966117079373), T(-0.440120368706115), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));
    CORRADE_VERIFY(!(Math::DualQuaternion<T>{{{T(0.199367934417197), T(0.0), T(0.0)}, T(0.97992470462083) + TypeTraits<T>::epsilon()*T(2.0)}, {{T(0.440966117079373), T(-0.440120368706115), T(-0.344665143363806)}, T(-0.0897155704877387)}}.isNormalized()));
}

template<class T> void DualQuaternionTest::isNormalizedEpsilonTranslation() {
    setTestCaseTemplateName(TypeTraits<T>::name());

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

template<class> struct NormalizedIterativeData;
template<> struct NormalizedIterativeData<Float> {
    static Math::Vector3<Float> translation() { return {10000.0f, -50.0f, 20000.0f}; }
};
template<> struct NormalizedIterativeData<Double> {
    static Math::Vector3<Double> translation() { return {10000000000000.0, -500.0, 20000000000000.0}; }
};

template<class T> void DualQuaternionTest::normalizedIterative() {
    setTestCaseTemplateName(TypeTraits<T>::name());

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

    DualQuaternion normalized = a.normalized();
    DualQuaternion inverted = normalized.invertedNormalized();
    CORRADE_COMPARE(normalized*inverted, DualQuaternion());
    CORRADE_COMPARE(inverted*normalized, DualQuaternion());
    CORRADE_COMPARE(inverted, b/Math::sqrt(Dual(30.0f, -3.6f)));
}

void DualQuaternionTest::invertedNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    DualQuaternion({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 2.5f, -3.1f,  3.3f}, 2.0f}).invertedNormalized();
    CORRADE_COMPARE(out.str(), "Math::DualQuaternion::invertedNormalized(): DualQuaternion({{1, 2, 3}, -4}, {{2.5, -3.1, 3.3}, 2}) is not normalized\n");
}

void DualQuaternionTest::rotation() {
    Vector3 axis(1.0f/Constants<Float>::sqrt3());

    DualQuaternion q = DualQuaternion::rotation(120.0_degf, axis);
    CORRADE_COMPARE(q.length(), 1.0f);
    CORRADE_COMPARE(q, DualQuaternion({Vector3(0.5f, 0.5f, 0.5f), 0.5f}, {{}, 0.0f}));
    CORRADE_COMPARE_AS(q.rotation().angle(), 120.0_degf, Deg);
    CORRADE_COMPARE(q.rotation().axis(), axis);

    /* Constexpr access to rotation */
    constexpr DualQuaternion b({{-1.0f, 2.0f, 3.0f}, 4.0f}, {});
    constexpr Quaternion c = b.rotation();
    CORRADE_COMPARE(c, Quaternion({-1.0f, 2.0f, 3.0f}, 4.0f));

    /* Conversion from a rotation quaternion should give the same result */
    CORRADE_COMPARE(DualQuaternion{Quaternion::rotation(120.0_degf, axis)}, q);
}

void DualQuaternionTest::rotationNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    DualQuaternion::rotation(120.0_degf, Vector3(2.0f));
    CORRADE_COMPARE(out.str(), "Math::Quaternion::rotation(): axis Vector(2, 2, 2) is not normalized\n");
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
    DualQuaternion a = DualQuaternion::translation(translation)*DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(23.0_degf, Vector3::xAxis())*DualQuaternion::translation(translation);

    CORRADE_COMPARE(a.rotation().axis(), Vector3::xAxis());
    CORRADE_COMPARE(b.rotation().axis(), Vector3::xAxis());
    CORRADE_COMPARE_AS(a.rotation().angle(), 23.0_degf, Rad);
    CORRADE_COMPARE_AS(b.rotation().angle(), 23.0_degf, Rad);

    CORRADE_COMPARE(a.translation(), translation);
    CORRADE_COMPARE(b.translation(), Quaternion::rotation(23.0_degf, Vector3::xAxis()).transformVector(translation));
}

void DualQuaternionTest::fromParts() {
    Vector3 axis(1.0f/Constants<Float>::sqrt3());
    Quaternion r = Quaternion::rotation(120.0_degf, axis);

    Vector3 vec(1.0f, -3.5f, 0.5f);
    DualQuaternion t = DualQuaternion::translation(vec);

    DualQuaternion rt = t*DualQuaternion{r};
    CORRADE_COMPARE(DualQuaternion::from(r, vec), rt);
}

void DualQuaternionTest::matrix() {
    DualQuaternion q = DualQuaternion::rotation(23.0_degf, Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::rotationX(23.0_degf)*Matrix4::translation({-1.0f, 2.0f, 3.0f});

    /* Verify that negated dual quaternion gives the same transformation */
    CORRADE_COMPARE(q.toMatrix(), m);
    CORRADE_COMPARE((-q).toMatrix(), m);

    CORRADE_COMPARE(DualQuaternion::fromMatrix(m), q);
}

void DualQuaternionTest::matrixNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    DualQuaternion::fromMatrix(Matrix4::rotationX(23.0_degf)*Matrix4::translation({-1.0f, 2.0f, 3.0f})*2);
    CORRADE_COMPARE(out.str(),
        "Math::DualQuaternion::fromMatrix(): the matrix doesn't represent a rigid transformation:\n"
        "Matrix(2, 0, 0, -2,\n"
        "       0, 1.84101, -0.781462, 1.33763,\n"
        "       0, 0.781462, 1.84101, 7.08595,\n"
        "       0, 0, 0, 2)\n");
}

void DualQuaternionTest::transformVector() {
    DualQuaternion a = DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    Quaternion q = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.transformVector(v);
    /* Delegates to Quaternion, so should give the same result */
    CORRADE_COMPARE(rotated, q.transformVector(v));
    CORRADE_COMPARE(rotated, (Vector3{5.0f, -3.58733f, -0.762279f}));
}

void DualQuaternionTest::transformVectorNormalized() {
    DualQuaternion a = DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    Quaternion q = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    Vector3 v(5.0f, -3.6f, 0.7f);

    Vector3 rotated = a.transformVectorNormalized(v);
    /* Delegates to Quaternion, so should give the same result */
    CORRADE_COMPARE(rotated, q.transformVector(v));
    CORRADE_COMPARE(rotated, a.transformVector(v));
}

void DualQuaternionTest::transformVectorNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Quaternion a = Quaternion::rotation(23.0_degf, Vector3::xAxis());
    (a*2).transformVectorNormalized({});
    /* Delegates to quaternion, so the assert prints Quaternion */
    CORRADE_COMPARE(out.str(), "Math::Quaternion::transformVectorNormalized(): Quaternion({0.398736, 0, 0}, 1.95985) is not normalized\n");
}

void DualQuaternionTest::transformPoint() {
    DualQuaternion a = DualQuaternion::translation({-1.0f, 2.0f, 3.0f})*DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(23.0_degf, Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::translation({-1.0f, 2.0f, 3.0f})*Matrix4::rotationX(23.0_degf);
    Matrix4 n = Matrix4::rotationX(23.0_degf)*Matrix4::translation({-1.0f, 2.0f, 3.0f});
    Vector3 v(0.0f, -3.6f, 0.7f);

    Vector3 transformedA = (a*Dual(2)).transformPoint(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector3(-1.0f, -1.58733f, 2.237721f));

    Vector3 transformedB = (b*Dual(2)).transformPoint(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector3(-1.0f, -2.918512f, 2.780698f));
}

void DualQuaternionTest::transformPointNormalized() {
    DualQuaternion a = DualQuaternion::translation({-1.0f, 2.0f, 3.0f})*DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    DualQuaternion b = DualQuaternion::rotation(23.0_degf, Vector3::xAxis())*DualQuaternion::translation({-1.0f, 2.0f, 3.0f});
    Matrix4 m = Matrix4::translation({-1.0f, 2.0f, 3.0f})*Matrix4::rotationX(23.0_degf);
    Matrix4 n = Matrix4::rotationX(23.0_degf)*Matrix4::translation({-1.0f, 2.0f, 3.0f});
    Vector3 v(0.0f, -3.6f, 0.7f);

    Vector3 transformedA = a.transformPointNormalized(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector3(-1.0f, -1.58733f, 2.237721f));

    Vector3 transformedB = b.transformPointNormalized(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector3(-1.0f, -2.918512f, 2.780698f));
}

void DualQuaternionTest::transformPointNormalizedNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    DualQuaternion a = DualQuaternion::translation({-1.0f, 2.0f, 3.0f})*DualQuaternion::rotation(23.0_degf, Vector3::xAxis());
    (a*Dual(2)).transformPointNormalized({});
    CORRADE_COMPARE(out.str(), "Math::DualQuaternion::transformPointNormalized(): DualQuaternion({{0.398736, 0, 0}, 1.95985}, {{-0.979925, 2.55795, 2.54104}, 0.199368}) is not normalized\n");
}

void DualQuaternionTest::sclerp() {
    auto from = DualQuaternion::translation({20.0f, 0.0f, 0.0f})*
        DualQuaternion::rotation(65.0_degf, Vector3::yAxis());
    auto to = DualQuaternion::translation({42.0f, 42.0f, 42.0f})*
        DualQuaternion::rotation(75.0_degf, Vector3::xAxis());

    const DualQuaternion begin = Math::sclerp(from, to, 0.0f);
    const DualQuaternion beginShortestPath = Math::sclerpShortestPath(from, to, 0.0f);
    const DualQuaternion end = Math::sclerp(from, to, 1.0f);
    const DualQuaternion endShortestPath = Math::sclerpShortestPath(from, to, 1.0f);
    CORRADE_COMPARE(begin, from);
    CORRADE_COMPARE(beginShortestPath, from);
    CORRADE_COMPARE(end, to);
    CORRADE_COMPARE(endShortestPath, to);

    DualQuaternion expected1{
        {{0.170316f, 0.424975f, 0.0f}, 0.889038f},
        {{10.689f, 7.47059f, 5.33428f}, -5.61881f}};
    DualQuaternion expected2{
        {{0.34568f, 0.282968f, 0.0f}, 0.89467f},
        {{12.8764f, 15.8357f, 5.03088f}, -9.98371f}};
    DualQuaternion expected3{
        {{0.550678f, 0.072563f, 0.0f}, 0.831558f},
        {{15.6916f, 26.3477f, 4.23219f}, -12.6905f}};

    const DualQuaternion interp1 = Math::sclerp(from, to, 0.25f);
    const DualQuaternion interp1ShortestPath = Math::sclerpShortestPath(from, to, 0.25f);
    const DualQuaternion interp2 = Math::sclerp(from, to, 0.52f);
    const DualQuaternion interp2ShortestPath = Math::sclerpShortestPath(from, to, 0.52f);
    const DualQuaternion interp3 = Math::sclerp(from, to, 0.88f);
    const DualQuaternion interp3ShortestPath = Math::sclerpShortestPath(from, to, 0.88f);

    CORRADE_COMPARE(interp1, expected1);
    CORRADE_COMPARE(interp1ShortestPath, expected1);
    CORRADE_COMPARE(interp2, expected2);
    CORRADE_COMPARE(interp2ShortestPath, expected2);
    CORRADE_COMPARE(interp3, expected3);
    CORRADE_COMPARE(interp3ShortestPath, expected3);

    /* Edge cases: */

    /* Dual quaternions with identical rotation */
    CORRADE_COMPARE(Math::sclerp(from, from, 0.42f), from);
    CORRADE_COMPARE(Math::sclerpShortestPath(from, from, 0.42f), from);
    CORRADE_COMPARE(Math::sclerp(from, -from, 0.42f), from);
    CORRADE_COMPARE(Math::sclerpShortestPath(from, -from, 0.42f), from);

    /* No difference in rotation, but in translation */
    {
        auto rotation = DualQuaternion::rotation(35.0_degf, Vector3{0.3f, 0.2f, 0.1f}.normalized());
        auto a = DualQuaternion::translation({1.0f, 2.0f, 4.0f})*rotation;
        auto b = DualQuaternion::translation({5.0f, -6.0f, 2.0f})*rotation;
        auto expected = DualQuaternion::translation({2.0f, 0.0f, 3.5f})*rotation;

        auto interpolateTranslation = Math::sclerp(a, b, 0.25f);
        auto interpolateTranslationShortestPath = Math::sclerpShortestPath(a, b, 0.25f);
        CORRADE_VERIFY(interpolateTranslation.isNormalized());
        CORRADE_VERIFY(interpolateTranslationShortestPath.isNormalized());
        CORRADE_COMPARE(interpolateTranslation, expected);
        CORRADE_COMPARE(interpolateTranslationShortestPath, expected);
    }
}

void DualQuaternionTest::sclerpShortestPath() {
    DualQuaternion a = DualQuaternion::translation({1.5f, 0.3f, 0.0f})*
        DualQuaternion::rotation(0.0_degf, Vector3::zAxis());
    DualQuaternion b = DualQuaternion::translation({3.5f, 0.3f, 1.0f})*
        DualQuaternion::rotation(225.0_degf, Vector3::zAxis());

    DualQuaternion sclerp = Math::sclerp(a, b, 0.25f);
    DualQuaternion sclerpShortestPath = Math::sclerpShortestPath(a, b, 0.25f);

    CORRADE_VERIFY(sclerp.isNormalized());
    CORRADE_VERIFY(sclerpShortestPath.isNormalized());
    CORRADE_COMPARE(sclerp.rotation().axis(), Vector3::zAxis());
    /** @todo why is this inverted compared to QuaternionTest::slerpShortestPath()? */
    CORRADE_COMPARE(sclerpShortestPath.rotation().axis(), -Vector3::zAxis());
    CORRADE_COMPARE(sclerp.rotation().angle(), 56.25_degf);
    /* Because the axis is inverted, this is also inverted compared to
       QuaternionTest::slerpShortestPath() */
    CORRADE_COMPARE(sclerpShortestPath.rotation().angle(), 360.0_degf - 326.25_degf);

    CORRADE_COMPARE(sclerp, (DualQuaternion{
        {{0.0f, 0.0f, 0.471397f}, 0.881921f},
        {{0.536892f, -0.692656f, 0.11024f}, -0.0589246f}}));
    /* Also inverted compared to QuaternionTest::slerpShortestPath() */
    CORRADE_COMPARE(sclerpShortestPath, (DualQuaternion{
        {{0.0f, 0.0f, -0.290285f}, 0.95694f},
        {{0.794402f, 0.651539f, 0.119618f}, 0.0362856f}}));

    /* Translation along Z should be the same in both, in 25% of the way.
       Translation in the XY plane is along a screw, so that's different. */
    CORRADE_COMPARE(sclerpShortestPath.translation().z(), 0.25f);
    CORRADE_COMPARE(sclerpShortestPath.translation().z(), 0.25f);
}

void DualQuaternionTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const DualQuaternion a{{{1.0f, 2.0f, 3.0f}, 0.0f}, {{1.0f, 2.0f, 3.0f}, 3.0f}};
    const DualQuaternion b{{{1.0f, 2.0f, 3.0f}, 2.0f}, {{3.0f, 2.0f, 3.0f}, 4.0f}};
    const DualQuaternion c{{{1.0f, 2.0f, 3.0f}, 0.0f}, {{1.0f, 2.0f, 3.0f}, 4.0f}};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));
    CORRADE_VERIFY(!o(a, a));
}

void DualQuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(o.str(), "DualQuaternion({{1, 2, 3}, -4}, {{0.5, -3.1, 3.3}, 2})\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualQuaternionTest)
