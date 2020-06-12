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

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Mat3 {
    float a[9];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<3, 3, float, Mat3> {
    constexpr static RectangularMatrix<3, 3, Float> from(const Mat3& other) {
        return RectangularMatrix<3, 3, Float>(
            Vector<3, Float>(other.a[0], other.a[1], other.a[2]),
            Vector<3, Float>(other.a[3], other.a[4], other.a[5]),
            Vector<3, Float>(other.a[6], other.a[7], other.a[8]));
    }

    constexpr static Mat3 to(const RectangularMatrix<3, 3, Float>& other) {
        return Mat3{{other[0][0], other[0][1], other[0][2],
                     other[1][0], other[1][1], other[1][2],
                     other[2][0], other[2][1], other[2][2]}};
    }
};

}

namespace Test { namespace {

struct MatrixTest: Corrade::TestSuite::Tester {
    explicit MatrixTest();

    void construct();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructOneValue();
    void constructOneComponent();
    void constructConversion();
    void constructFromDifferentSize();
    void constructCopy();
    void convert();

    void isOrthogonal();

    void trace();
    void ij();
    void adjugateCofactor();
    void determinant();
    void inverted();
    void invertedOrthogonal();
    void invertedOrthogonalNotOrthogonal();

    void strictWeakOrdering();

    void subclassTypes();
    void subclass();

    void debug();
};

typedef Matrix<2, Float> Matrix2x2;
typedef Matrix<3, Float> Matrix3x3;
typedef Matrix<4, Float> Matrix4x4;
typedef Matrix<4, Int> Matrix4x4i;
typedef Vector<2, Float> Vector2;
typedef Vector<3, Float> Vector3;
typedef Vector<4, Float> Vector4;
typedef Vector<4, Int> Vector4i;
typedef Math::Constants<Float> Constants;

MatrixTest::MatrixTest() {
    addTests({&MatrixTest::construct,
              &MatrixTest::constructIdentity,
              &MatrixTest::constructZero,
              &MatrixTest::constructNoInit,
              &MatrixTest::constructOneValue,
              &MatrixTest::constructOneComponent,
              &MatrixTest::constructConversion,
              &MatrixTest::constructFromDifferentSize,
              &MatrixTest::constructCopy,
              &MatrixTest::convert,

              &MatrixTest::isOrthogonal,

              &MatrixTest::trace,
              &MatrixTest::ij,
              &MatrixTest::adjugateCofactor,
              &MatrixTest::determinant,
              &MatrixTest::inverted,
              &MatrixTest::invertedOrthogonal,
              &MatrixTest::invertedOrthogonalNotOrthogonal,

              &MatrixTest::strictWeakOrdering,

              &MatrixTest::subclassTypes,
              &MatrixTest::subclass,

              &MatrixTest::debug});
}

void MatrixTest::construct() {
    constexpr Matrix4x4 a = {Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                             Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                             Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                             Vector4(7.9f, -1.0f, 8.0f, -1.5f)};
    CORRADE_COMPARE(a, Matrix4x4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                                 Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                                 Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                                 Vector4(7.9f, -1.0f, 8.0f, -1.5f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, Vector4, Vector4, Vector4, Vector4>::value));
}

void MatrixTest::constructIdentity() {
    constexpr Matrix4x4 identity;
    constexpr Matrix4x4 identity2{IdentityInit};
    constexpr Matrix4x4 identity3{IdentityInit, 4.0f};

    Matrix4x4 identityExpected(Vector4(1.0f, 0.0f, 0.0f, 0.0f),
                               Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                               Vector4(0.0f, 0.0f, 1.0f, 0.0f),
                               Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    Matrix4x4 identity3Expected(Vector4(4.0f, 0.0f, 0.0f, 0.0f),
                                Vector4(0.0f, 4.0f, 0.0f, 0.0f),
                                Vector4(0.0f, 0.0f, 4.0f, 0.0f),
                                Vector4(0.0f, 0.0f, 0.0f, 4.0f));

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Matrix4x4>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, Matrix4x4>::value));
}

void MatrixTest::constructZero() {
    constexpr Matrix4x4 a{ZeroInit};
    CORRADE_COMPARE(a, Matrix4x4(Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                                 Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                                 Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                                 Vector4(0.0f, 0.0f, 0.0f, 0.0f)));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Matrix4x4>::value));
}

