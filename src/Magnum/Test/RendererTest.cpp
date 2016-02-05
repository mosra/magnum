/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Renderer.h"

namespace Magnum { namespace Test {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    void debugError();
    void debugResetNotificationStrategy();
    void debugGraphicsResetStatus();
};

RendererTest::RendererTest() {
    addTests({&RendererTest::debugError});
}

void RendererTest::debugError() {
    std::ostringstream out;

    Debug(&out) << Renderer::Error::InvalidOperation;
    CORRADE_COMPARE(out.str(), "Renderer::Error::InvalidOperation\n");
}

void RendererTest::debugResetNotificationStrategy() {
    std::ostringstream out;

    Debug(&out) << Renderer::ResetNotificationStrategy::LoseContextOnReset;
    CORRADE_COMPARE(out.str(), "Renderer::ResetNotificationStrategy::LoseContextOnReset\n");
}

void RendererTest::debugGraphicsResetStatus() {
    std::ostringstream out;

    Debug(&out) << Renderer::GraphicsResetStatus::GuiltyContextReset;
    CORRADE_COMPARE(out.str(), "Renderer::GraphicsResetStatus::GuiltyContextReset\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::RendererTest)
