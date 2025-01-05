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
#include <Corrade/Containers/ArrayView.h> /* arraySize() */
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Half.h"
#include "Magnum/Math/Vector.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Vec3 {
    float x, y, z;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<3, Float, Vec3> {
    constexpr static Vector<3, Float> from(const Vec3& other) {
        return {other.x, other.y, other.z};
    }

    constexpr static Vec3 to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

}

namespace Test { namespace {

struct VectorTest: TestSuite::Tester {
    explicit VectorTest();

    void construct();
    void constructFromData();
    void constructPad();
    void constructPadDefaultHalf();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructOneComponent();
    void constructArray();
    void constructArrayRvalue();
    void constructConversion();
    void constructBit();
    void constructCopy();
    void convert();

    void isZeroFloat();
    void isZeroInteger();
    void isNormalized();

    void data();

    void promotedNegated();
    void addSubtract();
    void multiplyDivide();
    void multiplyDivideIntegral();
    void multiplyDivideComponentWise();
    void multiplyDivideComponentWiseIntegral();
    void modulo();
    void bitwise();

    void compare();
    void compareComponentWise();

    void dot();
    void dotSelf();
    void length();
    void lengthInverted();
    void normalized();
    void resized();

    void sum();
    void product();
    void min();
    void max();
    void minmax();

    void nanIgnoring();

    void projected();
    void projectedOntoNormalized();
    void projectedOntoNormalizedNotNormalized();
    void flipped();

    void angle();
    void angleNormalizedButOver1();
    void angleNotNormalized();

    void subclassTypes();
    void subclass();

    void strictWeakOrdering();

