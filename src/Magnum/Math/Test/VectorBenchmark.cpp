/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct VectorBenchmark: Corrade::TestSuite::Tester {
    explicit VectorBenchmark();

    void dot();

    template<class T> void cross2Baseline();
    void cross2();

    template<class T> void cross3Baseline();
    void cross3();
};

VectorBenchmark::VectorBenchmark() {
    addBenchmarks({
        &VectorBenchmark::dot,

        &VectorBenchmark::cross2Baseline<Float>,
        &VectorBenchmark::cross2Baseline<Double>,
        &VectorBenchmark::cross2,

        &VectorBenchmark::cross3Baseline<Float>,
        &VectorBenchmark::cross3Baseline<Double>,
        &VectorBenchmark::cross3,
    }, 500);
}

typedef Math::Constants<Float> Constants;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;

enum: std::size_t { Repeats = 100000 };

using namespace Literals;

void VectorBenchmark::dot() {
    Vector3 a{1.3f, -1.1f, 1.0f};
    Vector3 b{4.5f, 3.2f, 7.3f};
    CORRADE_COMPARE(Math::dot(a, b), 9.63f);

    CORRADE_BENCHMARK(Repeats) {
        a.x() = Math::dot(a, b);
    }

    CORRADE_COMPARE(a, (Vector3{Constants::inf(), -1.1f, 1.0f}));
}

template<class T> inline T cross2Baseline(const Vector2& v1, const Vector2& v2) {
    T v1x = T(v1.x()),
      v1y = T(v1.y());
    T v2x = T(v2.x()),
      v2y = T(v2.y());
    return T(v1x*v2y - v1y*v2x);
}

template<class T> void VectorBenchmark::cross2Baseline() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    Vector2 a{1.3f, -1.1f};
    Vector2 b{4.5f, 3.2f};
    CORRADE_COMPARE(Test::cross2Baseline<T>(a, b), 9.11f);

    CORRADE_BENCHMARK(Repeats) {
        a.x() = Test::cross2Baseline<T>(a, b);
    }

    CORRADE_COMPARE(a, (Vector2{Constants::inf(), -1.1f}));
}

void VectorBenchmark::cross2() {
    Vector2 a{1.3f, -1.1f};
    Vector2 b{4.5f, 3.2f};
    CORRADE_COMPARE(Math::cross(a, b), 9.11f);

    CORRADE_BENCHMARK(Repeats) {
        a.x() = Math::cross(a, b);
    }

    CORRADE_COMPARE(a, (Vector2{Constants::inf(), -1.1f}));
}

template<class T> inline Vector3 cross3Baseline(const Vector3& v1, const Vector3& v2) {
    T v1x = T(v1.x()),
      v1y = T(v1.y()),
      v1z = T(v1.z());
    T v2x = T(v2.x()),
      v2y = T(v2.y()),
      v2z = T(v2.z());
    return Vector3(v1y*v2z - v1z*v2y,
                   v1z*v2x - v1x*v2z,
                   v1x*v2y - v1y*v2x);
}

template<class T> void VectorBenchmark::cross3Baseline() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    Vector3 a{1.3f, -1.1f, 1.0f};
    Vector3 b{4.5f, 3.2f, 7.3f};
    CORRADE_COMPARE(Test::cross3Baseline<T>(a, b),
        (Vector3{-11.23f, -4.99f, 9.11f}));

    CORRADE_BENCHMARK(Repeats) {
        a = Test::cross3Baseline<T>(a, b);
    }

    CORRADE_VERIFY(a != a);
}

void VectorBenchmark::cross3() {
    Vector3 a{1.3f, -1.1f, 1.0f};
    Vector3 b{4.5f, 3.2f, 7.3f};
    CORRADE_COMPARE(Math::cross(a, b),
        (Vector3{-11.23f, -4.99f, 9.11f}));

    CORRADE_BENCHMARK(Repeats) {
        a = Math::cross(a, b);
    }

    CORRADE_VERIFY(a != a);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::VectorBenchmark)
