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

#include "Magnum/Math/RectangularMatrix.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Mat2x3 {
    float a[6];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<2, 3, Float, Mat2x3> {
    constexpr static RectangularMatrix<2, 3, Float> from(const Mat2x3& other) {
        return RectangularMatrix<2, 3, Float>(
            Vector<3, Float>(other.a[0], other.a[1], other.a[2]),
            Vector<3, Float>(other.a[3], other.a[4], other.a[5]));
    }

    constexpr static Mat2x3 to(const RectangularMatrix<2, 3, Float>& other) {
        return Mat2x3{{other[0][0], other[0][1], other[0][2],
                       other[1][0], other[1][1], other[1][2]}};
    }
};

}

namespace Test { namespace {

struct RectangularMatrixTest: TestSuite::Tester {
    explicit RectangularMatrixTest();

    void construct();
    void constructZero();
    void constructIdentity();
    void constructNoInit();
    void constructOneValue();
    void constructOneComponent();
    void constructArray();
    void constructArrayRvalue();
    void constructConversion();
    void constructFromDifferentSize();
    void constructFromData();
    void constructFromDiagonal();
    void constructCopy();
    void convert();

    void data();
    void row();

    void compare();
    void compareComponentWise();

    void promotedNegated();
    void addSubtract();
    void multiplyDivide();
    void multiply();
    void multiplyVector();
    void multiplyRowVector();

    void transposed();
    void flippedCols();
    void flippedRows();
    void diagonal();

    void vector();

    void subclassTypes();
    void subclass();

    void strictWeakOrdering();

    void debug();
    void debugPacked();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Matrix4x3;
using Magnum::Matrix4x2;
using Magnum::Matrix3x4;
typedef RectangularMatrix<3, 3, Float> Matrix3x3;
using Magnum::Matrix3x2;
typedef RectangularMatrix<2, 2, Float> Matrix2x2;
using Magnum::Matrix2x3;
using Magnum::Matrix2x4;
typedef RectangularMatrix<2, 2, Int> Matrix2x2i;
typedef Vector<4, Float> Vector4;
typedef Vector<3, Float> Vector3;
typedef Vector<2, Float> Vector2;
using Magnum::BitVector3;

typedef Math::Matrix3x1<Int> Matrix3x1i;
typedef Math::Matrix4x1<Int> Matrix4x1i;
typedef Math::Matrix4x3<Int> Matrix4x3i;
typedef Math::Matrix3x4<Int> Matrix3x4i;
typedef Vector<4, Int> Vector4i;
typedef Vector<3, Int> Vector3i;
typedef Vector<2, Int> Vector2i;

RectangularMatrixTest::RectangularMatrixTest() {
    addTests({&RectangularMatrixTest::construct,
              &RectangularMatrixTest::constructZero,
              &RectangularMatrixTest::constructIdentity,
              &RectangularMatrixTest::constructNoInit,
              &RectangularMatrixTest::constructOneValue,
              &RectangularMatrixTest::constructOneComponent,
              &RectangularMatrixTest::constructArray,
              &RectangularMatrixTest::constructArrayRvalue,
              &RectangularMatrixTest::constructConversion,
              &RectangularMatrixTest::constructFromDifferentSize,
              &RectangularMatrixTest::constructFromData,
              &RectangularMatrixTest::constructFromDiagonal,
              &RectangularMatrixTest::constructCopy,
              &RectangularMatrixTest::convert,

              &RectangularMatrixTest::data,
              &RectangularMatrixTest::row,

              &RectangularMatrixTest::compare,
              &RectangularMatrixTest::compareComponentWise,

              &RectangularMatrixTest::promotedNegated,
              &RectangularMatrixTest::addSubtract,
              &RectangularMatrixTest::multiplyDivide,
              &RectangularMatrixTest::multiply,
              &RectangularMatrixTest::multiplyVector,
              &RectangularMatrixTest::multiplyRowVector,

              &RectangularMatrixTest::transposed,
              &RectangularMatrixTest::flippedCols,
              &RectangularMatrixTest::flippedRows,
              &RectangularMatrixTest::diagonal,

              &RectangularMatrixTest::vector,

              &RectangularMatrixTest::subclassTypes,
              &RectangularMatrixTest::subclass,

              &RectangularMatrixTest::strictWeakOrdering,

              &RectangularMatrixTest::debug,
              &RectangularMatrixTest::debugPacked});
}

void RectangularMatrixTest::construct() {
    constexpr Matrix3x4 a = {Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                             Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                             Vector4(9.0f, 10.0f, 11.0f, 12.0f)};
    CORRADE_COMPARE(a, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                 Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                 Vector4(9.0f, 10.0f, 11.0f, 12.0f)));

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix3x4, Vector4, Vector4, Vector4>::value);
}

void RectangularMatrixTest::constructZero() {
    constexpr Matrix4x3 a;
    constexpr Matrix4x3 b{ZeroInit};
    CORRADE_COMPARE(a, Matrix4x3(Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f)));
    CORRADE_COMPARE(b, Matrix4x3(Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f)));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Matrix4x3>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4x3, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Matrix4x3>::value);
}