    void debug();
    void debugPacked();
    void debugPropagateFlags();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace */
using Magnum::Constants;
using Magnum::Rad;
typedef Vector<2, Float> Vector2;
typedef Vector<2, Half> Vector2h;
typedef Vector<3, Float> Vector3;
typedef Vector<4, Float> Vector4;
typedef Vector<4, Half> Vector4h;
typedef Vector<4, Int> Vector4i;
typedef Vector<2, Int> Vector2i;

using namespace Literals;

VectorTest::VectorTest() {
    addTests({&VectorTest::construct,
              &VectorTest::constructFromData,
              &VectorTest::constructPad,
              &VectorTest::constructPadDefaultHalf,
              &VectorTest::constructDefault,
              &VectorTest::constructNoInit,
              &VectorTest::constructOneValue,
              &VectorTest::constructOneComponent,
              &VectorTest::constructArray,
              &VectorTest::constructArrayRvalue,
              &VectorTest::constructConversion,
              &VectorTest::constructBit,
              &VectorTest::constructCopy,
              &VectorTest::convert,

              &VectorTest::isZeroFloat,
              &VectorTest::isZeroInteger,
              &VectorTest::isNormalized,

              &VectorTest::data,

              &VectorTest::promotedNegated,
              &VectorTest::addSubtract,
              &VectorTest::multiplyDivide,
              &VectorTest::multiplyDivideIntegral,
              &VectorTest::multiplyDivideComponentWise,
              &VectorTest::multiplyDivideComponentWiseIntegral,
              &VectorTest::modulo,
              &VectorTest::bitwise,

              &VectorTest::compare,
              &VectorTest::compareComponentWise,

              &VectorTest::dot,
              &VectorTest::dotSelf,
              &VectorTest::length,
              &VectorTest::lengthInverted,
              &VectorTest::normalized,
              &VectorTest::resized,

              &VectorTest::sum,
              &VectorTest::product,
              &VectorTest::min,
              &VectorTest::max,
              &VectorTest::minmax,

              &VectorTest::nanIgnoring,

              &VectorTest::projected,
              &VectorTest::projectedOntoNormalized,
              &VectorTest::projectedOntoNormalizedNotNormalized,
              &VectorTest::flipped,

              &VectorTest::angle,
              &VectorTest::angleNormalizedButOver1,
              &VectorTest::angleNotNormalized,

              &VectorTest::subclassTypes,
              &VectorTest::subclass,

              &VectorTest::strictWeakOrdering,

              &VectorTest::debug,
              &VectorTest::debugPacked,
              &VectorTest::debugPropagateFlags});
}

void VectorTest::construct() {
    constexpr Vector4 a = {1.0f, 2.0f, -3.0f, 4.5f};
    CORRADE_COMPARE(a, Vector4(1.0f, 2.0f, -3.0f, 4.5f));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Float, Float, Float, Float>::value);
}

void VectorTest::constructFromData() {
    Float data[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    CORRADE_COMPARE(Vector4::from(data), Vector4(1.0f, 2.0f, 3.0f, 4.0f));
}

void VectorTest::constructPad() {
    constexpr Vector<2, Float> a{1.0f, -1.0f};
    constexpr Vector4 b = Vector4::pad(a);
    constexpr Vector4 c = Vector4::pad(a, 5.0f);
    CORRADE_COMPARE(b, Vector4(1.0f, -1.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(c, Vector4(1.0f, -1.0f, 5.0f, 5.0f));

    constexpr Vector<5, Float> d{1.0f, -1.0f, 8.0f, 2.3f, -1.1f};
    constexpr Vector4 e = Vector4::pad(d);
    CORRADE_COMPARE(e, Vector4(1.0f, -1.0f, 8.0f, 2.3f));
}

void VectorTest::constructPadDefaultHalf() {
    using namespace Literals;
    /* The default pad value should work also for the Half type */
    Vector4h a = Vector4h::pad(Vector2h{1.0_h, -1.0_h});
    CORRADE_COMPARE(a, (Vector4h{1.0_h, -1.0_h, 0.0_h, 0.0_h}));
}

void VectorTest::constructDefault() {
    constexpr Vector4 a;
    constexpr Vector4 b{ZeroInit};
    CORRADE_COMPARE(a, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(b, Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Vector4>::value);
}

void VectorTest::constructNoInit() {
    Vector4 a{1.0f, 2.0f, -3.0f, 4.5f};
    new(&a) Vector4{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, (Vector4{1.0f, 2.0f, -3.0f, 4.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Vector4>::value);
}

void VectorTest::constructOneValue() {
    constexpr Vector4 a(7.25f);

    CORRADE_COMPARE(a, Vector4(7.25f, 7.25f, 7.25f, 7.25f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Vector4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Float>::value);
}

void VectorTest::constructOneComponent() {
    typedef Vector<1, Float> Vector1;

    /* Implicit constructor must work */
    constexpr Vector1 vec = 1.0f;
    CORRADE_COMPARE(vec, Vector1(1));

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector1, Float>::value);
}

void VectorTest::constructArray() {
    float data[]{1.3f, 2.7f, -15.0f};
    Vector3 a{data};
    CORRADE_COMPARE(a, (Vector3{1.3f, 2.7f, -15.0f}));

    constexpr float cdata[]{1.3f, 2.7f, -15.0f};
    constexpr Vector3 ca{cdata};
    CORRADE_COMPARE(ca, (Vector3{1.3f, 2.7f, -15.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[3], Vector3>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector3, float[3]>::value);

    /* It should always be constructible only with exactly the matching number
       of elements. As that's checked with a static_assert(), it's impossible
       to verify with std::is_constructible unfortunately and the only way to
       test that is manually, thus uncomment the code below to test the error
       behavior.

       Additionally, to avoid noise in the compiler output, the first should
       only produce "excess elements in array initializer" and a static assert,
       the second just a static assert, no other compiler error. */
    #if 0
    float data1[]{1.3f};
    float data4[]{1.3f, 2.7f, -15.0f, 7.0f};
    Vector3 b{data1};
    Vector3 c{data4};
    #endif
}

void VectorTest::constructArrayRvalue() {
    /* Silly but why not. Could theoretically help with some fancier types
       that'd otherwise require explicit typing with the variadic
       constructor. */
    Vector3 a{{1.3f, 2.7f, -15.0f}};
    CORRADE_COMPARE(a, (Vector3{1.3f, 2.7f, -15.0f}));

    constexpr Vector3 ca{{1.3f, 2.7f, -15.0f}};
    CORRADE_COMPARE(ca, (Vector3{1.3f, 2.7f, -15.0f}));

    /* It should always be constructible only with exactly the matching number
       of elements. As that's checked with a static_assert(), it's impossible
       to verify with std::is_constructible unfortunately and the only way to
       test that is manually, thus uncomment the code below to test the error
       behavior.

       Additionally, to avoid noise in the compiler output, the first should
       only produce "excess elements in array initializer" and a static assert,
       the second just a static assert, no other compiler error. */
    #if 0
    Vector3 c{{1.3f, 2.7f, -15.0f, 7.0f}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a Vector from a smaller array isn't an error on GCC 4.8.");
    Vector3 b{{1.3f, 2.7f}};
    #endif
}

void VectorTest::constructConversion() {
    constexpr Vector4 a(1.3f, 2.7f, -15.0f, 7.0f);
    constexpr Vector4i b(a);

    CORRADE_COMPARE(b, Vector4i(1, 2, -15, 7));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vector4, Vector4i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, Vector4i>::value);
}

void VectorTest::constructBit() {
    BitVector4 a{'\xa'}; /* 0b1010 */
    CORRADE_COMPARE(Vector4{a}, (Vector4{0.0f, 1.0f, 0.0f, 1.0f}));

    constexpr BitVector4 ca{'\xa'}; /* 0b1010 */
    constexpr Vector4 cb{ca};
    CORRADE_COMPARE(cb, (Vector4{0.0f, 1.0f, 0.0f, 1.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector4, Vector4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Vector4, BitVector4>::value);
}

void VectorTest::constructCopy() {
    constexpr Vector4 a(1.0f, 3.5f, 4.0f, -2.7f);
    constexpr Vector4 b(a);
    CORRADE_COMPARE(b, Vector4(1.0f, 3.5f, 4.0f, -2.7f));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Vector4>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Vector4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Vector4>::value);
}

void VectorTest::convert() {
    constexpr Vec3 a{1.5f, 2.0f, -3.5f};
    constexpr Vector3 b(1.5f, 2.0f, -3.5f);

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Vector3 c{a};
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Vec3(b);
    #else
    constexpr Vec3 d(b);
    #endif
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Vec3, Vector3>::value);
    CORRADE_VERIFY(!std::is_convertible<Vector3, Vec3>::value);
}

void VectorTest::isZeroFloat() {
    CORRADE_VERIFY(!Vector3(0.01f, 0.0f, 0.0f).isZero());
    CORRADE_VERIFY(Vector3(0.0f, Math::TypeTraits<float>::epsilon()/2.0f, 0.0f).isZero());
    CORRADE_VERIFY(Vector3(0.0f, 0.0f, 0.0f).isZero());
}

void VectorTest::isZeroInteger() {
    CORRADE_VERIFY(!Math::Vector<3, Int>{0, 1, 0}.isZero());
    CORRADE_VERIFY(Math::Vector<3, Int>{0, 0, 0}.isZero());
}

void VectorTest::isNormalized() {
    CORRADE_VERIFY(!Vector3(1.0f, 2.0f, -1.0f).isNormalized());
    CORRADE_VERIFY(Vector3(0.0f, 1.0f, 0.0f).isNormalized());
}

void VectorTest::data() {
    Vector4 a(4.0f, 5.0f, 6.0f, 7.0f);
    a[2] = 1.0f;
    a[3] = 1.5f;

    CORRADE_COMPARE(a[2], 1.0f);
    CORRADE_COMPARE(a[3], 1.5f);
    CORRADE_COMPARE(a, Vector4(4.0f, 5.0f, 1.0f, 1.5f));

    constexpr Vector4 ca(1.0f, 2.0f, -3.0f, 4.5f);
    constexpr Float f = ca[3];
    CORRADE_COMPARE(f, 4.5f);

    /* Pointer chasings, i.e. *(b.data()[3]), are not possible */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing a pointer is verboten */
    constexpr
    #endif
    Float g = *ca.data();
    CORRADE_COMPARE(a.data()[1], 5.0f);
    CORRADE_COMPARE(g, 1.0f);

    /* It actually returns an array */
    CORRADE_COMPARE(Containers::arraySize(a.data()), 4);
    CORRADE_COMPARE(Containers::arraySize(ca.data()), 4);
}

void VectorTest::compare() {
    CORRADE_VERIFY(Vector4(1.0f, -3.5f, 5.0f, -10.0f) == Vector4(1.0f + TypeTraits<Float>::epsilon()/2, -3.5f, 5.0f, -10.0f));
    CORRADE_VERIFY(Vector4(1.0f, -1.0f, 5.0f, -10.0f) != Vector4(1.0f, -1.0f + TypeTraits<Float>::epsilon()*2, 5.0f, -10.0f));

    CORRADE_VERIFY(Vector4i(1, -3, 5, -10) == Vector4i(1, -3, 5, -10));
    CORRADE_VERIFY(Vector4i(1, -3, 5, -10) != Vector4i(1, -2, 5, -10));
}

void VectorTest::compareComponentWise() {
    typedef BitVector<3> BitVector3;
    typedef BitVector<4> BitVector4;

    Vector4 a{1.0f, -3.5f, 5.0f, -10.0f};
    Vector4 b{1.0f + TypeTraits<Float>::epsilon()/2, -3.5f, 5.0f - TypeTraits<Float>::epsilon()*2, -10.0f};
    Vector4 c{1.0f + TypeTraits<Float>::epsilon()*2, -3.5f, 5.0f - TypeTraits<Float>::epsilon()*10, -10.0f};
    CORRADE_COMPARE(equal(a, b), BitVector4{0xf});
    CORRADE_COMPARE(equal(a, c), BitVector4{0xa});
    CORRADE_COMPARE(notEqual(a, b), BitVector4{0x0});
    CORRADE_COMPARE(notEqual(a, c), BitVector4{0x5});

    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) < Vector3(1.1f, -1.0f, 3.0f), BitVector3(0x1));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) <= Vector3(1.1f, -1.0f, 3.0f), BitVector3(0x3));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) >= Vector3(1.1f, -1.0f, 3.0f), BitVector3(0x6));
    CORRADE_COMPARE(Vector3(1.0f, -1.0f, 5.0f) > Vector3(1.1f, -1.0f, 3.0f), BitVector3(0x4));
}

void VectorTest::promotedNegated() {
    CORRADE_COMPARE(+(Vector4{1.0f, -3.0f, 5.0f, -10.0f}),
                     (Vector4{1.0f, -3.0f, 5.0f, -10.0f}));
    CORRADE_COMPARE(-(Vector4{1.0f, -3.0f, 5.0f, -10.0f}),
                     (Vector4{-1.0f, 3.0f, -5.0f, 10.0f}));

    constexpr Vector4 a{1.0f, -3.0f, 5.0f, -10.0f};
    constexpr Vector4 promotedA = +a;
    constexpr Vector4 negatedA = -a;
    CORRADE_COMPARE(promotedA, a);
    CORRADE_COMPARE(negatedA, (Vector4{-1.0f, 3.0f, -5.0f, 10.0f}));
}

void VectorTest::addSubtract() {
    const Vector4 a{1.0f, -3.0f, 5.0f, -10.0f};
    const Vector4 b{7.5f, 33.0f, -15.0f, 0.0f};
    const Vector4 c{8.5f, 30.0f, -10.0f, -10.0f};

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
    {
        Vector4 v{1.0f, -3.0f, 5.0f, -10.0f};
        CORRADE_COMPARE(&(v += b), &v);
        CORRADE_COMPARE(v, c);
    } {
        Vector4 v{8.5f, 30.0f, -10.0f, -10.0f};
        CORRADE_COMPARE(&(v -= b), &v);
        CORRADE_COMPARE(v, a);
    }

    constexpr Vector4 ca{1.0f, -3.0f, 5.0f, -10.0f};
    constexpr Vector4 cb{7.5f, 33.0f, -15.0f, 0.0f};
    constexpr Vector4 cd = ca + cb;
    CORRADE_COMPARE(cd, c);

    constexpr Vector4 cc{8.5f, 30.0f, -10.0f, -10.0f};
    constexpr Vector4 ce = cc - cb;
    CORRADE_COMPARE(ce, a);
}

void VectorTest::multiplyDivide() {
    const Vector4 vector{1.0f, 2.0f, 3.0f, 4.0f};
    const Vector4 multiplied{-1.5f, -3.0f, -4.5f, -6.0f};

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*vector, multiplied);
    {
        Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
        CORRADE_COMPARE(&(v *= -1.5f), &v);
        CORRADE_COMPARE(v, multiplied);
    }

    CORRADE_COMPARE(multiplied/-1.5f, vector);
    {
        Vector4 v{-1.5f, -3.0f, -4.5f, -6.0f};
        CORRADE_COMPARE(&(v /= -1.5f), &v);
        CORRADE_COMPARE(v, vector);
    }

    constexpr Vector4 cvector{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Vector4 ca1 = cvector*-1.5f;
    constexpr Vector4 ca2 = -1.5f*cvector;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);

    constexpr Vector4 cmultiplied{-1.5f, -3.0f, -4.5f, -6.0f};
    constexpr Vector4 cb = cmultiplied/-1.5f;
    CORRADE_COMPARE(cb, vector);

    /* Divide a vector with a scalar and invert */
    const Vector4 divisor{1.0f, 2.0f, -4.0f, 8.0f};
    const Vector4 result{1.0f, 0.5f, -0.25f, 0.125f};
    CORRADE_COMPARE(1.0f/divisor, result);

    constexpr Vector4 cdivisor(1.0f, 2.0f, -4.0f, 8.0f);
    constexpr Vector4 ce = 1.0f/cdivisor;
    CORRADE_COMPARE(ce, result);
}

void VectorTest::multiplyDivideIntegral() {
    const Vector4i vector{32, 10, -6, 2};
    const Vector4i multiplied{-48, -15, 9, -3};

    CORRADE_COMPARE(vector*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*vector, multiplied);
    {
        Vector4i v{32, 10, -6, 2};
        CORRADE_COMPARE(&(v *= -1.5f), &v);
        CORRADE_COMPARE(v, multiplied);
    }

    CORRADE_COMPARE(multiplied/-1.5f, vector);
    {
        Vector4i v{-48, -15, 9, -3};
        CORRADE_COMPARE(&(v /= -1.5f), &v);
        CORRADE_COMPARE(v, vector);
    }

    constexpr Vector4i cvector{32, 10, -6, 2};
    constexpr Vector4i ca1 = cvector*-1.5f;
    constexpr Vector4i ca2 = -1.5f*cvector;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);

    constexpr Vector4i cmultiplied{-48, -15, 9, -3};
    constexpr Vector4i cb = cmultiplied/-1.5f;
    CORRADE_COMPARE(cb, cvector);

    /* Using integer vector as divisor is not supported */
}

void VectorTest::multiplyDivideComponentWise() {
    const Vector4 vec{1.0f, 2.0f, 3.0f, 4.0f};
    const Vector4 multiplier{7.0f, -4.0f, -1.5f, 1.0f};
    const Vector4 multiplied{7.0f, -8.0f, -4.5f, 4.0f};

    CORRADE_COMPARE(vec*multiplier, multiplied);
    {
        Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
        CORRADE_COMPARE(&(v *= multiplier), &v);
        CORRADE_COMPARE(v, multiplied);
    }

    CORRADE_COMPARE(multiplied/multiplier, vec);
    {
        Vector4 v{7.0f, -8.0f, -4.5f, 4.0f};
        CORRADE_COMPARE(&(v /= multiplier), &v);
        CORRADE_COMPARE(v, vec);
    }

    constexpr Vector4 cvec{1.0f, 2.0f, 3.0f, 4.0f};
    constexpr Vector4 cmultiplier{7.0f, -4.0f, -1.5f, 1.0f};
    constexpr Vector4 cmultiplied{7.0f, -8.0f, -4.5f, 4.0f};
    constexpr Vector4 ca = cvec*cmultiplier;
    constexpr Vector4 cb = cmultiplied/cmultiplier;
    CORRADE_COMPARE(ca, multiplied);
    CORRADE_COMPARE(cb, vec);
}

void VectorTest::multiplyDivideComponentWiseIntegral() {
    const Vector4i vec{7, 2, -16, -1};
    const Vector4 multiplier{2.0f, -1.5f, 0.5f, 10.0f};
    const Vector4i multiplied{14, -3, -8, -10};

    CORRADE_COMPARE(vec*multiplier, multiplied);
    CORRADE_COMPARE(multiplier*vec, multiplied);
    {
        Vector4i v{7, 2, -16, -1};
        CORRADE_COMPARE(&(v *= multiplier), &v);
        CORRADE_COMPARE(v, multiplied);
    }

    CORRADE_COMPARE(multiplied/multiplier, vec);
    {
        Vector4i v{14, -3, -8, -10};
        CORRADE_COMPARE(&(v /= multiplier), &v);
        CORRADE_COMPARE(v, vec);
    }

    constexpr Vector4i cvec{7, 2, -16, -1};
    constexpr Vector4 cmultiplier{2.0f, -1.5f, 0.5f, 10.0f};
    constexpr Vector4i ca1 = cvec*cmultiplier;
    constexpr Vector4i ca2 = cmultiplier*cvec;
    CORRADE_COMPARE(ca1, multiplied);
    CORRADE_COMPARE(ca2, multiplied);

    constexpr Vector4i cmultiplied{14, -3, -8, -10};
    constexpr Vector4i cb = cmultiplied/cmultiplier;
    CORRADE_COMPARE(cb, cvec);

    /* Using integer vector as divisor is not supported */
}

void VectorTest::modulo() {
    const Vector4i a{4, 13, 255, -6};
    const Vector4i b{2, 5, 64, -4};

    CORRADE_COMPARE(a % 2, (Vector4i{0, 1, 1, 0}));
    {
        Vector4i v{4, 13, 255, -6};
        CORRADE_COMPARE(&(v %= 2), &v);
        CORRADE_COMPARE(v, (Vector4i{0, 1, 1, 0}));
    }

    CORRADE_COMPARE(a % b, (Vector4i{0, 3, 63, -2}));
    {
        Vector4i v{4, 13, 255, -6};
        CORRADE_COMPARE(&(v %= b), &v);
        CORRADE_COMPARE(v, (Vector4i{0, 3, 63, -2}));
    }

    constexpr Vector4i ca{4, 13, 255, -6};
    constexpr Vector4i cb{2, 5, 64, -4};
    constexpr Vector4i cc = ca % 2;
    constexpr Vector4i cd = ca % cb;
    CORRADE_COMPARE(cc, (Vector4i{0, 1, 1, 0}));
    CORRADE_COMPARE(cd, (Vector4i{0, 3, 63, -2}));
}

void VectorTest::bitwise() {
    const Vector4i a{85, 240, -241, 33};
    const Vector4i b{170, 85, 13, -11};
    CORRADE_COMPARE(~a, (Vector4i{-86, -241, 240, -34}));

    CORRADE_COMPARE(a & b, (Vector4i{0, 80, 13, 33}));
    {
        Vector4i v{85, 240, -241, 33};
        CORRADE_COMPARE(&(v &= b), &v);
        CORRADE_COMPARE(v, (Vector4i{0, 80, 13, 33}));
    }

    CORRADE_COMPARE(a | b, (Vector4i{255, 245, -241, -11}));
    {
        Vector4i v{85, 240, -241, 33};
        CORRADE_COMPARE(&(v |= b), &v);
        CORRADE_COMPARE(v, (Vector4i{255, 245, -241, -11}));
    }

    CORRADE_COMPARE(a ^ b, (Vector4i{255, 165, -254, -44}));
    {
        Vector4i v{85, 240, -241, 33};
        CORRADE_COMPARE(&(v ^= b), &v);
        CORRADE_COMPARE(v, (Vector4i{255, 165, -254, -44}));
    }

    constexpr Vector4i ca{85, 240, -241, 33};
    constexpr Vector4i cb{170, 85, 13, -11};
    constexpr Vector4i cd = ~ca;
    constexpr Vector4i ce = ca & cb;
    constexpr Vector4i cf = ca | cb;
    constexpr Vector4i cg = ca ^ cb;
    CORRADE_COMPARE(cd, (Vector4i{-86, -241, 240, -34}));
    CORRADE_COMPARE(ce, (Vector4i{0, 80, 13, 33}));
    CORRADE_COMPARE(cf, (Vector4i{255, 245, -241, -11}));
    CORRADE_COMPARE(cg, (Vector4i{255, 165, -254, -44}));

    const Vector4i c{7, 32, 1, 15};
    CORRADE_COMPARE(c << 2, (Vector4i{28, 128, 4, 60}));
    {
        Vector4i v{7, 32, 1, 15};
        CORRADE_COMPARE(&(v <<= 2), &v);
        CORRADE_COMPARE(v, (Vector4i{28, 128, 4, 60}));
    }

    CORRADE_COMPARE(c >> 2, (Vector4i{1, 8, 0, 3}));
    {
        Vector4i v{7, 32, 1, 15};
        CORRADE_COMPARE(&(v >>= 2), &v);
        CORRADE_COMPARE(v, (Vector4i{1, 8, 0, 3}));
    }

    constexpr Vector4i cc{7, 32, 1, 15};
    constexpr Vector4i ch = cc << 2;
    constexpr Vector4i ci = cc >> 2;
    CORRADE_COMPARE(ch, (Vector4i{28, 128, 4, 60}));
    CORRADE_COMPARE(ci, (Vector4i{1, 8, 0, 3}));
}

void VectorTest::dot() {
    CORRADE_COMPARE(Math::dot(Vector4{1.0f, 0.5f, 0.75f, 1.5f}, {2.0f, 4.0f, 1.0f, 7.0f}), 15.25f);
}

void VectorTest::dotSelf() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).dot(), 30.0f);
}

