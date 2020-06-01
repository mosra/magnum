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

#include "Magnum/Sampler.h"

namespace Magnum { namespace Test { namespace {

struct SamplerTest: TestSuite::Tester {
    explicit SamplerTest();

    void debugFilter();
    void debugMipmap();
    void debugWrapping();
};

SamplerTest::SamplerTest() {
    addTests({&SamplerTest::debugFilter,
              &SamplerTest::debugMipmap,
              &SamplerTest::debugWrapping});
}

void SamplerTest::debugFilter() {
    std::ostringstream out;

    Debug(&out) << SamplerFilter::Linear << SamplerFilter(0xdead);
    CORRADE_COMPARE(out.str(), "SamplerFilter::Linear SamplerFilter(0xdead)\n");
}

void SamplerTest::debugMipmap() {
    std::ostringstream out;

    Debug(&out) << SamplerMipmap::Base << SamplerMipmap(0xdead);
    CORRADE_COMPARE(out.str(), "SamplerMipmap::Base SamplerMipmap(0xdead)\n");
}

void SamplerTest::debugWrapping() {
    std::ostringstream out;

    Debug(&out) << SamplerWrapping::ClampToEdge << SamplerWrapping(0xdead);
    CORRADE_COMPARE(out.str(), "SamplerWrapping::ClampToEdge SamplerWrapping(0xdead)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::SamplerTest)
