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
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Functions.h"

#ifdef CORRADE_TARGET_SSE2
#include <xmmintrin.h>
#endif

namespace Magnum { namespace Math { namespace Test { namespace {

struct FunctionsBenchmark: Corrade::TestSuite::Tester {
    explicit FunctionsBenchmark();

    void sqrt();
    template<class T> void sqrtInverted();
    template<class T> void sqrtInvertedFast();
    #ifdef CORRADE_TARGET_SSE2
    void sqrtSse();
    void sqrtSseFromInverted();
    void sqrtInvertedSse();
    #endif

    void sinCosSeparate();
    void sinCosCombined();
};

FunctionsBenchmark::FunctionsBenchmark() {
    addBenchmarks({
        &FunctionsBenchmark::sqrt,
        &FunctionsBenchmark::sqrtInverted<Float>,
        &FunctionsBenchmark::sqrtInverted<Double>,
        &FunctionsBenchmark::sqrtInvertedFast<Float>,
        &FunctionsBenchmark::sqrtInvertedFast<Double>,
        #ifdef CORRADE_TARGET_SSE2
        &FunctionsBenchmark::sqrtSse,
        &FunctionsBenchmark::sqrtSseFromInverted,
        &FunctionsBenchmark::sqrtInvertedSse,
        #endif
    }, 500);

    addBenchmarks({&FunctionsBenchmark::sinCosSeparate,
                   &FunctionsBenchmark::sinCosCombined}, 100);
}

typedef Math::Constants<Float> Constants;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;

enum: std::size_t { Repeats = 100000 };

using namespace Literals;

void FunctionsBenchmark::sqrt() {
    CORRADE_COMPARE(Math::sqrt(25.0f), 5.0f);

    Float a = 1000000.0f;

    CORRADE_BENCHMARK(Repeats) {
        a = Math::sqrt(a);
    }

    CORRADE_COMPARE(a, 1.0f);
}

template<class T> void FunctionsBenchmark::sqrtInverted() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_COMPARE(Math::sqrtInverted(T(25.0)), 1/T(5.0));
    T a = T(1000000.0);

    CORRADE_BENCHMARK(Repeats) {
        a = Math::sqrtInverted(a);
    }

    CORRADE_COMPARE(a, T(1.0));
}

/* https://en.wikipedia.org/wiki/Fast_inverse_square_root */
inline Float sqrtInvertedFast(Float a) {
    const Float x2 = a*0.5f;
    Float y  = a;
    Int i  = reinterpret_cast<Int&>(y);
    i = 0x5f375a86 - (i >> 1); /* the more precise variant */
    y = reinterpret_cast<Float&>(i);
    y = y*(1.5f - (x2*y*y));
    return y;
}
inline Double sqrtInvertedFast(Double a) {
    const Double x2 = a*0.5;
    Double y  = a;
    Long i  = reinterpret_cast<Long&>(y);
    i = 0x5fe6eb50c7b537a9 - (i >> 1);
    y = reinterpret_cast<Double&>(i);
    y = y*(1.5 - (x2*y*y));
    return y;
}

template<class T> void FunctionsBenchmark::sqrtInvertedFast() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_COMPARE_WITH(Test::sqrtInvertedFast(T(25.0)), 1/T(5.0),
        Corrade::TestSuite::Compare::around(T(0.0005)));

    T a = T(1000000.0);

    CORRADE_BENCHMARK(Repeats) {
        a = Test::sqrtInvertedFast(a);
    }

    CORRADE_COMPARE_WITH(a, T(1.0),
        Corrade::TestSuite::Compare::around(T(0.002)));
}

#ifdef CORRADE_TARGET_SSE2
inline Float sqrtSse(Float a) {
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(a)));
}

void FunctionsBenchmark::sqrtSse() {
    CORRADE_COMPARE(Test::sqrtSse(25.0f), 5.0f);

    Float a = 1000000.0f;

    CORRADE_BENCHMARK(Repeats) {
        a = Test::sqrtSse(a);
    }

    CORRADE_COMPARE(a, 1.0f);
}

inline Float sqrtSseFromInverted(Float a) {
    const __m128 aa = _mm_set_ss(a);
    return _mm_cvtss_f32(_mm_mul_ss(aa, _mm_rsqrt_ss(aa)));
}

void FunctionsBenchmark::sqrtSseFromInverted() {
    CORRADE_COMPARE_WITH(Test::sqrtSseFromInverted(25.0f), 5.0f,
        Corrade::TestSuite::Compare::around(0.0005f));

    Float a = 1000000.0f;

    CORRADE_BENCHMARK(Repeats) {
        a = Test::sqrtSseFromInverted(a);
    }

    CORRADE_COMPARE_WITH(a, 1.0f,
        Corrade::TestSuite::Compare::around(0.0002f));
}

inline Float sqrtInvertedSse(Float a) {
    return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(a)));
}

void FunctionsBenchmark::sqrtInvertedSse() {
    CORRADE_COMPARE_WITH(Test::sqrtInvertedSse(25.0f), 1/5.0f,
        Corrade::TestSuite::Compare::around(0.00002f));

    Float a = 1000000.1f;

    CORRADE_BENCHMARK(Repeats) {
        a = Test::sqrtInvertedSse(a);
    }

    CORRADE_COMPARE_WITH(a, 1.0f,
        Corrade::TestSuite::Compare::around(0.0003f));
}
#endif

void FunctionsBenchmark::sinCosSeparate() {
    Float sin{}, cos{}, a{};
    CORRADE_BENCHMARK(1000) {
        sin += Math::sin(Rad(a));
        cos += Math::cos(Rad(a));
        a += 0.1f;
    }

    CORRADE_COMPARE_AS(a, 10.0f, Corrade::TestSuite::Compare::Greater);
}

void FunctionsBenchmark::sinCosCombined() {
    Float sin{}, cos{}, a{};
    CORRADE_BENCHMARK(1000) {
        auto sincos = Math::sincos(Rad(a));
        sin += sincos.first;
        cos += sincos.second;
        a += 0.1f;
    }

    CORRADE_COMPARE_AS(a, 10.0f, Corrade::TestSuite::Compare::Greater);
}


}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FunctionsBenchmark)
