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

#include "Math/Matrix.h"

namespace Magnum { namespace Math { namespace Test {

class MatrixTest: public Corrade::TestSuite::Tester {
    public:
        MatrixTest();

        void construct();
        void constructIdentity();
        void constructZero();
        void constructConversion();
        void constructCopy();

        void isOrthogonal();

        void trace();
        void ij();
        void determinant();
        void inverted();

        void debug();
        void configuration();
};

typedef Matrix<4, Float> Matrix4;
typedef Matrix<4, Int> Matrix4i;
typedef Matrix<3, Float> Matrix3;
typedef Vector<4, Float> Vector4;
typedef Vector<4, Int> Vector4i;
typedef Vector<3, Float> Vector3;

MatrixTest::MatrixTest() {
    addTests({&MatrixTest::construct,
              &MatrixTest::constructIdentity,
              &MatrixTest::constructZero,
              &MatrixTest::constructConversion,
              &MatrixTest::constructCopy,

              &MatrixTest::isOrthogonal,

              &MatrixTest::trace,
              &MatrixTest::ij,
              &MatrixTest::determinant,
              &MatrixTest::inverted,
              &MatrixTest::debug,
              &MatrixTest::configuration});
}

void MatrixTest::construct() {
    /* Value constructor */
    constexpr Matrix4 a(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                        Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                        Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                        Vector4(7.9f, -1.0f, 8.0f, -1.5f));
    CORRADE_COMPARE(a, Matrix4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                               Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                               Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                               Vector4(7.9f, -1.0f, 8.0f, -1.5f)));
}

void MatrixTest::constructIdentity() {
    Matrix4 identity;
    Matrix4 identity2(Matrix4::Identity);
    Matrix4 identity3(Matrix4::Identity, 4.0f);

    Matrix4 identityExpected(Vector4(1.0f, 0.0f, 0.0f, 0.0f),
                             Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                             Vector4(0.0f, 0.0f, 1.0f, 0.0f),
                             Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    Matrix4 identity3Expected(Vector4(4.0f, 0.0f, 0.0f, 0.0f),
                              Vector4(0.0f, 4.0f, 0.0f, 0.0f),
                              Vector4(0.0f, 0.0f, 4.0f, 0.0f),
                              Vector4(0.0f, 0.0f, 0.0f, 4.0f));

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);
}

void MatrixTest::constructZero() {
    constexpr Matrix4 a(Matrix4::Zero);
    CORRADE_COMPARE(a, Matrix4(Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                               Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                               Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                               Vector4(0.0f, 0.0f, 0.0f, 0.0f)));
}

void MatrixTest::constructConversion() {
    constexpr Matrix4 a(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                        Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                        Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                        Vector4(7.9f, -1.0f, 8.0f, -1.5f));
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Matrix4i b(a);
    #else
    Matrix4i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Matrix4i(Vector4i(3,  5, 8, -3),
                                Vector4i(4,  4, 7,  2),
                                Vector4i(1,  2, 3, -1),
                                Vector4i(7, -1, 8, -1)));
}

void MatrixTest::constructCopy() {
    constexpr Matrix4 a(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                        Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                        Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                        Vector4(7.9f, -1.0f, 8.0f, -1.5f));
    constexpr Matrix4 b(a);
    CORRADE_COMPARE(b, Matrix4(Vector4(3.0f,  5.0f, 8.0f, -3.0f),
                               Vector4(4.5f,  4.0f, 7.0f,  2.0f),
                               Vector4(1.0f,  2.0f, 3.0f, -1.0f),
                               Vector4(7.9f, -1.0f, 8.0f, -1.5f)));
}

void MatrixTest::isOrthogonal() {
    CORRADE_VERIFY(!Matrix3(Vector3(1.0f, 0.0f, 0.0f),
                            Vector3(0.0f, 1.0f, 0.0f),
                            Vector3(0.0f, 0.1f, 1.0f)).isOrthogonal());
    CORRADE_VERIFY(!Matrix3(Vector3(1.0f, 0.0f, 0.0f),
                            Vector3(0.0f, 1.0f, 0.0f),
                            Vector3(0.0f, 1.0f, 0.0f)).isOrthogonal());
    CORRADE_VERIFY(Matrix3(Vector3(1.0f, 0.0f, 0.0f),
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
    Matrix4 original(Vector4( 0.0f,  1.0f,  2.0f,  3.0f),
                     Vector4( 4.0f,  5.0f,  6.0f,  7.0f),
                     Vector4( 8.0f,  9.0f, 10.0f, 11.0f),
                     Vector4(12.0f, 13.0f, 14.0f, 15.0f));

    Matrix3 skipped(Vector3( 0.0f,  1.0f,  3.0f),
                    Vector3( 8.0f,  9.0f, 11.0f),
                    Vector3(12.0f, 13.0f, 15.0f));

    CORRADE_COMPARE(original.ij(1, 2), skipped);
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
    Matrix4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
              Vector4(4.0f,  4.0f, 7.0f, 3.0f),
              Vector4(7.0f, -1.0f, 8.0f, 0.0f),
              Vector4(9.0f,  4.0f, 5.0f, 9.0f));

    Matrix4 inverse(Vector4(-60/103.0f,   71/103.0f,  -4/103.0f,  3/103.0f),
                    Vector4(-66/103.0f,  109/103.0f, -25/103.0f, -7/103.0f),
                    Vector4(177/412.0f,  -97/206.0f,  53/412.0f, -7/206.0f),
                    Vector4(259/412.0f, -185/206.0f,  31/412.0f, 27/206.0f));

    Matrix4 _inverse = m.inverted();

    CORRADE_COMPARE(_inverse, inverse);
    CORRADE_COMPARE(_inverse*m, Matrix4());
}

void MatrixTest::debug() {
    Matrix4 m(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
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
    Debug(&o) << "a" << Matrix4() << "b" << Matrix4();
    CORRADE_COMPARE(o.str(), "a Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1) b Matrix(1, 0, 0, 0,\n"
                             "       0, 1, 0, 0,\n"
                             "       0, 0, 1, 0,\n"
                             "       0, 0, 0, 1)\n");
}

void MatrixTest::configuration() {
    Corrade::Utility::Configuration c;

    Matrix4 m(Vector4(3.0f,  5.0f, 8.0f,   4.0f),
              Vector4(4.0f,  4.0f, 7.0f, 3.125f),
              Vector4(7.0f, -1.0f, 8.0f,   0.0f),
              Vector4(9.0f,  4.0f, 5.0f,  9.55f));
    std::string value("3 4 7 9 5 4 -1 4 8 7 8 5 4 3.125 0 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix4>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::MatrixTest)
