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

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Algorithms/Svd.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test { namespace {

struct SvdTest: Corrade::TestSuite::Tester {
    explicit SvdTest();

    template<class T> void test();
    void decomposeRotationShear();
};

template<class T> using Matrix5x8 = RectangularMatrix<5, 8, T>;
template<class T> using Matrix8 = Matrix<8, T>;
template<class T> using Matrix5 = Matrix<5, T>;
template<class T> using Vector8 = Vector<8, T>;
template<class T> using Vector5 = Vector<5, T>;

SvdTest::SvdTest() {
    addTests({&SvdTest::test<Float>,
              &SvdTest::test<Double>,
              &SvdTest::decomposeRotationShear});
}

template<class T> void SvdTest::test() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    constexpr const Matrix5x8<T> a{
        Vector8<T>{T{22}, T{14}, T{ -1}, T{-3}, T{ 9}, T{ 9}, T{ 2}, T{ 4}},
        Vector8<T>{T{10}, T{ 7}, T{ 13}, T{-2}, T{ 8}, T{ 1}, T{-6}, T{ 5}},
        Vector8<T>{T{ 2}, T{10}, T{ -1}, T{13}, T{ 1}, T{-7}, T{ 6}, T{ 0}},
        Vector8<T>{T{ 3}, T{ 0}, T{-11}, T{-2}, T{-2}, T{ 5}, T{ 5}, T{-2}},
        Vector8<T>{T{ 7}, T{ 8}, T{  3}, T{ 4}, T{ 4}, T{-1}, T{ 1}, T{ 2}}};
    const Vector5<T> expected(std::sqrt(T{1248}), T{0}, T{20}, std::sqrt(T{384}), T{0});

    Matrix5x8<T> u{Magnum::NoInit};
    Vector5<T> w{Magnum::NoInit};
    Matrix5<T> v{Magnum::NoInit};
    std::tie(u, w, v) = Algorithms::svd(a);

    /* Test composition */
    Matrix8<T> u2{u[0], u[1], u[2], u[3], u[4], Vector8<T>{}, Vector8<T>{}, Vector8<T>{}};
    Matrix5x8<T> w2 = Matrix5x8<T>::fromDiagonal(w);
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF((std::is_same<T, Double>::value) && u2*w2*v.transposed() != a,
            "Some strange problems with Double on recent Emscripten versions "
            "(1.36.5 worked fine, 1.37.1 works fine on larger optimization "
            "levels, not on -O1).");
        #endif
        CORRADE_COMPARE(u2*w2*v.transposed(), a);
    }

    /* Test that V is unitary */
    CORRADE_COMPARE(v*v.transposed(), Matrix5<T>{IdentityInit});
    CORRADE_COMPARE(v.transposed()*v, Matrix5<T>{IdentityInit});

    /* Test W */
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        CORRADE_EXPECT_FAIL_IF((std::is_same<T, Double>::value && w != expected),
            "Some strange problems with Double on recent Emscripten versions "
            "(1.36.5 worked fine, 1.37.1 worked fine on larger optimization "
            "levels, not on -O1, 1.37.5 works fine again).");
        #endif
        CORRADE_COMPARE(w, expected);
    }
}

void SvdTest::decomposeRotationShear() {
    typedef Math::Matrix4<Float> Matrix4;
    typedef Math::Matrix3x3<Float> Matrix3x3;
    typedef Math::Vector3<Float> Vector3;

    using namespace Math::Literals;

    Matrix4 a = Matrix4::scaling({1.5f, 2.0f, 1.0f})*Matrix4::rotationZ(35.0_degf);

    Matrix3x3 u{Magnum::NoInit};
    Vector3 w{Magnum::NoInit};
    Matrix3x3 v{Magnum::NoInit};
    std::tie(u, w, v) = Algorithms::svd(a.rotationScaling());

    CORRADE_COMPARE(u*Matrix3x3::fromDiagonal(w)*v.transposed(), a.rotationScaling());

    /* U contains a flipped sign for Z, use it to remove the sign from the
       transposed rotation matrix V */
    CORRADE_COMPARE(w, (Vector3{1.5f, 2.0f, 1.0f}));
    CORRADE_COMPARE(Matrix4::from(u*v.transposed(), {}), Matrix4::rotationZ(35.0_degf));
}

}}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::SvdTest)
