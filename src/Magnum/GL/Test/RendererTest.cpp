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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Renderer.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    void debugError();
    #ifndef MAGNUM_TARGET_WEBGL
    void debugResetNotificationStrategy();
    void debugGraphicsResetStatus();
    #endif
};

RendererTest::RendererTest() {
    addTests({&RendererTest::debugError,
              #ifndef MAGNUM_TARGET_WEBGL
              &RendererTest::debugResetNotificationStrategy,
              &RendererTest::debugGraphicsResetStatus
              #endif
              });
}

void RendererTest::debugError() {
    std::ostringstream out;

    Debug(&out) << Renderer::Error::InvalidOperation << Renderer::Error(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Renderer::Error::InvalidOperation GL::Renderer::Error(0xdead)\n");
}

#ifndef MAGNUM_TARGET_WEBGL
void RendererTest::debugResetNotificationStrategy() {
    std::ostringstream out;

    Debug(&out) << Renderer::ResetNotificationStrategy::LoseContextOnReset << Renderer::ResetNotificationStrategy(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Renderer::ResetNotificationStrategy::LoseContextOnReset GL::Renderer::ResetNotificationStrategy(0xdead)\n");
}

void RendererTest::debugGraphicsResetStatus() {
    std::ostringstream out;

    Debug(&out) << Renderer::GraphicsResetStatus::GuiltyContextReset << Renderer::GraphicsResetStatus(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Renderer::GraphicsResetStatus::GuiltyContextReset GL::Renderer::GraphicsResetStatus(0xdead)\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::RendererTest)
