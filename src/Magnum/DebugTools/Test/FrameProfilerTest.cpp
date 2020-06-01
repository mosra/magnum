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

#include <sstream>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/System.h>

#include "Magnum/DebugTools/FrameProfiler.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct FrameProfilerTest: TestSuite::Tester {
    explicit FrameProfilerTest();

    void defaultConstructed();
    void noMeasurements();

    void singleFrame();
    void multipleFrames();

    void enableDisable();
    void reSetup();

    void copy();
    void move();

    void frameCountZero();
    void delayZero();
    void delayTooLittleFrames();
    void startStopFrameUnexpected();
    void measurementOutOfBounds();
    void frameOutOfBounds();
    void dataNotAvailableYet();
    void meanNotAvailableYet();

    void statistics();

    #ifdef MAGNUM_TARGET_GL
    void gl();
    void glNotEnabled();
    #endif

    void debugUnits();
    #ifdef MAGNUM_TARGET_GL
    void debugGLValue();
    void debugGLValues();

    void configurationGLValue();
    void configurationGLValues();
    #endif
};

struct {
    const char* name;
    bool delayed;
} SingleFrameData[]{
    {"", false},
    {"delayed by 1", true}
};

struct {
    const char* name;
    bool delayed;
    UnsignedInt delay;
} MultipleFramesData[]{
    {"", false, 1},
    {"delayed by 1", true, 1},
    {"delayed by 2", true, 2},
    {"delayed by 3", true, 3}
};

#ifdef MAGNUM_TARGET_GL
struct {
    const char* name;
    GLFrameProfiler::Values values;
    UnsignedInt measurementCount;
    UnsignedInt measurementDelay;
} GLData[]{
    {"empty", {}, 0, 1},
    {"frame time", GLFrameProfiler::Value::FrameTime, 1, 2},
    {"cpu duration", GLFrameProfiler::Value::CpuDuration, 1, 1},
    {"frame time + cpu duration", GLFrameProfiler::Value::FrameTime|GLFrameProfiler::Value::CpuDuration, 2, 2}
};
#endif

FrameProfilerTest::FrameProfilerTest() {
    addTests({&FrameProfilerTest::defaultConstructed,
              &FrameProfilerTest::noMeasurements});

    addInstancedTests({&FrameProfilerTest::singleFrame},
        Containers::arraySize(SingleFrameData));
    addInstancedTests({&FrameProfilerTest::multipleFrames},
        Containers::arraySize(MultipleFramesData));

    addTests({&FrameProfilerTest::enableDisable,
              &FrameProfilerTest::reSetup,

              &FrameProfilerTest::copy,
              &FrameProfilerTest::move,

              &FrameProfilerTest::frameCountZero,
              &FrameProfilerTest::delayZero,
              &FrameProfilerTest::delayTooLittleFrames,
              &FrameProfilerTest::startStopFrameUnexpected,
              &FrameProfilerTest::measurementOutOfBounds,
              &FrameProfilerTest::frameOutOfBounds,
              &FrameProfilerTest::dataNotAvailableYet,
              &FrameProfilerTest::meanNotAvailableYet,

              &FrameProfilerTest::statistics});

    #ifdef MAGNUM_TARGET_GL
    addInstancedTests({&FrameProfilerTest::gl},
        Containers::arraySize(GLData));
    #endif

    addTests({
              #ifdef MAGNUM_TARGET_GL
              &FrameProfilerTest::glNotEnabled,
              #endif

              &FrameProfilerTest::debugUnits,
              #ifdef MAGNUM_TARGET_GL
              &FrameProfilerTest::debugGLValue,
              &FrameProfilerTest::debugGLValues,

              &FrameProfilerTest::configurationGLValue,
              &FrameProfilerTest::configurationGLValues
              #endif
              });
}

void FrameProfilerTest::defaultConstructed() {
    FrameProfiler profiler;
    CORRADE_COMPARE(profiler.maxFrameCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementCount(), 0);
    CORRADE_COMPARE(profiler.statistics(), "Last 0 frames:");

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 1);

    /* Shouldn't crash on any silly division by zero even when called a second
       time */
    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 2);
}

void FrameProfilerTest::noMeasurements() {
    FrameProfiler profiler{{}, 3};
    CORRADE_COMPARE(profiler.maxFrameCount(), 3);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementCount(), 0);
    CORRADE_COMPARE(profiler.statistics(), "Last 0 frames:");

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 1);

    /* Shouldn't crash on any silly division by zero even after a wraparound */
    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 2);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 4);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 5);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measuredFrameCount(), 6);
}