void VectorTest::length() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).length(), 5.4772256f);
}

void VectorTest::lengthInverted() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).lengthInverted(), 0.182574f);
}

void VectorTest::normalized() {
    const auto vec = Vector4(1.0f, 1.0f, 1.0f, 1.0f).normalized();
    CORRADE_COMPARE(vec, Vector4(0.5f, 0.5f, 0.5f, 0.5f));
    CORRADE_COMPARE(vec.length(), 1.0f);
}

void VectorTest::resized() {
    const auto vec = Vector4(2.0f, 2.0f, 0.0f, 1.0f).resized(9.0f);
    CORRADE_COMPARE(vec, Vector4(6.0f, 6.0f, 0.0f, 3.0f));
    CORRADE_COMPARE(vec.length(), 9.0f);
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

void VectorTest::max() {
    /* Check also that initial value isn't initialized to 0 */
    CORRADE_COMPARE(Vector3(-1.0f, -2.0f, -3.0f).max(), -1.0f);
}

void VectorTest::minmax() {
    const auto expected = Containers::pair(-3.0f, 2.0f);
    CORRADE_COMPARE((Vector3{-1.0f, 2.0f, -3.0f}.minmax()), expected);
    CORRADE_COMPARE((Vector3{-1.0f, -3.0f, 2.0f}.minmax()), expected);
    CORRADE_COMPARE((Vector3{2.0f, -1.0f, -3.0f}.minmax()), expected);
    CORRADE_COMPARE((Vector3{2.0f, -3.0f, -1.0f}.minmax()), expected);
    CORRADE_COMPARE((Vector3{-3.0f, 2.0f, -1.0f}.minmax()), expected);
    CORRADE_COMPARE((Vector3{-3.0f, -1.0f, 2.0f}.minmax()), expected);
}

void VectorTest::nanIgnoring() {
    Vector3 oneNan{1.0f, Constants::nan(), -3.0f};
    Vector3 firstNan{Constants::nan(), 1.0f, -3.0f};
    Vector3 allNan{Constants::nan(), Constants::nan(), Constants::nan()};

    CORRADE_COMPARE(oneNan.min(), -3.0f);
    CORRADE_COMPARE(firstNan.min(), -3.0f);
    CORRADE_COMPARE(allNan.min(), Constants::nan());

    CORRADE_COMPARE(oneNan.max(), 1.0f);
    CORRADE_COMPARE(firstNan.max(), 1.0f);
    CORRADE_COMPARE(allNan.max(), Constants::nan());

    CORRADE_COMPARE(oneNan.minmax(), Containers::pair(-3.0f, 1.0f));
    CORRADE_COMPARE(firstNan.minmax(), Containers::pair(-3.0f, 1.0f));
    /* Need to compare this way because of NaNs */
    CORRADE_COMPARE(allNan.minmax().first(), Constants::nan());
    CORRADE_COMPARE(allNan.minmax().second(), Constants::nan());
}

void VectorTest::projected() {
    Vector3 line(1.0f, -1.0f, 0.5f);
    Vector3 projected = Vector3(1.0f, 2.0f, 3.0f).projected(line);

    CORRADE_COMPARE(projected, Vector3(0.222222f, -0.222222f, 0.111111f));
    CORRADE_COMPARE(projected.normalized(), line.normalized());
}

void VectorTest::projectedOntoNormalized() {
    Vector3 vector(1.0f, 2.0f, 3.0f);
    Vector3 line(1.0f, -1.0f, 0.5f);

    Vector3 projected = vector.projectedOntoNormalized(line.normalized());
    CORRADE_COMPARE(projected, Vector3(0.222222f, -0.222222f, 0.111111f));
    CORRADE_COMPARE(projected.normalized(), line.normalized());
    CORRADE_COMPARE(projected, vector.projected(line));
}

void VectorTest::projectedOntoNormalizedNotNormalized() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Vector3 vector(1.0f, 2.0f, 3.0f);
    Vector3 line(1.0f, -1.0f, 0.5f);

    Containers::String out;
    Error redirectError{&out};
    vector.projectedOntoNormalized(line);
    CORRADE_COMPARE(out, "Math::Vector::projectedOntoNormalized(): line Vector(1, -1, 0.5) is not normalized\n");
}