void RectangularMatrixTest::constructIdentity() {
    constexpr Matrix4x3 a{IdentityInit};
    constexpr Matrix4x3 b{IdentityInit, 4.0f};
    CORRADE_COMPARE(a, (Matrix4x3{Vector3{1.0f, 0.0f, 0.0f},
                                  Vector3{0.0f, 1.0f, 0.0f},
                                  Vector3{0.0f, 0.0f, 1.0f},
                                  Vector3{0.0f, 0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (Matrix4x3{Vector3{4.0f, 0.0f, 0.0f},
                                  Vector3{0.0f, 4.0f, 0.0f},
                                  Vector3{0.0f, 0.0f, 4.0f},
                                  Vector3{0.0f, 0.0f, 0.0f}}));

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4x3, IdentityInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<IdentityInitT, Matrix4x3>::value);
}

void RectangularMatrixTest::constructNoInit() {
    Matrix3x4 a{Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                Vector4(9.0f, 10.0f, 11.0f, 12.0f)};
    new(&a) Matrix3x4{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                     Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                     Vector4(9.0f, 10.0f, 11.0f, 12.0f)));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix3x4, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Matrix3x4>::value);
}

void RectangularMatrixTest::constructOneValue() {
    constexpr Matrix3x4 a{1.5f};
    CORRADE_COMPARE(a, (Matrix3x4{Vector4{1.5f, 1.5f, 1.5f, 1.5f},
                                  Vector4{1.5f, 1.5f, 1.5f, 1.5f},
                                  Vector4{1.5f, 1.5f, 1.5f, 1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Matrix3x4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix3x4, Float>::value);
}

void RectangularMatrixTest::constructOneComponent() {
    typedef Math::RectangularMatrix<1, 1, Float> Matrix1x1;
    typedef Math::Vector<1, Float> Vector1;

    constexpr Matrix1x1 a{1.5f};
    constexpr Matrix1x1 b{Vector1{1.5f}};
    CORRADE_COMPARE(a, b);

    /* Implicit constructor must work */
    constexpr Matrix1x1 c = Vector1{1.5f};
    CORRADE_COMPARE(c, Matrix1x1{Vector1{1.5f}});

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix1x1, Vector1>::value);
}

void RectangularMatrixTest::constructArray() {
    float data[2][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
    };
    Matrix2x4 a{data};
    CORRADE_COMPARE(a, (Matrix2x4{Vector4{3.0f, 5.0f, 8.0f, -3.0f},
                                  Vector4{4.5f, 4.0f, 7.0f,  2.0f}}));

    constexpr float cdata[2][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
    };
    constexpr Matrix2x4 ca{cdata};
    CORRADE_COMPARE(ca, (Matrix2x4{Vector4{3.0f, 5.0f, 8.0f, -3.0f},
                                   Vector4{4.5f, 4.0f, 7.0f,  2.0f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[2][4], Matrix2x4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix2x4, float[2][4]>::value);

    /* It should always be constructible only with exactly the matching number
       of elements. As that's checked with a static_assert(), it's impossible
       to verify with std::is_constructible unfortunately and the only way to
       test that is manually, thus uncomment the code below to test the error
       behavior.

       Additionally, to avoid noise in the compiler output, the second and
       fourth should only produce "excess elements in array initializer" and a
       static assert, the first and third just a static assert, no other
       compiler error. */
    #if 0
    float data23[2][3]{
        {3.0f, 5.0f, 8.0f},
        {4.5f, 4.0f, 7.0f}
    };
    float data25[2][5]{
        {3.0f, 5.0f, 8.0f, -3.0f, 17.0f},
        {4.5f, 4.0f, 7.0f,  2.0f, 23.2f}
    };
    float data14[1][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
    };
    float data34[3][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
        {3.0f, 7.0f, 0.0f,  1.0f}
    };
    Matrix2x4 b{data23};
    Matrix2x4 c{data25};
    Matrix2x4 d{data14};
    Matrix2x4 e{data34};
    #endif
}

void RectangularMatrixTest::constructArrayRvalue() {
    /* This, the extra {} to supply an array, avoids the need to have to
       explicitly type out Vector for every column */
    Matrix2x4 a{{{3.0f, 5.0f, 8.0f, -3.0f},
                 {4.5f, 4.0f, 7.0f,  2.0f}}};
    CORRADE_COMPARE(a, (Matrix2x4{Vector4{3.0f, 5.0f, 8.0f, -3.0f},
                                  Vector4{4.5f, 4.0f, 7.0f,  2.0f}}));

    /* Unfortunately on MSVC (even 2022) this leads to
        error C2131: expression did not evaluate to a constant
        note: failure was caused by out of range index 3; allowed range is 0 <= index < 2
       and similarly in other tests. Not sure where that comes from, for Vector
       this all works, constructArray() above works, and the GCC 4.8 workaround
       with fixed size doesn't help here. */
    #ifndef CORRADE_TARGET_MSVC
    constexpr Matrix2x4 ca{{{3.0f, 5.0f, 8.0f, -3.0f},
                            {4.5f, 4.0f, 7.0f,  2.0f}}};
    CORRADE_COMPARE(ca, (Matrix2x4{Vector4{3.0f, 5.0f, 8.0f, -3.0f},
                                   Vector4{4.5f, 4.0f, 7.0f,  2.0f}}));
    #endif

    /* It should always be constructible only with exactly the matching number
       of elements. As that's checked with a static_assert(), it's impossible
       to verify with std::is_constructible unfortunately and the only way to
       test that is manually, thus uncomment the code below to test the error
       behavior.

       Additionally, to avoid noise in the compiler output, the first and
       second should only produce "excess elements in array initializer" and a
       static assert, the third and fourth a static assert, no other compiler
       error. */
    #if 0
    Matrix2x4 c{{{3.0f, 5.0f, 8.0f, -3.0f, 17.0f},
                 {4.5f, 4.0f, 7.0f,  2.0f, 23.2f}}};
    Matrix2x4 e{{{3.0f, 5.0f, 8.0f, -3.0f},
                 {4.5f, 4.0f, 7.0f,  2.0f},
                 {3.0f, 7.0f, 0.0f,  1.0f}}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a RectangularMatrix from a smaller array isn't an error on GCC 4.8.");
    Matrix2x4 b{{{3.0f, 5.0f, 8.0f},
                 {4.5f, 4.0f, 7.0f}}};
    Matrix2x4 d{{{3.0f, 5.0f, 8.0f, -3.0f}}};
    #endif
}

void RectangularMatrixTest::constructConversion() {
    constexpr Matrix2x2 a(Vector2(  1.3f, 2.7f),
                          Vector2(-15.0f, 7.0f));
    constexpr Matrix2x2i b(a);

    CORRADE_COMPARE(b, Matrix2x2i(Vector2i(  1, 2),
                                  Vector2i(-15, 7)));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Matrix2x2, Matrix2x2i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix2x2, Matrix2x2i>::value);
}

void RectangularMatrixTest::constructFromDifferentSize() {
    /* Test converting to more columns, less rows */
    constexpr Matrix2x4 a{Vector4{3.0f,  5.0f, 8.0f, -3.0f},
                          Vector4{4.5f,  4.0f, 7.0f,  2.0f}};
    constexpr Matrix3x2 aExpected{Vector2{3.0f, 5.0f},
                                  Vector2{4.5f, 4.0f},
                                  Vector2{0.0f, 0.0f}};
    constexpr Matrix3x3 aExpectedZero{Vector3{3.0f, 5.0f, 8.0f},
                                      Vector3{4.5f, 4.0f, 7.0f},
                                      Vector3{0.0f, 0.0f, 0.0f}};
    constexpr Matrix3x3 aExpectedIdentity{Vector3{3.0f, 5.0f, 8.0f},
                                          Vector3{4.5f, 4.0f, 7.0f},
                                          Vector3{0.0f, 0.0f, 0.5f}};

    constexpr Matrix3x2 a2{a};
    CORRADE_COMPARE(a2, aExpected);
    CORRADE_COMPARE(Matrix3x2{a}, aExpected);

    constexpr Matrix3x3 aZero1{a};
    constexpr Matrix3x3 aZero2{ZeroInit, a};
    CORRADE_COMPARE(aZero1, aExpectedZero);
    CORRADE_COMPARE(aZero2, aExpectedZero);
    CORRADE_COMPARE(Matrix3x3{a}, aExpectedZero);
    CORRADE_COMPARE((Matrix3x3{ZeroInit, a}), aExpectedZero);

    constexpr Matrix3x3 aIdentity{IdentityInit, a, 0.5f};
    CORRADE_COMPARE(aIdentity, aExpectedIdentity);
    CORRADE_COMPARE((Matrix3x3{IdentityInit, aIdentity, 0.5f}), aExpectedIdentity);

    /* Test converting to less columns, more rows */
    constexpr Matrix4x2 b{Vector2{3.0f,  5.0f},
                          Vector2{8.0f, -3.0f},
                          Vector2{4.5f,  4.0f},
                          Vector2{7.0f,  2.0f}};
    constexpr Matrix2x3 bExpected{Vector3{3.0f,  5.0f, 0.0f},
                                  Vector3{8.0f, -3.0f, 0.0f}};
    constexpr Matrix3x3 bExpectedZero{Vector3{3.0f,  5.0f, 0.0f},
                                      Vector3{8.0f, -3.0f, 0.0f},
                                      Vector3{4.5f,  4.0f, 0.0f}};
    constexpr Matrix3x3 bExpectedIdentity{Vector3{3.0f,  5.0f, 0.0f},
                                          Vector3{8.0f, -3.0f, 0.0f},
                                          Vector3{4.5f,  4.0f, 0.5f}};

    constexpr Matrix2x3 b2{b};
    CORRADE_COMPARE(b2, bExpected);
    CORRADE_COMPARE(Matrix2x3{b}, bExpected);

    constexpr Matrix3x3 bZero1{b};
    constexpr Matrix3x3 bZero2{ZeroInit, b};
    CORRADE_COMPARE(bZero1, bExpectedZero);
    CORRADE_COMPARE(bZero2, bExpectedZero);
    CORRADE_COMPARE(Matrix3x3{b}, bExpectedZero);
    CORRADE_COMPARE((Matrix3x3{ZeroInit, b}), bExpectedZero);

    constexpr Matrix3x3 bIdentity{IdentityInit, b, 0.5f};
    CORRADE_COMPARE(bIdentity, bExpectedIdentity);
    CORRADE_COMPARE((Matrix3x3{IdentityInit, bIdentity}), bExpectedIdentity);
}

void RectangularMatrixTest::constructFromData() {
    Float m[] = {
        3.0f, 5.0f, 8.0f, 4.0f,
        4.0f, 4.0f, 7.0f, 3.0f,
        7.0f, -1.0f, 8.0f, 0.0f
    };

    Matrix3x4 expected(Vector4(3.0f, 5.0f, 8.0f, 4.0f),
                       Vector4(4.0f, 4.0f, 7.0f, 3.0f),
                       Vector4(7.0f, -1.0f, 8.0f, 0.0f));

    CORRADE_COMPARE(Matrix3x4::from(m), expected);
}

void RectangularMatrixTest::constructFromDiagonal() {
    constexpr Vector3 diagonal(-1.0f, 5.0f, 11.0f);

    constexpr auto a = Matrix3x4::fromDiagonal(diagonal);
    Matrix3x4 expectedA(Vector4(-1.0f, 0.0f,  0.0f, 0.0f),
                        Vector4( 0.0f, 5.0f,  0.0f, 0.0f),
                        Vector4( 0.0f, 0.0f, 11.0f, 0.0f));
    CORRADE_COMPARE(a, expectedA);

    constexpr auto b = Matrix4x3::fromDiagonal(diagonal);
    Matrix4x3 expectedB(Vector3(-1.0f, 0.0f,  0.0f),
                        Vector3( 0.0f, 5.0f,  0.0f),
                        Vector3( 0.0f, 0.0f, 11.0f),
                        Vector3( 0.0f, 0.0f,  0.0f));
    CORRADE_COMPARE(b, expectedB);
}

void RectangularMatrixTest::constructCopy() {
    constexpr Matrix3x4 a(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                          Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                          Vector4(9.0f, 10.0f, 11.0f, 12.0f));
    constexpr Matrix3x4 b(a);
    CORRADE_COMPARE(b, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                 Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                 Vector4(9.0f, 10.0f, 11.0f, 12.0f)));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Matrix3x4>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Matrix3x4>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Matrix3x4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Matrix3x4>::value);
}

void RectangularMatrixTest::convert() {
    constexpr Mat2x3 a{{1.5f,  2.0f, -3.5f,
                        2.0f, -3.1f,  0.4f}};
    constexpr Matrix2x3 b(Vector3(1.5f, 2.0f, -3.5f),
                          Vector3(2.0f, -3.1f,  0.4f));

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Matrix2x3 c{a};
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Mat2x3(b);
    #else
    constexpr Mat2x3 d(b);
    #endif
    for(std::size_t i = 0; i != 5; ++i)
        CORRADE_COMPARE(d.a[i], a.a[i]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Mat2x3, Matrix2x3>::value);
    CORRADE_VERIFY(!std::is_convertible<Matrix2x3, Mat2x3>::value);
}

void RectangularMatrixTest::data() {
    Matrix3x4 a;
    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);

    a[2] = vector;
    a[1][1] = 1.0f;
    a[0][2] = 1.5f;

    CORRADE_COMPARE(a[1][1], 1.0f);
    CORRADE_COMPARE(a[0][2], 1.5f);
    CORRADE_COMPARE(a[2], vector);

    CORRADE_COMPARE(a, Matrix3x4(Vector4(0.0f, 0.0f, 1.5f, 0.0f),
                                 Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                                 Vector4(4.0f, 5.0f, 6.0f, 7.0f)));

    constexpr Matrix3x4 ca(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                           Vector4(4.5f,  4.0f, 7.0f, 3.0f),
                           Vector4(7.0f, -1.7f, 8.0f, 0.0f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector4 b = ca[2];
    constexpr Float c = ca[1][2];
    CORRADE_COMPARE(b, Vector4(7.0f, -1.7f, 8.0f, 0.0f));
    CORRADE_COMPARE(c, 7.0f);

    /* Not constexpr anymore, as it has to reinterpret to return a
       correctly-sized array */
    CORRADE_COMPARE(a.data()[8], 4.0f);
    CORRADE_COMPARE(ca.data()[1], 5.0f);

    /* It actually returns an array */
    CORRADE_COMPARE(Containers::arraySize(a.data()), 12);
    CORRADE_COMPARE(Containers::arraySize(ca.data()), 12);
}

void RectangularMatrixTest::row() {
    Matrix3x4 a(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                Vector4(9.0f, 10.0f, 11.0f, 12.0f));

    CORRADE_COMPARE(a.row(1), Vector3(2.0f, 6.0f, 10.0f));

    a.setRow(1, {-2.1f, -6.1f, -10.1f});
    CORRADE_COMPARE(a, (Matrix3x4{Vector4{1.0f,  -2.1f,  3.0f,  4.0f},
                                  Vector4{5.0f,  -6.1f,  7.0f,  8.0f},
                                  Vector4{9.0f, -10.1f, 11.0f, 12.0f}}));
}

void RectangularMatrixTest::compare() {
    Matrix2x2 a(Vector2(1.0f, -3.0f),
                Vector2(5.0f, -10.0f));
    Matrix2x2 b(Vector2(1.0f + TypeTraits<Float>::epsilon()/2, -3.0f),
                Vector2(5.0f, -10.0f));
    Matrix2x2 c(Vector2(1.0f, -1.0f + TypeTraits<Float>::epsilon()*2),
                Vector2(5.0f, -10.0f));
    CORRADE_VERIFY(a == b);
    CORRADE_VERIFY(a != c);

    Matrix2x2i ai(Vector2i(1, -3),
                  Vector2i(5, -10));
    Matrix2x2i bi(Vector2i(1, -2),
                  Vector2i(5, -10));
    CORRADE_VERIFY(ai == ai);
    CORRADE_VERIFY(ai != bi);
}

void RectangularMatrixTest::compareComponentWise() {
    typedef RectangularMatrix<3, 1, Float> Matrix3x1;
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) < Matrix3x1(1.1f, -1.0f, 3.0f), BitVector3(0x1));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) <= Matrix3x1(1.1f, -1.0f, 3.0f), BitVector3(0x3));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) >= Matrix3x1(1.1f, -1.0f, 3.0f), BitVector3(0x6));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) > Matrix3x1(1.1f, -1.0f, 3.0f), BitVector3(0x4));
}

