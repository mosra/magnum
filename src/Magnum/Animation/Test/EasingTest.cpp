/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Animation/Easing.h"
#include "Magnum/Math/TypeTraits.h"

namespace Magnum { namespace Animation { namespace Test { namespace {

struct EasingTest: TestSuite::Tester {
    explicit EasingTest();

    template<class T> void bounds();
    template<class T> void monotonicity();
    template<class T> void symmetry();
    template<class T> void values();

    template<class T> void benchmark();
};

#define _c(name) #name, Easing::name, Easingd::name
constexpr struct Bounds {
    const char* name;
    Float(*function)(Float);
    Double(*functiond)(Double);
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

constexpr struct Monotonicity {
    const char* name;
    Float(*function)(Float);
    Double(*functiond)(Double);
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

constexpr struct Symmetry {
    const char* name;
    Float(*function)(Float);
    Double(*functiond)(Double);
    const char* symmetricName;
    Float(*symmetric)(Float);
    Double(*symmetricd)(Double);
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

constexpr struct Value {
    const char* name;
    Float(*function)(Float);
    Double(*functiond)(Double);
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

template<class T> struct FunctionFor;
template<> struct FunctionFor<Float>  {
    static auto function(const Bounds& s) -> Float(*)(Float) {
        return s.function;
    }
    static auto function(const Monotonicity& s) -> Float(*)(Float) {
        return s.function;
    }
    static auto function(const Symmetry& s) -> Float(*)(Float) {
        return s.function;
    }
    static auto symmetric(const Symmetry& s) -> Float(*)(Float) {
        return s.symmetric;
    }
    static auto function(const Value& s) -> Float(*)(Float) {
        return s.function;
    }
};
template<> struct FunctionFor<Double>  {
    static auto function(const Bounds& s) -> Double(*)(Double) {
        return s.functiond;
    }
    static auto function(const Monotonicity& s) -> Double(*)(Double) {
        return s.functiond;
    }
    static auto function(const Symmetry& s) -> Double(*)(Double) {
        return s.functiond;
    }
    static auto symmetric(const Symmetry& s) -> Double(*)(Double) {
        return s.symmetricd;
    }
    static auto function(const Value& s) -> Double(*)(Double) {
        return s.functiond;
    }
};

EasingTest::EasingTest() {
    addInstancedTests<EasingTest>({
        &EasingTest::bounds<Float>,
        &EasingTest::bounds<Double>},
        Containers::arraySize(BoundsData));

    addInstancedTests<EasingTest>({
        &EasingTest::monotonicity<Float>,
        &EasingTest::monotonicity<Double>},
        Containers::arraySize(MonotonicityData));

    addInstancedTests<EasingTest>({
        &EasingTest::symmetry<Float>,
        &EasingTest::symmetry<Double>},
        Containers::arraySize(SymmetryData));

    addInstancedTests<EasingTest>({
        &EasingTest::values<Float>,
        &EasingTest::values<Double>},
        Containers::arraySize(ValueData));

    addInstancedBenchmarks<EasingTest>({
        &EasingTest::benchmark<Float>,
        &EasingTest::benchmark<Double>}, 100,
        Containers::arraySize(ValueData));
}

enum: std::size_t { PropertyVerificationStepCount = 50 };

template<class> struct BoundsLimits;
template<> struct BoundsLimits<Float> {
    static Float min() { return 0.0f; }
    static Float max() { return 1.0f; }
};
template<> struct BoundsLimits<Double> {
    /* Doubles very slightly overflow the bounds in release builds for some
       of the functions, have some epsilon there */
    static Double min() { return 0.0 - Math::TypeTraits<Double>::epsilon(); }
    static Double max() { return 1.0 + Math::TypeTraits<Double>::epsilon(); }
};

template<class T> void EasingTest::bounds() {
    auto&& data = BoundsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T scale = T(1.0)/T(PropertyVerificationStepCount - 1);
    for(std::size_t i = 0; i != PropertyVerificationStepCount; ++i) {
        T t = i*scale;
        CORRADE_COMPARE_AS(FunctionFor<T>::function(data)(t), BoundsLimits<T>::min(), TestSuite::Compare::GreaterOrEqual);
        CORRADE_COMPARE_AS(FunctionFor<T>::function(data)(t), BoundsLimits<T>::max(), TestSuite::Compare::LessOrEqual);
    }
}

template<class T> void EasingTest::monotonicity() {
    auto&& data = MonotonicityData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T scale = T(1.0)/T(PropertyVerificationStepCount - 1);
    T prev = FunctionFor<T>::function(data)(0);
    for(std::size_t i = 1; i != PropertyVerificationStepCount; ++i) {
        T cur = FunctionFor<T>::function(data)(i*scale);
        CORRADE_COMPARE_AS(cur, prev, TestSuite::Compare::GreaterOrEqual);
        prev = cur;
    }
}

template<class T> void EasingTest::symmetry() {
    auto&& data = SymmetryData[testCaseInstanceId()];
    if(data.name == Containers::StringView{data.symmetricName})
        setTestCaseDescription(data.name);
    else
        setTestCaseDescription(Utility::format("{} : {}", data.name, data.symmetricName));
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Not testing the edges, as these are tested in values() anyway (and are
       problematic in functions that have explicit handling for them) */
    T scale = T(1.0)/T(PropertyVerificationStepCount + 1);
    std::size_t max = PropertyVerificationStepCount/(FunctionFor<T>::function(data) == FunctionFor<T>::symmetric(data) ? 2 : 1);
    for(std::size_t i = 1; i != max; ++i) {
        T t = i*scale;
        CORRADE_COMPARE(FunctionFor<T>::function(data)(t), T(1.0) - FunctionFor<T>::symmetric(data)(T(1.0) - t));
    }
}

template<class T> void EasingTest::values() {
    auto&& data = ValueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    CORRADE_COMPARE(FunctionFor<T>::function(data)(T(0.0)), T(0.0));
    CORRADE_COMPARE(FunctionFor<T>::function(data)(T(1.0)), T(1.0));
    CORRADE_COMPARE(FunctionFor<T>::function(data)(T(0.25)), data.values[0]);
    CORRADE_COMPARE(FunctionFor<T>::function(data)(T(0.50)), data.values[1]);
    CORRADE_COMPARE(FunctionFor<T>::function(data)(T(0.75)), data.values[2]);
}

enum: Int { BenchmarkStepCount = 5000 };

template<class T> void EasingTest::benchmark() {
    auto&& data = ValueData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Skip edges because the cumulated number may exceed 1 (on asm.js
       Emscripten), producing NaN on some functions and failing the test */
    T scale = T(1.0)/T(BenchmarkStepCount + 1);
    T result = T(0.0);
    std::size_t i = 0;
    CORRADE_BENCHMARK(BenchmarkStepCount)
        result += FunctionFor<T>::function(data)(++i*scale);

    /* backIn() has -340 */
    CORRADE_COMPARE_AS(result, -350.0f, TestSuite::Compare::Greater);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::EasingTest)
