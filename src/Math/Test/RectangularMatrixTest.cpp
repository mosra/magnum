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

#include "Math/RectangularMatrix.h"

struct Mat2x3 {
    float a[6];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<2, 3, float, Mat2x3> {
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

namespace Test {

class RectangularMatrixTest: public Corrade::TestSuite::Tester {
    public:
        RectangularMatrixTest();

        void construct();
        void constructDefault();
        void constructConversion();
        void constructFromData();
        void constructFromDiagonal();
        void constructCopy();

        void convert();
        void data();
        void row();

        void compare();

        void negative();
        void addSubtract();
        void multiplyDivide();
        void multiply();

        void transposed();
        void diagonal();

        void vector();

        void debug();
        void configuration();
};

typedef RectangularMatrix<4, 3, Float> Matrix4x3;
typedef RectangularMatrix<3, 4, Float> Matrix3x4;
typedef RectangularMatrix<2, 2, Float> Matrix2;
typedef RectangularMatrix<2, 2, Int> Matrix2i;
typedef Vector<4, Float> Vector4;
typedef Vector<3, Float> Vector3;
typedef Vector<2, Float> Vector2;
typedef Vector<2, Int> Vector2i;

RectangularMatrixTest::RectangularMatrixTest() {
    addTests({&RectangularMatrixTest::construct,
              &RectangularMatrixTest::constructDefault,
              &RectangularMatrixTest::constructConversion,
              &RectangularMatrixTest::constructFromData,
              &RectangularMatrixTest::constructFromDiagonal,
              &RectangularMatrixTest::constructCopy,

              &RectangularMatrixTest::convert,
              &RectangularMatrixTest::data,
              &RectangularMatrixTest::row,

              &RectangularMatrixTest::compare,

              &RectangularMatrixTest::negative,
              &RectangularMatrixTest::addSubtract,
              &RectangularMatrixTest::multiplyDivide,
              &RectangularMatrixTest::multiply,

              &RectangularMatrixTest::transposed,
              &RectangularMatrixTest::diagonal,

              &RectangularMatrixTest::vector,

              &RectangularMatrixTest::debug,
              &RectangularMatrixTest::configuration});
}

void RectangularMatrixTest::construct() {
    constexpr Matrix3x4 a(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                          Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                          Vector4(9.0f, 10.0f, 11.0f, 12.0f));
    CORRADE_COMPARE(a, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                 Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                 Vector4(9.0f, 10.0f, 11.0f, 12.0f)));
}

void RectangularMatrixTest::constructDefault() {
    constexpr Matrix4x3 a;
    CORRADE_COMPARE(a, Matrix4x3(Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f),
                                 Vector3(0.0f, 0.0f, 0.0f)));
}

void RectangularMatrixTest::constructConversion() {
    constexpr Matrix2 a(Vector2(  1.3f, 2.7f),
                        Vector2(-15.0f, 7.0f));
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Matrix2i b(a);
    #else
    Matrix2i b(a); /* Not constexpr under GCC < 4.7 */
    #endif

    CORRADE_COMPARE(b, Matrix2i(Vector2i(  1, 2),
                                Vector2i(-15, 7)));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Matrix2, Matrix2i>::value));
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
    Vector3 diagonal(-1.0f, 5.0f, 11.0f);

    Matrix3x4 expectedA(Vector4(-1.0f, 0.0f,  0.0f, 0.0f),
                        Vector4( 0.0f, 5.0f,  0.0f, 0.0f),
                        Vector4( 0.0f, 0.0f, 11.0f, 0.0f));
    CORRADE_COMPARE(Matrix3x4::fromDiagonal(diagonal), expectedA);

    Matrix4x3 expectedB(Vector3(-1.0f, 0.0f,  0.0f),
                        Vector3( 0.0f, 5.0f,  0.0f),
                        Vector3( 0.0f, 0.0f, 11.0f),
                        Vector3( 0.0f, 0.0f,  0.0f));
    CORRADE_COMPARE(Matrix4x3::fromDiagonal(diagonal), expectedB);
}

void RectangularMatrixTest::constructCopy() {
    constexpr Matrix3x4 a(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                          Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                          Vector4(9.0f, 10.0f, 11.0f, 12.0f));
    constexpr Matrix3x4 b(a);
    CORRADE_COMPARE(b, Matrix3x4(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                                 Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                                 Vector4(9.0f, 10.0f, 11.0f, 12.0f)));
}

