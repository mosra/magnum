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

#include "Magnum/Shaders/PhongGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

/* There's an underscore between GL and Test to disambiguate from GLTest, which
   is a common suffix used to mark tests that need a GL context. Ugly, I know. */
struct PhongGL_Test: TestSuite::Tester {
    explicit PhongGL_Test();

    void constructNoCreate();
    void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

PhongGL_Test::PhongGL_Test() {
    addTests({&PhongGL_Test::constructNoCreate,
              &PhongGL_Test::constructCopy,

              &PhongGL_Test::debugFlag,
              &PhongGL_Test::debugFlags,
              &PhongGL_Test::debugFlagsSupersets});
}

void PhongGL_Test::constructNoCreate() {
    {
        PhongGL shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
        CORRADE_COMPARE(shader.flags(), PhongGL::Flags{});
        CORRADE_COMPARE(shader.lightCount(), 0);
    }

    CORRADE_VERIFY(true);
}

void PhongGL_Test::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<PhongGL>{});
    CORRADE_VERIFY(!std::is_copy_assignable<PhongGL>{});
}

void PhongGL_Test::debugFlag() {
    std::ostringstream out;

    Debug{&out} << PhongGL::Flag::AmbientTexture << PhongGL::Flag(0xcafedead);
    CORRADE_COMPARE(out.str(), "Shaders::PhongGL::Flag::AmbientTexture Shaders::PhongGL::Flag(0xcafedead)\n");
}

void PhongGL_Test::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (PhongGL::Flag::DiffuseTexture|PhongGL::Flag::SpecularTexture) << PhongGL::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::PhongGL::Flag::DiffuseTexture|Shaders::PhongGL::Flag::SpecularTexture Shaders::PhongGL::Flags{}\n");
}

void PhongGL_Test::debugFlagsSupersets() {
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId is a superset of ObjectId so only one should be
       printed */
    {
        std::ostringstream out;
        Debug{&out} << (PhongGL::Flag::ObjectId|PhongGL::Flag::InstancedObjectId);
        CORRADE_COMPARE(out.str(), "Shaders::PhongGL::Flag::InstancedObjectId\n");
    }
    #endif

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    {
        std::ostringstream out;
        Debug{&out} << (PhongGL::Flag::InstancedTextureOffset|PhongGL::Flag::TextureTransformation);
        CORRADE_COMPARE(out.str(), "Shaders::PhongGL::Flag::InstancedTextureOffset\n");
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* MultiDraw is a superset of UniformBuffers so only one should be printed */
    {
        std::ostringstream out;
        Debug{&out} << (PhongGL::Flag::MultiDraw|PhongGL::Flag::UniformBuffers);
        CORRADE_COMPARE(out.str(), "Shaders::PhongGL::Flag::MultiDraw\n");
    }
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGL_Test)
