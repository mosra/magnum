/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/DebugOutput.h"

namespace Magnum {

OpenGLTester::OpenGLTester(): TestSuite::Tester{TestSuite::Tester::TesterConfiguration{}.setSkippedArgumentPrefixes({"magnum"})}, _windowlessApplication{{arguments().first, arguments().second}} {
    /* Try to create debug context, fallback to normal one if not possible. No
       such thing on macOS or iOS. */
    #ifndef CORRADE_TARGET_APPLE
    if(!_windowlessApplication.tryCreateContext(Platform::WindowlessApplication::Configuration{}.setFlags(Platform::WindowlessApplication::Configuration::Flag::Debug)))
        _windowlessApplication.createContext();
    #else
    _windowlessApplication.createContext();
    #endif

    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>()) {
        Renderer::enable(Renderer::Feature::DebugOutput);
        Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
        DebugOutput::setDefaultCallback();

        /* Disable "Buffer detailed info" message on NV (too spammy) */
        DebugOutput::setEnabled(DebugOutput::Source::Api, DebugOutput::Type::Other, {131185}, false);
    }
}

OpenGLTester::~OpenGLTester() = default;

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

}