void VectorTest::flipped() {
    constexpr Vector4 vector{1.0f, -3.5f, 2.1f, 0.5f};
    constexpr Vector4 flipped = vector.flipped();
    CORRADE_COMPARE(flipped, (Vector4{0.5f, 2.1f, -3.5f, 1.0f}));
}

void VectorTest::angle() {
    auto a = Vector3{2.0f,  3.0f, 4.0f}.normalized();
    auto b = Vector3{1.0f, -2.0f, 3.0f}.normalized();
    CORRADE_COMPARE(Math::angle(a, b), 1.162514_radf);
    CORRADE_COMPARE(Math::angle(-a, -b), 1.162514_radf);
    CORRADE_COMPARE(Math::angle(-a, b), Rad(180.0_degf) - 1.162514_radf);
    CORRADE_COMPARE(Math::angle(a, -b), Rad(180.0_degf) - 1.162514_radf);

    /* Same / opposite. Well, almost. It's interesting how imprecise
       normalization can get. */
    CORRADE_COMPARE_WITH(Math::angle(a, a), 0.0_radf,
        TestSuite::Compare::around(0.0005_radf));
    CORRADE_COMPARE_WITH(Math::angle(a, -a), 180.0_degf,
        TestSuite::Compare::around(0.0005_radf));
}

