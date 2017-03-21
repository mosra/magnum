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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Sampler.h"

namespace Magnum { namespace Test {

struct SamplerTest: TestSuite::Tester {
    explicit SamplerTest();

    void debugFilter();
    void debugMipmap();
    void debugWrapping();
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void debugCompareMode();
    void debugCompareFunction();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void debugDepthStencilMode();
    #endif
};

SamplerTest::SamplerTest() {
    addTests({&SamplerTest::debugFilter,
              &SamplerTest::debugMipmap,
              &SamplerTest::debugWrapping,
              #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
              &SamplerTest::debugCompareMode,
              &SamplerTest::debugCompareFunction,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &SamplerTest::debugDepthStencilMode
              #endif
             });
}

void SamplerTest::debugFilter() {
    std::ostringstream out;

    Debug(&out) << Sampler::Filter::Linear << Sampler::Filter(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::Filter::Linear Sampler::Filter(0xdead)\n");
}

void SamplerTest::debugMipmap() {
    std::ostringstream out;

    Debug(&out) << Sampler::Mipmap::Base << Sampler::Mipmap(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::Mipmap::Base Sampler::Mipmap(0xdead)\n");
}

void SamplerTest::debugWrapping() {
    std::ostringstream out;

    Debug(&out) << Sampler::Wrapping::ClampToEdge << Sampler::Wrapping(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::Wrapping::ClampToEdge Sampler::Wrapping(0xdead)\n");
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void SamplerTest::debugCompareMode() {
    std::ostringstream out;

    Debug(&out) << Sampler::CompareMode::CompareRefToTexture << Sampler::CompareMode(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::CompareMode::CompareRefToTexture Sampler::CompareMode(0xdead)\n");
}

void SamplerTest::debugCompareFunction() {
    std::ostringstream out;

    Debug(&out) << Sampler::CompareFunction::GreaterOrEqual << Sampler::CompareFunction(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::CompareFunction::GreaterOrEqual Sampler::CompareFunction(0xdead)\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
void SamplerTest::debugDepthStencilMode() {
    std::ostringstream out;

    Debug(&out) << Sampler::DepthStencilMode::StencilIndex << Sampler::DepthStencilMode(0xdead);
    CORRADE_COMPARE(out.str(), "Sampler::DepthStencilMode::StencilIndex Sampler::DepthStencilMode(0xdead)\n");
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::SamplerTest)