void RectangularMatrixTest::promotedNegated() {
    Matrix2x2 matrix(Vector2(1.0f,  -3.0f),
                     Vector2(5.0f, -10.0f));
    Matrix2x2 negated(Vector2(-1.0f,  3.0f),
                      Vector2(-5.0f, 10.0f));
    CORRADE_COMPARE(+matrix, matrix);
    CORRADE_COMPARE(-matrix, negated);
}

void RectangularMatrixTest::addSubtract() {
    const Matrix4x3 a{Vector3{0.0f,   1.0f,   3.0f},
                      Vector3{4.0f,   5.0f,   7.0f},
                      Vector3{8.0f,   9.0f,   11.0f},
                      Vector3{12.0f, 13.0f,  15.0f}};
    const Matrix4x3 b{Vector3{-4.0f,  0.5f,   9.0f},
                      Vector3{-9.0f, 11.0f,  0.25f},
                      Vector3{ 0.0f, -8.0f,  19.0f},
                      Vector3{-3.0f, -5.0f,   2.0f}};
    const Matrix4x3 c{Vector3{-4.0f,  1.5f,  12.0f},
                      Vector3{-5.0f, 16.0f,  7.25f},
                      Vector3{ 8.0f,  1.0f,  30.0f},
                      Vector3{ 9.0f,  8.0f,  17.0f}};

    CORRADE_COMPARE(a + b, c);
    {
        Matrix4x3 m{Vector3{0.0f,   1.0f,   3.0f},
                    Vector3{4.0f,   5.0f,   7.0f},
                    Vector3{8.0f,   9.0f,   11.0f},
                    Vector3{12.0f, 13.0f,  15.0f}};
        CORRADE_COMPARE(&(m += b), &m);
        CORRADE_COMPARE(m, c);
    }

    CORRADE_COMPARE(c - b, a);
    {
        Matrix4x3 m{Vector3{-4.0f,  1.5f,  12.0f},
                    Vector3{-5.0f, 16.0f,  7.25f},
                    Vector3{ 8.0f,  1.0f,  30.0f},
                    Vector3{ 9.0f,  8.0f,  17.0f}};
        CORRADE_COMPARE(&(m -= b), &m);
        CORRADE_COMPARE(m, a);
    }
}