void RectangularMatrixTest::convert() {
    typedef RectangularMatrix<2, 3, Float> Matrix2x3;
    constexpr Mat2x3 a{{1.5f,  2.0f, -3.5f,
                        2.0f, -3.1f,  0.4f}};
    constexpr Matrix2x3 b(Vector3(1.5f, 2.0f, -3.5f),
                          Vector3(2.0f, -3.1f,  0.4f));

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Matrix2x3 c(b);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Mat2x3 d(b);
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
    constexpr Vector4 b = a[2];
    constexpr Float c = a[1][2];
    constexpr Float d = *a.data();
    CORRADE_COMPARE(b, Vector4(7.0f, -1.7f, 8.0f, 0.0f));
    CORRADE_COMPARE(c, 7.0f);
    CORRADE_COMPARE(d, 3.0f);
}

void RectangularMatrixTest::row() {
    const Matrix3x4 a(Vector4(1.0f,  2.0f,  3.0f,  4.0f),
                      Vector4(5.0f,  6.0f,  7.0f,  8.0f),
                      Vector4(9.0f, 10.0f, 11.0f, 12.0f));

    CORRADE_COMPARE(a.row(1), Vector3(2.0f, 6.0f, 10.0f));
}

void RectangularMatrixTest::compare() {
    Matrix2 a(Vector2(1.0f, -3.0f),
              Vector2(5.0f, -10.0f));
    Matrix2 b(Vector2(1.0f + TypeTraits<Float>::epsilon()/2, -3.0f),
              Vector2(5.0f, -10.0f));
    Matrix2 c(Vector2(1.0f, -1.0f + TypeTraits<Float>::epsilon()*2),
              Vector2(5.0f, -10.0f));
    CORRADE_VERIFY(a == b);
    CORRADE_VERIFY(a != c);

    Matrix2i ai(Vector2i(1, -3),
                Vector2i(5, -10));
    Matrix2i bi(Vector2i(1, -2),
                Vector2i(5, -10));
    CORRADE_VERIFY(ai == ai);
    CORRADE_VERIFY(ai != bi);
}

void RectangularMatrixTest::negative() {
    Matrix2 matrix(Vector2(1.0f,  -3.0f),
                   Vector2(5.0f, -10.0f));
    Matrix2 negated(Vector2(-1.0f,  3.0f),
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
    Matrix2 matrix(Vector2(1.0f, 2.0f),
                   Vector2(3.0f, 4.0f));
    Matrix2 multiplied(Vector2(-1.5f, -3.0f),
                       Vector2(-4.5f, -6.0f));

    CORRADE_COMPARE(matrix*-1.5f, multiplied);
    CORRADE_COMPARE(-1.5f*matrix, multiplied);
    CORRADE_COMPARE(multiplied/-1.5f, matrix);

    Math::RectangularMatrix<1, 1, Byte> matrixChar(32);
    Math::RectangularMatrix<1, 1, Byte> multipliedChar(-48);
    CORRADE_COMPARE(matrixChar*-1.5f, multipliedChar);
    CORRADE_COMPARE(multipliedChar/-1.5f, matrixChar);
    CORRADE_COMPARE(-1.5f*matrixChar, multipliedChar);

    /* Divide vector with number and inverse */
    Matrix2 divisor(Vector2( 1.0f, 2.0f),
                    Vector2(-4.0f, 8.0f));
    Matrix2 result(Vector2(  1.0f,   0.5f),
                   Vector2(-0.25f, 0.125f));
    CORRADE_COMPARE(1.0f/divisor, result);
    CORRADE_COMPARE(-1550.0f/multipliedChar, matrixChar);
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

void RectangularMatrixTest::diagonal() {
    Vector3 diagonal(-1.0f, 5.0f, 11.0f);

    Matrix4x3 a(Vector3(-1.0f,  1.0f,  3.0f),
                Vector3( 4.0f,  5.0f,  7.0f),
                Vector3( 8.0f,  9.0f, 11.0f),
                Vector3(12.0f, 13.0f, 15.0f));
    CORRADE_COMPARE(a.diagonal(), diagonal);

    Matrix3x4 b(Vector4(-1.0f, 4.0f,  8.0f, 12.0f),
                Vector4( 1.0f, 5.0f,  9.0f, 13.0f),
                Vector4( 3.0f, 7.0f, 11.0f, 15.0f));
    CORRADE_COMPARE(b.diagonal(), diagonal);
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

void RectangularMatrixTest::configuration() {
    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f,   4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.125f),
                Vector4(7.0f, -1.0f, 8.0f,  9.55f));
    std::string value("3 4 7 5 4 -1 8 7 8 4 3.125 9.55");

    Corrade::Utility::Configuration c;
    c.setValue<Matrix3x4>("matrix", m);

    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3x4>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RectangularMatrixTest)
