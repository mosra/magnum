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

#include <tuple>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Algorithms/Qr.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test {

struct QrTest: Corrade::TestSuite::Tester {
    explicit QrTest();

    void test();
};

typedef Matrix3<Float> Matrix3;

QrTest::QrTest() {
    addTests({&QrTest::test});
}

void QrTest::test() {
    Matrix3 a{{  0.0f,   3.0f,   4.0f},
              {-20.0f,  27.0f,  11.0f},
              {-14.0f,  -4.0f,  -2.0f}};

    Matrix3 q, r;
    std::tie(q, r) = Algorithms::qr(a);

    CORRADE_COMPARE(q*r, a);

    Matrix3 qExpected = Matrix3{{  0.0f,  15.0f, 20.0f},
                                {-20.0f,  12.0f, -9.0f},
                                {-15.0f, -16.0f, 12.0f}}/25.0f;
    CORRADE_COMPARE(q, qExpected);

    Matrix3 rExpected{{ 5.0f,  0.0f,  0.0f},
                      {25.0f, 25.0f,  0.0f},
                      {-4.0f, 10.0f, 10.0f}};
    CORRADE_COMPARE(r, rExpected);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::QrTest)