void VectorTest::angleNormalizedButOver1() {
    /* This vector *is* normalized, but its length is larger than 1, which
       would cause acos() to return a NaN. Ensure it's clamped to correct range
       before passing it there. */
    Vector3 a{1.0f + Math::TypeTraits<Float>::epsilon()/2,  0.0f, 0.0f};
    CORRADE_VERIFY(a.isNormalized());

    CORRADE_COMPARE(Math::angle(a, a), 0.0_radf);
    CORRADE_COMPARE(Math::angle(a, -a), 180.0_degf);
}

void VectorTest::angleNotNormalized() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    Math::angle(Vector3(2.0f, 3.0f, 4.0f).normalized(), {1.0f, -2.0f, 3.0f});
    Math::angle({2.0f, 3.0f, 4.0f}, Vector3(1.0f, -2.0f, 3.0f).normalized());

    CORRADE_COMPARE(out,
        "Math::angle(): vectors Vector(0.371391, 0.557086, 0.742781) and Vector(1, -2, 3) are not normalized\n"
        "Math::angle(): vectors Vector(2, 3, 4) and Vector(0.267261, -0.534522, 0.801784) are not normalized\n");
}

template<class T> class BasicVec2: public Math::Vector<2, T> {
    public:
        template<class ...U> constexpr BasicVec2(U&&... args): Math::Vector<2, T>{args...} {}

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(2, BasicVec2)
};

