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

#include "Magnum/Shaders/Phong.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct PhongTest: TestSuite::Tester {
    explicit PhongTest();

    void constructNoCreate();
    void constructCopy();

    void debugFlag();
    void debugFlags();
    void debugFlagsSupersets();
};

PhongTest::PhongTest() {
    addTests({&PhongTest::constructNoCreate,
              &PhongTest::constructCopy,

              &PhongTest::debugFlag,
              &PhongTest::debugFlags,
              &PhongTest::debugFlagsSupersets});
}

void PhongTest::constructNoCreate() {
    {
        Phong shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void PhongTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Phong, const Phong&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Phong, const Phong&>{}));
}

void PhongTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << Phong::Flag::AmbientTexture << Phong::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::Phong::Flag::AmbientTexture Shaders::Phong::Flag(0xf0)\n");
}

void PhongTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture) << Phong::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::Phong::Flag::DiffuseTexture|Shaders::Phong::Flag::SpecularTexture Shaders::Phong::Flags{}\n");
}

void PhongTest::debugFlagsSupersets() {
    #ifndef MAGNUM_TARGET_GLES2
    /* InstancedObjectId is a superset of ObjectId so only one should be
       printed */
    {
        std::ostringstream out;
        Debug{&out} << (Phong::Flag::ObjectId|Phong::Flag::InstancedObjectId);
        CORRADE_COMPARE(out.str(), "Shaders::Phong::Flag::InstancedObjectId\n");
    }
    #endif

    /* InstancedTextureOffset is a superset of TextureTransformation so only
       one should be printed */
    std::ostringstream out;
    Debug{&out} << (Phong::Flag::InstancedTextureOffset|Phong::Flag::TextureTransformation);
    CORRADE_COMPARE(out.str(), "Shaders::Phong::Flag::InstancedTextureOffset\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongTest)