void FrameProfilerTest::singleFrame() {
    auto&& data = SingleFrameData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    UnsignedLong time = 0, memory = 50;
    FrameProfiler profiler;
    if(!data.delayed) {
        profiler.setup({
            FrameProfiler::Measurement{
                "Lag", FrameProfiler::Units::Nanoseconds,
                [](void* state) {
                    *static_cast<UnsignedLong*>(state) += 15;
                },
                [](void* state) {
                    return *static_cast<UnsignedLong*>(state) - 15;
                }, &time},
            FrameProfiler::Measurement{
                "Bloat", FrameProfiler::Units::Bytes,
                [](void* state) {
                    *static_cast<UnsignedLong*>(state) *= 2;
                },
                [](void* state) {
                    return *static_cast<UnsignedLong*>(state) - 100;
                }, &memory},
            FrameProfiler::Measurement{
                "Constant", FrameProfiler::Units::Count,
                [](void*) {},
                [](void*) {
                    return UnsignedLong{100000};
                }, nullptr}
        }, 1);
    } else {
        profiler.setup({
            FrameProfiler::Measurement{
                "Lag", FrameProfiler::Units::Nanoseconds, 1,
                [](void* state, UnsignedInt current) {
                    CORRADE_COMPARE(current, 0);
                    static_cast<UnsignedLong*>(state)[current] += 30;
                },
                [](void* state, UnsignedInt current) {
                    CORRADE_COMPARE(current, 0);
                    static_cast<UnsignedLong*>(state)[current] -= 15;
                },
                [](void* state, UnsignedInt previous, UnsignedInt current) {
                    CORRADE_COMPARE(previous, 0);
                    CORRADE_COMPARE(current, 0);
                    return static_cast<UnsignedLong*>(state)[previous] - 15;
                }, &time},
            FrameProfiler::Measurement{
                "Bloat", FrameProfiler::Units::Bytes, 1,
                [](void* state, UnsignedInt current) {
                    CORRADE_COMPARE(current, 0);
                    static_cast<UnsignedLong*>(state)[current] *= 4;
                },
                [](void* state, UnsignedInt current) {
                    CORRADE_COMPARE(current, 0);
                    static_cast<UnsignedLong*>(state)[current] /= 2;
                },
                [](void* state, UnsignedInt previous, UnsignedInt current) {
                    CORRADE_COMPARE(previous, 0);
                    CORRADE_COMPARE(current, 0);
                    return static_cast<UnsignedLong*>(state)[previous] - 100;
                }, &memory},
            FrameProfiler::Measurement{
                "Constant", FrameProfiler::Units::Count, 1,
                [](void*, UnsignedInt) {},
                [](void*, UnsignedInt) {},
                [](void*, UnsignedInt, UnsignedInt) {
                    return UnsignedLong{100000};
                }, nullptr}
        }, 1);
    }
    CORRADE_COMPARE(profiler.maxFrameCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementCount(), 3);

    CORRADE_COMPARE(profiler.measurementName(0), "Lag");
    CORRADE_COMPARE(profiler.measurementUnits(0), FrameProfiler::Units::Nanoseconds);
    CORRADE_COMPARE(profiler.measurementDelay(0), 1);

    CORRADE_COMPARE(profiler.measurementName(1), "Bloat");
    CORRADE_COMPARE(profiler.measurementUnits(1), FrameProfiler::Units::Bytes);
    CORRADE_COMPARE(profiler.measurementDelay(1), 1);

    CORRADE_COMPARE(profiler.measurementName(2), "Constant");
    CORRADE_COMPARE(profiler.measurementUnits(2), FrameProfiler::Units::Count);
    CORRADE_COMPARE(profiler.measurementDelay(2), 1);

    CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(1));
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(2));

    profiler.beginFrame();
    CORRADE_COMPARE(time, data.delayed ? 30 : 15);
    CORRADE_COMPARE(memory, data.delayed ? 200 : 100);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(1));
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(2));

    profiler.endFrame();
    CORRADE_COMPARE(time, 15);
    CORRADE_COMPARE(memory, 100);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 1);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    CORRADE_VERIFY(profiler.isMeasurementAvailable(1));
    CORRADE_VERIFY(profiler.isMeasurementAvailable(2));
    CORRADE_COMPARE(profiler.measurementData(0, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), 0.0);
    CORRADE_COMPARE(profiler.measurementMean(1), 0.0);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(time, 30);
    CORRADE_COMPARE(memory, 200);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 2);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 15);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 100);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), 15.0);
    CORRADE_COMPARE(profiler.measurementMean(1), 100.0);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(time, 45);
    CORRADE_COMPARE(memory, 400);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 30);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 300);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), 30.0);
    CORRADE_COMPARE(profiler.measurementMean(1), 300.0);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(time, 60);
    CORRADE_COMPARE(memory, 800);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 45);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 700);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), 45.0);
    CORRADE_COMPARE(profiler.measurementMean(1), 700.0);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);
}

