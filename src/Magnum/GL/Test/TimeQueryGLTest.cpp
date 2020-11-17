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

#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/TimeQuery.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct TimeQueryGLTest: OpenGLTester {
    explicit TimeQueryGLTest();

    void constructMove();
    void wrap();

    void queryTime();
    void queryTimestamp();
};

TimeQueryGLTest::TimeQueryGLTest() {
    addTests({&TimeQueryGLTest::constructMove,
              &TimeQueryGLTest::wrap,

              &TimeQueryGLTest::queryTime,
              &TimeQueryGLTest::queryTimestamp});
}

void TimeQueryGLTest::constructMove() {
    /* Move constructor tested in AbstractQuery, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(TimeQuery), sizeof(AbstractQuery));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<TimeQuery>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<TimeQuery>::value);
}

void TimeQueryGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::timer_query>())
        CORRADE_SKIP(Extensions::ARB::timer_query::string() + std::string(" is not available"));
    #elif defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query_webgl2>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query_webgl2::string() + std::string(" is not available"));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query::string() + std::string(" is not available"));
    #endif

    GLuint id;
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &id);
    #else
    glGenQueriesEXT(1, &id);
    #endif

    /* Releasing won't delete anything */
    {
        auto query = TimeQuery::wrap(id, TimeQuery::Target::TimeElapsed, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(query.release(), id);
    }

    /* ...so we can wrap it again */
    TimeQuery::wrap(id, TimeQuery::Target::TimeElapsed);
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &id);
    #else
    glDeleteQueriesEXT(1, &id);
    #endif
}

void TimeQueryGLTest::queryTime() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::timer_query>())
        CORRADE_SKIP(Extensions::ARB::timer_query::string() + std::string(" is not available"));
    #elif defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query_webgl2>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query_webgl2::string() + std::string(" is not available"));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query::string() + std::string(" is not available"));
    #endif

    TimeQuery q1{TimeQuery::Target::TimeElapsed};
    q1.begin();
    q1.end();
    const auto result1 = q1.result<UnsignedInt>();

    MAGNUM_VERIFY_NO_GL_ERROR();

    TimeQuery q2{TimeQuery::Target::TimeElapsed};
    q2.begin();
    Renderer::enable(Renderer::Feature::Blending);
    Renderer::finish();
    q2.end();
    const auto result2 = q2.result<UnsignedInt>();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(result2, result1, TestSuite::Compare::GreaterOrEqual);
}

void TimeQueryGLTest::queryTimestamp() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::timer_query>())
        CORRADE_SKIP(Extensions::ARB::timer_query::string() + std::string(" is not available"));
    #elif defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query_webgl2>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query_webgl2::string() + std::string(" is not available"));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query>())
        CORRADE_SKIP(Extensions::EXT::disjoint_timer_query::string() + std::string(" is not available"));
    #endif

    TimeQuery q1{TimeQuery::Target::Timestamp},
        q2{TimeQuery::Target::Timestamp},
        q{TimeQuery::Target::TimeElapsed};

    q1.timestamp();

    q.begin();
    Renderer::enable(Renderer::Feature::Blending);
    Renderer::finish();
    q.end();

    q2.timestamp();

    const auto result = q.result<UnsignedInt>();
    const auto result1 = q1.result<UnsignedLong>();
    const auto result2 = q2.result<UnsignedLong>();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE_AS(result2, result1, TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(result2 - result1, result, TestSuite::Compare::GreaterOrEqual);
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TimeQueryGLTest)