void RectangularMatrixTest::multiplyDivide() {
    const Matrix2x2 matrix{Vector2{1.0f, 2.0f},
                           Vector2{3.0f, 4.0f}};
    const Matrix2x2 multiplied{Vector2{-1.5f, -3.0f},
                               Vector2{-4.5f, -6.0f}};

    CORRADE_COMPARE(matrix*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*matrix, multiplied);
    {
        Matrix2x2 m{Vector2{1.0f, 2.0f},
                    Vector2{3.0f, 4.0f}};
        CORRADE_COMPARE(&(m *= -1.5f), &m);
        CORRADE_COMPARE(m, multiplied);
    }

    CORRADE_COMPARE(multiplied/-1.5f, matrix);
    {
        Matrix2x2 m{Vector2{-1.5f, -3.0f},
                    Vector2{-4.5f, -6.0f}};
        CORRADE_COMPARE(&(m /= -1.5f), &m);
        CORRADE_COMPARE(m, matrix);
    }

    /* Divide vector with number and inverse */
    const Matrix2x2 divisor{Vector2{ 1.0f, 2.0f},
                            Vector2{-4.0f, 8.0f}};
    const Matrix2x2 result{Vector2{  1.0f,   0.5f},
                           Vector2{-0.25f, 0.125f}};
    CORRADE_COMPARE(1.0f/divisor, result);
}

