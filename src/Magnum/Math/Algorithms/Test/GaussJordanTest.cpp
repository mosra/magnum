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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Algorithms/GaussJordan.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test { namespace {

struct GaussJordanTest: Corrade::TestSuite::Tester {
    explicit GaussJordanTest();

    void test();
    void singular();
    void inverted();
};

typedef Matrix<4, Float> Matrix4x4;
typedef Vector<4, Float> Vector4;

GaussJordanTest::GaussJordanTest() {
    addTests({&GaussJordanTest::test,
              &GaussJordanTest::singular,
              &GaussJordanTest::inverted});
}

void GaussJordanTest::test() {
    /* Basic matrix inversion */
    const Matrix4x4 m{Vector4{3.0f,  5.0f, 8.0f, 4.0f},
                      Vector4{4.0f,  4.0f, 7.0f, 3.0f},
                      Vector4{7.0f, -1.0f, 8.0f, 0.0f},
                      Vector4{9.0f,  4.0f, 5.0f, 9.0f}};

    const Matrix4x4 expected{Vector4{-60/103.0f,   71/103.0f,  -4/103.0f,  3/103.0f},
                             Vector4{-66/103.0f,  109/103.0f, -25/103.0f, -7/103.0f},
                             Vector4{177/412.0f,  -97/206.0f,  53/412.0f, -7/206.0f},
                             Vector4{259/412.0f, -185/206.0f,  31/412.0f, 27/206.0f}};

    Matrix4x4 a2{m};
    Matrix4x4 inverse{Math::IdentityInit};
    CORRADE_VERIFY(gaussJordanInPlace(a2, inverse));

    CORRADE_COMPARE(inverse, expected);
    CORRADE_COMPARE(m*inverse, Matrix4x4{});
}

void GaussJordanTest::singular() {
    Matrix4x4 a(Vector4(1.0f, 2.0f,  3.0f,  4.0f),
                Vector4(2.0f, 3.0f, -7.0f, 11.0f),
                Vector4(2.0f, 4.0f,  6.0f,  8.0f),
                Vector4(1.0f, 2.0f,  7.0f, 40.0f));
    RectangularMatrix<4, 1, Float> t;

    CORRADE_VERIFY(!gaussJordanInPlaceTransposed(a, t));
}

void GaussJordanTest::inverted() {
    const Matrix4x4 m{Vector4{3.0f,  5.0f, 8.0f, 4.0f},
                      Vector4{4.0f,  4.0f, 7.0f, 3.0f},
                      Vector4{7.0f, -1.0f, 8.0f, 0.0f},
                      Vector4{9.0f,  4.0f, 5.0f, 9.0f}};

    const Matrix4x4 expected{Vector4{-60/103.0f,   71/103.0f,  -4/103.0f,  3/103.0f},
                             Vector4{-66/103.0f,  109/103.0f, -25/103.0f, -7/103.0f},
                             Vector4{177/412.0f,  -97/206.0f,  53/412.0f, -7/206.0f},
                             Vector4{259/412.0f, -185/206.0f,  31/412.0f, 27/206.0f}};

    const Matrix4x4 inverse = gaussJordanInverted(m);
    CORRADE_COMPARE(inverse, expected);
    CORRADE_COMPARE(inverse*m, Matrix4x4{});
}

}}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::GaussJordanTest)
