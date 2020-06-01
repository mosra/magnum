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

#include <cstring>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Animation/Easing.h"

namespace Magnum { namespace Animation { namespace Test { namespace {

struct EasingTest: TestSuite::Tester {
    explicit EasingTest();

    void bounds();
    void monotonicity();
    void symmetry();
    void values();

    void benchmark();
};

#define _c(name) #name, Easing::name
constexpr struct {
    const char* name;
    Float(*function)(Float);
} BoundsData[] {
    {_c(linear)},
    {_c(step)},
    {_c(smoothstep)},
    {_c(smootherstep)},
    {_c(quadraticIn)},
    {_c(quadraticOut)},
    {_c(quadraticInOut)},
    {_c(cubicIn)},
    {_c(cubicOut)},
    {_c(cubicInOut)},
    {_c(quarticIn)},
    {_c(quarticOut)},
    {_c(quarticInOut)},
    {_c(quinticIn)},
    {_c(quinticOut)},
    {_c(quinticInOut)},
    {_c(sineIn)},
    {_c(sineOut)},
    {_c(sineInOut)},
    {_c(circularIn)},
    {_c(circularOut)},
    {_c(circularInOut)},
    {_c(exponentialIn)},
    {_c(exponentialOut)},
    {_c(exponentialInOut)},
    /* elastic and back are out of [0, 1] bounds */
    {_c(bounceIn)},
    {_c(bounceOut)},
    {_c(bounceInOut)}
};

constexpr struct {
    const char* name;
    Float(*function)(Float);
} MonotonicityData[] {
    {_c(linear)},
    {_c(step)},
    {_c(smoothstep)},
    {_c(smootherstep)},
    {_c(quadraticIn)},
    {_c(quadraticOut)},
    {_c(quadraticInOut)},
    {_c(cubicIn)},
    {_c(cubicOut)},
    {_c(cubicInOut)},
    {_c(quarticIn)},
    {_c(quarticOut)},
    {_c(quarticInOut)},
    {_c(quinticIn)},
    {_c(quinticOut)},
    {_c(quinticInOut)},
    {_c(sineIn)},
    {_c(sineOut)},
    {_c(sineInOut)},
    {_c(circularIn)},
    {_c(circularOut)},
    {_c(circularInOut)},
    {_c(exponentialIn)},
    {_c(exponentialOut)},
    {_c(exponentialInOut)}
    /* elastic, back and bounce are not monotonic */
};

constexpr struct {
    const char* name;
    Float(*function)(Float);
    const char* symmetricName;
    Float(*symmetric)(Float);
} SymmetryData[] {
    {_c(linear), _c(linear)},
    {_c(step), _c(step)},
    {_c(smoothstep), _c(smoothstep)},
    {_c(smootherstep), _c(smootherstep)},
    {_c(quadraticIn), _c(quadraticOut)},
    {_c(quadraticInOut), _c(quadraticInOut)},
    {_c(cubicIn), _c(cubicOut)},
    {_c(cubicInOut), _c(cubicInOut)},
    {_c(quarticIn), _c(quarticOut)},
    {_c(quarticInOut), _c(quarticInOut)},
    {_c(quinticIn), _c(quinticOut)},
    {_c(quinticInOut), _c(quinticInOut)},
    {_c(sineIn), _c(sineOut)},
    {_c(sineInOut), _c(sineInOut)},
    {_c(circularIn), _c(circularOut)},
    {_c(circularInOut), _c(circularInOut)},
    {_c(exponentialIn), _c(exponentialOut)},
    {_c(exponentialInOut), _c(exponentialInOut)},
    {_c(elasticIn), _c(elasticOut)},
    {_c(elasticInOut), _c(elasticInOut)},
    {_c(backIn), _c(backOut)},
    {_c(backInOut), _c(backInOut)},
    {_c(bounceIn), _c(bounceOut)},
    {_c(bounceInOut), _c(bounceInOut)}
};

constexpr struct {
    const char* name;
    Float(*function)(Float);
    Float values[3];
} ValueData[] {
    {_c(linear), {0.25f, 0.5f, 0.75f}},
    {_c(step), {0.0f, 1.0f, 1.0f}},
    {_c(smoothstep), {0.15625f, 0.5f, 0.84375f}},
    {_c(smootherstep), {0.103516f, 0.5f, 0.896484f}},
    {_c(quadraticIn), {0.0625f, 0.25f, 0.5625f}},
    {_c(quadraticOut), {0.4375f, 0.75f, 0.9375f}},
    {_c(quadraticInOut), {0.125f, 0.5f, 0.875f}},
    {_c(cubicIn), {0.015625f, 0.125f, 0.421875f}},
    {_c(cubicOut), {0.578125f, 0.875f, 0.984375f}},
    {_c(cubicInOut), {0.0625f, 0.5f, 0.9375f}},
    {_c(quarticIn), {0.00390625f, 0.0625f, 0.316406f}},
    {_c(quarticOut), {0.683594f, 0.9375f, 0.996094f}},
    {_c(quarticInOut), {0.03125f, 0.5f, 0.96875f}},
    {_c(quinticIn), {0.000976562f, 0.03125f, 0.237305f}},
    {_c(quinticOut), {0.762695f, 0.96875f, 0.999023f}},
    {_c(quinticInOut), {0.015625f, 0.5f, 0.984375f}},
    {_c(sineIn), {0.0761205f, 0.292893f, 0.617317f}},
    {_c(sineOut), {0.382683f, 0.707107f, 0.92388f}},
    {_c(sineInOut), {0.146447f, 0.5f, 0.853553f}},
    {_c(circularIn), {0.0317541f, 0.133975f, 0.338562f}},
    {_c(circularOut), {0.661438f, 0.866025f, 0.968246f}},
    {_c(circularInOut), {0.0669873f, 0.5f, 0.933013f}},
    {_c(exponentialIn), {0.00552427f, 0.03125f, 0.176777f}},
    {_c(exponentialOut), {0.823223f, 0.96875f, 0.994476f}},
    {_c(exponentialInOut), {0.015625f, 0.5f, 0.984375f}},
    {_c(elasticIn), {-0.00510376f, -0.0220971f, 0.0676494f}},
    {_c(elasticOut), {0.932351f, 1.022097f, 1.005104f}},
    {_c(elasticInOut), {-0.0110485f, 0.5f, 1.01105f}},
    {_c(backIn), {-0.161152f, -0.375f, -0.108455f}},
    {_c(backOut), {1.108455f, 1.375f, 1.161152f}},
    {_c(backInOut), {-0.1875f, 0.5f, 1.1875f}},
    {_c(bounceIn), {0.0411367f, 0.28125f, 0.527344f}},
    {_c(bounceOut), {0.472656f, 0.71875f, 0.958863f}},
    {_c(bounceInOut), {0.140625f, 0.5f, 0.859375f}}
};
#undef _c

EasingTest::EasingTest() {
    addInstancedTests({&EasingTest::bounds},
        Containers::arraySize(BoundsData));

    addInstancedTests({&EasingTest::monotonicity},
        Containers::arraySize(MonotonicityData));

    addInstancedTests({&EasingTest::symmetry},
        Containers::arraySize(SymmetryData));

    addInstancedTests({&EasingTest::values},
        Containers::arraySize(ValueData));

    addInstancedBenchmarks({&EasingTest::benchmark}, 100,
        Containers::arraySize(ValueData));
}

enum: std::size_t { PropertyVerificationStepCount = 50 };

void EasingTest::bounds() {
    auto&& data = BoundsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Float scale = 1.0f/Float(PropertyVerificationStepCount - 1);
    for(std::size_t i = 0; i != PropertyVerificationStepCount; ++i) {
        Float t = i*scale;
        CORRADE_COMPARE_AS(data.function(t), 0.0f, TestSuite::Compare::GreaterOrEqual);
        CORRADE_COMPARE_AS(data.function(t), 1.0f, TestSuite::Compare::LessOrEqual);
    }
}

void EasingTest::monotonicity() {
    auto&& data = MonotonicityData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Float scale = 1.0f/Float(PropertyVerificationStepCount - 1);
    Float prev = data.function(0);
    for(std::size_t i = 1; i != PropertyVerificationStepCount; ++i) {
        Float cur = data.function(i*scale);
        CORRADE_COMPARE_AS(cur, prev, TestSuite::Compare::GreaterOrEqual);
        prev = cur;
    }
}

void EasingTest::symmetry() {
    auto&& data = SymmetryData[testCaseInstanceId()];
    if(std::strcmp(data.name, data.symmetricName) == 0)
        setTestCaseDescription(data.name);
    else
        setTestCaseDescription(Utility::formatString("{} : {}", data.name, data.symmetricName));

    /* Not testing the edges, as these are tested in values() anyway (and are
       problematic in functions that have explicit handling for them) */
    Float scale = 1.0f/Float(PropertyVerificationStepCount + 1);
    std::size_t max = PropertyVerificationStepCount/(data.function == data.symmetric ? 2 : 1);
    for(std::size_t i = 1; i != max; ++i) {
        Float t = i*scale;
        CORRADE_COMPARE(data.function(t), 1.0f - data.symmetric(1.0f - t));
    }
}

void EasingTest::values() {
    auto&& data = ValueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    CORRADE_COMPARE(data.function(0.0f), 0.0f);
    CORRADE_COMPARE(data.function(1.0f), 1.0f);
    CORRADE_COMPARE(data.function(0.25f), data.values[0]);
    CORRADE_COMPARE(data.function(0.50f), data.values[1]);
    CORRADE_COMPARE(data.function(0.75f), data.values[2]);
}

enum: Int { BenchmarkStepCount = 5000 };

void EasingTest::benchmark() {
    auto&& data = ValueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Skip edges because the cumulated number may exceed 1 (on asm.js
       Emscripten), producing NaN on some functions and failing the test */
    Float scale = 1.0f/Float(BenchmarkStepCount + 1);
    Float result = 0.0f;
    std::size_t i = 0;
    CORRADE_BENCHMARK(BenchmarkStepCount)
        result += data.function(++i*scale);

    /* backIn() has -340 */
    CORRADE_COMPARE_AS(result, -350.0f, TestSuite::Compare::Greater);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::EasingTest)