#ifdef CORRADE_MSVC2015_COMPATIBILITY
MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(2, BasicVec2)
#endif

typedef BasicVec2<Float> Vec2;
typedef BasicVec2<Int> Vec2i;

void VectorTest::subclassTypes() {
    Float* const data = nullptr;
    const Float* const cdata = nullptr;
    CORRADE_VERIFY(std::is_same<decltype(Vec2::from(data)), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(Vec2::from(cdata)), const Vec2&>::value);

    Vector<1, Float> one;
    CORRADE_VERIFY(std::is_same<decltype(Vec2::pad(one)), Vec2>::value);

    /* Const operators */
    const Vec2 c;
    const Vec2 c2;
    CORRADE_VERIFY(std::is_same<decltype(+c), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(-c), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c + c), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c*1.0f), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f*c), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c/1.0f), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f/c), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c*c2), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c/c2), Vec2>::value);

    /* Assignment operators */
    Vec2 a;
    CORRADE_VERIFY(std::is_same<decltype(a = c), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a += c), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a -= c), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a *= 1.0f), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a /= 1.0f), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a *= c), Vec2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a /= c), Vec2&>::value);

    /* Modulo operations */
    const Vec2i ci;
    Vec2i i;
    const Int j = {};
    CORRADE_VERIFY(std::is_same<decltype(ci % j), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(i %= j), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci % ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(i %= ci), Vec2i&>::value);

    /* Bitwise operations */
    CORRADE_VERIFY(std::is_same<decltype(~ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci & ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci | ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci ^ ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci << 1), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci >> 1), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(i &= ci), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(i |= ci), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(i ^= ci), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(i <<= 1), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(i >>= 1), Vec2i&>::value);

    /* Integer multiplication/division */
    CORRADE_VERIFY(std::is_same<decltype(ci*1.0f), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f*ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(c*ci), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci*c), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(ci/c), Vec2i>::value);
    CORRADE_VERIFY(std::is_same<decltype(i *= c), Vec2i&>::value);
    CORRADE_VERIFY(std::is_same<decltype(i /= c), Vec2i&>::value);

    /* Functions */
    CORRADE_VERIFY(std::is_same<decltype(c.normalized()), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c.resized(1.0f)), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c.projected(c2)), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c.projectedOntoNormalized(c2)), Vec2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c.flipped()), Vec2>::value);
}

