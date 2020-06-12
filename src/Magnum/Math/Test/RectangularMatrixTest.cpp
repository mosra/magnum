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

struct RectangularMatrixTest: Corrade::TestSuite::Tester {
    explicit RectangularMatrixTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructOneComponent();
    void constructConversion();
    void constructFromData();
    void constructFromDiagonal();
    void constructCopy();
    void convert();

    void data();
    void row();

    void compare();
    void compareComponentWise();

    void negative();
    void addSubtract();
    void multiplyDivide();
    void multiply();
    void multiplyVector();

    void transposed();
    void flippedCols();
    void flippedRows();
    void diagonal();

    void vector();

    void subclassTypes();
    void subclass();

    void strictWeakOrdering();

    void debug();
};

typedef RectangularMatrix<4, 3, Float> Matrix4x3;
typedef RectangularMatrix<3, 4, Float> Matrix3x4;
typedef RectangularMatrix<2, 2, Float> Matrix2x2;
typedef RectangularMatrix<2, 2, Int> Matrix2x2i;
typedef Vector<4, Float> Vector4;
typedef Vector<3, Float> Vector3;
typedef Vector<2, Float> Vector2;

typedef RectangularMatrix<4, 3, Int> Matrix4x3i;
typedef RectangularMatrix<3, 4, Int> Matrix3x4i;
typedef Vector<4, Int> Vector4i;
typedef Vector<3, Int> Vector3i;
typedef Vector<2, Int> Vector2i;

RectangularMatrixTest::RectangularMatrixTest() {
    addTests({&RectangularMatrixTest::construct,
              &RectangularMatrixTest::constructDefault,
              &RectangularMatrixTest::constructNoInit,
              &RectangularMatrixTest::constructOneValue,
              &RectangularMatrixTest::constructOneComponent,
              &RectangularMatrixTest::constructConversion,
              &RectangularMatrixTest::constructFromData,
              &RectangularMatrixTest::constructFromDiagonal,
              &RectangularMatrixTest::constructCopy,
              &RectangularMatrixTest::convert,

              &RectangularMatrixTest::data,
              &RectangularMatrixTest::row,

              &RectangularMatrixTest::compare,
              &RectangularMatrixTest::compareComponentWise,

              &RectangularMatrixTest::negative,
              &RectangularMatrixTest::addSubtract,
              &RectangularMatrixTest::multiplyDivide,
              &RectangularMatrixTest::multiply,
              &RectangularMatrixTest::multiplyVector,

              &RectangularMatrixTest::transposed,
              &RectangularMatrixTest::flippedCols,
              &RectangularMatrixTest::flippedRows,
              &RectangularMatrixTest::diagonal,

              &RectangularMatrixTest::vector,

              &RectangularMatrixTest::subclassTypes,
              &RectangularMatrixTest::subclass,

              &RectangularMatrixTest::strictWeakOrdering,

              &RectangularMatrixTest::debug});
}

void RectangularMatrixTest::construct() {
    constexpr Matrix3x4 a = {Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                             Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                             Vector4(9.0f, 10.0f, 11.0f, 12.0f)};
    CORRADE_COMPARE(a, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                 Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                 Vector4(9.0f, 10.0f, 11.0f, 12.0f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3x4, Vector4, Vector4, Vector4>::value));
}

void RectangularMatrixTest::constructDefault() {
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
    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x3, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Matrix4x3>::value));
}

void RectangularMatrixTest::constructNoInit() {
    Matrix3x4 a{Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                Vector4(9.0f, 10.0f, 11.0f, 12.0f)};
    new(&a) Matrix3x4{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                     Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                     Vector4(9.0f, 10.0f, 11.0f, 12.0f)));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3x4, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Matrix3x4>::value));
}

void RectangularMatrixTest::constructOneValue() {
    constexpr Matrix3x4 a{1.5f};
    CORRADE_COMPARE(a, (Matrix3x4{Vector4{1.5f, 1.5f, 1.5f, 1.5f},
                                  Vector4{1.5f, 1.5f, 1.5f, 1.5f},
                                  Vector4{1.5f, 1.5f, 1.5f, 1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Matrix3x4>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3x4, Float>::value));
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

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix1x1, Vector1>::value));
}

