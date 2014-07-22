/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Query.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class TimeQueryGLTest: public AbstractOpenGLTester {
    public:
        explicit TimeQueryGLTest();

        void queryTime();
        void queryTimestamp();
};

TimeQueryGLTest::TimeQueryGLTest() {
    addTests({&TimeQueryGLTest::queryTime,
              &TimeQueryGLTest::queryTimestamp});
}

void TimeQueryGLTest::queryTime() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::timer_query>())
        CORRADE_SKIP(Extensions::GL::ARB::timer_query::string() + std::string(" is not available"));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::disjoint_timer_query>())
        CORRADE_SKIP(Extensions::GL::EXT::disjoint_timer_query::string() + std::string(" is not available"));
    #endif

    TimeQuery q1;
    q1.begin(TimeQuery::Target::TimeElapsed);
    q1.end();
    const auto result1 = q1.result<UnsignedInt>();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(result1 > 0);

    TimeQuery q2;
    q2.begin(TimeQuery::Target::TimeElapsed);
    Renderer::enable(Renderer::Feature::Blending);
    Renderer::finish();
    q2.end();
    const auto result2 = q2.result<UnsignedInt>();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(result2 > result1);
}

void TimeQueryGLTest::queryTimestamp() {
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::disjoint_timer_query>())
        CORRADE_SKIP(Extensions::GL::EXT::disjoint_timer_query::string() + std::string(" is not available"));
    #endif

    TimeQuery q1, q2, q;

    q1.timestamp();

    q.begin(TimeQuery::Target::TimeElapsed);
    Renderer::enable(Renderer::Feature::Blending);
    Renderer::finish();
    q.end();

    q2.timestamp();

    const auto result = q.result<UnsignedInt>();
    const auto result1 = q1.result<UnsignedLong>();
    const auto result2 = q2.result<UnsignedLong>();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(result > 0);
    CORRADE_VERIFY(result2 > result1);
    CORRADE_VERIFY(result2-result1 > result);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::TimeQueryGLTest)
