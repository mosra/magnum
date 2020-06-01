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

#include "OpenGLTester.h"

#include <string>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#endif

namespace Magnum { namespace GL {

OpenGLTester::OpenGLTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})}, _windowlessApplication{{arguments().first, arguments().second}} {}

OpenGLTester::~OpenGLTester() = default;

void OpenGLTester::gpuTimeBenchmarkBegin() {
    setBenchmarkName("GPU time");

    /* Initialize, if not already; check for extension presence and skip if not
       available. This function is always called from inside CORRADE_BENCHMARK,
       which does the test case registration on a proper function. */
    if(!_gpuTimeQuery.id()) {
        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current().isExtensionSupported<Extensions::ARB::timer_query>())
            skip("GL_ARB_timer_query is not supported");
        #elif defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query_webgl2>())
            skip("GL_EXT_disjoint_timer_query_webgl2 is not supported");
        #else
        if(!Context::current().isExtensionSupported<Extensions::EXT::disjoint_timer_query>())
            skip("GL_EXT_disjoint_timer_query is not supported");
        #endif
        _gpuTimeQuery = TimeQuery{TimeQuery::Target::TimeElapsed};
    }

    _gpuTimeQuery.begin();
}

std::uint64_t OpenGLTester::gpuTimeBenchmarkEnd() {
    _gpuTimeQuery.end();
    return _gpuTimeQuery.result<UnsignedLong>();
}

}}