void FrameProfilerTest::multipleFrames() {
    auto&& data = MultipleFramesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct State {
        UnsignedLong currentTime, currentMemory;
        UnsignedLong time[3];
        UnsignedLong memory[3];
        UnsignedInt delay;
    } state {0, 50, {0, 0, 0}, {50, 0, 0}, data.delay};
    FrameProfiler profiler;
    if(!data.delayed) {
        profiler.setup({
            FrameProfiler::Measurement{
                "Lag", FrameProfiler::Units::Nanoseconds,
                [](void* state) {
                    *static_cast<UnsignedLong*>(state) += 15;
                },
                [](void* state) {
                    return *static_cast<UnsignedLong*>(state) - 15;
                }, &state.time[0]},
            FrameProfiler::Measurement{
                "Bloat", FrameProfiler::Units::Bytes,
                [](void* state) {
                    *static_cast<UnsignedLong*>(state) *= 2;
                },
                [](void* state) {
                    return *static_cast<UnsignedLong*>(state) - 100;
                }, &state.memory[0]},
            FrameProfiler::Measurement{
                "Constant", FrameProfiler::Units::Count,
                [](void*) {},
                [](void*) {
                    return UnsignedLong{100000};
                }, nullptr}
        }, 3);
    } else {
        profiler.setup({
            FrameProfiler::Measurement{
                "Lag", FrameProfiler::Units::Nanoseconds, data.delay,
                [](void* state, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    s.time[current] = (s.currentTime += 15) + 15;
                },
                [](void* state, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    s.time[current] -= 15;
                },
                [](void* state, UnsignedInt previous, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(previous, s.delay, TestSuite::Compare::Less);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    CORRADE_VERIFY(current + 1 == previous || (current == s.delay - 1 && previous == 0));
                    return s.time[previous] - 15;
                }, &state},
            FrameProfiler::Measurement{
                "Bloat", FrameProfiler::Units::Bytes, data.delay,
                [](void* state, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    s.memory[current] = (s.currentMemory *= 2)*2;
                },
                [](void* state, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    s.memory[current] /= 2;
                },
                [](void* state, UnsignedInt previous, UnsignedInt current) {
                    auto& s = *static_cast<State*>(state);
                    CORRADE_COMPARE_AS(previous, s.delay, TestSuite::Compare::Less);
                    CORRADE_COMPARE_AS(current, s.delay, TestSuite::Compare::Less);
                    CORRADE_VERIFY(current + 1 == previous || (current == s.delay - 1 && previous == 0));
                    return s.memory[previous] - 100;
                }, &state},
            FrameProfiler::Measurement{
                "Undelayed constant", FrameProfiler::Units::Count, 1,
                [](void*, UnsignedInt) {},
                [](void*, UnsignedInt) {},
                [](void*, UnsignedInt, UnsignedInt) {
                    return UnsignedLong{100000};
                }, nullptr}
        }, 3);
    }
    CORRADE_COMPARE(profiler.maxFrameCount(), 3);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementDelay(0), data.delay);
    CORRADE_COMPARE(profiler.measurementDelay(1), data.delay);
    CORRADE_COMPARE(profiler.measurementDelay(2), 1);

    for(std::size_t i = 0; i != data.delay - 1; ++i) {
        profiler.beginFrame();
        profiler.endFrame();
        CORRADE_COMPARE(state.time[i], 15*(i + 1));
        CORRADE_COMPARE(state.memory[i], 100*(i + 1));
        CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));
        CORRADE_VERIFY(!profiler.isMeasurementAvailable(1));
        CORRADE_VERIFY(profiler.isMeasurementAvailable(2));
    }

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[0 % data.delay], 15);
    CORRADE_COMPARE(state.memory[0 % data.delay], 100);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    CORRADE_VERIFY(profiler.isMeasurementAvailable(1));
    CORRADE_VERIFY(profiler.isMeasurementAvailable(2));
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), 0.0);
    CORRADE_COMPARE(profiler.measurementMean(1), 0.0);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[1 % data.delay], 30);
    CORRADE_COMPARE(state.memory[1 % data.delay], 200);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 1 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(0, 1), 15);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(1, 1), 100);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 1), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), (15.0 + 0.0)/2);
    CORRADE_COMPARE(profiler.measurementMean(1), (100.0 + 0.0)/2);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[2 % data.delay], 45);
    CORRADE_COMPARE(state.memory[2 % data.delay], 400);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 2 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(0, 1), 15);
    CORRADE_COMPARE(profiler.measurementData(0, 2), 30);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 0);
    CORRADE_COMPARE(profiler.measurementData(1, 1), 100);
    CORRADE_COMPARE(profiler.measurementData(1, 2), 300);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 1), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 2), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), (30.0 + 15.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(1), (300.0 + 100.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    /* At this point it wraps around and should be evicting old values from the
       moving average */

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[3 % data.delay], 60);
    CORRADE_COMPARE(state.memory[3 % data.delay], 800);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 15);
    CORRADE_COMPARE(profiler.measurementData(0, 1), 30);
    CORRADE_COMPARE(profiler.measurementData(0, 2), 45);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 100);
    CORRADE_COMPARE(profiler.measurementData(1, 1), 300);
    CORRADE_COMPARE(profiler.measurementData(1, 2), 700);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 1), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 2), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), (45.0 + 30.0 + 15.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(1), (700.0 + 300.0 + 100.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[4 % data.delay], 75);
    CORRADE_COMPARE(state.memory[4 % data.delay], 1600);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 4 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 30);
    CORRADE_COMPARE(profiler.measurementData(0, 1), 45);
    CORRADE_COMPARE(profiler.measurementData(0, 2), 60);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 300);
    CORRADE_COMPARE(profiler.measurementData(1, 1), 700);
    CORRADE_COMPARE(profiler.measurementData(1, 2), 1500);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 1), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 2), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), (60 + 45.0 + 30.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(1), (1500.0 + 700.0 + 300.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(state.time[5 % data.delay], 90);
    CORRADE_COMPARE(state.memory[5 % data.delay], 3200);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 5 + data.delay);
    CORRADE_COMPARE(profiler.measurementData(0, 0), 45);
    CORRADE_COMPARE(profiler.measurementData(0, 1), 60);
    CORRADE_COMPARE(profiler.measurementData(0, 2), 75);
    CORRADE_COMPARE(profiler.measurementData(1, 0), 700);
    CORRADE_COMPARE(profiler.measurementData(1, 1), 1500);
    CORRADE_COMPARE(profiler.measurementData(1, 2), 3100);
    CORRADE_COMPARE(profiler.measurementData(2, 0), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 1), 100000);
    CORRADE_COMPARE(profiler.measurementData(2, 2), 100000);
    CORRADE_COMPARE(profiler.measurementMean(0), (75.0 + 60.0 + 45.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(1), (3100.0 + 1500.0 + 700.0)/3);
    CORRADE_COMPARE(profiler.measurementMean(2), 100000.0);
}