void RectangularMatrixTest::multiply() {
    const RectangularMatrix<4, 6, Int> left{
        Vector<6, Int>{-5,   27, 10,  33, 0, -15},
        Vector<6, Int>{ 7,   56, 66,   1, 0, -24},
        Vector<6, Int>{ 4,   41,  4,   0, 1,  -4},
        Vector<6, Int>{ 9, -100, 19, -49, 1,   9}
    };

    const RectangularMatrix<5, 4, Int> right{
        Vector<4, Int>{1,  -7,  0,  158},
        Vector<4, Int>{2,  24, -3,   40},
        Vector<4, Int>{3, -15, -2,  -50},
        Vector<4, Int>{4,  17, -1, -284},
        Vector<4, Int>{5,  30,  4,   18}
    };

    const RectangularMatrix<5, 6, Int> expected{
       Vector<6, Int>{ 1368, -16165,  2550, -7716,  158,  1575},
       Vector<6, Int>{  506,  -2725,  2352, -1870,   37,  -234},
       Vector<6, Int>{ -578,   4159, -1918,  2534,  -52,  -127},
       Vector<6, Int>{-2461,  29419, -4238, 14065, -285, -3020},
       Vector<6, Int>{  363,    179,  2388,  -687,   22,  -649}
    };

    CORRADE_COMPARE(left*right, expected);

    /* There's no *= for matrix multiplication as it makes operation order
       unclear */
}

