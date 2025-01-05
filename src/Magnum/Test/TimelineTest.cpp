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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Magnum.h"
#include "Magnum/Timeline.h"

namespace Magnum { namespace Test { namespace {

struct TimelineTest: TestSuite::Tester {
    explicit TimelineTest();

    void test();
};

TimelineTest::TimelineTest() {
    addTests({&TimelineTest::test});
}

void TimelineTest::test() {
    /* Can't reliably test that the measured time is less than something unless
       we'd have the sleeps prohibitively long. So just verifying the measured
       time isn't less than expected, and that it increases over time.

       On Emscripten the returned time is however sometimes *less* than the
       sleep, so subtracting an epsilon to account for that. */
    constexpr std::size_t ms = 50;
    constexpr Float s = 0.001f*ms;
    constexpr Float epsilon = 0.01f;

    /* The timeline is stopped by default, so everything is zero */
    Timeline timeline;
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameDuration(), 0.0f);

    /* And it continues to be 0 */
    Utility::System::sleep(ms);
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameDuration(), 0.0f);

    /* There's no previous frame right after the start */
    timeline.start();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* Still no previous frame, but current frame starts growing */
    Utility::System::sleep(ms);
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.currentFrameDuration(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_WITH(timeline.currentFrameTime(),
        timeline.currentFrameDuration(),
        TestSuite::Compare::around(0.01f));

    /* Now the previous frame is there */
    timeline.nextFrame();
    Float firstFrameTime = timeline.previousFrameTime();
    Float firstFrameDuration = timeline.previousFrameDuration();
    CORRADE_COMPARE_AS(firstFrameTime, s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(firstFrameDuration, s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(),
        timeline.previousFrameTime(),
        TestSuite::Compare::LessOrEqual);

    /* And it doesn't change until another nextFrame() call. Current frame
       grows again. */
    Utility::System::sleep(ms);
    CORRADE_COMPARE(timeline.previousFrameTime(), firstFrameTime);
    CORRADE_COMPARE(timeline.previousFrameDuration(), firstFrameDuration);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), firstFrameTime,
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), 2*s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.currentFrameDuration(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);

    /* Third frame being measured now */
    timeline.nextFrame();
    Float secondFrameTime = timeline.previousFrameTime();
    Float secondFrameDuration = timeline.previousFrameDuration();
    CORRADE_COMPARE_AS(secondFrameTime, firstFrameTime,
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(secondFrameTime, 2*s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(secondFrameDuration, s - epsilon,
        TestSuite::Compare::GreaterOrEqual);

    /* Previous frame doesn't change now either, current grows */
    Utility::System::sleep(ms);
    CORRADE_COMPARE(timeline.previousFrameTime(), secondFrameTime);
    CORRADE_COMPARE(timeline.previousFrameDuration(), secondFrameDuration);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), secondFrameTime,
        TestSuite::Compare::Greater);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), 3*s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.currentFrameDuration(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);

    /* Calling start() resets the time to 0 */
    timeline.start();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* And it continues to be counted */
    Utility::System::sleep(ms);
    CORRADE_COMPARE_AS(timeline.currentFrameTime(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.currentFrameDuration(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);

    timeline.nextFrame();
    CORRADE_COMPARE_AS(timeline.previousFrameTime(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(), s - epsilon,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(),
        timeline.previousFrameTime(),
        TestSuite::Compare::LessOrEqual);

    /* Calling stop() resets the time to 0 */
    timeline.stop();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameDuration(), 0.0f);

    /* And it continues to be 0 */
    Utility::System::sleep(ms);
    timeline.nextFrame();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.currentFrameDuration(), 0.0f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::TimelineTest)