void RectangularMatrixTest::constructConversion() {
    constexpr Matrix2x2 a(Vector2(  1.3f, 2.7f),
                          Vector2(-15.0f, 7.0f));
    constexpr Matrix2x2i b(a);

    CORRADE_COMPARE(b, Matrix2x2i(Vector2i(  1, 2),
                                  Vector2i(-15, 7)));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Matrix2x2, Matrix2x2i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix2x2, Matrix2x2i>::value));
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

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Matrix3x4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Matrix3x4>::value);
}

void RectangularMatrixTest::convert() {
    typedef RectangularMatrix<2, 3, Float> Matrix2x3;
    constexpr Mat2x3 a{{1.5f,  2.0f, -3.5f,
                        2.0f, -3.1f,  0.4f}};
    constexpr Matrix2x3 b(Vector3(1.5f, 2.0f, -3.5f),
                          Vector3(2.0f, -3.1f,  0.4f));

    /* GCC 5.1 had a bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66450
       Hopefully this does not reappear. */
    constexpr Matrix2x3 c{a};
    CORRADE_COMPARE(c, b);

    constexpr Mat2x3 d(b);
    for(std::size_t i = 0; i != 5; ++i)
        CORRADE_COMPARE(d.a[i], a.a[i]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Mat2x3, Matrix2x3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Matrix2x3, Mat2x3>::value));
}

void RectangularMatrixTest::data() {
    Matrix3x4 m;
    Vector4 vector(4.0f, 5.0f, 6.0f, 7.0f);

    m[2] = vector;
    m[1][1] = 1.0f;
    m[0][2] = 1.5f;

    CORRADE_COMPARE(m[1][1], 1.0f);
    CORRADE_COMPARE(m[0][2], 1.5f);
    CORRADE_COMPARE(m[2], vector);

    CORRADE_COMPARE(m, Matrix3x4(Vector4(0.0f, 0.0f, 1.5f, 0.0f),
                                 Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                                 Vector4(4.0f, 5.0f, 6.0f, 7.0f)));

    /* Pointer chasings, i.e. *(b.data()[1]), are not possible */
    constexpr Matrix3x4 a(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                          Vector4(4.5f,  4.0f, 7.0f, 3.0f),
                          Vector4(7.0f, -1.7f, 8.0f, 0.0f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector4 b = a[2];
    constexpr Float c = a[1][2];
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing pointer is verboten */
    constexpr
    #endif
    Float d = *a.data();
    CORRADE_COMPARE(b, Vector4(7.0f, -1.7f, 8.0f, 0.0f));
    CORRADE_COMPARE(c, 7.0f);
    CORRADE_COMPARE(d, 3.0f);
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
    typedef BoolVector<3> BoolVector3;
    typedef RectangularMatrix<3, 1, Float> Matrix3x1;
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) < Matrix3x1(1.1f, -1.0f, 3.0f), BoolVector3(0x1));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) <= Matrix3x1(1.1f, -1.0f, 3.0f), BoolVector3(0x3));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) >= Matrix3x1(1.1f, -1.0f, 3.0f), BoolVector3(0x6));
    CORRADE_COMPARE(Matrix3x1(1.0f, -1.0f, 5.0f) > Matrix3x1(1.1f, -1.0f, 3.0f), BoolVector3(0x4));
}

void RectangularMatrixTest::negative() {
    Matrix2x2 matrix(Vector2(1.0f,  -3.0f),
                     Vector2(5.0f, -10.0f));
    Matrix2x2 negated(Vector2(-1.0f,  3.0f),
                      Vector2(-5.0f, 10.0f));
    CORRADE_COMPARE(-matrix, negated);
}