void RectangularMatrixTest::multiplyVector() {
    const Vector4i a{-5, 27, 10, 33};
    const RectangularMatrix<3, 1, Int> b{1, 2, 3};
    CORRADE_COMPARE(a*b, (Matrix3x4i{
       Vector4i{ -5,  27, 10,  33},
       Vector4i{-10,  54, 20,  66},
       Vector4i{-15,  81, 30,  99}
    }));

    const Matrix3x4i c{Vector4i{0, 4,  8, 12},
                       Vector4i{1, 5,  9, 13},
                       Vector4i{3, 7, 11, 15}};
    const Vector3i d{2, -2, 3};
    CORRADE_COMPARE(c*d, (Vector4i{7, 19, 31, 43}));

    /* There's no *= for vector and matrix multiplication either */
}

void RectangularMatrixTest::multiplyRowVector() {
    /* Like multiplyVector() above, just transposed */

    const Vector3i d{2, -2, 3};
    const Matrix4x3i c{Vector3i{ 0,  1,  3},
                       Vector3i{ 4,  5,  7},
                       Vector3i{ 8,  9, 11},
                       Vector3i{12, 13, 15}};
    CORRADE_COMPARE(Matrix3x1i::fromVector(d)*c, (Matrix4x1i{
       7, 19, 31, 43
    }));
}

void RectangularMatrixTest::transposed() {
    Matrix4x3 original(Vector3( 0.0f,  1.0f,  3.0f),
                       Vector3( 4.0f,  5.0f,  7.0f),
                       Vector3( 8.0f,  9.0f, 11.0f),
                       Vector3(12.0f, 13.0f, 15.0f));

    Matrix3x4 transposed(Vector4(0.0f, 4.0f,  8.0f, 12.0f),
                         Vector4(1.0f, 5.0f,  9.0f, 13.0f),
                         Vector4(3.0f, 7.0f, 11.0f, 15.0f));

    CORRADE_COMPARE(original.transposed(), transposed);
}

void RectangularMatrixTest::flippedCols() {
    constexpr Matrix4x3 original{Vector3{ 0.0f,  1.0f,  3.0f},
                                 Vector3{ 4.0f,  5.0f,  7.0f},
                                 Vector3{ 8.0f,  9.0f, 11.0f},
                                 Vector3{12.0f, 13.0f, 15.0f}};
    constexpr Matrix4x3 flipped = original.flippedCols();

    Matrix4x3 expectedFlipped{Vector3{12.0f, 13.0f, 15.0f},
                              Vector3{ 8.0f,  9.0f, 11.0f},
                              Vector3{ 4.0f,  5.0f,  7.0f},
                              Vector3{ 0.0f,  1.0f,  3.0f}};

    CORRADE_COMPARE(flipped, expectedFlipped);
}

void RectangularMatrixTest::flippedRows() {
    constexpr Matrix4x3 original{Vector3{ 0.0f,  1.0f,  3.0f},
                                 Vector3{ 4.0f,  5.0f,  7.0f},
                                 Vector3{ 8.0f,  9.0f, 11.0f},
                                 Vector3{12.0f, 13.0f, 15.0f}};
    constexpr Matrix4x3 flipped = original.flippedRows();

    Matrix4x3 expectedFlipped{Vector3{ 3.0f,  1.0f,  0.0f},
                              Vector3{ 7.0f,  5.0f,  4.0f},
                              Vector3{11.0f,  9.0f,  8.0f},
                              Vector3{15.0f, 13.0f, 12.0f}};

    CORRADE_COMPARE(flipped, expectedFlipped);
}

