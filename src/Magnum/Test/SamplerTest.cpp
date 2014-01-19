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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Sampler.h"

namespace Magnum { namespace Test {

class SamplerTest: public TestSuite::Tester {
    public:
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

    Debug(&out) << Sampler::Filter::Linear;
    CORRADE_COMPARE(out.str(), "Sampler::Filter::Linear\n");
}

void SamplerTest::debugMipmap() {
    std::ostringstream out;

    Debug(&out) << Sampler::Mipmap::Base;
    CORRADE_COMPARE(out.str(), "Sampler::Mipmap::Base\n");
}

void SamplerTest::debugWrapping() {
    std::ostringstream out;

    Debug(&out) << Sampler::Wrapping::ClampToEdge;
    CORRADE_COMPARE(out.str(), "Sampler::Wrapping::ClampToEdge\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::SamplerTest)