void VectorTest::subclass() {
    Float data[] = {1.0f, -2.0f};
    CORRADE_COMPARE(Vec2::from(data), Vec2(1.0f, -2.0f));

    const Float cdata[] = {1.0f, -2.0f};
    CORRADE_COMPARE(Vec2::from(cdata), Vec2(1.0f, -2.0f));

    {
        const Vector<1, Float> a = 5.0f;
        Vec2 b = Vec2::pad(a);
        Vec2 c = Vec2::pad(a, -1.0f);
        CORRADE_COMPARE(b, Vec2(5.0f, 0.0f));
        CORRADE_COMPARE(c, Vec2(5.0f, -1.0f));

        constexpr Vector<1, Float> ca = 5.0f;
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
        constexpr
        #endif
        Vec2 cb = Vec2::pad(ca);
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
        constexpr
        #endif
        Vec2 cc = Vec2::pad(ca, -1.0f);
        CORRADE_COMPARE(cb, Vec2(5.0f, 0.0f));
        CORRADE_COMPARE(cc, Vec2(5.0f, -1.0f));
    }

    /* Unary operators */
    CORRADE_COMPARE(+Vec2(-2.0f, 5.0f), Vec2(-2.0f, 5.0f));
    {
        constexpr Vec2 ca{-2.0f, 5.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
        constexpr
        #endif
        Vec2 cb = +ca;
        CORRADE_COMPARE(cb, ca);
    }

    CORRADE_COMPARE(-Vec2(-2.0f, 5.0f), Vec2(2.0f, -5.0f));
    {
        constexpr Vec2 ca{-2.0f, 5.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cb = -ca;
        CORRADE_COMPARE(cb, (Vec2{2.0f, -5.0f}));
    }

    /* Addition / subtraction */
    CORRADE_COMPARE(Vec2(-2.0f, 5.0f) + Vec2(1.0f, -3.0f), Vec2(-1.0f, 2.0f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a += Vec2{1.0f, -3.0f}), &a);
        CORRADE_COMPARE(a, (Vec2{-1.0f, 2.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        constexpr Vec2 cb{1.0f, -3.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cc = ca + cb;
        CORRADE_COMPARE(cc, (Vec2{-1.0f, 2.0f}));
    }

    CORRADE_COMPARE(Vec2(-2.0f, 5.0f) - Vec2(1.0f, -3.0f), Vec2(-3.0f, 8.0f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a -= Vec2{1.0f, -3.0f}), &a);
        CORRADE_COMPARE(a, (Vec2{-3.0f, 8.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        constexpr Vec2 cb{1.0f, -3.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cc = ca - cb;
        CORRADE_COMPARE(cc, (Vec2{-3.0f, 8.0f}));
    }

    /* Multiplication and division with a scalar */
    CORRADE_COMPARE(Vec2(-2.0f, 5.0f)*2.0f, Vec2(-4.0f, 10.0f));
    CORRADE_COMPARE(2.0f*Vec2(-2.0f, 5.0f), Vec2(-4.0f, 10.0f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a *= 2.0f), &a);
        CORRADE_COMPARE(a, (Vec2{-4.0f, 10.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cb1 = ca*2.0f;
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cb2 = 2.0f*ca;
        CORRADE_COMPARE(cb1, (Vec2{-4.0f, 10.0f}));
        CORRADE_COMPARE(cb2, (Vec2{-4.0f, 10.0f}));
    }

    CORRADE_COMPARE(Vec2(-2.0f, 5.0f)/0.5f, Vec2(-4.0f, 10.0f));
    CORRADE_COMPARE(2.0f/Vec2(-2.0f, 5.0f), Vec2(-1.0f, 0.4f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a /= 0.5f), &a);
        CORRADE_COMPARE(a, (Vec2{-4.0f, 10.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cb1 = ca/0.5f;
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cb2 = 2.0f/ca;
        CORRADE_COMPARE(cb1, (Vec2{-4.0f, 10.0f}));
        CORRADE_COMPARE(cb2, (Vec2{-1.0f, 0.4f}));
    }

    /* Multiplication/division with an integer scalar */
    CORRADE_COMPARE(Vec2i(2, 4)*1.5f, Vec2i(3, 6));
    CORRADE_COMPARE(1.5f*Vec2i(2, 4), Vec2i(3, 6));
    {
        Vec2i a{2, 4};
        CORRADE_COMPARE(&(a *= 1.5f), &a);
        CORRADE_COMPARE(a, (Vec2i{3, 6}));

        constexpr Vec2i ca{2, 4};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb1 = ca*1.5f;
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb2 = 1.5f*ca;
        CORRADE_COMPARE(cb1, (Vec2i{3, 6}));
        CORRADE_COMPARE(cb2, (Vec2i{3, 6}));
    }

    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(Vec2i(2, 4)/(2.0f/3.0f) == Vec2i(2, 5),
            "Emscripten -O1 misoptimizes the following (-O2 works).");
        #endif
        CORRADE_COMPARE(Vec2i(2, 4)/(2.0f/3.0f), Vec2i(3, 6));
    } {
        Vec2i a{2, 4};
        CORRADE_COMPARE(&(a /= (2.0f/3.0f)), &a);
        CORRADE_COMPARE(a, (Vec2i{3, 6}));

        constexpr Vec2i ca{2, 4};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb = ca/(2.0f/3.0f);
        CORRADE_COMPARE(cb, (Vec2i{3, 6}));
    }

    /* Multiplication and division with a vector */
    CORRADE_COMPARE(Vec2(-2.0f, 5.0f)*Vec2(1.5f, -2.0f), Vec2(-3.0f, -10.0f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a *= Vec2{1.5f, -2.0f}), &a);
        CORRADE_COMPARE(a, (Vec2{-3.0f, -10.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        constexpr Vec2 cb{1.5f, -2.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cc = ca*cb;
        CORRADE_COMPARE(cc, (Vec2{-3.0f, -10.0f}));
    }

    CORRADE_COMPARE(Vec2(-2.0f, 5.0f)/Vec2(2.0f/3.0f, -0.5f), Vec2(-3.0f, -10.0f));
    {
        Vec2 a{-2.0f, 5.0f};
        CORRADE_COMPARE(&(a /= Vec2{2.0f/3.0f, -0.5f}), &a);
        CORRADE_COMPARE(a, (Vec2{-3.0f, -10.0f}));

        constexpr Vec2 ca{-2.0f, 5.0f};
        constexpr Vec2 cb{2.0f/3.0f, -0.5f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2 cc = ca/cb;
        CORRADE_COMPARE(cc, (Vec2{-3.0f, -10.0f}));
    }

    /* Multiplication/division with an integer vector */
    CORRADE_COMPARE(Vec2i(2, 4)*Vec2(-1.5f, 0.5f), Vec2i(-3, 2));
    CORRADE_COMPARE(Vec2(-1.5f, 0.5f)*Vec2i(2, 4), Vec2i(-3, 2));
    {
        Vec2i a{2, 4};
        CORRADE_COMPARE(&(a *= Vec2{-1.5f, 0.5f}), &a);
        CORRADE_COMPARE(a, (Vec2i{-3, 2}));

        constexpr Vec2i ca{2, 4};
        constexpr Vec2 cb{-1.5f, 0.5f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc1 = ca*cb;
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc2 = cb*ca;
        CORRADE_COMPARE(cc1, (Vec2i{-3, 2}));
        CORRADE_COMPARE(cc2, (Vec2i{-3, 2}));
    }

    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF(Vec2i(2, 4)/Vec2(-2.0f/3.0f, 2.0f) == Vec2i(-2, 2),
            "Emscripten -O1 misoptimizes the following (-O2 works).");
        #endif
        CORRADE_COMPARE(Vec2i(2, 4)/Vec2(-2.0f/3.0f, 2.0f), Vec2i(-3, 2));
    } {
        Vec2i a{2, 4};
        CORRADE_COMPARE(&(a /= Vec2{-2.0f/3.0f, 2.0f}), &a);
        CORRADE_COMPARE(a, (Vec2i{-3, 2}));

        constexpr Vec2i ca{2, 4};
        constexpr Vec2 cb{-2.0f/3.0f, 2.0f};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc = ca/cb;
        CORRADE_COMPARE(cc, (Vec2i{-3, 2}));
    }

    /* Modulo operations */
    CORRADE_COMPARE(Vec2i(4, 13) % 2, Vec2i(0, 1));
    {
        Vec2i a{4, 13};
        CORRADE_COMPARE(&(a %= 2), &a);
        CORRADE_COMPARE(a, (Vec2i{0, 1}));

        constexpr Vec2i ca{4, 13};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb = ca % 2;
        CORRADE_COMPARE(cb, (Vec2i{0, 1}));
    }

    CORRADE_COMPARE(Vec2i(4, 13) % Vec2i(2, 5), Vec2i(0, 3));
    {
        Vec2i a{4, 13};
        CORRADE_COMPARE(&(a %= Vec2i{2, 5}), &a);
        CORRADE_COMPARE(a, (Vec2i{0, 3}));

        constexpr Vec2i ca{4, 13};
        constexpr Vec2i cb{2, 5};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc = ca % cb;
        CORRADE_COMPARE(cc, (Vec2i{0, 3}));
    }

    /* Unary bitwise operations */
    CORRADE_COMPARE(~Vec2i(85, 240), Vec2i(-86, -241));
    {
        constexpr Vec2i ca{85, 240};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb = ~ca;
        CORRADE_COMPARE(cb, (Vec2i{-86, -241}));
    }

    /* Bitwise AND, OR and XOR */
    CORRADE_COMPARE(Vec2i(85, 240) & Vec2i(170, 85), Vec2i(0, 80));
    {
        Vec2i a{85, 240};
        CORRADE_COMPARE(&(a &= Vec2i{170, 85}), &a);
        CORRADE_COMPARE(a, (Vec2i{0, 80}));

        constexpr Vec2i ca{85, 240};
        constexpr Vec2i cb{170, 85};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc = ca & cb;
        CORRADE_COMPARE(cc, (Vec2i{0, 80}));
    }

    CORRADE_COMPARE(Vec2i(85, 240) | Vec2i(170, 85), Vec2i(255, 245));
    {
        Vec2i a{85, 240};
        CORRADE_COMPARE(&(a |= Vec2i{170, 85}), &a);
        CORRADE_COMPARE(a, (Vec2i{255, 245}));

        constexpr Vec2i ca{85, 240};
        constexpr Vec2i cb{170, 85};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc = ca | cb;
        CORRADE_COMPARE(cc, (Vec2i{255, 245}));
    }

    CORRADE_COMPARE(Vec2i(85, 240) ^ Vec2i(170, 85), Vec2i(255, 165));
    {
        Vec2i a{85, 240};
        CORRADE_COMPARE(&(a ^= Vec2i{170, 85}), &a);
        CORRADE_COMPARE(a, (Vec2i{255, 165}));

        constexpr Vec2i ca{85, 240};
        constexpr Vec2i cb{170, 85};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cc = ca ^ cb;
        CORRADE_COMPARE(cc, (Vec2i{255, 165}));
    }

    /* Bit shift */
    CORRADE_COMPARE(Vec2i(7, 32) << 2, Vec2i(28, 128));
    {
        Vec2i a{7, 32};
        CORRADE_COMPARE(&(a <<= 2), &a);
        CORRADE_COMPARE(a, (Vec2i{28, 128}));

        constexpr Vec2i ca{7, 32};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb = ca << 2;
        CORRADE_COMPARE(cb, (Vec2i{28, 128}));
    }

    CORRADE_COMPARE(Vec2i(7, 32) >> 2, Vec2i(1, 8));
    {
        Vec2i a{7, 32};
        CORRADE_COMPARE(&(a >>= 2), &a);
        CORRADE_COMPARE(a, (Vec2i{1, 8}));

        constexpr Vec2i ca{7, 32};
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* No idea? */
        constexpr
        #endif
        Vec2i cb = ca >> 2;
        CORRADE_COMPARE(cb, (Vec2i{1, 8}));
    }

    /* Functions */
    CORRADE_COMPARE(Vec2(3.0f, 0.0f).normalized(), Vec2(1.0f, 0.0f));
    CORRADE_COMPARE(Vec2(3.0f, 0.0f).resized(6.0f), Vec2(6.0f, 0.0f));
    CORRADE_COMPARE(Vec2(1.0f, 1.0f).projected({0.0f, 2.0f}), Vec2(0.0f, 1.0f));
    CORRADE_COMPARE(Vec2(1.0f, 1.0f).projectedOntoNormalized({0.0f, 1.0f}), Vec2(0.0f, 1.0f));
    CORRADE_COMPARE((Vec2{1.0f, 0.4f}).flipped(), (Vec2{0.4f, 1.0f}));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vec2 cflipped = Vec2{1.0f, 0.4f}.flipped();
    CORRADE_COMPARE(cflipped, (Vec2{0.4f, 1.0f}));
}

void VectorTest::strictWeakOrdering() {
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

void VectorTest::debug() {
    Containers::String out;
    Debug{&out} << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(out, "Vector(0.5, 15, 1, 1)\n");

    out = {};
    Debug{&out} << "a" << Vector4() << "b" << Vector4();
    CORRADE_COMPARE(out, "a Vector(0, 0, 0, 0) b Vector(0, 0, 0, 0)\n");
}

void VectorTest::debugPacked() {
    Containers::String out;
    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << Vector4(0.5f, 15.0f, 1.0f, 1.0f) << Vector4();
    CORRADE_COMPARE(out, "{0.5, 15, 1, 1} Vector(0, 0, 0, 0)\n");
}

void VectorTest::debugPropagateFlags() {
    Containers::String out;
    /* The modifier shouldn't become persistent for values after. The nospace
       modifier shouldn't get propagated. */
    Debug{&out} << ">" << Debug::nospace  << Debug::hex << Vector2i(0xab, 0xcd) << Vector2i(12, 13);
    CORRADE_COMPARE(out, ">Vector(0xab, 0xcd) Vector(12, 13)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::VectorTest)