void RectangularMatrixTest::diagonal() {
    Vector3 diagonal(-1.0f, 5.0f, 11.0f);

    constexpr Matrix4x3 a(Vector3(-1.0f,  1.0f,  3.0f),
                          Vector3( 4.0f,  5.0f,  7.0f),
                          Vector3( 8.0f,  9.0f, 11.0f),
                          Vector3(12.0f, 13.0f, 15.0f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 aDiagonal = a.diagonal();
    CORRADE_COMPARE(aDiagonal, diagonal);

    constexpr Matrix3x4 b(Vector4(-1.0f, 4.0f,  8.0f, 12.0f),
                          Vector4( 1.0f, 5.0f,  9.0f, 13.0f),
                          Vector4( 3.0f, 7.0f, 11.0f, 15.0f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 bDiagonal = b.diagonal();
    CORRADE_COMPARE(bDiagonal, diagonal);
}

void RectangularMatrixTest::vector() {
    typedef Vector<12, Int> Vector12i;

    Matrix4x3i a(Vector3i(0, 1, 2),
                 Vector3i(3, 4, 5),
                 Vector3i(6, 7, 8),
                 Vector3i(9, 10, 11));

    Vector12i b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);

    CORRADE_COMPARE(a.toVector(), b);
    CORRADE_COMPARE(Matrix4x3i::fromVector(b), a);
}

template<std::size_t size, class T> class BasicMat: public Math::RectangularMatrix<size, size, T> {
    public:
        template<class ...U> constexpr BasicMat(U&&... args): Math::RectangularMatrix<size, size, T>{args...} {}

        _MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(size, size, BasicMat<size, T>)
};

_MAGNUM_MATRIX_OPERATOR_IMPLEMENTATION(BasicMat<size, T>)

template<class T> class BasicMat2x2: public BasicMat<2, T> {
    public:
        template<class ...U> constexpr BasicMat2x2(U&&... args): BasicMat<2, T>{args...} {}

        _MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(2, 2, BasicMat2x2<T>)
};

_MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(2, BasicMat2x2)

typedef BasicMat2x2<Float> Mat2x2;

void RectangularMatrixTest::subclassTypes() {
    Float* const data = nullptr;
    const Float* const cdata = nullptr;
    CORRADE_VERIFY(std::is_same<decltype(Mat2x2::from(data)), Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(Mat2x2::from(cdata)), const Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(Mat2x2::fromDiagonal({})), Mat2x2>::value);

    /* Const operators */
    const Mat2x2 c;
    CORRADE_VERIFY(std::is_same<decltype(+c), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(-c), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c + c), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c*1.0f), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f*c), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(c/1.0f), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f/c), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(Vector2()*Math::RectangularMatrix<2, 1, Float>()), Mat2x2>::value);

    /* Assignment operators */
    Mat2x2 a;
    CORRADE_VERIFY(std::is_same<decltype(a = c), Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a += c), Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a -= c), Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a *= 1.0f), Mat2x2&>::value);
    CORRADE_VERIFY(std::is_same<decltype(a /= 1.0f), Mat2x2&>::value);

    /* Operators on variable-sized matrix */
    const BasicMat<3, Float> c2;
    CORRADE_VERIFY(std::is_same<decltype(1.0f*c2), BasicMat<3, Float>>::value);
    CORRADE_VERIFY(std::is_same<decltype(1.0f/c2), BasicMat<3, Float>>::value);
    CORRADE_VERIFY(std::is_same<decltype(Vector3()*Math::RectangularMatrix<3, 1, Float>()), BasicMat<3, Float>>::value);

    /* Functions */
    CORRADE_VERIFY(std::is_same<decltype(a.flippedCols()), Mat2x2>::value);
    CORRADE_VERIFY(std::is_same<decltype(a.flippedRows()), Mat2x2>::value);
}

