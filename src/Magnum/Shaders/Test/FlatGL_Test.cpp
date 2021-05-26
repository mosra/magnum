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

#include "Magnum/Shaders/FlatGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct FlatGL_Test: TestSuite::Tester {
    explicit FlatGL_Test();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

FlatGL_Test::FlatGL_Test() {
    addTests({&FlatGL_Test::constructNoCreate<2>,
              &FlatGL_Test::constructNoCreate<3>,

              &FlatGL_Test::constructCopy<2>,
              &FlatGL_Test::constructCopy<3>,

              &FlatGL_Test::debugFlag,
              &FlatGL_Test::debugFlags,
              &FlatGL_Test::debugFlagsSupersets});
}

template<UnsignedInt dimensions> void FlatGL_Test::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        FlatGL<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), typename FlatGL<dimensions>::Flags{});
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void FlatGL_Test::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY(!std::is_copy_constructible<FlatGL<dimensions>>{});
    CORRADE_VERIFY(!std::is_copy_assignable<FlatGL<dimensions>>{});
}

void FlatGL_Test::debugFlag() {
    std::ostringstream out;

    Debug{&out} << FlatGL3D::Flag::Textured << FlatGL3D::Flag(0xf00d);
    CORRADE_COMPARE(out.str(), "Shaders::FlatGL::Flag::Textured Shaders::FlatGL::Flag(0xf00d)\n");
}

void FlatGL_Test::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (FlatGL3D::Flag::Textured|FlatGL3D::Flag::AlphaMask) << FlatGL3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::FlatGL::Flag::Textured|Shaders::FlatGL::Flag::AlphaMask Shaders::FlatGL::Flags{}\n");
}

void FlatGL_Test::debugFlagsSupersets() {
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId is a superset of ObjectId so only one should be
       printed */
    {
        std::ostringstream out;
        Debug{&out} << (FlatGL3D::Flag::ObjectId|FlatGL3D::Flag::InstancedObjectId);
        CORRADE_COMPARE(out.str(), "Shaders::FlatGL::Flag::InstancedObjectId\n");
    }
    #endif

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    {
        std::ostringstream out;
        Debug{&out} << (FlatGL3D::Flag::InstancedTextureOffset|FlatGL3D::Flag::TextureTransformation);
        CORRADE_COMPARE(out.str(), "Shaders::FlatGL::Flag::InstancedTextureOffset\n");
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* MultiDraw is a superset of UniformBuffers so only one should be printed */
    {
        std::ostringstream out;
        Debug{&out} << (FlatGL3D::Flag::MultiDraw|FlatGL3D::Flag::UniformBuffers);
        CORRADE_COMPARE(out.str(), "Shaders::FlatGL::Flag::MultiDraw\n");
    }
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatGL_Test)
