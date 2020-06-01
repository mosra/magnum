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
#include "Magnum/GL/Sampler.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct SamplerTest: TestSuite::Tester {
    explicit SamplerTest();

    void mapFilter();
    void mapFilterInvalid();
    void mapMipmap();
    void mapMipmapInvalid();
    void mapWrapping();
    void mapWrappingArray();
    void mapWrappingInvalid();
    void mapWrappingUnsupported();

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
    addTests({&SamplerTest::mapFilter,
              &SamplerTest::mapFilterInvalid,
              &SamplerTest::mapMipmap,
              &SamplerTest::mapMipmapInvalid,
              &SamplerTest::mapWrapping,
              &SamplerTest::mapWrappingArray,
              &SamplerTest::mapWrappingInvalid,
              &SamplerTest::mapWrappingUnsupported,

              &SamplerTest::debugFilter,
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

void SamplerTest::mapFilter() {
    CORRADE_COMPARE(samplerFilter(Magnum::SamplerFilter::Nearest), SamplerFilter::Nearest);
    CORRADE_COMPARE(samplerFilter(Magnum::SamplerFilter::Linear), SamplerFilter::Linear);
}

void SamplerTest::mapFilterInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    samplerFilter(Magnum::SamplerFilter(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::samplerFilter(): invalid filter SamplerFilter(0x123)\n");
}

void SamplerTest::mapMipmap() {
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Base), SamplerMipmap::Base);
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Nearest), SamplerMipmap::Nearest);
    CORRADE_COMPARE(samplerMipmap(Magnum::SamplerMipmap::Linear), SamplerMipmap::Linear);
}

void SamplerTest::mapMipmapInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    samplerMipmap(Magnum::SamplerMipmap(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::samplerMipmap(): invalid mode SamplerMipmap(0x123)\n");
}

void SamplerTest::mapWrapping() {
    CORRADE_VERIFY(hasSamplerWrapping(Magnum::SamplerWrapping::Repeat));
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::Repeat), SamplerWrapping::Repeat);

    CORRADE_VERIFY(hasSamplerWrapping(Magnum::SamplerWrapping::MirroredRepeat));
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::MirroredRepeat), SamplerWrapping::MirroredRepeat);

    CORRADE_VERIFY(hasSamplerWrapping(Magnum::SamplerWrapping::ClampToEdge));
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::ClampToEdge), SamplerWrapping::ClampToEdge);

    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_VERIFY(hasSamplerWrapping(Magnum::SamplerWrapping::ClampToBorder));
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::ClampToBorder), SamplerWrapping::ClampToBorder);
    #endif

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_VERIFY(hasSamplerWrapping(Magnum::SamplerWrapping::MirrorClampToEdge));
    CORRADE_COMPARE(samplerWrapping(Magnum::SamplerWrapping::MirrorClampToEdge), SamplerWrapping::MirrorClampToEdge);
    #endif
}

void SamplerTest::mapWrappingArray() {
    CORRADE_COMPARE(samplerWrapping<2>({Magnum::SamplerWrapping::Repeat, Magnum::SamplerWrapping::ClampToEdge}), (Array2D<SamplerWrapping>{SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge}));
}

void SamplerTest::mapWrappingInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    hasSamplerWrapping(Magnum::SamplerWrapping(0x123));
    samplerWrapping(Magnum::SamplerWrapping(0x123));
    CORRADE_COMPARE(out.str(),
        "GL::hasSamplerWrapping(): invalid wrapping SamplerWrapping(0x123)\n"
        "GL::samplerWrapping(): invalid wrapping SamplerWrapping(0x123)\n");
}

void SamplerTest::mapWrappingUnsupported() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_SKIP("All pixel formats are supported on desktop");
    #else
    CORRADE_VERIFY(!hasSamplerWrapping(Magnum::SamplerWrapping::MirrorClampToEdge));

    std::ostringstream out;
    {
        Error redirectError{&out};
        samplerWrapping(Magnum::SamplerWrapping::MirrorClampToEdge);
    }
    CORRADE_COMPARE(out.str(),
        "GL::samplerWrapping(): wrapping SamplerWrapping::MirrorClampToEdge is not supported on this target\n");
    #endif
}

void SamplerTest::debugFilter() {
    std::ostringstream out;

    Debug(&out) << SamplerFilter::Linear << SamplerFilter(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerFilter::Linear GL::SamplerFilter(0xdead)\n");
}

void SamplerTest::debugMipmap() {
    std::ostringstream out;

    Debug(&out) << SamplerMipmap::Base << SamplerMipmap(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerMipmap::Base GL::SamplerMipmap(0xdead)\n");
}

void SamplerTest::debugWrapping() {
    std::ostringstream out;

    Debug(&out) << SamplerWrapping::ClampToEdge << SamplerWrapping(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerWrapping::ClampToEdge GL::SamplerWrapping(0xdead)\n");
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void SamplerTest::debugCompareMode() {
    std::ostringstream out;

    Debug(&out) << SamplerCompareMode::CompareRefToTexture << SamplerCompareMode(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerCompareMode::CompareRefToTexture GL::SamplerCompareMode(0xdead)\n");
}

void SamplerTest::debugCompareFunction() {
    std::ostringstream out;

    Debug(&out) << SamplerCompareFunction::GreaterOrEqual << SamplerCompareFunction(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerCompareFunction::GreaterOrEqual GL::SamplerCompareFunction(0xdead)\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
void SamplerTest::debugDepthStencilMode() {
    std::ostringstream out;

    Debug(&out) << SamplerDepthStencilMode::StencilIndex << SamplerDepthStencilMode(0xdead);
    CORRADE_COMPARE(out.str(), "GL::SamplerDepthStencilMode::StencilIndex GL::SamplerDepthStencilMode(0xdead)\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::SamplerTest)