void RectangularMatrixTest::addSubtract() {
    Matrix4x3 a(Vector3(0.0f,   1.0f,   3.0f),
                Vector3(4.0f,   5.0f,   7.0f),
                Vector3(8.0f,   9.0f,   11.0f),
                Vector3(12.0f, 13.0f,  15.0f));
    Matrix4x3 b(Vector3(-4.0f,  0.5f,   9.0f),
                Vector3(-9.0f, 11.0f,  0.25f),
                Vector3( 0.0f, -8.0f,  19.0f),
                Vector3(-3.0f, -5.0f,   2.0f));
    Matrix4x3 c(Vector3(-4.0f,  1.5f,  12.0f),
                Vector3(-5.0f, 16.0f,  7.25f),
                Vector3( 8.0f,  1.0f,  30.0f),
                Vector3( 9.0f,  8.0f,  17.0f));

    CORRADE_COMPARE(a + b, c);
    CORRADE_COMPARE(c - b, a);
}

void RectangularMatrixTest::multiplyDivide() {
    Matrix2x2 matrix(Vector2(1.0f, 2.0f),
                     Vector2(3.0f, 4.0f));
    Matrix2x2 multiplied(Vector2(-1.5f, -3.0f),
                         Vector2(-4.5f, -6.0f));

    CORRADE_COMPARE(matrix*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*matrix, multiplied);
    CORRADE_COMPARE(multiplied/-1.5f, matrix);

    /* Divide vector with number and inverse */
    Matrix2x2 divisor(Vector2( 1.0f, 2.0f),
                      Vector2(-4.0f, 8.0f));
    Matrix2x2 result(Vector2(  1.0f,   0.5f),
                     Vector2(-0.25f, 0.125f));
    CORRADE_COMPARE(1.0f/divisor, result);
}

void RectangularMatrixTest::multiply() {
    RectangularMatrix<4, 6, Int> left(
        Vector<6, Int>(-5,   27, 10,  33, 0, -15),
        Vector<6, Int>( 7,   56, 66,   1, 0, -24),
        Vector<6, Int>( 4,   41,  4,   0, 1,  -4),
        Vector<6, Int>( 9, -100, 19, -49, 1,   9)
    );

    RectangularMatrix<5, 4, Int> right(
        Vector<4, Int>(1,  -7,  0,  158),
        Vector<4, Int>(2,  24, -3,   40),
        Vector<4, Int>(3, -15, -2,  -50),
        Vector<4, Int>(4,  17, -1, -284),
        Vector<4, Int>(5,  30,  4,   18)
    );

    RectangularMatrix<5, 6, Int> expected(
       Vector<6, Int>( 1368, -16165,  2550, -7716,  158,  1575),
       Vector<6, Int>(  506,  -2725,  2352, -1870,   37,  -234),
       Vector<6, Int>( -578,   4159, -1918,  2534,  -52,  -127),
       Vector<6, Int>(-2461,  29419, -4238, 14065, -285, -3020),
       Vector<6, Int>(  363,    179,  2388,  -687,   22,  -649)
    );

    CORRADE_COMPARE(left*right, expected);
}

void RectangularMatrixTest::multiplyVector() {
    Vector4i a(-5, 27, 10, 33);
    RectangularMatrix<3, 1, Int> b(1, 2, 3);
    CORRADE_COMPARE(a*b, Matrix3x4i(
       Vector4i( -5,  27, 10,  33),
       Vector4i(-10,  54, 20,  66),
       Vector4i(-15,  81, 30,  99)
    ));

    Matrix3x4i c(Vector4i(0, 4,  8, 12),
                 Vector4i(1, 5,  9, 13),
                 Vector4i(3, 7, 11, 15));
    Vector3i d(2, -2, 3);
    CORRADE_COMPARE(c*d, Vector4i(7, 19, 31, 43));
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
    typedef Vector<3, Int> Vector3i;
    typedef RectangularMatrix<4, 3, Int> Matrix4x3i;
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

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(size, size, BasicMat<size, T>)
};

MAGNUM_MATRIX_OPERATOR_IMPLEMENTATION(BasicMat<size, T>)

