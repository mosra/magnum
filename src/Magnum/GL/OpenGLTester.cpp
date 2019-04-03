/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "OpenGLTester.h"

#include <string>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#endif

namespace Magnum { namespace GL {

OpenGLTester::OpenGLTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})}, _windowlessApplication{{arguments().first, arguments().second}} {
    /* Try to create debug context, fallback to normal one if not possible. No
       such thing on macOS, iOS or WebGL. */
    #if !defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_WEBGL)
    if(!_windowlessApplication.tryCreateContext(Platform::WindowlessApplication::Configuration{}.addFlags(Platform::WindowlessApplication::Configuration::Flag::Debug)))
        _windowlessApplication.createContext();
    #else
    _windowlessApplication.createContext();
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>()) {
        Renderer::enable(Renderer::Feature::DebugOutput);
        Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
        DebugOutput::setDefaultCallback();

        /* Disable "Buffer detailed info" message on NV (too spammy) */
        if(Context::current().detectedDriver() & Context::DetectedDriver::NVidia)
            DebugOutput::setEnabled(DebugOutput::Source::Api, DebugOutput::Type::Other, {131185}, false);
    }
    #endif
}

OpenGLTester::~OpenGLTester() = default;

#ifndef MAGNUM_TARGET_WEBGL
void OpenGLTester::gpuTimeBenchmarkBegin() {
    setBenchmarkName("GPU time");

    /* Initialize, if not already */
    if(!_gpuTimeQuery.id()) _gpuTimeQuery = TimeQuery{TimeQuery::Target::TimeElapsed};

    _gpuTimeQuery.begin();
}

std::uint64_t OpenGLTester::gpuTimeBenchmarkEnd() {
    _gpuTimeQuery.end();
    return _gpuTimeQuery.result<UnsignedLong>();
}
#endif

}}
