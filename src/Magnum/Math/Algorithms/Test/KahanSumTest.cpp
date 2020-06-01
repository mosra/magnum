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

#include <numeric>
#include <Corrade/Containers/Array.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Algorithms/KahanSum.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test { namespace {

struct KahanSumTest: TestSuite::Tester {
    explicit KahanSumTest();

    void floats();
    void integers();
    void iterative();

    void accumulate100kFloats();
    void accumulate100kDoubles();
    void kahan100kFloats();

};

KahanSumTest::KahanSumTest() {
    addTests({&KahanSumTest::floats,
              &KahanSumTest::integers,
              &KahanSumTest::iterative});

    addBenchmarks({&KahanSumTest::accumulate100kFloats,
                   &KahanSumTest::accumulate100kDoubles,
                   &KahanSumTest::kahan100kFloats}, 50);
}

/* Custom iterator class to avoid allocating half a gigabyte for hundred
   million values */
template<class T> struct Iterator
    #ifdef CORRADE_MSVC2017_COMPATIBILITY
    /* This is needed on MSVC in order to have std::accumulate() eat these
       iterators properly. However, std::iterator is being deprecated in C++17
       and the go-to way of typedef'ing all the things doesn't work (the
       compiler complains about iterator_category not being defined in
       std::iterator_traits) */
    : std::iterator<std::forward_iterator_tag, T>
    #endif
{
    explicit Iterator(T value, std::size_t i = 0): _value{value}, _i{i} {}

    /* MSVC 2017 needs operator==() as well */
    bool operator==(const Iterator& other) const { return _i == other._i; }
    bool operator!=(const Iterator& other) const { return _i != other._i; }

    Iterator& operator++() {
        ++_i;
        return *this;
    }

    T operator*() const { return _value; }

    private:
        T _value{};
        std::size_t _i{};
};

void KahanSumTest::floats() {
    Iterator<Float> begin{1.0f};
    Iterator<Float> end{1.0f, 100000000};

    CORRADE_COMPARE(kahanSum(begin, end), 1.0e8f);
    float sum = std::accumulate(begin, end, 0.0f);
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        /* Last tested: emscripten 1.37.18, asm.js. Wasm gives proper result. */
        CORRADE_EXPECT_FAIL_IF(sum == 1.0e8f,
            "Gives the same result as kahanSum(), might be because doubles "
            "are used internally on asm.js.");
        #endif
        CORRADE_COMPARE(sum, 1.6777216e7f);
    }
}

void KahanSumTest::integers() {
    Iterator<Int> begin{1};
    Iterator<Int> end{1, 100000};

    CORRADE_COMPARE(kahanSum(begin, end), 100000);
    CORRADE_COMPARE(std::accumulate(begin, end, 0.0f), 100000);
}

void KahanSumTest::iterative() {
    Iterator<Float> begin{1.0f};
    Iterator<Float> end{1.0f, 100000000};

    Float sum{}, sumKahan{}, c{};
    for(auto it = begin; it != end; ++it) {
        Float d = *it;
        sum += d;
        sumKahan = kahanSum(&d, &d + 1, sumKahan, &c);
    }

    /* This is very weird on Emscripten. */
    {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        /* Last tested: emscripten 1.37.1 */
        CORRADE_EXPECT_FAIL_IF(sumKahan == 1.6777216e7f,
            "Probably because of some misoptimization, happens only on -O1 "
            "and not on larger optimization levels.");
        #endif
        CORRADE_COMPARE(sumKahan, 1.0e8f);
    } {
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        /* Last tested: emscripten 1.37.1 */
        CORRADE_EXPECT_FAIL_IF(sum == 1.0e8f,
            "Gives the same result as kahanSum(), might be because doubles are "
            "used internally on asm.js. Happens only on larger optimization "
            "levels, not on -O1.");
        #endif
        CORRADE_COMPARE(sum, 1.6777216e7f);
    }
}

void KahanSumTest::accumulate100kFloats() {
    Containers::Array<Float> data(Containers::DirectInit, 100000, 1.0f);

    volatile Float a; /* to avoid optimizing the loop out */
    CORRADE_BENCHMARK(10) {
        a = std::accumulate(data.begin(), data.end(), 0.0f);
    }

    CORRADE_COMPARE(Float(a), 100000.0f);
}

void KahanSumTest::accumulate100kDoubles() {
    Containers::Array<Double> data(Containers::DirectInit, 100000, 1.0);

    volatile Double a; /* to avoid optimizing the loop out */
    CORRADE_BENCHMARK(10) {
        a = std::accumulate(data.begin(), data.end(), 0.0);
    }

    CORRADE_COMPARE(Double(a), 100000.0);
}

void KahanSumTest::kahan100kFloats() {
    Containers::Array<Float> data(Containers::DirectInit, 100000, 1.0f);

    volatile Float a; /* to avoid optimizing the loop out */
    CORRADE_BENCHMARK(10) {
        a = kahanSum(data.begin(), data.end());
    }

    CORRADE_COMPARE(Float(a), 100000.0f);
}

}}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::KahanSumTest)
