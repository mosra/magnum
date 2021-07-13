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

#include "Magnum/Shaders/VertexColorGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct VertexColorGL_Test: TestSuite::Tester {
    explicit VertexColorGL_Test();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    #ifndef MAGNUM_TARGET_GLES2
    void debugFlagsSupersets();
    #endif
};

VertexColorGL_Test::VertexColorGL_Test() {
    addTests<VertexColorGL_Test>({
        &VertexColorGL_Test::constructNoCreate<2>,
        &VertexColorGL_Test::constructNoCreate<3>,

        &VertexColorGL_Test::constructCopy<2>,
        &VertexColorGL_Test::constructCopy<3>,

        &VertexColorGL_Test::debugFlag,
        &VertexColorGL_Test::debugFlags,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGL_Test::debugFlagsSupersets
        #endif
    });
}

template<UnsignedInt dimensions> void VertexColorGL_Test::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        VertexColorGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void VertexColorGL_Test::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<VertexColorGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<VertexColorGL<dimensions>>{});
}

void VertexColorGL_Test::debugFlag() {
    std::ostringstream out;

    #ifndef MAGNUM_TARGET_GLES2
    Debug{&out} << VertexColorGL3D::Flag::UniformBuffers << VertexColorGL3D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::VertexColorGL::Flag::UniformBuffers Shaders::VertexColorGL::Flag(0xf0)\n");
    #else
    Debug{&out} << VertexColorGL3D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::VertexColorGL::Flag(0xf0)\n");
    #endif
}

void VertexColorGL_Test::debugFlags() {
    std::ostringstream out;

    #ifndef MAGNUM_TARGET_GLES2
    Debug{&out} << (VertexColorGL3D::Flag::UniformBuffers|VertexColorGL3D::Flag(0xf0)) << VertexColorGL3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::VertexColorGL::Flag::UniformBuffers|Shaders::VertexColorGL::Flag(0xf0) Shaders::VertexColorGL::Flags{}\n");
    #else
    Debug{&out} << VertexColorGL3D::Flag(0xf0) << VertexColorGL3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::VertexColorGL::Flag(0xf0) Shaders::VertexColorGL::Flags{}\n");
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void VertexColorGL_Test::debugFlagsSupersets() {
    /* MultiDraw is a superset of UniformBuffers so only one should be printed */
    std::ostringstream out;
    Debug{&out} << (VertexColorGL3D::Flag::MultiDraw|VertexColorGL3D::Flag::UniformBuffers);
    CORRADE_COMPARE(out.str(), "Shaders::VertexColorGL::Flag::MultiDraw\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VertexColorGL_Test)
