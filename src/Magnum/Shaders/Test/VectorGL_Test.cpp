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

#include "Magnum/Shaders/VectorGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct VectorGL_Test: TestSuite::Tester {
    explicit VectorGL_Test();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    #ifndef MAGNUM_TARGET_GLES2
    void debugFlagsSupersets();
    #endif
};

VectorGL_Test::VectorGL_Test() {
    addTests({
        &VectorGL_Test::constructNoCreate<2>,
        &VectorGL_Test::constructNoCreate<3>,

        &VectorGL_Test::constructCopy<2>,
        &VectorGL_Test::constructCopy<3>,

        &VectorGL_Test::debugFlag,
        &VectorGL_Test::debugFlags,
        #ifndef MAGNUM_TARGET_GLES2
        &VectorGL_Test::debugFlagsSupersets
        #endif
    });
}

template<UnsignedInt dimensions> void VectorGL_Test::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        VectorGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename VectorGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void VectorGL_Test::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<VectorGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<VectorGL<dimensions>>{});
}

void VectorGL_Test::debugFlag() {
    std::ostringstream out;

    Debug{&out} << VectorGL2D::Flag::TextureTransformation << VectorGL2D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::VectorGL::Flag::TextureTransformation Shaders::VectorGL::Flag(0xf0)\n");
}

void VectorGL_Test::debugFlags() {
    std::ostringstream out;

    Debug{&out} << VectorGL3D::Flags{VectorGL3D::Flag::TextureTransformation|VectorGL3D::Flag(0xf0)} << VectorGL3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::VectorGL::Flag::TextureTransformation|Shaders::VectorGL::Flag(0xf0) Shaders::VectorGL::Flags{}\n");
}

#ifndef MAGNUM_TARGET_GLES2
void VectorGL_Test::debugFlagsSupersets() {
    /* MultiDraw is a superset of UniformBuffers so only one should be printed */
    std::ostringstream out;
    Debug{&out} << (VectorGL3D::Flag::MultiDraw|VectorGL3D::Flag::UniformBuffers);
    CORRADE_COMPARE(out.str(), "Shaders::VectorGL::Flag::MultiDraw\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorGL_Test)
