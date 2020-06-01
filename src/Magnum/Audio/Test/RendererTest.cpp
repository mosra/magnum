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

#include "Magnum/Magnum.h"
#include "Magnum/Audio/Renderer.h"

namespace Magnum { namespace Audio { namespace Test { namespace {

struct RendererTest: TestSuite::Tester {
    explicit RendererTest();

    void debugError();
    void debugDistanceModel();
};

RendererTest::RendererTest() {
    addTests({&RendererTest::debugError,
              &RendererTest::debugDistanceModel});
}

void RendererTest::debugError() {
    std::ostringstream out;
    Debug(&out) << Renderer::Error::InvalidOperation << Renderer::Error(0xdead);
    CORRADE_COMPARE(out.str(), "Audio::Renderer::Error::InvalidOperation Audio::Renderer::Error(0xdead)\n");
}

void RendererTest::debugDistanceModel() {
    std::ostringstream out;
    Debug(&out) << Renderer::DistanceModel::Inverse << Renderer::DistanceModel(0xdead);
    CORRADE_COMPARE(out.str(), "Audio::Renderer::DistanceModel::Inverse Audio::Renderer::DistanceModel(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Audio::Test::RendererTest)
