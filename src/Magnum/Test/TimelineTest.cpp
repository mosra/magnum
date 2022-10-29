/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
    /* The timeline is stopped by default, so everything is zero */
    Timeline timeline;
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* And it continues to be 0 */
    Utility::System::sleep(50);
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* There's no previous frame right after the start */
    timeline.start();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* Still no previous frame */
    Utility::System::sleep(50);
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* Now it is */
    timeline.nextFrame();
    Float firstFrameTime = timeline.previousFrameTime();
    Float firstFrameDuration = timeline.previousFrameDuration();
    CORRADE_COMPARE_AS(firstFrameTime, 0.05f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(firstFrameTime, 0.05f + 0.05f,
        TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(firstFrameDuration, 0.05f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(firstFrameDuration, 0.05f + 0.05f,
        TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(),
        timeline.previousFrameTime(),
        TestSuite::Compare::LessOrEqual);

    /* And it doesn't change until another nextFrame() call */
    Utility::System::sleep(50);
    CORRADE_COMPARE(timeline.previousFrameTime(), firstFrameTime);
    CORRADE_COMPARE(timeline.previousFrameDuration(), firstFrameDuration);

    /* Third frame being measured now */
    timeline.nextFrame();
    Float secondFrameTime = timeline.previousFrameTime();
    Float secondFrameDuration = timeline.previousFrameDuration();
    CORRADE_COMPARE_AS(secondFrameTime, 0.10f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(secondFrameTime, 0.10f + 0.05f,
        TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(secondFrameDuration, 0.05f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(secondFrameDuration, 0.05f + 0.05f,
        TestSuite::Compare::Less);

    /* And it doesn't change now either */
    Utility::System::sleep(50);
    CORRADE_COMPARE(timeline.previousFrameTime(), secondFrameTime);
    CORRADE_COMPARE(timeline.previousFrameDuration(), secondFrameDuration);

    /* Calling start() resets the time to 0 */
    timeline.start();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* And it continues to be counted */
    Utility::System::sleep(50);
    timeline.nextFrame();
    CORRADE_COMPARE_AS(timeline.previousFrameTime(), 0.05f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.previousFrameTime(), 0.05f + 0.05f,
        TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(), 0.05f,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(), 0.05f + 0.05f,
        TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(timeline.previousFrameDuration(),
        timeline.previousFrameTime(),
        TestSuite::Compare::LessOrEqual);

    /* Calling stop() resets the time to 0 */
    timeline.stop();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);

    /* And it continues to be 0 */
    Utility::System::sleep(50);
    timeline.nextFrame();
    CORRADE_COMPARE(timeline.previousFrameTime(), 0.0f);
    CORRADE_COMPARE(timeline.previousFrameDuration(), 0.0f);
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::TimelineTest)