void MatrixTest::constructNoInit() {
    Matrix4x4 a{Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                Vector4(7.9f, -1.0f, 8.0f, -1.5f)};
    new(&a) Matrix4x4{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Matrix4x4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                                     Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                                     Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                                     Vector4(7.9f, -1.0f, 8.0f, -1.5f)));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, Magnum::NoInitT>::value));

        /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Matrix4x4>::value));
}

void MatrixTest::constructOneValue() {
    constexpr Matrix3x3 a{1.5f};
    CORRADE_COMPARE(a, (Matrix3x3{Vector3{1.5f, 1.5f, 1.5f},
                                  Vector3{1.5f, 1.5f, 1.5f},
                                  Vector3{1.5f, 1.5f, 1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Matrix3x3>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3x3, Float>::value));
}

void MatrixTest::constructOneComponent() {
    typedef Math::Matrix<1, Float> Matrix1x1;
    typedef Math::Vector<1, Float> Vector1;

    constexpr Matrix1x1 a{1.5f};
    constexpr Matrix1x1 b{Vector1{1.5f}};
    CORRADE_COMPARE(a, b);

    /* Implicit constructor must work */
    constexpr Matrix1x1 c = Vector1{1.5f};
    CORRADE_COMPARE(c, Matrix1x1{Vector1{1.5f}});

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix1x1, Vector1>::value));
}

void MatrixTest::constructConversion() {
    constexpr Matrix4x4 a(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                          Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                          Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                          Vector4(7.9f, -1.0f, 8.0f, -1.5f));
    constexpr Matrix4x4i b(a);
    CORRADE_COMPARE(b, Matrix4x4i(Vector4i(3,  5, 8, -3),
                                  Vector4i(4,  4, 7,  2),
                                  Vector4i(1,  2, 3, -1),
                                  Vector4i(7, -1, 8, -1)));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Matrix4x4, Matrix4x4i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix4x4, Matrix4x4i>::value));
}

void MatrixTest::constructFromDifferentSize() {
    constexpr Matrix4x4 a{Vector4{3.0f,  5.0f, 8.0f, -3.0f},
                          Vector4{4.5f,  4.0f, 7.0f,  2.0f},
                          Vector4{1.0f,  2.0f, 3.0f, -1.0f},
                          Vector4{7.9f, -1.0f, 8.0f, -1.5f}};
    constexpr Matrix2x2 b{Vector2{3.0f,  5.0f},
                          Vector2{4.5f,  4.0f}};
    constexpr Matrix4x4 c{Vector4{3.0f, 5.0f, 0.0f, 0.0f},
                          Vector4{4.5f, 4.0f, 0.0f, 0.0f},
                          Vector4{0.0f, 0.0f, 1.0f, 0.0f},
                          Vector4{0.0f, 0.0f, 0.0f, 1.0f}};

    constexpr Matrix4x4 larger{b};
    CORRADE_COMPARE(larger, c);
    CORRADE_COMPARE(Matrix4x4{b}, c);

    constexpr Matrix2x2 smaller{a};
    CORRADE_COMPARE(smaller, b);
    CORRADE_COMPARE(Matrix2x2{a}, b);
}

void MatrixTest::constructCopy() {
    constexpr RectangularMatrix<4, 4, Float> a(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                                               Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                                               Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                                               Vector4(7.9f, -1.0f, 8.0f, -1.5f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Matrix4x4 b(a);
    CORRADE_COMPARE(b, Matrix4x4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                                 Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                                 Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                                 Vector4(7.9f, -1.0f, 8.0f, -1.5f)));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Matrix4x4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Matrix4x4>::value);
}

void MatrixTest::convert() {
    constexpr Mat3 a{{1.5f,  2.0f, -3.5f,
                      2.0f, -3.1f,  0.4f,
                      9.5f, -1.5f,  0.1f}};
    constexpr Matrix3x3 b(Vector3(1.5f, 2.0f, -3.5f),
                          Vector3(2.0f, -3.1f,  0.4f),
                          Vector3(9.5f, -1.5f,  0.1f));

    constexpr Matrix3x3 c(b);
    CORRADE_COMPARE(c, b);

    constexpr Mat3 d(b);
    for(std::size_t i = 0; i != 9; ++i)
        CORRADE_COMPARE(d.a[i], a.a[i]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Mat3, Matrix3x3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Matrix3x3, Mat3>::value));
}

