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

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Algorithms/Qr.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test { namespace {

struct QrTest: Corrade::TestSuite::Tester {
    explicit QrTest();

    void test();
    void decomposeRotationShear();
};

using namespace Math::Literals;

typedef Matrix3x3<Float> Matrix3x3;
typedef Vector3<Float> Vector3;
typedef Matrix4<Float> Matrix4;

QrTest::QrTest() {
    addTests({&QrTest::test,
              &QrTest::decomposeRotationShear});
}

void QrTest::test() {
    Matrix3x3 a{Vector3{  0.0f,   3.0f,   4.0f},
                Vector3{-20.0f,  27.0f,  11.0f},
                Vector3{-14.0f,  -4.0f,  -2.0f}};

    std::pair<Matrix3x3, Matrix3x3> qr = Algorithms::qr(a);

    auto qExpected = Matrix3x3{Vector3{  0.0f,  15.0f, 20.0f},
                               Vector3{-20.0f,  12.0f, -9.0f},
                               Vector3{-15.0f, -16.0f, 12.0f}}/25.0f;
    CORRADE_COMPARE(qr.first, qExpected);

    Matrix3x3 rExpected{Vector3{ 5.0f,  0.0f,  0.0f},
                        Vector3{25.0f, 25.0f,  0.0f},
                        Vector3{-4.0f, 10.0f, 10.0f}};
    CORRADE_COMPARE(qr.second, rExpected);
}

void QrTest::decomposeRotationShear() {
    Matrix4 a = Matrix4::scaling({1.5f, 2.0f, 1.0f})*Matrix4::rotationZ(35.0_degf);

    std::pair<Matrix3x3, Matrix3x3> qr = Algorithms::qr(a.rotationScaling());
    CORRADE_COMPARE(qr.first*qr.second, a.rotationScaling());

    auto q4 = Matrix4::from(qr.first, {});
    auto r4 = Matrix4::from(qr.second, {});

    CORRADE_COMPARE(q4, Matrix4::rotationZ(43.03357_degf));
    CORRADE_COMPARE(r4.scaling(), (Vector3{1.68099f, 1.85048f, 1.0f}));
    CORRADE_COMPARE(r4.rotationShear(), Matrix4::shearingXZ(0.274077f, 0.0f).rotationShear());
}

}}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::QrTest)