void FrameProfilerTest::enableDisable() {
    UnsignedLong i = 15;
    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 2,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void* state, UnsignedInt, UnsignedInt) {
                return (*static_cast<UnsignedLong*>(state))++;
            }, &i},
    }, 5};

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measurementCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3);
    CORRADE_COMPARE(profiler.measurementDelay(0), 2);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    CORRADE_COMPARE(profiler.measurementMean(0), 15.5);

    /* It should only freeze everything, not wipe out any data */
    profiler.disable();
    CORRADE_COMPARE(profiler.measurementCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3);
    CORRADE_COMPARE(profiler.measurementDelay(0), 2);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    CORRADE_COMPARE(profiler.measurementMean(0), 15.5);

    /* These are a no-op now */
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    CORRADE_COMPARE(profiler.measurementCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3);
    CORRADE_COMPARE(profiler.measurementDelay(0), 2);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    CORRADE_COMPARE(profiler.measurementMean(0), 15.5);

    /* Enabling should reset the data to have a clean slate, but not the
       measurements */
    profiler.enable();
    CORRADE_COMPARE(profiler.measurementCount(), 1);
    CORRADE_COMPARE(profiler.maxFrameCount(), 5);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementDelay(0), 2);
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));

    /* Even though there was no call to endFrame() before, reset() should make
       beginFrame() expected again */
    i = 0;
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measurementCount(), 1);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 3);
    CORRADE_COMPARE(profiler.measurementDelay(0), 2);
    CORRADE_VERIFY(profiler.isMeasurementAvailable(0));
    /* The per-measurement moving sum should be reset by enable() as well, so
       the 15s from before won't contribute to the mean anymore */
    CORRADE_COMPARE(profiler.measurementMean(0), 0.5);
}

void FrameProfilerTest::reSetup() {
    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 3,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) { return UnsignedLong{}; }, nullptr},
    }, 5};

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();

    /* Setup should replace everything */
    profiler.setup({
        FrameProfiler::Measurement{
            "Lag", FrameProfiler::Units::Nanoseconds,
            [](void*) {},
            [](void*) { return UnsignedLong{}; },
            nullptr},
        FrameProfiler::Measurement{
            "Bloat", FrameProfiler::Units::Bytes,
            [](void*) {},
            [](void*) { return UnsignedLong{}; },
            nullptr},
    }, 10);
    CORRADE_COMPARE(profiler.measurementCount(), 2);
    CORRADE_COMPARE(profiler.maxFrameCount(), 10);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 0);
    CORRADE_COMPARE(profiler.measurementDelay(0), 1);
    CORRADE_COMPARE(profiler.measurementDelay(1), 1);
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(1));

    /* Even though there was no call to endFrame() before, setup() should make
       beginFrame() expected again */
    profiler.beginFrame();
    profiler.endFrame();
}

void FrameProfilerTest::copy() {
    CORRADE_VERIFY(!(std::is_constructible<FrameProfiler, const FrameProfiler&>{}));
    CORRADE_VERIFY(!(std::is_assignable<FrameProfiler, const FrameProfiler&>{}));
}