template<class T> class BasicMat2x2: public BasicMat<2, T> {
    public:
        template<class ...U> constexpr BasicMat2x2(U&&... args): BasicMat<2, T>{args...} {}

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(2, 2, BasicMat2x2<T>)
};

MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(2, BasicMat2x2)

typedef BasicMat2x2<Float> Mat2x2;

void RectangularMatrixTest::subclassTypes() {
    Float* const data = nullptr;
    const Float* const cdata = nullptr;
    CORRADE_VERIFY((std::is_same<decltype(Mat2x2::from(data)), Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(Mat2x2::from(cdata)), const Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(Mat2x2::fromDiagonal({})), Mat2x2>::value));

    /* Const operators */
    const Mat2x2 c;
    CORRADE_VERIFY((std::is_same<decltype(-c), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c + c), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c*1.0f), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(1.0f*c), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c/1.0f), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(1.0f/c), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(Vector2()*Math::RectangularMatrix<2, 1, Float>()), Mat2x2>::value));

    /* Assignment operators */
    Mat2x2 a;
    CORRADE_VERIFY((std::is_same<decltype(a = c), Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(a += c), Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(a -= c), Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(a *= 1.0f), Mat2x2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(a /= 1.0f), Mat2x2&>::value));

    /* Operators on variable-sized matrix */
    const BasicMat<3, Float> c2;
    CORRADE_VERIFY((std::is_same<decltype(1.0f*c2), BasicMat<3, Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(1.0f/c2), BasicMat<3, Float>>::value));
    CORRADE_VERIFY((std::is_same<decltype(Vector3()*Math::RectangularMatrix<3, 1, Float>()), BasicMat<3, Float>>::value));

    /* Functions */
    CORRADE_VERIFY((std::is_same<decltype(a.flippedCols()), Mat2x2>::value));
    CORRADE_VERIFY((std::is_same<decltype(a.flippedRows()), Mat2x2>::value));
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
    CORRADE_COMPARE(-a, Mat2x2(Vector2(-1.0f, 3.0f),
                               Vector2(3.0f, -1.0f)));

    Mat2x2 b(Vector2(-2.0f, 5.0f),
             Vector2(5.0f, -2.0f));
    const Mat2x2 bExpected(Vector2(-1.0f, 2.0f),
                           Vector2(2.0f, -1.0f));
    CORRADE_COMPARE(b + a, bExpected);

    Mat2x2 c(Vector2(-2.0f, 5.0f),
             Vector2(5.0f, -2.0f));
    const Mat2x2 cExpected(Vector2(-3.0f, 8.0f),
                           Vector2(8.0f, -3.0f));
    CORRADE_COMPARE(c - a, cExpected);

    Mat2x2 d(Vector2(-2.0f, 5.0f),
             Vector2(5.0f, -2.0f));
    const Mat2x2 dExpected(Vector2(-4.0f, 10.0f),
                           Vector2(10.0f, -4.0f));
    CORRADE_COMPARE(d*2.0f, dExpected);
    CORRADE_COMPARE(2.0f*d, dExpected);

    /* No need to test in-place operators as the other ones are implemented
       using them */

    Mat2x2 e(Vector2(-2.0f, 5.0f),
             Vector2(5.0f, -2.0f));
    CORRADE_COMPARE(e/0.5f, dExpected);
    CORRADE_COMPARE(2.0f/e, Mat2x2(Vector2(-1.0f, 0.4f),
                                   Vector2(0.4f, -1.0f)));
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

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7,\n"
                             "       5, 4, -1,\n"
                             "       8, 7, 8,\n"
                             "       4, 3, 0)\n");

    o.str({});
    Debug(&o) << "a" << Matrix3x4() << "b" << RectangularMatrix<4, 3, Byte>();
    CORRADE_COMPARE(o.str(), "a Matrix(0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0,\n"
                             "       0, 0, 0) b Matrix(0, 0, 0, 0,\n"
                             "       0, 0, 0, 0,\n"
                             "       0, 0, 0, 0)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RectangularMatrixTest)
