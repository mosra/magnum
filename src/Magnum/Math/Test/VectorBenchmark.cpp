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

#include "Magnum/Math/Vector3.h"

#ifdef CORRADE_TARGET_SSE2
#include <xmmintrin.h>
#endif

namespace Magnum { namespace Math { namespace Test { namespace {

struct VectorBenchmark: Corrade::TestSuite::Tester {
    explicit VectorBenchmark();

    void dot();

    template<class T> void cross2Baseline();
    void cross2();

    template<class T> void cross3Baseline();
    void cross3();
    #ifdef CORRADE_TARGET_SSE2
    void cross3SseNaive();
    void cross3SseOneShuffleLess();
    #endif
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
        #ifdef CORRADE_TARGET_SSE2
        &VectorBenchmark::cross3SseNaive,
        &VectorBenchmark::cross3SseOneShuffleLess,
        #endif
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

#ifdef CORRADE_TARGET_SSE2
inline Vector3 crossSseNaive(const Vector3& a, const Vector3& b) {
    union {
        __m128 v;
        Float s[4];
    };

    const __m128 aa = _mm_set_ps(0.0f, a[2], a[1], a[0]);
    const __m128 bb = _mm_set_ps(0.0f, b[2], b[1], b[0]);

    v = _mm_sub_ps(
        _mm_mul_ps(_mm_shuffle_ps(aa, aa, _MM_SHUFFLE(3, 0, 2, 1)),
                   _mm_shuffle_ps(bb, bb, _MM_SHUFFLE(3, 1, 0, 2))),
        _mm_mul_ps(_mm_shuffle_ps(aa, aa, _MM_SHUFFLE(3, 1, 0, 2)),
                   _mm_shuffle_ps(bb, bb, _MM_SHUFFLE(3, 0, 2, 1))));
    return {s[0], s[1], s[2]};
}

/* https://twitter.com/sjb3d/status/563640846671953920. Originally the
   Math::cross() was doing this, implemented as
    gather<'y', 'z', 'x'>(a*gather<'y', 'z', 'x'>(b) -
                          b*gather<'y', 'z', 'x'>(a))
   but while slightly faster in Release (on GCC at least) than the
   straightforward version, it was insanely slow in Debug. */
inline Vector3 crossSseOneShuffleLess(const Vector3& a, const Vector3& b) {
    union {
        __m128 v;
        Float s[4];
    };

    const __m128 aa = _mm_set_ps(0.0f, a[2], a[1], a[0]);
    const __m128 bb = _mm_set_ps(0.0f, b[2], b[1], b[0]);
    const __m128 cc = _mm_sub_ps(
        _mm_mul_ps(aa, _mm_shuffle_ps(bb, bb, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(bb, _mm_shuffle_ps(aa, aa, _MM_SHUFFLE(3, 0, 2, 1))));

    v = _mm_shuffle_ps(cc, cc, _MM_SHUFFLE(3, 0, 2, 1));
    return {s[0], s[1], s[2]};
}

void VectorBenchmark::cross3SseNaive() {
    Vector3 a{1.3f, -1.1f, 1.0f};
    Vector3 b{4.5f, 3.2f, 7.3f};
    CORRADE_COMPARE(Test::crossSseNaive(a, b),
        (Vector3{-11.23f, -4.99f, 9.11f}));

    CORRADE_BENCHMARK(Repeats) {
        a = Test::crossSseNaive(a, b);
    }

    CORRADE_VERIFY(a != a);
}

void VectorBenchmark::cross3SseOneShuffleLess() {
    Vector3 a{1.3f, -1.1f, 1.0f};
    Vector3 b{4.5f, 3.2f, 7.3f};
    CORRADE_COMPARE(Test::crossSseOneShuffleLess(a, b),
        (Vector3{-11.23f, -4.99f, 9.11f}));

    CORRADE_BENCHMARK(Repeats) {
        a = Test::crossSseOneShuffleLess(a, b);
    }

    CORRADE_VERIFY(a != a);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::VectorBenchmark)