void FrameProfilerTest::move() {
    /* Have two state variables, one in a subclass, one outside. On move the
       pointer to a subclass should get patched but the outside not */

    UnsignedLong i = 15;
    struct MyProfiler: FrameProfiler {
        UnsignedLong j = 30;
    } a;
    a.setup({
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count,
            [](void*) {},
            [](void* state) {
                return (*static_cast<UnsignedLong*>(state))++;
            }, &i},
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 2,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void* state, UnsignedInt, UnsignedInt) {
                return static_cast<MyProfiler*>(state)->j++;
            }, &a},
    }, 5);

    /* Move construction */
    MyProfiler b{std::move(a)};
    a.j = 100; /* This shouldn't affect b's measurements anymore */
    b.beginFrame();
    b.endFrame();
    b.beginFrame();
    b.endFrame();
    b.beginFrame();
    b.endFrame();
    CORRADE_COMPARE(b.measurementCount(), 2);
    CORRADE_COMPARE(b.measuredFrameCount(), 3);
    CORRADE_COMPARE(b.measurementDelay(0), 1);
    CORRADE_COMPARE(b.measurementDelay(1), 2);
    CORRADE_COMPARE(b.measurementMean(0), 16.0);
    CORRADE_COMPARE(b.measurementMean(1), 30.5);

    /* Another fully populated instance */
    UnsignedLong k = 45;
    MyProfiler c;
    c.j = 60;
    c.setup({
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count,
            [](void*) {},
            [](void* state) {
                return (*static_cast<UnsignedLong*>(state))++;
            }, &k},
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 3,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void* state, UnsignedInt, UnsignedInt) {
                return static_cast<MyProfiler*>(state)->j++;
            }, &c},
    }, 5);
    c.beginFrame();
    c.endFrame();
    c.beginFrame();
    c.endFrame();
    c.beginFrame();
    c.endFrame();
    c.beginFrame();
    c.endFrame();
    CORRADE_COMPARE(c.measurementCount(), 2);
    CORRADE_COMPARE(c.measuredFrameCount(), 4);
    CORRADE_COMPARE(c.measurementDelay(0), 1);
    CORRADE_COMPARE(c.measurementDelay(1), 3);
    CORRADE_COMPARE(c.measurementMean(0), 46.5);
    CORRADE_COMPARE(c.measurementMean(1), 60.5);

    /* Move assignment */
    CORRADE_COMPARE(c.j, 62);
    c = std::move(b);
    b.j = 62; /* std::move() didn't swap this one, so we do; this shouldn't
                 affect c's measurements anymore */
    c.beginFrame();
    c.endFrame();
    c.beginFrame();
    c.endFrame();
    CORRADE_COMPARE(c.measurementCount(), 2);
    CORRADE_COMPARE(c.measuredFrameCount(), 5);
    CORRADE_COMPARE(c.measurementDelay(0), 1);
    CORRADE_COMPARE(c.measurementDelay(1), 2);
    CORRADE_COMPARE(c.measurementMean(0), 17.0);
    CORRADE_COMPARE(c.measurementMean(1), 31.5);

    /* Calling these on the swapped instance should affect only itself */
    b.beginFrame();
    b.endFrame();
    CORRADE_COMPARE(b.measurementMean(0), 47.0); /* originally c */
    CORRADE_COMPARE(b.measurementMean(1), 61.0); /* originally c */
    CORRADE_COMPARE(c.measurementCount(), 2);
    CORRADE_COMPARE(c.measuredFrameCount(), 5);
    CORRADE_COMPARE(c.measurementDelay(0), 1);
    CORRADE_COMPARE(c.measurementDelay(1), 2);
    CORRADE_COMPARE(c.measurementMean(0), 17.0);
    CORRADE_COMPARE(c.measurementMean(1), 31.5);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<FrameProfiler>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<FrameProfiler>::value);
}

void FrameProfilerTest::delayZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 0,
        nullptr, nullptr, nullptr, nullptr};
    CORRADE_COMPARE(out.str(), "DebugTools::FrameProfiler::Measurement: delay can't be zero\n");
}

void FrameProfilerTest::frameCountZero() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FrameProfiler{{}, 0};
    CORRADE_COMPARE(out.str(), "DebugTools::FrameProfiler::setup(): max frame count can't be zero\n");
}

void FrameProfilerTest::delayTooLittleFrames() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 3,
            nullptr, nullptr, nullptr, nullptr}
    }, 2};
    CORRADE_COMPARE(out.str(), "DebugTools::FrameProfiler::setup(): max delay 3 is larger than max frame count 2\n");
}

void FrameProfilerTest::startStopFrameUnexpected() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FrameProfiler profiler;

    std::ostringstream out;
    {
        Error redirectError{&out};
        profiler.endFrame();
    }
    profiler.beginFrame(); /* this is not an error */
    {
        Error redirectError{&out};
        profiler.beginFrame();
    }
    CORRADE_COMPARE(out.str(),
        "DebugTools::FrameProfiler::endFrame(): expected begin of frame\n"
        "DebugTools::FrameProfiler::beginFrame(): expected end of frame\n");
}

