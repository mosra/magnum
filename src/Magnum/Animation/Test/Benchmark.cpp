/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Animation/Track.h"

namespace Magnum { namespace Animation { namespace Test {

struct Benchmark: TestSuite::Tester {
    explicit Benchmark();

    void interpolateEmpty();
    void interpolateInterleaved();
    void interpolateInterleavedStrict();

    void atEmpty();
    void at();
    void atHint();
    void atStrict();
    void atStrictInterleaved();
    void atStrictInterleavedDirectInterpolator();

    Containers::Array<Float> _keys;
    Containers::Array<Int> _values;
    Containers::Array<std::pair<Float, Int>> _interleaved;
    Containers::StridedArrayView<const Float> _keysInterleaved;
    Containers::StridedArrayView<const Int> _valuesInterleaved;
    TrackView<Float, Int> _track;
    TrackView<Float, Int> _trackInterleaved;
};

namespace {
    enum: std::size_t { DataSize = 2000 };
}

Benchmark::Benchmark() {
    addBenchmarks({&Benchmark::interpolateEmpty,
                   &Benchmark::interpolateInterleaved,
                   &Benchmark::interpolateInterleavedStrict,

                   &Benchmark::atEmpty,
                   &Benchmark::at,
                   &Benchmark::atHint,
                   &Benchmark::atStrict,
                   &Benchmark::atStrictInterleaved,
                   &Benchmark::atStrictInterleavedDirectInterpolator}, 10);

    _keys = Containers::Array<Float>{DataSize};
    _values = Containers::Array<Int>{Containers::DirectInit, DataSize, 1};
    _interleaved = Containers::Array<std::pair<Float, Int>>{Containers::DirectInit, DataSize, 0.0f, 1};
    for(std::size_t i = 0; i != DataSize; ++i)
        _keys[i] = _interleaved[i].first = Float(i)*3.1254f;

    _keysInterleaved = {&_interleaved[0].first, _interleaved.size(), sizeof(std::pair<Float, Int>)};
    _valuesInterleaved = {&_interleaved[0].second, _interleaved.size(), sizeof(std::pair<Float, Int>)};

    _track = TrackView<Float, Int>{
        Containers::arrayView(_keys), Containers::arrayView(_values), Math::select};
    _trackInterleaved = {_keysInterleaved, _valuesInterleaved, Math::select};
}

void Benchmark::interpolateEmpty() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += interpolate<Float, Float>(nullptr, nullptr, {}, {}, Math::select, i, hint);
    }
    CORRADE_COMPARE(result, 0);
}

void Benchmark::interpolateInterleaved() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += interpolate(_keysInterleaved, _valuesInterleaved, {}, {}, Math::select, i, hint);
    }
    CORRADE_COMPARE(result, 125000);
}

void Benchmark::interpolateInterleavedStrict() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += interpolateStrict(_keysInterleaved, _valuesInterleaved, Math::select, i, hint);
    }
    CORRADE_COMPARE(result, 125000);
}

void Benchmark::atEmpty() {
    TrackView<Float, Int> empty{nullptr, nullptr, Math::select};

    Int result{};
    CORRADE_BENCHMARK(250)
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += empty.at(i);
    CORRADE_COMPARE(result, 0);
}

void Benchmark::at() {
    Int result{};
    CORRADE_BENCHMARK(250)
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += _track.at(i);
    CORRADE_COMPARE(result, 125000);
}

void Benchmark::atHint() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += _track.at(i, hint);
    }
    CORRADE_COMPARE(result, 125000);
}

void Benchmark::atStrict() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += _track.atStrict(i, hint);
    }
    CORRADE_COMPARE(result, 125000);
}

void Benchmark::atStrictInterleaved() {
    Int result{};
    CORRADE_BENCHMARK(250) {
        std::size_t hint{};
        for(Float i = 0.0f; i < 500.0f; i += 1.0f)
            result += _trackInterleaved.atStrict(i, hint);
    }
    CORRADE_COMPARE(result, 125000);
}

}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::Benchmark)
