/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Shaders/DistanceFieldVectorGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct DistanceFieldVectorGL_Test: TestSuite::Tester {
    explicit DistanceFieldVectorGL_Test();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    #ifndef MAGNUM_TARGET_GLES2
    void debugFlagsSupersets();
    #endif
};

DistanceFieldVectorGL_Test::DistanceFieldVectorGL_Test() {
    addTests({
        &DistanceFieldVectorGL_Test::constructNoCreate<2>,
        &DistanceFieldVectorGL_Test::constructNoCreate<3>,

        &DistanceFieldVectorGL_Test::constructCopy<2>,
        &DistanceFieldVectorGL_Test::constructCopy<3>,

        &DistanceFieldVectorGL_Test::debugFlag,
        &DistanceFieldVectorGL_Test::debugFlags,
        #ifndef MAGNUM_TARGET_GLES2
        &DistanceFieldVectorGL_Test::debugFlagsSupersets
        #endif
    });
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        DistanceFieldVectorGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename DistanceFieldVectorGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void DistanceFieldVectorGL_Test::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<DistanceFieldVectorGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DistanceFieldVectorGL<dimensions>>{});
}

void DistanceFieldVectorGL_Test::debugFlag() {
    std::ostringstream out;

    Debug{&out} << DistanceFieldVectorGL2D::Flag::TextureTransformation << DistanceFieldVectorGL2D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::DistanceFieldVectorGL::Flag::TextureTransformation Shaders::DistanceFieldVectorGL::Flag(0xf0)\n");
}

void DistanceFieldVectorGL_Test::debugFlags() {
    std::ostringstream out;

    Debug{&out} << DistanceFieldVectorGL3D::Flags{DistanceFieldVectorGL3D::Flag::TextureTransformation|DistanceFieldVectorGL3D::Flag(0xf0)} << DistanceFieldVectorGL3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::DistanceFieldVectorGL::Flag::TextureTransformation|Shaders::DistanceFieldVectorGL::Flag(0xf0) Shaders::DistanceFieldVectorGL::Flags{}\n");
}

#ifndef MAGNUM_TARGET_GLES2
void DistanceFieldVectorGL_Test::debugFlagsSupersets() {
    /* MultiDraw is a superset of UniformBuffers so only one should be printed */
    std::ostringstream out;
    Debug{&out} << (DistanceFieldVectorGL3D::Flag::MultiDraw|DistanceFieldVectorGL3D::Flag::UniformBuffers);
    CORRADE_COMPARE(out.str(), "Shaders::DistanceFieldVectorGL::Flag::MultiDraw\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::DistanceFieldVectorGL_Test)