void FrameProfilerTest::measurementOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count,
            nullptr, nullptr, nullptr},
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count,
            nullptr, nullptr, nullptr}
    }, 1};

    std::ostringstream out;
    Error redirectError{&out};
    profiler.measurementName(2);
    profiler.measurementUnits(2);
    profiler.measurementDelay(2);
    profiler.measurementData(2, 0);
    profiler.measurementMean(2);
    CORRADE_COMPARE(out.str(),
        "DebugTools::FrameProfiler::measurementName(): index 2 out of range for 2 measurements\n"
        "DebugTools::FrameProfiler::measurementUnits(): index 2 out of range for 2 measurements\n"
        "DebugTools::FrameProfiler::measurementDelay(): index 2 out of range for 2 measurements\n"
        "DebugTools::FrameProfiler::measurementData(): index 2 out of range for 2 measurements\n"
        "DebugTools::FrameProfiler::measurementMean(): index 2 out of range for 2 measurements\n");
}

void FrameProfilerTest::frameOutOfBounds() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count,
            [](void*) {},
            [](void*) { return UnsignedLong{}; }, nullptr},
    }, 3};

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();

    std::ostringstream out;
    Error redirectError{&out};
    profiler.measurementData(0, 3);
    CORRADE_COMPARE(out.str(),
        "DebugTools::FrameProfiler::measurementData(): frame 3 out of bounds for max 3 frames\n");
}

void FrameProfilerTest::dataNotAvailableYet() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 3,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) { return UnsignedLong{}; }, nullptr},
    }, 5};

    /* Empty state */
    {
        std::ostringstream out;
        Error redirectError{&out};
        profiler.measurementData(0, 0);
        CORRADE_COMPARE(out.str(),
            "DebugTools::FrameProfiler::measurementData(): frame 0 of measurement 0 not available yet (delay 3, 0 frames measured so far)\n");
    }

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();

    /* No wraparound yet */
    {
        profiler.measurementData(0, 0);
        profiler.measurementData(0, 1);

        std::ostringstream out;
        Error redirectError{&out};
        profiler.measurementData(0, 2);
        profiler.measurementData(0, 3);
        profiler.measurementData(0, 4);
        CORRADE_COMPARE(out.str(),
            "DebugTools::FrameProfiler::measurementData(): frame 2 of measurement 0 not available yet (delay 3, 4 frames measured so far)\n"
            "DebugTools::FrameProfiler::measurementData(): frame 3 of measurement 0 not available yet (delay 3, 4 frames measured so far)\n"
            "DebugTools::FrameProfiler::measurementData(): frame 4 of measurement 0 not available yet (delay 3, 4 frames measured so far)\n");
    }

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();

    /* Wraparound, one last measurement missing */
    {
        profiler.measurementData(0, 0);
        profiler.measurementData(0, 1);
        profiler.measurementData(0, 2);
        profiler.measurementData(0, 3);

        std::ostringstream out;
        Error redirectError{&out};
        profiler.measurementData(0, 4);
        CORRADE_COMPARE(out.str(),
            "DebugTools::FrameProfiler::measurementData(): frame 4 of measurement 0 not available yet (delay 3, 6 frames measured so far)\n");
    }
}

void FrameProfilerTest::meanNotAvailableYet() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    FrameProfiler profiler{{
        FrameProfiler::Measurement{"", FrameProfiler::Units::Count, 3,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) { return UnsignedLong{}; }, nullptr},
    }, 5};

    profiler.beginFrame();
    profiler.endFrame();
    CORRADE_COMPARE(profiler.measurementDelay(0), 3);
    CORRADE_COMPARE(profiler.measuredFrameCount(), 1);
    CORRADE_VERIFY(!profiler.isMeasurementAvailable(0));

    std::ostringstream out;
    Error redirectError{&out};
    profiler.measurementMean(0);
    CORRADE_COMPARE(out.str(),
        "DebugTools::FrameProfiler::measurementMean(): measurement data available after 2 more frames\n");
}