void RectangularMatrixTest::subclass() {
    Float data[] = {1.0f, -2.0f, 3.0f, -4.5f};
    CORRADE_COMPARE(Mat2x2::from(data), Mat2x2(Vector2(1.0f, -2.0f),
                                               Vector2(3.0f, -4.5f)));

    const Float cdata[] = {1.0f, -2.0f, 3.0f, -4.5f};
    CORRADE_COMPARE(Mat2x2::from(cdata), Mat2x2(Vector2(1.0f, -2.0f),
                                                Vector2(3.0f, -4.5f)));

    CORRADE_COMPARE(Mat2x2::fromDiagonal({1.0f, -2.0f}), Mat2x2(Vector2(1.0f,  0.0f),
                                                                Vector2(0.0f, -2.0f)));

    /* Constexpr constructor */
    constexpr Mat2x2 a(Vector2(1.0f, -3.0f),
                       Vector2(-3.0f, 1.0f));
    CORRADE_COMPARE(+a, Mat2x2(Vector2(1.0f, -3.0f),
                               Vector2(-3.0f, 1.0f)));
    CORRADE_COMPARE(-a, Mat2x2(Vector2(-1.0f, 3.0f),
                               Vector2(3.0f, -1.0f)));

    const Mat2x2 b{Vector2{-2.0f, 5.0f},
                   Vector2{5.0f, -2.0f}};
    const Mat2x2 bExpected{Vector2{-1.0f, 2.0f},
                           Vector2{2.0f, -1.0f}};
    CORRADE_COMPARE(b + a, bExpected);
    {
        Mat2x2 m{Vector2{-2.0f, 5.0f},
                 Vector2{5.0f, -2.0f}};
        CORRADE_COMPARE(&(m += a), &m);
        CORRADE_COMPARE(m, bExpected);
    }

    const Mat2x2 c{Vector2{-2.0f, 5.0f},
                   Vector2{5.0f, -2.0f}};
    const Mat2x2 cExpected{Vector2{-3.0f, 8.0f},
                           Vector2{8.0f, -3.0f}};
    CORRADE_COMPARE(c - a, cExpected);
    {
        Mat2x2 m{Vector2{-2.0f, 5.0f},
                 Vector2{5.0f, -2.0f}};
        CORRADE_COMPARE(&(m -= a), &m);
        CORRADE_COMPARE(m, cExpected);
    }

    const Mat2x2 d{Vector2{-2.0f, 5.0f},
                   Vector2{5.0f, -2.0f}};
    const Mat2x2 dExpected{Vector2{-4.0f, 10.0f},
                           Vector2{10.0f, -4.0f}};
    CORRADE_COMPARE(d*2.0f, dExpected);
    CORRADE_COMPARE(2.0f*d, dExpected);
    {
        Mat2x2 m{Vector2{-2.0f, 5.0f},
                 Vector2{5.0f, -2.0f}};
        CORRADE_COMPARE(&(m *= 2.0f), &m);
        CORRADE_COMPARE(m, dExpected);
    }

    const Mat2x2 e{Vector2{-2.0f, 5.0f},
                   Vector2{5.0f, -2.0f}};
    CORRADE_COMPARE(e/0.5f, dExpected);
    CORRADE_COMPARE(2.0f/e, (Mat2x2{Vector2{-1.0f, 0.4f},
                                    Vector2{0.4f, -1.0f}}));
    {
        Mat2x2 m{Vector2{-2.0f, 5.0f},
                 Vector2{5.0f, -2.0f}};
        CORRADE_COMPARE(&(m /= 0.5f), &m);
        CORRADE_COMPARE(m, dExpected);
    }

    const Vector2 f(2.0f, 5.0f);
    const Math::RectangularMatrix<2, 1, Float> g(3.0f, -1.0f);
    CORRADE_COMPARE(f*g, Mat2x2(Vector2(6.0f, 15.0f),
                                Vector2(-2.0f, -5.0f)));

    /* Operators on variable-sized matrix */
    const BasicMat<1, Float> h(-2.0f);
    CORRADE_COMPARE(2.0f*h, (BasicMat<1, Float>(-4.0f)));
    CORRADE_COMPARE(2.0f/h, (BasicMat<1, Float>(-1.0f)));

    const Math::Vector<1, Float> i(2.0f);
    const Math::RectangularMatrix<1, 1, Float> j(3.0f);
    CORRADE_COMPARE(i*j, (BasicMat<1, Float>(6.0f)));

    /* Functions */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Mat2x2 flippedCols = Mat2x2{Vector2{-1.0f,  5.0f},
                                Vector2{ 7.0f, -2.0f}}.flippedCols();
    CORRADE_COMPARE(flippedCols, (Mat2x2{Vector2{ 7.0f, -2.0f},
                                         Vector2{-1.0f,  5.0f}}));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Mat2x2 flippedRows = Mat2x2{Vector2{-1.0f,  5.0f},
                                Vector2{ 7.0f, -2.0f}}.flippedRows();
    CORRADE_COMPARE(flippedRows, (Mat2x2{Vector2{ 5.0f, -1.0f},
                                         Vector2{-2.0f,  7.0f}}));
}

void RectangularMatrixTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Matrix2x2 a{Vector2{1.0f, 2.0f}, Vector2{3.0f, 4.0f}};
    const Matrix2x2 b{Vector2{2.0f, 3.0f}, Vector2{4.0f, 5.0f}};
    const Matrix2x2 c{Vector2{1.0f, 2.0f}, Vector2{3.0f, 5.0f}};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));

    CORRADE_VERIFY(!o(a, a));
}

void RectangularMatrixTest::debug() {
    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f));

    Containers::String out;
    Debug{&out} << m;
    CORRADE_COMPARE(out, "Matrix(3, 4, 7,\n"
                             "       5, 4, -1,\n"
                             "       8, 7, 8,\n"
                             "       4, 3, 0)\n");

    out = {};
    Debug{&out} << "a" << Matrix3x4() << "b" << RectangularMatrix<4, 3, Byte>();
    CORRADE_COMPARE(out, "a Matrix(0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0) b Matrix(0, 0, 0, 0,\n"
                             "       0, 0, 0, 0,\n"
                             "       0, 0, 0, 0)\n");
}

void RectangularMatrixTest::debugPacked() {
    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f));

    Containers::String out;
    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << m << Matrix2x2{};
    CORRADE_COMPARE(out,
        "{3, 4, 7,\n"
        " 5, 4, -1,\n"
        " 8, 7, 8,\n"
        " 4, 3, 0} Matrix(0, 0,\n"
        "       0, 0)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RectangularMatrixTest)
