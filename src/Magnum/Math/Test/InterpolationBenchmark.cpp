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

#ifndef CORRADE_NO_ASSERT
#define CORRADE_NO_ASSERT
#endif

#include "Magnum/Math/DualQuaternion.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct InterpolationBenchmark: Corrade::TestSuite::Tester {
    explicit InterpolationBenchmark();

    void baseline();
    void quaternionLerp();
    void quaternionLerpShortestPath();
    void quaternionSlerp();
    void quaternionSlerpShortestPath();
    void dualQuaternionSclerp();
    void dualQuaternionSclerpShortestPath();
};

using namespace Math::Literals;

typedef Math::Quaternion<Float> Quaternion;
typedef Math::DualQuaternion<Float> DualQuaternion;
typedef Math::Vector3<Float> Vector3;

InterpolationBenchmark::InterpolationBenchmark() {
    addBenchmarks({&InterpolationBenchmark::baseline,
                   &InterpolationBenchmark::quaternionLerp,
                   &InterpolationBenchmark::quaternionLerpShortestPath,
                   &InterpolationBenchmark::quaternionSlerp,
                   &InterpolationBenchmark::quaternionSlerpShortestPath,
                   &InterpolationBenchmark::dualQuaternionSclerp,
                   &InterpolationBenchmark::dualQuaternionSclerpShortestPath}, 100);
}

void InterpolationBenchmark::baseline() {
    Quaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(10000) {
        c += Quaternion{};
        t += 0.0002f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::quaternionLerp() {
    auto a = Quaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(10000) {
        c += lerp(a, b, t);
        t += 0.0002f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::quaternionLerpShortestPath() {
    auto a = Quaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(10000) {
        c += lerpShortestPath(a, b, t);
        t += 0.0002f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::quaternionSlerp() {
    auto a = Quaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(10000) {
        c += slerp(a, b, t);
        t += 0.0002f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::quaternionSlerpShortestPath() {
    auto a = Quaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = Quaternion::rotation(0.0_degf, Vector3::zAxis());
    Quaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(10000) {
        c += slerpShortestPath(a, b, t);
        t += 0.0002f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::dualQuaternionSclerp() {
    auto a = DualQuaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = DualQuaternion::rotation(0.0_degf, Vector3::zAxis());
    DualQuaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(1000) {
        c += sclerp(a, b, t);
        t += 0.001f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

void InterpolationBenchmark::dualQuaternionSclerpShortestPath() {
    auto a = DualQuaternion::rotation(225.0_degf, Vector3::zAxis());
    auto b = DualQuaternion::rotation(0.0_degf, Vector3::zAxis());
    DualQuaternion c;
    Float t = 0.0f;
    CORRADE_BENCHMARK(1000) {
        c += sclerpShortestPath(a, b, t);
        t += 0.001f;
    }

    CORRADE_VERIFY(!c.isNormalized());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::InterpolationBenchmark)