void FrameProfilerTest::statistics() {
    UnsignedLong time = 0;
    FrameProfiler profiler{{
        FrameProfiler::Measurement{
            "Lag", FrameProfiler::Units::Nanoseconds, 2,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void* state, UnsignedInt, UnsignedInt) {
                return *static_cast<UnsignedLong*>(state) += 15;
            }, &time},
        FrameProfiler::Measurement{
            "Bloat", FrameProfiler::Units::Bytes,
            [](void*) {},
            [](void*) {
                return UnsignedLong{1007300*1024*1024ull};
            }, nullptr},
        FrameProfiler::Measurement{
            "Age", FrameProfiler::Units::Nanoseconds,
            [](void*) {},
            [](void*) {
                return UnsignedLong{273*1000*1000};
            }, nullptr},
        FrameProfiler::Measurement{
            "GC", FrameProfiler::Units::Nanoseconds, 3,
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{52660};
            }, nullptr},
        FrameProfiler::Measurement{
            "Optimizations", FrameProfiler::Units::Count,
            [](void*) {},
            [](void*) {
                return UnsignedLong{0};
            }, nullptr},
        FrameProfiler::Measurement{
            "Frame time", FrameProfiler::Units::Nanoseconds,
            [](void*) {},
            [](void*) {
                return UnsignedLong{1000*1000*1000ull};
            }, nullptr},
        FrameProfiler::Measurement{
            "Sanity ratio", FrameProfiler::Units::RatioThousandths,
            [](void*) {},
            [](void*) {
                return UnsignedLong{855};
            }, nullptr},
        FrameProfiler::Measurement{
            "CPU usage", FrameProfiler::Units::PercentageThousandths,
            [](void*) {},
            [](void*) {
                return UnsignedLong{98655};
            }, nullptr}
    }, 3};

    CORRADE_COMPARE(profiler.statistics(),
        "Last 0 frames:\n"
        "  Lag: -.-- s\n"
        "  Bloat: -.-- B\n"
        "  Age: -.-- s\n"
        "  GC: -.-- s\n"
        "  Optimizations: -.--\n"
        "  Frame time: -.-- s\n"
        "  Sanity ratio: -.--\n"
        "  CPU usage: -.-- %");

    profiler.beginFrame();
    profiler.endFrame();

    CORRADE_COMPARE(profiler.statistics(),
        "Last 1 frames:\n"
        "  Lag: -.-- s\n"
        "  Bloat: 983.69 GB\n"
        "  Age: 273.00 ms\n"
        "  GC: -.-- s\n"
        "  Optimizations: 0.00\n"
        "  Frame time: 1.00 s\n"
        "  Sanity ratio: 0.85\n"
        "  CPU usage: 98.66 %");

    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();
    profiler.beginFrame();
    profiler.endFrame();

    CORRADE_COMPARE(profiler.statistics(),
        "Last 3 frames:\n"
        "  Lag: 60.00 ns\n"
        "  Bloat: 983.69 GB\n"
        "  Age: 273.00 ms\n"
        "  GC: 52.66 µs\n"
        "  Optimizations: 0.00\n"
        "  Frame time: 1.00 s\n"
        "  Sanity ratio: 0.85\n"
        "  CPU usage: 98.66 %");

    /* Disabling should print the last known state */
    profiler.disable();
    CORRADE_COMPARE(profiler.statistics(),
        "Last 3 frames:\n"
        "  Lag: 60.00 ns\n"
        "  Bloat: 983.69 GB\n"
        "  Age: 273.00 ms\n"
        "  GC: 52.66 µs\n"
        "  Optimizations: 0.00\n"
        "  Frame time: 1.00 s\n"
        "  Sanity ratio: 0.85\n"
        "  CPU usage: 98.66 %");

    /* Enabling again should go back to initial state */
    profiler.enable();
    CORRADE_COMPARE(profiler.statistics(),
        "Last 0 frames:\n"
        "  Lag: -.-- s\n"
        "  Bloat: -.-- B\n"
        "  Age: -.-- s\n"
        "  GC: -.-- s\n"
        "  Optimizations: -.--\n"
        "  Frame time: -.-- s\n"
        "  Sanity ratio: -.--\n"
        "  CPU usage: -.-- %");
}

#ifdef MAGNUM_TARGET_GL
void FrameProfilerTest::gl() {
    auto&& data = GLData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Test that we use the right state pointers to survive a move */
    Containers::Pointer<GLFrameProfiler> profiler_{Containers::InPlaceInit,
        data.values, 4u};
    GLFrameProfiler profiler = std::move(*profiler_);
    profiler_ = nullptr;
    CORRADE_COMPARE(profiler.values(), data.values);
    CORRADE_COMPARE(profiler.maxFrameCount(), 4);
    CORRADE_COMPARE(profiler.measurementCount(), data.measurementCount);

    /* MSVC 2015 needs the {} */
    for(auto value: {GLFrameProfiler::Value::CpuDuration,
                     GLFrameProfiler::Value::FrameTime}) {
        if(data.values & value)
            CORRADE_VERIFY(!profiler.isMeasurementAvailable(value));
    }

    profiler.beginFrame();
    Utility::System::sleep(1);
    profiler.endFrame();

    profiler.beginFrame();
    profiler.endFrame();

    Utility::System::sleep(10);

    profiler.beginFrame();
    Utility::System::sleep(1);
    profiler.endFrame();

    profiler.beginFrame();
    Utility::System::sleep(1);
    profiler.endFrame();

    for(std::size_t i = 0; i != data.measurementCount; ++i)
        CORRADE_VERIFY(profiler.isMeasurementAvailable(i));

    /* 3/4 frames took 1 ms, the ideal average is 0.75 ms. Can't test upper
       bound because (especially on overloaded CIs) it all takes a magnitude
       more than expected. Emscripten builds have it as low as 0.5, account for
       that. */
    if(data.values & GLFrameProfiler::Value::CpuDuration) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(GLFrameProfiler::Value::CpuDuration));
        CORRADE_COMPARE_AS(profiler.cpuDurationMean(), 0.50*1000*1000,
            TestSuite::Compare::GreaterOrEqual);
    }

    /* 3/4 frames took 1 ms, and one 10 ms, the ideal average is 3.25 ms. Can't
       test upper bound because (especially on overloaded CIs) it all takes a
       magnitude more than expected. */
    if(data.values & GLFrameProfiler::Value::FrameTime) {
        CORRADE_VERIFY(profiler.isMeasurementAvailable(GLFrameProfiler::Value::FrameTime));
        CORRADE_COMPARE_AS(profiler.frameTimeMean(), 3.20*1000*1000,
            TestSuite::Compare::GreaterOrEqual);
    }

    /* GPU time tested separately */
}