void MatrixTest::isOrthogonal() {
    CORRADE_VERIFY(!Matrix3x3(Vector3(1.0f, 0.0f, 0.0f),
                              Vector3(0.0f, 1.0f, 0.0f),
                              Vector3(0.0f, 0.1f, 1.0f)).isOrthogonal());
    CORRADE_VERIFY(!Matrix3x3(Vector3(1.0f, 0.0f, 0.0f),
                              Vector3(0.0f, 1.0f, 0.0f),
                              Vector3(0.0f, 1.0f, 0.0f)).isOrthogonal());
    CORRADE_VERIFY(Matrix3x3(Vector3(1.0f, 0.0f, 0.0f),
                             Vector3(0.0f, 1.0f, 0.0f),
                             Vector3(0.0f, 0.0f, 1.0f)).isOrthogonal());
}

void MatrixTest::trace() {
    Matrix<5, Int> m(
        Vector<5, Int>(1, 2,   3,  0,  0),
        Vector<5, Int>(2, 3,   2,  1, -2),
        Vector<5, Int>(1, 1, -20,  1,  0),
        Vector<5, Int>(2, 0,   0, 10,  2),
        Vector<5, Int>(3, 1,   0,  1, -2)
    );

    CORRADE_COMPARE(m.trace(), -8);
}

void MatrixTest::ij() {
    Matrix4x4 original(Vector4( 0.0f,  1.0f,  2.0f,  3.0f),
                       Vector4( 4.0f,  5.0f,  6.0f,  7.0f),
                       Vector4( 8.0f,  9.0f, 10.0f, 11.0f),
                       Vector4(12.0f, 13.0f, 14.0f, 15.0f));

    Matrix3x3 skipped(Vector3( 0.0f,  1.0f,  3.0f),
                      Vector3( 8.0f,  9.0f, 11.0f),
                      Vector3(12.0f, 13.0f, 15.0f));

    CORRADE_COMPARE(original.ij(1, 2), skipped);
}

void MatrixTest::adjugateCofactor() {
    Matrix4x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f),
                Vector4(9.0f,  4.0f, 5.0f, 9.0f));

    /* Adjugate is used in inverted(), which is tested below; so just verify
       these are a transpose of each other */
    CORRADE_COMPARE(m.adjugate().transposed(), m.comatrix());
}

void MatrixTest::determinant() {
    Matrix<5, Int> m(
        Vector<5, Int>(1, 2, 2, 1,  0),
        Vector<5, Int>(2, 3, 2, 1, -2),
        Vector<5, Int>(1, 1, 1, 1,  0),
        Vector<5, Int>(2, 0, 0, 1,  2),
        Vector<5, Int>(3, 1, 0, 1, -2)
    );

    CORRADE_COMPARE(m.determinant(), -2);
}

void MatrixTest::inverted() {
    Matrix4x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f),
                Vector4(9.0f,  4.0f, 5.0f, 9.0f));

    Matrix4x4 inverse(Vector4(-60/103.0f,   71/103.0f,  -4/103.0f,  3/103.0f),
                      Vector4(-66/103.0f,  109/103.0f, -25/103.0f, -7/103.0f),
                      Vector4(177/412.0f,  -97/206.0f,  53/412.0f, -7/206.0f),
                      Vector4(259/412.0f, -185/206.0f,  31/412.0f, 27/206.0f));

    Matrix4x4 _inverse = m.inverted();

    CORRADE_COMPARE(_inverse, inverse);
    CORRADE_COMPARE(_inverse*m, Matrix4x4());
}

void MatrixTest::invertedOrthogonal() {
    Matrix3x3 a(Vector3(Constants::sqrt3()/2.0f, 0.5f, 0.0f),
                Vector3(-0.5f, Constants::sqrt3()/2.0f, 0.0f),
                Vector3(0.0f, 0.0f, 1.0f));

    CORRADE_COMPARE(a.invertedOrthogonal()*a, Matrix3x3());
    CORRADE_COMPARE(a.invertedOrthogonal(), a.inverted());
}

