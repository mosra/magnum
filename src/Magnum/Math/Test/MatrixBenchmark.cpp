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
#include "Magnum/Math/Algorithms/GaussJordan.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct MatrixBenchmark: Corrade::TestSuite::Tester {
    explicit MatrixBenchmark();

    void multiply3();
    void multiply4();

    void comatrix3();
    void invert3();
    void invert3GaussJordan();
    void invert3Rigid();
    void invert3Orthogonal();
    void comatrix4();
    void invert4();
    void invert4GaussJordan();
    void invert4Rigid();
    void invert4Orthogonal();

    void transformVector3();
    void transformPoint3();
    void transformVector4();
    void transformPoint4();
};

MatrixBenchmark::MatrixBenchmark() {
    addBenchmarks({&MatrixBenchmark::multiply3,
                   &MatrixBenchmark::multiply4}, 500);

    addBenchmarks({&MatrixBenchmark::comatrix3,
                   &MatrixBenchmark::invert3,
                   &MatrixBenchmark::invert3GaussJordan,
                   &MatrixBenchmark::invert3Rigid,
                   &MatrixBenchmark::invert3Orthogonal,
                   &MatrixBenchmark::comatrix4,
                   &MatrixBenchmark::invert4,
                   &MatrixBenchmark::invert4GaussJordan,
                   &MatrixBenchmark::invert4Rigid,
                   &MatrixBenchmark::invert4Orthogonal}, 50);

    addBenchmarks({&MatrixBenchmark::transformVector3,
                   &MatrixBenchmark::transformPoint3,
                   &MatrixBenchmark::transformVector4,
                   &MatrixBenchmark::transformPoint4}, 1000);
}

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Matrix3<Float> Matrix3;

enum: std::size_t { Repeats = 10000 };

using namespace Literals;

const Matrix3 Data3Orthogonal = Matrix3::rotation(134.7_degf);
const Matrix3 Data3Rigid = Data3Orthogonal*Matrix3::translation(Vector2::yAxis());
const Matrix3 Data3 = Data3Orthogonal*Matrix3::scaling(Vector2{2.5f})*Matrix3::translation(Vector2::yAxis());

const Matrix4 Data4Orthogonal = Matrix4::rotation(134.7_degf, Vector3{1.0f, 3.0f, -1.4f}.normalized());
const Matrix4 Data4Rigid = Data4Orthogonal*Matrix4::translation(Vector3::zAxis());
const Matrix4 Data4 = Data4Orthogonal*Matrix4::scaling(Vector3{2.5f})*Matrix4::translation(Vector3::zAxis());

void MatrixBenchmark::multiply3() {
    Matrix3 a = Data3;
    CORRADE_BENCHMARK(Repeats) {
        a = a*a;
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::multiply4() {
    Matrix4 a = Data4;
    CORRADE_BENCHMARK(Repeats) {
        a = a*a;
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::comatrix3() {
    Matrix3 a = Data3;
    CORRADE_BENCHMARK(Repeats) {
        a = a.comatrix();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert3() {
    Matrix3 a = Data3;
    CORRADE_BENCHMARK(Repeats) {
        a = a.inverted();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert3GaussJordan() {
    Matrix3 a = Data3;
    CORRADE_BENCHMARK(Repeats) {
        a = Math::Algorithms::gaussJordanInverted(a);
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert3Rigid() {
    Matrix3 a = Data3Rigid;
    CORRADE_BENCHMARK(Repeats) {
        a = a.invertedRigid();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert3Orthogonal() {
    Matrix3 a = Data3Orthogonal;
    CORRADE_BENCHMARK(Repeats) {
        a = a.invertedOrthogonal();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::comatrix4() {
    Matrix4 a = Data4;
    CORRADE_BENCHMARK(Repeats) {
        a = a.comatrix();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert4() {
    Matrix4 a = Data4;
    CORRADE_BENCHMARK(Repeats) {
        a = a.inverted();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert4GaussJordan() {
    Matrix4 a = Data4;
    CORRADE_BENCHMARK(Repeats) {
        a = Math::Algorithms::gaussJordanInverted(a);
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert4Rigid() {
    Matrix4 a = Data4Rigid;
    CORRADE_BENCHMARK(Repeats) {
        a = a.invertedRigid();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::invert4Orthogonal() {
    Matrix4 a = Data4Orthogonal;
    CORRADE_BENCHMARK(Repeats) {
        a = a.invertedOrthogonal();
    }

    CORRADE_VERIFY(a.toVector().sum() != 0);
}

void MatrixBenchmark::transformVector3() {
    Vector2 a{3.0f, -2.2f};
    CORRADE_BENCHMARK(Repeats) {
        a = Data3.transformVector(a);
    }

    CORRADE_VERIFY(a.sum() != 0);
}

void MatrixBenchmark::transformPoint3() {
    Vector2 a{3.0f, -2.2f};
    CORRADE_BENCHMARK(Repeats) {
        a = Data3.transformPoint(a);
    }

    CORRADE_VERIFY(a.sum() != 0);
}

void MatrixBenchmark::transformVector4() {
    Vector3 a{1.0f, 3.0f, -2.2f};
    CORRADE_BENCHMARK(Repeats) {
        a = Data4.transformVector(a);
    }

    CORRADE_VERIFY(a.sum() != 0);
}

void MatrixBenchmark::transformPoint4() {
    Vector3 a{1.0f, 3.0f, -2.2f};
    CORRADE_BENCHMARK(Repeats) {
        a = Data4.transformVector(a);
    }

    CORRADE_VERIFY(a.sum() != 0);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::MatrixBenchmark)