void FrameProfilerTest::glNotEnabled() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    GLFrameProfiler profiler{{}, 5};

    std::ostringstream out;
    Error redirectError{&out};
    profiler.isMeasurementAvailable(GLFrameProfiler::Value::CpuDuration);
    profiler.frameTimeMean();
    profiler.cpuDurationMean();
    profiler.gpuDurationMean();
    CORRADE_COMPARE(out.str(),
        "DebugTools::GLFrameProfiler::isMeasurementAvailable(): DebugTools::GLFrameProfiler::Value::CpuDuration not enabled\n"
        "DebugTools::GLFrameProfiler::frameTimeMean(): not enabled\n"
        "DebugTools::GLFrameProfiler::cpuDurationMean(): not enabled\n"
        "DebugTools::GLFrameProfiler::gpuDurationMean(): not enabled\n");
}
#endif

void FrameProfilerTest::debugUnits() {
    std::ostringstream out;

    Debug{&out} << FrameProfiler::Units::Nanoseconds << FrameProfiler::Units(0xf0);
    CORRADE_COMPARE(out.str(), "DebugTools::FrameProfiler::Units::Nanoseconds DebugTools::FrameProfiler::Units(0xf0)\n");
}

#ifdef MAGNUM_TARGET_GL
void FrameProfilerTest::debugGLValue() {
    std::ostringstream out;

    Debug{&out} << GLFrameProfiler::Value::GpuDuration << GLFrameProfiler::Value(0xfff0);
    CORRADE_COMPARE(out.str(), "DebugTools::GLFrameProfiler::Value::GpuDuration DebugTools::GLFrameProfiler::Value(0xfff0)\n");
}

void FrameProfilerTest::debugGLValues() {
    std::ostringstream out;

    Debug{&out} << (GLFrameProfiler::Value::CpuDuration|GLFrameProfiler::Value::FrameTime) << GLFrameProfiler::Values{};
    CORRADE_COMPARE(out.str(), "DebugTools::GLFrameProfiler::Value::FrameTime|DebugTools::GLFrameProfiler::Value::CpuDuration DebugTools::GLFrameProfiler::Values{}\n");
}

void FrameProfilerTest::configurationGLValue() {
    Utility::ConfigurationGroup c;

    c.setValue("value", GLFrameProfiler::Value::GpuDuration);
    CORRADE_COMPARE(c.value("value"), "GpuDuration");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Value>("value"), GLFrameProfiler::Value::GpuDuration);

    c.setValue("zero", GLFrameProfiler::Value{});
    CORRADE_COMPARE(c.value("zero"), "");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Value>("zero"), GLFrameProfiler::Value{});

    c.setValue("invalid", GLFrameProfiler::Value(0xdead));
    CORRADE_COMPARE(c.value("invalid"), "");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Value>("invalid"), GLFrameProfiler::Value{});
}

void FrameProfilerTest::configurationGLValues() {
    Utility::ConfigurationGroup c;

    c.setValue("value", GLFrameProfiler::Value::FrameTime|GLFrameProfiler::Value::CpuDuration|GLFrameProfiler::Value::GpuDuration);
    CORRADE_COMPARE(c.value("value"), "FrameTime CpuDuration GpuDuration");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Values>("value"), GLFrameProfiler::Value::FrameTime|GLFrameProfiler::Value::CpuDuration|GLFrameProfiler::Value::GpuDuration);

    c.setValue("empty", GLFrameProfiler::Values{});
    CORRADE_COMPARE(c.value("empty"), "");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Values>("empty"), GLFrameProfiler::Values{});

    c.setValue("invalid", GLFrameProfiler::Value::CpuDuration|GLFrameProfiler::Value::GpuDuration|GLFrameProfiler::Value(0xff00));
    CORRADE_COMPARE(c.value("invalid"), "CpuDuration GpuDuration");
    CORRADE_COMPARE(c.value<GLFrameProfiler::Values>("invalid"), GLFrameProfiler::Value::CpuDuration|GLFrameProfiler::Value::GpuDuration);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::FrameProfilerTest)