void MatrixTest::invertedOrthogonalNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};

    Matrix3x3 a(Vector3(Constants::sqrt3()/2.0f, 0.5f, 0.0f),
                Vector3(-0.5f, Constants::sqrt3()/2.0f, 0.0f),
                Vector3(0.0f, 0.0f, 1.0f));
    (a*2).invertedOrthogonal();
    CORRADE_COMPARE(o.str(),
        "Math::Matrix::invertedOrthogonal(): the matrix is not orthogonal:\n"
        "Matrix(1.73205, -1, 0,\n"
        "       1, 1.73205, 0,\n"
        "       0, 0, 2)\n");
}

void MatrixTest::strictWeakOrdering() {
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

template<class T> class BasicVec2: public Math::Vector<2, T> {
    public:
        template<class ...U> constexpr BasicVec2(U&&... args): Math::Vector<2, T>{args...} {}
};

template<class T> class BasicMat2: public Math::Matrix<2, T> {
    public:
        template<class ...U> constexpr BasicMat2(U&&... args): Math::Matrix<2, T>{args...} {}

        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(2, BasicMat2, BasicVec2)
};

typedef BasicVec2<Float> Vec2;
typedef BasicMat2<Float> Mat2;

void MatrixTest::subclassTypes() {
    const Mat2 c;
    Mat2 a;
    CORRADE_VERIFY((std::is_same<decltype(c[1]), const Vec2>::value));
    CORRADE_VERIFY((std::is_same<decltype(a[1]), Vec2&>::value));
    CORRADE_VERIFY((std::is_same<decltype(c.row(1)), Vec2>::value));

    const Mat2 c2;
    const Vec2 cv;
    CORRADE_VERIFY((std::is_same<decltype(c*c2), Mat2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c*cv), Vec2>::value));

    CORRADE_VERIFY((std::is_same<decltype(c.transposed()), Mat2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c.diagonal()), Vec2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c.inverted()), Mat2>::value));
    CORRADE_VERIFY((std::is_same<decltype(c.invertedOrthogonal()), Mat2>::value));
}

void MatrixTest::subclass() {
    /* Constexpr constructor */
    constexpr Mat2 a(Vec2(2.0f, 3.5f),
                     Vec2(3.0f, 1.0f));
    Mat2 b(Vec2(2.0f, 3.5f),
           Vec2(3.0f, 1.0f));
    CORRADE_COMPARE(a[1], Vec2(3.0f, 1.0f));
    CORRADE_COMPARE(b[1], Vec2(3.0f, 1.0f));
    CORRADE_COMPARE(a.row(1), Vec2(3.5f, 1.0f));

    CORRADE_COMPARE(a*b, Mat2(Vec2(14.5f, 10.5f),
                              Vec2(9.0f, 11.5f)));
    CORRADE_COMPARE(a*Vec2(1.0f, 0.5f), Vec2(3.5f, 4.0f));

    CORRADE_COMPARE(a.transposed(), Mat2(Vec2(2.0f, 3.0f),
                                         Vec2(3.5f, 1.0f)));
    CORRADE_COMPARE(a.diagonal(), Vec2(2.0f, 1.0f));

    Mat2 c(Vec2(Constants::sqrt2()/2.0f, Constants::sqrt2()/2.0f),
           Vec2(-Constants::sqrt2()/2.0f, Constants::sqrt2()/2.0f));
    CORRADE_COMPARE(c.inverted(), Mat2(Vec2(Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f),
                                       Vec2(Constants::sqrt2()/2.0f, Constants::sqrt2()/2.0f)));
    CORRADE_COMPARE(c.invertedOrthogonal(), Mat2(Vec2(Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f),
                                                 Vec2(Constants::sqrt2()/2.0f, Constants::sqrt2()/2.0f)));
}

void MatrixTest::debug() {
    Matrix4x4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.0f),
                Vector4(7.0f, -1.0f, 8.0f, 0.0f),
                Vector4(9.0f,  4.0f, 5.0f, 9.0f));

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7, 9,\n"
                             "       5, 4, -1, 4,\n"
                             "       8, 7, 8, 5,\n"
                             "       4, 3, 0, 9)\n");

    o.str({});
    Debug(&o) << "a" << Matrix4x4() << "b" << Matrix4x4();
    CORRADE_COMPARE(o.str(), "a Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1) b Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::MatrixTest)
